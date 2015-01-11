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
		#define GLIP_STR_EXPAND(tok) 	#tok
		#define GLIP_STR(tok) 		GLIP_STR_EXPAND(tok)
		#define __HERE__		"[" GLIP_STR(__FUNCTION__) " at " GLIP_STR(__FILE__) " : " GLIP_STR(__LINE__) "] "

		#define OPENGL_ERROR_TRACKER(functionName, callLine) \
			{ \
				GLenum err = glGetError(); \
				if(err!=GL_NO_ERROR) \
				{ \
					const std::string description = getGLErrorDescription(err); \
					std::cerr << functionName << " / " << callLine << " - Opengl error : " << description << std::endl; \
					Glip::CoreGL::debugGL(); \
				} \
			}

#endif
