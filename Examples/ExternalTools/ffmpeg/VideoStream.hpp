/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : VideoStream.hpp                                                                           */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Read video stream with ffmpeg, output to GL texture.                                      */
/*                     Follows information at http://dranger.com/ffmpeg/tutorial01.html                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIP_VIDEO_STREAM__
#define __GLIP_VIDEO_STREAM__

	// Display messages on std::cout :
	//#define __VIDEO_STREAM_VERBOSE__

	// Use PBOs for uploading data to the GPU :
	#define __USE_PBO__

	#include "GLIPLib.hpp"
	#include "InterfaceFFMPEG.hpp"

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class VideoStream : public InterfaceFFMPEG
	{
		private :

			// libav*/ffmpeg data :
			AVFormatContext 		*pFormatCtx;
			AVCodecContext 			*pCodecCtx;
			AVCodec 			*pCodec;
			AVFrame 			*pFrame,
							*pFrameRGB;
			uint8_t 			*buffer;
			unsigned int			idVideoStream,
							bufferSizeBytes;
			SwsContext 			*pSWSCtx;

			// gliplib data :
			unsigned int			idCurrentBufferForWritting;
			std::vector<HdlTexture*>	textureBuffers;

			#ifdef __USE_PBO__
				HdlPBO 			*pbo;
			#endif

			// Misc :
			bool				endReached;
			float 				timeStampFrameRate,
							duration_sec;
			int64_t 			readFrameCount,
							timeStampOffset,
							timeStampOfLastFrameRead;

		public :
			VideoStream(const std::string& filename, unsigned int numFrameBuffered = 1, GLenum minFilter=GL_NEAREST, GLenum magFilter=GL_NEAREST, GLenum sWrapping=GL_CLAMP, GLenum tWrapping=GL_CLAMP, int maxLevel=0);
			~VideoStream(void);

			int		getNumBuffers(void) const;
			int  		getReadFrameCount(void) const;
			float		getVideoDurationSec(void) const;
			float  		getCurrentTimeSec(void) const;
			bool		isOver(void) const;

			void 		readNextFrame(void);
			HdlTexture& 	texture(unsigned int id=0); // newest is 0, oldest is getNumBuffers()-1
			void		seek(float time_sec);
	};

#endif
