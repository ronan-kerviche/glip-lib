/* ************************************************************************************************************* */
/*                                                                                                               */
/*     LIBRAW INTERFACE                                                                                          */
/*     Interface to the LibRaw library for the OpenGL Image Processing LIBrary                                   */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : LibRawInterface.cpp                                                                       */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : LibRaw interface for image input/output.                                                  */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Includes :
	#include "LibRawInterface.hpp"
	#include <libraw/libraw.h>

namespace LibRawInterface
{
	Glip::CoreGL::HdlTextureFormat decodeRaw(LibRaw& rawProcessor, const std::string& filename)
	{
		int returnCode = rawProcessor.open_file(filename.c_str());
		if(returnCode!=LIBRAW_SUCCESS)
			throw Glip::Exception("libRawLoadImage - Error while loading the file " + filename + " : " + std::string(libraw_strerror(returnCode)) + ".", __FILE__, __LINE__);
	
		returnCode = rawProcessor.unpack();
		if(returnCode!=LIBRAW_SUCCESS)
			throw Glip::Exception("libRawLoadImage - Error while unpacking the file " + filename + " : " + std::string(libraw_strerror(returnCode)) + ".", __FILE__, __LINE__);
	
		libraw_decoder_info_t decoderInfo;
		rawProcessor.get_decoder_info(&decoderInfo);
		if(!(decoderInfo.decoder_flags & LIBRAW_DECODER_FLATFIELD))
			throw Glip::Exception("libRawLoadImage - Error while unpacking the file " + filename + " : not a Bayer-pattern RAW file.", __FILE__, __LINE__);

		// WARNING, THE FORMAT MUST BE GL_LUMINANCE16 TO ALLOW THE SHADERS TO READ FULL 16 BITS PRECISION. 
		return Glip::CoreGL::HdlTextureFormat(rawProcessor.imgdata.sizes.raw_width, rawProcessor.imgdata.sizes.raw_height, GL_LUMINANCE16, GL_UNSIGNED_SHORT);
	}

	Glip::Modules::ImageBuffer* loadImage(const std::string& filename)
	{
		LibRaw rawProcessor;
		Glip::CoreGL::HdlTextureFormat format = decodeRaw(rawProcessor, filename);
		Glip::Modules::ImageBuffer* imageBuffer = new Glip::Modules::ImageBuffer(format);
		Glip::Modules::ImageBuffer original(const_cast<void*>(reinterpret_cast<const void*>(rawProcessor.imgdata.rawdata.raw_image)), format, 4);
		imageBuffer->blit(original, 0, 0, 0, 0, -1, -1, false, true);
		return imageBuffer;
	}

	Glip::CoreGL::HdlTexture* loadTexture(const std::string& filename)
	{
		LibRaw rawProcessor;
		Glip::CoreGL::HdlTextureFormat format = decodeRaw(rawProcessor, filename);
		Glip::CoreGL::HdlTexture* texture = new Glip::CoreGL::HdlTexture(format);
		// Flip y, in place :
		const int alignment = 4;
		const size_t rowSize = static_cast<size_t>(format.getWidth()*format.getPixelSize() + (alignment-1)) & ~static_cast<size_t>(alignment-1);
		char* buffer = new char[rowSize];
		for(int i=0; i<format.getHeight()/2; i++)
		{
			void	*topLine = reinterpret_cast<void*>(reinterpret_cast<char*>(rawProcessor.imgdata.rawdata.raw_image)+i*rowSize),
				*bottomLine = reinterpret_cast<void*>(reinterpret_cast<char*>(rawProcessor.imgdata.rawdata.raw_image)+(format.getHeight()-i-1)*rowSize);
			std::memcpy(reinterpret_cast<void*>(buffer), topLine, rowSize);
			std::memcpy(topLine, bottomLine, rowSize);
			std::memcpy(bottomLine, reinterpret_cast<void*>(buffer), rowSize);
		}
		delete[] buffer;
		texture->write(reinterpret_cast<void*>(rawProcessor.imgdata.rawdata.raw_image), GL_ZERO, GL_ZERO, alignment);
		return texture;
	}
}

