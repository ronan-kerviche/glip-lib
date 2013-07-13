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
	GeometryFormat::GeometryFormat(const GeometryPrimitive& _primitive, const int& _dim, const int& _numVerticesPerEl, const GLenum& _primitiveGL, const bool& _hasTexCoord = true)
	 : primitive(_primitive), dim(_dim), numVerticesPerEl(_numVerticesPerEl), primitiveGL(_primitiveGL), hasTexCoord(_hasTexCoord)
	{
		if(dim!=2 && dim!=3)
			throw Exception("GeometryFormat::GeometryFormat - Dimension must be either 2 or 3 (current : " + to_string(dim) + ").", __FILE__, __LINE__);
	}

	GeometryFormat::GeometryFormat(const GeometryFormat& fmt)
	 : pos(fmt.pos), tex(fmt.tex), dim(fmt.dim), numVerticesPerEl(fmt.numVerticesPerEl), primitiveGL(fmt.primitiveGL), elements(fmt.elements), primitive(fmt.primitive), hasTexCoord(fmt.hasTexCoord)
	{ }

	GeometryFormat::~GeometryFormat(void)
	{
		pos.clear();
		tex.clear();
		elements.clear();
	}

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

	GLfloat& GeometryFormat::x(int i)
	{
		return pos[i*dim+0];
	}

	GLfloat& GeometryFormat::y(int i)
	{
		return pos[i*dim+1];
	}

	GLfloat& GeometryFormat::z(int i)
	{
		if(dim<3)
			throw Exception("GeometryFormat::z - This geometry has only " + to_string(dim) + " dimensions.", __FILE__, __LINE__);

		return pos[i*dim+2];
	}

	GLfloat& GeometryFormat::u(int i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+0];
	}

	GLfloat& GeometryFormat::v(int i)
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+1];
	}

	const GLfloat& GeometryFormat::x(int i) const
	{
		return pos[i*dim+0];
	}

	const GLfloat& GeometryFormat::y(int i) const
	{
		return pos[i*dim+1];
	}

	const GLfloat& GeometryFormat::z(int i) const
	{
		if(dim<3)
			throw Exception("GeometryFormat::z - This geometry has only " + to_string(dim) + " dimensions.", __FILE__, __LINE__);

		return pos[i*dim+2];
	}

	const GLfloat& GeometryFormat::u(int i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::u - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+0];
	}

	const GLfloat& GeometryFormat::v(int i) const
	{
		if(!hasTexCoord)
			throw Exception("GeometryFormat::v - Current geometry does not have texture coordinates.", __FILE__, __LINE__);

		return tex[i*2+1];
	}

	int GeometryFormat::getNumVertices(void) const
	{
		return pos.size() / dim;
	}

	int GeometryFormat::getNumElements(void) const
	{
		return elements.size() / numVerticesPerEl;
	}

	bool GeometryFormat::operator==(const GeometryFormat& fmt)
	{
		return 		(primitive==fmt.primitive)
			&&	(hasTexCoord==fmt.hasTexCoord)
			&&	(dim==fmt.dim)
			&&	(numVerticesPerEl==fmt.numVerticesPerEl)
			&&	(primitiveGL==fmt.primitiveGL)
			&& 	(std::equal(pos.begin(), pos.end(), fmt.pos.begin()))
			&&	(std::equal(tex.begin(), tex.end(), fmt.tex.begin()))
			&&	(std::equal(elements.begin(), elements.end(), fmt.elements.begin()));
	}

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

	GeometryInstance::GeometryInstance(const GeometryInstance& instance)
	 : id(instance.id)	
	{
		counters[id]++;
	}

	GeometryInstance::~GeometryInstance(void)
	{
		counters[id]--;

		// If no more reference : 
		if(counters[id]<=0)
		{
			delete vbos[id];
			vbos[id] = NULL;
			delete formats[id];
			formats[id] = NULL;
		}
	}

	const GeometryFormat& GeometryInstance::format(void) const
	{
		return (*formats[id]);
	}

	void GeometryInstance::draw(void)
	{
		vbos[id]->draw();
	}

// Geometries : 
	// Standard quad : 
		StandardQuadGeometry::StandardQuadGeometry(void)
		 : GeometryFormat(GeometryQuad, 2, 4, GL_QUADS, true)
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

		PointsGrid2DGeometry::PointsGrid2DGeometry(int w, int h)
		 : GeometryFormat(GeometryPointsGrid, 2, 1, GL_POINTS, false)
		{
			for(int i=0; i<h; i++)
			{
				for(int j=0; j<w; j++)
				{
					addVertex2D(j,  i);
				}
			}
		}

		PointsGrid3DGeometry::PointsGrid3DGeometry(int w, int h, int z)
		 : GeometryFormat(GeometryPointsGrid, 3, 1, GL_POINTS, false)
		{
			for(int k=0; k<z; k++)
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

