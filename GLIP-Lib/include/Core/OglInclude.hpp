/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : OglInclude.hpp                                                                            */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : OpenGL includes and tools                                                                 */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    OglInclude.hpp
 * \brief   OpenGL includes and tools
 * \author  R. KERVICHE
 * \date    October 17th 2010
**/

#ifndef __OGLINCLUDE_INCLUDE__
#define __OGLINCLUDE_INCLUDE__

	#include <iostream>
	#include <vector>
	#include "devDebugTools.hpp"
	#include "Core/LibTools.hpp"

	#ifdef _WIN32
		#define WINDOWS_LEAN_AND_MEAN
		#define NOMINMAX		
		#include <windows.h>	
	#endif

	#if defined(linux) || defined(__linux) || defined(__linux__)
		// No specifics...
	#endif

	// Include OpenGL/ES :
	#if defined(GLIP_USE_GLES)
		#if defined(GLIP_USE_GLES2) || defined(GLIP_USE_GLES3) || defined(GLIP_USE_GL)
			#error "Multiple bindings selected among GL, GLES, GLES2 and GLES3."
		#endif
		#error Test!
		#include <GLES/gl.h>
		#include <GLES/glext.h>
	#elif defined(GLIP_USE_GLES2)
		#if defined(GLIP_USE_GLES3) || defined(GLIP_USE_GL)
			#error "Multiple bindings selected among GL, GLES, GLES2 and GLES3."
		#endif
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#elif defined(GLIP_USE_GLES3)
		#if defined(GLIP_USE_GL)
			#error "Multiple bindings selected among GL, GLES, GLES2 and GLES3."
		#endif
		#include <GLES3/gl31.h>
		#include <GLES3/gl3ext.h>
	#else
		#ifndef GLIP_USE_GL
			#define GLIP_USE_GL
		#endif
		#ifdef GLIP_USE_EXTERNAL_GLEW
			// Can only be set when compiling the library.
			#include <glew.h>
		#else
			#define GLEW_STATIC
			#include "glew.h"
		#endif
		#include <GL/gl.h>
	#endif

	// Fix missing defines :
	#ifndef GL_TEXTURE0
		#define GL_TEXTURE0 GL_TEXTURE0_ARB
	#endif
	#ifndef GL_TEXTURE1
		#define GL_TEXTURE1 GL_TEXTURE1_ARB
	#endif
	#ifndef GL_TEXTURE2
		#define GL_TEXTURE2 GL_TEXTURE2_ARB
	#endif
	#ifndef GL_TEXTURE3
		#define GL_TEXTURE3 GL_TEXTURE3_ARB
	#endif
	#ifndef GL_TEXTURE4
		#define GL_TEXTURE4 GL_TEXTURE4_ARB
	#endif
	#ifndef GL_TEXTURE5
		#define GL_TEXTURE5 GL_TEXTURE5_ARB
	#endif
	#ifndef GL_TEXTURE6
		#define GL_TEXTURE6 GL_TEXTURE6_ARB
	#endif
	#ifndef GL_TEXTURE7
		#define GL_TEXTURE7 GL_TEXTURE7_ARB
	#endif
	#ifndef GL_TEXTURE8
		#define GL_TEXTURE8 GL_TEXTURE8_ARB
	#endif
	#ifndef GL_TEXTURE9
		#define GL_TEXTURE9 GL_TEXTURE9_ARB
	#endif
	#ifndef GL_TEXTURE10
		#define GL_TEXTURE10 GL_TEXTURE10_ARB
	#endif
	#ifndef GL_TEXTURE11
		#define GL_TEXTURE11 GL_TEXTURE11_ARB
	#endif
	#ifndef GL_TEXTURE12
		#define GL_TEXTURE12 GL_TEXTURE12_ARB
	#endif
	#ifndef GL_TEXTURE13
		#define GL_TEXTURE13 GL_TEXTURE13_ARB
	#endif
	#ifndef GL_TEXTURE14
		#define GL_TEXTURE14 GL_TEXTURE14_ARB
	#endif
	#ifndef GL_TEXTURE15
		#define GL_TEXTURE15 GL_TEXTURE15_ARB
	#endif
	#ifndef GL_TEXTURE16
		#define GL_TEXTURE16 GL_TEXTURE16_ARB
	#endif
	#ifndef GL_TEXTURE17
		#define GL_TEXTURE17 GL_TEXTURE17_ARB
	#endif
	#ifndef GL_TEXTURE18
		#define GL_TEXTURE18 GL_TEXTURE18_ARB
	#endif
	#ifndef GL_TEXTURE19
		#define GL_TEXTURE19 GL_TEXTURE19_ARB
	#endif
	#ifndef GL_TEXTURE20
		#define GL_TEXTURE20 GL_TEXTURE20_ARB
	#endif
	#ifndef GL_TEXTURE21
		#define GL_TEXTURE21 GL_TEXTURE21_ARB
	#endif
	#ifndef GL_TEXTURE22
		#define GL_TEXTURE22 GL_TEXTURE22_ARB
	#endif
	#ifndef GL_TEXTURE23
		#define GL_TEXTURE23 GL_TEXTURE23_ARB
	#endif
	#ifndef GL_TEXTURE24
		#define GL_TEXTURE24 GL_TEXTURE24_ARB
	#endif
	#ifndef GL_TEXTURE25
		#define GL_TEXTURE25 GL_TEXTURE25_ARB
	#endif
	#ifndef GL_TEXTURE26
		#define GL_TEXTURE26 GL_TEXTURE26_ARB
	#endif
	#ifndef GL_TEXTURE27
		#define GL_TEXTURE27 GL_TEXTURE27_ARB
	#endif
	#ifndef GL_TEXTURE28
		#define GL_TEXTURE28 GL_TEXTURE28_ARB
	#endif
	#ifndef GL_TEXTURE29
		#define GL_TEXTURE29 GL_TEXTURE29_ARB
	#endif
	#ifndef GL_TEXTURE30
		#define GL_TEXTURE30 GL_TEXTURE30_ARB
	#endif
	#ifndef GL_TEXTURE31
		#define GL_TEXTURE31 GL_TEXTURE31_ARB
	#endif

	#ifndef GL_PIXEL_UNPACK_BUFFER
		#define GL_PIXEL_UNPACK_BUFFER GL_PIXEL_UNPACK_BUFFER_ARB
	#endif
	#ifndef GL_PIXEL_PACK_BUFFER
		#define GL_PIXEL_PACK_BUFFER GL_PIXEL_PACK_BUFFER_ARB
	#endif
	#ifndef GL_ARRAY_BUFFER
		#define GL_ARRAY_BUFFER GL_ARRAY_BUFFER_ARB
	#endif
	#ifndef GL_ELEMENT_ARRAY_BUFFER
		#define GL_ELEMENT_ARRAY_BUFFER GL_ELEMENT_ARRAY_BUFFER_ARB
	#endif
	#ifndef GL_WRITE_ONLY
		#define GL_WRITE_ONLY GL_WRITE_ONLY_ARB
	#endif
	#ifndef GL_READ_ONLY
		#define GL_READ_ONLY GL_READ_ONLY_ARB
	#endif
	#ifndef GL_STATIC_DRAW
		#define GL_STATIC_DRAW GL_STATIC_DRAW_ARB
	#endif
	#ifndef GL_STATIC_READ
		#define GL_STATIC_READ GL_STATIC_READ_ARB
	#endif
	#ifndef GL_STATIC_COPY
		#define GL_STATIC_COPY GL_STATIC_COPY_ARB
	#endif
	#ifndef GL_DYNAMIC_DRAW
		#define GL_DYNAMIC_DRAW GL_DYNAMIC_DRAW_ARB
	#endif
	#ifndef GL_DYNAMIC_READ
		#define GL_DYNAMIC_READ GL_DYNAMIC_READ_ARB
	#endif
	#ifndef GL_DYNAMIC_COPY
		#define GL_DYNAMIC_COPY GL_DYNAMIC_COPY_ARB
	#endif
	#ifndef GL_STREAM_DRAW
		#define GL_STREAM_DRAW GL_STREAM_DRAW_ARB
	#endif
	#ifndef GL_STREAM_READ
		#define GL_STREAM_READ GL_STREAM_READ_ARB
	#endif
	#ifndef GL_STREAM_COPY
		#define GL_STREAM_COPY GL_STREAM_COPY_ARB
	#endif	

	#ifndef GL_FRAMEBUFFER
		#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
	#endif
	#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
		#define GL_INVALID_FRAMEBUFFER_OPERATION GL_INVALID_FRAMEBUFFER_OPERATION_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT0
		#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT1
		#define GL_COLOR_ATTACHMENT1 GL_COLOR_ATTACHMENT1_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT2
		#define GL_COLOR_ATTACHMENT2 GL_COLOR_ATTACHMENT2_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT3
		#define GL_COLOR_ATTACHMENT3 GL_COLOR_ATTACHMENT3_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT4
		#define GL_COLOR_ATTACHMENT4 GL_COLOR_ATTACHMENT4_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT5
		#define GL_COLOR_ATTACHMENT5 GL_COLOR_ATTACHMENT5_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT6
		#define GL_COLOR_ATTACHMENT6 GL_COLOR_ATTACHMENT6_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT7
		#define GL_COLOR_ATTACHMENT7 GL_COLOR_ATTACHMENT7_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT8
		#define GL_COLOR_ATTACHMENT8 GL_COLOR_ATTACHMENT8_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT9
		#define GL_COLOR_ATTACHMENT9 GL_COLOR_ATTACHMENT9_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT10
		#define GL_COLOR_ATTACHMENT10 GL_COLOR_ATTACHMENT10_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT11
		#define GL_COLOR_ATTACHMENT11 GL_COLOR_ATTACHMENT11_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT12
		#define GL_COLOR_ATTACHMENT12 GL_COLOR_ATTACHMENT12_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT13
		#define GL_COLOR_ATTACHMENT13 GL_COLOR_ATTACHMENT13_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT14
		#define GL_COLOR_ATTACHMENT14 GL_COLOR_ATTACHMENT14_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT15
		#define GL_COLOR_ATTACHMENT15 GL_COLOR_ATTACHMENT15_EXT
	#endif

	// Force definition for these :
	#ifndef GL_CLEAR
		#define GL_CLEAR 0x1500
	#endif
	#ifndef GL_FEEDBACK
		#define GL_FEEDBACK 0x1C01
	#endif
	#ifndef GL_RENDER
		#define GL_RENDER 0x1C00
	#endif
	#ifndef GL_SELECT
		#define GL_SELECT 0x1C02
	#endif

	namespace Glip
	{
		namespace CoreGL
		{
			// Macros
			#define NEED_EXTENSION(ext) 				if(!ext) \
											throw Exception("OpenGL Extension " #ext " is required but not available on Hardware or Driver (updating the driver might help).",__FILE__,__LINE__,Exception::GLException);

			#define FIX_MISSING_GLEW_CALL(call, replacement) 	if(call==NULL) \
										{ \
											std::cerr << "OpenGL Function " #call " is required but GLEW failed to load it, the library will (try to) use " #replacement " instead." << std::endl; \
											if(replacement==NULL) \
												throw Exception("OpenGL Functions " #call " and " #replacement " aren't available according to GLEW (updating the driver might help).", __FILE__, __LINE__,Exception::GLException); \
											call = replacement; \
										}

			#ifdef GLIP_USE_GL
				#define GLIP_NUM_SHADER_TYPES (6)
			#else
				#define GLIP_NUM_SHADER_TYPES (3)
			#endif

			// Object
			/**
			\class HandleOpenGL
			\brief Main OpenGL Handle.
			**/
			class GLIP_API HandleOpenGL
			{
				public :
					/// ID of the supported hardware vendors
					enum SupportedVendor
					{
						/// For NVIDIA
						vd_NVIDIA,
						/// For AMD/ATI
						vd_AMDATI,
						/// For INTEL
						vd_INTEL,
						/// When unknown
						vd_UNKNOWN
					};

					/// Number of different shader types.
					static const unsigned int numShaderTypes = GLIP_NUM_SHADER_TYPES; // See GL_SHADER_TYPE and GL_*_SHADER
					/// List (tight) of various shader types.
					static const GLenum shaderTypes[GLIP_NUM_SHADER_TYPES];

				private :
					// OpenGL keyword :
					struct GLIP_API KeywordPair
					{
						const GLenum 		value;
						const std::string 	name;
					};

					static HandleOpenGL		*instance;
					static SupportedVendor 		vendor;
					static const KeywordPair 	glKeywords[];

				public :
					HandleOpenGL(void);
					~HandleOpenGL(void);

					static void init(void);
					static void deinit(void);
					static bool isInitialized(void);
					static SupportedVendor getVendorID(void);
					static std::string getVendorName(void);
					static std::string getRendererName(void);
					static std::string getVersion(void);
					static void getVersion(int& major, int& minor);
					static std::string getGLSLVersion(void);
					static std::vector<std::string> getAvailableGLSLVersions(void);
					static unsigned int getShaderTypeIndex(GLenum shaderType);

					// Friend functions :
					GLIP_API friend std::string getGLEnumName(const GLenum& p);
					GLIP_API friend std::string getGLEnumNameSafe(const GLenum& p) throw();
					GLIP_API friend GLenum getGLEnum(const std::string& s);
			};

			// Tools
			GLIP_API_FUNC std::string	getGLErrorDescription(const GLenum& e);
			GLIP_API_FUNC void 		debugGL(void);
			GLIP_API_FUNC std::string 	getGLEnumName(const GLenum& p);
			GLIP_API_FUNC std::string 	getGLEnumNameSafe(const GLenum& p) throw();
			GLIP_API_FUNC GLenum 		getGLEnum(const std::string& s);
			GLIP_API_FUNC bool		belongsToGLEnums(const GLenum& p, const GLenum* list, const size_t s);

			/**
			\fn template<size_t N> bool belongsToGLEnums(const GLenum& p, const GLenum (&l) [N])
			\related Glip::CoreGL::HandleOpenGL	
			\brief Test if a symbol belongs to a list of possible symbols.
			\param p Symbol to be tested.
			\param l List of symbols.
			\return True if the symbol belongs to the list.

			Sample usage :
			\code
			const GLenum list[] = {GL_RED, GL_LUMINANCE, GL_RGB, GL_RGBA};
			bool test = belongsToGLEnums(symbol, list);
			\endcode
			**/
			template<size_t N>
			bool belongsToGLEnums(const GLenum& p, const GLenum (&list) [N])
			{
				return (std::find(list, list+N, p)!=(list+N));
			}
		}
	}

#endif

