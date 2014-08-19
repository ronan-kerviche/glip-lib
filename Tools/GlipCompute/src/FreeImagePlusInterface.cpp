/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FreeImagePlusInterface.cpp                                                                */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : FreeImage3 interface for image input/output.                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "FreeImagePlusInterface.hpp"

	Glip::CoreGL::HdlTexture* loadImage(const std::string& filename)
	{
		fipImage inputImage;

		inputImage.load(filename.c_str());

		if(!inputImage.isValid())
			throw Glip::Exception("loadImage - Cannot load \"" + filename + "\".", __FILE__, __LINE__);

		FREE_IMAGE_COLOR_TYPE fipColorFormat = inputImage.getColorType();

		GLenum 	mode 	= GL_NONE;
		int 	planes	= 0;
		
		switch(fipColorFormat)
		{
			case FIC_RGB :
				mode 	= GL_RGB;
				planes 	= 3;
				break;
			case FIC_RGBALPHA :
				mode 	= GL_RGBA;
				planes 	= 4;
				break;
			default : 
				throw Glip::Exception("Unknown/Unsupported color format for \"" + filename + "\".", __FILE__, __LINE__);
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
				throw Glip::Exception("Unknown/Unsupported bit depth for \"" + filename + "\".", __FILE__, __LINE__);
		}

		Glip::CoreGL::HdlTextureFormat format(inputImage.getWidth(), inputImage.getHeight(), mode, depth);

		Glip::Modules::ImageBuffer buffer(format);

		// Copy : 
		RGBQUAD pixel;
		for(unsigned int y=0; y<format.getHeight(); y++)
		{
			for(unsigned int x=0; x<format.getWidth(); x++)
			{
				//height = in.getHeight();
				//in.getPixelColor(50, height-1-50, &pixel);

				inputImage.getPixelColor(x, y, &pixel);
				buffer.set(x, y, GL_RED, 	pixel.rgbRed);
				buffer.set(x, y, GL_GREEN, 	pixel.rgbGreen);
				buffer.set(x, y, GL_BLUE, 	pixel.rgbBlue);
			
				if(planes>=4)
				//	buffer.set(x, y, GL_ALPHA, pixel.rgbAlpha);
					throw Glip::Exception("Alpha layer unsupported.", __FILE__, __LINE__);
			}	
		}

		Glip::CoreGL::HdlTexture* texture = new Glip::CoreGL::HdlTexture(format);
		buffer >> (*texture);

		return texture;
	}

	void saveImage(Glip::CoreGL::HdlTexture& texture, const std::string& filename)
	{
		const Glip::CoreGL::HdlTextureFormatDescriptor& descriptor = texture.getFormatDescriptor();

		int bpp = descriptor.pixelSizeInBits;
		if(bpp==0)
			bpp = descriptor.numChannels * Glip::CoreGL::HdlTextureFormatDescriptorsList::getTypeDepth(texture.getGLDepth())*8;

		fipImage outputImage(FIT_BITMAP, texture.getWidth(), texture.getHeight(), bpp);
		Glip::Modules::ImageBuffer buffer(texture);

		if(!outputImage.isValid())
			throw Glip::Exception("Could not save image to \"" + filename + "\", format is incompatible.", __FILE__, __LINE__);

		// Copy : 
		RGBQUAD pixel;
		for(unsigned int y=0; y<texture.getHeight(); y++)
		{
			for(unsigned int x=0; x<texture.getWidth(); x++)
			{
				pixel.rgbRed 	= buffer.get(x, y, GL_RED);
				pixel.rgbGreen	= buffer.get(x, y, GL_GREEN);
				pixel.rgbBlue	= buffer.get(x, y, GL_BLUE);

				if(descriptor.numChannels>=4)
				//	buffer.set(x, y, GL_ALPHA, pixel.rgbAlpha);
					throw Glip::Exception("Alpha layer unsupported.", __FILE__, __LINE__);

				outputImage.setPixelColor(x, y, &pixel);
			}	
		}

		// Save : 
		outputImage.save(filename.c_str());
	}

