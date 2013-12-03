/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ProceduralInput.hpp                                                                       */
/*     Original Date : December 22th 2011                                                                        */
/*                                                                                                               */
/*     Description   : Input modules for generating textures from direct computation and few parameters.         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ProceduralInput.hpp
 * \brief   Input modules for generating textures from direct computation and few parameters.
 * \author  R. KERVICHE
 * \date    May 23th 2012
**/

#ifndef __PROCEDURAL_INPUT_INCLUDE__
#define __PROCEDURAL_INPUT_INCLUDE__

	// Includes
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Core/Devices.hpp"
	#include "Core/HdlTexture.hpp"
	#include "Core/HdlVBO.hpp"
	#include "Core/HdlFBO.hpp"
	#include "Core/HdlShader.hpp"
	#include "Core/Filter.hpp"
	#include "Core/Geometry.hpp"

namespace Glip
{
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		// Structure
		/**
		\class ProceduralInput
		\brief Generate texture from a set of few parameters.
		**/
		class GLIP_API ProceduralInput : public InputDevice
		{
			private :
				// Data :
				HdlShader*		fragmentShader;
				HdlShader*		vertexShader;
				HdlProgram*		program;
				GeometryInstance 	quad;
				HdlFBO*			renderer;

				static InputDeviceLayout getLayout(const ShaderSource& fragment);

			public :
				ProceduralInput(const __ReadOnly_HdlTextureFormat& fmt, const ShaderSource& fragment, const std::string& name);
				virtual ~ProceduralInput(void);

				// Tools :
				void generateNewFrame(void);
				HdlProgram& prgm(void);
		};
	}
}

#endif
