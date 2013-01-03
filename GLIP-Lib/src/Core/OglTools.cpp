/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : OglTools.cpp                                                                              */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : OpenGL includes and tools                                                                 */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    OglTools.cpp
 * \brief   OpenGL includes and tools
 * \author  R. KERVICHE
 * \date    October 17th 2010
**/

#include "OglInclude.hpp"
#include "HdlTexture.hpp"
#include "Exception.hpp"
#include <string>

using namespace Glip::CoreGL;

// Structure
	// Data
		bool 				HandleOpenGL::initDone 	= false;
		HandleOpenGL::SupportedVendor 	HandleOpenGL::vendor 	= vd_UNKNOWN;

	// Functions
		/**
		\fn void HandleOpenGL::init(void)
		\brief Initialize Glew and other tools for the OpenGL state machine. You have to call it before functions using OpenGL.
		**/
		void HandleOpenGL::init(void)
		{
			if(!initDone)
			{
				GLenum err = glewInit();
				if(err != GLEW_OK)
				{
					std::string error = reinterpret_cast<const char*>(glewGetErrorString(err));
					throw Exception("HandleOpenGL::init - Failed to init GLEW with the following error : " + error, __FILE__, __LINE__);
				}

				// Update vendor :
				std::string name = HandleOpenGL::getVendorName();

				if(name.find("NVIDIA")!=std::string::npos)
				    vendor = vd_NVIDIA;
				else if(name.find("ATI")!=std::string::npos || name.find("AMD")!=std::string::npos)
				    vendor = vd_AMDATI;
				else if(name.find("Intel")!=std::string::npos)
				    vendor = vd_INTEL;
				else
				    vendor = vd_UNKNOWN;

				initDone = true;
			}
		}

		/**
		\fn HandleOpenGL::SupportedVendor HandleOpenGL::getVendorID(void)
		\return The ID (HandleOpenGL::SupportedVendor) of the Hardware vendor for this platform.
		**/
		HandleOpenGL::SupportedVendor HandleOpenGL::getVendorID(void)
		{
		    return vendor;
		}

		/**
		\fn std::string HandleOpenGL::getVendorName(void)
		\return A std::string object containing the vendor name.
		**/
		std::string HandleOpenGL::getVendorName(void)
		{
			const GLubyte* str = glGetString(GL_VENDOR);
			return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::string HandleOpenGL::getRendererName(void)
		\return A std::string object containing the renderer name.
		**/
		std::string HandleOpenGL::getRendererName(void)
		{
			const GLubyte* str = glGetString(GL_RENDERER);
			return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::string HandleOpenGL::getVersion(void)
		\return A std::string object containing the OpenGL version.
		**/
		std::string HandleOpenGL::getVersion(void)
		{
			const GLubyte* str = glGetString(GL_VERSION);
			return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::string HandleOpenGL::getGLSLVersion(void)
		\return A std::string object containing the GLSL version.
		**/
		std::string HandleOpenGL::getGLSLVersion(void)
		{
			if(GL_VERSION_2_0)
			{
				const GLubyte* str = glGetString(GL_SHADING_LANGUAGE_VERSION);
				return std::string(reinterpret_cast<const char*>(str));
			}
			else
				return std::string("<Error : OpenGL version is less than 2.0>");
		}

// Errors Monitoring
	/**
	\fn std::string Glip::CoreGL::glErrorToString(bool* caughtError = NULL)
	\brief Get a string corresponding to the last error raised by OpenGL.
	\param caughtError Pointer to a boolean, will be set to true if an error is caught, or false otherwise.
	\return A standard string containing human readable information.
	**/
	std::string Glip::CoreGL::glErrorToString(bool* caughtError)
	{
		GLenum err = glGetError();

		if(caughtError!=NULL)
			(*caughtError) = err!=GL_NO_ERROR;

		#define Err( errorcode, message) case errorcode: return message ;
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
			Err( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,	"Incomplete attachment for framebuffer - Most likely incompatibility with texture format")
			Err( GL_FRAMEBUFFER_UNSUPPORTED,		"Unsupported framebuffer - Likely incompatibility with texture format")
			case GL_NO_ERROR : return "(No error)";
			default          : return "Unknown error (Code : " + to_string(err) + ")";
		}
		#undef Err
	}

	/**
	\fn bool Glip::CoreGL::glErrors(bool verbose, bool quietSituations)
	\brief Check for errors returned by OpenGL.
	\param verbose If set to true, the error will be output on std::cout.
	\param quietSituations If set to true, no print will be sent on std::cout in case there is no error.
	\return true if an error occured.
	**/
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
				Err( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,	"Incomplete attachment for framebuffer - Most likely incompatibility with texture format")
				Err( GL_FRAMEBUFFER_UNSUPPORTED,		"Unsupported framebuffer - Likely incompatibility with texture format")

				case GL_NO_ERROR : if(!quietSituations){ std::cout << "OpenGL error : No error recorded" << std::endl; } break;
				default          :                       std::cout << "OpenGL error : Error NOT recognized (Code : " << err << ')' << std::endl; break;
			}
			#undef Err
		}

		return err!=GL_NO_ERROR;
	}

	/**
	\fn void Glip::CoreGL::glDebug(void)
	\brief Print the current binding points state on std::cout (GL_TEXTURE_BINDING_1D, GL_TEXTURE_BINDING_2D, GL_TEXTURE_BINDING_3D, GL_ARRAY_BUFFER_BINDING, GL_ELEMENT_ARRAY_BUFFER_BINDING, GL_PIXEL_PACK_BUFFER_BINDING, GL_PIXEL_UNPACK_BUFFER_BINDING, GL_FRAMEBUFFER_BINDING, GL_TEXTURE_BUFFER_EXT, GL_CURRENT_PROGRAM).
	**/
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
	/**
	\fn std::string Glip::CoreGL::glParamName(GLenum param)
	\brief Get the parameter name as a string.
	\param param The GLenum parameter.
	\return A standard string.
	**/
	std::string Glip::CoreGL::glParamName(GLenum param)
	{
		#define NMTOOL(X) case X: return #X;
		switch(param)
		{
			// Geometry
			case GL_POINTS: return "GL_POINTS/GL_ZERO/GL_NONE";
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
			NMTOOL( GL_ALPHA4 )
			NMTOOL( GL_ALPHA8 )
			NMTOOL( GL_ALPHA8_SNORM )
			NMTOOL( GL_ALPHA12 )
			NMTOOL( GL_ALPHA16 )
			NMTOOL( GL_ALPHA16F_ARB )
			NMTOOL( GL_ALPHA16_SNORM )
			NMTOOL( GL_ALPHA32F_ARB )
			NMTOOL( GL_RGB )
			NMTOOL( GL_BGR )
			NMTOOL( GL_RGBA )
			NMTOOL( GL_BGRA )
			NMTOOL( GL_LUMINANCE )
			NMTOOL( GL_LUMINANCE4 )
			NMTOOL( GL_LUMINANCE8 )
			NMTOOL( GL_LUMINANCE8_SNORM )
			NMTOOL( GL_LUMINANCE12 )
			NMTOOL( GL_LUMINANCE16 )
			NMTOOL( GL_LUMINANCE16F_ARB )
			NMTOOL( GL_LUMINANCE16_SNORM )
			NMTOOL( GL_LUMINANCE32F_ARB )
			NMTOOL( GL_LUMINANCE_ALPHA )
			NMTOOL( GL_LUMINANCE4_ALPHA4 )
			NMTOOL( GL_LUMINANCE8_ALPHA8 )
			NMTOOL( GL_LUMINANCE8_ALPHA8_SNORM )
			NMTOOL( GL_LUMINANCE12_ALPHA4 )
			NMTOOL( GL_LUMINANCE12_ALPHA12 )
			NMTOOL( GL_LUMINANCE16_ALPHA16 )
			NMTOOL( GL_LUMINANCE_ALPHA32F_ARB )
			NMTOOL( GL_INTENSITY )
			NMTOOL( GL_INTENSITY4 )
			NMTOOL( GL_INTENSITY8 )
			NMTOOL( GL_INTENSITY8_SNORM )
			NMTOOL( GL_INTENSITY12 )
			NMTOOL( GL_INTENSITY16 )
			NMTOOL( GL_INTENSITY16F_ARB )
			NMTOOL( GL_INTENSITY16_SNORM )
			NMTOOL( GL_INTENSITY32F_ARB )
			NMTOOL( GL_RGB32F_ARB )
			NMTOOL( GL_RG )
			NMTOOL( GL_RG8 )
			NMTOOL( GL_RG8I )
			NMTOOL( GL_RG8UI )
			NMTOOL( GL_RG8_SNORM )
			NMTOOL( GL_RG16 )
			NMTOOL( GL_RG16I )
			NMTOOL( GL_RG16UI )
			NMTOOL( GL_RG16F )
			NMTOOL( GL_RG16_SNORM )
			NMTOOL( GL_RG32I )
			NMTOOL( GL_RG32UI )
			NMTOOL( GL_RG32F )
			NMTOOL( GL_RGB32I )
			NMTOOL( GL_RGB32UI )
			NMTOOL( GL_RGB16_SNORM )
			NMTOOL( GL_RGB16F )
			NMTOOL( GL_RGB16I )
			NMTOOL( GL_RGB16UI )
			NMTOOL( GL_RGB16 )
			NMTOOL( GL_RGB8_SNORM )
			NMTOOL( GL_RGB8 )
			NMTOOL( GL_RGB8I )
			NMTOOL( GL_RGB8UI )
			NMTOOL( GL_SRGB )
			NMTOOL( GL_RGB4 )
			NMTOOL( GL_SRGB8 )
			NMTOOL( GL_RGB9_E5 )
			NMTOOL( GL_RGB10 )
			NMTOOL( GL_RGB12 )
			NMTOOL( GL_RGBA32F )
			NMTOOL( GL_RGBA32I )
			NMTOOL( GL_RGBA32UI )
			NMTOOL( GL_RGBA16 )
			NMTOOL( GL_RGBA16F )
			NMTOOL( GL_RGBA16_SNORM )
			NMTOOL( GL_RGBA16I )
			NMTOOL( GL_RGBA16UI )
			NMTOOL( GL_RGBA8 )
			NMTOOL( GL_RGBA8I )
			NMTOOL( GL_RGBA8UI )
			NMTOOL( GL_RGBA8_SNORM )
			NMTOOL( GL_SRGB8_ALPHA8 )
			NMTOOL( GL_RGBA_SNORM )
			NMTOOL( GL_SRGB_ALPHA )
			NMTOOL( GL_RGBA4 )
			NMTOOL( GL_RGB10_A2 )
			NMTOOL( GL_RGB10_A2UI )

			// Compressed version :
			NMTOOL( GL_COMPRESSED_ALPHA )
			NMTOOL( GL_COMPRESSED_INTENSITY )
			NMTOOL( GL_COMPRESSED_LUMINANCE )
			NMTOOL( GL_COMPRESSED_LUMINANCE_ALPHA )
			NMTOOL( GL_COMPRESSED_RED )
			NMTOOL( GL_COMPRESSED_RED_RGTC1 )
			NMTOOL( GL_COMPRESSED_RG )
			NMTOOL( GL_COMPRESSED_RGBA )
			NMTOOL( GL_COMPRESSED_RGBA_BPTC_UNORM_ARB )
			NMTOOL( GL_COMPRESSED_RGBA_FXT1_3DFX )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT3_EXT )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
			NMTOOL( GL_COMPRESSED_RGB )
			NMTOOL( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB )
			NMTOOL( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB )
			NMTOOL( GL_COMPRESSED_RGB_FXT1_3DFX )
			NMTOOL( GL_COMPRESSED_RGB_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_RG_RGTC2 )
			NMTOOL( GL_COMPRESSED_SRGB )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_S3TC_DXT1_EXT )

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
			NMTOOL( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
			NMTOOL( GL_FRAMEBUFFER_UNSUPPORTED )

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
			default : return "(Unknown OpenGL constant)";
		}
		#undef NMTOOL
	}

	/**
	\fn GLenum Glip::CoreGL::gl_from_string(const std::string& name)
	\brief Get the parameter from its name in a string.
	\param param The GLenum parameter name.
	\return The corresponding enum.
	**/
	GLenum Glip::CoreGL::gl_from_string(const std::string& name)
	{
		#define NMTOOL(X) if(name==#X) return X;

			// Misc
			NMTOOL( GL_NONE )
			NMTOOL( GL_ZERO )
			NMTOOL( GL_ONE )

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
			NMTOOL( GL_ALPHA4 )
			NMTOOL( GL_ALPHA8 )
			NMTOOL( GL_ALPHA8_SNORM )
			NMTOOL( GL_ALPHA12 )
			NMTOOL( GL_ALPHA16 )
			NMTOOL( GL_ALPHA16F_ARB )
			NMTOOL( GL_ALPHA16_SNORM )
			NMTOOL( GL_ALPHA32F_ARB )
			NMTOOL( GL_RGB )
			NMTOOL( GL_BGR )
			NMTOOL( GL_RGBA )
			NMTOOL( GL_BGRA )
			NMTOOL( GL_LUMINANCE )
			NMTOOL( GL_LUMINANCE4 )
			NMTOOL( GL_LUMINANCE8 )
			NMTOOL( GL_LUMINANCE8_SNORM )
			NMTOOL( GL_LUMINANCE12 )
			NMTOOL( GL_LUMINANCE16 )
			NMTOOL( GL_LUMINANCE16F_ARB )
			NMTOOL( GL_LUMINANCE16_SNORM )
			NMTOOL( GL_LUMINANCE32F_ARB )
			NMTOOL( GL_LUMINANCE_ALPHA )
			NMTOOL( GL_LUMINANCE4_ALPHA4 )
			NMTOOL( GL_LUMINANCE8_ALPHA8 )
			NMTOOL( GL_LUMINANCE8_ALPHA8_SNORM )
			NMTOOL( GL_LUMINANCE12_ALPHA4 )
			NMTOOL( GL_LUMINANCE12_ALPHA12 )
			NMTOOL( GL_LUMINANCE16_ALPHA16 )
			NMTOOL( GL_LUMINANCE_ALPHA32F_ARB )
			NMTOOL( GL_INTENSITY )
			NMTOOL( GL_INTENSITY4 )
			NMTOOL( GL_INTENSITY8 )
			NMTOOL( GL_INTENSITY8_SNORM )
			NMTOOL( GL_INTENSITY12 )
			NMTOOL( GL_INTENSITY16 )
			NMTOOL( GL_INTENSITY16F_ARB )
			NMTOOL( GL_INTENSITY16_SNORM )
			NMTOOL( GL_INTENSITY32F_ARB )
			NMTOOL( GL_RGB32F )
			NMTOOL( GL_RG )
			NMTOOL( GL_RG8 )
			NMTOOL( GL_RG8I )
			NMTOOL( GL_RG8UI )
			NMTOOL( GL_RG8_SNORM )
			NMTOOL( GL_RG16 )
			NMTOOL( GL_RG16I )
			NMTOOL( GL_RG16UI )
			NMTOOL( GL_RG16F )
			NMTOOL( GL_RG16_SNORM )
			NMTOOL( GL_RG32I )
			NMTOOL( GL_RG32UI )
			NMTOOL( GL_RG32F )
			NMTOOL( GL_RGB32I )
			NMTOOL( GL_RGB32UI )
			NMTOOL( GL_RGB16_SNORM )
			NMTOOL( GL_RGB16F )
			NMTOOL( GL_RGB16I )
			NMTOOL( GL_RGB16UI )
			NMTOOL( GL_RGB16 )
			NMTOOL( GL_RGB8_SNORM )
			NMTOOL( GL_RGB8 )
			NMTOOL( GL_RGB8I )
			NMTOOL( GL_RGB8UI )
			NMTOOL( GL_SRGB )
			NMTOOL( GL_RGB4 )
			NMTOOL( GL_SRGB8 )
			NMTOOL( GL_RGB9_E5 )
			NMTOOL( GL_RGB10 )
			NMTOOL( GL_RGB12 )
			NMTOOL( GL_RGBA32F )
			NMTOOL( GL_RGBA32I )
			NMTOOL( GL_RGBA32UI )
			NMTOOL( GL_RGBA16 )
			NMTOOL( GL_RGBA16F )
			NMTOOL( GL_RGBA16_SNORM )
			NMTOOL( GL_RGBA16I )
			NMTOOL( GL_RGBA16UI )
			NMTOOL( GL_RGBA8 )
			NMTOOL( GL_RGBA8I )
			NMTOOL( GL_RGBA8UI )
			NMTOOL( GL_RGBA8_SNORM )
			NMTOOL( GL_SRGB8_ALPHA8 )
			NMTOOL( GL_RGBA_SNORM )
			NMTOOL( GL_SRGB_ALPHA )
			NMTOOL( GL_RGBA4 )
			NMTOOL( GL_RGB10_A2 )
			NMTOOL( GL_RGB10_A2UI )

			// Compressed version :
			NMTOOL( GL_COMPRESSED_ALPHA )
			NMTOOL( GL_COMPRESSED_INTENSITY )
			NMTOOL( GL_COMPRESSED_LUMINANCE )
			NMTOOL( GL_COMPRESSED_LUMINANCE_ALPHA )
			NMTOOL( GL_COMPRESSED_RED )
			NMTOOL( GL_COMPRESSED_RED_RGTC1 )
			NMTOOL( GL_COMPRESSED_RG )
			NMTOOL( GL_COMPRESSED_RGBA )
			NMTOOL( GL_COMPRESSED_RGBA_BPTC_UNORM_ARB )
			NMTOOL( GL_COMPRESSED_RGBA_FXT1_3DFX )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT3_EXT )
			NMTOOL( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
			NMTOOL( GL_COMPRESSED_RGB )
			NMTOOL( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB )
			NMTOOL( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB )
			NMTOOL( GL_COMPRESSED_RGB_FXT1_3DFX )
			NMTOOL( GL_COMPRESSED_RGB_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_RG_RGTC2 )
			NMTOOL( GL_COMPRESSED_SRGB )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT )
			NMTOOL( GL_COMPRESSED_SRGB_S3TC_DXT1_EXT )

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
			NMTOOL( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
			NMTOOL( GL_FRAMEBUFFER_UNSUPPORTED )


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
			return GL_FALSE;
		#undef NMTOOL
	}

