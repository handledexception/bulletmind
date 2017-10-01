#include "imgfile.h"
#include <stdio.h>
#include <stdlib.h>

struct tga_header_s {
    uint8_t id_length;             // field 1
    uint8_t color_map_type;        // field 2
    uint8_t image_type;            // field 3
                        // field 4
    uint16_t first_entry_index;
    uint16_t color_map_length;
    uint8_t color_map_entry_size;    
    
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t img_width;
    uint16_t img_height;
    uint8_t img_bpp;
    uint8_t img_desc;
} __attribute__((packed));

struct tga_header_s header;

typedef struct imgfile_s {
    int32_t width, height;
    uint8_t *data;
} imgfile_t;

bool imgfile_init(const char *path)
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
        printf("imgfile_init: error reading file %s!\n", path);
        return false;
    }
    else if (fread(buf, sizeof(*buf), fsize, fptr) != fsize) {
        printf("imgfile_init: error reading entire file %s\n", path);
        return false;
    }
    
    struct tga_header_s *header = (struct tga_header_s *)buf;
    printf("%d\n", sizeof(struct tga_header_s));
    printf("%dpx, %dpx @ %dbpp\n\n", header->img_width, header->img_height, header->img_bpp);

    int32_t bytecount = 0;
    do {
        printf("%d\n", *buf);
        buf++;
        bytecount++;
    } while(bytecount < 18);

    fclose(fptr);

    return true;
}