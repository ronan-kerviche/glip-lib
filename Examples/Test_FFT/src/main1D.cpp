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

		// Initialize GLFW
		if( !glfwInit() )
		{
			exit( EXIT_FAILURE );
		}

		// Open an OpenGL window
		if( !glfwOpenWindow(512,512,0,0,0,0,0,0, GLFW_WINDOW ) )
		{
			glfwTerminate();
			exit( EXIT_FAILURE );
		}

		// Set resizing callback :
		glfwSetWindowSizeCallback( WindowResize );

		int i=0;

		// Initialize GLIP-LIB
		HandleOpenGL::init();

		// Print info :
		std::cout << "Vendor name    : " << HandleOpenGL::getVendorName() << std::endl;
		std::cout << "Renderer name  : " << HandleOpenGL::getRendererName() << std::endl;
		std::cout << "OpenGL version : " << HandleOpenGL::getVersion() << std::endl;
		std::cout << "GLSL version   : " << HandleOpenGL::getGLSLVersion() << std::endl;

		// Create a Quad inside a VBO for display
		HdlVBO* vbo = HdlVBO::generate2DStandardQuad();

		// Create a format for the filters
		HdlTextureFormat fmt(512, 1, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
		ShaderSource src("./Filters/gen1D.glsl");
		ProceduralInput input("ProceduralInput", fmt, src);

		// FFT :
		FFT1D fft1D(fmt.getWidth(), FFT1D::Shifted | FFT1D::ComputeMagnitude);
		FFT1D ifft1D(fft1D.size, FFT1D::Inversed | FFT1D::Shifted | FFT1D::ComputeMagnitude);

		std::cout << " FFT1D - nchannel : " << fft1D.output().getChannel() << std::endl;
		std::cout << "iFFT1D - nchannel : " << ifft1D.output().getChannel() << std::endl;
		std::cout << " FFT1D - size     : " << fft1D.getSize(true)/(1024) << " Ko" << std::endl;
		std::cout << "iFFT1D - size     : " << fft1D.getSize(true)/(1024) << " Ko" << std::endl;

		LayoutLoader loader;

		PipelineLayout* ppl = NULL;

		ppl = loader("./Filters/visu1D.ppl");
		Pipeline visualization(*ppl,"Visualization");
		delete ppl;

		// Convolution :
		/*
		loader.addRequiredElement("format", fft1D.output().format());
		PipelineLayout* ppl = loader("./Filters/convolution.ppl");
		Pipeline conv(*ppl,"Convolution");
		delete ppl;

		loader.clearRequiredElements("format");
		loader.addRequiredElement("format", fmt);
		ppl = loader("./Filters/mix.ppl");
		Pipeline mix(*ppl,"mix");
		delete ppl;*/

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

			input.generateNewFrame();

			fft1D.process(input.texture());
			ifft1D.process(fft1D.output());

			/*conv << fft2D.output() << Pipeline::Process;
			ifft2D.process(conv.out(0));*/


			/*if(showConvolved)
				fft1D.output().bind();
			else
				input.texture().bind();*/

			//visualization << input.texture() << fft1D.output() << Pipeline::Process;
			//visualization << ifft1D.output() << fft1D.output() << Pipeline::Process;
			//visualization << input.texture() << ifft1D.output() << Pipeline::Process;
			visualization << input.texture() << fft1D.output() << ifft1D.output() << Pipeline::Process;
			visualization.out().bind();

			vbo->draw();

			i++;

			// Swap front and back rendering buffers
			glfwSwapBuffers();

			// Check if ESC key was pressed or window was closed
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

			glfwSleep(0.05);
		}

		delete vbo;

		// Close window and terminate GLFW
		glfwTerminate();

		// Exit program
		exit( EXIT_SUCCESS );
	}
