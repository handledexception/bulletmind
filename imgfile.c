#include "imgfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct tga_header_s {
    uint8_t id_length;              // field 1
    uint8_t color_map_type;         // field 2
    uint8_t image_type;             // field 3                        
                                    
    uint8_t color_map_spec[5];      // field 4
    /*
        uint16_t first_entry_index;     
        uint16_t color_map_length;
        uint8_t color_map_entry_size;
    */    
    
    uint16_t x_origin;              // field 5
    uint16_t y_origin;
    uint16_t img_width;
    uint16_t img_height;
    uint8_t img_bpp;
    uint8_t img_desc;    
};

struct tga_header_s header;

// todo(paulh): refactor so file reading is in its own source
bool imgfile_init(const char *path, imgfile_t *out)
{
    FILE *fptr = NULL;
    size_t fsize = 0;
    uint8_t *buf = NULL;

    fptr = fopen(path, "rb");
    fseek(fptr, 0 , SEEK_END);
    fsize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    buf = (uint8_t *)malloc(fsize);
    if (fptr == NULL) {
        printf("imgfile_init: file %s has no data!\n", path);
        return false;
    }
    else if (fread(buf, sizeof(*buf), fsize, fptr) != fsize) {
        printf("imgfile_init: could not read to end of file %s\n", path);
        free(buf);
        buf = NULL;
        return false;
    }
    
    struct tga_header_s *header = (struct tga_header_s *)buf;
    size_t tga_header_size = sizeof(*header);    
    // make sure we have a valid minimal TGA header and raw unmapped RGB data
    if (tga_header_size != 18 || header->color_map_type > 0 || header->image_type != 2) {
        printf("imgfile_init: Incorrect TGA header size! (%zu bytes) Should be 18 bytes.\n");
        free(buf);
        buf = NULL;
        return false;
    }    
    
    uint16_t width = header->img_width;
    uint16_t height = header->img_height;
    uint8_t bytes_per_pixel = header->img_bpp / 8;
    size_t pixel_size = width * height * bytes_per_pixel;
    //uint8_t pixelbuf[pixel_size];
    printf("imgfile_init: %dx%d @ %d bytes per pixel\n", width, height, bytes_per_pixel);
    out->data = (uint8_t *)malloc(pixel_size);

    uint8_t *tgapixels = buf + tga_header_size;
    // origin bit of img_desc set to 1 ... upper-left origin pixel
    if ((header->img_desc >> 5) & 1) {        
        memcpy(out->data, tgapixels, pixel_size);
    }
    // origin bit set to 0 ... lower-left origin pixel
    else {
        int32_t stride = width * bytes_per_pixel;
        for (int32_t c = 0; c < height; c++) {            
            memcpy(out->data + stride * c, tgapixels + stride * (height - (c+1)), stride);            
        }
    }

    // uint8_t swap;
    // for (int32_t c = 0; c < pixel_size; c+= bytes_per_pixel) {
    //     swap = out->data[c];
    //     out->data[c] = out->data[c + 2];
    //     out->data[c + 2] = swap;
    // }

    out->width = width;
    out->height = height;
    out->bpp = header->img_bpp;
    out->filesize = pixel_size;
    out->stride = width * bytes_per_pixel;
    out->type = BGR;
    
    fclose(fptr);

    return true;
}