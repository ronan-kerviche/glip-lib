/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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

	// Display messages on std::cout :
	//#define __VIDEO_RECORDER_VERBOSE__

	// Use PBOs for uploading data to the GPU :
	#define __USE_PBO__

	#include "GLIPLib.hpp"
	#include "InterfaceFFMPEG.hpp"

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class VideoRecorder : public InterfaceFFMPEG, public OutputDevice
	{
		private :

			int				numEncodedFrame,
							frameRate;

			// libav*/ffmpeg data :
			AVOutputFormat 			*fmt;		// Proxy (?)
			AVFormatContext 		*oc;
			AVCodec				*video_codec;
			AVCodecContext 			*c;		// Proxy
			AVStream 			*video_stream;
			AVFrame 			*frame;
			AVPicture 			src_picture,
							dst_picture,
							*buffer;	// Proxy
			SwsContext			*swsContext;

			#ifdef __FFMPEG_VX1__
				uint8_t 		*video_outbuf;
				int 			video_outbuf_size;
			#endif

			// From GLIP lib :
			#ifndef __USE_PBO__
				TextureReader		textureReader;
			#else
				PBOTextureReader	textureReader;
			#endif

			 OutputDevice::OutputDeviceLayout declareLayout(void);
		
			// Inherited from OutputDevice :
			void process(void);

		public :
			VideoRecorder(const std::string& filename, const __ReadOnly_HdlTextureFormat &format, int _frameRate, int videoBitRate_BitPerSec=400000, PixelFormat pixFormat=PIX_FMT_YUV420P);
			~VideoRecorder(void);

			const __ReadOnly_HdlTextureFormat& format(void);
			unsigned int getNumEncodedFrames(void) const;
			float getTotalVideoDurationSec(void) const;
	};

#endif
