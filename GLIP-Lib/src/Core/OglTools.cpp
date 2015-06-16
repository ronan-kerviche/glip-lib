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
#include <algorithm>

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
				if(err!=GLEW_OK)
				{
					const char* str = reinterpret_cast<const char*>(glewGetErrorString(err));
					std::string error((str==NULL) ? "" : str);
					throw Exception("HandleOpenGL::HandleOpenGL - Failed to init GLEW with the following error : " + error, __FILE__, __LINE__, Exception::GLException);
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
				throw Exception("HandleOpenGL::HandleOpenGL - GLIP-LIB has already been initialized.", __FILE__, __LINE__, Exception::CoreException);
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
				throw Exception("HandleOpenGL::init - GLIP-LIB has already been initialized.", __FILE__, __LINE__, Exception::CoreException);
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
				throw Exception("HandleOpenGL::deinit - GLIP-LIB has never been initialized for this program.", __FILE__, __LINE__, Exception::CoreException);
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
			if(str==NULL)
				return "";
			else
				return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::string HandleOpenGL::getRendererName(void)
		\return A std::string object containing the renderer name.
		**/
		std::string HandleOpenGL::getRendererName(void)
		{
			const GLubyte* str = glGetString(GL_RENDERER);
			if(str==NULL)
				return "";
			else
				return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::string HandleOpenGL::getVersion(void)
		\return A std::string object containing the OpenGL version.
		**/
		std::string HandleOpenGL::getVersion(void)
		{
			const GLubyte* str = glGetString(GL_VERSION);
			if(str==NULL)
				return "";
			else
				return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		/fn void HandleOpenGL::getVersion(int& major, int& minor)
		\brief Get the major and minor version number for OpenGL.
		\param major The major number of the version.
		\param minor The minor number of the version.
		**/
		void HandleOpenGL::getVersion(int& major, int& minor)
		{
			major = 0;
			minor = 0;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);
		}

		/**
		\fn std::string HandleOpenGL::getGLSLVersion(void)
		\return A std::string object containing the GLSL version or an empty string if the OpenGL version is less than 2.0.
		**/
		std::string HandleOpenGL::getGLSLVersion(void)
		{
			const GLubyte* str = GL_VERSION_2_0 ? glGetString(GL_SHADING_LANGUAGE_VERSION) : NULL;
			if(str==NULL)
				return "";
			else
				return std::string(reinterpret_cast<const char*>(str));
		}

		/**
		\fn std::vector<std::string> HandleOpenGL::getAvailableGLSLVersions(void)
		\brief Get the list of available GLSL version.
		\return A vector of string containing the available GLSL versions.
		**/
		std::vector<std::string> HandleOpenGL::getAvailableGLSLVersions(void)
		{
			if(!GL_VERSION_4_3)
				return std::vector<std::string>();
			else
			{
				GLint numStrings = -1;
				std::vector<std::string> results;
				glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &numStrings);

				for(int k=0; k<numStrings; k++)
				{
					const char* str = reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, k));
					if(str!=NULL)
						results.push_back(std::string(str));				
				}
				return results;
			}
		}

		/**
		\fn unsigned int HandleOpenGL::getShaderTypeIndex(GLenum shaderType)
		\brief Get the index of a shader type.
		\param shaderType The shader type : GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER.
		\return The index associated with the type.
		**/
		unsigned int HandleOpenGL::getShaderTypeIndex(GLenum shaderType)
		{
			switch(shaderType)
			{
				case GL_VERTEX_SHADER :			return 0;
				case GL_FRAGMENT_SHADER : 		return 1;
				case GL_COMPUTE_SHADER : 		return 2;
				case GL_TESS_CONTROL_SHADER : 		return 3;
				case GL_TESS_EVALUATION_SHADER : 	return 4;
				case GL_GEOMETRY_SHADER : 		return 5;
				default : 
					throw Exception("HdlProgram::getShaderIndex - Unknown shader type : " + getGLEnumNameSafe(shaderType) + ".", __FILE__, __LINE__, Exception::GLException);
			}
		}

// Errors Monitoring
	/**
	\fn std::string Glip::CoreGL::getGLErrorDescription(const GLenum& e)
	\related HandleOpenGL
	\brief Get the description for a particular error code.
	\param e The GL error code.
	\return A std::string containing a description of the error or a string with the error code if it is unknown.
	**/
	std::string Glip::CoreGL::getGLErrorDescription(const GLenum& e)
	{
		#define MAP(errorcode, message) case errorcode: return message ;
		switch(e)
		{
			MAP( GL_INVALID_ENUM,					"One or more GLenum parameter is incorrect.")
			MAP( GL_INVALID_VALUE,					"One or more value is incorrect.")
			MAP( GL_INVALID_OPERATION, 				"Operation is invalid.")
			MAP( GL_STACK_OVERFLOW,					"Stack was overflowed.")
			MAP( GL_STACK_UNDERFLOW,				"Stack was underflowed.")
			MAP( GL_OUT_OF_MEMORY,					"No sufficient memory available to complete the operation.")
			MAP( GL_TABLE_TOO_LARGE,				"Table is too large.")
			MAP( GL_INVALID_FRAMEBUFFER_OPERATION_EXT,		"Invalid framebuffer operation (possible incomplete texture).")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,		"Incomplete attachment for framebuffer (possible incompatibility with texture format).")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,	"The framebuffer does not have at least one image attached to it.")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,		"The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,		"GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,		"Either the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES or if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.")
			MAP( GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,		"Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.")
			MAP( GL_FRAMEBUFFER_UNSUPPORTED,			"Unsupported framebuffer format (possible incompatibility with texture format).")
			MAP( GL_FRAMEBUFFER_COMPLETE,				"(Framebuffer complete, no error).")
			MAP( GL_NO_ERROR, 					"(No error).")
			default          : 					return "Unknown error (code : " + toString(e) + ").";
		}
		#undef Err
	}	

	/**
	\fn void Glip::CoreGL::debugGL(void)
	\related HandleOpenGL
	\brief Print the current binding points state on std::cout (GL_TEXTURE_BINDING_1D, GL_TEXTURE_BINDING_2D, GL_TEXTURE_BINDING_3D, GL_ARRAY_BUFFER_BINDING, GL_ELEMENT_ARRAY_BUFFER_BINDING, GL_PIXEL_PACK_BUFFER_BINDING, GL_PIXEL_UNPACK_BUFFER_BINDING, GL_FRAMEBUFFER_BINDING, GL_TEXTURE_BUFFER_EXT, GL_CURRENT_PROGRAM).
	**/
	void Glip::CoreGL::debugGL(void)
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
		KEYWORD_PAIR( GL_TEXTURE )
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

		// Shader types
		KEYWORD_PAIR( GL_SHADER_TYPE )
		KEYWORD_PAIR( GL_VERTEX_SHADER )
		KEYWORD_PAIR( GL_FRAGMENT_SHADER )
		KEYWORD_PAIR( GL_COMPUTE_SHADER )
		KEYWORD_PAIR( GL_TESS_CONTROL_SHADER )
        	KEYWORD_PAIR( GL_TESS_EVALUATION_SHADER )
		KEYWORD_PAIR( GL_GEOMETRY_SHADER )

		// Errors, (no-error)
		KEYWORD_PAIR( GL_INVALID_ENUM )
		KEYWORD_PAIR( GL_INVALID_VALUE )
		KEYWORD_PAIR( GL_INVALID_OPERATION )
		KEYWORD_PAIR( GL_STACK_OVERFLOW )
		KEYWORD_PAIR( GL_STACK_UNDERFLOW )
		KEYWORD_PAIR( GL_OUT_OF_MEMORY )
		KEYWORD_PAIR( GL_TABLE_TOO_LARGE )
		KEYWORD_PAIR( GL_INVALID_FRAMEBUFFER_OPERATION_EXT )

		KEYWORD_PAIR( GL_FRAMEBUFFER_COMPLETE )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER )
		KEYWORD_PAIR( GL_FRAMEBUFFER_UNSUPPORTED )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE )
		KEYWORD_PAIR( GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS )

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

		// Vertex Array :
		KEYWORD_PAIR( GL_VERTEX_ARRAY )
		KEYWORD_PAIR( GL_NORMAL_ARRAY )
		KEYWORD_PAIR( GL_COLOR_ARRAY )
		KEYWORD_PAIR( GL_INDEX_ARRAY )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY )
		KEYWORD_PAIR( GL_EDGE_FLAG_ARRAY )
		KEYWORD_PAIR( GL_VERTEX_ARRAY_SIZE )
		KEYWORD_PAIR( GL_VERTEX_ARRAY_TYPE )
		KEYWORD_PAIR( GL_VERTEX_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_NORMAL_ARRAY_TYPE )
		KEYWORD_PAIR( GL_NORMAL_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_COLOR_ARRAY_SIZE )
		KEYWORD_PAIR( GL_COLOR_ARRAY_TYPE )
		KEYWORD_PAIR( GL_COLOR_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_INDEX_ARRAY_TYPE )
		KEYWORD_PAIR( GL_INDEX_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY_SIZE )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY_TYPE )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_EDGE_FLAG_ARRAY_STRIDE )
		KEYWORD_PAIR( GL_VERTEX_ARRAY_POINTER )
		KEYWORD_PAIR( GL_NORMAL_ARRAY_POINTER )
		KEYWORD_PAIR( GL_COLOR_ARRAY_POINTER )
		KEYWORD_PAIR( GL_INDEX_ARRAY_POINTER )
		KEYWORD_PAIR( GL_TEXTURE_COORD_ARRAY_POINTER )
		KEYWORD_PAIR( GL_EDGE_FLAG_ARRAY_POINTER )

		// Comparison
		KEYWORD_PAIR( GL_LEQUAL )
		KEYWORD_PAIR( GL_GEQUAL )
		KEYWORD_PAIR( GL_LESS )
		KEYWORD_PAIR( GL_GREATER )
		KEYWORD_PAIR( GL_EQUAL )
		KEYWORD_PAIR( GL_NOTEQUAL )
		KEYWORD_PAIR( GL_ALWAYS )
		KEYWORD_PAIR( GL_NEVER )

		// Function
		KEYWORD_PAIR( GL_FUNC_ADD )
		KEYWORD_PAIR( GL_FUNC_SUBTRACT )
		KEYWORD_PAIR( GL_FUNC_REVERSE_SUBTRACT )
		KEYWORD_PAIR( GL_MIN )
		KEYWORD_PAIR( GL_MAX )

		// Blending
		KEYWORD_PAIR( GL_SRC_COLOR )
		KEYWORD_PAIR( GL_ONE_MINUS_SRC_COLOR )
		KEYWORD_PAIR( GL_DST_COLOR )
		KEYWORD_PAIR( GL_ONE_MINUS_DST_COLOR )
		KEYWORD_PAIR( GL_SRC_ALPHA )
		KEYWORD_PAIR( GL_ONE_MINUS_SRC_ALPHA )
		KEYWORD_PAIR( GL_DST_ALPHA )
		KEYWORD_PAIR( GL_ONE_MINUS_DST_ALPHA )
		KEYWORD_PAIR( GL_CONSTANT_COLOR )
		KEYWORD_PAIR( GL_ONE_MINUS_CONSTANT_COLOR )
		KEYWORD_PAIR( GL_CONSTANT_ALPHA )
		KEYWORD_PAIR( GL_ONE_MINUS_CONSTANT_ALPHA )
		KEYWORD_PAIR( GL_SRC_ALPHA_SATURATE )

		// Pipeline parts
		KEYWORD_PAIR( GL_CLEAR )
		KEYWORD_PAIR( GL_BLEND )
		KEYWORD_PAIR( GL_DEPTH_TEST )
		KEYWORD_PAIR( GL_STENCIL_TEST )

		// Render : 
		KEYWORD_PAIR( GL_FEEDBACK )
		KEYWORD_PAIR( GL_RENDER )
		KEYWORD_PAIR( GL_SELECT )

		#undef KEYWORD_PAIR
	};

	/**
	\related HandleOpenGL
	\fn std::string Glip::CoreGL::getGLEnumName(const GLenum& p)
	\brief Get the parameter name as a string.
	\param p The GLenum parameter.
	\return A standard string.
	**/
	std::string Glip::CoreGL::getGLEnumName(const GLenum& p)
	{
		const int numTokens = static_cast<int>(sizeof(HandleOpenGL::glKeywords)/sizeof(HandleOpenGL::KeywordPair));

		// Mixed name :
		if(p==GL_POINTS || p==GL_ZERO || p==GL_FALSE)
			return "GL_POINTS/GL_ZERO/GL_NONE";

		for(int i=0; i<numTokens; i++)
		{
			if(HandleOpenGL::glKeywords[i].value==p)
				return HandleOpenGL::glKeywords[i].name;
		}

		throw Exception("Unknown GLenum code : " + toString(p) + ".", __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\related HandleOpenGL
	\fn std::string Glip::CoreGL::getGLEnumNameSafe(const GLenum& p)
	\brief Get the parameter name as a string.
	\param p The GLenum parameter.
	\return A standard string or <Unknown:ID> where ID is the decimal representation of the enum.
	**/
	std::string Glip::CoreGL::getGLEnumNameSafe(const GLenum& p) throw()
	{
		const int numTokens = static_cast<int>(sizeof(HandleOpenGL::glKeywords)/sizeof(HandleOpenGL::KeywordPair));

		// Mixed name :
		if(p==GL_POINTS || p==GL_ZERO || p==GL_FALSE)
			return "GL_POINTS/GL_ZERO/GL_NONE";

		for(int i=0; i<numTokens; i++)
		{
			if(HandleOpenGL::glKeywords[i].value==p)
				return HandleOpenGL::glKeywords[i].name;
		}

		return std::string("<Unknown:") + toString(p) + ">";
	}

	/**
	\related HandleOpenGL
	\fn GLenum Glip::CoreGL::getGLEnum(const std::string& s)
	\brief Get the parameter from its name in a string.
	\param s The GLenum parameter name.
	\return The corresponding enum or throw an exception if no enum was found.
	**/
	GLenum Glip::CoreGL::getGLEnum(const std::string& s)
	{
		const int numTokens = static_cast<int>(sizeof(HandleOpenGL::glKeywords)/sizeof(HandleOpenGL::KeywordPair));

		for(int i=0; i<numTokens; i++)
		{
			if(HandleOpenGL::glKeywords[i].name==s)
				return HandleOpenGL::glKeywords[i].value;
		}

		throw Exception("Unknown GLenum keyword : \"" + s + "\".", __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\related HandleOpenGL
	\fn bool Glip::CoreGL::belongsToGLEnums(const GLenum& p, const GLenum* l, const size_t s)
	\brief Test if a symbol belongs to a list of possible symbols.
	\param p Symbol to be tested.
	\param l List of symbols.
	\param s Size of the list, in bytes (from sizeof, for instance).
	\return True if the symbol belongs to the list.

	Sample usage :
	\code
	const GLenum[] list = {GL_RED, GL_LUMINANCE, GL_RGB, GL_RGBA};
	bool test = belongsToGLEnums(symbol, list, sizeof(list));
	\endcode
	**/
	bool Glip::CoreGL::belongsToGLEnums(const GLenum& p, const GLenum* l, const size_t s)
	{
		const size_t t = s/sizeof(GLenum);
		return (std::find(l, l+t, p)!=(l+t));
	}

