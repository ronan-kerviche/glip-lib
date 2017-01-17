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

	VideoStream::VideoStream(const std::string& filename, unsigned int numFrameBuffered, GLenum minFilter, GLenum magFilter, GLenum sWrapping, GLenum tWrapping, int maxLevel)
	 : 	idVideoStream(0),
		readFrameCount(0),
		timeStampFrameRate(1.0f),
		timeStampOffset(0),
		timeStampOfLastFrameRead(0),
		endReached(false),
	   	pFormatCtx(NULL),
		pCodecCtx(NULL),
		pCodec(NULL),
		pFrame(NULL),
		pFrameRGB(NULL),
		buffer(NULL),
		pSWSCtx(NULL),
		idCurrentBufferForWritting(0)
	{
		#ifdef __USE_PBO__
			#ifdef __VIDEO_STREAM_VERBOSE__
				std::cout << "VideoStream::VideoStream - Using PBO for uploading data to the GPU." << std::endl;
			#endif
			pbo = NULL;
		#else
			#ifdef __VIDEO_STREAM_VERBOSE__
				std::cout << "VideoStream::VideoStream - Using standard method HdlTexture::write for uploading data to the GPU." << std::endl;
			#endif
		#endif

		int retCode = 0;
		// Open stream :
		retCode = avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL);
		if(retCode!=0)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at av_open_input_file) : " + filename + ".", __FILE__, __LINE__);

		// Find stream information :
		retCode = avformat_find_stream_info(pFormatCtx, NULL);
		if(retCode<0)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at av_find_stream_info) : " + filename + ".", __FILE__, __LINE__);

		// Walk through pFormatCtx->nb_streams to find a/the first video stream :
		for(idVideoStream=0; idVideoStream<pFormatCtx->nb_streams; idVideoStream++)
			if(pFormatCtx->streams[idVideoStream]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
				break;
		if(idVideoStream>=pFormatCtx->nb_streams)
			throw Exception("VideoStream::VideoStream - Failed to find video stream (at streams[idVideoStream]->codec->codec_type==CODEC_TYPE_VIDEO).", __FILE__, __LINE__);

		// Get a pointer to the codec context for the video stream :
		pCodecCtx = pFormatCtx->streams[idVideoStream]->codec;
		// Find the decoder for the video stream :
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec==NULL)
			throw Exception("VideoStream::VideoStream - No suitable codec found (at avcodec_find_decoder).", __FILE__, __LINE__);

		// Open codec :
		retCode = avcodec_open2(pCodecCtx, pCodec, NULL);
		if(retCode<0)
			throw Exception("VideoStream::VideoStream - Could not open codec (at avcodec_open).", __FILE__, __LINE__);

		// Get the framerate :
		timeStampFrameRate = static_cast<float>(pFormatCtx->streams[idVideoStream]->time_base.den)/static_cast<float>(pFormatCtx->streams[idVideoStream]->time_base.num);
		// Get the duration :
		duration_sec = pFormatCtx->duration / AV_TIME_BASE;
		#ifdef __VIDEO_STREAM_VERBOSE__
			std::cout << "VideoStream::VideoStream" << std::endl;
			std::cout << "    Frame rate : " << timeStampFrameRate << " frames per second (for time stamps)" << std::endl;
			std::cout << "    Duration   : " << duration_sec << " second(s)" << std::endl;
		#endif
		// Allocate video frame :
		pFrame = avcodec_alloc_frame();
		// Allocate an AVFrame structure :
		pFrameRGB = avcodec_alloc_frame();
		if(pFrameRGB==NULL)
			throw Exception("VideoStream::VideoStream - Failed to open stream (at avcodec_alloc_frame).", __FILE__, __LINE__);

		// Determine required buffer size and allocate buffer :
		bufferSizeBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
		buffer = (uint8_t *)av_malloc(bufferSizeBytes*sizeof(uint8_t));
		#ifdef __VIDEO_STREAM_VERBOSE__
			std::cout << "VideoStream::VideoStream - Frame size : " << pCodecCtx->width << "x" << pCodecCtx->height << std::endl;
		#endif
		if(buffer==NULL)
			throw Exception("VideoStream::VideoStream - Unable to allocate video frame buffer.", __FILE__, __LINE__);

		// Assign appropriate parts of buffer to image planes in pFrameRGB (Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture) :
		avpicture_fill( (AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
		// Initialize libswscale :
		pSWSCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24, SWS_POINT, NULL, NULL, NULL);
		// Create format :
		HdlTextureFormat frameFormat(pCodecCtx->width, pCodecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, minFilter, magFilter, sWrapping, tWrapping, 0, maxLevel);

		// Create the texture :
		textureLinks.assign(numFrameBuffered, NULL);
		for(unsigned int i=0; i<numFrameBuffered; i++)
		{
			textureBuffers.push_back( new HdlTexture(frameFormat) );
			// We must write once before using PBO::copyToTexture on it.
			// We are also doing this to prevent reading from an empty (not-yet-allocated) texture.
			textureBuffers.back()->fill(0);
		}

		#ifdef __USE_PBO__
			// Create PBO for uplodaing data to GPU :
			pbo = new HdlPBO(frameFormat, GL_PIXEL_UNPACK_BUFFER_ARB,GL_STREAM_DRAW_ARB);
		#endif

		// Finish by forcing read of first frame :
		readNextFrame();
	}

	VideoStream::~VideoStream(void)
	{
		#ifdef __USE_PBO__
			pbo->unmap();
			delete pbo;
		#endif
		for(std::vector<HdlTexture*>::iterator it=textureBuffers.begin(); it!=textureBuffers.end(); it++)
			delete *it;
		textureBuffers.clear();
		textureLinks.clear();
		sws_freeContext(pSWSCtx);
		// Free the RGB image
		av_free(buffer);
		av_free(pFrameRGB);
		// Free the YUV frame
		av_free(pFrame);
		// Close the codec
		avcodec_close(pCodecCtx);
		// Close the video file
		avformat_close_input(&pFormatCtx);
	}	

	int VideoStream::getReadFrameCount(void) const
	{
		return readFrameCount;
	}

	float VideoStream::getVideoDurationSec(void) const
	{
		return duration_sec;
	}

	float VideoStream::getCurrentTimeSec(void) const
	{
		return static_cast<float>(timeStampOfLastFrameRead-timeStampOffset) / timeStampFrameRate;
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
			retCode = av_read_frame(pFormatCtx, &packet);
			if(retCode==0)
			{
				// Is this a packet from the video stream?
				if(static_cast<unsigned int>(packet.stream_index)==idVideoStream)
				{
					// Decode video frame
					avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

					// Did we get a video frame?
					if(frameFinished>0)
					{
						// Convert the image from its native format to RGB
						sws_scale(pSWSCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
						#ifndef __USE_PBO__
							textureBuffers[idCurrentBufferForWritting]->write(pFrameRGB->data[0]);
						#else
							unsigned char* ptr = reinterpret_cast<unsigned char*>(pbo->map());
							memcpy(ptr, pFrameRGB->data[0], textureBuffers.front()->getNumPixels()*3);
							pbo->unmap();
							pbo->writeTexture(*textureBuffers[idCurrentBufferForWritting]);
						#endif

						// Change links :
						//for(int k=0; k<textureBuffers.size(); k++)
						//	setTextureLink(textureBuffers[(idCurrentBufferForWritting+k)%textureBuffers.size()], k);
						for(int k=0; k<textureBuffers.size(); k++)
							textureLinks[k] = textureBuffers[(idCurrentBufferForWritting+k)%textureBuffers.size()];

						// Update count :
						idCurrentBufferForWritting++;
						idCurrentBufferForWritting = idCurrentBufferForWritting % textureBuffers.size();
						foundNewFrame = true;
						// Add frame :
						readFrameCount++;
						// Save DTS (which is not exactly PTS) :
						if(packet.pts!=AV_NOPTS_VALUE)
						{
							if(timeStampOfLastFrameRead==0)
								timeStampOffset = packet.pts;
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
		int64_t timestamp = static_cast<int64_t>(time_sec)*timeStampFrameRate;
		retCode = av_seek_frame(pFormatCtx, idVideoStream, timestamp, flags);
		if(retCode<0)
			throw Exception("VideoStream::seek - Seek operation failed (at av_seek_frame).", __FILE__, __LINE__);
		avcodec_flush_buffers(pCodecCtx);
	}

	HdlTexture& VideoStream::frame(const int& id)
	{
		return *textureLinks[id];
	}
