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
#include "Core/Exception.hpp"

using namespace Glip;
using namespace Glip::CoreGL;
using namespace Glip::Modules;

	const unsigned int 	ImageBuffer::headerNumBytes 	= (8 + 4*3 + 4*6 + 4*2 + 4);	// See the load/write functions for more precisions (size * num elements).
	const unsigned int 	ImageBuffer::maxCommentLength	= 1048576;			// 1MB
	const std::string 	ImageBuffer::headerSignature 	= "GLIPRAW1";

	/**
	\fn ImageBuffer::ImageBuffer(const HdlAbstractTextureFormat& format, int _alignment)
	\brief ImageBuffer constructor.
	\param format The format of this buffer (or the equivalent uncompressed format).
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(const HdlAbstractTextureFormat& format, int _alignment)
	 : 	HdlAbstractTextureFormat(format),
		descriptor(format.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.mode==GL_FLOAT) || (descriptor.mode==GL_DOUBLE);
		table = HdlDynamicTable::build(format.getGLDepth(), format.getWidth(), format.getHeight(), descriptor.numChannels, normalized, _alignment);
	}

	/**
	\fn ImageBuffer::ImageBuffer(void* buffer, const HdlAbstractTextureFormat& format, int _alignment)
	\brief ImageBuffer proxy constructor.
	\param buffer The buffer to point to, this memory table will NOT be deleted when this object is destroyed.
	\param format The format of this buffer (or the equivalent uncompressed format).
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(void* buffer, const HdlAbstractTextureFormat& format, int _alignment)
	 : 	HdlAbstractTextureFormat(format),
		descriptor(format.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.mode==GL_FLOAT) || (descriptor.mode==GL_DOUBLE);
		table = HdlDynamicTable::buildProxy(buffer, format.getGLDepth(), format.getWidth(), format.getHeight(), descriptor.numChannels, normalized, _alignment);
	}

	
	/**
	\fn ImageBuffer::ImageBuffer(HdlTexture& texture, int _alignment)	
	\brief ImageBuffer constructor.
	\param texture Copy the format and data of the this texture.
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(HdlTexture& texture, int _alignment)
	 :	HdlAbstractTextureFormat(texture),
		descriptor(texture.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.mode==GL_FLOAT) || (descriptor.mode==GL_DOUBLE);
		table = HdlDynamicTable::build(texture.getGLDepth(), texture.getWidth(), texture.getHeight(), descriptor.numChannels, normalized, _alignment);

		// Copy : 
		(*this) << texture;
	}

	/**
	\fn ImageBuffer::ImageBuffer(const ImageBuffer& image)
	\brief ImageBuffer copy constructor.
	\param image Image buffer to copy.
	**/
	ImageBuffer::ImageBuffer(const ImageBuffer& image)
	 :	HdlAbstractTextureFormat(image),
		descriptor(image.getFormatDescriptor()),
		table(NULL)
	{
		table = HdlDynamicTable::copy(*image.table);
	}

	ImageBuffer::~ImageBuffer(void)
	{
		delete table;
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
	\fn void* ImageBuffer::getPtr(void)
	\brief Get access to the buffer.
	\return A pointer to the buffer array.
	**/
	void* ImageBuffer::getPtr(void)
	{
		return table->getPtr();
	}

	/**
	\fn const void* ImageBuffer::getPtr(void) const
	\brief Get access to the buffer.
	\return A pointer to the buffer array.
	**/
	const void* ImageBuffer::getPtr(void) const
	{
		return table->getPtr();
	}

	void* ImageBuffer::getRowPtr(int i)
	{
		return table->getRowPtr(i);
	}

	const void* ImageBuffer::getRowPtr(int i) const
	{
		return table->getRowPtr(i);
	}
	
	/**
	\fn HdlDynamicTable& ImageBuffer::getTable(void)
	\brief Access the underlying table.
	\return A reference to the storage table in use.
	**/
	HdlDynamicTable& ImageBuffer::getTable(void)
	{
		return (*table);
	}
	
	/**
	\fn const HdlDynamicTable& ImageBuffer::getTable(void) const
	\brief Access the underlying table.
	\return A constant reference to the storage table in use.
	**/
	const HdlDynamicTable& ImageBuffer::getTable(void) const
	{
		return (*table);
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
			throw Exception("ImageBuffer::operator<< - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__, Exception::ModuleException);
		else
		{
			// Bind ; 
			texture.bind();

			glPixelStorei(GL_PACK_ALIGNMENT, getAlignment());

			// Read :
			glGetTexImage(GL_TEXTURE_2D, 0, descriptor.aliasMode, getGLDepth(), table->getPtr());
			
			GLenum err = glGetError();
			if(err!=GL_NO_ERROR)
				throw Exception("ImageBuffer::operator<< - Unable to copy data from texture (glGetTexImage). (OpenGL error : " + getGLEnumNameSafe(err) + ").", __FILE__, __LINE__, Exception::ModuleException);

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
			throw Exception("ImageBuffer::operator<< - ImageBuffer objects are incompatible.", __FILE__, __LINE__, Exception::ModuleException);
		else
		{
			if(image.getAlignment()==getAlignment())
				std::memcpy(table->getPtr(), image.table->getPtr(), getSize());
			else
			{
				for(int i=0; i<getHeight(); i++)
					std::memcpy(table->getRowPtr(i), image.table->getRowPtr(i), std::min(table->getRowSize(), image.table->getRowSize()));
			}	

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
	\param bytes The buffer to copy (assumed to be of size HdlAbstractTextureFormat::getSize()).
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator<<(const void* bytes)
	{
		std::memcpy(table->getPtr(), bytes, table->getSize());

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
			throw Exception("ImageBuffer::operator>> - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__, Exception::ModuleException);
		else
		{
			// Bind : 
			texture.bind();

			glPixelStorei(GL_UNPACK_ALIGNMENT, getAlignment());

			// Write :
			glTexImage2D(GL_TEXTURE_2D, 0, getGLMode(), getWidth(), getHeight(), 0, descriptor.aliasMode, getGLDepth(), table->getPtr());

			GLenum err = glGetError();
			if(err!=GL_NO_ERROR)
				throw Exception("ImageBuffer::operator>> - Unable to copy data to texture (glTexImage2D). (OpenGL error : " + getGLEnumNameSafe(err) + ").", __FILE__, __LINE__, Exception::ModuleException);

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
	\param bytes The buffer to copy (assumed to be of size HdlAbstractTextureFormat::getSize()).
	\return This.
	**/
	const ImageBuffer& ImageBuffer::operator>>(void* bytes) const
	{
		std::memcpy(bytes, table->getPtr(), table->getSize());

		return (*this);
	}

	/*
	\fn bool ImageBuffer::isInside(const int& x, const int& y) const
	\brief Check if coordinates are valid.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\return True if the coordinates lie inside the current image.
	*/
	bool ImageBuffer::isInside(const int& x, const int& y) const
	{
		return table->isInside(x, y, 0);
	}

	/*
	\fn bool ImageBuffer::isInside(const int& x, const int& y, const GLenum& channel) const
	\brief Check if coordinates are valid.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return True if the coordinates lie inside the current image.
	*/
	bool ImageBuffer::isInside(const int& x, const int& y, const GLenum& channel) const
	{
		const int c = descriptor.getChannelIndex(channel);

		if(c<0)
			return false;
		else
			return table->isInside(x, y, c);
	}

	/*
	\fn int ImageBuffer::getIndex(const int&x, const int& y, const GLenum& channel) const
	\brief Get data index.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The corresponding index, or -1 if the index is not valid.
	*/
	int ImageBuffer::getIndex(const int&x, const int& y, const GLenum& channel) const
	{
		const int c = descriptor.getChannelIndex(channel);
		
		if(c<0)
			return -1;
		else
			return table->getIndex(x, y, c);
	}

	/**
	\fn long long ImageBuffer::get(const int& x, const int& y, const GLenum& channel) const
	\brief Get an element from the array. The output type is much larger than any possible type and the value from buffer is not stretched except if the buffer is in single or double floating point precision.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The corresponding value.
	**/
	long long ImageBuffer::get(const int& x, const int& y, const GLenum& channel) const
	{
		return table->getl(x, y, descriptor.getChannelIndex(channel));
	}

	/**
	\fn void ImageBuffer::set(const long long& value, const int& x, const int& y, const GLenum& channel)
	\brief Set an element in the array. The value is clamped to fit the precision of the buffer or normalized if the buffer is in single or double floating point precision.
	\param value The input value.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	**/
	void ImageBuffer::set(const long long& value, const int& x, const int& y, const GLenum& channel)
	{
		table->setl(value, x, y, descriptor.getChannelIndex(channel));
	}

	/**
	\fn float ImageBuffer::getNormalized(const int& x, const int& y, const GLenum& channel) const
	\brief Get an element from the array. The output is between 0.0f and 1.0f.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	\return The corresponding value.
	**/
	float ImageBuffer::getNormalized(const int& x, const int& y, const GLenum& channel) const
	{
		return table->getNormalized(x, y, descriptor.getChannelIndex(channel));
	}

	/**
	\fn void ImageBuffer::setNormalized(const float& value, const int& x, const int& y, const GLenum& channel)
	\brief Set an element in the array. The value is clamped to the range [0.0f, 1.0f] and stretched to the buffer precision.
	\param value The input value.
	\param x X-axis coordinate (along the width).
	\param y Y-axis coordinate (along the height).
	\param channel The channel (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA or GL_LUMINANCE).
	**/
	void ImageBuffer::setNormalized(const float& value, const int& x, const int& y, const GLenum& channel)
	{
		table->setNormalized(value, x, y, descriptor.getChannelIndex(channel));
	}

	void ImageBuffer::blit(const ImageBuffer& src, const int& xSrc, const int& ySrc, const int& xDst, const int& yDst, int _width, int _height, const bool xFlip, const bool yFlip)
	{
		const int width = ((_width>0) ? _width : src.getWidth()),
			  height = ((_height>0) ? _height : src.getHeight());

		// Test the claim : 
		if(!src.isInside(xSrc, ySrc) || !src.isInside(xSrc+width-1, ySrc+height-1))
			throw Exception("ImageBuffer::blit - Invalid source rectangle starting at (" + toString(xSrc) + ";" + toString(ySrc) + ") of size (" + toString(width) + "x" + toString(height) + ") in an image of size (" + toString(src.getWidth()) + "x" + toString(src.getHeight()) + ").", __FILE__, __LINE__, Exception::ModuleException);
		if(!isInside(xDst, yDst) || !isInside(xDst+width-1, yDst+height-1))
			throw Exception("ImageBuffer::blit - Invalid destination rectangle starting at (" + toString(xDst) + ";" + toString(yDst) + ") of size (" + toString(width) + "x" + toString(height) + ") in an image of size (" + toString(getWidth()) + "x" + toString(getHeight()) + ").", __FILE__, __LINE__, Exception::ModuleException);

		const bool sameLayout = (src.getGLMode()==getGLMode()),	
			   sameDepth = (src.getGLDepth()==getGLDepth());
		const int rowOffset = (yFlip ? (height-1) : 0),
			  rowDirection = (yFlip ? -1 : 1),
			  columnOffset = (xFlip ? (width-1) : 0),
			  columnDirection = (xFlip ? -1 : 1);

		// Shortcut : 
		if(sameLayout && sameDepth && !xFlip)
		{
			const size_t rowSize = table->getRowSize();
			
			for(int y=0; y<height; y++)
				std::memcpy(reinterpret_cast<char*>(table->getRowPtr(yDst + y)), reinterpret_cast<const char*>(src.table->getRowPtr(ySrc + rowOffset + rowDirection*y)), rowSize);
		}
		else if(!table->isNormalized() && src.table->isNormalized())
		{
			const int srcPixelSize = src.descriptor.getPixelSize(src.getGLDepth()),
				  dstPixelSize = descriptor.getPixelSize(getGLDepth());

			bool isBlack = false;
			const int maxShuffleLength = 32;
			char shuffle[maxShuffleLength];
			const int length = HdlTextureFormatDescriptor::getBitShuffle(descriptor, getGLDepth(), src.descriptor, src.getGLDepth(), shuffle, maxShuffleLength, &isBlack);

			// Shortcut : 
			if(isBlack)
			{
				for(int y=0; y<height; y++)
					std::memset(table->getRowPtr(yDst + y), 0, width*dstPixelSize);
			}
			else
			{
				for(int y=0; y<height; y++)
				{
					char* dstRow = reinterpret_cast<char*>(table->getRowPtr(yDst + y));
					const char* srcRow = reinterpret_cast<const char*>(src.table->getRowPtr(ySrc + rowOffset + rowDirection*y));
			
					for(int x=0; x<width; x++)
					{
						char* dstPixel = dstRow + x*dstPixelSize;
						const char* srcPixel = srcRow + (columnOffset + columnDirection*x)*srcPixelSize;
						HdlTextureFormatDescriptor::applyBitShuffle(dstPixel, srcPixel, shuffle, length);
					}
				}
			}
		}
		else
		{
			const int srcPixelSize = src.descriptor.getPixelSize(src.getGLDepth()),
				  dstPixelSize = descriptor.getPixelSize(getGLDepth());

			const HdlTextureFormatDescriptor proxyDst = (table->isNormalized() ? HdlTextureFormatDescriptorsList::get(descriptor.aliasMode) : descriptor),
							 proxySrc = (src.table->isNormalized() ? HdlTextureFormatDescriptorsList::get(src.descriptor.aliasMode) : src.descriptor);
			const GLenum proxyDepthDst = (table->isNormalized() ? GL_UNSIGNED_INT : getGLDepth()),
				     proxyDepthSrc = (src.table->isNormalized() ? GL_UNSIGNED_INT : src.getGLDepth());

			bool isBlack = false;
			const int maxShuffleLength = 32;
			char shuffle[maxShuffleLength];
			const int length = HdlTextureFormatDescriptor::getBitShuffle(proxyDst, proxyDepthDst, proxySrc, proxyDepthSrc, shuffle, maxShuffleLength, &isBlack);

			// Shortcut : 
			if(isBlack)
			{
				for(int y=0; y<height; y++)
					std::memset(table->getRowPtr(yDst + y), 0, width*dstPixelSize);
			}
			else
			{
				unsigned int 	bufferIn[HdlTextureFormatDescriptor_MaxNumChannels],
						bufferOut[HdlTextureFormatDescriptor_MaxNumChannels];
				char*	intermediateIn = NULL;

				for(int y=0; y<height; y++)
				{
					char* dstRow = reinterpret_cast<char*>(table->getRowPtr(yDst + y));
					const char* srcRow = reinterpret_cast<const char*>(src.table->getRowPtr(ySrc + rowOffset + rowDirection*y));
			
					for(int x=0; x<width; x++)
					{
						char* dstPixel = dstRow + x*dstPixelSize;
						const char* srcPixel = srcRow + (columnOffset + columnDirection*x)*srcPixelSize;

						if(src.table->isNormalized())
						{
							const float* srcPixelFloat = reinterpret_cast<const float*>(srcPixel);

							for(int k=0; k<src.descriptor.numChannels; k++)
								bufferIn[k] = static_cast<unsigned int>(srcPixelFloat[k]*static_cast<float>(std::numeric_limits<unsigned int>::max()));

							intermediateIn = reinterpret_cast<char*>(bufferIn);
						}
						else
							intermediateIn = const_cast<char*>(srcPixel);

						if(!table->isNormalized())
							HdlTextureFormatDescriptor::applyBitShuffle(dstPixel, intermediateIn, shuffle, length);
						else
						{
							HdlTextureFormatDescriptor::applyBitShuffle(reinterpret_cast<char*>(bufferOut), intermediateIn, shuffle, length);
							
							for(int k=0; k<src.descriptor.numChannels; k++)
								reinterpret_cast<float*>(dstPixel)[k] = static_cast<float>(bufferOut[k])/static_cast<float>(std::numeric_limits<unsigned int>::max());
						}
					}
				}
			}
		}
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
			throw Exception("ImageBuffer::load - Cannot open file \"" + filename + "\" for reading.", __FILE__, __LINE__, Exception::ModuleException);
		}

		file.seekg(0, std::ios_base::end);
		size_t fileLength = file.tellg();

		if(fileLength<headerNumBytes)
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : header is to short.", __FILE__, __LINE__, Exception::ModuleException);

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
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the file is not a raw file (version 0).", __FILE__, __LINE__, Exception::ModuleException);
		}

		p += signature.size();

		// Get the sizes and other data : 
		int	width,
			height,
			alignment;
	
		width		= * reinterpret_cast<int*>(header + p);			p += sizeof(width);
		height		= * reinterpret_cast<int*>(header + p);			p += sizeof(height);
		alignment	= * reinterpret_cast<int*>(header + p);			p += sizeof(alignment);

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
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the comment embedded in the file is too long.", __FILE__, __LINE__, Exception::ModuleException);
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
		ImageBuffer* imageBuffer = new ImageBuffer(format, alignment);

		// Test remaining space in the file :
		size_t remainingBytes = fileLength - file.tellg();

		if(remainingBytes!=imageBuffer->getSize())
		{
			file.close();
			delete imageBuffer;
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the image length does not match expectation.", __FILE__, __LINE__, Exception::ModuleException);
		}

		// Else : load!
		file.read( reinterpret_cast<char*>(imageBuffer->getPtr()), remainingBytes);

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
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\" : the comment is too long (it cannot exceed " + toString(maxCommentLength) + " characters).", __FILE__, __LINE__, Exception::ModuleException);

		// Try to open for read : 
		std::fstream file;

		file.open( filename.c_str(), std::fstream::out | std::fstream::binary );

		if(!file.is_open())
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

		// Signature :
		file.write(headerSignature.c_str(), headerSignature.size());

		// Data :
		int	width		= getWidth(),
			height		= getHeight(),
			alignment	= getAlignment();
		GLenum	mode		= getGLMode(),
			depth		= getGLDepth(),
			minFilter	= getMinFilter(),
			magFilter	= getMagFilter(),
			sWrapping 	= getSWrapping(),
			tWrapping	= getTWrapping();
		int	minMipmap	= getBaseLevel(),
			maxMipmap	= getMaxLevel();

		file.write(	reinterpret_cast<char*>(&width), 	sizeof(width) );
		file.write(	reinterpret_cast<char*>(&height), 	sizeof(height) );
		file.write(	reinterpret_cast<char*>(&alignment), 	sizeof(alignment) );
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
		file.write( reinterpret_cast<const char*>(getPtr()), getSize());

		// Finally : 
		file.close();
	}

