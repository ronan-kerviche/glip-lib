/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : HdlFBO.hpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL FrameBuffer Object Handle                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlFBO.hpp
 * \brief   OpenGL FrameBuffer Object Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
*/

#ifndef __HDLFBO_INCLUDE__
#define __HDLFBO_INCLUDE__

	// Includes
        #include <vector>
	#include "Core/LibTools.hpp"
        #include "Core/HdlTexture.hpp"
        #include "Core/OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			// RenderBuffer Handle
			/**
			\class HdlRenderBuffer
			\brief Object handle for OpenGL Render Buffer Objects.

			This class can manage a Render Buffer Object.
			**/
			class GLIP_API HdlRenderBuffer
			{
				private : 
					GLuint	rboID;
					GLenum	internalFormat;
					int 	width,
						height;

					// No copy : 
					HdlRenderBuffer(const HdlRenderBuffer&);
					const HdlRenderBuffer& operator=(const HdlRenderBuffer&);

				public : 
					HdlRenderBuffer(const GLenum& _internalFormat, int _width, int _height);
					virtual ~HdlRenderBuffer(void);

					GLuint getID(void) const;
					GLenum getInternalFormat(void) const;
					int getWidth(void) const;
					int getHeight(void) const;
					void bind(void);
	
					static void unbind(void);
			};

			// FBO Handle
			/**
			\class HdlFBO
			\brief Object handle for OpenGL Frame Buffer Objects.

			This class can manage a Frame Buffer Object and multiple rendering target, each one of these rendering target have the same format.
			**/
			class GLIP_API HdlFBO : public HdlAbstractTextureFormat
			{
				private :
					// Data
					std::vector<HdlTexture*>	targets;
					GLuint				fboID;
					bool				firstRendering;
					HdlRenderBuffer*		depthBuffer;
					bool				depthBufferAttached;

					// Tools
					void bindTextureToFBO(int i);
					void unbindTextureFromFBO(int i);
				
					// No copy :
					HdlFBO(const HdlFBO&);
					const HdlFBO& operator=(const HdlFBO&);	

				public :
					// Tools
					HdlFBO(const HdlAbstractTextureFormat& f, int numTarget = 1);
					virtual ~HdlFBO(void);
					
					int		addTarget(void);
					int 		getNumTargets(void) const;
					int		getAttachmentCount(void) const;
					void		addDepthBuffer(void);
					bool		hasDepthBuffer(void);
					void		removeDepthBuffer(void);

					void		beginRendering(int usedTarget=0, bool useExistingDepthBuffer=false);
					void		endRendering(void);
					void		bind(void);
					HdlTexture* 	operator[](int i);
					size_t		getSize(bool askDriver = false);
					GLenum		test(void);

					// Static tools :
					static int    getMaximumColorAttachment(void);
					static GLenum getAttachment(int i);
					static int    getIndexFromAttachment(GLenum attachment);
					static void   unbind(void);
			};
		}
	}

#endif

