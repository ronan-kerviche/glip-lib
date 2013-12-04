/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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
	#include "devDebugTools.hpp"
	#include "Core/LibTools.hpp"

	#ifdef _WIN32
		#define WINDOWS_LEAN_AND_MEAN
		#define NOMINMAX		
		#include <windows.h>
		#define GLEW_STATIC
		#include "glew.h"
		#include <GL/gl.h>
	#endif

	#if defined(linux) || defined(__linux) || defined(__linux__)
		#define GLEW_STATIC
		#include "glew.h"
		#include <GL/gl.h>
	#endif

	namespace Glip
	{
		namespace CoreGL
		{
			// Macros
			#define NEED_EXTENSION(ext) 				if(!ext) \
											throw Exception("OpenGL Extension " #ext " is required but not available on Hardware or Driver (updating the driver might help).",__FILE__,__LINE__);

			#define FIX_MISSING_GLEW_CALL(call, replacement) 	if(call==NULL) \
										{ \
											std::cerr << "OpenGL Function " #call " is required but GLEW failed to load it, the library will (try to) use " #replacement " instead." << std::endl; \
											if(replacement==NULL) \
												throw Exception("OpenGL Functions " #call " and " #replacement " aren't available according to GLEW (updating the driver might help).", __FILE__, __LINE__); \
											call = replacement; \
										}

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

				private :
					// OpenGL keyword :
					struct GLIP_API KeywordPair
					{
						GLenum 		value;
						std::string 	name;
					};

					static HandleOpenGL		*instance;
					static SupportedVendor 		vendor;
					static const KeywordPair 	glKeywords[];

				public :
					HandleOpenGL(void);
					~HandleOpenGL(void);

					static void		init(void);
					static void		deinit(void);
					static bool		isInitialized(void);
					static SupportedVendor 	getVendorID(void);
					static std::string 	getVendorName(void);
					static std::string 	getRendererName(void);
					static std::string 	getVersion(void);
					static std::string 	getGLSLVersion(void);

					// Friend functions :
					GLIP_API friend std::string glParamName(GLenum);
					GLIP_API friend GLenum glFromString(const std::string&);
			};

			// Tools
			GLIP_API_FUNC std::string	glErrorToString(bool* caughtError = NULL);
			GLIP_API_FUNC bool 		glErrors(bool verbose = true, bool quietSituations = true);
			GLIP_API_FUNC void 		glDebug(void);
			GLIP_API_FUNC std::string 	glParamName(GLenum param);
			GLIP_API_FUNC GLenum 		glFromString(const std::string& name);
		}
	}

#endif

