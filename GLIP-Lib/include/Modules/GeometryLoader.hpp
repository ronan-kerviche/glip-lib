/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GeometryLoader.hpp                                                                        */
/*     Original Date : May 29th 2015                                                                             */
/*                                                                                                               */
/*     Description   : Geometry loaders for Wavefront Object (OBJ) and STL file formats.                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    GeometryLoader.hpp
 * \brief   Geometry loaders for Wavefront Object (OBJ) and STL file formats.
 * \author  R. KERVICHE
 * \date    May 29th 2015
**/

#ifndef __GEOMETRY_LOADER_INCLUDE__
#define __GEOMETRY_LOADER_INCLUDE__

	// Includes
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Core/Geometry.hpp"
	#include "Modules/LayoutLoaderModules.hpp"

namespace Glip
{
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::CorePipeline::GeometryPrimitives;

	namespace Modules
	{
		/**
		\class OBJLoader
		\brief Wavefront Object file loader.
		**/
		class OBJLoader : public LayoutLoaderModule
		{
			private : 
				struct UnshapedData
				{
					std::vector<GLfloat> 	x,
								y,
								z,
								nx,
								ny,
								nz,
								u,
								v;
					std::vector<GLuint>	av,
								an,
								at,
								bv,
								bn,
								bt,
								cv,
								cn,
								ct;
					bool 	hasNormals,
						hasTexCoords;
				};

				static GLfloat readNextNumber(const std::string& line, size_t& p, const int lineNumber, const std::string& sourceName);
				static void readFaceComponent(const std::string& line, size_t& p, std::vector<GLuint>& v, std::vector<GLuint>& n, std::vector<GLuint>& t, const int lineNumber, const std::string& sourceName);
				static void processLine(const std::string& line, UnshapedData& data, const bool strict, const int lineNumber, const std::string& sourceName);
				static bool testIndices(const UnshapedData& data);
				static void reorderData(UnshapedData& data);

			public : 
				OBJLoader(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static CustomModel load(const std::string& filename, const bool strict=false);
		};

		//class STLLoader : public LayoutLoaderModule
	}
}

#endif

