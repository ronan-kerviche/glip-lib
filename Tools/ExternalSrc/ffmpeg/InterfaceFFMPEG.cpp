/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : InterfqceFFMPEG.cpp                                                                       */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Interface to FFMPEG library                                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "InterfaceFFMPEG.hpp"

	bool InterfaceFFMPEG::initOnce = true;

	InterfaceFFMPEG::InterfaceFFMPEG(void)
	{
		if(initOnce)
		{
			av_register_all();

			initOnce = false;
		}
	}

	InterfaceFFMPEG::~InterfaceFFMPEG(void)
	{ }

	std::string InterfaceFFMPEG::getPixFormatName(PixelFormat pixFmt)
	{
		// A list of formats can be found at : http://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5a1aa7677092740d8def31655b5d7f0cc2
		switch(pixFmt)
		{
			case PIX_FMT_YUV410P :	return "PIX_FMT_YUV410P";
			case PIX_FMT_YUV411P :	return "PIX_FMT_YUV411P";
			case PIX_FMT_YUV420P :	return "PIX_FMT_YUV420P";
			case PIX_FMT_YUV422P :	return "PIX_FMT_YUV422P";
			case PIX_FMT_YUV440P :	return "PIX_FMT_YUV440P";
			case PIX_FMT_YUV444P :	return "PIX_FMT_YUV444P";
			case PIX_FMT_RGB32 : 	return "PIX_FMT_RGB32";
			case PIX_FMT_BGR32 : 	return "PIX_FMT_BGR32";
			default :		return "(Unknown pixel format)";
		}
	}
