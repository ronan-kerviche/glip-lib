/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : InterfqceFFMPEG.hpp                                                                       */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Interface to FFMPEG library                                                               */
/*                     Compile with the following libraries :                                                    */
/*                      -lavutil -lavformat -lavcodec -lavutil -lswscale                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIP_INTERFACE_FFMPEG__
#define __GLIP_INTERFACE_FFMPEG__

	#include <utility>
	#include <string>
	#include <vector>

	// FFMPEG version (VX1/VX2) :
	#define __FFMPEG_VX1__
	
	// Include FFMPEG (which is a pure C project, protect with : )
	extern "C"
	{
		// Fix lacking definition in common.h
		#ifndef INT64_C
			#define INT64_C(c) (c ## LL)
			#define UINT64_C(c) (c ## ULL)
		#endif

		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
		#include <libswscale/swscale.h>
		#include <libavutil/mathematics.h>
	}

	// Display messages on std::cout :
	#define __FFMPEG_VERBOSE__

namespace FFMPEGInterface
{
	class FFMPEGContext
	{
		public :
			struct Format
			{
				const PixelFormat format;
				const std::string name;
			};
			static const Format formatsList[];

		private :
			static bool initOnce;

		public :
			FFMPEGContext(void);
			~FFMPEGContext(void);

			// Tools :
			static std::string getPixFormatName(const PixelFormat& pixFmt);
			static std::string getSafePixFormatName(const PixelFormat& pixFmt);
			static PixelFormat getPixFormat(const std::string& name);
	};
}

#endif

