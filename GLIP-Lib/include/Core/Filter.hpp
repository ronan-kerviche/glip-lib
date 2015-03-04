/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Filter.hpp                                                                                */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Filter object                                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Filter.hpp
 * \brief   Filter object
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIPLIB_FILTER__
#define __GLIPLIB_FILTER__

	// Includes
	#include "devDebugTools.hpp"
	#include "Core/LibTools.hpp"
        #include "Core/Component.hpp"
        #include "Core/HdlTexture.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Prototypes
			class ShaderSource;
			class HdlShader;
			class HdlProgram;
			class HdlVBO;
			class HdlFBO;
		}

		namespace CorePipeline
		{
			// Prototypes
			class Pipeline;
			class FilterLayout;
			class GeometryModel;
			class GeometryInstance;

			// Objects
			/**
			\class AbstractFilterLayout
			\brief Filter layout (Read Only).
			**/
			class GLIP_API AbstractFilterLayout : virtual public AbstractComponentLayout, virtual public HdlAbstractTextureFormat
			{
				private :
					// Data
					ShaderSource 	*shaderSources[HandleOpenGL::numShaderTypes];
					GeometryModel	*geometryModel;
					bool		clearing,
							blending,
							depthTesting,
							//isStandardVertex, 
							isStandardGeometry;
					GLenum		sFactor,
							dFactor,
							blendingEquation,
							depthTestingFunction;

					// Friends
					friend class FilterLayout;

				protected :
					// Tools
					AbstractFilterLayout(const std::string& type, const HdlAbstractTextureFormat& f);

				public :
					// Tools
					AbstractFilterLayout(const AbstractFilterLayout&);
					virtual ~AbstractFilterLayout(void);
					//ShaderSource* getShaderSource(void) const;
					//ShaderSource& getFragmentSource(void) const;
					const ShaderSource* getShaderSource(GLenum shaderType) const;
					GeometryModel& getGeometryModel(void) const;

					bool isStandardVertexSource(void) const;
					bool isStandardGeometryModel(void) const;
					int getNumUniformVars(void) const;
					bool isClearingEnabled(void) const;
					void enableClearing(void);
					void disableClearing(void);
					bool isBlendingEnabled(void) const;
					const GLenum& getSFactor(void) const;
					const GLenum& getDFactor(void) const;
					const GLenum& getBlendingEquation(void) const;
					void enableBlending(const GLenum& _sFactor = GL_ONE, const GLenum& _dFactor = GL_ONE, const GLenum& _blendingEquation = GL_FUNC_ADD);
					void disableBlending(void);
					bool isDepthTestingEnabled(void) const;
					const GLenum& getDepthTestingFunction(void) const;
					void enableDepthTesting(const GLenum& _depthTestingFunction = GL_LESS);
					void disableDepthTesting(void);
			};

			/**
			\class FilterLayout
			\brief Filter layout.
			**/
			class GLIP_API FilterLayout : virtual public ComponentLayout, virtual public AbstractFilterLayout
			{
				public :
					// Tools
					FilterLayout(const std::string& type, const HdlAbstractTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex = NULL, GeometryModel* geometry = NULL);
			};

			/**
			\class Filter
			\brief Filter object.
			**/
			class GLIP_API Filter : virtual public Component, virtual public AbstractFilterLayout
			{
				private :
					// Data
					HdlShader  			*shaders[HandleOpenGL::numShaderTypes];
					HdlProgram 			*prgm;
					GeometryInstance		*geometry;
					bool				firstRun,
									broken;
					std::vector<HdlTexture*>	arguments;

				protected :
					// Tools
					Filter(const AbstractFilterLayout&, const std::string& name);

					void setInputForNextRendering(int id, HdlTexture* ptr);
					void process(HdlFBO& renderer);

					// Friends
					friend class Pipeline;

				public :
					// Tools
					virtual ~Filter(void);

					HdlProgram& program(void);
					bool wentThroughFirstRun(void) const;
					bool isBroken(void) const;
			};
		}
	}

#endif
