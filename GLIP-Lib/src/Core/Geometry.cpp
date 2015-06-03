/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Geometry.cpp                                                                              */
/*     Original Date : June 29th 2013                                                                            */
/*                                                                                                               */
/*     Description   : Geometry objects and memory management                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Geometry.cpp
 * \brief   Geometry objects and memory management
 * \author  R. KERVICHE
 * \date    June 29th 2013
**/

#include <cmath>
#include <algorithm>
#include "Core/Exception.hpp"
#include "Core/HdlVBO.hpp"
#include "Core/Geometry.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// GeometryModel
	/**
	\fn GeometryModel::GeometryModel(GeometryType _type, int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords)
	\brief Geometry model constructor.
	\param _type The type of the geometry (MUST BE GeometryModel::CustomModel for all user defined models).
	\param _dim The minimum number of spatial dimensions needed to describe the geometry (either 2 or 3).
	\param _primitiveGL The GL ID of the element primitive (eg. GL_POINTS, GL_LINES, GL_TRIANGLES, etc.)
	\param _hasNormals Set to true if the geometry has normals data attached.
	\param _hasTexCoords Set to true if the geometry has texel coordinates attached.
	**/
	GeometryModel::GeometryModel(GeometryType _type, int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords)
	 :	vertices(),
		texCoords(),
		elements(),
		type(_type), 
		hasNormals(_hasNormals),		
		hasTexCoords(_hasTexCoords),		
		dim(_dim), 
		numVerticesPerElement(getNumVerticesFromPrimitive(_primitiveGL)), 
		primitiveGL(_primitiveGL)
	{
		if(dim!=2 && dim!=3)
			throw Exception("GeometryModel::GeometryModel - Dimension must be either 2 or 3 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);
	}

	/**
	\fn GeometryModel::GeometryModel(const GeometryModel& mdl)
	\brief GeometryModel copy constructor.
	\param mdl The source model.
	**/
	GeometryModel::GeometryModel(const GeometryModel& mdl)
	 :	vertices(mdl.vertices),
		normals(mdl.normals),
		texCoords(mdl.texCoords),
		elements(mdl.elements),
		type(mdl.type),
		hasNormals(mdl.hasNormals),
		hasTexCoords(mdl.hasTexCoords),
		dim(mdl.dim),
		numVerticesPerElement(mdl.numVerticesPerElement),
		primitiveGL(mdl.primitiveGL)
	{ }

	GeometryModel::~GeometryModel(void)
	{
		vertices.clear();
		texCoords.clear();
		elements.clear();
	}

	/**
	\fn void GeometryModel::reserveVertices(size_t nVertices)
	\brief Prepare the memory to receive a number of vertices.
	\param nVertices The total number of vertices to contain.
	**/
	void GeometryModel::reserveVertices(size_t nVertices)
	{
		vertices.reserve(nVertices*dim);

		if(hasNormals)
			normals.reserve(nVertices*dim);

		if(hasTexCoords)
			texCoords.reserve(2*nVertices);
	}

	/**
	\fn void GeometryModel::increaseVerticesReservation(size_t nNewVertices)
	\brief Prepare the memory to receive a new number of vertices.
	\param nNewVertices The number of vertices to be added.
	**/
	void GeometryModel::increaseVerticesReservation(size_t nNewVertices)
	{
		GeometryModel::reserveVertices(getNumVertices() + nNewVertices);
	}

	/**
	\fn void GeometryModel::reserveElements(size_t nElements)
	\brief Prepare the memory to receive a number of elements.
	\param nElements The total number of elements to contain.
	**/
	void GeometryModel::reserveElements(size_t nElements)
	{
		elements.reserve(nElements*numVerticesPerElement);
	}

	/**
	\fn void GeometryModel::increaseElementsReservation(size_t nNewElements)
	\brief Prepare the memory to receive a new number of elements.
	\param nNewElements The number of elements to be added.
	**/
	void GeometryModel::increaseElementsReservation(size_t nNewElements)
	{
		reserveElements(getNumElements() + nNewElements);
	}

	/**
	\fn void GeometryModel::addVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY, const GLfloat* interleavedUV)
	\brief Add multiple vertives in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param N The number of vertices to be added.
	\param interleavedXY The interleaved array of the spatial coordinates.
	\param interleavedNormalsXY The interleaved array of the  normals data.
	\param interleavedUV The interleaved array of the texture coordinates.

	The vertices coordinates and normals arrays must contain N*dim elements. The texture coordinates array must contain 2*N elements.
	**/
	void GeometryModel::addVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY, const GLfloat* interleavedUV)
	{
		if(dim!=2)
			throw Exception("GeometryModel::addVertices2D - Dimensions should be equal to 2 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.insert(vertices.end(), interleavedXY, interleavedXY+dim*N);
		
		if(hasNormals)
		{
			if(interleavedNormalsXY==NULL)
				throw Exception("GeometryModel::addVertices2D - Missing normals data.", __FILE__, __LINE__, Exception::CoreException);

			normals.insert(normals.end(), interleavedNormalsXY, interleavedNormalsXY+dim*N);
		}

		if(hasTexCoords)
		{
			if(interleavedUV==NULL)
				throw Exception("GeometryModel::addVertices2D - Missing texture coordinates data.", __FILE__, __LINE__, Exception::CoreException);
			
			texCoords.insert(texCoords.end(), interleavedUV, interleavedUV+2*N);
		}
	}

	/**
	\fn void GeometryModel::addVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx, const GLfloat* ny, const GLfloat* u, const GLfloat* v)
	\brief Add multiple vertices in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param N The number of vertices to be added.
	\param x The array of spatial X coordinates.
	\param y The array of spatial Y coordinates.
	\param nx The array of normal components along X.
	\param ny The array of normal components along Y.
	\param u The array of texture U coordinates.
	\param v The array of texture V coordinates.

	All arrays must contain N indices.
	**/
	void GeometryModel::addVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx, const GLfloat* ny, const GLfloat* u, const GLfloat* v)
	{
		if(dim!=2)
			throw Exception("GeometryModel::addVertices2D - Dimensions should be equal to 2 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.reserve(vertices.size() + dim*N);
		for(size_t k=0; k<N; k++)
		{
			vertices.push_back(x[k]);
			vertices.push_back(y[k]);
		}

		if(hasNormals)
		{
			if(nx==NULL || ny==NULL)
				throw Exception("GeometryModel::addVertices2D - Missing normals data.", __FILE__, __LINE__, Exception::CoreException);

			normals.reserve(normals.size() + dim*N);
			for(size_t k=0; k<N; k++)
			{
				normals.push_back(nx[k]);
				normals.push_back(ny[k]);
			}
		}
	
		if(hasTexCoords)
		{
			if(u==NULL || v==NULL)
				throw Exception("GeometryModel::addVertices2D - Missing texture coordinates data.", __FILE__, __LINE__, Exception::CoreException);

			texCoords.reserve(texCoords.size() + 2*N);
			for(size_t k=0; k<N; k++)
			{
				texCoords.push_back(u[k]);
				texCoords.push_back(v[k]);
			}
		}
	}

	/**
	\fn GLuint GeometryModel::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx, const GLfloat& ny, const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param nx The X component of the normal.
	\param ny The Y component of the normal.
	\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
	\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
	\return The index of the newly created vertex.
	**/
	GLuint GeometryModel::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx, const GLfloat& ny, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=2)
			throw Exception("GeometryModel::addVertex2D - Dimensions should be equal to 2 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.push_back(x);
		vertices.push_back(y);

		if(hasNormals)
		{
			normals.push_back(nx);
			normals.push_back(ny);
		}

		if(hasTexCoords)
		{
			texCoords.push_back(u);
			texCoords.push_back(v);
		}

		return getNumVertices() - 1;
	}

	/**
	\fn void GeometryModel::addVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ, const GLfloat* interleavedUV)
	\brief Add multiple vertices in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param N The number of vertices to be added.
	\param interleavedXYZ The interleaved array of the spatial coordinates.
	\param interleavedNormalsXYZ The interleaved array of the  normals data.
	\param interleavedUV The interleaved array of the texture coordinates.
	
	The vertices coordinates and normals arrays must contain N*dim elements. The texture coordinates array must contain 2*N elements.
	**/
	void GeometryModel::addVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ, const GLfloat* interleavedUV)
	{
		if(dim!=2)
			throw Exception("GeometryModel::addVertices3D - Dimensions should be equal to 2 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.insert(vertices.end(), interleavedXYZ, interleavedXYZ+dim*N);
		
		if(hasNormals)
		{
			if(interleavedNormalsXYZ==NULL)
				throw Exception("GeometryModel::addVertices3D - Missing normals data.", __FILE__, __LINE__, Exception::CoreException);

			normals.insert(normals.end(), interleavedNormalsXYZ, interleavedNormalsXYZ+dim*N);
		}

		if(hasTexCoords)
		{
			if(interleavedUV==NULL)
				throw Exception("GeometryModel::addVertices3D - Missing texture coordinates data.", __FILE__, __LINE__, Exception::CoreException);
			
			texCoords.insert(texCoords.end(), interleavedUV, interleavedUV+2*N);
		}
	}

	/**
	\fn void GeometryModel::addVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx, const GLfloat* ny, const GLfloat* nz, const GLfloat* u, const GLfloat* v)
	\brief Add multiple vertices in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param N The number of vertices to be added.
	\param x The array of spatial X coordinates.
	\param y The array of spatial Y coordinates.
	\param z The array of spatial Z coordinates.
	\param nx The array of normal components along X.
	\param ny The array of normal components along Y.
	\param nz The array of normal components along Z.
	\param u The array of texture U coordinates.
	\param v The array of texture V coordinates.

	All arrays must contain N indices.
	**/
	void GeometryModel::addVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx, const GLfloat* ny, const GLfloat* nz, const GLfloat* u, const GLfloat* v)
	{
		if(dim!=3)
			throw Exception("GeometryModel::addVertices3D - Dimensions should be equal to 3 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.reserve(vertices.size() + dim*N);
		for(size_t k=0; k<N; k++)
		{
			vertices.push_back(x[k]);
			vertices.push_back(y[k]);
			vertices.push_back(z[k]);
		}

		if(hasNormals)
		{
			if(nx==NULL || ny==NULL || nz==NULL)
				throw Exception("GeometryModel::addVertices3D - Missing normals data.", __FILE__, __LINE__, Exception::CoreException);

			normals.reserve(normals.size() + dim*N);
			for(size_t k=0; k<N; k++)
			{
				normals.push_back(nx[k]);
				normals.push_back(ny[k]);
				normals.push_back(nz[k]);
			}
		}

		if(hasTexCoords)
		{
			if(u==NULL || v==NULL)
				throw Exception("GeometryModel::addVertices3D - Missing texture coordinates data.", __FILE__, __LINE__, Exception::CoreException);

			texCoords.reserve(texCoords.size() + 2*N);
			for(size_t k=0; k<N; k++)
			{
				texCoords.push_back(u[k]);
				texCoords.push_back(v[k]);
			}
		}
	}

	/**
	\fn GLuint GeometryModel::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx, const GLfloat& ny, const GLfloat& nz,const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param z The Z coordinate.
	\param nx The X component of the normal.
	\param ny The Y component of the normal.
	\param nz The Z component of the normal.
	\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
	\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
	\return The index of the newly created vertex.
	**/
	GLuint GeometryModel::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx, const GLfloat& ny, const GLfloat& nz, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=3)
			throw Exception("GeometryModel::addVertex3D - Dimensions should be equal to 3 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		if(hasNormals)
		{
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);
		}

		if(hasTexCoords)
		{
			texCoords.push_back(u);
			texCoords.push_back(v);
		}

		return getNumVertices() - 1;
	}

	/**
	\fn void GeometryModel::addElementsInterleaved(const size_t N, GLuint* interleavedIndices)
	\brief Add primitive elements to the model.
	\param N Number of primitive elements.
	\param interleavedIndices Array of vertices indices. Must contain N*numVerticesPerElement indices.
	**/
	void GeometryModel::addElementsInterleaved(const size_t N, GLuint* interleavedIndices)
	{
		elements.insert(elements.end(), interleavedIndices, interleavedIndices+N*numVerticesPerElement);
	}

	/**
	\fn void GeometryModel::addElements(const size_t N, GLuint* a, GLuint* b=NULL, GLuint* c=NULL, GLuint* d=NULL)
	\brief Add primitive elements to the model.
	\param N Number of primitive elements.
	\param a First vertices indices.
	\param b Second vertices indices.
	\param c Third vertices indices.
	\param d Fourth vertices indices.

	All arrays must contain N indices.
	**/
	void GeometryModel::addElements(const size_t N, GLuint* a, GLuint* b, GLuint* c, GLuint* d)
	{
		if((numVerticesPerElement>=1 && a==NULL) || (numVerticesPerElement>=2 && b==NULL) || (numVerticesPerElement>=3 && c==NULL) || (numVerticesPerElement>=4 && d==NULL) || numVerticesPerElement>4)
			throw Exception("GeometryModel::addElements - Missing indices.", __FILE__, __LINE__, Exception::CoreException);

		elements.reserve(elements.size() + N*numVerticesPerElement);
		for(size_t k=0; k<N; k++)
		{
							elements.push_back(a[k]);
			if(numVerticesPerElement>=2)	elements.push_back(b[k]);
			if(numVerticesPerElement>=3)	elements.push_back(c[k]);
			if(numVerticesPerElement>=4)	elements.push_back(d[k]);
		}
	}

	/**
	\fn GLuint GeometryModel::addElement(GLuint a)
	\brief Add a primitive element to the model.
	\param a Index of the first vertex.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(GLuint a)
	{
		const int expectedNumberOfVertices = 1;

		if(numVerticesPerElement!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);

		elements.push_back(a);
		return getNumElements() - 1;
	}

	/**
	\fn GLuint GeometryModel::addElement(GLuint a, GLuint b)
	\brief Add a primitive element to the model.
	\param a Index of the first vertex.
	\param b Index of the second vertex.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(GLuint a, GLuint b)
	{
		const int expectedNumberOfVertices = 2;

		if(numVerticesPerElement!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);

		elements.push_back(a);
		elements.push_back(b);
		return getNumElements() - 1;
	}

	/**
	\fn GLuint GeometryModel::addElement(GLuint a, GLuint b, GLuint c)
	\brief Add a primitive element to the model.
	\param a Index of the first vertex.
	\param b Index of the second vertex.
	\param c Index of the third vertex.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(GLuint a, GLuint b, GLuint c)
	{
		const int expectedNumberOfVertices = 3;

		if(numVerticesPerElement!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);

		elements.push_back(a);
		elements.push_back(b);
		elements.push_back(c);
		return getNumElements() - 1;
	}

	/**
	\fn GLuint GeometryModel::addElement(GLuint a, GLuint b, GLuint c, GLuint d)
	\brief Add a primitive element to the model.
	\param a Index of the first vertex.
	\param b Index of the second vertex.
	\param c Index of the third vertex.
	\param d Index of the fourth vertex.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(GLuint a, GLuint b, GLuint c, GLuint d)
	{
		const int expectedNumberOfVertices = 4;

		if(numVerticesPerElement!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);

		elements.push_back(a);
		elements.push_back(b);
		elements.push_back(c);
		elements.push_back(d);
		return getNumElements() - 1;
	}

	/**
	\fn GLuint GeometryModel::addElement(const std::vector<GLuint>& indices)
	\brief Add a primitive element to the model.
	\param indices A vector containing the indices of all the vertices used.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(const std::vector<GLuint>& indices)
	{
		if(numVerticesPerElement!=static_cast<int>(indices.size()))
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(indices.size()) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);

		elements.insert(elements.end(), indices.begin(), indices.end());
		return getNumElements() - 1;
	}

	/**
	\fn void GeometryModel::generateNormals(void)
	\brief Automatically generate the normals.

	If the primitive is GL_POINTS, this will reset the normals to 0.
	**/
	void GeometryModel::generateNormals(void)
	{
		const GLuint nElements = getNumElements();
		normals.clear();
		normals.assign(vertices.size(), 0.0f);

		// No normals defined for dots : 
		if(numVerticesPerElement<=1)
			return ;

		std::vector<unsigned int> counts(getNumVertices(), 0);

		for(GLuint p=0; p<nElements; p++)
		{
			const unsigned int offset = p*numVerticesPerElement;
			const unsigned int nNoLoop = (numVerticesPerElement>=3) ? numVerticesPerElement : 1;

			if(dim==2)
			{
				for(unsigned int q=0; q<nNoLoop; q++)
				{
					const GLuint 	a = elements[offset+q],
							b = elements[offset+((q+1)%numVerticesPerElement)];

					if(a<vertices.size() && b<vertices.size())
					{
						// Compute the normal : 
						const float	dx = vertices[b*dim+0] - vertices[a*dim+0],
								dy = vertices[b*dim+1] - vertices[a*dim+1],
								l = std::sqrt(dx*dx+dy*dy),
								nx = -dy/l,
								ny = dx/l;
						normals[a*dim+0] += nx;
						normals[a*dim+1] += ny;
						counts[a]++;
						normals[b*dim+0] += nx;
						normals[b*dim+1] += ny;
						counts[b]++;	
					}
				}
			}
			else if(dim==3)
			{
				for(unsigned int q=0; q<nNoLoop; q++)
				{
					const GLuint 	a = elements[offset+q],
							b = elements[offset+((q+1)%numVerticesPerElement)],
							c = elements[offset+((q+2)%numVerticesPerElement)];

					if(a<vertices.size() && b<vertices.size() && c<vertices.size())
					{
						// Compute the normal : 
						const float	dx1 = vertices[b*dim+0] - vertices[a*dim+0],
								dy1 = vertices[b*dim+1] - vertices[a*dim+1],
								dz1 = vertices[b*dim+2] - vertices[a*dim+2],
								dx2 = vertices[c*dim+0] - vertices[b*dim+0],
								dy2 = vertices[c*dim+1] - vertices[b*dim+1],
								dz2 = vertices[c*dim+2] - vertices[b*dim+2],
								l1 = std::sqrt(dx1*dx1+dy1*dy1),
								l2 = std::sqrt(dx2*dx2+dy2*dy2),
								nx = (dy1*dz2 - dy2*dz1)/(l1*l2),
								ny = (dx2*dz1 - dx1*dz2)/(l1*l2),
								nz = (dx1*dy2 - dx2*dy1)/(l1*l2);
						normals[a*dim+0] += nx;
						normals[a*dim+1] += ny;
						normals[a*dim+2] += nz;
						counts[a]++;
						normals[b*dim+0] += nx;
						normals[b*dim+1] += ny;
						normals[b*dim+2] += nz;
						counts[b]++;	
						normals[c*dim+0] += nx;
						normals[c*dim+1] += ny;
						normals[c*dim+2] += nz;
						counts[c]++;
					}
				}
			}
			else
				throw Exception("Cannot compute normal in " + toString(dim) + " dimensions.", __FILE__, __LINE__, Exception::CoreException);
		}

		// Final averages : 
		for(unsigned int k=0; k<counts.size(); k++)
		{
			for(int q=0; q<dim; q++)
			{
				if(counts[k]==0)
					normals[k*dim+q] = 0.0f;
				else
					normals[k*dim+q] /= static_cast<float>(counts[k]);
			}
		}
	}

	/**
	\fn GLfloat& GeometryModel::x(GLuint i)
	\brief Access the X coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryModel::x(GLuint i)
	{
		return vertices[i*dim+0];
	}

	/**
	\fn GLfloat& GeometryModel::y(GLuint i)
	\brief Access the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryModel::y(GLuint i)
	{
		return vertices[i*dim+1];
	}

	/**
	\fn GLfloat& GeometryModel::z(GLuint i)
	\brief Access the Z coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::dim is smaller than 3.
	**/
	GLfloat& GeometryModel::z(GLuint i)
	{
		if(dim<3)
			throw Exception("GeometryModel::z - This geometry has only " + toString(dim) + " dimensions.", __FILE__, __LINE__, Exception::CoreException);

		return vertices[i*dim+2];
	}

	/**
	\fn GLfloat& GeometryModel::nx(GLuint i)
	\brief Access the X coordinate of the normal at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryModel::nx(GLuint i)
	{
		return normals[i*dim+0];
	}

	/**
	\fn GLfloat& GeometryModel::ny(GLuint i)
	\brief Access the Y coordinate of the normal at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryModel::ny(GLuint i)
	{
		return normals[i*dim+1];
	}

	/**
	\fn GLfloat& GeometryModel::z(GLuint i)
	\brief Access the Z coordinate of the normal at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::dim is smaller than 3.
	**/
	GLfloat& GeometryModel::nz(GLuint i)
	{
		if(dim<3)
			throw Exception("GeometryModel::z - This geometry has only " + toString(dim) + " dimensions.", __FILE__, __LINE__, Exception::CoreException);

		return normals[i*dim+2];
	}

	/**
	\fn GLfloat& GeometryModel::u(GLuint i)
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoords is set to false.
	**/
	GLfloat& GeometryModel::u(GLuint i)
	{
		if(!hasTexCoords)
			throw Exception("GeometryModel::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return texCoords[i*2+0];
	}

	/**
	\fn GLfloat& GeometryModel::v(GLuint i)
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoords is set to false.
	**/
	GLfloat& GeometryModel::v(GLuint i)
	{
		if(!hasTexCoords)
			throw Exception("GeometryModel::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return texCoords[i*2+1];
	}

	/**
	\fn GLuint& GeometryModel::a(GLuint i)
	\brief Access the index of the first vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the first vertex of the element.
	**/
	GLuint& GeometryModel::a(GLuint i)
	{
		const int expectedNumberOfVertices = 1;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 0];
	}

	/**
	\fn GLuint& GeometryModel::b(GLuint i)
	\brief Access the index of the second vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the second vertex of the element.
	**/
	GLuint& GeometryModel::b(GLuint i)
	{
		const int expectedNumberOfVertices = 2;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 1];
	}

	/**
	\fn GLuint& GeometryModel::c(GLuint i)
	\brief Access the index of the third vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the third vertex of the element.
	**/
	GLuint& GeometryModel::c(GLuint i)
	{
		const int expectedNumberOfVertices = 3;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 2];
	}

	/**
	\fn GLuint& GeometryModel::a(GLuint i)
	\brief Access the index of the fourth vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the fourth vertex of the element.
	**/
	GLuint& GeometryModel::d(GLuint i)
	{
		const int expectedNumberOfVertices = 4;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 3];
	}
	
	/**
	\fn const GLfloat& GeometryModel::x(GLuint i) const
	\brief Read the X coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::x(GLuint i) const
	{
		return vertices[i*dim+0];
	}

	/**
	\fn const GLfloat& GeometryModel::y(GLuint i) const
	\brief Read the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::y(GLuint i) const
	{
		return vertices[i*dim+1];
	}

	/**
	\fn const GLfloat& GeometryModel::z(GLuint i) const
	\brief Read the Z coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable. Will raise an exception if GeometryModel::dim is smaller than 3.
	**/
	const GLfloat& GeometryModel::z(GLuint i) const
	{
		if(dim<3)
			throw Exception("GeometryModel::z - This geometry has only " + toString(dim) + " dimensions.", __FILE__, __LINE__, Exception::CoreException);

		return vertices[i*dim+2];
	}

	/**
	\fn const GLfloat& GeometryModel::nx(GLuint i)
	\brief Access the X coordinate of the normal at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::nx(GLuint i) const 
	{
		return normals[i*dim+0];
	}

	/**
	\fn const GLfloat& GeometryModel::ny(GLuint i) const 
	\brief Access the Y coordinate of the normal at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::ny(GLuint i) const 
	{
		return normals[i*dim+1];
	}

	/**
	\fn const GLfloat& GeometryModel::z(GLuint i) const 
	\brief Access the Z coordinate of the normal at given index.
	\param i The vertex index.
	\return Read access to corresponding variable. Will raise an exception if GeometryModel::dim is smaller than 3.
	**/
	const GLfloat& GeometryModel::nz(GLuint i) const 
	{
		if(dim<3)
			throw Exception("GeometryModel::z - This geometry has only " + toString(dim) + " dimensions.", __FILE__, __LINE__, Exception::CoreException);

		return normals[i*dim+2];
	}

	/**
	\fn const GLfloat& GeometryModel::u(GLuint i) const
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoords is set to false.
	**/
	const GLfloat& GeometryModel::u(GLuint i) const
	{
		if(!hasTexCoords)
			throw Exception("GeometryModel::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return texCoords[i*2+0];
	}

	/**
	\fn GLfloat& GeometryModel::v(GLuint i) const
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoords is set to false.
	**/
	const GLfloat& GeometryModel::v(GLuint i) const
	{
		if(!hasTexCoords)
			throw Exception("GeometryModel::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return texCoords[i*2+1];
	}

	/**
	\fn const GLuint& GeometryModel::a(GLuint i) const
	\brief Access the index of the first vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the first vertex of the element.
	**/
	const GLuint& GeometryModel::a(GLuint i) const
	{
		const int expectedNumberOfVertices = 1;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 0];
	}

	/**
	\fn const GLuint& GeometryModel::b(GLuint i) const
	\brief Access the index of the second vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the second vertex of the element.
	**/
	const GLuint& GeometryModel::b(GLuint i) const
	{
		const int expectedNumberOfVertices = 2;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 1];
	}

	/**
	\fn const GLuint& GeometryModel::c(GLuint i) const
	\brief Access the index of the third vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the third vertex of the element.
	**/
	const GLuint& GeometryModel::c(GLuint i) const
	{
		const int expectedNumberOfVertices = 3;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 2];
	}

	/**
	\fn const GLuint& GeometryModel::a(GLuint i) const
	\brief Access the index of the fourth vertex of the element at given index.
	\param i The element index.
	\return Read/write access to the index of the fourth vertex of the element.
	**/
	const GLuint& GeometryModel::d(GLuint i) const
	{
		const int expectedNumberOfVertices = 4;

		if(numVerticesPerElement<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerElement) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerElement + 3];
	}

	/**
	\fn GLuint GeometryModel::getNumVertices(void) const
	\brief Get the number of vertices.
	\return The current number of vertices registered for this model.
	**/
	GLuint GeometryModel::getNumVertices(void) const
	{
		return vertices.size() / dim;
	}

	/**
	\fn GLuint GeometryModel::getNumElements(void) const
	\brief Get the number of GL primitives (elements).
	\return The current number of GL primitives (element).
	**/
	GLuint GeometryModel::getNumElements(void) const
	{
		return elements.size() / numVerticesPerElement;
	}

	/**
	\fn bool GeometryModel::testIndices(void) const
	\brief Test if all the indices are valid.
	\return True if all the indices are valid.
	**/
	bool GeometryModel::testIndices(void) const
	{
		if(hasNormals && vertices.size()!=normals.size())
			return false;

		if(hasTexCoords && getNumVertices()*2!=texCoords.size())
			return false;

		for(std::vector<GLuint>::const_iterator it=elements.begin(); it!=elements.end(); it++)
		{
			if((*it)>=vertices.size())
				return false;
		}
		return true;	
	}

	/**
	\fn bool GeometryModel::operator==(const GeometryModel& mdl) const
	\brief Test if two models are identical.
	\param mdl The reference model.
	\return true if the two models are identical.
	**/
	bool GeometryModel::operator==(const GeometryModel& mdl) const
	{
		return 		(hasTexCoords==mdl.hasTexCoords)
			&&	(dim==mdl.dim)
			&&	(numVerticesPerElement==mdl.numVerticesPerElement)
			&&	(primitiveGL==mdl.primitiveGL)
			&& 	(std::equal(vertices.begin(), vertices.end(), mdl.vertices.begin()))
			&& 	(std::equal(normals.begin(), normals.end(), mdl.normals.begin()))
			&&	(std::equal(texCoords.begin(), texCoords.end(), mdl.texCoords.begin()))
			&&	(std::equal(elements.begin(), elements.end(), mdl.elements.begin()));
	}

	/**
	\fn HdlVBO* GeometryModel::getVBO(GLenum freq) const
	\brief Get the VBO corresponding to this model. It is not recommended to use directly the VBO but, instead, use a GeometryInstance object.
	\param freq The GL frequency of read/write operations (among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	\return A pointer to the newly created VBO. It is of the responsability of the user to call delete on this object.
	**/
	HdlVBO* GeometryModel::getVBO(GLenum freq) const
	{
		if(vertices.empty())
			throw Exception("GeometryModel::getVBO - Empty vertices list.", __FILE__, __LINE__, Exception::CoreException);

		GLuint 		localNumElements = 0,
				localNumVerticesPerEl = 0,
				localNumDimTexCoord = 0;

		const GLuint 	*elementsPtr = NULL;
		const GLfloat	*normalsPtr = NULL,
				*texPtr = NULL;

		if(!elements.empty())
		{
			localNumElements = getNumElements();
			localNumVerticesPerEl = numVerticesPerElement;
			elementsPtr = &elements.front();
		}

		if(!normals.empty())
			normalsPtr = &normals.front();

		if(!texCoords.empty())
		{
			localNumDimTexCoord = 2;
			texPtr = &texCoords.front();
		}

		return new HdlVBO(getNumVertices(), dim, freq, &vertices.front(), localNumElements, localNumVerticesPerEl, elementsPtr, primitiveGL, normalsPtr, localNumDimTexCoord, texPtr);
	}
	
	/**
	\fn int GeometryModel::getNumVerticesFromPrimitive(const GLenum& _primitiveGL)
	\brief Get the number of vertices per element for the given primitive.
	\param _primitiveGL The GL primitive (accepted are :  GL_POINTS​, GL_LINES​, GL_LINE_LOOP, GL_LINE_STRIP, GL_TRIANGLES​, GL_QUADS).
	\return The number of elements per primitive.
	**/
	int GeometryModel::getNumVerticesFromPrimitive(const GLenum& _primitiveGL)
	{
		switch(_primitiveGL)
		{
			case GL_POINTS :			return 1;
			case GL_LINES :				return 2;
			case GL_LINE_LOOP :			return 1;
			case GL_LINE_STRIP : 			return 1;
			case GL_TRIANGLES : 			return 3;
			case GL_QUADS : 			return 4;
			case GL_TRIANGLE_STRIP : 
			case GL_TRIANGLE_FAN : 
			case GL_LINES_ADJACENCY : 
			case GL_LINE_STRIP_ADJACENCY : 
			case GL_TRIANGLES_ADJACENCY : 
			case GL_TRIANGLE_STRIP_ADJACENCY : 
			case GL_QUAD_STRIP :
			case GL_POLYGON :
				throw Exception("GeometryModel::getNumVerticesFromPrimitive - Unsupported primitive type : \"" + getGLEnumNameSafe(_primitiveGL) + "\".", __FILE__, __LINE__, Exception::CoreException);
			default : 
				throw Exception("GeometryModel::getNumVerticesFromPrimitive - Unknown primitive type : \"" + getGLEnumNameSafe(_primitiveGL) + "\".", __FILE__, __LINE__, Exception::CoreException);
		}
	}

// GeometryInstance
	int GeometryInstance::nextIndex = 0;
	std::map<int, HdlVBO*> 		GeometryInstance::vbos;
	std::map<int, GeometryModel*>	GeometryInstance::models;
	std::map<int, int>		GeometryInstance::counters;

	/**
	\fn GeometryInstance::GeometryInstance(const GeometryModel& mdl, GLenum freq)
	\brief GeometryInstance constructor.
	\param mdl The original model to use.
	\param freq The GL frequency of read/write operations (among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	**/
 	GeometryInstance::GeometryInstance(const GeometryModel& mdl, GLenum freq)
	 : id(-1)
	{
		// Find if a similar model exist : 
		for(std::map<int, GeometryModel*>::iterator it=models.begin(); it!=models.end(); it++)
		{
			if(it->second!=NULL && (*it->second)==mdl)
			{
				// Found!
				id = it->first;
				counters[id]++;
				break;
			}
		}

		if(id<0)
		{
			// No item found, create : 
			id = nextIndex;
			nextIndex++;

			vbos[id] 	= mdl.getVBO(freq);
			models[id] 	= new GeometryModel(mdl);
			counters[id] 	= 1;
		}
	}

	/**
	\fn GeometryInstance::GeometryInstance(const GeometryInstance& instance)
	\brief GeometryInstance copy constructor.
	\param instance The source model.
	**/
	GeometryInstance::GeometryInstance(const GeometryInstance& instance)
	 : id(instance.id)
	{
		counters[id]++;
	}

	GeometryInstance::~GeometryInstance(void)
	{
		counters[id]--;

		// If there is no reference anymore :
		if(counters[id]<=0)
		{
			delete vbos[id];
			delete models[id];

			vbos.erase(id);
			models.erase(id);
			counters.erase(id);
		}
	}

	/**
	\fn const GeometryModel& GeometryInstance::model(void) const
	\brief Access to the model of this instance.
	\return A read-only access to the GeometryModel object.
	**/
	const GeometryModel& GeometryInstance::model(void) const
	{
		return (*models[id]);
	}

	/**
	\fn const HdlVBO& GeometryInstance::vbo(void) const
	\brief Access to the VBO of this instance.
	\return A read-only access to the HdlVBO object.
	**/
	const HdlVBO& GeometryInstance::vbo(void) const
	{
		return (*vbos[id]);
	}

	/**
	\fn void GeometryInstance::draw(void)
	\brief Draw this instance.
	**/
	void GeometryInstance::draw(void)
	{
		vbos[id]->draw();
	}

// Geometries :
	using namespace GeometryPrimitives;

	// Standard quad :
		/**
		\fn StandardQuad::StandardQuad(void)
		\brief StandardQuad constructor.
		**/
		StandardQuad::StandardQuad(void)
		 : GeometryModel(GeometryModel::StandardQuad, 2, GL_TRIANGLES, false, true)
		{
			// Create the geometry :
			reserveVertices(3);
			addVertex2D(-1.0, -1.0, 0.0, 0.0, 0.0, 0.0);
			addVertex2D(-1.0,  1.0, 0.0, 0.0, 0.0, 1.0);
			addVertex2D( 1.0,  1.0, 0.0, 0.0, 1.0, 1.0);
			addVertex2D( 1.0, -1.0, 0.0, 0.0, 1.0, 0.0);

			reserveElements(2);
			addElement(0, 1, 3);
			addElement(1, 2, 3);
		}

		/**
		\fn StandardQuad::StandardQuad(const StandardQuad& mdl)
		\brief Copy constructor.
		\param mdl Original model.
		**/
		StandardQuad::StandardQuad(const StandardQuad& mdl)
		 : GeometryModel(mdl)
		{ }

	// Reversed quad :
		/**
		\fn ReversedQuad::ReversedQuad(void)
		\brief ReversedQuad constructor.
		**/
		ReversedQuad::ReversedQuad(void)
		 : GeometryModel(GeometryModel::ReversedQuad, 2, GL_TRIANGLES, false, true)
		{
			// Create the geometry :
			reserveVertices(3);			
			addVertex2D(-1.0,  1.0, 0.0, 0.0, 0.0, 0.0);
			addVertex2D( 1.0,  1.0, 0.0, 0.0, 1.0, 0.0);
			addVertex2D( 1.0, -1.0, 0.0, 0.0, 1.0, 1.0);
			addVertex2D(-1.0, -1.0, 0.0, 0.0, 0.0, 1.0);

			reserveElements(2);
			addElement(0, 1, 3);
			addElement(1, 2, 3);
		}
	
		/**
		\fn ReversedQuad::ReversedQuad(const ReversedQuad& mdl)
		\brief Copy constructor.
		\param mdl Original model.
		**/
		ReversedQuad::ReversedQuad(const ReversedQuad& mdl)
		 : GeometryModel(mdl)
		{ }

	// 2D Grid of points
		/**
		\fn PointsGrid2D::PointsGrid2D(int w, int h, bool _normalized)
		\brief PointsGrid2D constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		\param _normalized If true, the coordinates will be normalized in the range [0, 1].
		**/
		PointsGrid2D::PointsGrid2D(int w, int h, bool _normalized)
		 :	GeometryModel(GeometryModel::PointsGrid2D, 2, GL_POINTS, false, false),
			width(w),
			height(h),
			normalized(_normalized)
		{
			reserveVertices(w*h);
			for(int i=0; i<h; i++)
			{
				for(int j=0; j<w; j++)
				{
					if(!_normalized)
						addVertex2D(j,  i);
					else
					{
						const float 	x = static_cast<float>(j)/static_cast<float>(w-1),
								y = static_cast<float>(i)/static_cast<float>(h-1);

						addVertex2D(x,  y);
					}
				}
			}
		}

		/**
		\fn PointsGrid2D::PointsGrid2D(const PointsGrid2D& mdl)
		\brief Copy constructor.
		\param mdl Original model.
		**/
		PointsGrid2D::PointsGrid2D(const PointsGrid2D& mdl)
		 : 	GeometryModel(mdl),
			width(mdl.width),
			height(mdl.height),
			normalized(mdl.normalized)
		{ }

	// 3D Grid of points
		/**
		\fn PointsGrid3D::PointsGrid3D(int w, int h, int d, bool _normalized)
		\brief PointsGrid3D constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		\param d Number of points along the Z dimension.
		\param _normalized If true, the coordinates will be normalized in the range [0, 1].
		**/
		PointsGrid3D::PointsGrid3D(int w, int h, int d, bool _normalized)
		 : 	GeometryModel(GeometryModel::PointsGrid3D, 3, GL_POINTS, false, false),
			width(w),
			height(h),
			depth(d),
			normalized(_normalized)
		{
			reserveVertices(w*h*d);
			for(int k=0; k<d; k++)
			{
				for(int i=0; i<h; i++)
				{
					for(int j=0; j<w; j++)
					{
						if(!_normalized)
							addVertex3D(j, i, k);
						else
						{
							const float 	x = static_cast<float>(j)/static_cast<float>(w-1),
									y = static_cast<float>(i)/static_cast<float>(h-1),
									z = static_cast<float>(k)/static_cast<float>(d-1);

							addVertex3D(x,  y, z);
						}
					}
				}
			}
		}

		/**
		\fn PointsGrid3D::PointsGrid3D(const PointsGrid3D& mdl)
		\brief Copy constructor.
		\param mdl Original model.
		**/
		PointsGrid3D::PointsGrid3D(const PointsGrid3D& mdl)
		 : 	GeometryModel(mdl),
			width(mdl.width),
			height(mdl.height),
			depth(mdl.depth),
			normalized(mdl.normalized)
		{ }

	// Custom model : 
		/**
		\fn CustomModel::CustomModel(int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords)
		\brief CustomModel constructor.
		\param _dim The minimum number of spatial dimensions needed to describe the geometry (either 2 or 3).
		\param _primitiveGL The GL ID of the element primitive (eg. GL_POINTS, GL_LINES, GL_TRIANGLES, etc.)
		\param _hasNormals Set to true if the geometry has normals data attached.
		\param _hasTexCoords Set to true if the geometry has texel coordinates attached.
		**/
		CustomModel::CustomModel(int _dim, GLenum _primitiveGL, bool _hasNormals, bool _hasTexCoords)
		 : GeometryModel( GeometryModel::CustomModel, _dim, _primitiveGL, _hasNormals, _hasTexCoords)
		{ }

		/**
		\fn void CustomModel::newVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY, const GLfloat* interleavedUV)
		\brief Add multiple vertives in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param N The number of vertices to be added.
		\param interleavedXY The interleaved array of the spatial coordinates.
		\param interleavedNormalsXY The interleaved array of the  normals data.
		\param interleavedUV The interleaved array of the texture coordinates.

		The vertices coordinates array must contain N*dim elements. The texture coordinates array must contain 2*N elements.
		**/
		void CustomModel::newVertices2DInterleaved(const size_t N, const GLfloat* interleavedXY, const GLfloat* interleavedNormalsXY, const GLfloat* interleavedUV)
		{
			addVertices2DInterleaved(N, interleavedXY, interleavedNormalsXY, interleavedUV);
		}

		/**
		\fn void CustomModel::newVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx, const GLfloat* ny, const GLfloat* u, const GLfloat* v)
		\brief Add multiple vertices in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param N The number of vertices to be added.
		\param x The array of spatial X coordinates.
		\param y The array of spatial Y coordinates.
		\param nx The array of normal components along X.
		\param ny The array of normal components along Y.
		\param u The array of texture U coordinates.
		\param v The array of texture V coordinates.

		All arrays must contain N indices.
		**/
		void CustomModel::newVertices2D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* nx, const GLfloat* ny, const GLfloat* u, const GLfloat* v)
		{
			addVertices2D(N, x, y, nx, ny, u, v);
		}

		/**
		\fn GLuint CustomModel::newVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx, const GLfloat& ny, const GLfloat& u, const GLfloat& v)
		\brief Add a vertex in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param x The X coordinate.
		\param y The Y coordinate.
		\param nx The X component of the normal.
		\param ny The Y component of the normal.
		\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
		\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
		\return The index of the newly created vertex.
		**/
		GLuint CustomModel::newVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& nx, const GLfloat& ny, const GLfloat& u, const GLfloat& v)
		{
			return addVertex2D(x, y, nx, ny, u, v);
		}

		/**
		\fn void CustomModel::newVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ, const GLfloat* interleavedUV)
		\brief Add multiple vertices in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param N The number of vertices to be added.
		\param interleavedXYZ The interleaved array of the spatial coordinates.
		\param interleavedNormalsXYZ The interleaved array of the  normals data.
		\param interleavedUV The interleaved array of the texture coordinates.

		The vertices coordinates array must contain N*dim elements. The texture coordinates array must contain 2*N elements.
		**/
		void CustomModel::newVertices3DInterleaved(const size_t N, const GLfloat* interleavedXYZ, const GLfloat* interleavedNormalsXYZ, const GLfloat* interleavedUV)
		{
			addVertices3DInterleaved(N, interleavedXYZ, interleavedNormalsXYZ, interleavedUV);
		}
		
		/**
		\fn void CustomModel::newVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx, const GLfloat* ny, const GLfloat* nz, const GLfloat* u, const GLfloat* v)
		\brief Add multiple vertices in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param N The number of vertices to be added.
		\param x The array of spatial X coordinates.
		\param y The array of spatial Y coordinates.
		\param z The array of spatial Z coordinates.
		\param nx The array of normal components along X.
		\param ny The array of normal components along Y.
		\param nz The array of normal components along Z.
		\param u The array of texture U coordinates.
		\param v The array of texture V coordinates.

		All arrays must contain N indices.
		**/
		void CustomModel::newVertices3D(const size_t N, const GLfloat* x, const GLfloat* y, const GLfloat* z, const GLfloat* nx, const GLfloat* ny, const GLfloat* nz, const GLfloat* u, const GLfloat* v)
		{
			addVertices3D(N, x, y, z, nx, ny, nz, u, v);
		}

		/**
		\fn GLuint CustomModel::newVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx, const GLfloat& ny, const GLfloat& nz, const GLfloat& u, const GLfloat& v)
		\brief Add a vertex in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoords is true, then it will also use u and v to create a texel coordinate.
		\param x The X coordinate.
		\param y The Y coordinate.
		\param z The Z coordinate.
		\param nx The X component of the normal.
		\param ny The Y component of the normal.
		\param nz The Z component of the normal.
		\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
		\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoords is set to false).
		\return The index of the newly created vertex.
		**/
		GLuint CustomModel::newVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& nx, const GLfloat& ny, const GLfloat& nz, const GLfloat& u, const GLfloat& v)
		{
			return addVertex3D(x, y, z, nx, ny, nz, u, v);
		}

		/**
		\fn void CustomModel::newElementsInterleaved(const size_t N, GLuint* interleavedIndices)
		\brief Add primitive elements to the model.
		\param N Number of primitive elements.
		\param interleavedIndices Array of vertices indices. Must contain N*numVerticesPerElement indices.
		**/
		void CustomModel::newElementsInterleaved(const size_t N, GLuint* interleavedIndices)
		{
			addElementsInterleaved(N, interleavedIndices);
		}

		/**
		\fn void CustomModel::newElements(const size_t N, GLuint* a, GLuint* b, GLuint* c, GLuint* d)
		\brief Add primitive elements to the model.
		\param N Number of primitive elements.
		\param a First vertices indices.
		\param b Second vertices indices.
		\param c Third vertices indices.
		\param d Fourth vertices indices.

		All arrays must contain N indices.
		**/
		void CustomModel::newElements(const size_t N, GLuint* a, GLuint* b, GLuint* c, GLuint* d)
		{
			addElements(N, a, b, c, d);
		}

		/**
		\fn GLuint CustomModel::newElement(GLuint a)
		\brief Add a primitive element to the model.
		\param a Index of the first vertex.
		\return The index of the newly created element.
		**/
		GLuint CustomModel::newElement(GLuint a)
		{
			return addElement(a);
		}

		/**
		\fn GLuint CustomModel::newElement(GLuint a, GLuint b)
		\brief Add a primitive element to the model.
		\param a Index of the first vertex.
		\param b Index of the second vertex.
		\return The index of the newly created element.
		**/
		GLuint CustomModel::newElement(GLuint a, GLuint b)
		{
			return addElement(a, b);
		}

		/**
		\fn GLuint CustomModel::newElement(GLuint a, GLuint b, GLuint c)
		\brief Add a primitive element to the model.
		\param a Index of the first vertex.
		\param b Index of the second vertex.
		\param c Index of the third vertex.
		\return The index of the newly created element.
		**/
		GLuint CustomModel::newElement(GLuint a, GLuint b, GLuint c)
		{
			return addElement(a, b, c);
		}

		/**
		\fn GLuint CustomModel::newElement(GLuint a, GLuint b, GLuint c, GLuint d)
		\brief Add a primitive element to the model.
		\param a Index of the first vertex.
		\param b Index of the second vertex.
		\param c Index of the third vertex.
		\param d Index of the fourth vertex.
		\return The index of the newly created element.
		**/
		GLuint CustomModel::newElement(GLuint a, GLuint b, GLuint c, GLuint d)
		{
			return addElement(a, b, c, d);
		}

		/**
		\fn GLuint CustomModel::newElement(const std::vector<GLuint>& indices)
		\brief Add a primitive element to the model.
		\param indices A vector containing the indices of all the vertices used.
		\return The index of the newly created element.
		**/
		GLuint CustomModel::newElement(const std::vector<GLuint>& indices)
		{
			return addElement(indices);
		}

		/**
		\fn void CustomModel::generateNormals(void)
		\brief Automatically generate the normals.
		**/
		void CustomModel::generateNormals(void)
		{
			GeometryModel::generateNormals();
		}

