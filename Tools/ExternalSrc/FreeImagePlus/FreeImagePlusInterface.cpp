/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : FreeImagePlusInterface.cpp                                                                */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : FreeImage3 interface for image input/output.                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Includes :
	#include "FreeImagePlusInterface.hpp"
	#include <FreeImagePlus.h>

	// To be impletemented : read Bayer data directly from RAW files.
	// See http://sourceforge.net/p/freeimage/discussion/36110/thread/2bd2ff7f/?limit=50

namespace FreeImagePlusInterface
{
	Glip::CoreGL::HdlTextureFormat loadFIPImage(fipImage& inputImage, GLenum& fipMode, const std::string& filename)
	{
		inputImage.load(filename.c_str());
		if(!inputImage.isValid())
			throw Glip::Exception("loadImage - Cannot load \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);

		FREE_IMAGE_COLOR_TYPE fipColorFormat = inputImage.getColorType();
		GLenum 	mode 	= GL_NONE;
			//fipMode	= GL_NONE;
		int 	planes	= 0;
		
		switch(fipColorFormat)
		{
			case FIC_MINISBLACK :
			case FIC_MINISWHITE :
				mode 	= GL_LUMINANCE;
				fipMode	= GL_LUMINANCE;
				planes 	= 1;
				break;
			case FIC_RGB :
				mode 	= GL_RGB;
				fipMode	= GL_BGR;
				planes 	= 3;
				break;
			case FIC_RGBALPHA :
				mode 	= GL_RGBA;
				fipMode	= GL_BGRA;
				planes 	= 4;
				break;
			default : 
				throw Glip::Exception("Unknown/Unsupported color format for \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);
		}

		GLenum depth = GL_NONE;				
		int bitsPerComponent = inputImage.getBitsPerPixel() / planes;

		switch(bitsPerComponent)
		{
			case 1:
				depth = GL_BOOL;
				break;
			case 8 :
				depth = GL_UNSIGNED_BYTE;
				break;
			case 16 : 
				depth = GL_UNSIGNED_SHORT;
				break;
			case 32 :
	 			depth = GL_UNSIGNED_INT;
				break;
			default : 
				throw Glip::Exception("Unknown/Unsupported bit depth for \"" + filename + "\" : " + Glip::toString(bitsPerComponent) + ".", __FILE__, __LINE__, Glip::Exception::ClientException);
		}
		return Glip::CoreGL::HdlTextureFormat(inputImage.getWidth(), inputImage.getHeight(), mode, depth);
	}

	Glip::Modules::ImageBuffer* loadImage(const std::string& filename)
	{
		fipImage inputImage;
		GLenum fipMode = GL_NONE;
		Glip::CoreGL::HdlTextureFormat format = loadFIPImage(inputImage, fipMode, filename);
		format.setGLMode(fipMode);
		Glip::Modules::ImageBuffer* imageBuffer = new Glip::Modules::ImageBuffer(format, 4);
		(*imageBuffer) << reinterpret_cast<void*>(inputImage.accessPixels());
		return imageBuffer;
	}

	Glip::CoreGL::HdlTexture* loadTexture(const std::string& filename)
	{
		/*fipImage inputImage;

		inputImage.load(filename.c_str());

		if(!inputImage.isValid())
			throw Glip::Exception("loadImage - Cannot load \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);

		FREE_IMAGE_COLOR_TYPE fipColorFormat = inputImage.getColorType();

		GLenum 	mode 	= GL_NONE,
			fipMode	= GL_NONE;
		int 	planes	= 0;
		
		switch(fipColorFormat)
		{
			case FIC_MINISBLACK :
			case FIC_MINISWHITE :
				mode 	= GL_LUMINANCE;
				fipMode	= GL_LUMINANCE;
				planes 	= 1;
				break;
			case FIC_RGB :
				mode 	= GL_RGB;
				fipMode	= GL_BGR;
				planes 	= 3;
				break;
			case FIC_RGBALPHA :
				mode 	= GL_RGBA;
				fipMode	= GL_BGRA;
				planes 	= 4;
				break;
			default : 
				throw Glip::Exception("Unknown/Unsupported color format for \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);
		}

		GLenum depth = GL_NONE;				
		int bitsPerComponent = inputImage.getBitsPerPixel() / planes;

		switch(bitsPerComponent)
		{
			case 1:
				depth = GL_BOOL;
				break;
			case 8 :
				depth = GL_UNSIGNED_BYTE;
				break;
			case 16 : 
				depth = GL_UNSIGNED_SHORT;
				break;
			case 32 :
	 			depth = GL_UNSIGNED_INT;
				break;
			default : 
				throw Glip::Exception("Unknown/Unsupported bit depth for \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);
		}

		Glip::CoreGL::HdlTextureFormat format(inputImage.getWidth(), inputImage.getHeight(), mode, depth);

		Glip::CoreGL::HdlTexture* texture = new Glip::CoreGL::HdlTexture(format);

		texture->write(inputImage.accessPixels(), fipMode, GL_ZERO, 4);

		return texture;*/

		fipImage inputImage;
		GLenum fipMode = GL_NONE;
		Glip::CoreGL::HdlTextureFormat format = loadFIPImage(inputImage, fipMode, filename);
		Glip::CoreGL::HdlTexture* texture = new Glip::CoreGL::HdlTexture(format);
		texture->write(inputImage.accessPixels(), fipMode, GL_ZERO, 4);
		return texture;
	}

	void saveTexture(Glip::CoreGL::HdlTexture& texture, const std::string& filename)
	{
		const Glip::CoreGL::HdlTextureFormatDescriptor& descriptor = texture.getFormatDescriptor();
		const GLenum depth = texture.getGLDepth();

		const int bpp = descriptor.getPixelSizeInBits(texture.getGLDepth());

		// Determine the type of the output image : 
		FREE_IMAGE_TYPE fipType = FIT_UNKNOWN;

		if((depth==GL_BYTE || depth==GL_UNSIGNED_BYTE) && descriptor.numChannels>=1 && descriptor.numChannels<=4)
			fipType = FIT_BITMAP;
		if(depth==GL_SHORT && descriptor.numChannels==1)
			fipType = FIT_INT16;
		else if(depth==GL_UNSIGNED_SHORT && descriptor.numChannels==1)
			fipType = FIT_UINT16;
		else if((depth==GL_SHORT || depth==GL_UNSIGNED_SHORT) && descriptor.numChannels==3)
			fipType = FIT_RGB16;
		else if((depth==GL_SHORT || depth==GL_UNSIGNED_SHORT) && descriptor.numChannels==4)
			fipType = FIT_RGBA16;
		else if((depth==GL_INT || depth==GL_UNSIGNED_INT) && descriptor.numChannels==1)
			fipType = FIT_INT32;
		else if(depth==GL_FLOAT && descriptor.numChannels==1)
			fipType = FIT_FLOAT;
		else if(depth==GL_FLOAT && descriptor.numChannels==3)
			fipType = FIT_RGBF;
		else if(depth==GL_FLOAT && descriptor.numChannels==4)
			fipType = FIT_RGBAF;

		if(fipType==FIT_UNKNOWN)
			throw Glip::Exception("Could not save image to \"" + filename + "\", format is incompatible with FreeImage interface (" + Glip::toString(descriptor.numChannels) + " channels, " + Glip::getGLEnumName(depth) + " depth, " + Glip::toString(bpp) + " bits per pixel.)", __FILE__, __LINE__, Glip::Exception::ClientException);

		fipImage outputImage(fipType, texture.getWidth(), texture.getHeight(), bpp);
		
		if(!outputImage.isValid())
			throw Glip::Exception("Could not save image to \"" + filename + "\", format is incompatible.", __FILE__, __LINE__, Glip::Exception::ClientException);

		GLenum 	fipMode	= GL_NONE;

		// Flip the channels : 
		switch(texture.getGLMode())
		{
			case GL_RED:
				fipMode = GL_RED;
				break;
			case GL_LUMINANCE:
				fipMode = GL_LUMINANCE;
				break;
			case GL_RGB:
				fipMode = GL_BGR;
				break;
			case GL_RGBA:
				fipMode = GL_BGRA;
				break;
			default : 
				throw Glip::Exception("[INTERNAL ERROR] Cannot swap channels for type : " + Glip::CoreGL::getGLEnumName(texture.getGLMode()) + ".", __FILE__, __LINE__, Glip::Exception::ClientException);
		}

		texture.read(outputImage.accessPixels(), fipMode, GL_ZERO, 4);

		// Save : 
		bool test = outputImage.save(filename.c_str());

		if(!test)
			throw Glip::Exception("Could not save image to \"" + filename + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);
	}
}

