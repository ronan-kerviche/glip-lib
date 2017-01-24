/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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

	#include "GLIPLib.hpp"
	#include "FFMPEGInterface.hpp"

namespace FFMPEGInterface
{
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class VideoStream : public FFMPEGContext
	{
		private :

			// libav*/ffmpeg data :
			AVFormatContext 		*formatContext;
			AVCodecContext 			*codecContext;
			AVCodec 			*codec;
			AVFrame 			*frameDecoded,
							*frameRGB;
			uint8_t 			*buffer;
			unsigned int			idVideoStream,
							bufferSizeBytes;
			SwsContext 			*SWSContext;

			// glip-lib data :
			unsigned int			idCurrentBufferForWritting;
			std::vector<HdlTexture*>	textureBuffers,
							textureLinks;
			HdlPBO				*pboWriter;

			// Misc :
			bool				endReached;
			float 				frameRate,
							duration_sec;
			int64_t 			readFrameCount,
							timeStampOffset,
							timeStampOfLastFrameRead,
							lastTimeStampDelta;

			VideoStream(const VideoStream&);
		public :
			VideoStream(const std::string& filename, unsigned int numFrameBuffered = 1, GLenum minFilter=GL_NEAREST, GLenum magFilter=GL_NEAREST, GLenum sWrapping=GL_CLAMP, GLenum tWrapping=GL_CLAMP, int maxLevel=0);
			~VideoStream(void);

			const int64_t& getReadFrameCount(void) const;
			const float& getDurationSec(void) const;
			const float& getFrameRate(void) const;
			float getCurrentTimeSec(void) const;
			float getEstimatedNextFrameTimeSec(void) const;
			bool isOver(void) const;
			void readNextFrame(void);
			void seek(float time_sec);
			const HdlTextureFormat format(void) const;
			HdlTexture& frame(const int& id);
	};
}

#endif
