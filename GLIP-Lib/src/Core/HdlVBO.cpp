/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
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
 * \version 0.6
 * \date    August 7th 2010
*/

#include <cstring>
#include "HdlVBO.hpp"
#include "../include/GLIPLib.hpp"


using namespace Glip::CoreGL;

// Tools
    HdlVBO::HdlVBO(int _nVert, int _dim, GLenum freq, GLfloat* _vertices, int _nElements, int _nIndPerElement, GLuint* _elements, GLenum _type, int _dimTexCoords, GLfloat* _texcoords)
     : nVert(_nVert), dim(_dim), nElements(_nElements), type(_type), nIndPerElement(_nIndPerElement), dimTexCoords(_dimTexCoords)
    {
        void* ptr = NULL;

        if(dimTexCoords!=0 && _texcoords==NULL)
		throw Exception("HdlVBO::HdlVBO - attempt to create texcoords without any data", __FILE__, __LINE__);

        // For the vertices and the texcoords :
        std::cout << " - Vertices - " << nVert*dim << std::endl;
        vertices = new HdlGeBO(nVert*dim*sizeof(GLfloat)+nVert*dimTexCoords*sizeof(GLfloat), GL_ARRAY_BUFFER_ARB, freq);
        ptr      = vertices->map(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        std::cout << "Mapping : "; glErrors(true, false);
        if(ptr!=NULL)
        {
            memcpy(ptr, 				_vertices, 	nVert*dim*sizeof(GLfloat));
            if(_texcoords!=NULL) memcpy((ptr+nVert*dim*sizeof(GLfloat)), 	_texcoords, 	nVert*dimTexCoords*sizeof(GLfloat));
        }
        else
            throw Exception("HdlVBO::HdlVBO - cannot map the VBO to set up the vertices", __FILE__, __LINE__);
        std::cout << "Unmapping..." << std::endl;
        HdlVBO::unmap();
        std::cout << "Vertices : "; glErrors(true, false);


        // For the elements :
        std::cout << " - Elements - " << nElements*nIndPerElement << std::endl;
        if(_elements!=NULL)
        {
            elements = new HdlGeBO(nElements*nIndPerElement*sizeof(GLuint), GL_ELEMENT_ARRAY_BUFFER_ARB, freq);
            ptr      = elements->map(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
            if(ptr!=NULL)
                memcpy(ptr, _elements, nElements*nIndPerElement*sizeof(GLuint));
            else
                throw Exception("HdlVBO::HdlVBO - cannot map the VBO to set up the elements", __FILE__, __LINE__);
            HdlVBO::unmap();
            glIndexPointer(GL_UNSIGNED_INT, 0, 0);
        }
        std::cout << "Elements : "; glErrors(true, false);

	std::cout << "Exit VBO : "; glErrors(true, false);
	HdlVBO::unbind();
    }

    HdlVBO::~HdlVBO(void)
    {
        delete vertices;
        delete elements;
    }

    int HdlVBO::getVerticesCount(void)  { return nVert; }
    int HdlVBO::getShapeDimension(void) { return dim; }
    int HdlVBO::getElementsCount(void)  { return nElements; }
    GLenum HdlVBO::getType(void)        { return type; }

	void HdlVBO::draw(void)
	{
		std::cout << "    Rendering start : "; glErrors(true, false);

		// First, bind the data
					vertices->bind(GL_ARRAY_BUFFER_ARB);
		if(elements!=NULL)   	elements->bind(GL_ELEMENT_ARRAY_BUFFER);
		std::cout << "    Binding : "; glErrors(true, false);

		// Then Enable client state :
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glEnableClientState(GL_INDEX_ARRAY);
		std::cout << "    Enabling : "; glErrors(true, false);

		// Then, describe the format given for this VBO
					glVertexPointer(dim, GL_FLOAT, 0, 0);
		if(dimTexCoords>0) 	glTexCoordPointer(dimTexCoords, GL_FLOAT, 0, (GLvoid*)((char*)NULL+nVert*dim)); //*sizeof(GLfloat)
		std::cout << "    Vertex description 1 : "; glErrors(true, false);
		/*if(elements!=NULL) 	glIndexPointer(GL_UNSIGNED_INT, 0, 0);
		std::cout << "    Vertex description 2 : "; glErrors(true, false);*/

		if(elements==NULL) 	glDrawArrays(GL_POINTS, 0, nVert);
		else               	glDrawElements(type, nElements, GL_UNSIGNED_INT, 0);
		std::cout << "    Drawing : "; glErrors(true, false);

					glDisableClientState(GL_VERTEX_ARRAY);
		//if(elements!=NULL)  	glDisableClientState(GL_INDEX_ARRAY);
		if(dimTexCoords>0) 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		std::cout << "    Disabling : "; glErrors(true, false);

		//HdlVBO::unbind();
		std::cout << "    Rendering end : "; glErrors(true, false);
	}

// Static tools
    HdlVBO* HdlVBO::generate2DStandardQuad(void)
    {
        GLfloat vertices[]  = { 100.0,  100.0,
                                100.0, -100.0,
                               -100.0, -100.0,
                               -100.0,  100.0};
        GLuint elements[]   = { 0, 1, 2, 3};
        GLfloat texcoords[] = { 1.0,  1.0,
                                1.0, -1.0,
                               -1.0, -1.0,
                               -1.0,  1.0};
        return new HdlVBO(4, 2, GL_STREAM_DRAW_ARB, vertices, 1, 4, elements, GL_QUADS, 2, texcoords);
    }

    HdlVBO* HdlVBO::generate2DGrid(int w, int h, GLfloat appW, GLfloat appH, GLfloat cX, GLfloat cY)
    {
        if(w<=0 || h<=0)
        {
            std::cerr << "HdlVBO::generate2DGrid : cannot create negative sized grid" << std::endl;
            return NULL;
        }

        //Create the grid first:
        GLfloat x = appW/w,
                y = appH/h;
        cX = cX - appW/2.0;
        cY = cY - appH/2.0;

        GLfloat* data = new GLfloat[w*h*2];
        unsigned int index = 0;

        for(unsigned int i=0; i<h; i++)
            for(unsigned int j=0; j<w; j++)
            {
                data[index + 0] = i*y + cY;
                data[index + 1] = j*x + cX;
                index += 2;
            }

        HdlVBO* result = new HdlVBO(w*h, 2, GL_STREAM_DRAW_ARB, data);
        delete[] data;

        return result;
    }


// Static tools
    void HdlVBO::unbind(void)
    {
        HdlGeBO::unbind(GL_ARRAY_BUFFER_ARB);
        HdlGeBO::unbind(GL_ELEMENT_ARRAY_BUFFER_ARB);
    }

    void HdlVBO::unmap(void)
    {
        HdlGeBO::unmap(GL_ARRAY_BUFFER_ARB);
        HdlGeBO::unmap(GL_ELEMENT_ARRAY_BUFFER_ARB);
    }
