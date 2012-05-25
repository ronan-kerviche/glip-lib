/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
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

	// Include :
	#include "../Core/OglInclude.hpp"
	#include "../Core/Devices.hpp"

namespace Glip
{
	// Prototypes
	namespace CoreGL
	{
		class __ReadOnly_HdlTextureFormat;
		class HdlVBO;
		class HdlFBO;
		class ShaderSource;
		class HdlShader;
		class HdlProgram;
	}

	namespace CorePipeline
	{
		class OutputDevice;
	}

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		// Structure
		/**
		\class ProceduralInput
		\brief Generate texture from a set of few parameters.
		**/
		class ProceduralInput : public InputDevice
		{
			private :
				// Data :
				HdlShader*	fragmentShader;
				HdlShader*	vertexShader;
				HdlProgram*	program;
				HdlVBO*		vbo;
				HdlFBO*		renderer;

			public :
				ProceduralInput(const std::string& name, const __ReadOnly_HdlTextureFormat& fmt, ShaderSource& fragment);

				// Tools :
				void generateNewFrame(void);
				HdlProgram& prgm(void);
		};
	}
}

#endif
