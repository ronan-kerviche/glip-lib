/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Geometry.hpp                                                                              */
/*     Original Date : June 29th 2013                                                                            */
/*                                                                                                               */
/*     Description   : Geometry objects and memory management                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Geometry.hpp
 * \brief   Geometry objects and memory management
 * \author  R. KERVICHE
 * \date    June 29th 2013
**/

#ifndef __GLIPLIB_GEOMETRY__
#define __GLIPLIB_GEOMETRY__

	// Includes
	#include "devDebugTools.hpp"
	#include "OglInclude.hpp"
	#include "HdlVBO.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Prototypes
			class HdlVBO;
		}

		using namespace CoreGL;

		namespace CorePipeline
		{
			/**
			\class GeometryFormat
			\brief Geometry stored on the host.
			**/
			class GeometryFormat
			{
				private :
					std::vector<GLfloat> 	pos,
								tex;

				protected :
					// Data :
					std::vector<GLuint>	elements;

					// Tools :
					GeometryFormat(const int& _dim, const int& _numVerticesPerEl, const GLenum& _primitiveGL, const bool& _hasTexCoord);

					int addVertex2D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					int addVertex3D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& z=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					GLfloat& x(int i);
					GLfloat& y(int i);
					GLfloat& z(int i);
					GLfloat& u(int i);
					GLfloat& v(int i);

				public :
					const bool		hasTexCoord;
					const int		dim,
								numVerticesPerEl;
					const GLenum 		primitiveGL;

					GeometryFormat(const GeometryFormat& fmt);
					virtual ~GeometryFormat(void);

					int getNumVertices(void) const;
					int getNumElements(void) const;
					const GLenum& getGLPrimitive(void) const;
					const GLfloat& x(int i) const;
					const GLfloat& y(int i) const;
					const GLfloat& z(int i) const;
					const GLfloat& u(int i) const;
					const GLfloat& v(int i) const;
					bool operator==(const GeometryFormat& fmt) const;

					HdlVBO* getVBO(GLenum freq) const;
			};

			/**
			\class GeometryInstance
			\brief Instance of the GeometryFormat. Stored on GPU (VBO).
			**/
			class GeometryInstance
			{
				private :
					// Static memory bank :
					static std::vector<HdlVBO*> 		vbos;
					static std::vector<GeometryFormat*>	formats;
					static std::vector<int>			counters;

					// Current :
					int id;

				public :
					GeometryInstance(const GeometryFormat& fmt, GLenum freq);
					GeometryInstance(const GeometryInstance& instance);
					~GeometryInstance(void);

					const GeometryFormat& format(void) const;
					const HdlVBO& vbo(void) const;
					void draw(void);
			};

			// Geometries :
			namespace GeometryPrimitives
			{
				/**
				\class StandardQuadGeometry
				\brief Geometry : the standard quad.
				**/
				class StandardQuadGeometry : public GeometryFormat
				{
					public :
						StandardQuadGeometry(void);
				};

				/**
				\class PointsGrid2DGeometry
				\brief Geometry : a 2D grid of points.
				**/
				class PointsGrid2DGeometry : public GeometryFormat
				{
					public :
						PointsGrid2DGeometry(int w, int h);
				};

				/**
				\class PointsGrid3DGeometry
				\brief Geometry : a 3D grid of points.
				**/
				class PointsGrid3DGeometry : public GeometryFormat
				{
					public :
						PointsGrid3DGeometry(int w, int h, int d);
				};
			}
		}
	}

#endif

