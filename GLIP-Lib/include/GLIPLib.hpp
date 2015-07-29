/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     AUTHOR        : R. Kerviche                                                                               */
/*     VERSION       : 0.7                                                                                       */
/*     LICENSE       : MIT License                                                                               */
/*     DATE          : August   23rd 2011                                                                        */
/*     Original Date : August    7th 2010                                                                        */
/*     Website       : glip-lib.net                                                                              */
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
			#include "Core/Geometry.hpp"
			#include "Core/Filter.hpp"
			#include "Core/Pipeline.hpp"

			// Modules
			#include "Modules/Modules.hpp"
	#endif

#endif
