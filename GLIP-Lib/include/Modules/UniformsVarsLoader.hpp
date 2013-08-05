/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : UniformsVarsLoader.hpp                                                                     */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Uniforms variables save/load.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    UniformsVarsLoader.hpp
 * \brief   Uniforms variables save/load.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

#ifndef __UNIFORMS_VARS_LOADER_INCLUDE__
#define __UNIFORMS_VARS_LOADER_INCLUDE__

	#include <map>
	#include "Core/HdlTexture.hpp"
	#include "Core/HdlShader.hpp"
	#include "Core/Pipeline.hpp"
	#include "Modules/VanillaParser.hpp"

namespace Glip
{
	using namespace CoreGL;
	using namespace CorePipeline;

	namespace Modules
	{
		enum UniformVarsLoaderKeyword
		{
			KW_UL_PIPELINE,
			KW_UL_FILTER,
			KW_UL_GL_FLOAT,
			KW_UL_GL_FLOAT_VEC2,
			KW_UL_GL_FLOAT_VEC3,
			KW_UL_GL_FLOAT_VEC4,
			KW_UL_GL_DOUBLE,
			KW_UL_GL_DOUBLE_VEC2,
			KW_UL_GL_DOUBLE_VEC3,
			KW_UL_GL_DOUBLE_VEC4,
			KW_UL_GL_INT,
			KW_UL_GL_INT_VEC2,
			KW_UL_GL_INT_VEC3,
			KW_UL_GL_INT_VEC4,
			KW_UL_GL_UNSIGNED_INT,
			KW_UL_GL_UNSIGNED_INT_VEC2,
			KW_UL_GL_UNSIGNED_INT_VEC3,
			KW_UL_GL_UNSIGNED_INT_VEC4,
			KW_UL_GL_BOOL,
			KW_UL_GL_BOOL_VEC2,
			KW_UL_GL_BOOL_VEC3,
			KW_UL_GL_BOOL_VEC4,
			KW_UL_GL_FLOAT_MAT2,
			KW_UL_GL_FLOAT_MAT3,
			KW_UL_GL_FLOAT_MAT4,
			UL_NumKeywords,
			UL_UnknownKeyword
		};

		extern const char* keywordsUniformsVarsLoader[UL_NumKeywords];

/**
\class UniformsVarsLoader
\brief Loads and writes a set of uniforms variables values from a file or a string.

Load, store and manage set of uniforms values for one, or multiple pipelines. The code is set to be human readable : <BR>
- Variable (see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible typenames) : <BR>
<b><i>TYPENAME</i></b> : <i>name</i>( [<i>values</i>] ); <BR>

- Filter : <BR>
<b>FILTER</b> : <i>name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>variables...</i><BR>
}<BR>

- Filter : <BR>
<b>PIPELINE</b> : <i>name</i> <BR>
{ <BR>
&nbsp;&nbsp;&nbsp;&nbsp; <i>filters...</i><BR>
}<BR>

Example : 
\code
PIPELINE:myPipeline
{
	FILTER:firstFilter
	{
		GL_FLOAT:scalar1(1.0)
		GL_VEC3:vector1(0.0, -2.0, 3.0)
	}
}
\endcode

Processing example : 
\code
	UniformsVarsLoader uLoader;

	// Load values from a current Pipeline, replace existing values :
	uLoader.load(mainPipeline, true);

	// Get the corresponding code :
	std::string uCode = uLoader.getCode( mainPipeline.getName() );

	// Get the code of all the data saved : 
	std::string uAllCode = uLoader.getCode();

	// Save to file : 
	uLoader.writeToFile("./currentData.uvd");

	// Clear all : 
	uLoader.clear();

	// Reload (no replacement, if an element already exists it will raise an exception) : 
	uLoader.load("./currentData.uvd");

	// Apply : 
	int c = uLoader.applyTo(mainPipeline);

	std::cout << c << " variables were loaded." << std::endl;
\endcode
**/
		class UniformsVarsLoader
		{
			private :
				struct Ressource
				{
					std::string name;
					GLenum type;
					void* data;

					Ressource(void);
					Ressource(const Ressource& cpy);
					~Ressource(void);

					void build(const VanillaParserSpace::Element& e);
					void build(const std::string& varName, GLenum t, HdlProgram& prgm);
					void apply(Filter& filter);
					VanillaParserSpace::Element getCode(void) const;
				};

				struct RessourceNode
				{
					std::string name;

					std::vector<RessourceNode> 	subNodes;
					std::vector<Ressource>		ressources;

					int apply(Pipeline& pipeline, __ReadOnly_PipelineLayout& current);
					int getNumVariables(void) const;
				};

				std::vector<RessourceNode> ressources;

				void processNode(std::string body, RessourceNode& root);
				void processNode(Pipeline& pipeline, __ReadOnly_PipelineLayout& current, RessourceNode& root);
				VanillaParserSpace::Element getNodeCode(const RessourceNode& node, const bool isRoot = false) const;
			public :
				UniformsVarsLoader(void);
				~UniformsVarsLoader(void);

				void load(std::string source, bool replace=false);
				void load(Pipeline& pipeline, bool replace=false);
				void clear(void);
				void clear(const std::string& name);
				bool hasPipeline(const std::string& name) const;
				std::vector<std::string> getPipelinesTypeNames(void) const;
				bool empty(void) const;
				int getNumVariables(void) const;
				int getNumVariables(const std::string& name) const;
				int applyTo(Pipeline& pipeline);
				std::string getCode(void) const;
				std::string getCode(const std::string& name) const;
				void writeToFile(const std::string& filename) const;
		};
	}
}

#endif

