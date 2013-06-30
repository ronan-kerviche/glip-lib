/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : UniformVarsLoader.hpp                                                                     */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Uniforms variables save/load.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    UniformVarsLoader.hpp 
 * \brief   Uniforms variables save/load.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

#ifndef __UNIFORMS_VARS_LOADER_INCLUDE__
#define __UNIFORMS_VARS_LOADER_INCLUDE__

	#include <map>
	#include "../Core/HdlTexture.hpp"
	#include "../Core/HdlShader.hpp"
	#include "../Core/Pipeline.hpp"
	#include "VanillaParser.hpp"

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
					VanillaParserSpace::Element getCode(void);
				};

				struct RessourceNode
				{
					std::string name;
					
					std::vector<RessourceNode> 	subNodes;
					std::vector<Ressource>		ressources;

					int apply(Pipeline& pipeline, __ReadOnly_PipelineLayout& current);
				};			

				std::vector<RessourceNode> ressources;

				void processNode(std::string body, RessourceNode& root);			
				void processNode(Pipeline& pipeline, __ReadOnly_PipelineLayout& current, RessourceNode& root);		
				VanillaParserSpace::Element getNodeCode(RessourceNode& node);
			public : 
				UniformsVarsLoader(void);
				~UniformsVarsLoader(void);

				void load(std::string source, bool replace=false);
				void load(Pipeline& pipeline, bool replace=false);
				void clear(const std::string& name="");
				bool hasPipeline(const std::string& name) const;
				bool empty(void) const;
				int applyTo(Pipeline& pipeline);
				std::string getCode(void);
				void writeToFile(const std::string& filename);
		};
	}
}

#endif

