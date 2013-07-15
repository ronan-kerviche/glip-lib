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

#include <algorithm>
#include "Exception.hpp"
#include "HdlVBO.hpp"
#include "Geometry.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// GeometryFormat
	/**
	\fn GeometryFormat::GeometryFormat(const int& _dim, const int& _numVerticesPerEl, const GLenum& _primitiveGL, const bool& _hasTexCoord = true)
	\brief Geometry format constructor.
	\param _dim The minimum number of spatial dimensions needed to describe the geometry (either 2 or 3).
	\param _numVerticesPerEl The number of vertices per elements.
	\param _primitiveGL The GL ID of the element primitive (eg. GL_POINTS, GL_QUAD, GL_TRIANGLES, etc.)
	\param _hasTexCoord Set to true if the geometry has texel coordinates attached.
	**/
	GeometryFormat::GeometryFormat(const int& _dim, const int& _numVerticesPerEl, const GLenum& _primitiveGL, const bool& _hasTexCoord = true)
	 : dim(_dim), numVerticesPerEl(_numVerticesPerEl), primitiveGL(_primitiveGL), hasTexCoord(_hasTexCoord)
	{
		if(dim!=2 && dim!=3)
			throw Exception("GeometryFormat::GeometryFormat - Dimension must be either 2 or 3 (current : " + to_string(dim) + ").", __FILE__, __LINE__);
	}

	/**
	\fn GeometryFormat::GeometryFormat(const GeometryFormat& fmt)
	\brief GeometryFormat copy constructor.
	\param fmt The source format.
	**/
	GeometryFormat::GeometryFormat(const GeometryFormat& fmt)
	 : pos(fmt.pos), tex(fmt.tex), dim(fmt.dim), numVerticesPerEl(fmt.numVerticesPerEl), primitiveGL(fmt.primitiveGL), elements(fmt.elements), hasTexCoord(fmt.hasTexCoord)
	{ }

	GeometryFormat::~GeometryFormat(void)
	{
		pos.clear();
		tex.clear();
		elements.clear();
	}

	/**
	\fn int GeometryFormat::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a two dimensions space (GeometryFormat::dim must be equal to 2). If GeometryFormat::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param u The U coordinate for the texel (ignored if GeometryFormat::hasTexCoord is set to false).
	\param v The V coordinate for the texel (ignored if GeometryFormat::hasTexCoord is set to false).
	\return The index of the newly created vertex.
	**/
	int GeometryFormat::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=2)
			throw Exception("GeometryFormat::addVertex2D - Dimensions should be equal to 2 (current : " + to_string(dim) + ").", __FILE__, __LINE__);

		pos.push_back(x);
		pos.push_back(y);

		if(hasTexCoord)
		{
			tex.push_back(u);
			tex.push_back(v);
		}

		return getNumVertices() - 1;
	}

	/**
	\fn int GeometryFormat::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a three dimensions space (GeometryFormat::dim must be equal to 3). If GeometryFormat::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param z The Z coordinate.
	\param u The U coordinate for the texel (ignored if GeometryFormat::hasTexCoord is set to false).
	\param v The V coordinate for the texel (ignored if GeometryFormat::hasTexCoord is set to false).
	\return The index of the newly created vertex.
	**/
	int GeometryFormat::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=3)
			throw Exception("GeometryFormat::addVertex3D - Dimensions should be equal to 3 (current : " + to_string(dim) + ").", __FILE__, __LINE__);

		pos.push_back(x);
		pos.push_back(y);
		pos.push_back(z);

		if(hasTexCoord)
		{
			tex.push_back(u);
			tex.push_back(v);
		}

		return getNumVertices() - 1;
	}

	/**
	\fn GLfloat& GeometryFormat::x(int i)
	\brief Access the X coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryFormat::x(int i)
	{
		return pos[i*dim+0];
	}

	/**
	\fn GLfloat& GeometryFormat::y(int i)
	\brief Access the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryFormat::y(int i)
	{
		return pos[i*dim+1];
	}

	/**
	\fn GLfloat& GeometryFormat::z(int i)
	\brief Access the Z coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryFormat::dim is smaller than 3.
	**/
	GLfloat& GeometryFormat::z(int i)
	{
		if(dim<3)
			throw Exception("GeometryFormat::z - This geometry has only " + to_string(dim) + " dimensions.", __FILE__, __LINE__);

		return pos[i*dim+2];
	}

	/**
	\fn GLfloat& GeometryFormat::u(int i)
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryFormat::hasTexCoord is set to false.
	**/
	GLfloat& GeometryFormat::u(int i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+0];
	}

	/**
	\fn GLfloat& GeometryFormat::v(int i)
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryFormat::hasTexCoord is set to false.
	**/
	GLfloat& GeometryFormat::v(int i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+1];
	}

	/**
	\fn const GLfloat& GeometryFormat::x(int i) const
	\brief Read the X coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryFormat::x(int i) const
	{
		return pos[i*dim+0];
	}

	/**
	\fn const GLfloat& GeometryFormat::y(int i) const
	\brief Read the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryFormat::y(int i) const
	{
		return pos[i*dim+1];
	}

	/**
	\fn const GLfloat& GeometryFormat::z(int i) const
	\brief Read the Z coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable. Will raise an exception if GeometryFormat::dim is smaller than 3.
	**/
	const GLfloat& GeometryFormat::z(int i) const
	{
		if(dim<3)
			throw Exception("GeometryFormat::z - This geometry has only " + to_string(dim) + " dimensions.", __FILE__, __LINE__);

		return pos[i*dim+2];
	}

	/**
	\fn const GLfloat& GeometryFormat::u(int i) const
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryFormat::hasTexCoord is set to false.
	**/
	const GLfloat& GeometryFormat::u(int i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+0];
	}

	/**
	\fn GLfloat& GeometryFormat::v(int i) const
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryFormat::hasTexCoord is set to false.
	**/
	const GLfloat& GeometryFormat::v(int i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+1];
	}

	/**
	\fn int GeometryFormat::getNumVertices(void) const
	\brief Get the number of vertices.
	\return The current number of vertices registered for this format.
	**/
	int GeometryFormat::getNumVertices(void) const
	{
		return pos.size() / dim;
	}

	/**
	\fn int GeometryFormat::getNumElements(void) const
	\brief Get the number of GL primitives (elements).
	\return The current number of GL primitives (element).
	**/
	int GeometryFormat::getNumElements(void) const
	{
		return elements.size() / numVerticesPerEl;
	}

	/**
	\fn bool GeometryFormat::operator==(const GeometryFormat& fmt)
	\brief Test if two formats are identical.
	\param fmt The reference format.
	\return true if the two formats are identical.
	**/
	bool GeometryFormat::operator==(const GeometryFormat& fmt)
	{
		return 		(hasTexCoord==fmt.hasTexCoord)
			&&	(dim==fmt.dim)
			&&	(numVerticesPerEl==fmt.numVerticesPerEl)
			&&	(primitiveGL==fmt.primitiveGL)
			&& 	(std::equal(pos.begin(), pos.end(), fmt.pos.begin()))
			&&	(std::equal(tex.begin(), tex.end(), fmt.tex.begin()))
			&&	(std::equal(elements.begin(), elements.end(), fmt.elements.begin()));
	}

	/**
	\fn HdlVBO* GeometryFormat::getVBO(GLenum freq)
	\brief Get the VBO corresponding to this format. It is not recommended to use directly the VBO but, instead, use a GeometryInstance object.
	\param freq The GL frequency of read/write operations (among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	\return A pointer to the newly created VBO. It is of the responsability of the user to call delete on this object.
	**/
	HdlVBO* GeometryFormat::getVBO(GLenum freq)
	{
		if(pos.empty())
			throw Exception("GeometryFormat::getVBO - Empty vertices list.", __FILE__, __LINE__);

		int 		localNumElements = 0,
				localNumVerticesPerEl = 0,
				localNumDimTexCoord = 0;

		GLuint* 	elementsPtr = NULL;
		GLfloat*	texPtr = NULL;

		if(!elements.empty())
		{
			localNumElements = getNumElements();
			localNumVerticesPerEl = numVerticesPerEl;
			elementsPtr = &elements[0];
		}

		if(!tex.empty())
		{
			localNumDimTexCoord = 2;
			texPtr = &tex[0];
		}

		return new HdlVBO(getNumVertices(), dim, freq, &pos[0], localNumElements, localNumVerticesPerEl, elementsPtr, primitiveGL, localNumDimTexCoord, texPtr);
	}

// GeometryInstance
	std::vector<HdlVBO*> 		GeometryInstance::vbos;
	std::vector<GeometryFormat*>	GeometryInstance::formats;
	std::vector<int>		GeometryInstance::counters;

	/**
	\fn GeometryInstance::GeometryInstance(GeometryFormat& fmt, GLenum freq)
	\brief GeometryInstance constructor.
	\param fmt The original format to use.
	\param freq The GL frequency of read/write operations (among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	**/
 	GeometryInstance::GeometryInstance(GeometryFormat& fmt, GLenum freq)
	 : id(-1)
	{
		// Find if a similar format exist :
		int 	k	= 0,
			kNull 	= -1;
		for(; k<formats.size(); k++)
		{
			if(formats[k]!=NULL)
			{
				if( (*formats[k])==fmt )
					break;
			}
			else
				kNull = k;
		}

		// If one was found :
		if(k<formats.size())
		{
			// Set the id and increase the counter :
			id = k;
			counters[id]++;
		}
		else
		{
			// No holes?
			if(kNull<0)
			{
				kNull = vbos.size();
				vbos.push_back(NULL);
				formats.push_back(NULL);
				counters.push_back(0);
			}

			// Create :
			vbos[kNull] 	= fmt.getVBO(freq);
			formats[kNull]	= new GeometryFormat(fmt);
			counters[kNull]	= 1;

			id = kNull;
		}
	}

	/**
	\fn GeometryInstance::GeometryInstance(const GeometryInstance& instance)
	\brief GeometryInstance copy constructor.
	\param instance The source format.
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
			vbos[id] = NULL;
			delete formats[id];
			formats[id] = NULL;
		}
	}

	/**
	\fn const GeometryFormat& GeometryInstance::format(void) const
	\brief Access to the format of this instance.
	\return A read-only access to the GeometryFormat object.
	**/
	const GeometryFormat& GeometryInstance::format(void) const
	{
		return (*formats[id]);
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
		\fn StandardQuadGeometry::StandardQuadGeometry(void)
		\brief StandardQuadGeometry constructor.
		**/
		StandardQuadGeometry::StandardQuadGeometry(void)
		 : GeometryFormat(2, 4, GL_QUADS, true)
		{
			// Create the geometry :
			addVertex2D(-1.0,  1.0, 0.0, 1.0);
			addVertex2D(-1.0, -1.0, 0.0, 0.0);
			addVertex2D( 1.0,  1.0, 1.0, 1.0);
			addVertex2D( 1.0, -1.0, 1.0, 0.0);

			elements.push_back(0);
			elements.push_back(1);
			elements.push_back(3);
			elements.push_back(2);
		}

	// 2D Grid of points
		/**
		\fn PointsGrid2DGeometry::PointsGrid2DGeometry(int w, int h)
		\brief PointsGrid2DGeometry constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		**/
		PointsGrid2DGeometry::PointsGrid2DGeometry(int w, int h)
		 : GeometryFormat(2, 1, GL_POINTS, false)
		{
			for(int i=0; i<h; i++)
			{
				for(int j=0; j<w; j++)
				{
					addVertex2D(j,  i);
				}
			}
		}

	// 3D Grid of points
		/**
		\fn PointsGrid3DGeometry::PointsGrid3DGeometry(int w, int h, int d)
		\brief PointsGrid2DGeometry constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		\param d Number of points along the Z dimension.
		**/
		PointsGrid3DGeometry::PointsGrid3DGeometry(int w, int h, int d)
		 : GeometryFormat(3, 1, GL_POINTS, false)
		{
			for(int k=0; k<d; k++)
			{
				for(int i=0; i<h; i++)
				{
					for(int j=0; j<w; j++)
					{
						addVertex3D(j, i, k);
					}
				}
			}
		}
