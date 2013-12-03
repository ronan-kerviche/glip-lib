/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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

			// Constants
			#define NO_ATTACHMENT (-1)

			// FBO Handle
			/**
			\class HdlFBO
			\brief Object handle for OpenGL Frame Buffer Objects.

			This class can manage a Frame Buffer Object and multiple rendering target, each one of these rendering target have the same format.
			**/
			class GLIP_API HdlFBO : public __ReadOnly_HdlTextureFormat
			{
				private :
					// Data
					std::vector<HdlTexture*> targets;
					GLuint                   fboID;

					// Tools
					void bindTextureToFBO(int i);
					void unbindTextureFromFBO(int i);
					/*void clearList(void);
					void addTolist(GLenum attach);
					void removeFromList(GLenum attach);*/

				public :
					// Tools
					HdlFBO(const __ReadOnly_HdlTextureFormat& f, int numTarget = 1);
					~HdlFBO(void);

					int		addTarget(void);
					int		getAttachmentCount(void) const;

					void		beginRendering(int usedTarget=0);
					void		endRendering(void);
					void		bind(void);
					HdlTexture* 	operator[](int i);
					int		getSize(bool askDriver = false);

					// Static tools :
					static int    getMaximumColorAttachment(void);
					static GLenum getAttachment(int i);
					static int    getIndexFromAttachment(GLenum attachment);
					static void   unbind(void);
			};
		}
	}

#endif

