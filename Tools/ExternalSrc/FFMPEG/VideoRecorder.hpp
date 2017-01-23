/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : VideoRecorder.hpp                                                                         */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Input from GL texture, Write video stream with ffmpeg, .                                  */
/*                     Follows information at http://ffmpeg.org/doxygen/trunk/muxing_8c_source.html              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIP_VIDEO_RECORDER__
#define __GLIP_VIDEO_RECORDER__

	#include "GLIPLib.hpp"
	#include "FFMPEGInterface.hpp"

namespace FFMPEGInterface
{
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class VideoRecorder : public HdlAbstractTextureFormat, public FFMPEGContext
	{
		private :
			int				numEncodedFrame,
							frameRate;
			// libav/ffmpeg data :
			AVOutputFormat 			*outputFormat;	// Proxy
			AVFormatContext 		*formatContext;
			AVCodec				*videoCodec;
			AVCodecContext 			*codecContext;	// Proxy
			AVStream 			*videoStream;
			AVFrame 			*frame;
			AVPicture 			srcPicture,
							dstPicture,
							*buffer;	// Proxy
			SwsContext			*swsContext;
			#ifdef __FFMPEG_VX1__
				uint8_t 		*video_outbuf;
				int 			video_outbuf_size;
			#endif
			// From GLIP lib :
			HdlPBO				pboReader;

			VideoRecorder(const VideoRecorder&);
		public :
			VideoRecorder(const std::string& filename, const HdlAbstractTextureFormat& format, const int& _frameRate, const int& videoBitRate_BitPerSec=400000, const PixelFormat& pixFormat=PIX_FMT_YUV420P);
			~VideoRecorder(void);
			unsigned int getNumEncodedFrames(void) const;
			float getTotalVideoDurationSec(void) const;
			void record(HdlTexture& newFrame);
	};
}

#endif
