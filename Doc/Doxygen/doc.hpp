/* ************************************************************************************************************* */
/**
\mainpage OpenGL Image Processing Library (GLIP-Lib)

\section sec_intro Introduction
	GLIP-LIB : The OpenGL Image Processing LIBrary is a C++ library which provides some tools to design image processing pipelines based on hardware accelerated computing (GPGPU) via OpenGL and GLSL. It uses a graph oriented approach to the description of processing tasks : you describe, in the initialization of the program, your processing pipeline as a series of filters (each can have multiple inputs and outputs). Then, you apply inputs as different image on the port(s) of the pipeline(s) and obtain the results at the output port(s).

	This library also ensures that the processing of images is not only made as a general purpose computation task on the GPU, but effectively use hardware enabled advantages of such architecture such as textured memory for fast global memory access, fast linear algebra in 2,3 and 4 dimensions and fast floating point functions. Moreover, this library can be interface either with CUDA or OpenCL APIs. Another advantage is the Just-In-Time (JIT) compilation of the shaders on the targeted plateform by the display driver. This guarantees at the same time the highest performances on the hardware and the flexibility in algorithm development.

	The library, in itself, DOES NOT provide any function for context handling (except for GLEW) nor image acquisition from files, videos or cameras, nor display tools. However, we provide examples and easily reusable code for interfacing the library with Qt, GLFW and FFMPEG.

\section sec_download Download and News
	<b>This project is currently in development, prefer downloading the code from the Git repository for latest updates</b> : http://sourceforge.net/scm/?type=git&group_id=363201 <i>(It also contains tests and examples)</i>

	The project page on SourceForge is at : http://sourceforge.net/projects/glip-lib

	You can download the last version of GLIP-Lib from the file page on SourceForge : http://sourceforge.net/projects/glip-lib/files/

	The news feed is at : https://sourceforge.net/news/?group_id=363201

\section sec_f_example Sample code
	The first thing you must start your code with is the initialization Glew library and other OpenGL related tools :
	\code
	HandleOpenGL::init();
	\endcode

	Then you can start to build a pipeline layout and some pipeline instances :
	\code
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
	\endcode

	The file <i>Filters/game.glsl</i> contains the code for a simple fragment shader. You can browse documentation on GLSL (<b>OpenGL Shading Language</b>) on this page : http://www.opengl.org/sdk/docs/manglsl/
	\code
	// Game of life : rule B3S23

	#version 130

	uniform sampler2D 	inText;
	out     vec4 		outText;

	void main()
	{
		ivec2 sz 	= textureSize(inText, 0);
		float sx 	= 1.0/(float(sz.x));
		float sy 	= 1.0/(float(sz.y));

		// Read all the eight previous and surrounding cells :
		vec4 a00	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,-sy)	, 0);
		vec4 a01	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,0.0)	, 0);
		vec4 a02	= textureLod(inText, gl_TexCoord[0].st + vec2(-sx,+sy)	, 0);
		vec4 a10	= textureLod(inText, gl_TexCoord[0].st + vec2(0.0,-sy)	, 0);
		vec4 a11	= textureLod(inText, gl_TexCoord[0].st			, 0);
		vec4 a12	= textureLod(inText, gl_TexCoord[0].st + vec2(0.0,+sy)	, 0);
		vec4 a20	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,-sy)	, 0);
		vec4 a21	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,0.0)	, 0);
		vec4 a22	= textureLod(inText, gl_TexCoord[0].st + vec2(+sx,+sy)	, 0);

		float s = a00.r+a01.r+a02.r+a10.r+a12.r+a20.r+a21.r+a22.r;

		// Older cell are shifted in the next channel :
		outText.g = a11.r;
		outText.b = a11.g;
		outText.a = 1.0;

		// New :
		if(a11.r==1.0)
		{
			if((s==2.0) || (s==3.0))
				outText.r = 1.0;
			else
				outText.r = 0.0;
		}
		else
		{
			if(s==3.0)
				outText.r = 1.0;
			else
				outText.r = 0.0;
		}
	}
	\endcode

	In the end, you can use these pipelines with an InputDevice or a simple texture and Display with an output device :
	\code
	// In this example, I show the ping pong between two pipelines. But the syntax is the same for a texture.

	// We create a first texture and a buffer :
	HdlTexture* temp = new HdlTexture(p1->out(0).format());
	unsigned char* buffer = new unsigned char[temp.getSize()];

	for(int i=0; i<temp->getSize(); i++)
		buffer[i] = static_cast<char>(rand()*255.0/RAND_MAX);

	// We write this initialization to the texture :
	temp->write(buffer);

	// We prepare the pipelines with this first image :
	// Pipeline << Argument 1 << Argument 2 << ... << Pipeline::Process;
	(*p1) << (*temp) << Pipeline::Process;
	(*p2) << (*temp) << Pipeline::Process;

	// Cleaning :
	delete buffer;
	delete temp;

	unsigned int i=0;

	while(running)
	{
		// Process and display :
		if(i%2==0)
		{
			(*p1) << p2->out(0) << Pipeline::Process;
			(*display) << p1->out(0); // display inherits from OutputDevice
		}
		else
		{
			(*p2) << p1->out(0) << Pipeline::Process;
			(*display) << p2->out(0);
		}

		i++;
	}
	\endcode

\section sec_pipeline_scripts Pipeline scripts
	The Glip::Modules::LayoutLoader module enables you to use dynamic pipeline saved in a file or a standard string. It will create either a Glip::Core::PipelineLayout or a Glip::Core::Pipeline that you can use directly or combined with other pipeline structures. Here is an example of script for histogram computation and display. Check Glip::Modules::LayoutLoader documentation page for further information.

	\code
	TEXTURE_FORMAT:colorDistribution(255,1,GL_RGB32F_ARB,GL_FLOAT,GL_NEAREST,GL_NEAREST); // Histograms bins
	TEXTURE_FORMAT:histogramWindow(640,480,GL_RGB,GL_UNSIGNED_BYTE,GL_LINEAR,GL_LINEAR); // Final window representation

	SHADER_SOURCE:vertexHistogram()
	{
		#version 130

		uniform sampler2D tex0;
		uniform int c; // 0:red 1:green 2:blue

		void main()
		{
			// Vertex texture fetching :
			vec4 col = textureLod(tex0, gl_Vertex.xy+vec2(0.5,0.5), 0.0);

			// "Payload"
			gl_FrontColor = vec4(0.0,0.0,1.0,1.0);

			// Compute the new position!
			float tmp = 0.0;
			if(c==0)
				tmp = col.r;
			if(c==1)
				tmp = col.g;
			if(c==2)
				tmp = col.b;

			// Done
			gl_Position.x = (tmp-0.5)*2.0;
			gl_Position.y = 0.0;
			gl_Position.z = 0.0;
		}
	}

	SHADER_SOURCE:fragHistogram()
	{
		#version 130

		uniform float nrm, scale;

		out vec4 outText;

		void main()
		{
			if(gl_Color.b>0.5)
				outText = vec4(nrm,nrm,nrm,1.0)*scale;
			else
				outText = vec4(0.0,0.0,0.0,0.0);
		}
	}

	SHADER_SOURCE:fragWinHist()
	{
		#version 130
		uniform sampler2D fRed, fGreen, fBlue, tex0;
		out vec4 outText;

		void main()
		{
			vec2 pos = gl_TexCoord[0].st;
			vec4 pRed 	= textureLod(fRed, 	pos, 0);
			vec4 pGreen	= textureLod(fGreen, 	pos, 0);
			vec4 pBlue	= textureLod(fBlue,	pos, 0);
			vec4 col	= textureLod(tex0, 	pos, 0);

			if(pos.t<pRed.r )
				outText.r = 1.0;
			else
				outText.r = col.r;
			if(pos.t<pGreen.r )
				outText.g = 1.0;
			else
				outText.g = col.g;
			if(pos.t<pBlue.r )
				outText.b = 1.0;
			else
				outText.b = col.b;
		}
	}


	FILTER_LAYOUT:filterHistogram(colorDistribution, fragHistogram, vertexHistogram, CLEARING_ON, BLENDING_ON);
	FILTER_LAYOUT:windowHistogram(histogramWindow, fragWinHist);

	PIPELINE_MAIN:pipelineHistogram()
	{
		INPUT_PORTS(i0);
		OUTPUT_PORTS(o0);

		FILTER_INSTANCE:instHistRed(filterHistogram);
		FILTER_INSTANCE:instHistGreen(filterHistogram);
		FILTER_INSTANCE:instHistBlue(filterHistogram);
		FILTER_INSTANCE:instWin(windowHistogram);

		CONNECTION(THIS, i0, instHistRed, tex0);
		CONNECTION(THIS, i0, instHistGreen, tex0);
		CONNECTION(THIS, i0, instHistBlue, tex0);
		CONNECTION(instHistRed, outText, instWin, fRed);
		CONNECTION(instHistGreen, outText, instWin, fGreen);
		CONNECTION(instHistBlue, outText, instWin, fBlue);
		CONNECTION(THIS, i0, instWin, tex0);
		CONNECTION(instWin, outText, THIS, o0);
	}
	\endcode

	You can load such a file with :
	\code
	LayoutLoader loader;
	PipelineLayout* layout = loader("./Filters/pipeline.ppl");

	Pipeline* p = new Pipeline(*layout, "HelloWorld");

	delete layout;
	\endcode

	Then, set the variables and the geometry to be used in the C++ code :
	\code
	// Setting the corresponding channel for all filters :
	((*pipeline)["instHistRed"]).prgm().modifyVar("c",HdlProgram::Var,   0);
	((*pipeline)["instHistGreen"]).prgm().modifyVar("c",HdlProgram::Var, 1);
	((*pipeline)["instHistBlue"]).prgm().modifyVar("c",HdlProgram::Var,  2);

	// Setting vertical scale factor :
	const float scale = 10.0f;
	((*pipeline)["instHistRed"]).prgm().modifyVar("scale",HdlProgram::Var,   scale);
	((*pipeline)["instHistGreen"]).prgm().modifyVar("scale",HdlProgram::Var, scale);
	((*pipeline)["instHistBlue"]).prgm().modifyVar("scale",HdlProgram::Var,  scale);

	// Set the geometry for an input image as a HdlTexture text :
	(*pipeline)["instHistRed"].setGeometry(HdlVBO::generate2DGrid(   text->getWidth(), text->getHeight()));
	(*pipeline)["instHistGreen"].setGeometry(HdlVBO::generate2DGrid( text->getWidth(), text->getHeight()));
	(*pipeline)["instHistBlue"].setGeometry(HdlVBO::generate2DGrid(  text->getWidth(), text->getHeight()));

	// Set the summing coefficient :
	float a = 1.0/static_cast<float>(text->getWidth()*text->getHeight());
	((*pipeline)["instHistRed"]).prgm().modifyVar("nrm",HdlProgram::Var,   a);
	((*pipeline)["instHistGreen"]).prgm().modifyVar("nrm",HdlProgram::Var, a);
	((*pipeline)["instHistBlue"]).prgm().modifyVar("nrm",HdlProgram::Var,  a);
	\endcode

	The result will look like :
	<img src="./histogram.png" />
	<div align="center"><i>Original Image Credit & Copyright: <a href="http://www.luisargerich.com/about.html">Luis Argerich</a></i></div>

\section sec_examples Examples and demos
	Adapter for Qt Code, based on <i>OutputDevice</i> (implements full control over translation, rotation, zoom with mouse and keyboard) :<BR>
	&nbsp;&nbsp;&nbsp;&nbsp; <a href="./WindowRendering.hpp">WindowRendering.hpp</a><BR>
	&nbsp;&nbsp;&nbsp;&nbsp; <a href="./WindowRendering.cpp">WindowRendering.cpp</a><BR>
	GLFW "Hello World" - Game of Life :<BR>
	&nbsp;&nbsp;&nbsp;&nbsp; <a href="./mainGLFW.cpp">mainGLFW.cpp</a><BR>
	&nbsp;&nbsp;&nbsp;&nbsp; <a href="./game.glsl">game.glsl</a><BR>
	<img src="./animation.gif" />

\section sec_contributors Contributors
	<a href="mailto:ronan-kerviche@users.sf.net">Ronan Kerviche</a> , Corentin Derbois.

\section sec_license License
	Copyright (C) 2011, 2012 Ronan Kerviche

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**/
/* ************************************************************************************************************* */
