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
			\class __ReadOnly_FilterLayout
			\brief Filter layout (Read Only).
			**/
			class GLIP_API __ReadOnly_FilterLayout : virtual public __ReadOnly_ComponentLayout, virtual public __ReadOnly_HdlTextureFormat
			{
				private :
					// Data
					ShaderSource 	*vertexSource,
							*fragmentSource;
					GeometryModel	*geometryModel;
					bool blending, clearing;
					bool isStandardVertex, isStandardGeometry;

					// Friends
					friend class FilterLayout;

				protected :
					// Tools
					__ReadOnly_FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& f);

				public :
					// Tools
					__ReadOnly_FilterLayout(const __ReadOnly_FilterLayout&);
					virtual ~__ReadOnly_FilterLayout(void);
					ShaderSource& getVertexSource(void) const;
					ShaderSource& getFragmentSource(void) const;
					GeometryModel& getGeometryModel(void) const;

					bool isBlendingEnabled(void) const;
					void enableBlending(void);
					void disableBlending(void);
					bool isClearingEnabled(void) const;
					void enableClearing(void);
					void disableClearing(void);

					bool isStandardVertexSource(void) const;
					bool isStandardGeometryModel(void) const;
			};

			/**
			\class FilterLayout
			\brief Filter layout.
			**/
			class GLIP_API FilterLayout : virtual public ComponentLayout, virtual public __ReadOnly_FilterLayout
			{
				public :
					// Tools
					FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex = NULL, GeometryModel* geometry = NULL);
			};

			/**
			\class Filter
			\brief Filter object.
			**/
			class GLIP_API Filter : virtual public Component, virtual public __ReadOnly_FilterLayout
			{
				private :
					// Data
					HdlShader*  			vertexShader;
					HdlShader*  			fragmentShader;
					HdlProgram* 			prgm;
					GeometryInstance*		geometry;
					bool				firstRun,
									broken;
					std::vector<HdlTexture*>	arguments;

				protected :
					// Tools
					Filter(const __ReadOnly_FilterLayout&, const std::string& name);

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
