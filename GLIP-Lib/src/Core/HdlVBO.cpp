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
        /* OLD : HdlGeBO(_nVert*_dim*sizeof(GLfloat), GL_ARRAY_BUFFER_ARB, freq)
        void* ptr = map(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

        if(ptr!=NULL)
            memcpy(ptr, data, nVert*dim*sizeof(GLfloat));
        else
            std::cerr << "HdlVBO::HdlVBO : cannot map the VBO" << std::endl;
        HdlVBO::unmap();*/

        void* ptr = NULL;

        // For the vertices :
        std::cout << " - Vertices - " << nVert*dim << std::endl;
        vertices = new HdlGeBO(nVert*dim*sizeof(GLfloat), GL_ARRAY_BUFFER_ARB, freq);
        // ERROR?
        ptr      = vertices->map(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        if(ptr!=NULL)
            memcpy(ptr, _vertices, nVert*dim*sizeof(GLfloat));
        else
            Exception("HdlVBO::HdlVBO - cannot map the VBO to set up the vertices", __FILE__, __LINE__);
        HdlVBO::unmap();


        // For the elements :
        std::cout << " - Elements - " << nElements*nIndPerElement << std::endl;
        if(_elements!=NULL)
        {
            elements = new HdlGeBO(nElements*nIndPerElement*sizeof(GLuint), GL_ELEMENT_ARRAY_BUFFER_ARB, freq);
            ptr      = elements->map(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
            if(ptr!=NULL)
                memcpy(ptr, _elements, nElements*nIndPerElement*sizeof(GLuint));
            else
                Exception("HdlVBO::HdlVBO - cannot map the VBO to set up the elements", __FILE__, __LINE__);
            HdlVBO::unmap();
        }

        // For the Texcoords :
        std::cout << " - Texcoord - " << std::endl;
        if(_texcoords!=NULL)
        {
            texcoords = new HdlGeBO(nVert*dimTexCoords*sizeof(GLfloat), GL_TEXTURE_COORD_ARRAY, freq);
            ptr      = texcoords->map(GL_TEXTURE_COORD_ARRAY, GL_WRITE_ONLY_ARB);
            if(ptr!=NULL)
                memcpy(ptr, _texcoords, nVert*dimTexCoords*sizeof(GLfloat));
            else
                Exception("HdlVBO::HdlVBO - cannot map the VBO to set up the texcoords", __FILE__, __LINE__);
            HdlVBO::unmap();
        }
    }

    HdlVBO::~HdlVBO(void)
    {
        delete vertices;
        delete elements;
        delete texcoords;
    }

    int HdlVBO::getVerticesCount(void)  { return nVert; }
    int HdlVBO::getShapeDimension(void) { return dim; }
    int HdlVBO::getElementsCount(void)  { return nElements; }
    GLenum HdlVBO::getType(void)        { return type; }

    void HdlVBO::draw(void)
    {
        // First, bind the data
                             vertices->bind(GL_ARRAY_BUFFER_ARB);
        if(elements!=NULL)   elements->bind(GL_ELEMENT_ARRAY_BUFFER);
        if(texcoords!=NULL) texcoords->bind(GL_TEXTURE_COORD_ARRAY);

        // Then, describe the format given for this VBO
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(dim, GL_FLOAT, 0, 0);

        if(elements!=NULL)
        {
            glEnableClientState(GL_INDEX_ARRAY);
            glIndexPointer(GL_UNSIGNED_INT, 0, 0);
        }

        if(texcoords!=NULL)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(dimTexCoords, GL_FLOAT, 0, 0);
        }

        if(elements==NULL) glDrawArrays(GL_POINTS, 0, nVert);
        else               glDrawElements(type, nElements, GL_UNSIGNED_INT, 0);


                            glDisableClientState(GL_VERTEX_ARRAY);
        if(elements!=NULL)  glDisableClientState(GL_INDEX_ARRAY);
        if(texcoords!=NULL) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        //HdlVBO::unbind();
    }

// Static tools
    HdlVBO* HdlVBO::generate2DStandardQuad(void)
    {
        GLfloat vertices[]  = { 1.0,  1.0,
                                1.0, -1.0,
                               -1.0, -1.0,
                               -1.0,  1.0};
        GLuint elements[]   = { 0, 0, 0, 0};
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
        HdlGeBO::unbind(GL_INDEX_ARRAY);
        HdlGeBO::unbind(GL_TEXTURE_COORD_ARRAY);
    }

    void HdlVBO::unmap(void)
    {
        HdlGeBO::unmap(GL_ARRAY_BUFFER_ARB);
        HdlGeBO::unmap(GL_INDEX_ARRAY);
        HdlGeBO::unmap(GL_TEXTURE_COORD_ARRAY);
    }
