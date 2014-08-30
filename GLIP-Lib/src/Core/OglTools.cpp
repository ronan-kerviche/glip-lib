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

#include "Core/OglInclude.hpp"
#include "Core/HdlTexture.hpp"
#include "Core/Exception.hpp"
#include "Core/HdlVBO.hpp"
#include <string>

using namespace Glip;
using namespace Glip::CoreGL;

// Structure
	// Data
		HandleOpenGL* 			HandleOpenGL::instance 	= NULL;
		HandleOpenGL::SupportedVendor 	HandleOpenGL::vendor 	= vd_UNKNOWN;

	// Functions
		/**
		\fn void HandleOpenGL::HandleOpenGL(void)
		\brief Initialize Glew and other tools for the OpenGL state machine.

		You have to create one object of this type before functions using OpenGL.
		**/
		HandleOpenGL::HandleOpenGL(void)
		{
			if(instance==NULL)
			{
				GLenum err = glewInit();
				if(err != GLEW_OK)
				{
					std::string error = reinterpret_cast<const char*>(glewGetErrorString(err));
					throw Exception("HandleOpenGL::HandleOpenGL - Failed to init GLEW with the following error : " + error, __FILE__, __LINE__);
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

				instance = this;
			}
			else
				throw Exception("HandleOpenGL::HandleOpenGL - GLIP-LIB has already been initialized.", __FILE__, __LINE__);
		}

		HandleOpenGL::~HandleOpenGL(void)
		{
			if(instance==this)
			{
				//OLD : delete standardQuad;
				instance = NULL;
			}
		}

		/**
		\fn void HandleOpenGL::init(void)
		\brief Initialize Glew and other tools for the OpenGL state machine.

		When using this function, you <b>MUST</b> make an explicit call to HandleOpenGL::deinit when exiting program.
		**/
		void HandleOpenGL::init(void)
		{
			if(instance==NULL)
				instance = new HandleOpenGL;
			else
				throw Exception("HandleOpenGL::init - GLIP-LIB has already been initialized.", __FILE__, __LINE__);
		}

		/**
		\fn void HandleOpenGL::deinit(void)
		\brief Deinitialize Glew and other tools for the OpenGL state machine.

		You have to call this function when the context has been created with HandleOpenGL::init.
		**/
		void HandleOpenGL::deinit(void)
		{
			if(instance!=NULL)
			{
				delete instance;
				instance = NULL;
			}
			else
				throw Exception("HandleOpenGL::deinit - GLIP-LIB has never been initialized for this program.", __FILE__, __LINE__);
		}

		/**
		\fn bool HandleOpenGL::isInitialized(void)
		\brief Test if the context was already initialized.
		\return True if the context was initialized (and not terminated).
		**/
		bool HandleOpenGL::isInitialized(void)
		{
			return (instance!=NULL);
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
	\related HandleOpenGL
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
			Err( GL_INVALID_FRAMEBUFFER_OPERATION_EXT,	"Invalid framebuffer operation (possible incomplete texture)")
			Err( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,	"Incomplete attachment for framebuffer (possible incompatibility with texture format)")
			Err( GL_FRAMEBUFFER_UNSUPPORTED,		"Unsupported framebuffer (possible incompatibility with texture format)")
			case GL_NO_ERROR : 				return "(No error)";
			default          : 				return "Unknown error (Code : " + toString(err) + ")";
		}
		#undef Err
	}

	/**
	\fn bool Glip::CoreGL::glErrors(bool verbose, bool quietSituations)
	\related HandleOpenGL
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
				Err( GL_INVALID_FRAMEBUFFER_OPERATION_EXT,	"Invalid framebuffer operation (possible incomplete texture)")
				Err( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,	"Incomplete attachment for framebuffer (possible incompatibility with texture format)")
				Err( GL_FRAMEBUFFER_UNSUPPORTED,		"Unsupported framebuffer (possible incompatibility with texture format)")

				case GL_NO_ERROR : if(!quietSituations){ std::cout << "OpenGL error : No error recorded" << std::endl; } break;
				default          :                       std::cout << "OpenGL error : Unknown error (Code : " << err << ')' << std::endl; break;
			}
			#undef Err
		}

		return err!=GL_NO_ERROR;
	}

	/**
	\fn void Glip::CoreGL::glDebug(void)
	\related HandleOpenGL
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


	// OpenGL Keyword list :
	const HandleOpenGL::KeywordPair HandleOpenGL::glKeywords[] =
	{
		#define KEYWORD_PAIR( v ) { v, #v },
		KEYWORD_PAIR( GL_NONE )
		KEYWORD_PAIR( GL_ZERO )
		KEYWORD_PAIR( GL_ONE )

		// Geometry
		KEYWORD_PAIR( GL_POINTS )
		KEYWORD_PAIR( GL_LINES )
		KEYWORD_PAIR( GL_LINE_STRIP )
		KEYWORD_PAIR( GL_LINE_LOOP )
		KEYWORD_PAIR( GL_TRIANGLES )
		KEYWORD_PAIR( GL_TRIANGLE_STRIP )
		KEYWORD_PAIR( GL_TRIANGLE_FAN )
		KEYWORD_PAIR( GL_QUADS )
		KEYWORD_PAIR( GL_QUAD_STRIP )
		KEYWORD_PAIR( GL_POLYGON )
		KEYWORD_PAIR( GL_LINES_ADJACENCY )
		KEYWORD_PAIR( GL_LINE_STRIP_ADJACENCY )
		KEYWORD_PAIR( GL_TRIANGLES_ADJACENCY )
		KEYWORD_PAIR( GL_TRIANGLE_STRIP_ADJACENCY )

		// Texture Mode
		KEYWORD_PAIR( GL_TEXTURE_1D )
		KEYWORD_PAIR( GL_TEXTURE_2D )
		KEYWORD_PAIR( GL_TEXTURE_3D )
		KEYWORD_PAIR( GL_TEXTURE_CUBE_MAP_POSITIVE_X )
		KEYWORD_PAIR( GL_TEXTURE_CUBE_MAP_NEGATIVE_X )
		KEYWORD_PAIR( GL_TEXTURE_CUBE_MAP_POSITIVE_Y )
		KEYWORD_PAIR( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y )
		KEYWORD_PAIR( GL_TEXTURE_CUBE_MAP_POSITIVE_Z )

		// Texture Parameters name
		KEYWORD_PAIR( GL_TEXTURE_WIDTH )
		KEYWORD_PAIR( GL_TEXTURE_HEIGHT )
		KEYWORD_PAIR( GL_TEXTURE_RED_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_GREEN_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_BLUE_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_ALPHA_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_LUMINANCE_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_INTENSITY_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_DEPTH_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_MIN_FILTER )
		KEYWORD_PAIR( GL_TEXTURE_MAG_FILTER )
		KEYWORD_PAIR( GL_TEXTURE_WRAP_S )
		KEYWORD_PAIR( GL_TEXTURE_WRAP_T )
		KEYWORD_PAIR( GL_TEXTURE_BASE_LEVEL )
		KEYWORD_PAIR( GL_TEXTURE_MAX_LEVEL )
		KEYWORD_PAIR( GL_GENERATE_MIPMAP )
		KEYWORD_PAIR( GL_TEXTURE_INTERNAL_FORMAT )
		KEYWORD_PAIR( GL_TEXTURE_COMPRESSED )
		KEYWORD_PAIR( GL_TEXTURE_COMPRESSED_IMAGE_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_INTERNAL_FORMAT )
		KEYWORD_PAIR( GL_TEXTURE_DEPTH )
		KEYWORD_PAIR( GL_TEXTURE_RED_TYPE )
		KEYWORD_PAIR( GL_TEXTURE_GREEN_TYPE )
		KEYWORD_PAIR( GL_TEXTURE_BLUE_TYPE )
		KEYWORD_PAIR( GL_TEXTURE_ALPHA_TYPE )
		KEYWORD_PAIR( GL_TEXTURE_DEPTH_TYPE )

		// Color Channel Format
		KEYWORD_PAIR( GL_RED )
		KEYWORD_PAIR( GL_GREEN )
		KEYWORD_PAIR( GL_BLUE )
		KEYWORD_PAIR( GL_R8 )
		KEYWORD_PAIR( GL_R8I )
		KEYWORD_PAIR( GL_R8UI )
		KEYWORD_PAIR( GL_R8_SNORM )
		KEYWORD_PAIR( GL_R16 )
		KEYWORD_PAIR( GL_R16I )
		KEYWORD_PAIR( GL_R16UI )
		KEYWORD_PAIR( GL_R16_SNORM )
		KEYWORD_PAIR( GL_R16F )
		KEYWORD_PAIR( GL_R32I )
		KEYWORD_PAIR( GL_R32UI )
		KEYWORD_PAIR( GL_R32F )
		KEYWORD_PAIR( GL_ALPHA )
		KEYWORD_PAIR( GL_ALPHA4 )
		KEYWORD_PAIR( GL_ALPHA8 )
		KEYWORD_PAIR( GL_ALPHA8_SNORM )
		KEYWORD_PAIR( GL_ALPHA12 )
		KEYWORD_PAIR( GL_ALPHA16 )
		KEYWORD_PAIR( GL_ALPHA16F_ARB )
		KEYWORD_PAIR( GL_ALPHA16_SNORM )
		KEYWORD_PAIR( GL_ALPHA32F_ARB )
		KEYWORD_PAIR( GL_RGB )
		KEYWORD_PAIR( GL_BGR )
		KEYWORD_PAIR( GL_RGBA )
		KEYWORD_PAIR( GL_BGRA )
		KEYWORD_PAIR( GL_LUMINANCE )
		KEYWORD_PAIR( GL_LUMINANCE4 )
		KEYWORD_PAIR( GL_LUMINANCE8 )
		KEYWORD_PAIR( GL_LUMINANCE8_SNORM )
		KEYWORD_PAIR( GL_LUMINANCE12 )
		KEYWORD_PAIR( GL_LUMINANCE16 )
		KEYWORD_PAIR( GL_LUMINANCE16F_ARB )
		KEYWORD_PAIR( GL_LUMINANCE16_SNORM )
		KEYWORD_PAIR( GL_LUMINANCE32F_ARB )
		KEYWORD_PAIR( GL_LUMINANCE_ALPHA )
		KEYWORD_PAIR( GL_LUMINANCE4_ALPHA4 )
		KEYWORD_PAIR( GL_LUMINANCE8_ALPHA8 )
		KEYWORD_PAIR( GL_LUMINANCE8_ALPHA8_SNORM )
		KEYWORD_PAIR( GL_LUMINANCE12_ALPHA4 )
		KEYWORD_PAIR( GL_LUMINANCE12_ALPHA12 )
		KEYWORD_PAIR( GL_LUMINANCE16_ALPHA16 )
		KEYWORD_PAIR( GL_LUMINANCE_ALPHA32F_ARB )
		KEYWORD_PAIR( GL_INTENSITY )
		KEYWORD_PAIR( GL_INTENSITY4 )
		KEYWORD_PAIR( GL_INTENSITY8 )
		KEYWORD_PAIR( GL_INTENSITY8_SNORM )
		KEYWORD_PAIR( GL_INTENSITY12 )
		KEYWORD_PAIR( GL_INTENSITY16 )
		KEYWORD_PAIR( GL_INTENSITY16F_ARB )
		KEYWORD_PAIR( GL_INTENSITY16_SNORM )
		KEYWORD_PAIR( GL_INTENSITY32F_ARB )
		KEYWORD_PAIR( GL_RGB32F )
		KEYWORD_PAIR( GL_RG )
		KEYWORD_PAIR( GL_RG8 )
		KEYWORD_PAIR( GL_RG8I )
		KEYWORD_PAIR( GL_RG8UI )
		KEYWORD_PAIR( GL_RG8_SNORM )
		KEYWORD_PAIR( GL_RG16 )
		KEYWORD_PAIR( GL_RG16I )
		KEYWORD_PAIR( GL_RG16UI )
		KEYWORD_PAIR( GL_RG16F )
		KEYWORD_PAIR( GL_RG16_SNORM )
		KEYWORD_PAIR( GL_RG32I )
		KEYWORD_PAIR( GL_RG32UI )
		KEYWORD_PAIR( GL_RG32F )
		KEYWORD_PAIR( GL_RGB32I )
		KEYWORD_PAIR( GL_RGB32UI )
		KEYWORD_PAIR( GL_RGB16_SNORM )
		KEYWORD_PAIR( GL_RGB16F )
		KEYWORD_PAIR( GL_RGB16I )
		KEYWORD_PAIR( GL_RGB16UI )
		KEYWORD_PAIR( GL_RGB16 )
		KEYWORD_PAIR( GL_RGB8_SNORM )
		KEYWORD_PAIR( GL_RGB8 )
		KEYWORD_PAIR( GL_RGB8I )
		KEYWORD_PAIR( GL_RGB8UI )
		KEYWORD_PAIR( GL_SRGB )
		KEYWORD_PAIR( GL_RGB4 )
		KEYWORD_PAIR( GL_SRGB8 )
		KEYWORD_PAIR( GL_RGB9_E5 )
		KEYWORD_PAIR( GL_RGB10 )
		KEYWORD_PAIR( GL_RGB12 )
		KEYWORD_PAIR( GL_RGBA32F )
		KEYWORD_PAIR( GL_RGBA32I )
		KEYWORD_PAIR( GL_RGBA32UI )
		KEYWORD_PAIR( GL_RGBA16 )
		KEYWORD_PAIR( GL_RGBA16F )
		KEYWORD_PAIR( GL_RGBA16_SNORM )
		KEYWORD_PAIR( GL_RGBA16I )
		KEYWORD_PAIR( GL_RGBA16UI )
		KEYWORD_PAIR( GL_RGBA8 )
		KEYWORD_PAIR( GL_RGBA8I )
		KEYWORD_PAIR( GL_RGBA8UI )
		KEYWORD_PAIR( GL_RGBA8_SNORM )
		KEYWORD_PAIR( GL_SRGB8_ALPHA8 )
		KEYWORD_PAIR( GL_RGBA_SNORM )
		KEYWORD_PAIR( GL_SRGB_ALPHA )
		KEYWORD_PAIR( GL_RGBA4 )
		KEYWORD_PAIR( GL_RGB10_A2 )
		KEYWORD_PAIR( GL_RGB10_A2UI )

		// Compressed version :
		KEYWORD_PAIR( GL_COMPRESSED_ALPHA )
		KEYWORD_PAIR( GL_COMPRESSED_INTENSITY )
		KEYWORD_PAIR( GL_COMPRESSED_LUMINANCE )
		KEYWORD_PAIR( GL_COMPRESSED_LUMINANCE_ALPHA )
		KEYWORD_PAIR( GL_COMPRESSED_RED )
		KEYWORD_PAIR( GL_COMPRESSED_RED_RGTC1 )
		KEYWORD_PAIR( GL_COMPRESSED_RG )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA_BPTC_UNORM_ARB )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA_FXT1_3DFX )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA_S3TC_DXT3_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_RGB )
		KEYWORD_PAIR( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB )
		KEYWORD_PAIR( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB )
		KEYWORD_PAIR( GL_COMPRESSED_RGB_FXT1_3DFX )
		KEYWORD_PAIR( GL_COMPRESSED_RGB_S3TC_DXT1_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_RG_RGTC2 )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_ALPHA )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT )
		KEYWORD_PAIR( GL_COMPRESSED_SRGB_S3TC_DXT1_EXT )

		// Type :
		KEYWORD_PAIR( GL_BOOL )
		KEYWORD_PAIR( GL_UNSIGNED_BYTE )
		KEYWORD_PAIR( GL_BYTE )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT )
		KEYWORD_PAIR( GL_SHORT )
		KEYWORD_PAIR( GL_UNSIGNED_INT )
		KEYWORD_PAIR( GL_INT )
		KEYWORD_PAIR( GL_FLOAT )
		KEYWORD_PAIR( GL_DOUBLE )
		KEYWORD_PAIR( GL_UNSIGNED_BYTE_3_3_2 )
		KEYWORD_PAIR( GL_UNSIGNED_BYTE_2_3_3_REV )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_5_6_5 )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_5_6_5_REV )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_4_4_4_4 )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_4_4_4_4_REV )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_5_5_5_1 )
		KEYWORD_PAIR( GL_UNSIGNED_SHORT_1_5_5_5_REV )
		KEYWORD_PAIR( GL_UNSIGNED_INT_8_8_8_8 )
		KEYWORD_PAIR( GL_UNSIGNED_INT_8_8_8_8_REV )
		KEYWORD_PAIR( GL_UNSIGNED_INT_10_10_10_2 )
		KEYWORD_PAIR( GL_UNSIGNED_INT_2_10_10_10_REV )

		// GLSL Types :
		KEYWORD_PAIR( GL_FLOAT_VEC2 )
		KEYWORD_PAIR( GL_FLOAT_VEC3 )
		KEYWORD_PAIR( GL_FLOAT_VEC4 )
		KEYWORD_PAIR( GL_DOUBLE_VEC2 )
		KEYWORD_PAIR( GL_DOUBLE_VEC3 )
		KEYWORD_PAIR( GL_DOUBLE_VEC4 )
		KEYWORD_PAIR( GL_INT_VEC2 )
		KEYWORD_PAIR( GL_INT_VEC3 )
		KEYWORD_PAIR( GL_INT_VEC4 )
		KEYWORD_PAIR( GL_UNSIGNED_INT )
		KEYWORD_PAIR( GL_UNSIGNED_INT_VEC2 )
		KEYWORD_PAIR( GL_UNSIGNED_INT_VEC3 )
		KEYWORD_PAIR( GL_UNSIGNED_INT_VEC4 )
		KEYWORD_PAIR( GL_BOOL_VEC2 )
		KEYWORD_PAIR( GL_BOOL_VEC3 )
		KEYWORD_PAIR( GL_BOOL_VEC4 )
		KEYWORD_PAIR( GL_FLOAT_MAT2 )
		KEYWORD_PAIR( GL_FLOAT_MAT3 )
		KEYWORD_PAIR( GL_FLOAT_MAT4 )
		KEYWORD_PAIR( GL_FLOAT_MAT2x3 )
		KEYWORD_PAIR( GL_FLOAT_MAT2x4 )
		KEYWORD_PAIR( GL_FLOAT_MAT3x2 )
		KEYWORD_PAIR( GL_FLOAT_MAT3x4 )
		KEYWORD_PAIR( GL_FLOAT_MAT4x2 )
		KEYWORD_PAIR( GL_FLOAT_MAT4x3 )
		KEYWORD_PAIR( GL_DOUBLE_MAT2 )
		KEYWORD_PAIR( GL_DOUBLE_MAT3 )
		KEYWORD_PAIR( GL_DOUBLE_MAT4 )
		KEYWORD_PAIR( GL_DOUBLE_MAT2x3 )
		KEYWORD_PAIR( GL_DOUBLE_MAT2x4 )
		KEYWORD_PAIR( GL_DOUBLE_MAT3x2 )
		KEYWORD_PAIR( GL_DOUBLE_MAT3x4 )
		KEYWORD_PAIR( GL_DOUBLE_MAT4x2 )
		KEYWORD_PAIR( GL_DOUBLE_MAT4x3 )
		KEYWORD_PAIR( GL_SAMPLER_1D )
		KEYWORD_PAIR( GL_SAMPLER_2D )
		KEYWORD_PAIR( GL_SAMPLER_3D )
		KEYWORD_PAIR( GL_SAMPLER_CUBE )
		KEYWORD_PAIR( GL_SAMPLER_1D_SHADOW )
		KEYWORD_PAIR( GL_SAMPLER_2D_SHADOW )
		KEYWORD_PAIR( GL_SAMPLER_1D_ARRAY )
		KEYWORD_PAIR( GL_SAMPLER_2D_ARRAY )
		KEYWORD_PAIR( GL_SAMPLER_1D_ARRAY_SHADOW )
		KEYWORD_PAIR( GL_SAMPLER_2D_ARRAY_SHADOW )
		KEYWORD_PAIR( GL_SAMPLER_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_SAMPLER_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_SAMPLER_CUBE_SHADOW )
		KEYWORD_PAIR( GL_SAMPLER_BUFFER )
		KEYWORD_PAIR( GL_SAMPLER_2D_RECT )
		KEYWORD_PAIR( GL_SAMPLER_2D_RECT_SHADOW )
		KEYWORD_PAIR( GL_INT_SAMPLER_1D )
		KEYWORD_PAIR( GL_INT_SAMPLER_2D )
		KEYWORD_PAIR( GL_INT_SAMPLER_3D )
		KEYWORD_PAIR( GL_INT_SAMPLER_CUBE )
		KEYWORD_PAIR( GL_INT_SAMPLER_1D_ARRAY )
		KEYWORD_PAIR( GL_INT_SAMPLER_2D_ARRAY )
		KEYWORD_PAIR( GL_INT_SAMPLER_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_INT_SAMPLER_BUFFER )
		KEYWORD_PAIR( GL_INT_SAMPLER_2D_RECT )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_1D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_2D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_3D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_CUBE )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_1D_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_2D_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_BUFFER )
		KEYWORD_PAIR( GL_UNSIGNED_INT_SAMPLER_2D_RECT )
		KEYWORD_PAIR( GL_IMAGE_1D )
		KEYWORD_PAIR( GL_IMAGE_2D )
		KEYWORD_PAIR( GL_IMAGE_3D )
		KEYWORD_PAIR( GL_IMAGE_2D_RECT )
		KEYWORD_PAIR( GL_IMAGE_CUBE )
		KEYWORD_PAIR( GL_IMAGE_BUFFER )
		KEYWORD_PAIR( GL_IMAGE_1D_ARRAY )
		KEYWORD_PAIR( GL_IMAGE_2D_ARRAY )
		KEYWORD_PAIR( GL_IMAGE_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_IMAGE_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_INT_IMAGE_1D )
		KEYWORD_PAIR( GL_INT_IMAGE_2D )
		KEYWORD_PAIR( GL_INT_IMAGE_3D )
		KEYWORD_PAIR( GL_INT_IMAGE_2D_RECT )
		KEYWORD_PAIR( GL_INT_IMAGE_CUBE )
		KEYWORD_PAIR( GL_INT_IMAGE_BUFFER )
		KEYWORD_PAIR( GL_INT_IMAGE_1D_ARRAY )
		KEYWORD_PAIR( GL_INT_IMAGE_2D_ARRAY )
		KEYWORD_PAIR( GL_INT_IMAGE_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_1D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_2D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_3D )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_2D_RECT )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_CUBE )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_BUFFER )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_1D_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_2D_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE )
		KEYWORD_PAIR( GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY )
		KEYWORD_PAIR( GL_UNSIGNED_INT_ATOMIC_COUNTER )

		// Texture filtering
		KEYWORD_PAIR( GL_TEXTURE_MIN_FILTER )
		KEYWORD_PAIR( GL_TEXTURE_MAG_FILTER )
		KEYWORD_PAIR( GL_TEXTURE_MIN_LOD )
		KEYWORD_PAIR( GL_TEXTURE_MAX_LOD )
		KEYWORD_PAIR( GL_TEXTURE_BASE_LEVEL )
		KEYWORD_PAIR( GL_TEXTURE_MAX_LEVEL )
		KEYWORD_PAIR( GL_TEXTURE_WRAP_S )
		KEYWORD_PAIR( GL_TEXTURE_WRAP_T )
		KEYWORD_PAIR( GL_TEXTURE_WRAP_R )
		KEYWORD_PAIR( GL_CLAMP )
		KEYWORD_PAIR( GL_CLAMP_TO_EDGE )
		KEYWORD_PAIR( GL_CLAMP_TO_BORDER )
		KEYWORD_PAIR( GL_REPEAT )
		KEYWORD_PAIR( GL_MIRRORED_REPEAT )
		KEYWORD_PAIR( GL_TEXTURE_PRIORITY )
		KEYWORD_PAIR( GL_LINEAR )
		KEYWORD_PAIR( GL_NEAREST )
		KEYWORD_PAIR( GL_NEAREST_MIPMAP_NEAREST )
		KEYWORD_PAIR( GL_LINEAR_MIPMAP_NEAREST )
		KEYWORD_PAIR( GL_NEAREST_MIPMAP_LINEAR )
		KEYWORD_PAIR( GL_LINEAR_MIPMAP_LINEAR )

		// Errors
		KEYWORD_PAIR( GL_INVALID_ENUM )
		KEYWORD_PAIR( GL_INVALID_VALUE )
		KEYWORD_PAIR( GL_INVALID_OPERATION )
		KEYWORD_PAIR( GL_STACK_OVERFLOW )
		KEYWORD_PAIR( GL_STACK_UNDERFLOW )
		KEYWORD_PAIR( GL_OUT_OF_MEMORY )
		KEYWORD_PAIR( GL_TABLE_TOO_LARGE )
		KEYWORD_PAIR( GL_INVALID_FRAMEBUFFER_OPERATION_EXT )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
		KEYWORD_PAIR( GL_FRAMEBUFFER_UNSUPPORTED )

		// GeBO packing
		KEYWORD_PAIR( GL_PIXEL_PACK_BUFFER )
		KEYWORD_PAIR( GL_PIXEL_UNPACK_BUFFER )
		KEYWORD_PAIR( GL_ARRAY_BUFFER_ARB )
		KEYWORD_PAIR( GL_ELEMENT_ARRAY_BUFFER_ARB )
		KEYWORD_PAIR( GL_READ_ONLY_ARB )
		KEYWORD_PAIR( GL_WRITE_ONLY_ARB )
		KEYWORD_PAIR( GL_READ_WRITE_ARB )

		KEYWORD_PAIR( GL_STATIC_DRAW_ARB )
		KEYWORD_PAIR( GL_STATIC_READ_ARB )
		KEYWORD_PAIR( GL_STATIC_COPY_ARB )
		KEYWORD_PAIR( GL_DYNAMIC_DRAW_ARB )
		KEYWORD_PAIR( GL_DYNAMIC_READ_ARB )
		KEYWORD_PAIR( GL_DYNAMIC_COPY_ARB )
		KEYWORD_PAIR( GL_STREAM_DRAW_ARB )
		KEYWORD_PAIR( GL_STREAM_READ_ARB )
		KEYWORD_PAIR( GL_STREAM_COPY_ARB )

		KEYWORD_PAIR( GL_INDEX_ARRAY )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY )

		// Comparison
		KEYWORD_PAIR( GL_LEQUAL )
		KEYWORD_PAIR( GL_GEQUAL )
		KEYWORD_PAIR( GL_LESS )
		KEYWORD_PAIR( GL_GREATER )
		KEYWORD_PAIR( GL_EQUAL )
		KEYWORD_PAIR( GL_NOTEQUAL )
		KEYWORD_PAIR( GL_ALWAYS )
		KEYWORD_PAIR( GL_NEVER )

		#undef KEYWORD_PAIR
	};

	// OpenGL - Parameters Naming :
	/**
	\related HandleOpenGL
	\fn std::string Glip::CoreGL::glParamName(GLenum param)
	\brief Get the parameter name as a string.
	\param param The GLenum parameter.
	\return A standard string.
	**/
	std::string Glip::CoreGL::glParamName(GLenum param)
	{
		const unsigned int numTokens = sizeof(HandleOpenGL::glKeywords)/sizeof(HandleOpenGL::KeywordPair);

		// Mixed name :
		if(param==GL_POINTS || param==GL_ZERO || param==GL_FALSE)
			return "GL_POINTS/GL_ZERO/GL_NONE";

		for(int i=0; i<numTokens; i++)
		{
			if(HandleOpenGL::glKeywords[i].value==param)
				return HandleOpenGL::glKeywords[i].name;
		}

		// Default :
		return "(Unknown OpenGL constant)";
	}

	/**
	\related HandleOpenGL
	\fn GLenum Glip::CoreGL::glFromString(const std::string& name)
	\brief Get the parameter from its name in a string.
	\param name The GLenum parameter name.
	\return The corresponding enum.
	**/
	GLenum Glip::CoreGL::glFromString(const std::string& name)
	{
		const unsigned int numTokens = sizeof(HandleOpenGL::glKeywords)/sizeof(HandleOpenGL::KeywordPair);

		for(int i=0; i<numTokens; i++)
		{
			if(HandleOpenGL::glKeywords[i].name==name)
				return HandleOpenGL::glKeywords[i].value;
		}

		// Default :
		return GL_FALSE;
	}

