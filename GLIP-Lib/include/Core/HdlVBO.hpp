/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlVBO.hpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Vertex Buffer Object Handle                                                        */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlVBO.hpp
 * \brief   OpenGL Pixel Buffer Object Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
*/

#ifndef __HDLVBO_INCLUDE__
#define __HDLVBO_INCLUDE__

	// GL include
        #include "OglInclude.hpp"
	#include "HdlGeBO.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Vertex Buffer Object Handle
			/**
			\class HdlVBO
			\brief Object handle for OpenGL Vertex Buffer Objects
			**/
			class HdlVBO
			{
				private :
					// Data
					int nVert, nElements, dim, nIndPerElement, dimTexCoords;
					HdlGeBO *vertices, *elements;
					GLenum type;
				public :
					// Tools
					HdlVBO(int _nVert, int _dim, GLenum freq, GLfloat* _vertices, int _nElements=0, int _nIndPerElement=0, GLuint* _elements=NULL, GLenum _type=GL_POINTS, int _dimTexCoords=0, GLfloat* _texcoords=NULL);
					// Type among {GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, }GL_POLYGON
					~HdlVBO(void);

					int    getVerticesCount(void);
					int    getShapeDimension(void);
					int    getElementsCount(void);
					GLenum getType(void);
					void   draw(void);

					// Static Tools
					static void    unbind(void);
					static void    unmap(void);
					static HdlVBO* generate2DStandardQuad(void);
					static HdlVBO* generate2DGrid(int w, int h, GLfloat appW=1.0, GLfloat appH=1.0, GLfloat cX=0.0, GLfloat cY=0.0);
					static HdlVBO* generate3DGrid(int w, int h, int d, GLfloat appW=1.0, GLfloat appH=1.0, GLfloat appD=1.0, GLfloat cX=0.0, GLfloat cY=0.0, GLfloat cZ=0.0);
			};
		}
	}

#endif

