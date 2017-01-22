/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : FFMPEGInterface.cpp                                                                       */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Interface to FFMPEG library                                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include <iostream>
#include "FFMPEGInterface.hpp"
#include "GLIPLib.hpp"

namespace FFMPEGInterface
{
	#define FORMAT(fmt) 	{ fmt, #fmt },
	// A list of formats can be found at : http://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5a1aa7677092740d8def31655b5d7f0cc2
	const FFMPEGContext::Format FFMPEGContext::formatsList[] = {	FORMAT(AV_PIX_FMT_YUV420P)
									FORMAT(AV_PIX_FMT_YUYV422)
									FORMAT(AV_PIX_FMT_RGB24)
									FORMAT(AV_PIX_FMT_BGR24)
									FORMAT(AV_PIX_FMT_YUV422P)
									FORMAT(AV_PIX_FMT_YUV444P)
									FORMAT(AV_PIX_FMT_YUV410P)
									FORMAT(AV_PIX_FMT_YUV411P)
									FORMAT(AV_PIX_FMT_GRAY8)
									FORMAT(AV_PIX_FMT_MONOWHITE)
									FORMAT(AV_PIX_FMT_MONOBLACK)
									FORMAT(AV_PIX_FMT_PAL8)
									FORMAT(AV_PIX_FMT_YUVJ420P)
									FORMAT(AV_PIX_FMT_YUVJ422P)
									FORMAT(AV_PIX_FMT_YUVJ444P)
									FORMAT(AV_PIX_FMT_XVMC_MPEG2_MC)
									FORMAT(AV_PIX_FMT_XVMC_MPEG2_IDCT)
									#ifdef AV_PIX_FMT_XVMC
										FORMAT(AV_PIX_FMT_XVMC)
									#endif
									FORMAT(AV_PIX_FMT_UYVY422)
									FORMAT(AV_PIX_FMT_UYYVYY411)
									FORMAT(AV_PIX_FMT_BGR8)
									FORMAT(AV_PIX_FMT_BGR4)
									FORMAT(AV_PIX_FMT_BGR4_BYTE)
									FORMAT(AV_PIX_FMT_RGB8)
									FORMAT(AV_PIX_FMT_RGB4)
									FORMAT(AV_PIX_FMT_RGB4_BYTE)
									FORMAT(AV_PIX_FMT_NV12)
									FORMAT(AV_PIX_FMT_NV21)
									FORMAT(AV_PIX_FMT_ARGB)
									FORMAT(AV_PIX_FMT_RGBA)
									FORMAT(AV_PIX_FMT_ABGR)
									FORMAT(AV_PIX_FMT_BGRA)
									FORMAT(AV_PIX_FMT_GRAY16BE)
									FORMAT(AV_PIX_FMT_GRAY16LE)
									FORMAT(AV_PIX_FMT_YUV440P)
									FORMAT(AV_PIX_FMT_YUVJ440P)
									FORMAT(AV_PIX_FMT_YUVA420P)
									FORMAT(AV_PIX_FMT_VDPAU_H264)
									FORMAT(AV_PIX_FMT_VDPAU_MPEG1)
									FORMAT(AV_PIX_FMT_VDPAU_MPEG2)
									FORMAT(AV_PIX_FMT_VDPAU_WMV3)
									FORMAT(AV_PIX_FMT_VDPAU_VC1)
									FORMAT(AV_PIX_FMT_RGB48BE)
									FORMAT(AV_PIX_FMT_RGB48LE)
									FORMAT(AV_PIX_FMT_RGB565BE)
									FORMAT(AV_PIX_FMT_RGB565LE)
									FORMAT(AV_PIX_FMT_RGB555BE)
									FORMAT(AV_PIX_FMT_RGB555LE)
									FORMAT(AV_PIX_FMT_BGR565BE)
									FORMAT(AV_PIX_FMT_BGR565LE)
									FORMAT(AV_PIX_FMT_BGR555BE)
									FORMAT(AV_PIX_FMT_BGR555LE)
									FORMAT(AV_PIX_FMT_VAAPI_MOCO)
									FORMAT(AV_PIX_FMT_VAAPI_IDCT)
									FORMAT(AV_PIX_FMT_VAAPI_VLD)
									#ifdef AV_PIX_FMT_VAAPI
										FORMAT(AV_PIX_FMT_VAAPI)
									#endif
									FORMAT(AV_PIX_FMT_YUV420P16LE)
									FORMAT(AV_PIX_FMT_YUV420P16BE)
									FORMAT(AV_PIX_FMT_YUV422P16LE)
									FORMAT(AV_PIX_FMT_YUV422P16BE)
									FORMAT(AV_PIX_FMT_YUV444P16LE)
									FORMAT(AV_PIX_FMT_YUV444P16BE)
									FORMAT(AV_PIX_FMT_VDPAU_MPEG4)
									FORMAT(AV_PIX_FMT_DXVA2_VLD)
									FORMAT(AV_PIX_FMT_RGB444LE)
									FORMAT(AV_PIX_FMT_RGB444BE)
									FORMAT(AV_PIX_FMT_BGR444LE)
									FORMAT(AV_PIX_FMT_BGR444BE)
									FORMAT(AV_PIX_FMT_YA8)
									FORMAT(AV_PIX_FMT_Y400A)
									#ifdef AV_PIX_FMT_GRAY8A
										FORMAT(AV_PIX_FMT_GRAY8A)
									#endif
									FORMAT(AV_PIX_FMT_BGR48BE)
									FORMAT(AV_PIX_FMT_BGR48LE)
									FORMAT(AV_PIX_FMT_YUV420P9BE)
									FORMAT(AV_PIX_FMT_YUV420P9LE)
									FORMAT(AV_PIX_FMT_YUV420P10BE)
									FORMAT(AV_PIX_FMT_YUV420P10LE)
									FORMAT(AV_PIX_FMT_YUV422P10BE)
									FORMAT(AV_PIX_FMT_YUV422P10LE)
									FORMAT(AV_PIX_FMT_YUV444P9BE)
									FORMAT(AV_PIX_FMT_YUV444P9LE)
									FORMAT(AV_PIX_FMT_YUV444P10BE)
									FORMAT(AV_PIX_FMT_YUV444P10LE)
									FORMAT(AV_PIX_FMT_YUV422P9BE)
									FORMAT(AV_PIX_FMT_YUV422P9LE)
									FORMAT(AV_PIX_FMT_VDA_VLD)
									FORMAT(AV_PIX_FMT_GBRP)
									#ifdef AV_PIX_FMT_GBR24P
										FORMAT(AV_PIX_FMT_GBR24P)
									#endif
									FORMAT(AV_PIX_FMT_GBRP9BE)
									FORMAT(AV_PIX_FMT_GBRP9LE)
									FORMAT(AV_PIX_FMT_GBRP10BE)
									FORMAT(AV_PIX_FMT_GBRP10LE)
									FORMAT(AV_PIX_FMT_GBRP16BE)
									FORMAT(AV_PIX_FMT_GBRP16LE)
									FORMAT(AV_PIX_FMT_YUVA422P)
									FORMAT(AV_PIX_FMT_YUVA444P)
									FORMAT(AV_PIX_FMT_YUVA420P9BE)
									FORMAT(AV_PIX_FMT_YUVA420P9LE)
									FORMAT(AV_PIX_FMT_YUVA422P9BE)
									FORMAT(AV_PIX_FMT_YUVA422P9LE)
									FORMAT(AV_PIX_FMT_YUVA444P9BE)
									FORMAT(AV_PIX_FMT_YUVA444P9LE)
									FORMAT(AV_PIX_FMT_YUVA420P10BE)
									FORMAT(AV_PIX_FMT_YUVA420P10LE)
									FORMAT(AV_PIX_FMT_YUVA422P10BE)
									FORMAT(AV_PIX_FMT_YUVA422P10LE)
									FORMAT(AV_PIX_FMT_YUVA444P10BE)
									FORMAT(AV_PIX_FMT_YUVA444P10LE)
									FORMAT(AV_PIX_FMT_YUVA420P16BE)
									FORMAT(AV_PIX_FMT_YUVA420P16LE)
									FORMAT(AV_PIX_FMT_YUVA422P16BE)
									FORMAT(AV_PIX_FMT_YUVA422P16LE)
									FORMAT(AV_PIX_FMT_YUVA444P16BE)
									FORMAT(AV_PIX_FMT_YUVA444P16LE)
									FORMAT(AV_PIX_FMT_VDPAU)
									FORMAT(AV_PIX_FMT_XYZ12LE)
									FORMAT(AV_PIX_FMT_XYZ12BE)
									FORMAT(AV_PIX_FMT_NV16)
									FORMAT(AV_PIX_FMT_NV20LE)
									FORMAT(AV_PIX_FMT_NV20BE)
									FORMAT(AV_PIX_FMT_RGBA64BE)
									FORMAT(AV_PIX_FMT_RGBA64LE)
									FORMAT(AV_PIX_FMT_BGRA64BE)
									FORMAT(AV_PIX_FMT_BGRA64LE)
									FORMAT(AV_PIX_FMT_YVYU422)
									FORMAT(AV_PIX_FMT_VDA)
									FORMAT(AV_PIX_FMT_YA16BE)
									FORMAT(AV_PIX_FMT_YA16LE)
									#ifdef AV_PIX_FMT_GBRAP
										FORMAT(AV_PIX_FMT_GBRAP)
									#endif
									#ifdef AV_PIX_FMT_GBRAP16BE
										FORMAT(AV_PIX_FMT_GBRAP16BE)
									#endif
									#ifdef AV_PIX_FMT_GBRAP16LE
										FORMAT(AV_PIX_FMT_GBRAP16LE)
									#endif
									#ifdef AV_PIX_FMT_QSV
										FORMAT(AV_PIX_FMT_QSV)
									#endif
									#ifdef AV_PIX_FMT_MMAL
										FORMAT(AV_PIX_FMT_MMAL)
									#endif
									#ifdef AV_PIX_FMT_D3D11VA_VLD
										FORMAT(AV_PIX_FMT_D3D11VA_VLD)
									#endif
									#ifdef AV_PIX_FMT_CUDA
										FORMAT(AV_PIX_FMT_CUDA)
									#endif
									#ifdef AV_PIX_FMT_0RGB
										FORMAT(AV_PIX_FMT_0RGB)
									#endif
									#ifdef AV_PIX_FMT_RGB0
										FORMAT(AV_PIX_FMT_RGB0)
									#endif
									#ifdef AV_PIX_FMT_0BGR
										FORMAT(AV_PIX_FMT_0BGR)
									#endif
									#ifdef AV_PIX_FMT_BGR0
										FORMAT(AV_PIX_FMT_BGR0)
									#endif
									#ifdef AV_PIX_FMT_YUV420P12BE
										FORMAT(AV_PIX_FMT_YUV420P12BE)
									#endif
									#ifdef AV_PIX_FMT_YUV420P12LE
										FORMAT(AV_PIX_FMT_YUV420P12LE)
									#endif
									#ifdef AV_PIX_FMT_YUV420P14BE
										FORMAT(AV_PIX_FMT_YUV420P14BE)
									#endif
									#ifdef AV_PIX_FMT_YUV420P14LE
										FORMAT(AV_PIX_FMT_YUV420P14LE)
									#endif
									#ifdef AV_PIX_FMT_YUV422P12BE
										FORMAT(AV_PIX_FMT_YUV422P12BE)
									#endif
									#ifdef AV_PIX_FMT_YUV422P12LE
										FORMAT(AV_PIX_FMT_YUV422P12LE)
									#endif
									#ifdef AV_PIX_FMT_YUV422P14BE
										FORMAT(AV_PIX_FMT_YUV422P14BE)
									#endif
									#ifdef AV_PIX_FMT_YUV422P14LE
										FORMAT(AV_PIX_FMT_YUV422P14LE)
									#endif
									#ifdef AV_PIX_FMT_YUV444P12BE
										FORMAT(AV_PIX_FMT_YUV444P12BE)
									#endif
									#ifdef AV_PIX_FMT_YUV444P12LE
										FORMAT(AV_PIX_FMT_YUV444P12LE)
									#endif
									#ifdef AV_PIX_FMT_YUV444P14BE
										FORMAT(AV_PIX_FMT_YUV444P14BE)
									#endif
									#ifdef AV_PIX_FMT_YUV444P14LE
										FORMAT(AV_PIX_FMT_YUV444P14LE)
									#endif
									#ifdef AV_PIX_FMT_GBRP12BE
										FORMAT(AV_PIX_FMT_GBRP12BE)
									#endif
									#ifdef AV_PIX_FMT_GBRP12LE
										FORMAT(AV_PIX_FMT_GBRP12LE)
									#endif
									#ifdef AV_PIX_FMT_GBRP14BE
										FORMAT(AV_PIX_FMT_GBRP14BE)
									#endif
									#ifdef AV_PIX_FMT_GBRP14LE
										FORMAT(AV_PIX_FMT_GBRP14LE)
									#endif
									#ifdef AV_PIX_FMT_YUVJ411P
										FORMAT(AV_PIX_FMT_YUVJ411P)
									#endif
									#ifdef AV_PIX_FMT_BAYER_BGGR8
										FORMAT(AV_PIX_FMT_BAYER_BGGR8)
									#endif
									#ifdef AV_PIX_FMT_BAYER_RGGB8
										FORMAT(AV_PIX_FMT_BAYER_RGGB8)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GBRG8
										FORMAT(AV_PIX_FMT_BAYER_GBRG8)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GRBG8
										FORMAT(AV_PIX_FMT_BAYER_GRBG8)
									#endif
									#ifdef AV_PIX_FMT_BAYER_BGGR16LE
										FORMAT(AV_PIX_FMT_BAYER_BGGR16LE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_BGGR16BE
										FORMAT(AV_PIX_FMT_BAYER_BGGR16BE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_RGGB16LE
										FORMAT(AV_PIX_FMT_BAYER_RGGB16LE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_RGGB16BE
										FORMAT(AV_PIX_FMT_BAYER_RGGB16BE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GBRG16LE
										FORMAT(AV_PIX_FMT_BAYER_GBRG16LE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GBRG16BE
										FORMAT(AV_PIX_FMT_BAYER_GBRG16BE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GRBG16LE
										FORMAT(AV_PIX_FMT_BAYER_GRBG16LE)
									#endif
									#ifdef AV_PIX_FMT_BAYER_GRBG16BE
										FORMAT(AV_PIX_FMT_BAYER_GRBG16BE)
									#endif
									#ifdef AV_PIX_FMT_YUV440P10LE
										FORMAT(AV_PIX_FMT_YUV440P10LE)
									#endif
									#ifdef AV_PIX_FMT_YUV440P10BE
										FORMAT(AV_PIX_FMT_YUV440P10BE)
									#endif
									#ifdef AV_PIX_FMT_YUV440P12LE
										FORMAT(AV_PIX_FMT_YUV440P12LE)
									#endif
									#ifdef AV_PIX_FMT_YUV440P12BE
										FORMAT(AV_PIX_FMT_YUV440P12BE)
									#endif
									#ifdef AV_PIX_FMT_AYUV64LE
										FORMAT(AV_PIX_FMT_AYUV64LE)
									#endif
									#ifdef AV_PIX_FMT_AYUV64BE
										FORMAT(AV_PIX_FMT_AYUV64BE)
									#endif
									#ifdef AV_PIX_FMT_VIDEOTOOLBOX
										FORMAT(AV_PIX_FMT_VIDEOTOOLBOX)
									#endif
									#ifdef AV_PIX_FMT_P010LE
										FORMAT(AV_PIX_FMT_P010LE)
									#endif
									#ifdef AV_PIX_FMT_P010BE
										FORMAT(AV_PIX_FMT_P010BE)
									#endif
									#ifdef AV_PIX_FMT_GBRAP12BE
										FORMAT(AV_PIX_FMT_GBRAP12BE)
									#endif
									#ifdef AV_PIX_FMT_GBRAP12LE
										FORMAT(AV_PIX_FMT_GBRAP12LE)
									#endif
									#ifdef AV_PIX_FMT_GBRAP10BE
										FORMAT(AV_PIX_FMT_GBRAP10BE)
									#endif
									#ifdef AV_PIX_FMT_GBRAP10LE
										FORMAT(AV_PIX_FMT_GBRAP10LE)
									#endif
									#ifdef AV_PIX_FMT_MEDIACODEC
										FORMAT(AV_PIX_FMT_MEDIACODEC)
									#endif
									#ifdef AV_PIX_FMT_GRAY12BE
										FORMAT(AV_PIX_FMT_GRAY12BE)
									#endif
									#ifdef AV_PIX_FMT_GRAY12LE
										FORMAT(AV_PIX_FMT_GRAY12LE)
									#endif
									#ifdef AV_PIX_FMT_GRAY10BE
										FORMAT(AV_PIX_FMT_GRAY10BE)
									#endif
									#ifdef AV_PIX_FMT_GRAY10LE
										FORMAT(AV_PIX_FMT_GRAY10LE)
									#endif
									#ifdef AV_PIX_FMT_P016LE
										FORMAT(AV_PIX_FMT_P016LE)
									#endif
									#ifdef AV_PIX_FMT_P016BE
										FORMAT(AV_PIX_FMT_P016BE)
									#endif
									FORMAT(AV_PIX_FMT_NONE) };
	#undef FORMAT
	bool FFMPEGContext::initOnce = true;

	FFMPEGContext::FFMPEGContext(void)
	{
		if(initOnce)
		{
			av_register_all();
			initOnce = false;	
			#ifdef __FFMPEG_VERBOSE__
				#if defined(__FFMPEG_VX1__)
					std::cout << "VideoRecorder::VideoRecorder - FFMPEG, VX1 is defined." << std::endl;
				#elif defined(__FFMPEG_X2__)
					std::cout << "VideoRecorder::VideoRecorder - FFMPEG, VX2 is defined." << std::endl;
				#else
					std::cout << "VideoRecorder::VideoRecorder - FFMPEG, VX is undefined." << std::endl;
				#endif
			#endif
		}
	}

	FFMPEGContext::~FFMPEGContext(void)
	{ }

	std::string FFMPEGContext::getPixFormatName(const PixelFormat& pixFmt)
	{
		int k=0;
		do
		{
			if(formatsList[k].format==pixFmt)
				return formatsList[k].name;
			k++;
		}
		while(formatsList[k].format!=AV_PIX_FMT_NONE);
		// Else :
		throw Glip::Exception("FFMPEGContext::getSafePixFormatName - Unknown pixel format : " + Glip::toString(pixFmt) + ".", __FILE__, __LINE__);
	}

	std::string FFMPEGContext::getSafePixFormatName(const PixelFormat& pixFmt)
	{
		int k=0;
		do
		{
			if(formatsList[k].format==pixFmt)
				return formatsList[k].name;
			k++;
		}
		while(formatsList[k].format!=AV_PIX_FMT_NONE);
		return "(Unknown FFMPEG pixel format)";
	}

	PixelFormat FFMPEGContext::getPixFormat(const std::string& name)
	{
		int k=0;
		while(formatsList[k].format!=AV_PIX_FMT_NONE)
		{
			if(formatsList[k].name==name)
				return formatsList[k].format;
			k++;
		}
		return AV_PIX_FMT_NONE;
	}
}

