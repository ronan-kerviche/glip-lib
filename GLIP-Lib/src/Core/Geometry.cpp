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
#include "Core/Exception.hpp"
#include "Core/HdlVBO.hpp"
#include "Core/Geometry.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// GeometryModel
	/**
	\fn GeometryModel::GeometryModel(const GeometryType& _type, const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord = true)
	\brief Geometry model constructor.
	\param _type The type of the geometry (MUST BE GeometryModel::CustomModel for all user defined models).
	\param _dim The minimum number of spatial dimensions needed to describe the geometry (either 2 or 3).
	\param _primitiveGL The GL ID of the element primitive (eg. GL_POINTS, GL_LINES, GL_TRIANGLES, etc.)
	\param _hasTexCoord Set to true if the geometry has texel coordinates attached.
	**/
	GeometryModel::GeometryModel(const GeometryType& _type, const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord = true)
	 :	pos(),
		tex(),
		elements(),
		type(_type), 
		hasTexCoord(_hasTexCoord),
		dim(_dim), 
		numVerticesPerEl(getNumVerticesFromPrimitive(_primitiveGL)), 
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
	 :	pos(mdl.pos),
		tex(mdl.tex),
		elements(mdl.elements),
		type(mdl.type),
		hasTexCoord(mdl.hasTexCoord),
		dim(mdl.dim),
		numVerticesPerEl(mdl.numVerticesPerEl),
		primitiveGL(mdl.primitiveGL)
	{ }

	GeometryModel::~GeometryModel(void)
	{
		pos.clear();
		tex.clear();
		elements.clear();
	}

	/**
	\fn GLuint GeometryModel::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
	\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
	\return The index of the newly created vertex.
	**/
	GLuint GeometryModel::addVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=2)
			throw Exception("GeometryModel::addVertex2D - Dimensions should be equal to 2 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

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
	\fn GLuint GeometryModel::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
	\brief Add a vertex in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
	\param x The X coordinate.
	\param y The Y coordinate.
	\param z The Z coordinate.
	\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
	\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
	\return The index of the newly created vertex.
	**/
	GLuint GeometryModel::addVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
	{
		if(dim!=3)
			throw Exception("GeometryModel::addVertex3D - Dimensions should be equal to 3 (current : " + toString(dim) + ").", __FILE__, __LINE__, Exception::CoreException);

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
	\fn GLuint GeometryModel::addElement(GLuint a)
	\brief Add a primitive element to the model.
	\param a Index of the first vertex.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(GLuint a)
	{
		const int expectedNumberOfVertices = 1;

		if(numVerticesPerEl!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			elements.push_back(a);
			return getNumElements() - 1;
		}
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

		if(numVerticesPerEl!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			elements.push_back(a);
			elements.push_back(b);
			return getNumElements() - 1;
		}
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

		if(numVerticesPerEl!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
			return getNumElements() - 1;
		}
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

		if(numVerticesPerEl!=expectedNumberOfVertices)
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
			elements.push_back(d);
			return getNumElements() - 1;
		}
	}

	/**
	\fn GLuint GeometryModel::addElement(const std::vector<GLuint>& indices)
	\brief Add a primitive element to the model.
	\param indices A vector containing the indices of all the vertices used.
	\return The index of the newly created element.
	**/
	GLuint GeometryModel::addElement(const std::vector<GLuint>& indices)
	{
		if(numVerticesPerEl!=static_cast<int>(indices.size()))
			throw Exception("GeometryModel::addElement - Wrong number of vertex indices (" + toString(indices.size()) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			elements.insert(elements.end(), indices.begin(), indices.end());
			return getNumElements() - 1;
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
		return pos[i*dim+0];
	}

	/**
	\fn GLfloat& GeometryModel::y(GLuint i)
	\brief Access the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read/write access to corresponding variable.
	**/
	GLfloat& GeometryModel::y(GLuint i)
	{
		return pos[i*dim+1];
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

		return pos[i*dim+2];
	}

	/**
	\fn GLfloat& GeometryModel::u(GLuint i)
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoord is set to false.
	**/
	GLfloat& GeometryModel::u(GLuint i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryModel::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return tex[i*2+0];
	}

	/**
	\fn GLfloat& GeometryModel::v(GLuint i)
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read/write access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoord is set to false.
	**/
	GLfloat& GeometryModel::v(GLuint i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryModel::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return tex[i*2+1];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 0];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 1];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 2];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 3];
	}
	
	/**
	\fn const GLfloat& GeometryModel::x(GLuint i) const
	\brief Read the X coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::x(GLuint i) const
	{
		return pos[i*dim+0];
	}

	/**
	\fn const GLfloat& GeometryModel::y(GLuint i) const
	\brief Read the Y coordinate of the vertex at given index.
	\param i The vertex index.
	\return Read access to corresponding variable.
	**/
	const GLfloat& GeometryModel::y(GLuint i) const
	{
		return pos[i*dim+1];
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

		return pos[i*dim+2];
	}

	/**
	\fn const GLfloat& GeometryModel::u(GLuint i) const
	\brief Access the U coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoord is set to false.
	**/
	const GLfloat& GeometryModel::u(GLuint i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryModel::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return tex[i*2+0];
	}

	/**
	\fn GLfloat& GeometryModel::v(GLuint i) const
	\brief Access the V coordinate of the texel at given index.
	\param i The vertex/texel index.
	\return Read-only access to corresponding variable. Will raise an exception if GeometryModel::hasTexCoord is set to false.
	**/
	const GLfloat& GeometryModel::v(GLuint i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryModel::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__, Exception::CoreException);

		return tex[i*2+1];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 0];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 1];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 2];
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

		if(numVerticesPerEl<expectedNumberOfVertices)
			throw Exception("GeometryModel::a - Wrong number of vertex indices (" + toString(expectedNumberOfVertices) + " argument(s) received, " + toString(numVerticesPerEl) + " expected ).", __FILE__, __LINE__, Exception::CoreException);
		else
			return elements[i * numVerticesPerEl + 3];
	}

	/**
	\fn GLuint GeometryModel::getNumVertices(void) const
	\brief Get the number of vertices.
	\return The current number of vertices registered for this model.
	**/
	GLuint GeometryModel::getNumVertices(void) const
	{
		return pos.size() / dim;
	}

	/**
	\fn GLuint GeometryModel::getNumElements(void) const
	\brief Get the number of GL primitives (elements).
	\return The current number of GL primitives (element).
	**/
	GLuint GeometryModel::getNumElements(void) const
	{
		return elements.size() / numVerticesPerEl;
	}

	/**
	\fn bool GeometryModel::operator==(const GeometryModel& mdl) const
	\brief Test if two models are identical.
	\param mdl The reference model.
	\return true if the two models are identical.
	**/
	bool GeometryModel::operator==(const GeometryModel& mdl) const
	{
		return 		(hasTexCoord==mdl.hasTexCoord)
			&&	(dim==mdl.dim)
			&&	(numVerticesPerEl==mdl.numVerticesPerEl)
			&&	(primitiveGL==mdl.primitiveGL)
			&& 	(std::equal(pos.begin(), pos.end(), mdl.pos.begin()))
			&&	(std::equal(tex.begin(), tex.end(), mdl.tex.begin()))
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
		if(pos.empty())
			throw Exception("GeometryModel::getVBO - Empty vertices list.", __FILE__, __LINE__, Exception::CoreException);

		GLuint 		localNumElements = 0,
				localNumVerticesPerEl = 0,
				localNumDimTexCoord = 0;

		const GLuint* 	elementsPtr = NULL;
		const GLfloat*	texPtr = NULL;

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
				throw Exception("GeometryModel::getNumVerticesFromPrimitive - Unsupported primitive type : \"" + glParamName(_primitiveGL) + "\".", __FILE__, __LINE__, Exception::CoreException);
			default : 
				throw Exception("GeometryModel::getNumVerticesFromPrimitive - Unknown primitive type : \"" + glParamName(_primitiveGL) + "\".", __FILE__, __LINE__, Exception::CoreException);
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
		 : GeometryModel(GeometryModel::StandardQuad, 2, GL_TRIANGLES, true)
		{
			// Create the geometry :
			addVertex2D(-1.0, -1.0, 0.0, 0.0);
			addVertex2D(-1.0,  1.0, 0.0, 1.0);
			addVertex2D( 1.0,  1.0, 1.0, 1.0);
			addVertex2D( 1.0, -1.0, 1.0, 0.0);

			addElement(0, 1, 3);
			addElement(1, 2, 3);
		}

	// Reversed quad :
		/**
		\fn ReversedQuad::ReversedQuad(void)
		\brief ReversedQuad constructor.
		**/
		ReversedQuad::ReversedQuad(void)
		 : GeometryModel(GeometryModel::ReversedQuad, 2, GL_TRIANGLES, true)
		{
			// Create the geometry :			
			addVertex2D(-1.0,  1.0, 0.0, 0.0);
			addVertex2D( 1.0,  1.0, 1.0, 0.0);
			addVertex2D( 1.0, -1.0, 1.0, 1.0);
			addVertex2D(-1.0, -1.0, 0.0, 1.0);

			addElement(0, 1, 3);
			addElement(1, 2, 3);
		}
	

	// 2D Grid of points
		/**
		\fn PointsGrid2D::PointsGrid2D(int w, int h, bool normalized)
		\brief PointsGrid2D constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		\param normalized If true, the coordinates will be normalized in the range [0, 1].
		**/
		PointsGrid2D::PointsGrid2D(int w, int h, bool normalized)
		 : GeometryModel(GeometryModel::PointsGrid2D, 2, GL_POINTS, false)
		{
			for(int i=0; i<h; i++)
			{
				for(int j=0; j<w; j++)
				{
					if(!normalized)
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

	// 3D Grid of points
		/**
		\fn PointsGrid3D::PointsGrid3D(int w, int h, int d, bool normalized)
		\brief PointsGrid3D constructor.
		\param w Number of points along the X dimension.
		\param h Number of points along the Y dimension.
		\param d Number of points along the Z dimension.
		\param normalized If true, the coordinates will be normalized in the range [0, 1].
		**/
		PointsGrid3D::PointsGrid3D(int w, int h, int d, bool normalized)
		 : GeometryModel(GeometryModel::PointsGrid3D, 3, GL_POINTS, false)
		{
			for(int k=0; k<d; k++)
			{
				for(int i=0; i<h; i++)
				{
					for(int j=0; j<w; j++)
					{
						if(!normalized)
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

	// Custom model : 
		/**
		\fn CustomModel::CustomModel(const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord)
		\brief CustomModel constructor.
		\param _dim The minimum number of spatial dimensions needed to describe the geometry (either 2 or 3).
		\param _primitiveGL The GL ID of the element primitive (eg. GL_POINTS, GL_LINES, GL_TRIANGLES, etc.)
		\param _hasTexCoord Set to true if the geometry has texel coordinates attached.
		**/
		CustomModel::CustomModel(const int& _dim, const GLenum& _primitiveGL, const bool& _hasTexCoord)
		 : GeometryModel( GeometryModel::CustomModel, _dim, _primitiveGL, _hasTexCoord)
		{ }

		/**
		\fn GLuint CustomModel::newVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
		\brief Add a vertex in a two dimensions space (GeometryModel::dim must be equal to 2). If GeometryModel::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
		\param x The X coordinate.
		\param y The Y coordinate.
		\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
		\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
		\return The index of the newly created vertex.
		**/
		GLuint CustomModel::newVertex2D(const GLfloat& x, const GLfloat& y, const GLfloat& u, const GLfloat& v)
		{
			return 	addVertex2D(x, y, u, v);
		}

		/**
		\fn GLuint CustomModel::newVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
		\brief Add a vertex in a three dimensions space (GeometryModel::dim must be equal to 3). If GeometryModel::hasTexCoord is true, then it will also use u and v to create a texel coordinate.
		\param x The X coordinate.
		\param y The Y coordinate.
		\param z The Z coordinate.
		\param u The U coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
		\param v The V coordinate for the texel (ignored if GeometryModel::hasTexCoord is set to false).
		\return The index of the newly created vertex.
		**/
		GLuint CustomModel::newVertex3D(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& u, const GLfloat& v)
		{
			return 	addVertex3D(x, y, z, u, v);
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

