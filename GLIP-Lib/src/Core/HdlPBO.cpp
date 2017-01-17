/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : HdlPBO.cpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlPBO.cpp
 * \brief   OpenGL Pixel Buffer Object Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
*/

#include "Core/Exception.hpp"
#include "Core/HdlPBO.hpp"

using namespace Glip::CoreGL;

// Tools
	/**
	\fn HdlPBO::HdlPBO(const int& _width, const int& _height, const int& _channelCount, const int& _channelSize, const GLenum& aim, const GLenum& freq, const int& _alignment)
	\brief HdlPBO constructor.
	\param _width The width of the buffer.
	\param _height The height of the buffer.
	\param _channelCount The number of channels.
	\param _channelSize The size of each channel (in bytes).
	\param aim The target (GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_PACK_BUFFER).
	\param freq The frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	\param _alignment The memory alignment of the rows, must be 1, 2, 4 or 8.
	**/
	HdlPBO::HdlPBO(const int& _width, const int& _height, const int& _channelCount, const int& _channelSize, const GLenum& aim, const GLenum& freq, const int& _alignment)
	 : 	HdlGeBO(computeSize(_width, _height, _channelCount, _channelSize, _alignment), aim, freq),
		width(_width),
		height(_height),
		channelCount(_channelCount),
		channelSize(_channelSize),
		alignment(_alignment)
	{ }

	/**
	\fn HdlPBO::HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq)
	\brief HdlPBO constructor.
	\param fmt The format to copy, this must an uncompressed format.
	\param aim The target (GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_PACK_BUFFER).
	\param freq The frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	**/
	HdlPBO::HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq)
	 : 	HdlGeBO(computeSize(fmt.getWidth(), fmt.getHeight(), fmt.getNumChannels(), fmt.getChannelDepth(), fmt.getAlignment()), aim, freq), 
		width(fmt.getWidth()),
		height(fmt.getHeight()),
		channelCount(fmt.getNumChannels()),
		channelSize(fmt.getChannelDepth()),
		alignment(fmt.getAlignment())
	{
		if(fmt.isCompressed())
			throw Exception("HdlPBO::HdlPBO - Can not create a PBO on HdlAbstractTextureFormat object for a compressed texture (size can not be obtained).", __FILE__, __LINE__, Exception::GLException);
	}

	HdlPBO::~HdlPBO(void)
	{ }

	/**
	\fn int HdlPBO::getWidth(void)
	\return The width of the buffer.
	\fn int HdlPBO::getHeight(void)
	\return The height of the buffer.
	\fn int HdlPBO::getChannelCount(void)
	\return The number of channels of the buffer.
	\fn int HdlPBO::getChannelSize(void)
	\return The channel size (in bytes) of the buffer.
	**/
	int HdlPBO::getWidth(void) const	{ return width;  }
	int HdlPBO::getHeight(void) const	{ return height;  }
	int HdlPBO::getChannelCount(void) const	{ return channelCount;  }
	int HdlPBO::getChannelSize(void)  const	{ return channelSize; }

	/**
	\fn void HdlPBO::writeTexture(HdlTexture& texture, int oX, int oY, int w, int h, GLenum mode, GLenum depth)
	\brief Write texture. No fitness test is performed here.
	\param texture The destination texture.
	\param oX The offset in X direction for the destination target, default is 0.
	\param oY The offset in Y direction for the destination target, default is 0.
	\param w The width to be copied, default is the width of texture.
	\param h The height to be copied, default is the height of texture.
	\param mode The channel layout of the input data, default is the mode of texture.
	\param depth The depth of the input data, default is the depth of texture.
	**/
	void HdlPBO::writeTexture(HdlTexture& texture, int oX, int oY, int w, int h, GLenum mode, GLenum depth)
	{
		if(mode==GL_NONE) 	mode = texture.getGLMode();
		if(depth==GL_NONE) 	depth = texture.getGLDepth();
		if(w<=0) 		w = texture.getWidth();
		if(h<=0) 		h = texture.getHeight();

		glPixelStorei(GL_UNPACK_ALIGNMENT, texture.getAlignment());
		texture.bind();
		bind(GL_PIXEL_UNPACK_BUFFER);
		glTexSubImage2D(GL_TEXTURE_2D, 0, oX, oY, w, h, mode, depth, 0);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlPBO::copyToTexture", "glTexSubImage2D()")
		#endif

		#ifdef __VERBOSE__
			if(glErrors(true, false))
				if(texture.isCompressed())
					throw Exception("HdlPBO::writeTexture - Writing into a compressed texture from a PBO : make sure that the data in the PBO is compressed too.", __FILE__, __LINE__, Exception::GLException);
				else
					throw Exception("HdlPBO::writeTexture - You must write at least in the target texture before using this function.", __FILE__, __LINE__, Exception::GLException);
		#endif

		// Unbind from target :
		unbind(GL_PIXEL_UNPACK_BUFFER);
	}

	/**
	\fn void HdlPBO::readTexture(HdlTexture& texture)
	\brief Read texture. No fitness test is performed here.
	\param texture The source texture.
	**/
	void HdlPBO::readTexture(HdlTexture& texture)
	{
		glPixelStorei(GL_PACK_ALIGNMENT, texture.getAlignment());
		texture.bind();
		bind(GL_PIXEL_PACK_BUFFER);
		glGetTexImage(GL_TEXTURE_2D, 0, texture.getGLMode(), texture.getGLDepth(), 0);
		
		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlPBO::readTexture", "glGetTexImage()")
		#endif

		#ifdef __VERBOSE__
			if(glErrors(true, false))
				if(texture.isCompressed())
					throw Exception("HdlPBO::readTexture - Reading a compressed texture from a PBO : make sure that the data in the PBO is compressed too.", __FILE__, __LINE__, Exception::GLException);
				else
					throw Exception("HdlPBO::readToTexture - You must write at least in the target texture before using this function.", __FILE__, __LINE__, Exception::GLException);
		#endif

		// Unbind from target :
		unbind(GL_PIXEL_PACK_BUFFER);
	}

//Other tools :
	size_t HdlPBO::computeSize(const int& _width, const int& _height, const int& _channelCount, const int& _channelSize, const int& _alignment)
	{
		const size_t 	rowSize = static_cast<size_t>(_width)*static_cast<size_t>(_channelCount)*static_cast<size_t>(_channelSize),
				paddedRowSize = rowSize + ((static_cast<size_t>(_alignment) - (rowSize & (static_cast<size_t>(_alignment)-1))) & (static_cast<size_t>(_alignment-1)));
		return static_cast<size_t>(_height)*paddedRowSize;
	}

	/**
	\fn void HdlPBO::unbind(GLenum target)
	\brief Unbind the PBO bound to target.
	\param target The target binding point.
	**
	void HdlPBO::unbind(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unbind(GL_PIXEL_UNPACK_BUFFER);
			HdlGeBO::unbind(GL_PIXEL_PACK_BUFFER);
		}
		else
			HdlGeBO::unbind(target);
	}

	**
	\fn void HdlPBO::unmap(GLenum target)
	\brief Unmap the PBO mapped from/to target.
	\param target The target binding point.
	**
	void HdlPBO::unmap(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unmap(GL_PIXEL_UNPACK_BUFFER);
			HdlGeBO::unmap(GL_PIXEL_PACK_BUFFER);
		}
		else
			HdlGeBO::unmap(target);
	}*/

