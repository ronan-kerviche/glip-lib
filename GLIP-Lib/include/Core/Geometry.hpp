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
	#include <map>
	#include "devDebugTools.hpp"
	#include "Core/LibTools.hpp"
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
			class GLIP_API GeometryModel
			{
				public : 
					/// Description of the type of geometry (automatic).
					enum GeometryType 
					{
						/// Custom model.
						CustomModel,
						/// Standard quad, covering the area between (-1,-1) and (1,1).
						StandardQuad,
						/// Reversed quad, covering the area between (-1,-1) and (1,1).
						ReversedQuad, 
						/// 2D grid of dots.
						PointsGrid2D,
						/// 3D grid of dots.
						PointsGrid3D,
						/// Unknown geometry type.
						Unknown
					};

				private :
					std::vector<GLfloat> 	vertices,
								normals,
								texCoords;
					std::vector<GLuint>	elements;

				protected :
					// Tools :
					GeometryModel(GeometryType _type, int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords);

					void reserveVertices(size_t nVertices);
					void increaseVerticesReservation(size_t nNewVertices);
					void reserveElements(size_t nVertices);
					void increaseElementsReservation(size_t nNewVertices);
					void addVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY=NULL, const GLfloat* interleavedUV=NULL);
					void addVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx=NULL, const GLfloat* ny=NULL, const GLfloat* u=NULL, const GLfloat* v=NULL);
					GLuint addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx=0.0f, const GLfloat& ny=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					void addVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ=NULL, const GLfloat* interleavedUV=NULL);
					void addVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx=NULL, const GLfloat* ny=NULL, const GLfloat* nz=NULL, const GLfloat* u=NULL, const GLfloat* v=NULL);
					GLuint addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx=0.0f, const GLfloat& ny=0.0f, const GLfloat& nz=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
					void addElementsInterleaved(const size_t N, GLuint* interleavedIndices);
					void addElements(const size_t N, GLuint* a, GLuint* b=NULL, GLuint* c=NULL, GLuint* d=NULL);
					GLuint addElement(GLuint a);
					GLuint addElement(GLuint a, GLuint b);
					GLuint addElement(GLuint a, GLuint b, GLuint c);
					GLuint addElement(GLuint a, GLuint b, GLuint c, GLuint d);
					GLuint addElement(const std::vector<GLuint>& indices);
					GLfloat& x(GLuint i);
					GLfloat& y(GLuint i);
					GLfloat& z(GLuint i);
					GLfloat& nx(GLuint i);
					GLfloat& ny(GLuint i);
					GLfloat& nz(GLuint i);
					GLfloat& u(GLuint i);
					GLfloat& v(GLuint i);
					GLuint& a(GLuint i);
					GLuint& b(GLuint i);
					GLuint& c(GLuint i);
					GLuint& d(GLuint i);
					void generateNormals(void);

				public :
								/// Geometry Type.
					const GeometryType	type;
								/// True if it has texture coordinates attached.
					const bool		hasNormals,
								/// Dimension of the geometry (either 2 or 3).
								hasTexCoords;
								/// True if it has normals data attached.
					const int		dim,
								/// Number of vertices per elements of the geometry.
								numVerticesPerElement,
								/// Stride between the elements.
								elementStride;
								/// ID of the primitive for GL.
					const GLenum 		primitiveGL;

					GeometryModel(const GeometryModel& mdl);
					virtual ~GeometryModel(void);

					unsigned int getNumVertices(void) const;
					unsigned int getNumElements(void) const;
					const GLenum& getGLPrimitive(void) const;
					const GLfloat& x(GLuint i) const;
					const GLfloat& y(GLuint i) const;
					const GLfloat& z(GLuint i) const;
					const GLfloat& nx(GLuint i) const;
					const GLfloat& ny(GLuint i) const;
					const GLfloat& nz(GLuint i) const;
					const GLfloat& u(GLuint i) const;
					const GLfloat& v(GLuint i) const;
					const GLuint& a(GLuint i) const;
					const GLuint& b(GLuint i) const;
					const GLuint& c(GLuint i) const;
					const GLuint& d(GLuint i) const;
					bool testIndices(void) const;
					bool operator==(const GeometryModel& mdl) const;

					HdlVBO* getVBO(GLenum freq) const;

					static int getNumVerticesInPrimitive(const GLenum& _primitiveGL);
					static int getPrimitiveStride(const GLenum& _primitiveGL);
			};

			/**
			\class GeometryInstance
			\brief Instance of the GeometryModel. Stored on GPU (VBO).
			**/
			class GLIP_API GeometryInstance
			{
				private :
					// Static memory bank :
					static int nextIndex;
					static std::map<int, HdlVBO*> 		vbos;
					static std::map<int, GeometryModel*>	models;
					static std::map<int, int>		counters;

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
				\brief Geometry : the standard quad, covering the area between (-1,-1) and (1,1).
				**/
				class GLIP_API StandardQuad : public GeometryModel
				{
					public :
						StandardQuad(void);
						StandardQuad(const StandardQuad& mdl);
				};

				/**
				\class ReversedQuad
				\brief Geometry : the reversed quad, covering the area between (-1,-1) and (1,1).
				**/
				class GLIP_API ReversedQuad : public GeometryModel
				{
					public :
						ReversedQuad(void);
						ReversedQuad(const ReversedQuad& mdl);
				};

				/**
				\class PointsGrid2D
				\brief Geometry : a 2D grid of points.
				**/
				class GLIP_API PointsGrid2D : public GeometryModel
				{
					public :
						const int width,
							  height;
						const bool normalized;

						PointsGrid2D(int w, int h, bool _normalized=false);
						PointsGrid2D(const PointsGrid2D& mdl);
				};

				/**
				\class PointsGrid3D
				\brief Geometry : a 3D grid of points.
				**/
				class GLIP_API PointsGrid3D : public GeometryModel
				{
					public :
						const int width,
							  height,
							  depth;
						const bool normalized;

						PointsGrid3D(int w, int h, int d, bool _normalized=false);
						PointsGrid3D(const PointsGrid3D& mdl);
				};

				/**
				\class CustomModel
				\brief Geometry : build a model.
				**/
				class GLIP_API CustomModel : public GeometryModel
				{
					public :
						CustomModel(int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords);

						void newVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY=NULL, const GLfloat* interleavedUV=NULL);
						void newVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx=NULL, const GLfloat* ny=NULL, const GLfloat* u=NULL, const GLfloat* v=NULL);
						GLuint newVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx=0.0f, const GLfloat& ny=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
						void newVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ=NULL, const GLfloat* interleavedUV=NULL);
						void newVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx=NULL, const GLfloat* ny=NULL, const GLfloat* nz=NULL, const GLfloat* u=NULL, const GLfloat* v=NULL);
						GLuint newVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx=0.0f, const GLfloat& ny=0.0f, const GLfloat& nz=0.0f, const GLfloat& u=0.0f, const GLfloat& v=0.0f);
						void newElementsInterleaved(const size_t N, GLuint* interleavedIndices);
						void newElements(const size_t N, GLuint* a, GLuint* b=NULL, GLuint* c=NULL, GLuint* d=NULL);
						GLuint newElement(GLuint a);
						GLuint newElement(GLuint a, GLuint b);
						GLuint newElement(GLuint a, GLuint b, GLuint c);
						GLuint newElement(GLuint a, GLuint b, GLuint c, GLuint d);
						GLuint newElement(const std::vector<GLuint>& indices);
						void generateNormals(void);
				};
			}
		}
	}

#endif

