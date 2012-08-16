/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ProceduralInput.cpp                                                                       */
/*     Original Date : December 22th 2011                                                                        */
/*                                                                                                               */
/*     Description   : Input modules for generating textures from direct computation and few parameters.         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ProceduralInput.cpp
 * \brief   Input modules for generating textures from direct computation and few parameters.
 * \author  R. KERVICHE
 * \date    May 23th 2012
**/

	// Includes
	#include "Exception.hpp"
	#include "ProceduralInput.hpp"
	#include "devDebugTools.hpp"
	#include "HdlTexture.hpp"
	#include "HdlVBO.hpp"
	#include "HdlFBO.hpp"
	#include "HdlShader.hpp"
	#include "Filter.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	/**
	\fn ProceduralInput::ProceduralInput(const std::string& name, const __ReadOnly_HdlTextureFormat& fmt, ShaderSource& fragment)
	\brief ProceduralInput constructor.
	\param name Name of the component.
	\param fmt Format ot the output.
	\param fragment Source of the fragment to be used.
	**/
	ProceduralInput::ProceduralInput(const std::string& name, const __ReadOnly_HdlTextureFormat& fmt, ShaderSource& fragment)
	 : InputDevice(name), fragmentShader(NULL), vertexShader(NULL), program(NULL), vbo(NULL), renderer(NULL)
	{
		ShaderSource* vertex=NULL;

		std::vector<std::string> varsOut = fragment.getOutputVars();

		std::string vertexSrc = 	"void main() \n { \n    gl_FrontColor  = gl_Color; \n"
						"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \n"
						"    gl_Position = gl_ModelViewMatrix * gl_Vertex; \n } \n";

		if(varsOut.size()!=1)
			throw Exception("ProceduralInput::ProceduralInput - The Fragment shader must have only one output (" + to_string(varsOut.size()) + " were found).", __FILE__, __LINE__);

		try
		{
			vertex		= new ShaderSource(vertexSrc);
			fragmentShader	= new HdlShader(GL_FRAGMENT_SHADER, fragment);
			vertexShader 	= new HdlShader(GL_VERTEX_SHADER, *vertex);
			program        	= new HdlProgram(*vertexShader, *fragmentShader);
			delete vertex;

			if(!fragmentShader->requiresCompatibility())
				program->setFragmentLocation(varsOut[0], 0);

			// Create a basic geometry :
			vbo = HdlVBO::generate2DStandardQuad();

			// Create a buffer :
			renderer	= new HdlFBO(fmt);

			// Link the buffer to the input device :
			setTextureLink((*renderer)[0]);
		}
		catch(Exception& e)
		{
			Exception m("ProceduralInput::ProceduralInput - Caught an exception while creating the shaders for " + name + ".", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("ProceduralInput::ProceduralInput - Caught an exception while creating the shaders for " + name + ".", __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn void ProceduralInput::generateNewFrame(void)
	\brief Generate a new image.
	**/
	void ProceduralInput::generateNewFrame(void)
	{
		// Prepare the renderer
			renderer->beginRendering(1);

		// Enable states
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Prepare geometry
			glLoadIdentity();

		// Load the shader
			program->use();

		// Draw
			vbo->draw();

		// Stop using the shader
			HdlProgram::stopProgram();

		// End rendering
			renderer->endRendering();

		declareNewImage();
	}

	/**
	\fn HdlProgram& ProceduralInput::prgm(void)
	\brief Access the program.
	\return A reference to the program, modifications are allowed.
	**/
	HdlProgram& ProceduralInput::prgm(void)
	{
		return *program;
	}
