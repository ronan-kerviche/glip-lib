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

	// Includes
	#include <limits>
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Core/HdlTexture.hpp"
	#include "Core/Exception.hpp"

namespace Glip
{
	// Prototypes
	using namespace Glip::CoreGL;

	namespace Modules
	{
		// Structure :
/**
\class ImageBuffer
\brief Host-side image buffer.
**/
		class GLIP_API ImageBuffer : public __ReadOnly_HdlTextureFormat
		{
			private : 
				static const unsigned int headerNumBytes;
				static const unsigned int maxCommentLength;
				static const std::string headerSignature;

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
				unsigned int getRowLength(void) const;
				unsigned char* getBuffer(void);
				const unsigned char* getBuffer(void) const;

				void setMinFilter(GLenum mf);
				void setMagFilter(GLenum mf);
				void setSWrapping(GLenum m);
				void setTWrapping(GLenum m);

				const ImageBuffer& operator<<(HdlTexture& texture);
				const ImageBuffer& operator<<(const ImageBuffer& image);
				const ImageBuffer& operator<<(const void* bytes);
				const ImageBuffer& operator>>(HdlTexture& texture) const;
				const ImageBuffer& operator>>(ImageBuffer& image) const;
				const ImageBuffer& operator>>(void* bytes) const;

				template<typename T>
				T* reinterpret(unsigned int x, unsigned int y, GLenum channel);

				template<typename T>
				const T* reinterpret(unsigned int x, unsigned int y, GLenum channel) const;

				signed long long get(unsigned int x, unsigned int y, GLenum channel) const;
				ImageBuffer& set(unsigned int x, unsigned int y, GLenum channel, const signed long long& value);
				float getNormalized(unsigned int x, unsigned int y, GLenum channel) const;
				ImageBuffer& setNormalized(unsigned int x, unsigned int y, GLenum channel, const float& value);

				// Static tools : 
				template<typename T>
				static float getRangeMax(void);
		
				template<typename T>
				static float getRangeMin(void);

				template<typename T>
				static float getDynamicRange(void);

				template<typename T>
				static float getNormalizedValue(const T& v);

				template<typename T>
				static T getDenormalizedValue(const float& v);

				template<typename T>
				static T clampValue(const signed long long& v);

				static ImageBuffer* load(const std::string& filename, std::string* comment=NULL);
				void write(const std::string& filename, const std::string& comment="") const;
		};

		// Template functions : 
			/**
			\fn T* ImageBuffer::reinterpret(unsigned int x, unsigned int y, GLenum channel)
			\brief Reinterpret the buffer to some given type.
			\param x X-axis coordinate (along the width).
			\param y Y-axis coordinate (along the height).
			\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
			\return The buffer at the requested component position, reinterpreted to the requested type.
			**/
			template<typename T>
			T* ImageBuffer::reinterpret(unsigned int x, unsigned int y, GLenum channel)
			{
				unsigned int pos = getIndex(x, y, channel) * getChannelDepth();
				return reinterpret_cast<T*>(buffer + pos);
			}

			/**
			\fn const T* ImageBuffer::reinterpret(unsigned int x, unsigned int y, GLenum channel) const
			\brief Reinterpret the buffer to some given type.
			\param x X-axis coordinate (along the width).
			\param y Y-axis coordinate (along the height).
			\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
			\return The buffer at the requested component position, reinterpreted to the requested type.
			**/
			template<typename T>
			const T* ImageBuffer::reinterpret(unsigned int x, unsigned int y, GLenum channel) const
			{
				unsigned int pos = getIndex(x, y, channel) * getChannelDepth();
				return reinterpret_cast<T*>(buffer + pos);
			}

			/**
			\fn float ImageBuffer::getRangeMax(void)
			\brief Get the maximum of a given type.
			\return The maximum of the type, in single precision floating point format.
			**/
			template<typename T>
			float ImageBuffer::getRangeMax(void)
			{
				return static_cast<float>(std::numeric_limits<T>::max());
			}
		
			/**
			\fn float ImageBuffer::getRangeMin(void)
			\brief Get the minimum of a given type.
			\return The minimum of the type, in single precision floating point format.
			**/
			template<typename T>
			float ImageBuffer::getRangeMin(void)
			{
				return static_cast<float>(std::numeric_limits<T>::min());
			}

			/**
			\fn float ImageBuffer::getDynamicRange(void)
			\brief Get the dynamic range of a given type.
			\return The range of the type, in single precision floating point format.
			**/
			template<typename T>
			float ImageBuffer::getDynamicRange(void)
			{
				return getRangeMax<T>() - getRangeMin<T>();
			}
	
			/**
			\fn float ImageBuffer::getNormalizedValue(const T& v)
			\brief Get the normalized value conversion. Assume the full dynamic range of the input type is used.
			\param v Input value.
			\return The normalized value in single precision floating point format (in the [0.0f, 1.0f] range).
			**/
			template<typename T>
			float ImageBuffer::getNormalizedValue(const T& v)
			{
				return static_cast<float>(v) / getDynamicRange<T>();
			}

			/**
			\fn T ImageBuffer::getDenormalizedValue(const float& v)
			\brief Get the denormalized value conversion. Clamp the input to the [0.0f, 1.0f] range.
			\param v Input value.
			\return The denormalized value expressed in the full dynamic range of the requested output type.
			**/
			template<typename T>
			T ImageBuffer::getDenormalizedValue(const float& v)
			{
				return static_cast<T>( std::min( std::max(v, 0.0f), 1.0f)  * getDynamicRange<T>() - getRangeMin<T>() );
			}

			/**
			\fn T ImageBuffer::clampValue(const signed long long& v)
			\brief Clamp the value to the full dynamic range of the output type.
			\param v Input value.
			\return The clamped value.
			**/
			template<typename T>
			T ImageBuffer::clampValue(const signed long long& v)
			{
				return static_cast<T>(  std::min( std::max(v, static_cast<signed long long>(std::numeric_limits<T>::min())), static_cast<signed long long>(std::numeric_limits<T>::max()) ));
			}
	}
}

#endif

