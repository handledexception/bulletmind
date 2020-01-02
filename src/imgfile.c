#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "imgfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tga_header {
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
} tga_header_t;

bool img_file_init(const char* path, img_file_t** out)
{
    FILE *fptr = NULL;
    size_t fsize = 0;
    uint8_t *buf = NULL;

    if (out == NULL)
        return false;

    struct img_file* img = *out;

    fptr = fopen(path, "rb");
    fseek(fptr, 0 , SEEK_END);
    fsize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    buf = (uint8_t *)malloc(fsize);
    if (fptr == NULL) {
        printf("img_file_init: file %s has no data!\n", path);
        return false;
    }
    else if (fread(buf, sizeof(*buf), fsize, fptr) != fsize) {
        printf("img_file_init: could not read to end of file %s\n", path);
        free(buf);
        buf = NULL;
        return false;
    }

    struct tga_header *header = (struct tga_header *)buf;
    size_t tga_header_size = sizeof(*header);
    // make sure we have a valid minimal TGA header and raw unmapped RGB data
    if (tga_header_size != 18 || header->color_map_type > 0 || header->image_type != 2) {
        printf("img_file_init: Incorrect TGA header size! (%zu bytes) Should be 18 bytes.\n", tga_header_size);
        free(buf);
        buf = NULL;
        return false;
    }

    uint16_t width = header->img_width;
    uint16_t height = header->img_height;
    uint8_t bytes_per_pixel = header->img_bpp / 8;
    size_t pixel_size = width * height * bytes_per_pixel;
    //uint8_t pixelbuf[pixel_size];
    printf("img_file_init: %dx%d @ %d bytes per pixel\n", width, height, bytes_per_pixel);
    img->data = (uint8_t *)malloc(pixel_size);

    uint8_t *tgapixels = buf + tga_header_size;
    // origin bit of img_desc set to 1 ... upper-left origin pixel
    if ((header->img_desc >> 5) & 1) {
        memcpy(img->data, tgapixels, pixel_size);
    }
    // origin bit set to 0 ... lower-left origin pixel
    else {
        int32_t stride = width * bytes_per_pixel;
        for (int32_t c = 0; c < height; c++) {
            memcpy(img->data + stride * c, tgapixels + stride * (height - (c+1)), stride);
        }
    }

    free(buf);
    // uint8_t swap;
    // for (int32_t c = 0; c < pixel_size; c+= bytes_per_pixel) {
    //     swap = img->data[c];
    //     img->data[c] = img->data[c + 2];
    //     img->data[c + 2] = swap;
    // }

    img->width = width;
    img->height = height;
    img->bpp = header->img_bpp;
    img->filesize = pixel_size;
    img->stride = width * bytes_per_pixel;
    img->type = BGR;

    fclose(fptr);

    printf("img_file_init: OK reading %s\n", path);

    return true;
}

void img_file_shutdown(img_file_t* img)
{
    if (img != NULL) {
        free(img);
        img = NULL;
        printf("imagefile_shutdown: OK!\n");
    }
}