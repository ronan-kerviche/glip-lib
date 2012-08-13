#include <cmath>
#include "GLIPLib.hpp"
#include <GL/glfw.h>
#include <stdlib.h>

// Namespace :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	void GLFWCALL WindowResize( int width, int height )
	{
		glViewport(0,0,width,height);
	}


	int main(int argc, char** argv)
	{
		bool running = true;

		// Initialize GLFW
		if( !glfwInit() )
		{
			exit( EXIT_FAILURE );
		}

		// Open an OpenGL window
		if( !glfwOpenWindow( 300,300, 0,0,0,0,0,0, GLFW_WINDOW ) )
		{
			glfwTerminate();
			exit( EXIT_FAILURE );
		}

		glfwSetWindowSizeCallback( WindowResize );


		int i=0;
		HandleOpenGL::init();

		HdlVBO* vbo = HdlVBO::generate2DStandardQuad();

		// Create a format for the filters
		HdlTextureFormat fmt(640, 480, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);

		// Load a shader source code from a file :
		ShaderSource src("./Filters/game.glsl");

		// Create a filter layout using the format and the shader source :
		FilterLayout fl("GameOfLife_Layout", fmt, src);
		// The filter layout will automatically create the corresponding input and output ports by analyzing the uniform samplers (input) and out vectors (output) of the shader source.

		// Create a pipeline :
		PipelineLayout pl("Main_GameOfLife");

		// Add one input and one output :
		pl.addInput("Input");
		pl.addOutput("Output");

		// Add an instance of the filter fl :
		pl.add(fl, "GameOfLife");
		// Here you can add mutliple filters in one pipeline.

		// Connect the elements :
		pl.connectToInput("Input", "GameOfLife", "inText");
		pl.connectToOutput("GameOfLife", "outText", "Output");
		// The connection between two filters is : pl.connect("NameFilter1","NameOutput","NameFilter2","NameInput"); for a connection going from NameFilter1::NameOutput to NameFilter2::NameInput.

		// Create two pipeline on this layout, they won't share any further information :
		Pipeline* p1 = new Pipeline(pl, "Ping");
		Pipeline* p2 = new Pipeline(pl, "Pong");

		// Init the first texture to random :
		HdlTexture start(fmt);
		char* tmp = new char[start.getSize()];

		for(int j=0; j<start.getSize(); j++)
			tmp[j] = static_cast<char>(sin(j*0.001)*255.0);
		start.write(tmp);

		delete[] tmp;

		// First run :
		(*p2) << start << Pipeline::Process;

		// Main loop
		while( running )
		{
			// OpenGL rendering goes here...
			glClear( GL_COLOR_BUFFER_BIT );
			glLoadIdentity();

			if(i%2==0)
			{
				// Pipeline << Argument 1 << Argument 2 << ... << Pipeline::Process;
				(*p1) << p2->out(0) << Pipeline::Process;
				p1->out(0).bind();
				vbo->draw();
			}
			else
			{
				(*p2) << p1->out(0) << Pipeline::Process;
				p2->out(0).bind();
				vbo->draw();
			}

			i++;

			// Swap front and back rendering buffers
			glfwSwapBuffers();
			// Check if ESC key was pressed or window was closed
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

			glfwSleep(0.1);
		}

		delete p1;
		delete p2;
		delete vbo;

		// Close window and terminate GLFW
		glfwTerminate();

		// Exit program
		exit( EXIT_SUCCESS );
	}
