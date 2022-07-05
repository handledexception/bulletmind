#ifndef H_MEDIA_ENUMS
#define H_MEDIA_ENUMS

#include "core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FourCC
#define FourCC(fcc)                                               \
	((((u32)(fcc)&0xFF) << 24) | (((u32)(fcc)&0xFF00) << 8) | \
	 (((u32)(fcc)&0xFF0000) >> 8) | (((u32)(fcc)&0xFF000000) >> 24))
#endif

enum image_format {
	MEDIA_CODEC_BMP = FourCC('bmp '),
	MEDIA_CODEC_JPEG = FourCC('jfif'),
	MEDIA_CODEC_PNG = FourCC('png '),
	MEDIA_CODEC_TARGA = FourCC('tga '),
	MEDIA_CODEC_UNKNOWN = FourCC('unkn')
};

// enum class AudioFormat : FourCC {
// 	PCM_Integer = FourCC('lpcm'),
// 	PCM_Float = FourCC('fpcm'),
// 	AAC = FourCC('aac '),
// 	FLAC = FourCC('flac'),
// 	MP3 = FourCC('mp3 '),
// 	MPEG = FourCC('mpa '),
// 	Unknown = FourCC('NONE')
// };

// enum class AudioSampleFormat : FourCC {
// 	PCM_U16LE = FourCC('raw '),
// 	PCM_S16BE = FourCC('twos'),
// 	PCM_S16LE = FourCC('sowt'),
// 	PCM_S24BE = FourCC('in24'),
// 	PCM_S32BE = FourCC('in32'),
// 	PCM_F32 = FourCC('fl32'),
// 	PCM_F64 = FourCC('fl64'),
// 	PCM_uLaw = FourCC('ulaw'),
// 	PCM_aLaw = FourCC('alaw'),
// 	Unknown = FourCC('NONE')
// };

// enum class VideoFormat : FourCC {
// 	RawRGB = FourCC('raw '),
// 	RawYUV = FourCC('yuv '),
// 	H264 = FourCC('H264'),
// 	H265 = FourCC('H265'),
// 	MJPEG = FourCC('MJPG'),
// 	VP8 = FourCC('VP80'),
// 	VP9 = FourCC('VP90'),
// 	Unknown = FourCC('NONE')
// };

enum pixel_format {
	PIX_FMT_NONE = 0,
	/* Uncompressed RGB */
	PIX_FMT_RGB8 = 0x00000003,
	PIX_FMT_RGB555 = FourCC('r555'),
	PIX_FMT_RGB565 = FourCC('r565'),
	PIX_FMT_RGB24 = FourCC('r24 '),
	PIX_FMT_RGB32 = FourCC('r32 '),
	PIX_FMT_RGBA32 = FourCC('rgba'), // 0x41424752
	PIX_FMT_ARGB32 = FourCC('argb'),
	PIX_FMT_BGRA32 = FourCC('bgra'),
	PIX_FMT_A2R10G10B10 = FourCC('r32a'),
	PIX_FMT_A16B16G16R16F = FourCC('r36f'),
	/* YUV 8-bit/Palettized */
	PIX_FMT_AI44 = FourCC('AI44'),
	PIX_FMT_AYUV = FourCC('AYUV'),
	PIX_FMT_I420 = FourCC('I420'),
	PIX_FMT_IYUV = FourCC('IYUV'),
	PIX_FMT_NV11 = FourCC('NV11'),
	PIX_FMT_NV12 = FourCC('NV12'),
	PIX_FMT_UYVY = FourCC('UYVY'),
	PIX_FMT_Y41P = FourCC('Y41P'),
	PIX_FMT_Y41T = FourCC('Y41T'),
	PIX_FMT_Y42T = FourCC('Y42T'),
	PIX_FMT_YUY2 = FourCC('YUY2'),
	PIX_FMT_YVU9 = FourCC('YVU9'),
	PIX_FMT_YV12 = FourCC('YV12'),
	PIX_FMT_YVYU = FourCC('YVYU'),
	/* YUV 10-bit & 16-bit */
	PIX_FMT_P010 = FourCC('P010'),
	PIX_FMT_P016 = FourCC('P016'),
	PIX_FMT_P210 = FourCC('P210'),
	PIX_FMT_P216 = FourCC('P216'),
	PIX_FMT_v210 = FourCC('v210'),
	PIX_FMT_v216 = FourCC('v216'),
	PIX_FMT_v410 = FourCC('v410'),
	PIX_FMT_Y210 = FourCC('Y210'),
	PIX_FMT_Y216 = FourCC('Y216'),
	PIX_FMT_Y410 = FourCC('Y410'),
	PIX_FMT_Y416 = FourCC('Y416'),
	/* DirectX Specific */
	PIX_FMT_DEPTH_24_8 = FourCC('D248'),
	PIX_FMT_UNKNOWN = FourCC('NONE')
};

// struct FrameRate {
// 	u32 numerator;
// 	u32 denominator;
// };

// struct VideoProperties {
// 	VideoFormat vid_fmt;
// 	PixelFormat pix_fmt;
// 	core::Size frame_size;
// 	FrameRate frame_rate;
// 	u32 stride;
// 	bool is_compressed;
// };

// struct AudioProperties {
// 	AudioFormat fmt;
// 	AudioSampleFormat sample_fmt;
// 	u32 channels;
// 	u32 sample_rate;
// 	u32 bytes_per_sample;
// 	bool is_compressed;
// };

// #if defined(PAS_WINDOWS)

// using MFVideoFormat = GUID;
// using MFAudioFormat = GUID;

// static inline auto PixelFormatToMFVideoFormat(PixelFormat pf) -> MFVideoFormat
// {
// 	/*! \brief Get Microsoft Media Foundation video format GUID from our PixelFormat
// 	 * @param pf - The PixelFormat enum
// 	 * @return GUID representing a Media Foundation video format
// 	 */
// 	if (pf == PixelFormat::RGB8)
// 		return MFVideoFormat_RGB8;
// 	if (pf == PixelFormat::RGB555)
// 		return MFVideoFormat_RGB555;
// 	if (pf == PixelFormat::RGB565)
// 		return MFVideoFormat_RGB565;
// 	if (pf == PixelFormat::RGB24)
// 		return MFVideoFormat_RGB24;
// 	if (pf == PixelFormat::RGB32)
// 		return MFVideoFormat_RGB32;
// 	if (pf == PixelFormat::ARGB32)
// 		return MFVideoFormat_ARGB32;
// 	if (pf == PixelFormat::A2R10G10B10)
// 		return MFVideoFormat_A2R10G10B10;
// 	if (pf == PixelFormat::A16B16G16R16F)
// 		return MFVideoFormat_A16B16G16R16F;
// 	if (pf == PixelFormat::AI44)
// 		return MFVideoFormat_AI44;
// 	if (pf == PixelFormat::AYUV)
// 		return MFVideoFormat_AYUV;
// 	if (pf == PixelFormat::I420)
// 		return MFVideoFormat_I420;
// 	if (pf == PixelFormat::IYUV)
// 		return MFVideoFormat_IYUV;
// 	if (pf == PixelFormat::NV11)
// 		return MFVideoFormat_NV11;
// 	if (pf == PixelFormat::NV12)
// 		return MFVideoFormat_NV12;
// 	if (pf == PixelFormat::UYVY)
// 		return MFVideoFormat_UYVY;
// 	if (pf == PixelFormat::Y41P)
// 		return MFVideoFormat_Y41P;
// 	if (pf == PixelFormat::Y41T)
// 		return MFVideoFormat_Y41T;
// 	if (pf == PixelFormat::Y42T)
// 		return MFVideoFormat_Y42T;
// 	if (pf == PixelFormat::YUY2)
// 		return MFVideoFormat_YUY2;
// 	if (pf == PixelFormat::YVU9)
// 		return MFVideoFormat_YVU9;
// 	if (pf == PixelFormat::YV12)
// 		return MFVideoFormat_YV12;
// 	if (pf == PixelFormat::YVYU)
// 		return MFVideoFormat_YVYU;
// 	if (pf == PixelFormat::P010)
// 		return MFVideoFormat_P010;
// 	if (pf == PixelFormat::P016)
// 		return MFVideoFormat_P016;
// 	if (pf == PixelFormat::P210)
// 		return MFVideoFormat_P210;
// 	if (pf == PixelFormat::P216)
// 		return MFVideoFormat_P216;
// 	if (pf == PixelFormat::v210)
// 		return MFVideoFormat_v210;
// 	if (pf == PixelFormat::v216)
// 		return MFVideoFormat_v216;
// 	if (pf == PixelFormat::v410)
// 		return MFVideoFormat_v410;
// 	if (pf == PixelFormat::Y210)
// 		return MFVideoFormat_Y210;
// 	if (pf == PixelFormat::Y216)
// 		return MFVideoFormat_Y216;
// 	if (pf == PixelFormat::Y410)
// 		return MFVideoFormat_Y410;
// 	if (pf == PixelFormat::Y416)
// 		return MFVideoFormat_Y416;
// 	return MFVideoFormat_Base;
// }

// static inline auto MFVideoFormatToVideoFormat(const MFVideoFormat &guid)
// {
// 	/*! \brief Get VideoFormat enum from a Microsoft Media Foundation video format GUID
// 	 * @param guid - GUID representing the Media Foundation video format
// 	 * @return The VideoFormat enum
// 	 */
// 	if (guid == MFVideoFormat_MJPG)
// 		return VideoFormat::MJPEG;
// 	if (guid == MFVideoFormat_H264)
// 		return VideoFormat::H264;
// 	if (guid == MFVideoFormat_H265)
// 		return VideoFormat::H265;
// 	if (guid == MFVideoFormat_HEVC)
// 		return VideoFormat::H265;
// 	if (guid == MFVideoFormat_VP80)
// 		return VideoFormat::VP8;
// 	if (guid == MFVideoFormat_VP90)
// 		return VideoFormat::VP9;
// 	return VideoFormat::Unknown;
// }

// static inline auto MFAudioFormatToAudioFormat(const MFAudioFormat &guid)
// 	-> AudioFormat
// {
// 	/*! \brief Get AudioFormat enum from a Microsoft Media Foundation audio format GUID
// 	 * @param guid - The MediaFoundation audio format GUID
// 	 * @return The AudioFormat enum
// 	 */
// 	if (guid == MFAudioFormat_PCM)
// 		return AudioFormat::PCM_Integer;
// 	if (guid == MFAudioFormat_Float)
// 		return AudioFormat::PCM_Float;
// 	// MFAudioFormat_DTS
// 	// MFAudioFormat_Dolby_AC3_SPDIF
// 	// MFAudioFormat_DRM
// 	// MFAudioFormat_WMAudioV8
// 	// MFAudioFormat_WMAudioV9
// 	// MFAudioFormat_WMAudio_Lossless
// 	// MFAudioFormat_WMASPDIF
// 	// MFAudioFormat_MSP1
// 	if (guid == MFAudioFormat_MP3)
// 		return AudioFormat::MP3;
// 	// MFAudioFormat_MPEG
// 	if (guid == MFAudioFormat_AAC)
// 		return AudioFormat::AAC;
// 	// MFAudioFormat_ADTS
// 	// MFAudioFormat_AMR_NB
// 	// MFAudioFormat_AMR_WB
// 	// MFAudioFormat_AMR_WP
// 	if (guid == MFAudioFormat_FLAC)
// 		return AudioFormat::FLAC;
// 	// MFAudioFormat_ALAC
// 	// MFAudioFormat_Opus
// 	// MFAudioFormat_Dolby_AC4
// 	// MFAudioFormat_Dolby_AC3
// 	// MFAudioFormat_Dolby_DDPlus
// 	// MFAudioFormat_Dolby_AC4_V1
// 	// MFAudioFormat_Dolby_AC4_V2
// 	// MFAudioFormat_Dolby_AC4_V1_ES
// 	// MFAudioFormat_Dolby_AC4_V2_ES
// 	// MFAudioFormat_Vorbis
// 	// MFAudioFormat_DTS_RAW
// 	// MFAudioFormat_DTS_HD
// 	// MFAudioFormat_DTS_XLL
// 	// MFAudioFormat_DTS_LBR
// 	// MFAudioFormat_DTS_UHD
// 	// MFAudioFormat_DTS_UHDY
// 	// MFAudioFormat_Float_SpatialObjects
// 	// MFAudioFormat_LPCM
// 	// MFAudioFormat_PCM_HDCP
// 	// MFAudioFormat_Dolby_AC3_HDCP
// 	// MFAudioFormat_AAC_HDCP
// 	// MFAudioFormat_ADTS_HDCP
// 	// MFAudioFormat_Base_HDCP
// 	return AudioFormat::Unknown;
// }

// static inline auto MFFormatToPixelFormat(const MFVideoFormat &guid)
// 	-> PixelFormat
// {
// 	/*! \brief Get PixelFormat enum from a Microsoft Media Foundation video format GUID
// 	 * @param guid - GUID representing a Media Foundation video format
// 	 * @return The PixelFormat enum
// 	 */
// 	if (guid == MFVideoFormat_RGB8)
// 		return PixelFormat::RGB8;
// 	if (guid == MFVideoFormat_RGB555)
// 		return PixelFormat::RGB555;
// 	if (guid == MFVideoFormat_RGB565)
// 		return PixelFormat::RGB565;
// 	if (guid == MFVideoFormat_RGB24)
// 		return PixelFormat::RGB24;
// 	if (guid == MFVideoFormat_RGB32)
// 		return PixelFormat::RGB32;
// 	if (guid == MFVideoFormat_ARGB32)
// 		return PixelFormat::ARGB32;
// 	if (guid == MFVideoFormat_A2R10G10B10)
// 		return PixelFormat::A2R10G10B10;
// 	if (guid == MFVideoFormat_A16B16G16R16F)
// 		return PixelFormat::A16B16G16R16F;
// 	if (guid == MFVideoFormat_AI44)
// 		return PixelFormat::AI44;
// 	if (guid == MFVideoFormat_AYUV)
// 		return PixelFormat::AYUV;
// 	if (guid == MFVideoFormat_I420)
// 		return PixelFormat::I420;
// 	if (guid == MFVideoFormat_IYUV)
// 		return PixelFormat::IYUV;
// 	if (guid == MFVideoFormat_NV11)
// 		return PixelFormat::NV11;
// 	if (guid == MFVideoFormat_NV12)
// 		return PixelFormat::NV12;
// 	if (guid == MFVideoFormat_UYVY)
// 		return PixelFormat::UYVY;
// 	if (guid == MFVideoFormat_Y41P)
// 		return PixelFormat::Y41P;
// 	if (guid == MFVideoFormat_Y41T)
// 		return PixelFormat::Y41T;
// 	if (guid == MFVideoFormat_Y42T)
// 		return PixelFormat::Y42T;
// 	if (guid == MFVideoFormat_YUY2)
// 		return PixelFormat::YUY2;
// 	if (guid == MFVideoFormat_YVU9)
// 		return PixelFormat::YVU9;
// 	if (guid == MFVideoFormat_YV12)
// 		return PixelFormat::YV12;
// 	if (guid == MFVideoFormat_YVYU)
// 		return PixelFormat::YVYU;
// 	if (guid == MFVideoFormat_P010)
// 		return PixelFormat::P010;
// 	if (guid == MFVideoFormat_P016)
// 		return PixelFormat::P016;
// 	if (guid == MFVideoFormat_P210)
// 		return PixelFormat::P210;
// 	if (guid == MFVideoFormat_P216)
// 		return PixelFormat::P216;
// 	if (guid == MFVideoFormat_v210)
// 		return PixelFormat::v210;
// 	if (guid == MFVideoFormat_v216)
// 		return PixelFormat::v216;
// 	if (guid == MFVideoFormat_v410)
// 		return PixelFormat::v410;
// 	if (guid == MFVideoFormat_Y210)
// 		return PixelFormat::Y210;
// 	if (guid == MFVideoFormat_Y216)
// 		return PixelFormat::Y216;
// 	if (guid == MFVideoFormat_Y410)
// 		return PixelFormat::Y410;
// 	if (guid == MFVideoFormat_Y416)
// 		return PixelFormat::Y416;
// 	return PixelFormat::None;
// }

// static inline auto MFVideoFormatIsEncoded(const MFVideoFormat &guid) -> bool
// {
// 	/*! \brief Determine if Media Foundation video format represents encoded data
// 	 * @param guid - GUID representing the Media Foundation video format
// 	 * @return true if encoded, otherwise false
// 	 */
// 	return (guid == MFVideoFormat_MP43 || guid == MFVideoFormat_MP4S ||
// 		guid == MFVideoFormat_M4S2 || guid == MFVideoFormat_MP4V ||
// 		guid == MFVideoFormat_WMV1 || guid == MFVideoFormat_WMV2 ||
// 		guid == MFVideoFormat_WMV3 || guid == MFVideoFormat_WVC1 ||
// 		guid == MFVideoFormat_MSS1 || guid == MFVideoFormat_MSS2 ||
// 		guid == MFVideoFormat_MPG1 || guid == MFVideoFormat_DVSL ||
// 		guid == MFVideoFormat_DVSD || guid == MFVideoFormat_DVHD ||
// 		guid == MFVideoFormat_DV25 || guid == MFVideoFormat_DV50 ||
// 		guid == MFVideoFormat_DVH1 || guid == MFVideoFormat_DVC ||
// 		guid == MFVideoFormat_H264 || guid == MFVideoFormat_H265 ||
// 		guid == MFVideoFormat_MJPG || guid == MFVideoFormat_420O ||
// 		guid == MFVideoFormat_HEVC || guid == MFVideoFormat_HEVC_ES ||
// 		guid == MFVideoFormat_VP80 || guid == MFVideoFormat_VP90 ||
// 		guid == MFVideoFormat_ORAW);
// }

// constexpr inline auto PixelFormatToDXGIFormat(PixelFormat pf) -> DXGI_FORMAT
// {
// 	/*! \brief Get DXGI_FORMAT from PixelFormat enum
// 	 * @param pf - The PixelFormat enum
// 	 * @return DXGI_FORMAT corresponding to our PixelFormat
// 	 */
// 	switch (pf) {
// 	case PixelFormat::RGBA32:
// 		return DXGI_FORMAT_R8G8B8A8_UNORM;
// 	case PixelFormat::BGRA32:
// 		return DXGI_FORMAT_B8G8R8A8_UNORM;
// 	case PixelFormat::Y210:
// 		return DXGI_FORMAT_Y210;
// 	case PixelFormat::NV12:
// 		return DXGI_FORMAT_NV12;
// 	case PixelFormat::Depth24Stencil8:
// 		return DXGI_FORMAT_D24_UNORM_S8_UINT;
// 	default:
// 		return DXGI_FORMAT_UNKNOWN;
// 	}
// }
// #endif // PAS_WINDOWS

/*
    Microsoft Media Foundation Video Formats - https://docs.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids

    Uncompressed RGB Formats
    Table 1
    GUID                           FORMAT    Description
    MFVideoFormat_RGB8             RGB       8 bits per pixel (bpp). (Same memory layout as D3DFMT_P8.)
    MFVideoFormat_RGB555           RGB       555, 16 bpp. (Same memory layout as D3DFMT_X1R5G5B5.)
    MFVideoFormat_RGB565           RGB       565, 16 bpp. (Same memory layout as D3DFMT_R5G6B5.)
    MFVideoFormat_RGB24            RGB       24 bpp.
    MFVideoFormat_RGB32            RGB       32 bpp.
    MFVideoFormat_ARGB32           RGB       32 bpp with alpha channel.
    MFVideoFormat_A2R10G10B10      RGB       10 bpp for each color and 2 bpp for alpha. (Same memory layout as D3DFMT_A2B10G10R10)
    MFVideoFormat_A16B16G16R16F    RGB       16 bpp with alpha channel. (Same memory layout as D3DFMT_A16B16G16R16F)

    YUV Formats: 8-Bit and Palettized
    Table 2
    GUID                Format  Sampling    Packed or planar    Bits per channel
    MFVideoFormat_AI44  AI44    4:4:4       Packed              Palettized
    MFVideoFormat_AYUV  AYUV    4:4:4       Packed              8
    MFVideoFormat_I420  I420    4:2:0       Planar              8
    MFVideoFormat_IYUV  IYUV    4:2:0       Planar              8
    MFVideoFormat_NV11  NV11    4:1:1       Planar              8
    MFVideoFormat_NV12  NV12    4:2:0       Planar              8
    MFVideoFormat_UYVY  UYVY    4:2:2       Packed              8
    MFVideoFormat_Y41P  Y41P    4:1:1       Packed              8
    MFVideoFormat_Y41T  Y41T    4:1:1       Packed              8
    MFVideoFormat_Y42T  Y42T    4:2:2       Packed              8
    MFVideoFormat_YUY2  YUY2    4:2:2       Packed              8
    MFVideoFormat_YVU9  YVU9    8:4:4       Planar              9
    MFVideoFormat_YV12  YV12    4:2:0       Planar              8
    MFVideoFormat_YVYU  YVYU    4:2:2       Packed              8

    YUV Formats: 10-Bit and 16-Bit
    Table 3
    GUID                Format  Sampling    Packed or planar    Bits per channel
    MFVideoFormat_P010  P010    4:2:0       Planar              10
    MFVideoFormat_P016  P016    4:2:0       Planar              16
    MFVideoFormat_P210  P210    4:2:2       Planar              10
    MFVideoFormat_P216  P216    4:2:2       Planar              16
    MFVideoFormat_v210  v210    4:2:2       Packed              10
    MFVideoFormat_v216  v216    4:2:2       Packed              16
    MFVideoFormat_v410  v40     4:4:4       Packed              10
    MFVideoFormat_Y210  Y210    4:2:2       Packed              10
    MFVideoFormat_Y216  Y216    4:2:2       Packed              16
    MFVideoFormat_Y410  Y40     4:4:4       Packed              10
    MFVideoFormat_Y416  Y416    4:4:4       Packed              16
*/

#ifdef __cplusplus
}
#endif

#endif // H_MEDIA_ENUMS
