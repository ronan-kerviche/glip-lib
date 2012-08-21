/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     AUTHOR        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     VERSION       : 0.7                                                                                       */
/*     LICENSE       : MIT License                                                                               */
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
			#include "Core/OglInclude.hpp"

		// Objects :
			// Tools
			#include "Core/Exception.hpp"

			// GL wrappers
			#include "Core/ShaderSource.hpp"
			#include "Core/HdlTexture.hpp"
			#include "Core/HdlFBO.hpp"
			#include "Core/HdlGeBO.hpp"
			#include "Core/HdlPBO.hpp"
			#include "Core/HdlShader.hpp"
			#include "Core/HdlVBO.hpp"

			// Pipeline Tools
			#include "Core/Filter.hpp"
			#include "Core/Pipeline.hpp"
			#include "Core/HdlInfo.hpp"
			#include "Core/Devices.hpp"

			// Modules
			#include "Modules/Modules.hpp"
	#endif

#endif
