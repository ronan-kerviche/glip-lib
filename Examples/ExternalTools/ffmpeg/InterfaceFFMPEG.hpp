/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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

	#include <string>

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

	class InterfaceFFMPEG
	{
		private :
			static bool initOnce;

		public :
			InterfaceFFMPEG(void);
			~InterfaceFFMPEG(void);

			// Tools :
			static std::string getPixFormatName(PixelFormat pixFmt);
	};

#endif
