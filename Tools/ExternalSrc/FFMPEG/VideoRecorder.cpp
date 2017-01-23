/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : VideoRecorder.cpp                                                                         */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Input from GL texture, Write video stream with ffmpeg, .                                  */
/*                     Follows information at http://ffmpeg.org/doxygen/trunk/muxing_8c_source.html              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "VideoRecorder.hpp"

namespace FFMPEGInterface
{
	// Find options for argument pixFormat at http://ffmpeg.org/doxygen/trunk/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5a1aa7677092740d8def31655b5d7f0cc2
	VideoRecorder::VideoRecorder(const std::string& filename, const HdlAbstractTextureFormat& format, const int& _frameRate, const int& videoBitRate_BitPerSec, const PixelFormat& pixFormat)
	 : 	HdlAbstractTextureFormat(format),
		numEncodedFrame(0),
		frameRate(_frameRate),
		outputFormat(NULL),
		formatContext(NULL),
		videoCodec(NULL),
		codecContext(NULL),
		videoStream(NULL),
		frame(NULL),
		buffer(NULL),
		swsContext(NULL),
		pboReader(format, GL_PIXEL_PACK_BUFFER_ARB, GL_STREAM_READ_ARB)
	{
		int retCode = 0;
		if(format.getWidth()%2!=0 || format.getHeight()%2!=0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Stream width and height must be a multiple of 2).", __FILE__, __LINE__, Glip::Exception::ClientException);
		else if(format.getGLMode()!=GL_RGB || format.getGLDepth()!=GL_UNSIGNED_BYTE)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Texture format must be GL_RGB + GL_UNSIGNED_BYTE (24 bits RGB)).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// allocate the output media context
		#if defined(__FFMPEG_VX1__)
			// Auto detect the output format from the name, default is MPEG :
			outputFormat = av_guess_format(NULL, filename.c_str(), NULL);
			if(!outputFormat)
			{
				std::cout << "VideoRecorder::VideoRecorder - Could not deduce output format from file extension: using MPEG." << std::endl;
				outputFormat = av_guess_format("mpeg", NULL, NULL);
			}
			if(!outputFormat)
				throw Exception("VideoRecorder::VideoRecorder - Could not find suitable output format", __FILE__, __LINE__, Glip::Exception::ClientException);

			formatContext = avformat_alloc_context();
			formatContext->oformat = outputFormat;
		#elif defined(__FFMPEG_VX2__)
			avformat_alloc_output_context(&formatContext, NULL, NULL, filename.c_str());
			if(!formatContext)
			{
				#ifdef __VIDEO_FFMPEG_VERBOSE__
					std::cout << "VideoRecorder::VideoRecorder - Could not deduce output format from file extension: using MPEG." << std::endl;
				#endif
				avformat_alloc_output_context2(&formatContext, NULL, "mpeg", filename.c_str());
			}
		#endif
		if(!formatContext)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (at avformat_alloc_output_context).", __FILE__, __LINE__, Glip::Exception::ClientException);

		outputFormat = formatContext->oformat;
		if(outputFormat->video_codec==AV_CODEC_ID_NONE)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (no video codec selected).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Add an output stream :
		// Find the encoder :
		videoCodec 	= avcodec_find_encoder(outputFormat->video_codec);
		if(!videoCodec)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (No suitable codec found).", __FILE__, __LINE__, Glip::Exception::ClientException);

		videoStream 	= avformat_new_stream(formatContext, videoCodec);
		if (!videoStream)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate stream).", __FILE__, __LINE__, Glip::Exception::ClientException);

		videoStream->id= formatContext->nb_streams-1;
		codecContext = videoStream->codec;
		if(videoCodec->type!=AVMEDIA_TYPE_VIDEO)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Codec is not of video type).", __FILE__, __LINE__, Glip::Exception::ClientException);

		avcodec_get_context_defaults3(codecContext, videoCodec);
		codecContext->codec_id = outputFormat->video_codec;
		codecContext->bit_rate = videoBitRate_BitPerSec;
		// Resolution must be a multiple of two :
		codecContext->width = format.getWidth();
		codecContext->height = format.getHeight();
		// timebase: 	This is the fundamental unit of time (in seconds) in terms
		// 		of which frame timestamps are represented. For fixed-fps content,
		// 		timebase should be 1/framerate and timestamp increments should be
		// 		identical to 1.
		codecContext->time_base.den = frameRate;
		codecContext->time_base.num = 1;
		codecContext->gop_size = 12; // emit one intra frame every twelve frames at most.
		codecContext->pix_fmt = pixFormat; //or PIX_FMT_YUV420P;

		// Just for testing, we also add B frames :
		if (codecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
			codecContext->max_b_frames = 2;

		// Needed to avoid using macroblocks in which some coeffs overflow.
		// This does not happen with normal video, it just happens here as
		// the motion of the chroma plane does not match the luma plane.
		if(codecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO)
			codecContext->mb_decision = 2;

		// Some formats want stream headers to be separate :
		if (formatContext->oformat->flags & AVFMT_GLOBALHEADER)
			codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
		// Now that all the parameters are set, we can open the audio and
		// video codecs and allocate the necessary encode buffers.

		// Open the codec :
		retCode = avcodec_open2(codecContext, videoCodec, NULL);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not open video codec).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Allocate and init a re-usable frame :
		frame = av_frame_alloc();
		if (!frame)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate video frame).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Allocate the encoded raw picture :
		retCode = avpicture_alloc(&dstPicture, codecContext->pix_fmt, codecContext->width, codecContext->height);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate picture).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// If the output format is not AV_PIX_FMT_RGB24, then a temporary AV_PIX_FMT_RGB24
		// picture is needed too. It is then converted to the required
		// output format.
		if(codecContext->pix_fmt!=PIX_FMT_RGB24)
		{
			retCode = avpicture_alloc(&srcPicture, PIX_FMT_RGB24, codecContext->width, codecContext->height);
			if(retCode<0)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not allocate temporary picture).", __FILE__, __LINE__, Glip::Exception::ClientException);
			buffer = &srcPicture;
		}
		else
			buffer = &dstPicture;

		// copy data and linesize picture pointers to frame :
		*((AVPicture *)frame) = dstPicture;
		av_dump_format(formatContext, 0, filename.c_str(), 1);

		 // open the output file, if needed :
		if(!(outputFormat->flags & AVFMT_NOFILE))
		{
			retCode = avio_open(&formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE);
			if(retCode<0)
				throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Could not open file " + filename + ").", __FILE__, __LINE__, Glip::Exception::ClientException);
		}

		// Write the stream header, if any :
		retCode = avformat_write_header(formatContext, NULL);
		if(retCode<0)
			throw Exception("VideoRecorder::VideoRecorder - Failed to start recorder (Error occurred when opening output file).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Set first PTS to zero :
		frame->pts = 0;

		// Allocate scaling/conversion context :
		if(codecContext->pix_fmt!=PIX_FMT_RGB24)
		{
			swsContext = sws_getContext( format.getWidth(), format.getHeight(), PIX_FMT_RGB24, format.getWidth(), format.getHeight(), codecContext->pix_fmt, SWS_POINT, NULL, NULL, NULL);
		}

		#if defined(__FFMPEG_VX1__)
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
		av_write_trailer(formatContext);

		// Close each codec :
		if(videoStream)
		{
			avcodec_close(videoStream->codec); // For AVStream *videoStream;
			av_free(srcPicture.data[0]);
			av_free(dstPicture.data[0]);
			av_frame_free(&frame);
		}

		// Free the streams :
		for(unsigned int i = 0; i<formatContext->nb_streams; i++)
		{
			av_freep(&formatContext->streams[i]->codec);
			av_freep(&formatContext->streams[i]);
		}

		// Close the output file :
		if (!(outputFormat->flags & AVFMT_NOFILE))
			avio_close(formatContext->pb);

		// free the stream :
		av_free(formatContext);
		sws_freeContext(swsContext);
		#ifdef __FFMPEG_VX1__
			av_free(video_outbuf);

		#endif
		// Remain :
		//AVCodec *videoCodec; // Not destroyed in the example
	}

	void VideoRecorder::record(HdlTexture& newFrame)
	{
		int 		retCode = 0;

		// Read texture and copy to the buffer :
		pboReader.readTexture(newFrame);
		unsigned char* ptr = reinterpret_cast<unsigned char*>(pboReader.map());
		// Without Y-flip :
		// memcpy(buffer->data[0], ptr, getSize());
		// With Y-flip :
		const int height = getHeight();
		const size_t rowSize = getRowSize();
		for(int y=0; y<height; y++)
			memcpy(buffer->data[0]+y*rowSize, ptr+(height-y-1)*rowSize, rowSize);
		pboReader.unmap();

		// Convert from RGB24 if needed :
		if(swsContext!=NULL)
			sws_scale(swsContext, buffer->data, buffer->linesize, 0, codecContext->height, dstPicture.data, dstPicture.linesize);

		// Push data to encoding queue :
		if(formatContext->oformat->flags & AVFMT_RAWPICTURE)
		{
			// Raw video case - directly store the picture in the packet :
			AVPacket pkt;
			av_init_packet(&pkt);

			pkt.flags |= AV_PKT_FLAG_KEY;
			pkt.stream_index = videoStream->index;
			pkt.data = dstPicture.data[0];
			pkt.size = sizeof(AVPicture);
			retCode = av_interleaved_write_frame(formatContext, &pkt);
			std::cout << "Writing frame." << std::endl;
		}
		else
		{
			// Encode the image :
			AVPacket pkt;
			int got_output = 0;
			av_init_packet(&pkt);
			pkt.data = NULL; // packet data will be allocated by the encoder.
			pkt.size = 0;

			//MODIFICATION 15/02/13 : retCode = avcodec_encode_video2(c, &pkt, frame, &got_output);
			//retCode = avcodec_encode_video(c, &pkt, frame, &got_output);
			retCode = avcodec_encode_video2(codecContext, &pkt, frame, &got_output);
			if(retCode<0)
				throw Exception("VideoRecorder::VideoRecorder - Error encoding video frame.", __FILE__, __LINE__, Glip::Exception::ClientException);

			// If size is zero, it means the image was buffered :
			if(got_output)
			{
				if (codecContext->coded_frame->key_frame)
					pkt.flags |= AV_PKT_FLAG_KEY;
				pkt.stream_index = videoStream->index;
				// Write the compressed frame to the media file :
				retCode = av_interleaved_write_frame(formatContext, &pkt);
			}
			else
				retCode = 0;
		}

		if(retCode!= 0)
			throw Exception("VideoRecorder::VideoRecorder - Error while writing video frame.", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Update next time stamp :
		frame->pts += av_rescale_q(1, videoStream->codec->time_base, videoStream->time_base);
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
}

