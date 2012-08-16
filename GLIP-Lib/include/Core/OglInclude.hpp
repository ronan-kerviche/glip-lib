/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
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

	#ifdef _WIN32
		#define WINDOWS_LEAN_AND_MEAN
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
											throw Exception("OpenGL Extension " #ext " is required but not available on Hardware or Driver.",__FILE__,__LINE__);

			#define FIX_MISSING_GLEW_CALL(call, replacement) 	if(call==NULL) \
										{ \
											std::cerr << "OpenGL Function " #call " is required but GLEW failed to load it, the library will (try to) use " #replacement " instead." << std::endl; \
											if(replacement==NULL) \
												throw Exception("OpenGL Functions " #call " and " #replacement " aren't available according to GLEW.", __FILE__, __LINE__); \
											call = replacement; \
										}

			// Object
			/**
			\class HandleOpenGL
			\brief Main OpenGL Handle
			**/
			class HandleOpenGL
			{
				private :
					static bool initDone;
					HandleOpenGL(void); //no Instance
				public :
					static void init(void);
			};

			// Tools
			std::string glErrorToString(void);
			bool glErrors(bool verbose = true, bool quietSituations = true);
			void glDebug(void);
			std::string glParamName(GLenum param);
			GLenum gl_from_string(const std::string& name);
		}
	}

#endif

