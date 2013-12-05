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
	\fn unsigned int ImageBuffer::getRowLength(void) const
	\brief Get the row length (number of pixels on a row times the number of components).
	\return The row length, in number of components actually present on a row.
	**/
	unsigned int ImageBuffer::getRowLength(void) const
	{
		return getWidth()*descriptor.numChannels;
	}

	/**
	\fn unsigned char* ImageBuffer::getBuffer(void)
	\brief Get access to the buffer.
	\return A pointer to the buffer array.
	**/
	unsigned char* ImageBuffer::getBuffer(void)
	{
		return buffer;
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

	/**
	\fn signed long long int ImageBuffer::get(unsigned int x, unsigned int y, GLenum channel) const
	\brief Get an element from the array. The output type is much larger than any possible type and the value from buffer is not stretched except if the buffer is in single or double floating point precision.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The corresponding value.
	**/
	signed long long int ImageBuffer::get(unsigned int x, unsigned int y, GLenum channel) const
	{
		unsigned int pos = getIndex(x, y, channel);
		GLenum depth = getGLDepth();

		#define ADAPT(depthEnum, Type) \
			if(depthEnum==depth) \
				return static_cast<signed long long>( (reinterpret_cast<Type*>(buffer))[pos] );

		#define ADAPTF(depthEnum, Type) \
			if(depthEnum==depth) \
				return getDenormalizedValue<signed long long>( (reinterpret_cast<Type*>(buffer))[pos] );

			ADAPT(	GL_BYTE, 		char)
		else 	ADAPT(	GL_UNSIGNED_BYTE,	unsigned char)
		else 	ADAPT(	GL_SHORT,		short)
		else 	ADAPT(	GL_UNSIGNED_SHORT,	unsigned short)
		else 	ADAPT(	GL_INT,			int)
		else 	ADAPT(	GL_UNSIGNED_INT,	unsigned int)
		else 	ADAPTF(	GL_FLOAT,		float)
		else 	ADAPTF(	GL_DOUBLE,		double)

		#undef ADAPT
		#undef ADAPTF
	}
		
	/**
	\fn ImageBuffer& ImageBuffer::set(unsigned int x, unsigned int y, GLenum channel, const signed long long& value)
	\brief Set an element in the array. The value is clamped to fit the precision of the buffer or normalized if the buffer is in single or double floating point precision.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\param value The input value.
	\return This.
	**/		
	ImageBuffer& ImageBuffer::set(unsigned int x, unsigned int y, GLenum channel, const signed long long& value)
	{
		unsigned int pos = getIndex(x, y, channel);

		#define ADAPT(depthEnum, Type) \
			if(depthEnum==depth) \
				(reinterpret_cast<Type*>(buffer))[pos] = static_cast<Type>( std::min( std::max( value, static_cast<signed long long>(std::numeric_limits<Type>::min()) ), static_cast<signed long long>(std::numeric_limits<Type>::max()) ) );

		#define ADAPTF(depthEnum, Type) \
			if(depthEnum==depth) \
				(reinterpret_cast<Type*>(buffer))[pos] = getNormalizedValue(value);

			ADAPT(	GL_BYTE, 		char)
		else 	ADAPT(	GL_UNSIGNED_BYTE,	unsigned char)
		else 	ADAPT(	GL_SHORT,		short)
		else 	ADAPT(	GL_UNSIGNED_SHORT,	unsigned short)
		else 	ADAPT(	GL_INT,			int)
		else 	ADAPT(	GL_UNSIGNED_INT,	unsigned int)
		else 	ADAPTF(	GL_FLOAT,		float)
		else 	ADAPTF(	GL_DOUBLE,		double)

		#undef ADAPT
		#undef ADAPTF

		return (*this);
	}

	/**
	\fn float ImageBuffer::getNormalized(unsigned int x, unsigned int y, GLenum channel) const
	\brief Get an element from the array. The output is between 0.0f and 1.0f.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The corresponding value.
	**/
	float ImageBuffer::getNormalized(unsigned int x, unsigned int y, GLenum channel) const
	{
		unsigned int pos = getIndex(x, y, channel);

		#define ADAPT(depthEnum, Type) \
			if(depthEnum==depth) \
				return getNormalizedValue( (reinterpret_cast<Type*>(buffer))[pos] );

		#define ADAPTF(depthEnum, Type) \
			if(depthEnum==depth) \
				return (reinterpret_cast<Type*>(buffer))[pos];

			ADAPT(	GL_BYTE, 		char)
		else 	ADAPT(	GL_UNSIGNED_BYTE,	unsigned char)
		else 	ADAPT(	GL_SHORT,		short)
		else 	ADAPT(	GL_UNSIGNED_SHORT,	unsigned short)
		else 	ADAPT(	GL_INT,			int)
		else 	ADAPT(	GL_UNSIGNED_INT,	unsigned int)
		else 	ADAPTF(	GL_FLOAT,		float)
		else 	ADAPTF(	GL_DOUBLE,		double)

		#undef ADAPT
		#undef ADAPTF
	}

	/**
	\fn ImageBuffer& ImageBuffer::setNormalized(unsigned int x, unsigned int y, GLenum channel, const float& value)
	\brief Set an element in the array. The value is clamped to the range [0.0f, 1.0f] and stretched to the buffer precision.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\param value The input value.
	\return This.
	**/
	ImageBuffer& ImageBuffer::setNormalized(unsigned int x, unsigned int y, GLenum channel, const float& value)
	{
		unsigned int pos = getIndex(x, y, channel);

		#define ADAPT(depthEnum, Type) \
			if(depthEnum==depth) \
				(reinterpret_cast<Type*>(buffer))[pos] = getDenormalizedValue<Type>(value);

		#define ADAPTF(depthEnum, Type) \
			if(depthEnum==depth) \
				(reinterpret_cast<Type*>(buffer))[pos] = std::min( std::max(value, 0.0f), 1.0f);

			ADAPT(	GL_BYTE, 		char)
		else 	ADAPT(	GL_UNSIGNED_BYTE,	unsigned char)
		else 	ADAPT(	GL_SHORT,		short)
		else 	ADAPT(	GL_UNSIGNED_SHORT,	unsigned short)
		else 	ADAPT(	GL_INT,			int)
		else 	ADAPT(	GL_UNSIGNED_INT,	unsigned int)
		else 	ADAPTF(	GL_FLOAT,		float)
		else 	ADAPTF(	GL_DOUBLE,		double)

		#undef ADAPT
		#undef ADAPTF

		return (*this);
	}

// Specialization of the template operators : 
namespace Glip
{
	namespace Modules
	{
		template<>
		float ImageBuffer::getRangeMax<float>(void)
		{
			return 1.0f;
		}

		template<>
		float ImageBuffer::getRangeMax<double>(void)
		{
			return 1.0f;
		}
	
		template<>
		float ImageBuffer::getRangeMin<float>(void)
		{
			return 0.0f;
		}

		template<>
		float ImageBuffer::getRangeMin<double>(void)
		{
			return 0.0f;
		}

		template<>
		float ImageBuffer::getDynamicRange<float>(void)
		{
			return 1.0f;
		}

		template<>
		float ImageBuffer::getDynamicRange<double>(void)
		{
			return 1.0f;
		}

		template<>
		float ImageBuffer::clampValue(const signed long long& v)
		{
			return getNormalizedValue(v);
		}

		template<>
		double ImageBuffer::clampValue(const signed long long& v)
		{
			return getNormalizedValue(v);
		}
	}
}

