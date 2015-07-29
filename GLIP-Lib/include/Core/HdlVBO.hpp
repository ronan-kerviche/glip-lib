/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
	#include "Core/LibTools.hpp"
        #include "Core/OglInclude.hpp"
	#include "Core/HdlGeBO.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// Vertex Buffer Object Handle
			/**
			\class HdlVBO
			\brief Object handle for OpenGL Vertex Buffer Objects.
			**/
			class GLIP_API HdlVBO
			{
				private :
					int	nVert,
						nElements,
						dim,
						nIndPerElement,
						dimTexCoords;
					bool	hasNormals;
					HdlGeBO *vertices,
						*elements;
					GLintptr offsetVertices,
						offsetNormals,
						offsetTexCoords;
					GLenum 	type;

				public :
					HdlVBO(int _nVert, int _dim, GLenum freq, const GLfloat* _vertices, int _nElements=0, int _nIndPerElement=0, const GLuint* _elements=NULL, GLenum _type=GL_POINTS, const GLfloat* _normals=NULL, int _dimTexCoords=0, const GLfloat* _texcoords=NULL);
					~HdlVBO(void);

					int    getVerticesCount(void);
					int    getShapeDimension(void);
					int    getElementsCount(void);
					GLenum getType(void);
					void   draw(void);

					static void    unbind(void);
					static void    unmap(void);
			};
		}
	}

#endif

