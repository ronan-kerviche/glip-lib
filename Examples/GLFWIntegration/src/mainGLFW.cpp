/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://glip-lib.net/                                                                      */
/*                                                                                                               */
/*     File          : mainGLFW.cpp                                                                              */
/*     Original Date : September 1st 2011                                                                        */
/*                                                                                                               */
/*     Description   : GLFW Based Hello World                                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Compile on Linux with :
// g++ -o mainGLFW src/*.cpp -I<your path>/GLIP-Lib/include/ -L<your path>/GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip

// Includes
	#include <cmath>
	#include "GLIPLib.hpp"
	#include <GL/glfw.h>
	#include <stdlib.h>

// Namespace :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Resizing callback :
	void GLFWCALL WindowResize( int width, int height )
	{
		glViewport(0,0,width,height);
	}

// Tool :
	void randomFill(HdlTexture& texture)
	{
		unsigned char* tmp = new unsigned char[texture.getSize()];
		for(int j=0; j<texture.getSize(); j++)
			tmp[j] = (rand()>0.8*RAND_MAX) ? 255 : 0;
		texture.write(tmp);
		delete[] tmp;
	}

// Main
	int main(int argc, char** argv)
	{
		bool running = true;

		try
		{
			// Initialize GLFW :
			if( !glfwInit() )
			{
				exit( EXIT_FAILURE );
			}

			// Open an OpenGL window :
			const int 	width = 1024,
					height = 768;
			if( !glfwOpenWindow(width,height,0,0,0,0,0,0, GLFW_WINDOW ) )
			{
				glfwTerminate();
				exit( EXIT_FAILURE );
			}

			// Set resizing callback :
			glfwSetWindowSizeCallback( WindowResize );

			// Initialize GLIP-LIB :
			HandleOpenGL::init();

			// Create a Quad inside a VBO for display :
			GeometryInstance quad(GeometryPrimitives::StandardQuad(), GL_STATIC_DRAW_ARB);

			// Create a format for the filters (with circular world repeat) :
			HdlTextureFormat format(width,height, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

			// Load a shader source code from a file :
			ShaderSource source("./Filters/game.glsl");

			// Create a filter layout using the format and the shader source :
			FilterLayout filterLayout("GameOfLife_FLayout", format, source);
			// The filter layout will automatically create the corresponding input and output ports by analyzing the uniform samplers (input) and out vectors (output) of the shader source.

			// Create a pipeline and add input and output ports :
			PipelineLayout pipelineLayout("GameOfLife_PLayout");
			pipelineLayout.addInput("Input");
			pipelineLayout.addOutput("Output");

			// Add an instance of the filter and connect :
			pipelineLayout.add(filterLayout, "GameOfLife");
			pipelineLayout.connectToInput("Input", "GameOfLife", "inputTexture");
			pipelineLayout.connectToOutput("GameOfLife", "outputTexture", "Output");
			// The connection between two filters is : .connect("NameFilter1","NameOutput","NameFilter2","NameInput"); for a connection going from NameFilter1::NameOutput to NameFilter2::NameInput.

			// Create one pipeline (which will also create one cell) then add a second cell : 
			Pipeline pipeline(pipelineLayout, "GameOfLifePipeline");
			const int cellAId = pipeline.getCurrentCellID(),
				  cellBId = pipeline.createBuffersCell();

			// Init the first texture to random :
			HdlTexture start(format);
			randomFill(start);	

			// First run :
			pipeline << start << Pipeline::Process;

			// Main loop :
			int counter = 0;
			while(running)
			{
				glClear( GL_COLOR_BUFFER_BIT );
				glLoadIdentity();

				if(counter%2==0)
				{
					// Pipeline << Argument 1 << Argument 2 << ... << Pipeline::Process;
					pipeline.changeTargetBuffersCell(cellBId);
					pipeline << pipeline.out(0, cellAId) << Pipeline::Process;
					// Prepare to use on the output :
					pipeline.out(0, cellBId).bind();
				}
				else
				{
					pipeline.changeTargetBuffersCell(cellAId);
					pipeline << pipeline.out(0, cellBId) << Pipeline::Process;
					pipeline.out(0, cellAId).bind();
				}

				counter++;

				// Swap front and back rendering buffers :
				quad.draw();
				glfwSwapBuffers();

				// Reset every cycle :
				if(counter%90==0)
				{
					randomFill(start);
					pipeline << start << Pipeline::Process;
				}

				// Check if ESC key was pressed or window was closed :
				running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

				glfwSleep(0.03);
			}

			HandleOpenGL::deinit();

			// Close window and terminate GLFW :
			glfwTerminate();

			// Exit program :
			exit( EXIT_SUCCESS );
		}
		catch(Glip::Exception& e)
		{
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;

			exit( EXIT_FAILURE );
		}
	}
