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
		#define GLEW_STATIC
		#include "glew.h"
		#ifdef __GLIP_USE_GLES__
			#include <GLES/gl.h>
		#else
			#include <GL/gl.h>
		#endif
	#endif

	#if defined(linux) || defined(__linux) || defined(__linux__)
		#define GLEW_STATIC
		#include "glew.h"
		#ifdef __GLIP_USE_GLES__
			#include <GLES/gl.h>
		#else
			#include <GL/gl.h>
		#endif
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

			#define GLIP_NUM_SHADER_TYPES (6)

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
						GLenum 			value;
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

