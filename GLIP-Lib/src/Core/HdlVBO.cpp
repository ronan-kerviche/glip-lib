/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlVBO.cpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Vertex Buffer Object Handle                                                        */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlVBO.cpp
 * \brief   OpenGL Vertex Buffer Object Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
*/

#include <cstring>
#include "Core/HdlVBO.hpp"
#include "Core/Exception.hpp"

using namespace Glip::CoreGL;

// Tools
	/**
	\fn HdlVBO::HdlVBO(int _nVert, int _dim, GLenum freq, const GLfloat* _vertices, int _nElements, int _nIndPerElement, const GLuint* _elements, GLenum _type, int _dimTexCoords, const GLfloat* _texcoords)
	\brief HdlVBO constructor.
	\param _nVert Number of vertices.
	\param _dim Dimension of the space containing the vertices (2 or 3).
	\param freq Frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	\param _vertices Pointer to vertices data.
	\param _nElements Number of elements.
	\param _nIndPerElement Number of index per elements (2, 3, 4...).
	\param _elements Pointer to elements data.
	\param _type Type of the elements (GL_POINTS, GL_LINE, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_QUADS, GL_QUAD_STRIP, GL_POLYGON).
	\param _dimTexCoords Dimension per texel (2 or 3).
	\param _texcoords Pointer to the texel coordiantes data.
	**/
	HdlVBO::HdlVBO(int _nVert, int _dim, GLenum freq, const GLfloat* _vertices, int _nElements, int _nIndPerElement, const GLuint* _elements, GLenum _type, int _dimTexCoords, const GLfloat* _texcoords)
	 : nVert(_nVert), nElements(_nElements), dim(_dim), nIndPerElement(_nIndPerElement), dimTexCoords(_dimTexCoords), vertices(NULL), elements(NULL), type(_type)
	{
		//REAL FUNCTION : FIX_MISSING_GLEW_CALL(glVertexPointer, glVertexPointerEXT)
		//REAL FUNCTION : FIX_MISSING_GLEW_CALL(glTexCoordPointer, glTexCoordPointerEXT)
		//REAL FUNCTION : FIX_MISSING_GLEW_CALL(glDrawArrays, glDrawArraysEXT)

		if(dimTexCoords!=0 && _texcoords==NULL)
			throw Exception("HdlVBO::HdlVBO - attempt to create texcoords without any data", __FILE__, __LINE__, Exception::GLException);

		/*std::cout << "Creating VBO : " << std::endl;
		std::cout << "    Vertices : " << nVert << std::endl;
		std::cout << "    Dim      : " << dim << std::endl;
		std::cout << "    Elements : " << nElements << std::endl;
		std::cout << "    Index    : " << nIndPerElement << std::endl;
		std::cout << "    Type     : " << glParamName(type) << std::endl;
		std::cout << "    DimTex   : " << dimTexCoords << std::endl;*/

		// For the vertices and the texcoords :
		//std::cout << " - Vertices - " << nVert*dim << std::endl;
		if(_texcoords!=NULL)
		{
			//std::cout << " - Texcoords - " << nVert*dimTexCoords*sizeof(GLfloat) << std::endl;
			vertices = new HdlGeBO(nVert*(dim+dimTexCoords)*sizeof(GLfloat), GL_ARRAY_BUFFER_ARB, freq);
		}
		else
			vertices = new HdlGeBO(nVert*dim*sizeof(GLfloat), GL_ARRAY_BUFFER_ARB, freq);

		vertices->subWrite(_vertices, nVert*dim*sizeof(GLfloat),0);
		//std::cout << "Vertices : "; glErrors(true, false);

		if(_texcoords!=NULL)
		{
			vertices->subWrite(_texcoords, nVert*dimTexCoords*sizeof(GLfloat), nVert*dim*sizeof(GLfloat));
			//std::cout << "Texcoords : "; glErrors(true, false);
		}

		// For the elements :
		if(_elements!=NULL)
		{
			//std::cout << " - Elements - " << nElements*nIndPerElement*sizeof(GLuint) << std::endl;
			elements = new HdlGeBO(nElements*nIndPerElement*sizeof(GLuint), GL_ELEMENT_ARRAY_BUFFER_ARB, freq);
			elements->subWrite(_elements, nElements*nIndPerElement*sizeof(GLuint), 0);
			//std::cout << "Elements : "; glErrors(true, false);
		}

		//std::cout << "Exit VBO : "; glErrors(true, false);
		HdlVBO::unbind();
	}

	HdlVBO::~HdlVBO(void)
	{
		delete vertices;
		delete elements;
	}

	/**
	\fn int HdlVBO::getVerticesCount(void)
	\return The number of vertices.
	\fn int HdlVBO::getShapeDimension(void)
	\return The dimension of the space.
	\fn int HdlVBO::getElementsCount(void)
	\return The number of elements.
	\fn GLenum HdlVBO::getType(void)
	\return The type of the elements.
	**/
	int HdlVBO::getVerticesCount(void)  { return nVert; }
	int HdlVBO::getShapeDimension(void) { return dim; }
	int HdlVBO::getElementsCount(void)  { return nElements; }
	GLenum HdlVBO::getType(void)        { return type; }

	/**
	\fn void HdlVBO::draw(void)
	\brief Draw the VBO according to its data.
	**/
	void HdlVBO::draw(void)
	{
		// First, bind the data
		vertices->bind(GL_ARRAY_BUFFER_ARB);
		glVertexPointer(dim, GL_FLOAT, 0, 0);

		if(dimTexCoords>0)
			glTexCoordPointer(dimTexCoords, GL_FLOAT, 0, reinterpret_cast<void*>(nVert*dim*sizeof(GLfloat)));

		if(elements!=NULL)
			elements->bind(GL_ELEMENT_ARRAY_BUFFER);;

		// Enable :
		glEnableClientState(GL_VERTEX_ARRAY);
		if(dimTexCoords>0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if(elements==NULL)
			glDrawArrays(GL_POINTS, 0, nVert);
		else
			glDrawElements(type, nElements*nIndPerElement, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		if(dimTexCoords>0)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		HdlVBO::unbind();
	}

// Static tools :
	/**
	\fn void HdlVBO::unbind(void)
	\brief Unbind all VBO targets (GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB);
	**/
	void HdlVBO::unbind(void)
	{
		HdlGeBO::unbind(GL_ARRAY_BUFFER_ARB);
		HdlGeBO::unbind(GL_ELEMENT_ARRAY_BUFFER_ARB);
	}

	/**
	\fn void HdlVBO::unmap(void)
	\brief Unmap all VBO targets (GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB);
	**/
	void HdlVBO::unmap(void)
	{
		HdlGeBO::unmap(GL_ARRAY_BUFFER_ARB);
		HdlGeBO::unmap(GL_ELEMENT_ARRAY_BUFFER_ARB);
	}

