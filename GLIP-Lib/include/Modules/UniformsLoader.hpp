/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : UniformsLoader.hpp                                                                        */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Uniforms variables save/load.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    UniformsLoader.hpp
 * \brief   Uniforms variables save/load.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

#ifndef __UNIFORMS_VARS_LOADER_INCLUDE__
#define __UNIFORMS_VARS_LOADER_INCLUDE__

	// Includes 
	#include <map>
	#include "Core/LibTools.hpp"
	#include "Core/HdlDynamicData.hpp"
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
		/// Keywords used by UniformsLoader. Use UniformsLoader::getKeyword() to get the actual string.
		enum UniformsLoaderKeyword
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

/**
\class UniformsLoader
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
	UniformsLoader uLoader;

	// Load values from a current Pipeline, replace existing values :
	uLoader.load(mainPipeline, true);

	// Get the corresponding code :
	std::string uCode = uLoader.getCode( mainPipeline.getTypeName() );

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
		class GLIP_API UniformsLoader
		{
			private :
				static const char* keywords[UL_NumKeywords];

			public : 
				// Prototypes : 
				class Node;
				class Resource;

				/// Const iterator over the nodes (refers to a std::pair, use second to access the const UniformsLoader::Node& object).
				typedef std::map<const std::string, Node>::const_iterator 	NodeConstIterator;
				/// Iterator over the nodes (refers to a std::pair, use second to access the UniformsLoader::Node& object).
				typedef std::map<const std::string, Node>::iterator 		NodeIterator;
				/// Const iterator over the resources (refers to a std::pair, use second to access the const UniformsLoader::Resource& object).
				typedef std::map<const std::string, Resource>::const_iterator 	ResourceConstIterator;
				/// Iterator over the resources (refers to a std::pair, use second to access the UniformsLoader::Resource& object).
				typedef std::map<const std::string, Resource>::iterator 	ResourceIterator;

				/// Flags used for loading data from pipelines, strings or files.
				enum LoadingFilter
				{
					/// Load all the data, append new data, replace existing data.
					LoadAll,
					/// Load only new data, throw an Exception if some data already exists.
					NewOnly,
					/// Load only new data, ignore data which already exists.
					NewOnlySilent,
					/// Replace existing data only, throw an Exception if there is some new data.
					ReplaceOnly,
					/// Replace existing data only, ignore new data.
					ReplaceOnlySilent
				};

/**
\class Resource
\brief Resource (variable) of a structure (filter, pipeline).
**/
				class GLIP_API Resource
				{
					private : 
						std::string 	name;
						HdlDynamicData* data;

					public : 
						/// This flag can be used to manually set the variable as modified or non-modified, in order to limit driver work time.
						bool modified;

						Resource(void);
						Resource(const Resource& cpy);
						Resource(const VanillaParserSpace::Element& e);
						Resource(const std::string& _name, const HdlDynamicData& _data);
						
						~Resource(void);

						Resource& operator=(const Resource& cpy);
						
						const std::string& getName(void) const;
						const HdlDynamicData& object(void) const;
						HdlDynamicData& object(void);
						int applyTo(Filter& filter, bool forceWrite=true) const;
						VanillaParserSpace::Element getCodeElement(void) const;
				};

/**
\class Node
\brief Container structure (filter, pipeline).
**/
				class GLIP_API Node
				{
					private :
						std::string 				name;
						std::map<const std::string, Node> 	subNodes;
						std::map<const std::string, Resource>	resources;

					public :
						Node(void);
						Node(const VanillaParserSpace::Element& e);
						Node(const std::string& _name, Pipeline& pipeline, const AbstractPipelineLayout& current);
						Node(const std::string& _name, Pipeline& pipeline, const AbstractPipelineLayout& parent, const AbstractFilterLayout& current);				
						Node(const Node& cpy);
						~Node(void);

						Node& operator=(const Node& cpy);
						void softCopy(const Node& cpy, bool appendNewNodes=true, bool appendNewResources=true, bool removeAbsentNodes=true, bool removeAbsentResources=true);
						void softCopy(const Node& cpy, LoadingFilter loadingFilter);

						const std::string& getName(void) const;
						bool isFilter(void) const;
						bool isPipeline(void) const;
						bool empty(void) const;
						void clear(void);
						bool hasModifications(void) const;
						void clearModifiedFlags(bool value=false);

						int getNumSubNodes(void) const;
						std::vector<std::string> getSubNodesNamesList(void) const;
						bool subNodeExists(const std::string& nodeName) const;
						const Node& subNode(const std::string& nodeName) const;
						Node& subNode(const std::string& nodeName);
						void eraseNode(const std::string& nodeName);
						NodeConstIterator nodeBegin(void) const;
						NodeConstIterator nodeEnd(void) const;
						NodeConstIterator findNode(const std::string& nodeName) const;
						NodeIterator nodeBegin(void);
						NodeIterator nodeEnd(void);
						NodeIterator findNode(const std::string& nodeName);

						int getNumResources(void) const;
						std::vector<std::string> getResourcesNamesList(void) const;
						bool resourceExists(const std::string& resourceName) const;
						const Resource& resource(const std::string& resourceName) const;
						Resource& resource(const std::string& resourceName);
						void eraseResource(const std::string& resourceName);
						ResourceConstIterator resourceBegin(void) const;
						ResourceConstIterator resourceEnd(void) const;
						ResourceConstIterator findResource(const std::string& resourceName) const;
						ResourceIterator resourceBegin(void);
						ResourceIterator resourceEnd(void);
						ResourceIterator findResource(const std::string& resourceName);

						int applyTo(Pipeline& pipeline, const AbstractPipelineLayout& current, bool forceWrite=true, bool silent=false) const;
						int applyTo(Pipeline& pipeline, Filter& filter, bool forceWrite=true, bool silent=false) const;
						VanillaParserSpace::Element getCodeElement(void) const;
				};

			private :
				std::map<const std::string, Node> nodes;

			public :
				UniformsLoader(void);
				UniformsLoader(const UniformsLoader& cpy);
				~UniformsLoader(void);

				void load(std::string source, LoadingFilter loadingFilter=LoadAll, int lineOffset=1);
				void load(Pipeline& pipeline, LoadingFilter loadingFilter=LoadAll);
				void load(const UniformsLoader& subLoader, LoadingFilter loadingFilter=LoadAll);
				bool empty(void) const;
				void clear(void);
				void clear(const std::string& name);
				bool hasPipeline(const std::string& name) const;
				std::vector<std::string> getPipelinesTypeNames(void) const;
				const Node& getRootNode(const std::string& name) const;
				Node& getRootNode(const std::string& name);
				NodeConstIterator rootNodeBegin(void) const;
				NodeConstIterator rootNodeEnd(void) const;
				NodeIterator rootNodeBegin(void);
				NodeIterator rootNodeEnd(void);
				int applyTo(Pipeline& pipeline, bool forceWrite=true, bool silent=false) const;
				std::string getCode(void) const;
				std::string getCode(const std::string& name) const;
				void writeToFile(const std::string& filename) const;

				static const char* getKeyword(UniformsLoaderKeyword k);
		};
	}
}

#endif

