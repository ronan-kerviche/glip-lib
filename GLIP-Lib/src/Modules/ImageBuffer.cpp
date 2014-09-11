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

	const unsigned int 	ImageBuffer::headerNumBytes 	= (8 + 4*3 + 4*6 + 4*2 + 4);	// See the load/write functions for more precisions (size * num elements).
	const unsigned int 	ImageBuffer::maxCommentLength	= 1048576;			// 1MB
	const std::string 	ImageBuffer::headerSignature 	= "GLIPRAW1";

	/**
	\fn ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format, int _alignment)
	\brief ImageBuffer constructor.
	\param format The format of this buffer (or the equivalent uncompressed format).
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(const __ReadOnly_HdlTextureFormat& format, int _alignment)
	 : 	__ReadOnly_HdlTextureFormat(format),
		descriptor(format.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.modeID==GL_FLOAT) || (descriptor.modeID==GL_DOUBLE);
		table = HdlDynamicTable::build(format.getGLDepth(), format.getWidth(), format.getHeight(), descriptor.numChannels(), normalized, _alignment);
	}

	/**
	\fn ImageBuffer::ImageBuffer(void* buffer, const __ReadOnly_HdlTextureFormat& format, int _alignment)
	\brief ImageBuffer proxy constructor.
	\param buffer The buffer to point to, this memory table will NOT be deleted when this object is destroyed.
	\param format The format of this buffer (or the equivalent uncompressed format).
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(void* buffer, const __ReadOnly_HdlTextureFormat& format, int _alignment)
	 : 	__ReadOnly_HdlTextureFormat(format),
		descriptor(format.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.modeID==GL_FLOAT) || (descriptor.modeID==GL_DOUBLE);
		table = HdlDynamicTable::buildProxy(buffer, format.getGLDepth(), format.getWidth(), format.getHeight(), descriptor.numChannels(), normalized, _alignment);
	}

	
	/**
	\fn ImageBuffer::ImageBuffer(HdlTexture& texture, int _alignment)	
	\brief ImageBuffer constructor.
	\param texture Copy the format and data of the this texture.
	\param _alignment Force the memory alignment to either 1, 4 or 8 bytes.
	**/
	ImageBuffer::ImageBuffer(HdlTexture& texture, int _alignment)
	 :	__ReadOnly_HdlTextureFormat(texture),
		descriptor(texture.getFormatDescriptor()),
		table(NULL)
	{
		setAlignment(_alignment);
		bool normalized = (descriptor.modeID==GL_FLOAT) || (descriptor.modeID==GL_DOUBLE);
		table = HdlDynamicTable::build(texture.getGLDepth(), texture.getWidth(), texture.getHeight(), descriptor.numChannels(), normalized, _alignment);

		// Copy : 
		(*this) << texture;
	}

	/**
	\fn ImageBuffer::ImageBuffer(const ImageBuffer& image)
	\brief ImageBuffer copy constructor.
	\param image Image buffer to copy.
	**/
	ImageBuffer::ImageBuffer(const ImageBuffer& image)
	 :	__ReadOnly_HdlTextureFormat(image),
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
			throw Exception("ImageBuffer::operator<< - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			// Bind ; 
			texture.bind();

			glPixelStorei(GL_PACK_ALIGNMENT, getAlignment());

			// Read : 
			glGetTexImage(GL_TEXTURE_2D, 0, descriptor.aliasMode, getGLDepth(), table->getPtr());
			
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
	\param bytes The buffer to copy (assumed to be of size __ReadOnly_HdlTextureFormat::getSize()).
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
			throw Exception("ImageBuffer::operator>> - Texture and ImageBuffer objects are incompatible.", __FILE__, __LINE__);
		else
		{
			// Bind : 
			texture.bind();

			glPixelStorei(GL_UNPACK_ALIGNMENT, getAlignment());

			// Write :
			glTexImage2D(GL_TEXTURE_2D, 0, getGLMode(), getWidth(), getHeight(), 0, descriptor.aliasMode, getGLDepth(), table->getPtr());

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
		std::memcpy(bytes, table->getPtr(), table->getSize());
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
		return table->getl(x, y, descriptor.channelIndex(channel));
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
		table->setl(value, x, y, descriptor.channelIndex(channel));
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
		return table->getNormalized(x, y, descriptor.channelIndex(channel));
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
		table->setNormalized(value, x, y, descriptor.channelIndex(channel));
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
		ImageBuffer* imageBuffer = new ImageBuffer(format, alignment);

		// Test remaining space in the file :
		size_t remainingBytes = fileLength - file.tellg();

		if(remainingBytes!=imageBuffer->getSize())
		{
			file.close();
			delete imageBuffer;
			throw Exception("ImageBuffer::load - Cannot read file \"" + filename + "\" : the image length does not match expectation.", __FILE__, __LINE__);
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
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\" : the comment is too long (it cannot exceed " + toString(maxCommentLength) + " characters).", __FILE__, __LINE__);

		// Try to open for read : 
		std::fstream file;

		file.open( filename.c_str(), std::fstream::out | std::fstream::binary );

		if(!file.is_open())
			throw Exception("ImageBuffer::write - Cannot write file \"" + filename + "\".", __FILE__, __LINE__);

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

// PixelIterator : 
	/**
	\fn PixelIterator::PixelIterator(ImageBuffer& _image)
	\brief Create an iterator over the pixels of an image. The ImageBuffer object must exist at all time. The iterator is initialized to the beginning of the image (top-left corner).
	\param _image The image to run over.
	**/
	PixelIterator::PixelIterator(ImageBuffer& _image)
	 : 	HdlDynamicTableIterator(_image.getTable()),
		image(_image)
	{ }

	/**
	\fn PixelIterator::PixelIterator(const PixelIterator& copy)
	\brief PixelIterator copy constructor.
	\param copy Original to copy.
	**/
	PixelIterator::PixelIterator(const PixelIterator& copy)
	 : 	HdlDynamicTableIterator(copy),
		image(copy.image)
	{ }

	PixelIterator::~PixelIterator(void)
	{ }

	/**
	\fn const ImageBuffer& PixelIterator::getImage(void) const
	\brief Acces the underlying image.
	\return Constant reference to the image.
	**/
	const ImageBuffer& PixelIterator::getImage(void) const
	{
		return image;
	}

	/**
	\fn ImageBuffer& PixelIterator::getImage(void)
	\brief Acces the underlying image.
	\return Reference to the image.
	**/
	ImageBuffer& PixelIterator::getImage(void)
	{
		return image;
	}
	
	/**
	\fn bool PixelIterator::isValid(void) const
	\brief Check if the iterator is still valid.
	\return True if the iterator is valid (can be used for I/O operation).
	**/
	bool PixelIterator::isValid(void) const
	{
		return HdlDynamicTableIterator::isValid();
	}

	/**
	\fn size_t PixelIterator::getPixelSize(void) const
	\brief Get the size of the pixel (all components) in bytes.
	\return The size of the pixels in bytes.
	**/
	size_t PixelIterator::getPixelSize(void) const
	{
		return HdlDynamicTableIterator::getTable().getSliceSize();
	}

	/**
	\fn int PixelIterator::getX(void) const
	\brief Get the current X coordinate.
	\return The current X coordinate.
	**/
	int PixelIterator::getX(void) const
	{
		return HdlDynamicTableIterator::getColumnIndex();
	}

	/**
	\fn int PixelIterator::getY(void) const
	\brief Get the current Y coordinate.
	\return The current Y coordinate.
	**/
	int PixelIterator::getY(void) const
	{
		return HdlDynamicTableIterator::getRowIndex();
	}

	/**
	\fn int PixelIterator::getDistanceToBottomBorder(void) const
	\brief Get the number of pixels between the current position and the bottom border (this pixel is included).
	\return Height minus current Y coordinate.
	**/
	int PixelIterator::getDistanceToBottomBorder(void) const
	{
		return HdlDynamicTableIterator::getDistanceToBottomBorder();
	}

	/**
	\fn int PixelIterator::getDistanceToRightBorder(void) const
	\brief Get the number of pixels between the current position and the right border (this pixel is included).
	\return Width minus current X coordinate.
	**/
	int PixelIterator::getDistanceToRightBorder(void) const
	{
		return HdlDynamicTableIterator::getDistanceToRightBorder();
	}

	/**
	\fn void PixelIterator::nextPixel(void)
	\brief Move to the next pixel.
	**/
	void PixelIterator::nextPixel(void)
	{
		HdlDynamicTableIterator::nextSlice();
	}

	/**
	\fn void PixelIterator::previousPixel(void)
	\brief Move to the previous pixel.
	**/
	void PixelIterator::previousPixel(void)
	{
		HdlDynamicTableIterator::previousSlice();
	}

	/**
	\fn void PixelIterator::nextLine(void)
	\brief Move to the next line.
	**/
	void PixelIterator::nextLine(void)
	{
		HdlDynamicTableIterator::nextRow();
	}

	/**
	\fn void PixelIterator::previousLine(void)
	\brief Move to the next line.
	**/
	void PixelIterator::previousLine(void)
	{
		HdlDynamicTableIterator::previousRow();
	}

	/**
	\fn void PixelIterator::lineBegin(void)
	\brief Move to the beginning of the line.
	**/
	void PixelIterator::lineBegin(void)
	{
		HdlDynamicTableIterator::rowBegin();
	}

	/**
	\fn void PixelIterator::lineEnd(void)
	\brief Move to the end of the line.
	**/
	void PixelIterator::lineEnd(void)
	{
		HdlDynamicTableIterator::rowEnd();
	}

	/**
	\fn void PixelIterator::imageBegin(void)
	\brief Move to the beginning of the image.
	**/
	void PixelIterator::imageBegin(void)
	{
		HdlDynamicTableIterator::tableBegin();
	}

	/**
	\fn void PixelIterator::imageEnd(void)
	\brief Move to the end of the image.
	**/
	void PixelIterator::imageEnd(void)
	{
		HdlDynamicTableIterator::tableEnd();
	}

	/**
	\fn void PixelIterator::jumpTo(const int& x, const int& y)
	\brief Jump to another position.
	\param x X coordinate.
	\param y Y coordinate.
	**/
	void PixelIterator::jumpTo(const int& x, const int& y)
	{
		HdlDynamicTableIterator::jumpTo(x, y, 0);
	}

	/**
	\fn const void* PixelIterator::getPtr(void) const
	\brief Get the current position in memory.
	\return Constant pointer to the current position in memory.
	**/
	const void* PixelIterator::getPtr(void) const
	{
		return HdlDynamicTableIterator::getPtr();
	}

	/**
	\fn void* PixelIterator::getPtr(void)
	\brief Get the current position in memory.
	\return Pointer to the current position in memory.
	**/
	void* PixelIterator::getPtr(void)
	{
		return HdlDynamicTableIterator::getPtr();
	}

	/**
	\fn float PixelIterator::readNormalized(const GLenum& channel) const
	\brief Read the given channel of the current pixel in a normalized range.
	\param channel The channel targeted
	\return The value in a normalized range or 0.0 if the channel is not registered.
	**/
	float PixelIterator::readNormalized(const GLenum& channel) const
	{
		int idx = image.getDescriptor().channelIndex(channel);

		if(idx<0)
			return 0.0f;
		else
			return HdlDynamicTableIterator::getTable().readNormalized(reinterpret_cast<const void*>(reinterpret_cast<const char*>(HdlDynamicTableIterator::getPtr()) + idx * HdlDynamicTableIterator::getTable().getElementSize())); 
	}

	/**
	\fn void PixelIterator::writeNormalized(const float& value, const GLenum& channel)
	\brief Write a normalized value in a the given channel. No action is performed if the channel is not registered.
	\param value The normalized value to write.
	\param channel The channel targeted.
	**/
	void PixelIterator::writeNormalized(const float& value, const GLenum& channel)
	{
		int idx = image.getDescriptor().channelIndex(channel);

		if(idx>=0)
			HdlDynamicTableIterator::getTable().writeNormalized(value, reinterpret_cast<void*>(reinterpret_cast<char*>(HdlDynamicTableIterator::getPtr()) + idx * HdlDynamicTableIterator::getTable().getElementSize()));
	}

	/**
	\fn void PixelIterator::writePixel(PixelIterator& it)
	\brief Write the value of another pixel iterator to the current position.
	\param it Another, valid, pixel iterator to copy value from.
	**/
	void PixelIterator::writePixel(PixelIterator& it)
	{
		float 	buffer[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
		// Read normalized and sort : 
		for(int k=0; k<it.image.getNumChannels(); k++)
		{
			int idxDest = image.getDescriptor().channelIndex(it.image.getDescriptor().channelAtIndex(k));

			if(idxDest>=0)
				buffer[idxDest] = it.HdlDynamicTableIterator::readNormalized();

			it.nextElement();
		}

		// Write
		for(int k=0; k<image.getNumChannels(); k++)
		{
			HdlDynamicTableIterator::writeNormalized(buffer[k]);
			nextElement();
		}

		sliceBegin();
		it.sliceBegin();
	}

		bool isPlatformLSB(void)
		{
			const unsigned int v = 0xFF000000;

			return (*reinterpret_cast<const char*>(&v)==0);
		}

		void memParse(char* dst, char* src, const char* indices, const int s)
		{
			for(int k=0; k<s; k++)
			{
				if(indices[k]>=0)
					dst[k] = src[indices[k]];
			}
		}

	/**
	\fn void PixelIterator::blit(PixelIterator& src, int maxWidth, int maxHeight)
	\brief Copy an area of src into the current pixel iterator (the current position of the iterator defines the upper-left corner).
	\param src The source to from.
	\param maxWidth The maximum width to copy.
	\param maxHeight The maximum height to copy.
	**/
	void PixelIterator::blit(PixelIterator& src, int maxWidth, int maxHeight)
	{
		bool 		destFloattingOrNormalized 	= (image.getTable().isNormalized() || image.getTable().isFloatingPointType()),
				srcFloattingOrNormalized	= (src.image.getTable().isNormalized() || src.image.getTable().isFloatingPointType());

		const int	w = (maxWidth<0) ? (std::min(src.getDistanceToRightBorder(), getDistanceToRightBorder())) : (std::min(src.getDistanceToRightBorder(), std::min(getDistanceToRightBorder(), maxWidth))),
				h = (maxHeight<0) ? (std::min(src.getDistanceToBottomBorder(),getDistanceToBottomBorder())) : (std::min(src.getDistanceToBottomBorder(), std::min(getDistanceToBottomBorder(), maxHeight)));

		const int 	dstOriginX	= getX(),
				dstOriginY	= getY(),
				srcOriginX	= src.getX(),
				srcOriginY	= src.getY();

		if(!destFloattingOrNormalized && !srcFloattingOrNormalized)
		{
			// Generate the shifting layout : 
			const int 	largestLayout 	= 256;
			char		indices[largestLayout];

			std::memset(reinterpret_cast<void*>(indices), -1, largestLayout);

			const int	sizeSrcComponent	= src.image.getTable().getElementSize(),
					sizeDstComponent	= image.getTable().getElementSize();

			if(isPlatformLSB())
			{
				for(int p=0; p<image.getNumChannels(); p++)
				{
					// Find where the current channel is in the source
					int target = src.image.getDescriptor().channelIndex( image.getDescriptor().channelAtIndex(p) );

					// If the current channel exists in source : 
					if(target>=0)
					{ 
						for(int q=std::min(0, sizeDstComponent-sizeSrcComponent); q<std::min(sizeSrcComponent, sizeDstComponent); q++)
							indices[p*sizeDstComponent+q] = target * sizeSrcComponent + q + std::min(0, sizeSrcComponent-sizeDstComponent);		
					}
				}
			}
			else // MSB platform : 	
			{
				for(int p=0; p<image.getNumChannels(); p++)
				{
					// Find where the current channel is in the source
					int target = src.image.getDescriptor().channelIndex( image.getDescriptor().channelAtIndex(p) );

					// If the current channel exists in source : 
					if(target>=0)
					{ 
						for(int q=0; q<std::min(sizeSrcComponent, sizeDstComponent); q++)
							indices[p*sizeDstComponent+q] = target * sizeSrcComponent + q;		
					}
				}
			}

			// Prepare to copy : 
			char 	bufferSrc[largestLayout],
				bufferDst[largestLayout];

			std::memset(reinterpret_cast<void*>(bufferSrc), 0, largestLayout);
			std::memset(reinterpret_cast<void*>(bufferDst), 0, largestLayout);

			for(int y=0; y<h; y++)
			{
				for(int x=0; x<w; x++)
				{
					// Copy the current pixel : 
					std::memcpy(reinterpret_cast<void*>(bufferSrc), src.getPtr(), src.getPixelSize());

					// Shift and adapt : 
					memParse(reinterpret_cast<char*>(bufferDst), reinterpret_cast<char*>(bufferSrc), indices, getPixelSize());
					
					// Copy the result : 
					std::memcpy(getPtr(), reinterpret_cast<void*>(bufferDst), getPixelSize());
				
					// Move to the next pixel : 
					src.nextPixel();
					nextPixel();
				}
				
				// Move back in the window : 
				src.jumpTo(srcOriginX, srcOriginY+y+1);
				jumpTo(dstOriginX, dstOriginY+y+1);
			}
		}
		else if(destFloattingOrNormalized && !srcFloattingOrNormalized)
		{
			throw Exception("PixelIterator::blit - Unable to copy from and/or to normalized data.", __FILE__, __LINE__);
			/*const int maxChannels = 4;
			char indices[maxChannels];

			std::memset(reinterpret_cast<void*>(indices), -1, maxChannels);

			for(int p=0; p<image.getNumChannels(); p++)
				indices[p] = src.image.getDescriptor().channelIndex( image.getDescriptor().channelAtIndex(p) );

			float buffer[maxChannels];

			for(int y=0; y<h; y++)
			{
				for(int x=0; x<w; x++)
				{
					for(int k=0; k<image.getNumChannels(); k++)
						
				}
			}*/
		}
		else if(!destFloattingOrNormalized && srcFloattingOrNormalized)
		{
			throw Exception("PixelIterator::blit - Unable to copy from and/or to normalized data.", __FILE__, __LINE__);
		}
		else // destFloattingOrNormalized && srcFloattingOrNormalized
		{
			throw Exception("PixelIterator::blit - Unable to copy from and/or to normalized data.", __FILE__, __LINE__);
		}
	}

