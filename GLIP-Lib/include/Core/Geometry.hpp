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
	#include "Core/OglInclude.hpp"
	#include "Core/HdlVBO.hpp"

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
			\class GeometryModel
			\brief Geometry stored on the host.
			**/
			class GeometryModel
			{
				public : 
					enum GeometryType 
					{
						CustomModel,
						StandardQuad,
						PointsGrid2D,
						PointsGrid3D,
						Unknown
					};

				private :
					std::vector<GLfloat> 	pos,
								tex;
					std::vector<GLuint>	elements;

				protected :
					// Tools :
					GeometryModel(const GeometryType& _type, const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord);

					GLuint addVertex2D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					GLuint addVertex3D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& z=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					GLuint addElement(GLuint a);
					GLuint addElement(GLuint a, GLuint b);
					GLuint addElement(GLuint a, GLuint b, GLuint c);
					GLuint addElement(GLuint a, GLuint b, GLuint c, GLuint d);
					GLuint addElement(const std::vector<GLuint>& indices);
					GLfloat& x(GLuint i);
					GLfloat& y(GLuint i);
					GLfloat& z(GLuint i);
					GLfloat& u(GLuint i);
					GLfloat& v(GLuint i);
					GLuint& a(GLuint i);
					GLuint& b(GLuint i);
					GLuint& c(GLuint i);
					GLuint& d(GLuint i);

				public :
					const GeometryType	type;
					const bool		hasTexCoord;
					const int		dim,
								numVerticesPerEl;
					const GLenum 		primitiveGL;

					GeometryModel(const GeometryModel& mdl);
					virtual ~GeometryModel(void);

					GLuint getNumVertices(void) const;
					GLuint getNumElements(void) const;
					const GLenum& getGLPrimitive(void) const;
					const GLfloat& x(GLuint i) const;
					const GLfloat& y(GLuint i) const;
					const GLfloat& z(GLuint i) const;
					const GLfloat& u(GLuint i) const;
					const GLfloat& v(GLuint i) const;
					const GLuint& a(GLuint i) const;
					const GLuint& b(GLuint i) const;
					const GLuint& c(GLuint i) const;
					const GLuint& d(GLuint i) const;
					bool operator==(const GeometryModel& mdl) const;

					HdlVBO* getVBO(GLenum freq) const;

					static int getNumVerticesFromPrimitive(const GLenum& _primitiveGL);
			};

			/**
			\class GeometryInstance
			\brief Instance of the GeometryModel. Stored on GPU (VBO).
			**/
			class GeometryInstance
			{
				private :
					// Static memory bank :
					static std::vector<HdlVBO*> 		vbos;
					static std::vector<GeometryModel*>	models;
					static std::vector<int>			counters;

					// Current :
					int id;

				public :
					GeometryInstance(const GeometryModel& mdl, GLenum freq);
					GeometryInstance(const GeometryInstance& instance);
					~GeometryInstance(void);

					const GeometryModel& model(void) const;
					const HdlVBO& vbo(void) const;
					void draw(void);
			};

			// Geometries :
			namespace GeometryPrimitives
			{
				/**
				\class StandardQuad
				\brief Geometry : the standard quad.
				**/
				class StandardQuad : public GeometryModel
				{
					public :
						StandardQuad(void);
				};

				/**
				\class PointsGrid2D
				\brief Geometry : a 2D grid of points.
				**/
				class PointsGrid2D : public GeometryModel
				{
					public :
						PointsGrid2D(int w, int h);
				};

				/**
				\class PointsGrid3D
				\brief Geometry : a 3D grid of points.
				**/
				class PointsGrid3D : public GeometryModel
				{
					public :
						PointsGrid3D(int w, int h, int d);
				};

				/**
				\class CustomModel
				\brief Geometry : build a model.
				**/
				class CustomModel : public GeometryModel
				{
					public :
						CustomModel(const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord);

						GLuint newVertex2D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
						GLuint newVertex3D(const GLfloat& x=0.0f, const GLfloat& y=0.0f, const GLfloat& z=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
						GLuint newElement(GLuint a);
						GLuint newElement(GLuint a, GLuint b);
						GLuint newElement(GLuint a, GLuint b, GLuint c);
						GLuint newElement(GLuint a, GLuint b, GLuint c, GLuint d);
						GLuint newElement(const std::vector<GLuint>& indices);
				};
			}
		}
	}

#endif

