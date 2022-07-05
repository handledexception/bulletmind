#ifndef H_MEDIA_TARGA
#define H_MEDIA_TARGA

#include "core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tga_header_s {
	u8 id_length;         /* field 1 */
	u8 color_map_type;    /* field 2 */
	u8 image_type;        /* field 3 */
	u8 color_map_spec[5]; /* field 4 */
	// u16 first_entry_index;
	// u16 color_map_length;
	// u8 color_map_entry_size;
	u16 x_org; /* field 5 */
	u16 y_org;
	u16 width;
	u16 height;
	u8 bpp;
	u8 desc; /* bits 3-0 = alpha channel depth
                                 * bits 5-4 = img direction */
} tga_header_t;

#ifdef __cplusplus
}
#endif

#endif // H_MEDIA_TARGA
