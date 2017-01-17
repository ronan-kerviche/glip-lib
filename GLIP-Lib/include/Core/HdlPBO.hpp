/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : HdlPBO.hpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlPBO.hpp
 * \brief   OpenGL Pixel Buffer Object Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
*/

/* ************************************************************************************************************* */
/*                                                                                                               */
/*         Tips : UNPACK <-> UPLOAD   Data to   GPU                                                              */
/*                PACK   <-> DOWNLOAD Data from GPU                                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __HDLPBO_INCLUDE__
#define __HDLPBO_INCLUDE__

	// GL include
	#include "Core/LibTools.hpp"
        #include "Core/OglInclude.hpp"
	#include "Core/HdlGeBO.hpp"
	#include "Core/HdlTexture.hpp"

	namespace Glip
	{
		namespace CoreGL
		{

			// prototypes :
			class HdlFBO;

			// Pixel Buffer Object Handle
/**
\class HdlPBO
\brief Object handle for OpenGL Pixel Buffer Objects.

How to use a PBO to upload data to a texture :
\code
	HdlTextureFormat fmt(...);
	HdlTexture someTexture(fmt);

	HdlPBO pbo(fmt, GL_PIXEL_UNPACK_BUFFER_ARB, GL_STREAM_DRAW_ARB);

	unsigned char* ptr = reinterpret_cast<unsigned char*>(pbo.map());
	memcpy(ptr, yourBuffer, fmt.getSize());
	pbo.unmap();
	pbo.writeTexture(someTexture);
\endcode

For reading operations, use :
\code
	HdlTextureFormat fmt(...);
	HdlTexture someTexture(fmt);

	HdlPBO pbo(fmt, GL_PIXEL_PACK_BUFFER_ARB, GL_STREAM_READ_ARB);

	pbo.readTexture(someTexture);
	unsigned char* ptr = reinterpret_cast<unsigned char*>(pbo.map());
	memcpy(yourBuffer, ptr, fmt.getSize());
	pbo.unmap();
\endcode
**/
			class GLIP_API HdlPBO : public HdlGeBO
			{
				private :
					// Data
					int 	width,
						height,
						channelCount,
						channelSize,
						alignment;

					// No copy :
					HdlPBO(const HdlPBO&);
					const HdlPBO& operator=(const HdlPBO&);

				public :
					// Tools
					HdlPBO(const int& _width, const int& _height, const int& _channelCount, const int& _channelSize, const GLenum& aim, const GLenum& freq, const int& _alignment=1);
					HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq);
					~HdlPBO(void);

					int  getWidth(void) const;
					int  getHeight(void) const;
					int  getChannelCount(void) const;
					int  getChannelSize(void) const;
					void writeTexture(HdlTexture& texture, int oX=0, int oY=0, int w=-1, int h=-1, GLenum mode=GL_NONE, GLenum depth=GL_NONE);
					void readTexture(HdlTexture& texture);

					//Static tools
					static size_t computeSize(const int& _width, const int& _height, const int& _channelCount, const int& _channelSize, const int& _alignment=1);
					//static void unbind(GLenum target=0);
					//static void unmap(GLenum target=0);
			};
		}
	}

#endif

