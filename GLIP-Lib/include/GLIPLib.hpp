/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     AUTHOR        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     VERSION       : 0.7                                                                                       */
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

/* ************************************************************************************************************* */
/**
	\mainpage OpenGL Image Processing Library (GLIP-Lib)

	\section sec_intro Introduction
		GLIP-LIB : OpenGL Image Processing LIBrary The library provides some tools to design image processing pipelines based on hardware accelerated computing (GPGPU) via OpenGL and GLSL.

	 \section sec_license License
		Copyright (C) <2011>  <Ronan Kerviche>

		This program is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program.  If not, see <http://www.gnu.org/licenses/>.

	\section sec_install Installation
		Todo

	\section sec_f_example First example
		Todo

	\section sec_pipeline_scripts Pipeline scripts
		Todo
**/
/* ************************************************************************************************************* */
