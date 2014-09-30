/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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

	memcpy(ptr, yourBuffer, fmt.getNumPixels()*fmt.getNumChannels());

	HdlPBO::unmap(GL_PIXEL_UNPACK_BUFFER_ARB);

	pbo.copyToTexture(someTexture);
\endcode

For reading operations, use Glip::Modules::PBOTextureReader.
**/
			class GLIP_API HdlPBO : public HdlGeBO
			{
				private :
					// Data
					int w, h, c, cs;

				public :
					// Tools
					HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq, int size=-1);
					HdlPBO(const HdlAbstractTextureFormat& fmt, GLenum aim, GLenum freq);
					~HdlPBO(void);

					int  getWidth(void);
					int  getHeight(void);
					int  getChannelCount(void);
					int  getChannelSize(void);
					void copyToTexture(HdlTexture& texture, int oX=0, int oY=0, int w=-1, int h=-1, GLenum mode=GL_NONE, GLenum depth=GL_NONE);
					void bindAsPack(void);
					void bindAsUnpack(void);

					//Static tools
					static void unbind(GLenum target=0);
					static void unmap(GLenum target=0);
			};
		}
	}

#endif

