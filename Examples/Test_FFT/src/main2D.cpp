/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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
	#include <fstream>
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

// Main
	int main(int argc, char** argv)
	{
		bool 	running 		= true,
			showConvolved 		= false,
			waitingForRelease 	= false;

		std::fstream log;

		log.open("./log.txt", std::fstream::out | std::fstream::trunc);
		if(!log.is_open())
			throw Exception("TestFFT1D - Cannot open log file.", __FILE__, __LINE__);

		// Initialize GLFW
		if( !glfwInit() )
		{
			log << "Failed to start GLFW." << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			exit( EXIT_FAILURE );
		}

		// Open an OpenGL window
		if( !glfwOpenWindow(512,512,0,0,0,0,0,0, GLFW_WINDOW ) )
		{
			glfwTerminate();
			log << "Failed to open GLFW window." << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			exit( EXIT_FAILURE );
		}

		// Set resizing callback :
		glfwSetWindowSizeCallback( WindowResize );

		try
		{
			double 	timeGOL  = 0.0,
				timeConv = 0.0,
				timeMix  = 0.0;
			int i=0;

			// Initialize GLIP-LIB
			HandleOpenGL::init();

			// Print info :
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			// Create a format for the filters
			HdlTextureFormat fmt(512, 512, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
			fmt.setSWrapping(GL_REPEAT);
			fmt.setTWrapping(GL_REPEAT);

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

			// Connect the elements :
			pl.connectToInput("Input", "GameOfLife", "inText");
			pl.connectToOutput("GameOfLife", "outText", "Output");
			// The connection between two filters is : pl.connect("NameFilter1","NameOutput","NameFilter2","NameInput"); for a connection going from NameFilter1::NameOutput to NameFilter2::NameInput.

			// Create two pipeline on this layout, they won't share any further information :
			Pipeline* p1 = new Pipeline(pl, "Ping");
			Pipeline* p2 = new Pipeline(pl, "Pong");

			// Init the first texture to random :
			HdlTexture start(fmt);
			unsigned char* tmp = new unsigned char[start.getSize()];

			srand(time(NULL));
			for(int j=0; j<start.getSize(); j++)
			{
				if(rand()>0.8*RAND_MAX)
					tmp[j] = 255;
				else
					tmp[j] = 0;
			}
			start.write(tmp);

			delete[] tmp;

			// First run :
			(*p1) << start << Pipeline::Process;
			(*p2) << start << Pipeline::Process;

			// FFT :
			FFT2D fft2D(fmt.getWidth(), fmt.getHeight(), FFT2D::Shifted);
			FFT2D ifft2D(fft2D.w, fft2D.h, FFT2D::Inversed | FFT2D::ComputeMagnitude | FFT2D::Shifted);

			// Convolution :
			LayoutLoader loader;
			loader.addRequiredElement("format", fft2D.output().format());
			Pipeline conv( loader("./Filters/convolution.ppl") ,"Convolution");

			loader.clearRequiredElements("format");
			loader.addRequiredElement("format", fmt);
			Pipeline mix( loader("./Filters/mix.ppl") ,"mix");

			p1->enablePerfsMonitoring();
			p2->enablePerfsMonitoring();
			conv.enablePerfsMonitoring();
			mix.enablePerfsMonitoring();

			fft2D.enablePerfsMonitoring();
			ifft2D.enablePerfsMonitoring();

			// Main loop
			while( running )
			{
				if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS && !waitingForRelease)
				{
					showConvolved = !showConvolved;
					waitingForRelease = true;
				}

				if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_RELEASE && waitingForRelease)
					waitingForRelease = false;

				glClear( GL_COLOR_BUFFER_BIT );
				glLoadIdentity();

				if(i%2==0)
				{
					// Pipeline << Argument 1 << Argument 2 << ... << Pipeline::Process;
					(*p1) << p2->out(0) << Pipeline::Process;
					timeGOL += p1->getTotalTiming();
					fft2D.process(p1->out(0));
				}
				else
				{
					(*p2) << p1->out(0) << Pipeline::Process;
					timeGOL += p2->getTotalTiming();
					fft2D.process(p2->out(0));
				}

				conv << fft2D.output() << Pipeline::Process;
				timeConv += conv.getTotalTiming();
				ifft2D.process(conv.out(0));

				if(i%2==0)
					mix << p1->out(0) << ifft2D.output() << Pipeline::Process;
				else
					mix << p2->out(0) << ifft2D.output() << Pipeline::Process;
				timeMix += mix.getTotalTiming();

				if(showConvolved)
					mix.out(0).bind();
				else
					if(i%2==0)
						p1->out(0).bind();
					else
						p2->out(0).bind();

				HandleOpenGL::standardQuadVBO().draw();

				i++;

				// Swap front and back rendering buffers
				glfwSwapBuffers();

				// Check if ESC key was pressed or window was closed
				running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

				glfwSleep(0.1);
			}

			log << "Total number of processes : " << i << std::endl;
			log << "Mean duration for GOL     : " << timeGOL/i << " ms" << std::endl;
			log << "Mean duration for CONV    : " << timeConv/i << " ms" << std::endl;
			log << "Mean duration for MIX     : " << timeMix/i << " ms" << std::endl;
			log << "FFT1D" << std::endl;
			log << "    Number of processes   : " << fft2D.getNumProcesses() << std::endl;
			log << "    Mean time             : " << fft2D.getMeanTime() << " ms" << std::endl;
			log << "    Std Dev on time       : " << fft2D.getStdDevTime() << " ms" << std::endl;
			log << "iFFT1D" << std::endl;
			log << "    Number of processes   : " << ifft2D.getNumProcesses() << std::endl;
			log << "    Mean time             : " << ifft2D.getMeanTime() << " ms" << std::endl;
			log << "    Std Dev on time       : " << ifft2D.getStdDevTime() << " ms" << std::endl;

			fft2D.disablePerfsMonitoring();
			ifft2D.disablePerfsMonitoring();

			log << "> End" << std::endl;
			log.close();

			delete p1;
			delete p2;

			HandleOpenGL::deinit();

			// Close window and terminate GLFW
			glfwTerminate();

			// Exit program
			exit( EXIT_SUCCESS );
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
		}
	}
