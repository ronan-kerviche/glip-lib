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

namespace Glip
{
	using namespace CoreGL;
	using namespace CorePipeline;

	namespace Modules
	{
		enum UniformVarsLoaderKeyword
		{
			PIPELINE,
			FILTER,
			KW_GL_FLOAT,
			KW_GL_FLOAT_VEC2,
			KW_GL_FLOAT_VEC3,
			KW_GL_FLOAT_VEC4,
			KW_GL_DOUBLE,
			KW_GL_DOUBLE_VEC2,
			KW_GL_DOUBLE_VEC3,
			KW_GL_DOUBLE_VEC4,
			KW_GL_INT,
			KW_GL_INT_VEC2,
			KW_GL_INT_VEC3,
			KW_GL_INT_VEC4,
			KW_GL_UNSIGNED_INT,
			KW_GL_UNSIGNED_INT_VEC2,
			KW_GL_UNSIGNED_INT_VEC3,
			KW_GL_UNSIGNED_INT_VEC4,
			KW_GL_BOOL,
			KW_GL_BOOL_VEC2,
			KW_GL_BOOL_VEC3,
			KW_GL_BOOL_VEC4,
			KW_GL_FLOAT_MAT2,
			KW_GL_FLOAT_MAT3,
			KW_GL_FLOAT_MAT4,
			NumKeywords,
			UnknownKeyword
		};

		extern const char* keywordsUniformsVarsLoader[NumKeywords];

		/*class UniformsVarsLoader
		{
			private : 
				struct Ressource
				{
					
				}

				std::map<std::string, Ressource> 			

			public : 
				UniformsVarsLoader(const std::string& source);
				~UniformsVarsLoader(void);

				int applyTo(Pipeline& pipeline);
		};*/
	}
}

#endif

