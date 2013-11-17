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
		enum LayoutLoaderKeyword
		{
			KW_LL_FORMAT_LAYOUT,
			KW_LL_SHADER_SOURCE,
			KW_LL_FILTER_LAYOUT,
			KW_LL_PIPELINE_LAYOUT,
			KW_LL_PIPELINE_MAIN,
			KW_LL_INCLUDE_FILE,
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
			KW_LL_SHARED_SOURCE,
			KW_LL_INCLUDE_SHARED_SOURCE,
			KW_LL_GEOMETRY,
			KW_LL_GRID_2D,
			KW_LL_GRID_3D,
			KW_LL_CUSTOM_MODEL,
			KW_LL_GEOMETRY_FROM_FILE,
			KW_LL_STANDARD_QUAD,
			KW_LL_VERTEX,
			KW_LL_ELEMENT,
			KW_LL_ADD_PATH,
			KW_LL_MODULE_CALL,
			LL_NumKeywords,
			LL_UnknownKeyword
		};

		extern const char* keywordsLayoutLoader[LL_NumKeywords];

/**
\class LayoutLoader
\brief Load and save pipelines layouts to file

The LayoutLoader module enables you to use dynamic pipeline saved in a file or a standard string. It will create either a Glip::Core::PipelineLayout or a Glip::Core::Pipeline that you can use directly or combined with other pipeline structures.

The script must be structured with the following commands (but no special order is needed except standard declaration order) :

- Format for the texture : <BR>
<b>TEXTURE_FORMAT</b>:<i>format_name</i>(<i>integer width</i>, <i>integer height</i>, <i>GLEnum mode</i>, <i>GLEnum depth</i>, [ <i>GLEnum minFiltering</i>, <i>GLEnum maxFiltering</i>, <i>GLEnum sWrapping</i>, <i>GLEnum TWrapping</i>, <i>integer maximum_mipmap_level</i> ])

- Required format to be provided by the application (for dynamic sizes). See LayoutLoader::addRequiredElement() : <BR>
If the format is not found in the required element, it is searched in the known formats. You can also use character '*' to keep some of the parameters or replace them by specifying a new value. <BR>
<b>REQUIRED_FORMAT</b>:<i>format_name</i>(<i>required_format_name</i> [, <i>integer width / *</i>, <i>integer height / *</i>, <i>GLEnum mode / *</i>, <i>GLEnum depth / *</i>, <i>GLEnum minFiltering / *</i>, <i>GLEnum maxFiltering / *</i>, <i>GLEnum sWrapping / *</i>, <i>GLEnum TWrapping / *</i>, <i>integer maximum_mipmap_level / *</i>])

- Geometry : <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GeometryType</i>, <i>param1</i>, ... <i>paramN</i>) <BR>
Examples : <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GRID_2D</i>, <i>integer : width</i>, <i>integer : height</i>) <BR>
<b>GEOMETRY</b>:<i>geometry_name</i>(<i>GRID_3D</i>, <i>integer : width</i>, <i>integer : height</i>, <i>interger : depth</i>) <BR>
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
<b>MODULE_CALL</b> : <i>module_name</i> (<i> module_arguments</i>) <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>(Optional)</i> <BR>
} <BR>

- Shader source code, from the same file : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i>
{
	<i>source code</i>
}

- Shader source code, from the another file : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i>(<i>string filename</i>)

- Filter layout :
<b>FILTER_LAYOUT</b>:<i>filter_layout_name</i>(<i>format_name</i>, <i>fragment_shader_source</i> [, <i>vertex_shader_source</i>/<b>DEFAULT_VERTEX_SHADER</b>, <b>CLEARING_ON</b>/<b>CLEARING_OFF</b>, <b>BLENDING_ON</b>/<b>BLENDING_OFF</b>])

- Pipeline layout : <BR>
<b>PIPELINE_LAYOUT</b>:<i>pipeline_layout_name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>INPUT_PORTS</b>(<i>input_port_name_1</i> [,<i>input_port_name_2</i>, ..., <i>input_port_name_n</i>] ) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>OUTPUT_PORTS</b>(<i>output_port_name_1</i> [,<i>output_port_name_2</i>, ..., <i>output_port_name_n</i>] ) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>FILTER_INSTANCE</b>:<i>filter_instance_name</i>(<i>filter_layout_name</i>) <BR>
&nbsp;&nbsp;&nbsp;&nbsp; ... <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>PIPELINE_INSTANCE</b>:<i>pipeline_instance_name</i>(<i>pipeline_layout_name</i>) <BR>
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
<i>Same description as PIPELINE_LAYOUT, but starting with </i> <b>PIPELINE_MAIN</b>.

- Add to the search path : <BR>
<b>ADD_PATH</b>(/some/path/) <BR>

- Include another script, in order to use some of its definition (format, source, filter or pipeline layout) : <BR>
<b>INCLUDE_FILE</b>(<i>string filename</i>)

- Distribute code to the shaders code with the SHARED_CODE marker. Insert them in the SHADER_SOURCE elements by adding INSERT_SHARED_CODE marker : <BR>
<b>SHARED_SOURCE:shared_segment_name</b> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>// shared code goes here...</i> <BR>
} <BR>
&nbsp;&nbsp; Then use : <BR>
<b>SHADER_SOURCE</b>:<i>source_name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <b>INCLUDE_SHARED_SOURCE:shared_segment_name</b> // The shared code will be inserted here... <BR>
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
		// Declare some input and output ports for this pipeline ::
		INPUT_PORTS(texInput)
		OUTPUT_PORTS(texOutput)

		FILTER_INSTANCE:instHello(helloFilter)

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
		Pipeline* myPipeline2 = new Pipeline(myLayout,"Pipeline1");

		// For a single pipeline :
		Pipeline* myPipelineU = loader("./path/otherPipeline.ppl","myPipelineName");

		// use them, see Glip::Core::Pipeline documentation...

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

**/
		class LayoutLoader
		{
			public : 
				/**
				\struct PipelineScriptElements
				\brief Contains data about a script, possibly made before the load operation.

				WARNING : It does not explore included files which might to incomplete list of requirements.
				**/
				struct PipelineScriptElements
				{
					std::vector<std::string> 			addedPaths,		/// Paths added by the script.
											includedFiles,		/// File included by the script.
											requiredFormats,	/// Names of the formats required by the script.
											requiredGeometries,	/// Names of the geometries required by the script.
											requiredPipelines,	/// Names of the pipelines required by the script.
											modulesCalls,		/// Names of the modules called by the script.
											formatsLayout,		/// Names of the formats contained in the script.
											shaderSources,		/// Names of the shader source code contained in the script.
											geometries,		/// Names of the geometries contained in the script.
											filtersLayout,		/// Names of the filter layouts contained in the script.
											pipelines;		/// Names of the pipelines layout contained in the script.

					std::vector< std::vector<std::string> >		pipelineInputs;		/// List of the input ports for each pipeline contained in the script (same order as pipelines).
					std::vector< std::vector<std::string> >		pipelineOutputs;	/// List of the output ports for each pipeline contained in the script (same order as pipelines).

					std::string					mainPipeline;		/// Name of the main pipeline contained in the script.

					std::vector<std::string> 			mainPipelineInputs,	/// List of the input ports for the main pipeline contained in the script.
											mainPipelineOutputs;	/// List of the output ports for the main pipeline contained in the script.
				};

			private :
				bool 						isSubLoader;

				// Reading dynamic :
				std::string					currentPath;
				std::vector<std::string>			dynamicPaths;
				std::vector<LayoutLoaderKeyword>		associatedKeyword;
				std::map<std::string, std::string>		sharedCodeList;
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
				LayoutLoaderKeyword getKeyword(const std::string& str);

				void 	clean(void);
				void	classify(const std::vector<VanillaParserSpace::Element>& elements, std::vector<LayoutLoaderKeyword>& associatedKeywords);
				bool	fileExists(const std::string& filename, std::string& source, const bool test=false);
				void	loadFile(const std::string& filename, std::string& content, std::string& usedPath);
				void	preliminaryTests(const VanillaParserSpace::Element& e, char nameProperty, int minArguments, int maxArguments, char bodyProperty, const std::string& objectName);
				void	enhanceShaderSource(std::string& str);
				void	append(LayoutLoader& subLoader);
				void 	appendPath(const VanillaParserSpace::Element& e);
				void	includeFile(const VanillaParserSpace::Element& e);
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
				void	process(const std::string& code, std::string& mainPipelineName);

				void	listPipelinePorts(const VanillaParserSpace::Element& e, std::vector<std::string>& inputs, std::vector<std::string>& outputs);

			public :
				LayoutLoader(void);
				virtual ~LayoutLoader(void);

				const std::vector<std::string>& paths(void) const;
				void clearPaths(void);
				void addToPaths(const std::string& p);
				void addToPaths(const std::vector<std::string>& paths);
				bool removeFromPaths(const std::string& p);

				__ReadOnly_PipelineLayout operator()(const std::string& source); //can be a file or directly the source
				Pipeline* operator()(const std::string& source, std::string pipelineName);

				void addRequiredElement(const std::string& name, const __ReadOnly_HdlTextureFormat& fmt);
				void addRequiredElement(const std::string& name, const GeometryModel& mdl);
				void addRequiredElement(const std::string& name, __ReadOnly_PipelineLayout& layout);
				int  clearRequiredElements(void);
				int  clearRequiredElements(const std::string& name);

				PipelineScriptElements listElements(const std::string& source);

				void addModule(LayoutLoaderModule* module, bool replace=false);
				std::vector<std::string> listModules(void) const;
				const LayoutLoaderModule& module(const std::string& name) const;
				void removeModule(const std::string& name);
		};

/**
\class LayoutWriter
\brief Get equivalent pipeline code from a pipeline layout.

The layout writer enables you to write a pipeline to a Pipeline Script file. Note that the uniforms values might be lost a loading. In order to avoid this problem, you have to use a UniformsVarsLoader to store and then load the correct profile (all the uniforms values).
\code
	LayoutWriter writer;
	
	std::string code = writer(mainPipeline);

	// Or directly to the disk : 
	writer.writeToFile(mainPipeline, "./myPipeline.ppl");
\endcode
**/
		class LayoutWriter
		{
			private :
				VanillaParserSpace::Element write(const __ReadOnly_HdlTextureFormat& hLayout, const std::string& name);
				VanillaParserSpace::Element write(const ShaderSource& source, const std::string& name);
				VanillaParserSpace::Element write(const GeometryModel& mdl, const std::string& name);
				VanillaParserSpace::Element write(const __ReadOnly_FilterLayout& fLayout);
				VanillaParserSpace::Element write(const __ReadOnly_PipelineLayout& pLayout, bool isMain=false);

				std::string code;

			public :
				LayoutWriter(void);
				virtual ~LayoutWriter(void);

				std::string operator()(const __ReadOnly_PipelineLayout& pipelineLayout);
				void writeToFile(const __ReadOnly_PipelineLayout& pipelineLayout, const std::string& filename);
		};
	}
}

#endif // LAYOUTLOADER_HPP_INCLUDED
