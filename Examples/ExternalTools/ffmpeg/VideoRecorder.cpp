/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : VideoRecorder.cpp                                                                         */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Input from GL texture, Write video stream with ffmpeg, .                                  */
/*                     Follows information at http://ffmpeg.org/doxygen/trunk/muxing_8c_source.html              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "VideoRecorder.hpp"

	// Find options for argument pixFormat at http://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5a1aa7677092740d8def31655b5d7f0cc2
	VideoRecorder::VideoRecorder(const std::string& filename, const __ReadOnly_HdlTextureFormat &format, int _frameRate, int videoBitRate_BitPerSec, PixelFormat pixFormat)
	 : __ReadOnly_ComponentLayout(declareLayout()), OutputDevice(declareLayout(), "VideoRecorder"), numEncodedFrame(0), frameRate(_frameRate),
	   oc(NULL), fmt(NULL), video_codec(NULL), video_stream(NULL), frame(NULL), buffer(NULL), swsContext(NULL), c(NULL),
	   #ifndef __USE_PBO__
		textureReader("VideoRecorder_TextureReader", format)
	   #else
		textureReader("VideoRecorder_PBOTextureReader", format, GL_STREAM_READ_ARB)
	   #endif
	{
		int retCode = 0;

		if(format.getWidth()%2!=0 || format.getHeight()%2!=0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Stream width and height must be a multiple of 2).", __FILE__, __LINE__);

		if(format.getGLMode()!=GL_RGB || format.getGLDepth()!=GL_UNSIGNED_BYTE)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Texture format must be GL_RGB + GL_UNSIGNED_BYTE (24 bits RGB)).", __FILE__, __LINE__);

		// allocate the output media context
		#ifdef __FFMPEG_VX1__
			// Find the format :
			AVOutputFormat 	*fmtTmp;

			// auto detect the output format from the name. default is mpeg.
			fmt = av_guess_format(NULL, filename.c_str(), NULL);
			if (!fmt)
			{
				std::cout << "VideoRecorder::VideoRecorder - Could not deduce output format from file extension: using MPEG." << std::endl;
				fmt = av_guess_format("mpeg", NULL, NULL);
			}

			if(!fmt)
				throw Exception("VideoRecorder::VideoRecorder - Could not find suitable output format", __FILE__, __LINE__);

			oc = avformat_alloc_context();

			oc->oformat = fmt;
		#elif __FFMPEG_VX2__
			//MODIFICATION 15/02/13 : avformat_alloc_output_context2(&oc, NULL, NULL, filename.c_str());
			avformat_alloc_output_context(&oc, NULL, NULL, filename.c_str());

			if(!oc)
			{
				#ifdef __VIDEO_RECORDER_VERBOSE__
					std::cout << "VideoRecorder::VideoRecorder - Could not deduce output format from file extension: using MPEG." << std::endl;
				#endif

				avformat_alloc_output_context2(&oc, NULL, "mpeg", filename.c_str());
			}
		#endif

		if(!oc)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (at avformat_alloc_output_context).", __FILE__, __LINE__);

		fmt 		= oc->oformat;

		#ifdef __FFMPEG_VX1__
			if(fmt->video_codec==CODEC_ID_NONE)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (no video codec selected).", __FILE__, __LINE__);
		#elif __FFMPEG_VX2__
			if(fmt->video_codec==AV_CODEC_ID_NONE)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (no video codec selected).", __FILE__, __LINE__);
		#endif

		// Add an output stream :
		// Find the encoder :
		video_codec 	= avcodec_find_encoder(fmt->video_codec);

		if(!video_codec)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (No suitable codec found).", __FILE__, __LINE__);


		video_stream 	= avformat_new_stream(oc, video_codec);

		if (!video_stream)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate stream).", __FILE__, __LINE__);

		video_stream->id= oc->nb_streams-1;
		c 		= video_stream->codec;

		if(video_codec->type!=AVMEDIA_TYPE_VIDEO)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Codec is not of video type).", __FILE__, __LINE__);

		avcodec_get_context_defaults3(c, video_codec);
		c->codec_id 	= fmt->video_codec;

		c->bit_rate 	= videoBitRate_BitPerSec;

		// Resolution must be a multiple of two :
		c->width 	= format.getWidth();
		c->height 	= format.getHeight();

		// timebase: 	This is the fundamental unit of time (in seconds) in terms
		// 		of which frame timestamps are represented. For fixed-fps content,
		// 		timebase should be 1/framerate and timestamp increments should be
		// 		identical to 1.
		c->time_base.den= frameRate;
		c->time_base.num= 1;
		c->gop_size 	= 12; // emit one intra frame every twelve frames at most.
		c->pix_fmt 	= pixFormat; //or PIX_FMT_YUV420P;

		// Just for testing, we also add B frames :
		#ifdef __FFMPEG_VX1__
			if (c->codec_id == CODEC_ID_MPEG2VIDEO)
				c->max_b_frames = 2;
		#elif __FFMPEG_VX2__
			if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
				c->max_b_frames = 2;
		#endif

		// Needed to avoid using macroblocks in which some coeffs overflow.
			// This does not happen with normal video, it just happens here as
			// the motion of the chroma plane does not match the luma plane.
		#ifdef __FFMPEG_VX1__
			if (c->codec_id == CODEC_ID_MPEG1VIDEO)
				c->mb_decision = 2;
		#elif __FFMPEG_VX2__
			if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
				c->mb_decision = 2;
		#endif

		// Some formats want stream headers to be separate :
		if (oc->oformat->flags & AVFMT_GLOBALHEADER)
			c->flags |= CODEC_FLAG_GLOBAL_HEADER;

		// Now that all the parameters are set, we can open the audio and
		// video codecs and allocate the necessary encode buffers.

		// Open the codec :
		retCode = avcodec_open2(c, video_codec, NULL);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not open video codec).", __FILE__, __LINE__);

		// Allocate and init a re-usable frame :
		frame = avcodec_alloc_frame();
		if (!frame)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate video frame).", __FILE__, __LINE__);

		// Allocate the encoded raw picture :
		retCode = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate picture).", __FILE__, __LINE__);

		// If the output format is not AV_PIX_FMT_RGB24, then a temporary AV_PIX_FMT_RGB24
		// picture is needed too. It is then converted to the required
		// output format.
		if(c->pix_fmt != PIX_FMT_RGB24)
		{
			retCode = avpicture_alloc(&src_picture, PIX_FMT_RGB24, c->width, c->height);

			if(retCode<0)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate temporary picture).", __FILE__, __LINE__);

			buffer = &src_picture;
		}
		else
			buffer = &dst_picture;

		// copy data and linesize picture pointers to frame :
		*((AVPicture *)frame) = dst_picture;

		av_dump_format(oc, 0, filename.c_str(), 1);

		 // open the output file, if needed :
		if(!(fmt->flags & AVFMT_NOFILE))
		{
			retCode = avio_open(&oc->pb, filename.c_str(), AVIO_FLAG_WRITE);
			if(retCode<0)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not open file " + filename + ").", __FILE__, __LINE__);
		}

		// Write the stream header, if any :
		retCode = avformat_write_header(oc, NULL);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Error occurred when opening output file).", __FILE__, __LINE__);

		// Set first PTS to zero :
		frame->pts = 0;

		// Allocate scaling/conversion context :
		if(c->pix_fmt!=PIX_FMT_RGB24)
		{
			swsContext = sws_getContext( format.getWidth(), format.getHeight(), PIX_FMT_RGB24, format.getWidth(), format.getHeight(), c->pix_fmt, SWS_POINT, NULL, NULL, NULL);
		}

		#ifdef __FFMPEG_VX1__
			video_outbuf_size 	= format.getSize();
			video_outbuf 		= reinterpret_cast<uint8_t*>( av_malloc(video_outbuf_size) );
		#endif
	}

	VideoRecorder::~VideoRecorder(void)
	{
		// Write the trailer, if any. The trailer must be written before you
		// close the CodecContexts open when you wrote the header; otherwise
		// av_write_trailer() may try to use memory that was freed on
		// av_codec_close().
		av_write_trailer(oc);

		// Close each codec :
		if(video_stream)
		{
			avcodec_close(video_stream->codec); // For AVStream *video_stream;
			av_free(src_picture.data[0]);
			av_free(dst_picture.data[0]);
			av_free(frame);
		}

		// Free the streams :
		for(unsigned int i = 0; i<oc->nb_streams; i++)
		{
			av_freep(&oc->streams[i]->codec);
			av_freep(&oc->streams[i]);
		}

		// Close the output file :
		if (!(fmt->flags & AVFMT_NOFILE))
			avio_close(oc->pb);

		// free the stream :
		av_free(oc);

		sws_freeContext(swsContext);

		#ifdef __FFMPEG_VX1__
			av_free(video_outbuf);

		#endif

		// Remain :
		//AVCodec		*video_codec; // Not destroyed in the example
	}

	 OutputDevice::OutputDeviceLayout VideoRecorder::declareLayout(void)
	{
		// Create a layout here, from some arguments...
		OutputDevice::OutputDeviceLayout tmp("Recorder");

		// Add input ports :
		tmp.addInputPort("input");

		return tmp;
	}


	const __ReadOnly_HdlTextureFormat& VideoRecorder::format(void)
	{
		return textureReader;
	}

	void VideoRecorder::process(void)
	{
		int 		retCode = 0;
		HdlTexture& 	texture	= in();

		// Read texture :
		textureReader << texture << OutputDevice::Process;

		// Copy the texture to the buffer :
		#ifndef __USE_PBO__
			for(int i=0; i<textureReader.getHeight(); i++)
			{
				for(int j=0; j<textureReader.getWidth(); j++)
				{
					for(int k=0; k<3; k++)
						*(buffer->data[0] + i*textureReader.getWidth()*3 + j*3 + k) = static_cast<unsigned char>(textureReader(j,i,k)*255);
				}
			}
		#else
			unsigned char* ptr = reinterpret_cast<unsigned char*>(textureReader.startReadingMemory());

			memcpy(buffer->data[0], ptr, textureReader.getNumPixels()*3);

			textureReader.endReadingMemory();
		#endif

		// Convert from RGB24 if needed :
		if(swsContext!=NULL)
			sws_scale(swsContext, buffer->data, buffer->linesize, 0, c->height, dst_picture.data, dst_picture.linesize);

		// Push data to encoding queue :
		if (oc->oformat->flags & AVFMT_RAWPICTURE)
		{
			// Raw video case - directly store the picture in the packet :
			AVPacket pkt;
			av_init_packet(&pkt);

			pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index = video_stream->index;
			pkt.data = dst_picture.data[0];
			pkt.size = sizeof(AVPicture);

			retCode = av_interleaved_write_frame(oc, &pkt);
		}
		else
		{
			#ifdef __FFMPEG_VX1__
				//encode the image
				int out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, frame);

				// if zero size, it means the image was buffered
				if (out_size > 0)
				{
					AVPacket pkt;

					av_init_packet(&pkt);

					if(c->coded_frame->key_frame)
						pkt.flags |= AV_PKT_FLAG_KEY;

					pkt.stream_index = video_stream->index;

					pkt.data = video_outbuf;

					pkt.size = out_size;

					//write the compressed frame in the media file
					retCode = av_interleaved_write_frame(oc, &pkt);
				}
			#elif __FMPEG_VX2__
				// Encode the image :
				AVPacket pkt;
				int got_output;

				av_init_packet(&pkt);
				pkt.data = NULL; // packet data will be allocated by the encoder.
				pkt.size = 0;

				//MODIFICATION 15/02/13 : retCode = avcodec_encode_video2(c, &pkt, frame, &got_output);
				retCode = avcodec_encode_video(c, &pkt, frame, &got_output);
				if(retCode<0)
					throw Exception("VideoRecorder::VideoRecorder - Error encoding video frame.", __FILE__, __LINE__);

				// If size is zero, it means the image was buffered :
				if (got_output)
				{
					if (c->coded_frame->key_frame)
						pkt.flags |= AV_PKT_FLAG_KEY;

					pkt.stream_index = video_stream->index;

					// Write the compressed frame to the media file :
					retCode = av_interleaved_write_frame(oc, &pkt);
				}
				else
					retCode = 0;
			#endif
		}

		if(retCode!= 0)
			throw Exception("VideoRecorder::VideoRecorder - Error while writing video frame.", __FILE__, __LINE__);

		// Update next time stamp :
		frame->pts += av_rescale_q(1, video_stream->codec->time_base, video_stream->time_base);

		numEncodedFrame++;
	}

	unsigned int VideoRecorder::getNumEncodedFrames(void) const
	{
		return numEncodedFrame;
	}

	float VideoRecorder::getTotalVideoDurationSec(void) const
	{
		return static_cast<float>(getNumEncodedFrames())/static_cast<float>(frameRate);
	}
