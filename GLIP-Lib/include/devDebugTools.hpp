/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : devDebugTools.hpp                                                                         */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Development and debugging tools                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    devDebugTools.hpp
 * \brief   Development and debugging tools
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __DEV_DEBUG_TOOLS__
#define __DEV_DEBUG_TOOLS__

	// Development flags
		//#define __GLIPLIB_VERBOSE__
		//#define __GLIPLIB_DEVELOPMENT_VERBOSE__
		//#define __GLIPLIB_TRACK_GL_ERRORS__

	// Tools
		#define STR_EXPAND(tok) 	#tok
		#define STR(tok) 		STR_EXPAND(tok)
		#define __HERE__		"[" STR(__FUNCTION__) " at " STR(__FILE__) " : " STR(__LINE__) "] "

		#define OPENGL_ERROR_TRACKER(functionName, callLine) \
			{ \
				bool caughtError; \
				std::string errDescription = Glip::CoreGL::glErrorToString(&caughtError); \
				if(caughtError) \
				{ \
					std::cout << functionName << " / " << callLine << " - Opengl error : " << errDescription << std::endl; \
					Glip::CoreGL::glDebug(); \
				} \
			}

#endif
