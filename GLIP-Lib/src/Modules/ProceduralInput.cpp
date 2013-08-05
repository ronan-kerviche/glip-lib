/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
	#include "Core/Exception.hpp"
	#include "Modules/ProceduralInput.hpp"
	#include "devDebugTools.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	/**
	\fn ProceduralInput::ProceduralInput(const __ReadOnly_HdlTextureFormat& fmt, const ShaderSource& fragment, const std::string& name)
	\brief ProceduralInput constructor.
	\param fmt Format ot the output.
	\param fragment Source of the fragment to be used.
	\param name Name of this Component instance.
	**/
	ProceduralInput::ProceduralInput(const __ReadOnly_HdlTextureFormat& fmt, const ShaderSource& fragment, const std::string& name)
	 : __ReadOnly_ComponentLayout(getLayout(fragment)), InputDevice(getLayout(fragment), name), fragmentShader(NULL), vertexShader(NULL), quad(GeometryPrimitives::StandardQuad(), GL_STATIC_DRAW_ARB), program(NULL), renderer(NULL)
	{
		ShaderSource* vertex=NULL;

		std::vector<std::string> varsOut = fragment.getOutputVars();

		std::string vertexSrc = 	"void main() \n { \n    gl_FrontColor  = gl_Color; \n"
						"    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \n"
						"    gl_Position = gl_ModelViewMatrix * gl_Vertex; \n } \n";

		try
		{
			vertex		= new ShaderSource(vertexSrc);
			fragmentShader	= new HdlShader(GL_FRAGMENT_SHADER, fragment);
			vertexShader 	= new HdlShader(GL_VERTEX_SHADER, *vertex);
			program        	= new HdlProgram(*vertexShader, *fragmentShader);
			delete vertex;

			if(!fragmentShader->requiresCompatibility())
				program->setFragmentLocation(varsOut[0], 0);

			// Create a buffer :
			renderer	= new HdlFBO(fmt, varsOut.size());

			// Link the buffer to the input device :
			for(int i=0; i<varsOut.size(); i++)
				setTextureLink((*renderer)[i],i);
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

	ProceduralInput::~ProceduralInput(void)
	{
		delete fragmentShader;
		delete vertexShader;
		delete program;
	}

	InputDevice::InputDeviceLayout ProceduralInput::getLayout(const ShaderSource& fragment)
	{
		std::vector<std::string> varsOut = fragment.getOutputVars();

		InputDeviceLayout l("ProceduralInput");

		if(varsOut.size()==0)
			throw Exception("ProceduralInput::ProceduralInput - The Fragment shader must have at least one output (none found).", __FILE__, __LINE__);

		for(int i=0; i<varsOut.size(); i++)
			l.addOutputPort(varsOut[i]);

		return InputDeviceLayout(l);
	}

	/**
	\fn void ProceduralInput::generateNewFrame(void)
	\brief Generate a new image.
	**/
	void ProceduralInput::generateNewFrame(void)
	{
		// Prepare the renderer
			renderer->beginRendering(); // all of the outputs...

		// Enable states
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Prepare geometry
			glLoadIdentity();

		// Load the shader
			program->use();

		// Draw
			quad.draw();

		// Stop using the shader
			HdlProgram::stopProgram();

		// End rendering
			renderer->endRendering();

		declareNewImageOnAllPorts();
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
