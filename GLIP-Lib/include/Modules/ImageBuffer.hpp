/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : TextureReader.hpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Module : Image Buffer 	                                                                 */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ImageBuffer.hpp
 * \brief   Module : Image Buffer
 * \author  R. KERVICHE
 * \date    November 23rd 2013
**/

#ifndef __IMAGE_BUFFER_INCLUDE__
#define __IMAGE_BUFFER_INCLUDE__

	// Include :
	#include <limits>
	#include "Core/OglInclude.hpp"
	#include "Core/HdlTexture.hpp"
	#include "Core/Exception.hpp"

namespace Glip
{
	// Prototypes
	using namespace Glip::CoreGL;

	namespace Modules
	{
		// Structure
/**
\class ImageBuffer
\brief Host-side image buffer.
**/
		class ImageBuffer : public __ReadOnly_HdlTextureFormat
		{
			private : 
				const HdlTextureFormatDescriptor	descriptor;
				unsigned char				*buffer;

			public : 
				ImageBuffer(const __ReadOnly_HdlTextureFormat& format);
				ImageBuffer(HdlTexture& texture);
				ImageBuffer(const ImageBuffer& image);
				~ImageBuffer(void);

				const HdlTextureFormatDescriptor& getDescriptor(void) const;
				bool isInside(unsigned int x, unsigned int y, GLenum channel=GL_NONE) const;
				unsigned int getPixelIndex(unsigned int x, unsigned int y) const;
				unsigned int getChannelIndex(GLenum channel) const;
				unsigned int getIndex(unsigned int x, unsigned int y, GLenum channel) const;

				const ImageBuffer& operator<<(HdlTexture& texture);
				const ImageBuffer& operator<<(const ImageBuffer& image);
				const ImageBuffer& operator<<(const void* bytes);
				const ImageBuffer& operator>>(HdlTexture& texture);
				const ImageBuffer& operator>>(ImageBuffer& image);
				const ImageBuffer& operator>>(void* bytes);

				template<typename T>
				T get(unsigned int x, unsigned int y, GLenum channel) const;
		
				template<typename T>
				ImageBuffer& set(unsigned int x, unsigned int y, GLenum channel, const T& value);
		};
		
		// Macro tools : 
		#define IMAGE_BUFFER_GET_CONVERT_TYPES( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					res = static_cast<EXTERNAL_TYPE>( static_cast<double>(s[pos])/(static_cast<double>(std::numeric_limits<BUFFER_TYPE>::max()) - 1.0)*(static_cast<double>(std::numeric_limits<EXTERNAL_TYPE>::max())-1.0) ); \
				}

		#define IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					res = static_cast<EXTERNAL_TYPE>( static_cast<double>(s[pos])*(static_cast<double>(std::numeric_limits<EXTERNAL_TYPE>::max())-1.0) ); \
				}

		#define IMAGE_BUFFER_GET_CONVERT_TYPES_EXTERNAL_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					res = static_cast<EXTERNAL_TYPE>(s[pos])/(static_cast<EXTERNAL_TYPE>(std::numeric_limits<BUFFER_TYPE>::max()) - 1.0); \
				}

		#define IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					res = static_cast<EXTERNAL_TYPE>(s[pos]); \
				}


		#define IMAGE_BUFFER_SET_CONVERT_TYPES( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					s[pos] = static_cast<BUFFER_TYPE>( static_cast<double>(value)/(static_cast<double>(std::numeric_limits<EXTERNAL_TYPE>::max())-1.0) * (static_cast<double>(std::numeric_limits<BUFFER_TYPE>::max())-1.0) ); \
				}

		#define IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					s[pos] = static_cast<BUFFER_TYPE>( static_cast<double>(value)/(static_cast<double>(std::numeric_limits<EXTERNAL_TYPE>::max())-1.0) ); \
				}

		#define IMAGE_BUFFER_SET_CONVERT_TYPES_EXTERNAL_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					s[pos] = static_cast<BUFFER_TYPE>( static_cast<double>(value) * (static_cast<double>(std::numeric_limits<BUFFER_TYPE>::max())-1.0) ); \
				}

		#define IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_AND_EXTERNAL_NORMALIZED( GL_TYPE_NAME, BUFFER_TYPE, EXTERNAL_TYPE) \
				if(depth==GL_TYPE_NAME) \
				{ \
					BUFFER_TYPE* s = reinterpret_cast<BUFFER_TYPE*>(buffer); \
					s[pos] = static_cast<BUFFER_TYPE>(value); \
				}

		// Template operators : 
		/**
		\fn T ImageBuffer::get(unsigned int x, unsigned int y, GLenum channel) const
		\brief Access data of the buffer with automatic dynamic range conversion.
		\param X-axis coordinate (along the width).
		\param Y-axis coordinate (along the height).
		\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
		\return The value of the targeted component.
		**/
		template<typename T>
		T ImageBuffer::get(unsigned int x, unsigned int y, GLenum channel) const
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);
			T res;
			
				IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_BYTE, 		char,		T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_UNSIGNED_BYTE, 	unsigned char,	T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_SHORT,		short,		T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_UNSIGNED_SHORT,	unsigned short,	T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_INT,			int,		T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES( 			GL_UNSIGNED_INT,	unsigned int,	T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_NORMALIZED( 	GL_FLOAT,		float,		T)
			else	IMAGE_BUFFER_GET_CONVERT_TYPES_BUFFER_NORMALIZED( 	GL_DOUBLE,		double,		T)
			else
				throw Exception("ImageBuffer::get - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return res;
		}

		/**
		\fn ImageBuffer& ImageBuffer::set(unsigned int x, unsigned int y, GLenum channel, const T& value)
		\brief Write data to the buffer with automatic dynamic range conversion.
		\param X-axis coordinate (along the width).
		\param Y-axis coordinate (along the height).
		\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
		\param value The value to be written.
		\return This.
		**/
		template<typename T>
		ImageBuffer& ImageBuffer::set(unsigned int x, unsigned int y, GLenum channel, const T& value)
		{
			GLenum depth = getGLDepth();
			unsigned int pos = getIndex(x, y, channel);

				IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_BYTE, 		char,		T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_UNSIGNED_BYTE, 	unsigned char,	T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_SHORT,		short,		T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_UNSIGNED_SHORT,	unsigned short,	T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_INT,			int,		T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES( 			GL_UNSIGNED_INT,	unsigned int,	T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_NORMALIZED( 	GL_FLOAT,		float,		T)
			else	IMAGE_BUFFER_SET_CONVERT_TYPES_BUFFER_NORMALIZED( 	GL_DOUBLE,		double,		T)
			else
				throw Exception("ImageBuffer::set - Unknown depth : \"" + glParamName(depth) + "\".", __FILE__, __LINE__);

			return (*this);
		}
	}
}

#endif

