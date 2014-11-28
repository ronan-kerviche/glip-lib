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
			throw Exception("HdlVBO::HdlVBO - attempt to create texcoords without any data", __FILE__, __LINE__);

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
		//std::cout << "    Rendering start : "; glErrors(true, false);

		// First, bind the data
			vertices->bind(GL_ARRAY_BUFFER_ARB);
		//std::cout << "    Binding : "; glErrors(true, false);
			glVertexPointer(dim, GL_FLOAT, 0, 0);
		//std::cout << "    Vertex description 1 : "; glErrors(true, false);
		if(dimTexCoords>0)
		{
			//OLD, offset on NLL : glTexCoordPointer(dimTexCoords, GL_FLOAT, 0, reinterpret_cast<void*>(NULL + nVert*dim*sizeof(GLfloat)));
			glTexCoordPointer(dimTexCoords, GL_FLOAT, 0, reinterpret_cast<void*>(nVert*dim*sizeof(GLfloat)));
			//std::cout << "    Texcoords description 1 : "; glErrors(true, false);
		}

		if(elements!=NULL)
		{
			elements->bind(GL_ELEMENT_ARRAY_BUFFER);
			//std::cout << "    Binding : "; glErrors(true, false);
		}

		// Enable :
		glEnableClientState(GL_VERTEX_ARRAY);
		if(dimTexCoords>0)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if(elements==NULL) 	glDrawArrays(GL_POINTS, 0, nVert); //Test with GL_TRIANGLE_STRIP
		else               	glDrawElements(type, nElements*nIndPerElement, GL_UNSIGNED_INT, 0);
		//std::cout << "    Drawing : "; glErrors(true, false);

					glDisableClientState(GL_VERTEX_ARRAY);
		if(dimTexCoords>0) 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//std::cout << "    Disabling : "; glErrors(true, false);

		HdlVBO::unbind();
		//std::cout << "    Rendering end : "; glErrors(true, false);
	}

// Static tools
	/*
	\fn HdlVBO* HdlVBO::generate2DStandardQuad(void)
	\brief Build a VBO containing a single quad between (-1.0,  1.0); (-1.0, -1.0); (1.0,  1.0); (1.0, -1.0) and adequate texcoords.
	\return A pointer to a VBO, you have reponsability of freeing it.
	*/
	/*HdlVBO* HdlVBO::generate2DStandardQuad(void)
	{
		GLfloat vertices[]  = {	-1.0,  1.0,
					-1.0, -1.0,
					 1.0,  1.0,
					 1.0, -1.0};
		GLuint elements[]   = { 0, 1, 3, 2};
		GLfloat texcoords[] = { 0.0,  1.0,
					0.0,  0.0,
					1.0,  1.0,
					1.0,  0.0};
		return new HdlVBO(4, 2, GL_STATIC_DRAW_ARB, vertices, 1, 4, elements, GL_QUADS, 2, texcoords);
	}*/

	/*
	\fn HdlVBO* HdlVBO::generate2DGrid(int w, int h, GLfloat appW, GLfloat appH, GLfloat cX, GLfloat cY)
	\brief Build a VBO containing a grid of points.
	\param w The number of dots in the X direction.
	\param h The number of dots in the Y direction.
	\param appW The size of the grid in the X direction.
	\param appH The size of the grid in the Y direction.
	\param cX The center of the grid in the X direction.
	\param cY The center of the grid in the Y direction.
	\return A pointer to a VBO, you have reponsability of freeing it.
	*/
	/*HdlVBO* HdlVBO::generate2DGrid(int w, int h, GLfloat appW, GLfloat appH, GLfloat cX, GLfloat cY)
	{
		if(w<=0 || h<=0)
			throw Exception("HdlVBO::generate2DGrid : cannot create negative sized grid",__FILE__, __LINE__);

		//Create the grid first:
		GLfloat x = appW/static_cast<GLfloat>(w-1),
			y = appH/static_cast<GLfloat>(h-1);
		cX = cX - appW/2.0;
		cY = cY - appH/2.0;

		GLfloat* data = new GLfloat[w*h*2];
		GLfloat* tex  = new GLfloat[w*h*2];
		unsigned int index = 0;

		for(unsigned int i=0; i<h; i++)
			for(unsigned int j=0; j<w; j++)
			{
				data[index + 0] = i*y + cY;
				data[index + 1] = j*x + cX;
				tex[index + 0]	= static_cast<GLfloat>(i)/static_cast<GLfloat>(h);
				tex[index + 1]	= static_cast<GLfloat>(j)/static_cast<GLfloat>(w);
				index += 2;
			}

		HdlVBO* result = new HdlVBO(w*h, 2, GL_STATIC_DRAW_ARB, data, 0, 0, NULL, GL_NONE, 2, tex);
		delete[] data;
		delete[] tex;

		return result;
	}*/

	/*
	\fn HdlVBO* HdlVBO::generate3DGrid(int w, int h, int d, GLfloat appW, GLfloat appH, GLfloat appD, GLfloat cX, GLfloat cY, GLfloat cZ)
	\brief Build a VBO containing a grid of points. This function will not create texel coordinates for the VBO.
	\param w The number of dots in the X direction.
	\param h The number of dots in the Y direction.
	\param d The number of dots in the Z direction.
	\param appW The size of the grid in the X direction.
	\param appH The size of the grid in the Y direction.
	\param appD The size of the grid in the Z direction.
	\param cX The center of the grid in the X direction.
	\param cY The center of the grid in the Y direction.
	\param cZ The center of the grid in the Z direction.
	\return A pointer to a VBO, you have reponsability of freeing it.
	*/
	/*HdlVBO* HdlVBO::generate3DGrid(int w, int h, int d, GLfloat appW, GLfloat appH, GLfloat appD, GLfloat cX, GLfloat cY, GLfloat cZ)
	{
		if(w<=0 || h<=0 || d<=0)
			throw Exception("HdlVBO::generate3DGrid : cannot create negative sized grid",__FILE__, __LINE__);

		// Compute the steps and upper-left corner position :
		GLfloat x = appW/static_cast<GLfloat>(w-1),
			y = appH/static_cast<GLfloat>(h-1),
			z = appD/static_cast<GLfloat>(d-1);
		cX = cX - appW/2.0;
		cY = cY - appH/2.0;
		cZ = cZ - appD/2.0;

		GLfloat* data = new GLfloat[w*h*d*3];
		unsigned int index = 0;

		for(unsigned int k=0; k<d; k++)
		{
			for(unsigned int i=0; i<h; i++)
			{
				for(unsigned int j=0; j<w; j++)
				{
					data[index + 0] = j*x + cX;
					data[index + 1] = i*y + cY;
					data[index + 2] = k*z + cZ;
					index += 3;
				}
			}
		}

		HdlVBO* result = new HdlVBO(w*h*d, 3, GL_STATIC_DRAW_ARB, data, 0, 0, NULL, GL_POINTS);
		delete[] data;

		return result;
	}*/

	// Static tools
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
