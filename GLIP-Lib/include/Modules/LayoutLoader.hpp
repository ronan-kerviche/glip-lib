/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : LayoutLoader.hpp                                                                          */
/*     Original Date : December 22th 2011                                                                        */
/*                                                                                                               */
/*     Description   : Layout Loader from files or strings.                                                      */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    LayoutLoader.hpp
 * \brief   Layout Loader from files or strings.
 * \author  R. KERVICHE
 * \date    December 22th 2011
**/

#ifndef __LAYOUT_LOADER_INCLUDE__
#define __LAYOUT_LOADER_INCLUDE__

	#include <map>
	#include "Core/LibTools.hpp"
	#include "Core/HdlTexture.hpp"
	#include "Core/Geometry.hpp"
	#include "Core/ShaderSource.hpp"
	#include "Core/Filter.hpp"
	#include "Core/Pipeline.hpp"
	#include "Modules/VanillaParser.hpp"
	#include "Modules/LayoutLoaderModules.hpp"

namespace Glip
{
	using namespace CoreGL;
	using namespace CorePipeline;

	namespace Modules
	{
		/**		
		\enum Glip::Modules::LayoutLoaderKeyword
		\brief Keywords used by LayoutLoader and LayoutWriter. Use LayoutLoader::getKeyword() to get the actual string.
		**/
		enum LayoutLoaderKeyword
		{
			KW_LL_TRUE,
			KW_LL_FALSE,
			KW_LL_FORMAT_LAYOUT,
			KW_LL_SHADER_SOURCE,
			KW_LL_FILTER_LAYOUT,
			KW_LL_PIPELINE_LAYOUT,
			KW_LL_PIPELINE_MAIN,
			KW_LL_INCLUDE,
			KW_LL_FILTER_INSTANCE,
			KW_LL_PIPELINE_INSTANCE,
			KW_LL_CONNECTION,
			KW_LL_INPUT_PORTS,
			KW_LL_OUTPUT_PORTS,
			KW_LL_THIS_PIPELINE,
			KW_LL_DEFAULT_VERTEX_SHADER,
			KW_LL_CLEARING_ON,
			KW_LL_CLEARING_OFF,
			KW_LL_BLENDING_ON,
			KW_LL_BLENDING_OFF,
			KW_LL_REQUIRED_FORMAT,
			KW_LL_REQUIRED_GEOMETRY,
			KW_LL_REQUIRED_PIPELINE,
			KW_LL_SHARED_CODE,
			KW_LL_INSERT,
			KW_LL_GEOMETRY,
			KW_LL_GRID_2D,
			KW_LL_GRID_3D,
			KW_LL_CUSTOM_MODEL,
			KW_LL_GEOMETRY_FROM_FILE,
			KW_LL_STANDARD_QUAD,
			KW_LL_VERTEX,
			KW_LL_ELEMENT,
			KW_LL_ADD_PATH,
			KW_LL_CALL,
			KW_LL_UNIQUE,
			LL_NumKeywords,
			LL_UnknownKeyword
		};

/**
\class LayoutLoader
\brief Load and save pipelines layouts to file

The LayoutLoader module enables you to use dynamic pipeline saved in a file or a standard string. It will create either a Glip::Core::PipelineLayout or a Glip::Core::Pipeline that you can use directly or combined with other pipeline structures.

The script must be structured with the following commands (but no special order is needed except standard declaration order) :

- Protect file from being included twice and leading to redefinition errors : <BR>
<b>UNIQUE</b>(<i>someUniqueString</i>) 

- Format for the texture : <BR>
<b>TEXTURE_FORMAT</b>:<i>format_name</i>(<i>integer width</i>, <i>integer height</i>, <i>GLEnum mode</i>, <i>GLEnum depth</i>, [ <i>GLEnum minFiltering</i>, <i>GLEnum maxFiltering</i>, <i>GLEnum sWrapping</i>, <i>GLEnum TWrapping</i>, <i>integer maximum_mipmap_level</i> ])

- Required format to be provided by the application (for dynamic sizes). See LayoutLoader::addRequiredElement() : <BR>
If the format is not found in the required element, it is searched in the known formats. You can also use character '*' to keep some of the parameters or replace them by specifying a new value. <BR>
<b>REQUIRED_FORMAT</b>:<i>format_name</i>(<i>required_format_name</i> [, <i>integer width / *</i>, <i>integer height / *</i>, <i>GLEnum mode / *</i>, <i>GLEnum depth / *</i>, <i>GLEnum minFiltering / *</i>, <i>GLEnum maxFiltering / *</i>, <i>GLEnum sWrapping / *</i>, <i>GLEnum TWrapping / *</i>, <i>integer maximum_mipmap_level / *</i>])

- Geometry : <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GeometryType</i>, <i>param1</i>, ... <i>paramN</i>) <BR>
Examples : <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GRID_2D</i>, <i>integer : width</i>, <i>integer : height</i>) <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GRID_3D</i>, <i>integer : width</i>, <i>integer : height</i>, <i>integer : depth</i>) <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>CUSTOM_MODEL</i>, <i>GL Primitive (GL_POINTS, GL_LINES, GL_TRIANGLES, etc.)</i>, [<i>true (if it has texcoord definition embedded)</i>]) <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>Definition of a vertex (depending on the format) :</i> <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>VERTEX</b>( <i>x</i>, <i>y</i>, [<i>z</i>], [<i>u</i>, <i>v</i>]) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>Definition of a primitive element (depending on the format) :</i> <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>ELEMENT</b>( <i>a</i>, [<i>b</i>], [<i>c</i>], [<i>d</i>]) <BR>
} <BR>

- Required geometry to be provided by the application (for dynamic geometry models). See LayoutLoader::addRequiredElement() : <BR>
<b>REQUIRED_GEOMETRY</b>:<i>geometry_name</i>(<i>required_geometry_name</i>)

- Module call, see LayoutLoaderModule for more information : <BR>
<b>CALL</b> : <i>module_name</i> (<i> module_arguments</i>) <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>(Optional)</i> <BR>
} <BR>

- Shader source code, from the same file : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>source code</i> <BR>
} <BR>

- Shader source code, from the another file : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i>(<i>string filename</i>)

- Filter layout :
<b>FILTER_LAYOUT</b>:<i>filter_layout_name</i>(<i>format_name</i>, <i>fragment_shader_source</i> [, <i>vertex_shader_source</i>/<b>DEFAULT_VERTEX_SHADER</b>, <i>geometry_name</i>]) <BR>
[{<BR>
&nbsp;&nbsp;&nbsp;&nbsp; GL_CLEAR(<i>true/false</i>)<BR>
&nbsp;&nbsp;&nbsp;&nbsp; GL_BLEND(<i>sFactor</i>,<i>dFactor</i>,<i>blendingFunction</i>)<BR>
&nbsp;&nbsp;&nbsp;&nbsp; GL_DEPTH_TEST(<i>depthTestFunction</i>)<BR>
}]<BR>

- Pipeline layout : <BR>
<b>PIPELINE_LAYOUT</b>:<i>pipeline_layout_name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>INPUT_PORTS</b>(<i>input_port_name_1</i> [,<i>input_port_name_2</i>, ..., <i>input_port_name_n</i>] ) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>OUTPUT_PORTS</b>(<i>output_port_name_1</i> [,<i>output_port_name_2</i>, ..., <i>output_port_name_n</i>] ) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>FILTER_INSTANCE</b>:<i>filter_instance_name</i>(<i>filter_layout_name</i>) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>FILTER_INSTANCE</b>:<i>filter_layout_name</i> <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>PIPELINE_INSTANCE</b>:<i>pipeline_instance_name</i>(<i>pipeline_layout_name</i>) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>PIPELINE_INSTANCE</b>:<i>pipeline_layout_name</i> <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(THIS,<i>this_port_name</i>,<i>element_name</i>,<i>e_port_name</i>) // Connection to input. <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(<i>element_1_name</i>, <i>e1_port_name</i>, <i>element_2_name</i>, <i>e2_port_name</i>) // Connection between two elements (filter, pipeline), the connection goes from <i>element_1_name</i>::<i>e1_port_name</i> to <i>element_2_name</i>::<i>e2_port_name</i>. <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>CONNECTION</b>(<i>element_name</i>,<i>e_port_name</i>,THIS,<i>this_port_name</i>) // Connection to output. <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>If you don't declare any connection, the loader will try to connect elements automatically using PipelineLayout::autoConnect(), make sure that the pipeline is compliant with the corresponding rules.</i> <BR>
}

- Required pipeline layout to be provided by the application (for decorators). See LayoutLoader::addRequiredElement() : <BR>
<b>REQUIRED_PIPELINE</b>:<i>pipeline_layout_name</i>(required_pipeline_name)

- Main pipeline layout (the layout at the end of the loading stage) : <BR>
<i>Same description as PIPELINE_LAYOUT, but starting with </i> <b>PIPELINE_MAIN</b> <BR>
Or, via an indirection : <BR>
<b>PIPELINE_MAIN</b>:<i>main_pipeline_name</i>(<i>pipeline_layout_name</i>) <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>INPUT_PORTS</b>(<i>force_input_port_name_1</i> [,<i>force_input_port_name_2</i>, ..., <i>force_input_port_name_n</i>] ) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>OUTPUT_PORTS</b>(<i>force_output_port_name_1</i> [,<i>force_output_port_name_2</i>, ..., <i>force_output_port_name_n</i>] ) <BR>
}

- Add to the search path : <BR>
<b>ADD_PATH</b>(/some/path/) <BR>

- Include another script, in order to use some of its definition (format, source, filter or pipeline layout) : <BR>
<b>INCLUDE</b>(<i>string filename</i>)

- Distribute code to the shaders code with the SHARED_CODE marker. Insert them in the SHADER_SOURCE elements by adding INSERT marker : <BR>
<b>SHARED_CODE:shared_segment_name</b> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>// shared code goes here...</i> <BR>
} <BR>
&nbsp;&nbsp; Then use : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>INSERT(shared_segment_name)</b> // The shared code will be inserted here... <BR>
} <BR>

- Comments : C++ style.

<i>Hello world pipeline example </i> (no processing, just resizing to <i>format</i>) :
	\code
	// The output format (for the output texture). Note that the filtering parameters are of no use in this pipeline. They will be use in the next processing/display step :
	TEXTURE_FORMAT:format(640,480,GL_RGB,GL_UNSIGNED_BYTE,GL_LINEAR,GL_LINEAR)

	// The shader source (for more information, check the GLSL language specifications at http://www.opengl.org/documentation/glsl/) :
	SHADER_SOURCE:HelloWorld
	{
		#version 130

		uniform sampler2D	texInput;
		out     vec4 		texOutput;

		void main()
		{
			// Get the input data :
			vec4 col  = textureLod(texInput, gl_TexCoord[0].st, 0.0);

			// Write the output data :
			texOutput = col;
		}
	}

	// Declare the filter layout :
	FILTER_LAYOUT:helloFilter(format,HelloWorld)
	// The filter layout will have one input port and one output port, which names are respectively texInput and texOutput.
	// This information is gathered from the shader source HelloWorld, by analyzing the variables declared as uniform sampler2D for inputs and out vec4 for outputs.

	PIPELINE_MAIN:pMainGradient
	{
		// Declare some input and output ports for this pipeline :
		INPUT_PORTS(texInput)
		OUTPUT_PORTS(texOutput)

		// Declare one filter component : 
		FILTER_INSTANCE:helloFilter
		// We could also give it a special name ('instHello') : 
		// FILTER_INSTANCE:instHello(helloFilter)

		// Since the input and output port names we chose for the pipeline are the same than for the filter
		// (as described in the shader source) then we don't need to do the connections (it will be made automatically).
		// However one can imagine replacing the previous code by :
		//
		//INPUT_PORTS(input)
		//OUTPUT_PORTS(output)
		//
		// In that case, we would have to declare the connections as :
		//
		//CONNECTION(THIS,input,instHello,texInput)
		//CONNECTION(instHello,texOutput,THIS,texOutput)
		//
	}
	\endcode

Loading Example :
	\code
	try
	{
		// Load a PipelineLayout :
		Loader loader;
		PipelineLayout myLayout = loader("./path/pipeline.ppl");

		// use it :
		Pipeline* myPipeline1 = new Pipeline(myLayout,"Pipeline1");
		Pipeline* myPipeline2 = new Pipeline(myLayout,"Pipeline2");

		// For a single pipeline :
		Pipeline* myPipelineU = loader("./path/otherPipeline.ppl","myPipelineName");

		// use them, see Glip::CorePipeline::Pipeline documentation...

		// Clean all :
		delete myPipeline1;
		delete myPipeline2;
		delete myPipelineU;
	}
	catch(Exception& e)
	{
		std::cout << "An exception was caught : " << std::endl;
		std::cout << e.what() << std::endl;
	}
	\endcode

# Script Specifications ######
## Unique
<blockquote>
<b>UNIQUE</b>(<i>identifier</i>)
</blockquote>

Protect a source from being loaded multiple times. Based on the <i>identifier</i> used.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>identifier</i></td>			<td>Unique identifier for a source.</td></tr>
</table>

## Add path
<blockquote>
<b>ADD_PATH</b>(<i>path</i>)
</blockquote>

Add a specific path to the search pool.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>path</i></td>				<td>Path to be added to the search pool. Separators are '/'.</td></tr>
</table>

## Include
<blockquote>
<b>INCLUDE</b>(<i>filename</i>)
</blockquote>

Include another file. All the elements declared after parsing this file becomes available. The main pipeline(s) are changed into (a) pipeline layout(s).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>filename</i></td>			<td>The filename of the file to be loaded. This file must also be a script file.</td></tr>
</table>

## Call
<blockquote>
<b>CALL</b>:<i>moduleName</i>(<i>argument0</i>, <i>argument1</i>, [...])<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>body</i><br>
}
</blockquote>

Enable the script to use predefined modules attached to this loader. These modules are implementing specific high-level functions. Each might request different arguments or some code in the body.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the module to be called. This module must have been added to the current LayoutLoader.</td></tr>
<tr class="glipDescrRow"><td><i>argument0</i></td>			<td>First possible argument passed to the module.</td></tr>
<tr class="glipDescrRow"><td><i>argumentN</i></td>			<td>...</td></tr>
<tr class="glipDescrRow"><td><i>body</i></td>				<td>Possible body, passed to the module.</td></tr>
</table>

For more information, refer to the modules documentation in Modules::LayoutLoaderModule and Modules::LayoutLoader::addModule.

## Format
### Format
<blockquote>
<b>TEXTURE_FORMAT</b>:<i>name</i>(<i>width</i>, <i>height</i>, <i>mode</i>, <i>depth</i>)
</blockquote>
<blockquote>
<b>TEXTURE_FORMAT</b>:<i>name</i>(<i>width</i>, <i>height</i>, <i>mode</i>, <i>depth</i>[, <i>minFiltering</i>, <i>maxFiltering</i>, <i>sWrapping</i>, <i>tWrapping</i>, <i>maxMipMapLevel</i>])
</blockquote>

Define a texture format (Core::HdlTextureFormat). The first line shows the minimum set of arguments required.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>width</i></td>				<td>Width in pixels of the format.</td></tr>
<tr class="glipDescrRow"><td><i>height</i></td>				<td>Height in pixels of the format.</td></tr>
<tr class="glipDescrRow"><td><i>mode</i></td>				<td>Mode of the format, among GL_RED, GL_LUMINANCE, GL_RGB, GL_RGBA, etc.</td></tr>
<tr class="glipDescrRow"><td><i>depth</i></td>				<td>Depth of the format, among GL_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_FLOAT, etc.</td></tr>
<tr class="glipDescrRow"><td><i>minFilter</i> = GL_NEAREST</td>		<td>Minification filtering.</td></tr>
<tr class="glipDescrRow"><td><i>magFilter</i> = GL_NEAREST</td>		<td>Magnification filtering.</td></tr>
<tr class="glipDescrRow"><td><i>sWrapping</i> = GL_CLAMP</td>		<td>S Wrapping.</td></tr>
<tr class="glipDescrRow"><td><i>tWrapping</i> = GL_CLAMP</td>		<td>T Wrapping.</td></tr>
<tr class="glipDescrRow"><td><i>maxMipMapLevel</i> = 0</td>		<td>Maximum MipMap level.</td></tr>
</table>

### Required Format
<blockquote>
REQUIRED_FORMAT:<i>name</i>(<i>requiredFormatName</i>)
</blockquote>
<blockquote>
REQUIRED_FORMAT:<i>name</i>(<i>requiredFormatName</i>[, <i>newWidth</i>, <i>newHeight</i>, <i>newMode</i>, <i>newDepth</i>, <i>newMinFilter</i>, <i>newMagFilter</i>, <i>newSWrapping</i>, <i>newTWrapping</i>, <i>newMaxMipMapLevel</i>)
</blockquote>

Define a texture format from a required resource. This enables the script to receive data from the program, as a dynamic specification.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the texture format created.</td></tr>
<tr class="glipDescrRow"><td><i>requiredFormatName</i></td>		<td>Name of the required format. Must be added to the LayoutLoader via LayoutLoader::addRequiredElement.</td></tr>	
<tr class="glipDescrRow"><td><i>newWidth</i></td>			<td>Replace the width (in pixels) of the original format. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newHeight</i></td>			<td>Replace the height (in pixels) of the original format. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newMode</i></td>			<td>Replace the mode of the original format. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newDepth</i></td>			<td>Replace the depth of the original format. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newMinFilter</i></td>			<td>Replace the minification filtering. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newMagFilter</i></td>			<td>Replace the magnification filtering. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newSWrapping</i></td>			<td>Replace the S Wrapping. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newTWrapping</i></td>			<td>Replace the T Wrapping. Use * to keep the original value.</td></tr>
<tr class="glipDescrRow"><td><i>newMaxMipMapLevel</i> = 0</td>		<td>Replace the Maximum MipMap level. Use * to keep the original value.</td></tr>
</table>

## Geometry
### Geometry
<blockquote>
GEOMETRY:<i>name</i>(<i>type</i>[, <i>argument0</i>, ...])
</blockquote>

Define a model, which can be used as the base drawing in a filter.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the model.</td></tr>
<tr class="glipDescrRow"><td><i>type</i></td>				<td>Type of the geometry. Can be GRID_2D, GRID_3D or CUSTOM_MODEL.</td></tr>
<tr class="glipDescrRow"><td><i>argument_0</i></td>			<td>Argument, depending on the choice of the geometry.</td></tr>
</table>

#### Grid2D
<blockquote>
GEOMETRY:<i>name</i>(GRID_2D, <i>width</i>, <i>height</i>)
</blockquote>

Create a 2D grid of dots.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the model.</td></tr>
<tr class="glipDescrRow"><td><i>width</i></td>				<td>Number of dots in the X direction.</td></tr>
<tr class="glipDescrRow"><td><i>height</i></td>				<td>Number of dots in the Y direction.</td></tr>
</table>

#### Grid3D
<blockquote>
GEOMETRY:<i>name</i>(GRID_3D, <i>width</i>, <i>height</i>, <i>depth</i>)
</blockquote>

Create a 3D grid of dots.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the model.</td></tr>
<tr class="glipDescrRow"><td><i>width</i></td>				<td>Number of dots in the X direction.</td></tr>
<tr class="glipDescrRow"><td><i>height</i></td>				<td>Number of dots in the Y direction.</td></tr>
<tr class="glipDescrRow"><td><i>depth</i></td>				<td>Number of dots in the Z direction.</td></tr>
</table>

#### CustomModel
<blockquote>
GEOMETRY:<i>name</i>(CUSTOM_MODEL, <i>primitive</i>, <i>hasTexCoords</i>)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>body</i><br>
}<br>
</blockquote>

Create a custom geometry model.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the model.</td></tr>
<tr class="glipDescrRow"><td><i>primitive</i></td>			<td>Type of the primitive.</td></tr>
<tr class="glipDescrRow"><td><i>hasTexCoords</i></td>			<td>If the VERTEX data will contain the texture coordinates. Either TRUE or FALSE.</td><tr>
<tr class="glipDescrRow"><td><i>body</i></td>				<td>Contains a description of the geometry, using VERTEX and ELEMENT.</td></tr>
</table>

##### Vertex
<blockquote>
VERTEX(<i>x</i>, <i>y</i>[, <i>u</i>, <i>v</i>])
</blockquote>
<blockquote>
VERTEX(<i>x</i>, <i>y</i>, <i>z</i>[, <i>u</i>, <i>v</i>])
</blockquote>

Define a vertex. If the geometry was set to receive texture coordinates, the VERTEX data must have the U and V coordinates specified.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>x</i></td>				<td>X Coordinate.</td></tr>
<tr class="glipDescrRow"><td><i>y</i></td>				<td>Y Coordinate.</td></tr>
<tr class="glipDescrRow"><td><i>z</i></td>				<td>Z Coordinate.</td></tr>
<tr class="glipDescrRow"><td><i>u</i></td>				<td>U Texture coordinate.</td></tr>
<tr class="glipDescrRow"><td><i>v</i></td>				<td>V Texture coordinate.</td></tr>
</table>

##### Element
<blockquote>
ELEMENT(<i>a</i>, [<i>b</i>, <i>c</i>, <i>d</i>])
</blockquote>

Define an element. An element is the polygon structure or primitive defined for the current geometry model. Only using GL_POINT wave the requirement for defining elements. The number of elements indices needed depend on the number of vertices per element.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>a</i></td>				<td>First vertex index.</td></tr>
<tr class="glipDescrRow"><td><i>b</i></td>				<td>Second vertex index.</td></tr>
<tr class="glipDescrRow"><td><i>c</i></td>				<td>Third vertex index.</td></tr>
<tr class="glipDescrRow"><td><i>d</i></td>				<td>Fourth vertex index.</td></tr>
</table>

### Required Geometry
<blockquote>
REQUIRED_GEOMETRY:<i>name</i>(<i>requiredGeometryName</i>)
</blockquote>

Define a geometry model from a required geometry. This enables the script to receive data from the program, as a dynamic specification.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the geometry to be created.</td></tr>
<tr class="glipDescrRow"><td><i>requiredGeometryName</i></td>		<td>Name of the required geometry to use.</td></tr>
</table>

## Shared Code
### Shared code
<blockquote>
SHARED_CODE:<i>name</i><br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>body</i><br>
}
</blockquote>

Define a portion of code which can be included in different source. Use INSERT to insert in another source.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of shared segment.</td></tr>
<tr class="glipDescrRow"><td><i>body</i></td>				<td>Source to be shared.</td></tr>
</table>

### Insert
<blockquote>
INSERT(<i>sharedCodeName</i>)
</blockquote>

Insert a shared code inside a source. This keyword must be inside, either another SHARED_CODE or a SHADER_SOURCE. This call must be on a single line, by itself (but possibly including comments).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>sharedCodeName</i></td>			<td>Name of the shared code segment to be inserted at the current position.</td></tr>
</table>

## Shader Source
<blockquote>
SHADER_SOURCE:<i>name</i><br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>code</i><br>
}
</blockquote>
<blockquote>
SHADER_SOURCE:<i>name</i>(<i>filename</i>)
</blockquote>

Define a shader source code.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the shader source.</td></tr>
<tr class="glipDescrRow"><td><i>code</i></td>				<td>Shader source code.</td></tr>
<tr class="glipDescrRow"><td><i>filename</i></td>			<td>Filename of the file to load the source code from. Will be checked against the search paths pool.</td></tr>
</table>

## Filter Layout
### Filter Layout
<blockquote>
FILTER_LAYOUT:<i>name</i>(<i>outputFormatName</i>, <i>fragmentShaderSourceName</i>[, <i>vertexShaderSourceName</i>, <i>geometryName</i>])
</blockquote>
<blockquote>
FILTER_LAYOUT:<i>name</i>(<i>outputFormatName</i>, <i>fragmentShaderSourceName</i>[, <i>vertexShaderSourceName</i>, <i>geometryName</i>])<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>extraOptions</i><br>
}
</blockquote>

Define a filter layout.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the filter layout created.</td></tr>
<tr class="glipDescrRow"><td><i>outputFormatName</i></td>		<td>Name of the format for the output.</td></tr>
<tr class="glipDescrRow"><td><i>fragmentShaderSourceName</i></td>	<td>Name of the fragment shader source.</td></tr>
<tr class="glipDescrRow"><td><i>vertexShaderSourceName</i></td>		<td>Name of the vertex shader source. Use DEFAULT_VERTEX_SHADER to get the default vertex shader.</td></tr>
<tr class="glipDescrRow"><td><i>geometryName</i></td>			<td>Name of the geometry model.</td></tr>
<tr class="glipDescrRow"><td><i>extraOptions</i></td>			<td>Set of options for the filter layout : GL_CLEAR, GL_BLEND, GL_DEPTH_TEST</td></tr>
</table>

### Clearing
<blockquote>
GL_CLEAR(<i>enabled</i>)
</blockquote>

Enable the clear operation before computing the filter.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>enabled</i></td>			<td>Either TRUE or FALSE.</td></tr>
</table>

### Blending
<blockquote>
GL_BLEND(<i>sourceFactor</i>, <i>destinationFactor</i>, <i>blendingFunction</i>)
</blockquote>

Enable blending in the filter and setup the parameters.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>sourceFactor</i></td>			<td>Blending factor for the source.</td></tr>
<tr class="glipDescrRow"><td><i>destinationFactor</i></td>		<td>Blending factor for the destination.</td></tr>
<tr class="glipDescrRow"><td><i>blendingFunction</i></td>		<td>Blending equation.</td></tr>
</table>

### Depth Test
<blockquote>
GL_DEPTH_TEST(depthTestFunction)
</blockquote>

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>depthTestFunction</i></td>		<td>Function for the depth test.</td></tr>
</table>

## Pipeline Layout
### Pipeline Layout
<blockquote>
PIPELINE_LAYOUT:<i>name</i><br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>elements</i><br>
}
</blockquote>

Define a pipeline layout.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td>				<td>Name of the pipeline layout.</td></tr>
<tr class="glipDescrRow"><td><i>elements</i></td>			<td>List of the elements in the layout.</td></tr>
</table>

#### Input Ports
<blockquote>
INPUT_PORTS(<i>portName0</i> [, ...])
</blockquote>

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>portName0</i></td>			<td>Name of the first input port.</td></tr>
<tr class="glipDescrRow"><td><i>portNameN</i></td>			<td>...</td></tr>
</table>

#### Output Ports
<blockquote>
OUTPUT_PORTS(<i>portName0</i> [, ...])
</blockquote>

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>portName0</i></td>			<td>Name of the first output port.</td></tr>
<tr class="glipDescrRow"><td><i>portNameN</i></td>			<td>...</td></tr>
</table>

#### Filter Instance
<blockquote>
FILTER_INSTANCE:<i>name</i>
</blockquote>
<blockquote>
FILTER_INSTANCE:<i>name</i>(<i>filterLayoutName</i>)
</blockquote>

#### Pipeline Instance
<blockquote>
PIPELINE_INSTANCE:<i>name</i>
</blockquote>
<blockquote>
PIPELINE_INSTANCE:<i>name</i>(<i>pipelineLayoutName</i>)
</blockquote>

#### Connection
<blockquote>
CONNECTION(<i>elementOutName</i>, <i>outputPortName</i>, <i>elementInName</i>, <i>inputPortName</i>)
</blockquote>

### Required Pipeline
<blockquote>
REQUIRED_PIPELINE:<i>name</i>(<i>requiredPipelineName</i>)
</blockquote>

### Main Pipeline
<blockquote>
MAIN_PIPELINE:<i>name</i><br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>elements</i><br>
}<br>
</blockquote>
<blockquote>
MAIN_PIPELINE:<i>name</i>(<i>basePipelineLayoutName</i>)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp; <i>ports</i><br>
}
</blockquote>

# Script Example #############
## Script ####################
## Loading Code ##############

**/
		class GLIP_API LayoutLoader
		{
			public : 
				/**
				\struct PipelineScriptElements
				\brief Contains data about a script, possibly made before the load operation.

				WARNING : It does not explore included files which might lead to an incomplete list of requirements.
				**/
				struct GLIP_API PipelineScriptElements
				{	
											/// Unique identifier.
					std::string					unique;
											/// Paths added by the script.
					std::vector<std::string> 			addedPaths,
											/// File included by the script.		
											includedFiles,		
											/// Names of the formats required by the script.
											requiredFormats,	
											/// Names of the geometries required by the script.
											requiredGeometries,	
											/// Names of the pipelines required by the script.
											requiredPipelines,	
											/// Names of the modules called by the script.
											modulesCalls,		
											/// Names of the formats contained in the script.
											formats,		
											/// Names of the shader source code contained in the script.
											shaderSources,		
											/// Names of the geometries contained in the script.
											geometries,		
											/// Names of the filter layouts contained in the script.
											filtersLayout,		
											/// Names of the pipelines layout contained in the script.
											pipelines;		
											/// List of the input ports for each pipeline contained in the script (same order as pipelines).
					std::vector< std::vector<std::string> >		pipelineInputs;	
											/// List of the output ports for each pipeline contained in the script (same order as pipelines).	
					std::vector< std::vector<std::string> >		pipelineOutputs;	
											/// Name of the main pipeline contained in the script.
					std::string					mainPipeline;		
											/// List of the input ports for the main pipeline contained in the script.
					std::vector<std::string> 			mainPipelineInputs,	
											/// List of the output ports for the main pipeline contained in the script.
											mainPipelineOutputs;	
				};

			private :
				static const char* keywords[LL_NumKeywords];

				bool 						isSubLoader;

				// Reading dynamic :
				std::string					currentPath;
				std::vector<std::string>			dynamicPaths;
				std::vector<LayoutLoaderKeyword>		associatedKeyword;
				std::vector<std::string>			uniqueList;
				std::map<std::string, ShaderSource>		sharedCodeList;
				std::map<std::string, HdlTextureFormat> 	formatList;
				std::map<std::string, ShaderSource> 		sourceList;
				std::map<std::string, GeometryModel>		geometryList;
				std::map<std::string, FilterLayout> 		filterList;
				std::map<std::string, PipelineLayout> 		pipelineList;

				// Static :
				std::vector<std::string>			staticPaths;
				std::map<std::string,HdlTextureFormat>		requiredFormatList;
				std::map<std::string,GeometryModel>		requiredGeometryList;
				std::map<std::string,PipelineLayout>		requiredPipelineList;
				std::map<std::string,LayoutLoaderModule*>	modules;			// Using pointers to avoid conflict between polymorphism and object slicing.

				// Tools :
				LayoutLoader(const LayoutLoader& master);

				LayoutLoaderKeyword getKeyword(const std::string& str);

				void 	clean(void);
				void	classify(const std::vector<VanillaParserSpace::Element>& elements, std::vector<LayoutLoaderKeyword>& associatedKeywords);
				bool	fileExists(const std::string& filename, std::string& source, const bool test=false);
				void	loadFile(const std::string& filename, std::string& content, std::string& usedPath);
				void	preliminaryTests(const VanillaParserSpace::Element& e, char nameProperty, int minArguments, int maxArguments, char bodyProperty, const std::string& objectName);
				ShaderSource enhanceShaderSource(const std::string& str, const std::string& sourceName, int startLine=1);
				void	append(LayoutLoader& subLoader);
				void 	appendPath(const VanillaParserSpace::Element& e);
				void	includeFile(const VanillaParserSpace::Element& e);
				bool	checkUnique(const VanillaParserSpace::Element& e);
				void	buildRequiredFormat(const VanillaParserSpace::Element& e);
				void	buildRequiredGeometry(const VanillaParserSpace::Element& e);
				void	buildRequiredPipeline(const VanillaParserSpace::Element& e);
				void    moduleCall(const VanillaParserSpace::Element& e);
				void	buildSharedCode(const VanillaParserSpace::Element& e);
				void	buildFormat(const VanillaParserSpace::Element& e);
				void	buildShaderSource(const VanillaParserSpace::Element& e);
				void	buildGeometry(const VanillaParserSpace::Element& e);
				void	buildFilter(const VanillaParserSpace::Element& e);
				void	buildPipeline(const VanillaParserSpace::Element& e);
				void	process(const std::string& code, std::string& mainPipelineName, const std::string& sourceName);

				void	listPipelinePorts(const VanillaParserSpace::Element& e, std::vector<std::string>& inputs, std::vector<std::string>& outputs);

			public :
				LayoutLoader(void);
				virtual ~LayoutLoader(void);

				const std::vector<std::string>& paths(void) const;
				void clearPaths(void);
				void addToPaths(std::string p);
				void addToPaths(const std::vector<std::string>& paths);
				bool removeFromPaths(const std::string& p);

				AbstractPipelineLayout getPipelineLayout(const std::string& source, std::string sourceName=""); //can be a file or directly the source
				Pipeline* getPipeline(const std::string& source, std::string pipelineName="", std::string sourceName="");

				void addRequiredElement(const std::string& name, const HdlAbstractTextureFormat& fmt);
				void addRequiredElement(const std::string& name, const GeometryModel& mdl);
				void addRequiredElement(const std::string& name, AbstractPipelineLayout& layout);
				int  clearRequiredElements(void);
				int  clearRequiredElements(const std::string& name);

				PipelineScriptElements listElements(const std::string& source, std::string sourceName="");

				void addModule(LayoutLoaderModule* module, bool replace=false);
				std::vector<std::string> listModules(void) const;
				const LayoutLoaderModule& module(const std::string& name) const;
				void removeModule(const std::string& name);

				static const char* getKeyword(LayoutLoaderKeyword k); 
		};

/**
\class LayoutWriter
\brief Get equivalent pipeline code from a pipeline layout.

The layout writer enables you to write a pipeline to a Pipeline Script file. Note that the uniforms values might be lost at loading. In order to avoid this problem, you have to use a UniformsLoader to store and then load the correct profile (all the uniforms values).
\code
	LayoutWriter writer;
	
	std::string code = writer(mainPipeline);

	// Or directly to the disk : 
	writer.writeToFile(mainPipeline, "./myPipeline.ppl");
\endcode
**/
		class GLIP_API LayoutWriter
		{
			private :
				VanillaParserSpace::Element write(const HdlAbstractTextureFormat& hLayout, const std::string& name);
				VanillaParserSpace::Element write(const ShaderSource& source, const std::string& name);
				VanillaParserSpace::Element write(const GeometryModel& mdl, const std::string& name);
				VanillaParserSpace::Element write(const AbstractFilterLayout& fLayout);
				VanillaParserSpace::Element write(const AbstractPipelineLayout& pLayout, bool isMain=false);

				std::string code;

			public :
				LayoutWriter(void);
				virtual ~LayoutWriter(void);

				std::string operator()(const AbstractPipelineLayout& pipelineLayout);
				void writeToFile(const AbstractPipelineLayout& pipelineLayout, const std::string& filename);
		};
	}
}

#endif // LAYOUTLOADER_HPP_INCLUDED
