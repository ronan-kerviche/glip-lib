/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : OglTools.hpp                                                                              */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : OpenGL includes and tools                                                                 */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    OglTools.hpp
 * \brief   OpenGL includes and tools
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

#include "OglInclude.hpp"
#include "HdlTexture.hpp"
#include <string>

using namespace Glip::CoreGL;

// Structure
    // Data
        bool HandleOpenGL::initDone = false;

    // Functions
        void HandleOpenGL::init(void)
        {
            if(!initDone)
            {
                glewInit();
                initDone = true;

		// Init Multitexturing if not done yet...
		//HdlMultiTexturing::init();
            }
        }

// Errors Monitoring
	bool Glip::CoreGL::glErrors(bool verbose, bool quietSituations)
	{
		GLenum err = glGetError();

		if(verbose)
		{
			#define Err( errorcode, message) case errorcode: std::cout << "OpenGL error : " << message << std::endl; break;
			switch(err)
			{
				Err( GL_INVALID_ENUM,				"Invalid Enum")
				Err( GL_INVALID_VALUE,				"Invalid Value")
				Err( GL_INVALID_OPERATION, 			"Invalid Operation")
				Err( GL_STACK_OVERFLOW,				"Stack OverFlow")
				Err( GL_STACK_UNDERFLOW,			"Stack UnderFlow")
				Err( GL_OUT_OF_MEMORY,				"Out of Memory")
				Err( GL_TABLE_TOO_LARGE,			"Table too large")
				Err( GL_INVALID_FRAMEBUFFER_OPERATION_EXT,	"Invalid framebuffer operation - Don't forget to write something once in the attached texture! (in the init)")

				case GL_NO_ERROR : if(!quietSituations){ std::cout << "OpenGL error : No error recorded" << std::endl; } break;
				default          :                       std::cout << "OpenGL error : Error NOT recognized (Code : " << err << ')' << std::endl; break;
			}
		}

		return err!=GL_NO_ERROR;
	}

	void Glip::CoreGL::glDebug(void)
	{
		GLint param;

		std::cout << "OpenGL DEBUG Tool" << std::endl;

		// Test if a texture is bound
		glGetIntegerv(GL_TEXTURE_BINDING_1D, &param);
		if(param!=0) std::cout << "  Binding - Texture 1D     : " << param << std::endl;

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &param);
		if(param!=0) std::cout << "  Binding - Texture 2D     : " << param << std::endl;

		glGetIntegerv(GL_TEXTURE_BINDING_3D, &param);
		if(param!=0) std::cout << "  Binding - Texture 3D     : " << param << std::endl;

		// Test if a Buffer is bound
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &param);
		if(param!=0) std::cout << "  Binding - Buffer array   : " << param << std::endl;

		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &param);
		if(param!=0) std::cout << "  Binding - Element array  : " << param << std::endl;

		glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &param);
		if(param!=0) std::cout << "  Binding - PACK buffer    : " << param << std::endl;

		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &param);
		if(param!=0) std::cout << "  Binding - UNPACK buffer  : " << param << std::endl;

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &param);
		if(param!=0) std::cout << "  Binding - Frame buffer   : " << param << std::endl;

		glGetIntegerv(GL_TEXTURE_BUFFER_EXT, &param);
		if(param!=0) std::cout << "  Binding - Texture buffer : " << param << std::endl;

		// Shaders
		glGetIntegerv(GL_CURRENT_PROGRAM, &param);
		if(param!=0) std::cout << "  Binding - program        : " << param << std::endl;

		std::cout << "End DEBUG" << std::endl;
	}

	// OpenGL - Parameters Naming :
	std::string Glip::CoreGL::glParamName(GLenum param)
	{
		#define NMTOOL(X) case X: return #X;
		switch(param)
		{
			// Geometry
			NMTOOL( GL_POINTS )
			NMTOOL( GL_LINES )
			NMTOOL( GL_LINE_STRIP )
			NMTOOL( GL_LINE_LOOP )
			NMTOOL( GL_TRIANGLES )
			NMTOOL( GL_TRIANGLE_STRIP )
			NMTOOL( GL_TRIANGLE_FAN )
			NMTOOL( GL_QUADS )
			NMTOOL( GL_QUAD_STRIP )
			NMTOOL( GL_POLYGON )

			// Texture Mode
			NMTOOL( GL_TEXTURE_1D )
			NMTOOL( GL_TEXTURE_2D )
			NMTOOL( GL_TEXTURE_3D )
			NMTOOL( GL_TEXTURE_CUBE_MAP_POSITIVE_X )
			NMTOOL( GL_TEXTURE_CUBE_MAP_NEGATIVE_X )
			NMTOOL( GL_TEXTURE_CUBE_MAP_POSITIVE_Y )
			NMTOOL( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y )
			NMTOOL( GL_TEXTURE_CUBE_MAP_POSITIVE_Z )

			// Color Channel Format
			NMTOOL( GL_RED )
			NMTOOL( GL_GREEN )
			NMTOOL( GL_BLUE )
			NMTOOL( GL_ALPHA )
			NMTOOL( GL_RGB )
			NMTOOL( GL_BGR )
			NMTOOL( GL_RGBA )
			NMTOOL( GL_BGRA )
			NMTOOL( GL_LUMINANCE )
			NMTOOL( GL_LUMINANCE_ALPHA )
			NMTOOL( GL_RGB32F_ARB )

			// Type
			NMTOOL( GL_UNSIGNED_BYTE )
			NMTOOL( GL_BYTE )
			NMTOOL( GL_UNSIGNED_SHORT )
			NMTOOL( GL_SHORT )
			NMTOOL( GL_UNSIGNED_INT )
			NMTOOL( GL_INT )
			NMTOOL( GL_FLOAT )
			NMTOOL( GL_UNSIGNED_BYTE_3_3_2 )
			NMTOOL( GL_UNSIGNED_BYTE_2_3_3_REV )
			NMTOOL( GL_UNSIGNED_SHORT_5_6_5 )
			NMTOOL( GL_UNSIGNED_SHORT_5_6_5_REV )
			NMTOOL( GL_UNSIGNED_SHORT_4_4_4_4 )
			NMTOOL( GL_UNSIGNED_SHORT_4_4_4_4_REV )
			NMTOOL( GL_UNSIGNED_SHORT_5_5_5_1 )
			NMTOOL( GL_UNSIGNED_SHORT_1_5_5_5_REV )
			NMTOOL( GL_UNSIGNED_INT_8_8_8_8 )
			NMTOOL( GL_UNSIGNED_INT_8_8_8_8_REV )
			NMTOOL( GL_UNSIGNED_INT_10_10_10_2 )
			NMTOOL( GL_UNSIGNED_INT_2_10_10_10_REV )

			// Texture filtering
			NMTOOL( GL_TEXTURE_MIN_FILTER )
			NMTOOL( GL_TEXTURE_MAG_FILTER )
			NMTOOL( GL_TEXTURE_MIN_LOD )
			NMTOOL( GL_TEXTURE_MAX_LOD )
			NMTOOL( GL_TEXTURE_BASE_LEVEL )
			NMTOOL( GL_TEXTURE_MAX_LEVEL )
			NMTOOL( GL_TEXTURE_WRAP_S )
			NMTOOL( GL_TEXTURE_WRAP_T )
			NMTOOL( GL_TEXTURE_WRAP_R )
			NMTOOL( GL_CLAMP )
			NMTOOL( GL_CLAMP_TO_EDGE )
			NMTOOL( GL_REPEAT )
			NMTOOL( GL_TEXTURE_PRIORITY )
			NMTOOL( GL_LINEAR )
			NMTOOL( GL_NEAREST )
			NMTOOL( GL_NEAREST_MIPMAP_NEAREST )
			NMTOOL( GL_LINEAR_MIPMAP_NEAREST )
			NMTOOL( GL_NEAREST_MIPMAP_LINEAR )
			NMTOOL( GL_LINEAR_MIPMAP_LINEAR )

			// Errors
			NMTOOL( GL_INVALID_ENUM )
			NMTOOL( GL_INVALID_VALUE )
			NMTOOL( GL_INVALID_OPERATION )
			NMTOOL( GL_STACK_OVERFLOW )
			NMTOOL( GL_STACK_UNDERFLOW )
			NMTOOL( GL_OUT_OF_MEMORY )
			NMTOOL( GL_TABLE_TOO_LARGE )
			NMTOOL( GL_INVALID_FRAMEBUFFER_OPERATION_EXT )

			// GeBO packing
			NMTOOL( GL_PIXEL_PACK_BUFFER )
			NMTOOL( GL_PIXEL_UNPACK_BUFFER )
			NMTOOL( GL_ARRAY_BUFFER_ARB )
			NMTOOL( GL_ELEMENT_ARRAY_BUFFER_ARB )
			NMTOOL( GL_READ_ONLY_ARB )
			NMTOOL( GL_WRITE_ONLY_ARB )
			NMTOOL( GL_READ_WRITE_ARB )

			NMTOOL( GL_STATIC_DRAW_ARB )
			NMTOOL( GL_STATIC_READ_ARB )
			NMTOOL( GL_STATIC_COPY_ARB )
			NMTOOL( GL_DYNAMIC_DRAW_ARB )
			NMTOOL( GL_DYNAMIC_READ_ARB )
			NMTOOL( GL_DYNAMIC_COPY_ARB )
			NMTOOL( GL_STREAM_DRAW_ARB )
			NMTOOL( GL_STREAM_READ_ARB )
			NMTOOL( GL_STREAM_COPY_ARB )

			NMTOOL( GL_INDEX_ARRAY )
			NMTOOL( GL_TEXTURE_COORD_ARRAY )

			// Default
			default : return "(OpenGL constant not recognized)";
		}
		#undef NMTOOL
	}

