/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlGeBO.hpp                                                                               */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlGeBO.hpp
 * \brief   OpenGL Generic handle for Buffer Objects (VBO, PBO, VAO, ...)
 * \author  R. KERVICHE
 * \version 0.6
 * \date    February 6th 2011
*/

#ifndef __HDLGEBO_INCLUDE__
#define __HDLGEBO_INCLUDE__

	// GL include
        #include "OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Buffer Object Handle
			/**
			\class HdlGeBO
			\brief Object handle for OpenGL Buffer Object (VBO, PBO, VAO, ...)
			**/
			class HdlGeBO
			{
				private :
					// Data
					GLuint 		bufferId;
					GLenum		buildTarget,
							buildUsage;
					int 		size;
					static bool 	binding[4],
					mapping[4];

					// No copy constructor
					HdlGeBO(const HdlGeBO&);

					// Tools
					static int getIDTarget(GLenum target);

				public :
					// Tools
					HdlGeBO(int _size, GLenum infoTarget, GLenum infoUsage);
					HdlGeBO(GLuint id, int _size, GLenum infoTarget, GLenum infoUsage);
					~HdlGeBO(void);

					int 	getSize(void);
					GLuint       	getID(void);
					GLenum       	getTarget(void);
					GLenum       	getUsage(void);
					void        	bind(GLenum target = GL_NONE);
					void*        	map(GLenum target, GLenum access);
					void		write(void* data);
					void		subWrite(void* data, int size, int offset);

					// Static tools
					static void     unbind(GLenum target);
					static void     unmap(GLenum target);
					static bool 	isBound(GLenum target);
					static bool 	isMapped(GLenum target);
			};

			/*
			TARGETS List
			GL_ARRAY_BUFFER_ARB
			GL_ELEMENT_ARRAY_BUFFER_ARB
			GL_PIXEL_UNPACK_BUFFER_ARB
			GL_PIXEL_PACK_BUFFER_ARB

			USAGE List
			GL_STATIC_DRAW_ARB
			GL_STATIC_READ_ARB
			GL_STATIC_COPY_ARB
			GL_DYNAMIC_DRAW_ARB
			GL_DYNAMIC_READ_ARB
			GL_DYNAMIC_COPY_ARB
			GL_STREAM_DRAW_ARB
			GL_STREAM_READ_ARB
			GL_STREAM_COPY_ARB

			ACCESS LIST
			GL_READ_ONLY_ARB
			GL_WRITE_ONLY_ARB
			GL_READ_WRITE_ARB
			*/
		}
	}

#endif

