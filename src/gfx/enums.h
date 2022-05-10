#ifndef H_GFX_ENUMS
#define H_GFX_ENUMS

#ifdef __cplusplus
extern "C" {
#endif

enum gfx_shader_var_type {
	GFX_SHADER_PARAM_BOOL,
	GFX_SHADER_PARAM_S32,
	GFX_SHADER_PARAM_U32,
	GFX_SHADER_PARAM_F32,
	GFX_SHADER_PARAM_F64,
	GFX_SHADER_PARAM_VEC2,
	GFX_SHADER_PARAM_VEC3,
	GFX_SHADER_PARAM_VEC4,
	GFX_SHADER_PARAM_MAT4,
	GFX_SHADER_PARAM_TEX
};

enum gfx_shader_type {
	GFX_SHADER_VERTEX,
	GFX_SHADER_PIXEL,
	GFX_SHADER_COMPUTE,
	GFX_SHADER_GEOMETRY,
	GFX_SHADER_UNKNOWN
};

enum gfx_pixel_format {
	GFX_FORMAT_BGRA,
	GFX_FORMAT_FIRST = GFX_FORMAT_BGRA,
	GFX_FORMAT_RGBA,
	GFX_FORMAT_ARGB,
	GFX_FORMAT_RGB24,
	GFX_FORMAT_NV12,
	GFX_FORMAT_DEPTH_U16,
	GFX_FORMAT_DEPTH_U24,
	GFX_FORMAT_DEPTH_F32,
	GFX_FORMAT_UNKNOWN,
	GFX_FORMAT_LAST = GFX_FORMAT_UNKNOWN
};

enum gfx_texture_type {
	GFX_TEXTURE_1D,
	GFX_TEXTURE_2D,
	GFX_TEXTURE_3D,
	GFX_TEXTURE_CUBE,
};

enum gfx_topology {
	GFX_TOPOLOGY_POINT_LIST,
	GFX_TOPOLOGY_LINE_LIST,
	GFX_TOPOLOGY_LINE_STRIP,
	GFX_TOPOLOGY_TRIANGLE_LIST,
	GFX_TOPOLOGY_TRIANGLE_STRIP
};

enum gfx_culling_mode {
	GFX_CULLING_NONE,
	GFX_CULLING_FRONT_FACE,
	GFX_CULLING_BACK_FACE,
};

enum gfx_raster_flags {
	GFX_RASTER_WINDING_CCW = (1 << 0),
	GFX_RASTER_SCISSOR = (1 << 1),
	GFX_RASTER_MULTI_SAMPLE = (1 << 2),
	GFX_RASTER_WIREFRAME = (1 << 3),
	GFX_RASTER_ANTIALIAS_LINES = (1 << 4),
};

enum gfx_sample_filter {
	GFX_FILTER_POINT,
	GFX_FILTER_LINEAR,
	GFX_FILTER_ANISOTROPIC,
	GFX_FILTER_MIN_MAG_POINT_MIP_LINEAR,
	GFX_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
	GFX_FILTER_MIN_POINT_MAG_MIP_LINEAR,
	GFX_FILTER_MIN_LINEAR_MAG_MIP_POINT,
	GFX_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
	GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	GFX_FILTER_UNKNOWN,
};

enum gfx_sample_address_mode {
	GFX_SAMPLER_ADDRESS_REPEAT,
	GFX_SAMPLER_ADDRESS_CLAMP,
	GFX_SAMPLER_ADDRESS_MIRROR,
	GFX_SAMPLER_ADDRESS_UNKNOWN,
};

enum gfx_texture_flags {
	GFX_TEXTURE_SHARED = (1 << 0),
	GFX_TEXTURE_KEYED_MUTEX = (1 << 1),
	GFX_TEXTURE_DYNAMIC = (1 << 2),
	GFX_TEXTURE_IS_RENDER_TARGET = (1 << 3),
	GFX_TEXTURE_IS_ZSTENCIL = (1 << 4),
	GFX_TEXTURE_GENERATE_MIPMAPS = (1 << 5),
};

enum gfx_buffer_type {
	GFX_BUFFER_VERTEX,
	GFX_BUFFER_INDEX,
	GFX_BUFFER_CONSTANT,
	GFX_BUFFER_UNKNOWN
};

enum gfx_buffer_usage {
	GFX_BUFFER_USAGE_DEFAULT,
	GFX_BUFFER_USAGE_DYNAMIC,
	GFX_BUFFER_USAGE_IMMUTABLE,
	GFX_BUFFER_USAGE_STAGING,
	GFX_BUFFER_USAGE_UNKNOWN
};

enum gfx_vertex_type {
	GFX_VERTEX_POS_UV = 0,
	GFX_VERTEX_FIRST = GFX_VERTEX_POS_UV,
	GFX_VERTEX_POS_NORM_UV = 1,
	GFX_VERTEX_POS_COLOR = 2,
	GFX_VERTEX_POS_NORM_COLOR = 3,
	GFX_VERTEX_UNKNOWN = 4,
	GFX_VERTEX_LAST = GFX_VERTEX_UNKNOWN
};

enum gfx_vertex_attribute {
	GFX_VERTEX_POS,
	GFX_VERTEX_UV,
	GFX_VERTEX_COLOR,
};

typedef enum {
	GFX_D3D11 = 1 << 0,
	GFX_OPENGL = 1 << 1,
	GFX_USE_ZBUFFER = 1 << 2,
	GFX_INVALID = 1 << 3
} gfx_system_flags;

#ifdef __cplusplus
}
#endif

#endif
