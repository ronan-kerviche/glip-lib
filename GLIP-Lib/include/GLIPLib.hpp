/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     AUTHOR        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     VERSION       : 0.6                                                                                       */
/*     LICENSE       : GPLv3                                                                                     */
/*     DATE          : August   23rd 2011                                                                        */
/*     Original Date : August    7th 2010                                                                        */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GLIPLib.hpp                                                                               */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    GLIPLib.hpp
 * \brief   Main header of the GLIP-Library
 * \author  R. KERVICHE
 * \version 0.7
 * \date    August 23rd 2011
*/

#ifndef __GLIP_LIB_INCLUDE__
#define __GLIP_LIB_INCLUDE__

	#ifndef __cplusplus
		#error A C++ compiler is required!
	#else

		// Standard
			#include <string>

		// GL include
			#include "OglInclude.hpp"

		// Objects :
			// Tools
			#include "Exception.hpp"

			// GL wrappers
			#include "ShaderSource.hpp"
			#include "HdlTexture.hpp"
			#include "HdlFBO.hpp"
			#include "HdlGeBO.hpp"
			#include "HdlPBO.hpp"
			#include "HdlShader.hpp"
			#include "HdlVBO.hpp"

			// Pipeline Tools
			#include "Filter.hpp"
			#include "Pipeline.hpp"
			#include "HdlInfo.hpp"

			// Modules
			//#define GLIP_LOAD_ALL_MODULES
			#define GLIP_DONT_LOAD_ANY_MODULE
			#include "Modules.hpp"
	#endif

#endif

