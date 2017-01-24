/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : VideoStream.cpp                                                                           */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Read video stream with ffmpeg, output to GL texture.                                      */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "VideoStream.hpp"
#include <cstring>

namespace FFMPEGInterface
{
	VideoStream::VideoStream(const std::string& filename, unsigned int numFrameBuffered, GLenum minFilter, GLenum magFilter, GLenum sWrapping, GLenum tWrapping, int maxLevel)
	 : 	formatContext(NULL),
		codecContext(NULL),
		codec(NULL),
		frameDecoded(NULL),
		frameRGB(NULL),
		buffer(NULL),
		idVideoStream(0),
		bufferSizeBytes(0),
		SWSContext(NULL),
		idCurrentBufferForWritting(0),
		pboWriter(NULL),
		endReached(false),
		frameRate(1.0f),
		duration_sec(0.0f),
		readFrameCount(0),
		timeStampOffset(0),
		timeStampOfLastFrameRead(0),
		lastTimeStampDelta(0)
	{
		int retCode = 0;
		// Open stream :
		retCode = avformat_open_input(&formatContext, filename.c_str(), NULL, NULL);
		if(retCode!=0)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at av_open_input_file) : " + filename + ".", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Find stream information :
		retCode = avformat_find_stream_info(formatContext, NULL);
		if(retCode<0)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at av_find_stream_info) : " + filename + ".", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Walk through formatContext->nb_streams to find a/the first video stream :
		for(idVideoStream=0; idVideoStream<formatContext->nb_streams; idVideoStream++)
			if(formatContext->streams[idVideoStream]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
				break;
		if(idVideoStream>=formatContext->nb_streams)
			throw Exception("VideoStream::VideoStream - Failed to find video stream (at streams[idVideoStream]->codec->codec_type==CODEC_TYPE_VIDEO).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Get a pointer to the codec context for the video stream :
		codecContext = formatContext->streams[idVideoStream]->codec;
		// Find the decoder for the video stream :
		codec = avcodec_find_decoder(codecContext->codec_id);
		if(codec==NULL)
			throw Exception("VideoStream::VideoStream - No suitable codec found (at avcodec_find_decoder).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Open codec :
		retCode = avcodec_open2(codecContext, codec, NULL);
		if(retCode<0)
			throw Exception("VideoStream::VideoStream - Could not open codec (at avcodec_open).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Get the framerate :
		frameRate = static_cast<float>(formatContext->streams[idVideoStream]->time_base.den)/static_cast<float>(formatContext->streams[idVideoStream]->time_base.num);
		// Get the duration :
		duration_sec = formatContext->duration / AV_TIME_BASE;
		#ifdef __FFMPEG_VERBOSE__
			std::cout << "VideoStream::VideoStream" << std::endl;
			std::cout << "    Filename   : " << filename << std::endl;
			std::cout << "    Frame rate : " << frameRate << " frames per second (for time stamps)" << std::endl;
			std::cout << "    Duration   : " << duration_sec << " second(s)" << std::endl;
		#endif
		// Allocate video frame :
		frameDecoded = av_frame_alloc();
		// Allocate an AVFrame structure :
		frameRGB = av_frame_alloc();
		if(frameRGB==NULL)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at avcodec_alloc_frame).", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Determine required buffer size and allocate buffer :
		bufferSizeBytes = avpicture_get_size(PIX_FMT_RGB24, codecContext->width, codecContext->height);
		buffer = (uint8_t *)av_malloc(bufferSizeBytes*sizeof(uint8_t));
		#ifdef __FFMPEG_VERBOSE__
			std::cout << "VideoStream::VideoStream - Frame size : " << codecContext->width << "x" << codecContext->height << std::endl;
		#endif
		if(buffer==NULL)
			throw Exception("VideoStream::VideoStream - Unable to allocate video frame buffer.", __FILE__, __LINE__, Glip::Exception::ClientException);

		// Assign appropriate parts of buffer to image planes in frameRGB (Note that frameRGB is an AVFrame, but AVFrame is a superset of AVPicture) :
		avpicture_fill( (AVPicture *)frameRGB, buffer, PIX_FMT_RGB24, codecContext->width, codecContext->height);
		// Initialize libswscale :
		SWSContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, codecContext->width, codecContext->height, PIX_FMT_RGB24, SWS_POINT, NULL, NULL, NULL);
		// Create format :
		HdlTextureFormat frameFormat(codecContext->width, codecContext->height, GL_RGB, GL_UNSIGNED_BYTE, minFilter, magFilter, sWrapping, tWrapping, 0, maxLevel);

		// Create the texture :
		textureLinks.assign(numFrameBuffered, NULL);
		for(unsigned int i=0; i<numFrameBuffered; i++)
		{
			textureBuffers.push_back(new HdlTexture(frameFormat));
			// We must write once before using PBO::copyToTexture on it.
			// We are also doing this to prevent reading from an empty (not-yet-allocated) texture.
			textureBuffers.back()->fill(0);
		}
		// Create PBO for uplodaing data to GPU :
		pboWriter = new HdlPBO(frameFormat, GL_PIXEL_UNPACK_BUFFER_ARB,GL_STREAM_DRAW_ARB);

		// Finish by forcing read of first frame :
		readNextFrame();
	}

	VideoStream::~VideoStream(void)
	{
		pboWriter->unmap();
		delete pboWriter;
		pboWriter = NULL;
		for(std::vector<HdlTexture*>::iterator it=textureBuffers.begin(); it!=textureBuffers.end(); it++)
			delete *it;
		textureBuffers.clear();
		textureLinks.clear();
		sws_freeContext(SWSContext);
		// Free the RGB image
		av_free(buffer);
		av_frame_free(&frameRGB);
		// Free the YUV frame
		av_frame_free(&frameDecoded);
		// Close the codec
		avcodec_close(codecContext);
		// Close the video file
		avformat_close_input(&formatContext);
	}	

	const int64_t& VideoStream::getReadFrameCount(void) const
	{
		return readFrameCount;
	}

	const float& VideoStream::getDurationSec(void) const
	{
		return duration_sec;
	}

	const float& VideoStream::getFrameRate(void) const
	{
		return frameRate;
	}

	float VideoStream::getCurrentTimeSec(void) const
	{
		return static_cast<float>(timeStampOfLastFrameRead-timeStampOffset) / frameRate;
	}

	float VideoStream::getEstimatedNextFrameTimeSec(void) const
	{
		return getCurrentTimeSec() + static_cast<float>(lastTimeStampDelta)/frameRate;
	}

	bool VideoStream::isOver(void) const
	{
		return endReached;
	}

	void VideoStream::readNextFrame(void)
	{
		bool		foundNewFrame = false;
		int 		frameFinished = 0,
				retCode = 0;
		AVPacket	packet;
		if(endReached)
			return ;
		while(!foundNewFrame)
		{
			retCode = av_read_frame(formatContext, &packet);
			if(retCode==0)
			{
				// Is this a packet from the video stream?
				if(static_cast<unsigned int>(packet.stream_index)==idVideoStream)
				{
					// Decode video frame
					avcodec_decode_video2(codecContext, frameDecoded, &frameFinished, &packet);

					// Did we get a video frame?
					if(frameFinished>0)
					{
						// Convert the image from its native format to RGB
						sws_scale(SWSContext, frameDecoded->data, frameDecoded->linesize, 0, codecContext->height, frameRGB->data, frameRGB->linesize);
						unsigned char* ptr = reinterpret_cast<unsigned char*>(pboWriter->map());
						memcpy(ptr, frameRGB->data[0], textureBuffers.front()->getNumPixels()*3);
						pboWriter->unmap();
						pboWriter->writeTexture(*textureBuffers[idCurrentBufferForWritting]);

						// Change links :
						for(unsigned int k=0; k<textureBuffers.size(); k++)
							textureLinks[k] = textureBuffers[(idCurrentBufferForWritting+k)%textureBuffers.size()];

						// Update count :
						idCurrentBufferForWritting++;
						idCurrentBufferForWritting = idCurrentBufferForWritting % textureBuffers.size();
						foundNewFrame = true;
						// Add frame :
						readFrameCount++;
						// Save DTS (which is not exactly PTS) :
						if(packet.pts!=static_cast<int>(AV_NOPTS_VALUE))
						{
							if(timeStampOfLastFrameRead==0)
								timeStampOffset = packet.pts;
							lastTimeStampDelta = packet.pts - timeStampOfLastFrameRead;
							timeStampOfLastFrameRead = packet.pts;
						}
						else
							timeStampOfLastFrameRead = 0;
					}
				}
				// Free the packet that was allocated by av_read_frame
				av_free_packet(&packet);
			}
			else
			{
				// Error or EOF
				endReached = true;
				return ;
			}
		}
	}

	void VideoStream::seek(float time_sec)
	{
		int retCode = 0;
		int flags = 0;
		endReached = false;
		int64_t timestamp = static_cast<int64_t>(time_sec)*frameRate;
		retCode = av_seek_frame(formatContext, idVideoStream, timestamp, flags);
		if(retCode<0)
			throw Exception("VideoStream::seek - Seek operation failed (at av_seek_frame).", __FILE__, __LINE__, Glip::Exception::ClientException);
		avcodec_flush_buffers(codecContext);
	}

	const HdlTextureFormat VideoStream::format(void) const
	{
		if(!textureLinks.empty())
			return textureLinks.front()->format();
		else
			return HdlTextureFormat(1, 1, GL_RGB, GL_UNSIGNED_BYTE);
	}

	HdlTexture& VideoStream::frame(const int& id)
	{
		return *textureLinks[id];
	}
}

