/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
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
	\fn HdlPBO::HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq, int size)
	\brief HdlPBO constructor.
	\param _w The width of the buffer.
	\param _h The height of the buffer.
	\param _c The number of channels.
	\param _cs The size of each channel (in bytes).
	\param aim The target (GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB).
	\param freq The frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	\param size The size of the buffer, computed by default with previous parameters.
	**/
	HdlPBO::HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq, int size)
	 : HdlGeBO((size<0)?(_w*_h*_c*_cs):size, aim, freq), w(_w), h(_h), c(_c), cs(_cs)
	{ }

	/**
	\fn HdlPBO::HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq)
	\brief HdlPBO constructor.
	\param fmt The format to copy, this must an uncompressed format.
	\param aim The target (GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB).
	\param freq The frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	**/
	HdlPBO::HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq)
	 : HdlGeBO(fmt.getSize(), aim, freq), w(fmt.getWidth()), h(fmt.getHeight()), c(fmt.getNumChannels()), cs(fmt.getChannelDepth())
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
	int HdlPBO::getWidth(void)        { return w;  }
	int HdlPBO::getHeight(void)       { return h;  }
	int HdlPBO::getChannelCount(void) { return c;  }
	int HdlPBO::getChannelSize(void)  { return cs; }

	/**
	\fn void HdlPBO::copyToTexture(HdlTexture& texture, int oX, int oY, int w, int h, GLenum mode, GLenum depth)
	\param texture The target texture.
	\param oX The offset in X direction for the destination target, default is 0.
	\param oY The offset in Y direction for the destination target, default is 0.
	\param w The width to be copied, default is the width of texture.
	\param h The height to be copied, default is the height of texture.
	\param mode The channel layout of the input data, default is the mode of texture.
	\param depth The depth of the input data, default is the depth of texture.
	**/
	void HdlPBO::copyToTexture(HdlTexture& texture, int oX, int oY, int w, int h, GLenum mode, GLenum depth)
	{
		if(mode==GL_NONE) 	mode = texture.getGLMode();
		if(depth==GL_NONE) 	depth = texture.getGLDepth();
		if(w<=0) 		w = texture.getWidth();
		if(h<=0) 		h = texture.getHeight();

		texture.bind();
		bind(GL_PIXEL_UNPACK_BUFFER_ARB);

		glTexSubImage2D(GL_TEXTURE_2D, 0, oX, oY, w, h, mode, depth, 0);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlPBO::copyToTexture", "glTexSubImage2D()")
		#endif

		#ifdef __VERBOSE__
			if(glErrors(true, false))
				if(texture.isCompressed())
					throw Exception("HdlPBO::copyToTexture - Writing into a compressed texture from a PBO : make sure that the data in the PBO is compressed too.", __FILE__, __LINE__, Exception::GLException);
				else
					throw Exception("HdlPBO::copyToTexture - You must write at least in the target texture before using this function.", __FILE__, __LINE__, Exception::GLException);
		#endif

		// Unbind from target :
		unbind(GL_PIXEL_UNPACK_BUFFER_ARB);
	}

	/**
	\fn void HdlPBO::bindAsPack(void)
	\brief Bind PBO to GL_PIXEL_PACK_BUFFER, to gather data from a buffer or a texture.
	**/
	void HdlPBO::bindAsPack(void)
	{
		bind(GL_PIXEL_PACK_BUFFER_ARB);
	}

	/**
	\fn void HdlPBO::bindAsUnpack(void)
	\brief Bind PBO to GL_PIXEL_UNPACK_BUFFER, to send data to a buffer or a texture.
	**/
	void HdlPBO::bindAsUnpack(void)
	{
		bind(GL_PIXEL_UNPACK_BUFFER_ARB);
	}

//Other tools
	/**
	\fn void HdlPBO::unbind(GLenum target)
	\brief Unbind the PBO bound to target.
	\param target The target binding point.
	**/
	void HdlPBO::unbind(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unbind(GL_PIXEL_UNPACK_BUFFER_ARB);
			HdlGeBO::unbind(GL_PIXEL_PACK_BUFFER_ARB  );
		}
		else
			HdlGeBO::unbind(target);
	}

	/**
	\fn void HdlPBO::unmap(GLenum target)
	\brief Unmap the PBO mapped from/to target.
	\param target The target binding point.
	**/
	void HdlPBO::unmap(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unmap(GL_PIXEL_UNPACK_BUFFER_ARB);
			HdlGeBO::unmap(GL_PIXEL_PACK_BUFFER_ARB  );
		}
		else
			HdlGeBO::unmap(target);
	}
