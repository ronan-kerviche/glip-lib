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
	#include <fstream>
	#include "Modules/ImageBuffer.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::Modules;

	const unsigned int 	ImageBuffer::headerNumBytes 	= (8 + 4*2 + 4*6 + 4*2 + 4);
	const unsigned int 	ImageBuffer::maxCommentLength	= 1048576;			// 1MB
	const std::string 	ImageBuffer::headerSignature 	= "GLIPRAW0";

	/**
	\fn ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format)
	\brief ImageBuffer constructor.
	\param format The format of this buffer (or the equivalent uncompressed format).
	**/
	ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format)
	 :  __ReadOnly_HdlTextureFormat(format.getUncompressedFormat()), descriptor(format.getFormatDescriptor()), buffer(NULL)
	{
		if(getSize()==0)
			throw Exception("ImageBuffer::ImageBuffer - Cannot build an empty format.", __FILE__, __LINE__);
	
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
		if(getSize()==0)
			throw Exception("ImageBuffer::ImageBuffer - Cannot build an empty format.", __FILE__, __LINE__);
		
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
	\fn    void ImageBuffer::setMinFilter(GLenum mf)
	\brief Sets the texture's minification parameter.
	\param mf The new minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR).
	\fn    void ImageBuffer::setMagFilter(GLenum mf)
	\brief Sets the texture's magnification parameter.
	\param mf The new magnification filter (e.g. GL_NEAREST or GL_LINEAR, only these two options are accepted).
	\fn    void ImageBuffer::setSWrapping(GLenum m)
	\brief Sets the texture's S wrapping parameter.
	\param m The new S wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	\fn    void ImageBuffer::setTWrapping(GLenum m)
	\brief Sets the texture's T wrapping parameter.
	\param m The new T wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	**/
	void ImageBuffer::setMinFilter(GLenum mf)	{ minFilter = mf; }
	void ImageBuffer::setMagFilter(GLenum mf)	{ magFilter = mf; }
	void ImageBuffer::setSWrapping(GLenum m)	{ wraps     = m;  }
	void ImageBuffer::setTWrapping(GLenum m)	{ wrapt     = m;  }

	/**
	\fn const ImageBuffer& ImageBuffer::operator<<(HdlTexture& texture)
	\brief Copy a texture to this buffer and the following properties : minification and magnification filters, S and T wrapping modes.
	\param texture The texture to copy.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator<<(HdlTexture& texture)
	{
		if(!isCompatibleWith(texture))
			throw Exception("ImageBuffer::operator<< - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			// Bind ; 
			texture.bind();

			glPixelStorei(GL_PACK_ALIGNMENT, 1);

			// Read : 
			glGetTexImage(GL_TEXTURE_2D, 0, descriptor.aliasMode, getGLDepth(), reinterpret_cast<GLvoid*>(buffer));
			
			GLenum err = glGetError();
			if(err!=GL_NO_ERROR)
				throw Exception("ImageBuffer::operator<< - Unable to copy data from texture (glGetTexImage). (OpenGL error : " + glParamName(err) + ").", __FILE__, __LINE__);

			HdlTexture::unbind();

			setMinFilter(texture.getMinFilter());
			setMagFilter(texture.getMagFilter());
			setSWrapping(texture.getSWrapping());
			setTWrapping(texture.getTWrapping());

			return (*this);
		}
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator<<(const ImageBuffer& image)
	\brief Copy a buffer and the following properties : minification and magnification filters, S and T wrapping modes.
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
	
			setMinFilter(image.getMinFilter());
			setMagFilter(image.getMagFilter());
			setSWrapping(image.getSWrapping());
			setTWrapping(image.getTWrapping());
			
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
	\fn const ImageBuffer& ImageBuffer::operator>>(HdlTexture& texture) const
	\brief Write buffer to a texture and the following properties : minification and magnification filters, S and T wrapping modes.
	\param texture The texture to be written.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(HdlTexture& texture) const
	{
		if(!isCompatibleWith(texture))
			throw Exception("ImageBuffer::operator>> - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			// Bind : 
			texture.bind();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// Write :
			glTexImage2D(GL_TEXTURE_2D, 0, getGLMode(), getWidth(), getHeight(), 0, descriptor.aliasMode, getGLDepth(), reinterpret_cast<GLvoid*>(buffer));

			GLenum err = glGetError();
			if(err!=GL_NO_ERROR)
				throw Exception("ImageBuffer::operator>> - Unable to copy data to texture (glTexImage2D). (OpenGL error : " + glParamName(err) + ").", __FILE__, __LINE__);

			if( texture.getMaxLevel()>0 )
			{
				glGenerateMipmap(GL_TEXTURE_2D);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("ImageBuffer::operator>>", "glGenerateMipmap()")
				#endif
			}

			HdlTexture::unbind();

			texture.setMinFilter(getMinFilter());
			texture.setMagFilter(getMagFilter());
			texture.setSWrapping(getSWrapping());
			texture.setTWrapping(getTWrapping());

			return (*this);
		}
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator>>(ImageBuffer& image) const
	\brief Copy a buffer and the following properties : minification and magnification filters, S and T wrapping modes.
	\param image The buffer to be written.
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(ImageBuffer& image) const
	{
		image << (*this);

		return (*this);
	}

	/**
	\fn const ImageBuffer& ImageBuffer::operator>>(void* bytes) const
	\brief Copy an array.
	\param bytes The buffer to copy (assumed to be of size __ReadOnly_HdlTextureFormat::getSize()).
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(void* bytes) const
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
		else
			throw Exception("ImageBuffer::get - Unknown GL depth type (internal error).", __FILE__, __LINE__);

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
		else
			throw Exception("ImageBuffer::set - Unknown GL depth type (internal error).", __FILE__, __LINE__);

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
		else
			throw Exception("ImageBuffer::getNormalized - Unknown GL depth type (internal error).", __FILE__, __LINE__);

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
		else
			throw Exception("ImageBuffer::setNormalized - Unknown GL depth type (internal error).", __FILE__, __LINE__);

		#undef ADAPT
		#undef ADAPTF

		return (*this);
	}

	/**
	\fn ImageBuffer* ImageBuffer::load(const std::string& filename, std::string* comment)
	\brief Load an image buffer from a RAW file. The raw file contain all image information, plus texture setting and an optional comment.
	\param filename The file name (and path).
	\param comment If the pointer is non-null and if there is a comment in the file, the comment will be written in the tarted string.
	\return A pointer to an ImageBuffer object. The user has the responsability to release the memory (with delete). Raise an exception if any error occurs.
	**/
	ImageBuffer* ImageBuffer::load(const std::string& filename, std::string* comment)
	{
		// Try to open for read : 
		std::fstream file;

		file.open( filename.c_str(), std::fstream::in | std::fstream::binary );

		if(!file.is_open())
		{
			file.close();
			throw Exception("ImageBuffer::load - Cannot open file \"" + filename + "\" for reading.", __FILE__, __LINE__);
		}

		file.seekg(0, std::ios_base::end);
		size_t fileLength = file.tellg();

		if(fileLength<headerNumBytes)
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : header is to short.", __FILE__, __LINE__);

		// Rewind : 
		file.seekg(std::ios_base::beg);

		// Read the header : 
		char header[headerNumBytes];

		file.read(header, headerNumBytes);

		unsigned int p = 0;

		// Check the magic signature : 
		std::string signature(header, 8);

		if(signature!=headerSignature)
		{
			file.close();
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the file is not a raw file (version 0).", __FILE__, __LINE__);
		}

		p += signature.size();

		// Get the sizes and other data : 
		unsigned int	width,
				height;
	
		width		= * reinterpret_cast<unsigned int*>(header + p);	p += sizeof(width);
		height		= * reinterpret_cast<unsigned int*>(header + p);	p += sizeof(height);

		GLenum		mode,
				depth,
				minFilter,
				magFilter,
				sWrapping,
				tWrapping;

		mode		= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(mode);
		depth		= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(depth);
		minFilter	= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(minFilter);
		magFilter	= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(magFilter);
		sWrapping	= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(sWrapping);
		tWrapping	= * reinterpret_cast<GLenum*>(header + p);		p += sizeof(tWrapping);

		unsigned int	minMipmap,
				maxMipmap;

		minMipmap	= * reinterpret_cast<unsigned int*>(header + p);	p += sizeof(minMipmap);
		maxMipmap	= * reinterpret_cast<unsigned int*>(header + p);	p += sizeof(maxMipmap);

		unsigned int	commentLength;
		
		commentLength	= * reinterpret_cast<unsigned int*>(header + p);	p += sizeof(commentLength);

		if(commentLength>maxCommentLength)
		{
			file.close();
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the comment embedded in the file is too long.", __FILE__, __LINE__);
		}

		// Load comment, if necessary :
		if(commentLength==0)
		{
			if(comment!=NULL)
				comment->clear();
		}
		else
		{
			char* commentBuffer = new char[commentLength];

			file.read(commentBuffer, commentLength);

			if(comment!=NULL)
				comment->assign(commentBuffer, commentLength);

			delete[] commentBuffer;
		}

		// Create the resulting imageBuffer : 
		HdlTextureFormat format(width, height, mode, depth, minFilter, magFilter, sWrapping, tWrapping, minMipmap, maxMipmap);
		ImageBuffer* imageBuffer = new ImageBuffer(format);

		// Test remaining space in the file :
		size_t remainingBytes = fileLength - file.tellg();

		if(remainingBytes!=imageBuffer->getSize())
		{
			file.close();
			delete imageBuffer;
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the image length does not match expectation.", __FILE__, __LINE__);
		}

		// Else : load!
		file.read( reinterpret_cast<char*>(imageBuffer->getBuffer()), remainingBytes);

		// Finally : 
		file.close();

		return imageBuffer;
	}

	/**
	\fn void ImageBuffer::write(const std::string& filename, const std::string& comment) const
	\brief Write an image buffer to a RAW file. The raw file contain all image information, plus texture setting and an optional comment. Raise an exception if any error occurs.
	\param filename The file name (and path).
	\param comment Save this comment to the file (the current limit is a 1MB string).
	**/
	void ImageBuffer::write(const std::string& filename, const std::string& comment) const
	{
		if(comment.size()>maxCommentLength)
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\" : the comment is too long (it cannot exceed " + toString(maxCommentLength) + " characters).", __FILE__, __LINE__);

		// Try to open for read : 
		std::fstream file;

		file.open( filename.c_str(), std::fstream::out | std::fstream::binary );

		if(!file.is_open())
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\".", __FILE__, __LINE__);

		// Signature :
		file.write(headerSignature.c_str(), headerSignature.size());

		// Data :
		unsigned int	width		= getWidth(),
				height		= getHeight();
		GLenum		mode		= getGLMode(),
				depth		= getGLDepth(),
				minFilter	= getMinFilter(),
				magFilter	= getMagFilter(),
				sWrapping 	= getSWrapping(),
				tWrapping	= getTWrapping();
		unsigned int	minMipmap	= getBaseLevel(),
				maxMipmap	= getMaxLevel();

		file.write(	reinterpret_cast<char*>(&width), 	sizeof(width) );
		file.write(	reinterpret_cast<char*>(&height), 	sizeof(height) );
		file.write(	reinterpret_cast<char*>(&mode), 	sizeof(mode) );
		file.write(	reinterpret_cast<char*>(&depth), 	sizeof(depth) );
		file.write(	reinterpret_cast<char*>(&minFilter), 	sizeof(minFilter) );
		file.write(	reinterpret_cast<char*>(&magFilter), 	sizeof(magFilter) );
		file.write(	reinterpret_cast<char*>(&sWrapping), 	sizeof(sWrapping) );
		file.write(	reinterpret_cast<char*>(&tWrapping), 	sizeof(tWrapping) );
		file.write(	reinterpret_cast<char*>(&minMipmap), 	sizeof(minMipmap) );
		file.write(	reinterpret_cast<char*>(&maxMipmap), 	sizeof(maxMipmap) );

		// Write comment : 
		if(comment.empty())
		{
			unsigned int l = 0;
			file.write(	reinterpret_cast<char*>(&l),	sizeof(l));
		}
		else
		{
			unsigned int l = comment.size();
			file.write(	reinterpret_cast<char*>(&l),	sizeof(l));

			file.write(	comment.c_str(), l);
		}

		// Write data :
		file.write( reinterpret_cast<const char*>(getBuffer()), getSize());

		// Finally : 
		file.close();
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

