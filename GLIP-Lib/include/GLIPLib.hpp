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

\section sec_download Download
	You can download the last version of GLIP-Lib from the file page on SourceForge : http://sourceforge.net/projects/glip-lib/files/

\section sec_f_example Sample code
	The first thing you must start your code with is the initialization Glew library and other OpenGL related tools :
	\code
	HandleOpenGL::init();
	\endcode

	Then you can start to build a pipeline layout and some pipeline :
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

	// Connect the elements :
	pl.connectToInput("Input", "GameOfLife", "inText");
	pl.connectToOutput("GameOfLife", "outText", "Output");

	// Create two pipeline on this layout, they won't share any further information :
	Pipeline* p1 = new Pipeline(pl, "Ping");
	Pipeline* p2 = new Pipeline(pl, "Pong");
	\endcode

	In the end, you can use these pipelines with an InputDevice or a simple texture and Display with an output device :
	\code
	static unsigned int i=0;

	// Process and display :
	if(i%2==0)
	{
		(*p1) << p2->out(0) << Process;
		(*display) << p1->out(0);
	}
	else
	{
		(*p2) << p1->out(0) << Process;
		(*display) << p2->out(0);
	}

	i++
	\endcode

\section sec_pipeline_scripts Pipeline scripts
	The LayoutLoader module enables you to use dynamic pipeline saved in a file or a standard string. It will load a PipelineLayout that you can use directly or combined with other pipeline structures.

	The script must be structured with the following commands :

		- Format for the texture : <BR>
		<b>TEXTURE_FORMAT</b>:<i>format_name</i>(<i>integer width</i>, <i>integer height</i>, <i>GLEnum mode</i>, <i>GLEnum depth</i>, <i>GLEnum minFiltering</i>, <i>GLEnum maxFiltering</i> [, <i>GLEnum sWrapping</i>, <i>GLEnum TWrapping</i>, <i>integer maximum_mipmap_level</i> ]);

		- Shader source code, from the same file : <BR>
		<b>SHADER_SOURCE</b>:<i>source_name</i>()
		{
			<i>source code</i>
		}

		- Shader source code, from the another file : <BR>
		<b>SHADER_SOURCE</b>:<i>source_name</i>(<i>string filename</i>);

		- Filter layout :
		<b>FILTER_LAYOUT</b>:<i>filter_layout_name</i>(<i>format_name</i>, <i>fragment_shader_source</i> [, <i>vertex_shader_source</i>]);

		- Pipeline layout : <BR>
		<b>PIPELINE_LAYOUT</b>:<i>pipeline_layout_name</i>() <BR>
		{ <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>INPUT_PORTS</b>(<i>input_port_name_1</i> [,<i>input_port_name_2</i>, ..., <i>input_port_name_n</i>] ); <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>OUTPUT_PORTS</b>(<i>output_port_name_1</i> [,<i>output_port_name_2</i>, ..., <i>output_port_name_n</i>] ); <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>FILTER_INSTANCE</b>:<i>filter_instance_name</i>(<i>filter_layout_name</i>); <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>PIPELINE_INSTANCE</b>:<i>pipeline_instance_name</i>(<i>pipeline_layout_name</i>); <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(THIS,<i>this_port_name</i>,<i>element_name</i>,<i>e_port_name</i>); // Connection to input. <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(<i>element_1_name</i>, <i>e1_port_name</i>, <i>element_2_name</i>, <i>e2_port_name</i>); // Connection between two elements (filter, pipeline), the connection goes from <i>element_1_name</i>::<i>e1_port_name</i> to <i>element_2_name</i>::<i>e2_port_name</i>. <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(<i>element_name</i>,<i>e_port_name</i>,THIS,<i>this_port_name</i>); // Connection to output. <BR>
   &nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
		}

		- Main pipeline layout (the layout at the end of the loading stage) : <BR>
		<i>Same description as PIPELINE_LAYOUT, but starting with </i> <b>PIPELINE_MAIN</b>.

		- Include another script, in order to use some of its definition (format, source, filter or pipeline layout) : <BR>
		<b>INCLUDE_FILE</b>(<i>string filename</i>);

		- Commentaries : C++ style.

	You can load such a file with :
	\code
	LayoutLoader loader;
	PipelineLayout* layout = loader("./Filters/pipeline.ppl");

	Pipeline* p = new Pipeline(*layout, "HelloWorld");

	delete layout;
	\endcode

\section sec_contributors Contributors
	Ronan Kerviche, Corentin Derbois.

\section sec_license License
	Copyright (C) 2011 Ronan Kerviche

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see http://www.gnu.org/licenses/.

**/
/* ************************************************************************************************************* */
