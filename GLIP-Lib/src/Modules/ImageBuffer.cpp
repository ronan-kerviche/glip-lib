/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : TextureReader.cpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Module : Image Buffer    								 */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ImageBuffer.hpp
 * \brief   Module : Image Buffer
 * \author  R. KERVICHE
 * \date    November 23rd 2013
**/

	#include <cstring>
	#include "Modules/ImageBuffer.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::Modules;

	/**
	\fn ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format)
	\brief ImageBuffer constructor.
	\param format The format of this buffer.
	**/
	ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format)
	 :  __ReadOnly_HdlTextureFormat(format.getUncompressedFormat()), descriptor(format.getFormatDescriptor()), buffer(NULL)
	{
		buffer = new unsigned char[getSize()];
	}

	/**
	\fn ImageBuffer::ImageBuffer(HdlTexture& texture)	
	\brief ImageBuffer constructor.
	\param texture Copy the format and data of the this texture.
	**/
	ImageBuffer::ImageBuffer(HdlTexture& texture)	
	 :  __ReadOnly_HdlTextureFormat(texture.getUncompressedFormat()), descriptor(texture.getFormatDescriptor()), buffer(NULL)
	{
		buffer = new unsigned char[getSize()];

		(*this) << texture;
	}

	/**
	\fn ImageBuffer::ImageBuffer(const ImageBuffer& image)
	\brief ImageBuffer copy constructor.
	\param image Image buffer to copy.
	**/
	ImageBuffer::ImageBuffer(const ImageBuffer& image)
	 :  __ReadOnly_HdlTextureFormat(image), descriptor(image.descriptor), buffer(NULL)
	{
		buffer = new unsigned char[getSize()];
		
		(*this) << image;
	}

	ImageBuffer::~ImageBuffer(void)
	{
		delete[] buffer;
	}

	/**
	\fn const HdlTextureFormatDescriptor& ImageBuffer::getDescriptor(void) const
	\brief Get the descriptor of the GL mode.
	\return A HdlTextureFormatDescriptor object reference.
	**/
	const HdlTextureFormatDescriptor& ImageBuffer::getDescriptor(void) const
	{
		return descriptor;
	}

	/**
	\fn bool ImageBuffer::isInside(unsigned int x, unsigned int y, GLenum channel) const
	\brief Test if coordinates are valid for this buffer.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return True if the coordinates are valid.
	**/
	bool ImageBuffer::isInside(unsigned int x, unsigned int y, GLenum channel) const
	{
		bool 	dimensions 	= (x<=getWidth() && y<=getHeight());

		if(channel==GL_NONE || !dimensions)
			return dimensions;
		else
			return descriptor.hasChannel(channel);
	}

	/**
	\fn unsigned int ImageBuffer::getPixelIndex(unsigned int x, unsigned int y) const
	\brief Get the pixel index corresponding to the position coordinates.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\return The pixel index.
	**/
	unsigned int ImageBuffer::getPixelIndex(unsigned int x, unsigned int y) const
	{
		return y*getWidth() + x;
	}

	/**
	\fn unsigned int ImageBuffer::getChannelIndex(GLenum channel) const
	\brief Get the channel index.
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The channel index.
	**/
	unsigned int ImageBuffer::getChannelIndex(GLenum channel) const
	{
		return descriptor.channelIndex(channel);
	}

	/**
	\fn unsigned int ImageBuffer::getIndex(unsigned int x, unsigned int y, GLenum channel) const
	\brief Get the component index.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The component index.
	**/
	unsigned int ImageBuffer::getIndex(unsigned int x, unsigned int y, GLenum channel) const
	{
		return getPixelIndex(x,y)*descriptor.numChannels + getChannelIndex(channel);
	}

	/**
	\fn const unsigned char* ImageBuffer::getBuffer(void) const
	\brief Get access to the buffer.
	\return A pointer to the buffer array.
	**/
	const unsigned char* ImageBuffer::getBuffer(void) const
	{
		return buffer;
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator<<(HdlTexture& texture)
	\brief Copy a texture to this buffer.
	\param texture The texture to copy.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator<<(HdlTexture& texture)
	{
		if(!isCompatibleWith(texture))
			throw Exception("ImageBuffer::operator<< - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			glPixelStorei(GL_PACK_ALIGNMENT, 1);

			// Read the texture : 
			texture.bind();
			glGetTexImage(GL_TEXTURE_2D, 0, descriptor.aliasMode, getGLDepth(), reinterpret_cast<GLvoid*>(buffer));
			
			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("ImageBuffer::operator<<", "glGetTexImage()")
			#endif

			HdlTexture::unbind();

			return (*this);
		}
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator<<(const ImageBuffer& image)
	\brief Copy a buffer.
	\param image The buffer to copy.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator<<(const ImageBuffer& image)
	{
		if(!isCompatibleWith(image))
			throw Exception("ImageBuffer::operator<< - ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			std::memcpy(buffer, image.buffer, getSize());
			
			return (*this);
		}
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator<<(const void* bytes)
	\brief Copy an array.
	\param bytes The buffer to copy (assumed to be of size __ReadOnly_HdlTextureFormat::getSize()).
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator<<(const void* bytes)
	{
		std::memcpy(buffer, bytes, getSize());

		return (*this);
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator>>(HdlTexture& texture)
	\brief Write buffer to a texture.
	\param texture The texture to be written.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(HdlTexture& texture)
	{
		if(!isCompatibleWith(texture))
			throw Exception("ImageBuffer::operator>> - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			// Bind it
			texture.bind();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			//write
			glTexImage2D(GL_TEXTURE_2D, 0, getGLMode(), getWidth(), getHeight(), 0, getGLMode(), getGLDepth(), reinterpret_cast<GLvoid*>(buffer));

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("ImageBuffer::operator>>", "glTexImage2D()")
			#endif

			if( texture.getMaxLevel()>0 )
			{
				glGenerateMipmap(GL_TEXTURE_2D);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("ImageBuffer::operator>>", "glGenerateMipmap()")
				#endif
			}

			HdlTexture::unbind();

			return (*this);
		}
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator>>(ImageBuffer& image)
	\brief Copy a buffer.
	\param image The buffer to be written.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(ImageBuffer& image)
	{
		image << (*this);

		return (*this);
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator>>(void* bytes)
	\brief Copy an array.
	\param bytes The buffer to copy (assumed to be of size __ReadOnly_HdlTextureFormat::getSize()).
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(void* bytes)
	{
		std::memcpy(bytes, buffer, getSize());

		return (*this);
	}

// Specialization of the template operators : 
namespace Glip
{
	namespace Modules
	{

		template<>
		float ImageBuffer::get<float>(unsigned int x, unsigned int y, GLenum channel) const
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);
			float res;
		
				IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_BYTE, 		char,		float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_BYTE, 	unsigned char,	float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_SHORT,		short,		float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_SHORT,	unsigned short,	float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_INT,			int,		float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_INT,	unsigned int,	float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_FLOAT,		float,		float)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_DOUBLE,		double,		float)
			else
				throw Exception("ImageBuffer::get - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return res;
		}

		template<>
		double ImageBuffer::get<double>(unsigned int x, unsigned int y, GLenum channel) const
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);
			double res;
		
				IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_BYTE, 		char,		double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_BYTE, 	unsigned char,	double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_SHORT,		short,		double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_SHORT,	unsigned short,	double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_INT,			int,		double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_INT,	unsigned int,	double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_FLOAT,		float,		double)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_DOUBLE,		double,		double)
			else
				throw Exception("ImageBuffer::get - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return res;
		}

		template<>
		ImageBuffer& ImageBuffer::set<float>(unsigned int x, unsigned int y, GLenum channel, const float& value)
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);

				IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_BYTE, 		char,		float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_BYTE, 	unsigned char,	float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_SHORT,		short,		float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_SHORT,	unsigned short,	float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_INT,			int,		float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_INT,	unsigned int,	float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_FLOAT,		float,		float)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_DOUBLE,		double,		float)
			else
				throw Exception("ImageBuffer::set - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return (*this);
		}

		template<>
		ImageBuffer& ImageBuffer::set<double>(unsigned int x, unsigned int y, GLenum channel, const double& value)
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);

				IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_BYTE, 		char,		double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_BYTE, 	unsigned char,	double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_SHORT,		short,		double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_SHORT,	unsigned short,	double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_INT,			int,		double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( 		GL_UNSIGNED_INT,	unsigned int,	double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_FLOAT,		float,		double)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( 	GL_DOUBLE,		double,		double)
			else
				throw Exception("ImageBuffer::set - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return (*this);
		}
	}
}

