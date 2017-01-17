/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
 * \date    February 6th 2011
*/

#ifndef __HDLGEBO_INCLUDE__
#define __HDLGEBO_INCLUDE__

	// GL include
	#include "Core/LibTools.hpp"
        #include "Core/OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Buffer Object Handle
			/**
			\class HdlGeBO
			\brief Object handle for OpenGL Buffer Object (VBO, PBO, VAO, ...).
			**/
			class GLIP_API HdlGeBO
			{
				private :
					// Data :
					GLuint 			bufferId;
					const GLenum		target,
								usage;
					const GLsizeiptr 	size;

					// No copy constructor :
					HdlGeBO(const HdlGeBO&);

					// Tools :
					static int getIDTarget(GLenum target);

				public :
					// Tools :
					HdlGeBO(const GLsizeiptr& _size, const GLenum& _target, const GLenum& _usage);
					HdlGeBO(const GLuint& id, const GLsizeiptr& _size, const GLenum& _target, const GLenum& _usage);
					~HdlGeBO(void);

					GLsizeiptr getSize(void) const;
					GLuint getID(void) const;
					GLenum getTarget(void) const;
					GLenum getUsage(void) const;
					void bind(GLenum tgt=GL_NONE);
					void unbind(GLenum tgt=GL_NONE);
					void* map(GLenum access = GL_NONE, GLenum tgt=GL_NONE);
					void unmap(GLenum tgt=GL_NONE);
					void write(const void* data);
					void subWrite(const void* data, GLsizeiptr size, GLintptr offset);

					// Static tools
					static void unbindAll(void);
					static void unmapAll(void);
					/*static bool isBound(const GLenum& tgt);
					static bool isMapped(const GLenum& tgt);*/
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

