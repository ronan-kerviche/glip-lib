/* ***************************************************************************************************************/
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlFBO.cpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL FrameBuffer Object Handle                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlFBO.cpp
 * \brief   OpenGL FrameBuffer Object Handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 7th 2010
*/

#include "HdlFBO.hpp"
#include "./Exception.hpp"

using namespace Glip::CoreGL;

// Functions
	/**
	\fn    HdlFBO::HdlFBO(const __ReadOnly_HdlTextureFormat& f, int numTarget)
	\brief HdlFBO Construtor.

	\param f Format of the textures attached to the rendering point.
	\param numTarget Number of targets to be built by the constructor.
	**/
	HdlFBO::HdlFBO(const __ReadOnly_HdlTextureFormat& f, int numTarget)
	 : __ReadOnly_HdlTextureFormat(f)
	{
		HandleOpenGL::init();

		glGenFramebuffersEXT(1, &fboID);

		for(int i=0; i<numTarget; i++) // At least one!
			addTarget();

		// check FBO status
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
	}

	HdlFBO::~HdlFBO(void)
	{
		// Remove all textures :
		for(int i=0; i<targets.size(); i++)
			unbindTextureFromFBO(i);

		glFlush();
		glDeleteFramebuffersEXT( 1, &fboID);

		// Delete all textures :
		for(std::vector<HdlTexture*>::iterator it=targets.begin(); it!=targets.end(); it++)
			delete (*it);
	}

	void HdlFBO::bindTextureToFBO(int i)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, getAttachment(i), GL_TEXTURE_2D, targets[i]->getID(), 0);
	}

	void HdlFBO::unbindTextureFromFBO(int i)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, getAttachment(i), GL_TEXTURE_2D, 0, 0);
	}

	/**
	\fn    int HdlFBO::addTarget(void)
	\brief Add a new texture to target list.
	**/
	int HdlFBO::addTarget(void)
	{
		if(targets.size()>=getMaximumColorAttachment())
			throw Exception("HdlFBO::addTarget - Can't add more target, limit reached : " + to_string(getMaximumColorAttachment()) + " textures.", __FILE__, __LINE__);
		else
		{
			int i = targets.size();
			targets.push_back(new HdlTexture(*this));
			targets[i]->fill(0);
			bindTextureToFBO(i);
			return i;
		}
	}

	/**
	\fn    int HdlFBO::getAttachmentCount(void) const
	\brief Return the number of attachment points.

	\return The number of attached textures.
	**/
	int HdlFBO::getAttachmentCount(void) const
	{
		return targets.size();
	}

	/**
	\fn    void HdlFBO::beginRendering(void)
	\brief Prepare the FBO for rendering in it
	**/
	void HdlFBO::beginRendering(void)
	{
		const GLenum attachmentsList[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT,
						  GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT, GL_COLOR_ATTACHMENT8_EXT, GL_COLOR_ATTACHMENT9_EXT};
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);

		/*/ Set up color_tex and depth_rb for render-to-texture
		Useless : glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, assTex.getID(), 0);
		Useless : glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboID);*/

		glDrawBuffers(targets.size(), attachmentsList);

		// Save viewport configuration
		glPushAttrib(GL_VIEWPORT_BIT);

		// Create a new viewport configuration
		glViewport(0,0,getWidth(),getHeight());
	}

	/**
	\fn    void HdlFBO::endRendering(void)
	\brief End the rendering in the FBO, build the mipmaps for the target.
	**/
	void HdlFBO::endRendering(void)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // unbind

		// trigger mipmaps generation explicitly
		for(std::vector<HdlTexture*>::iterator it=targets.begin(); it!=targets.end(); it++)
		{
			glBindTexture(GL_TEXTURE_2D, (*it)->getID());
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		//useless : glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind Framebuffer -> render in the window

		// restore viewport setting
		glPopAttrib();
	}

	/**
	\fn    HdlTexture* HdlFBO::operator[](int i)
	\brief Get a target of the FBO
	\param i Index of the target.
	\return Pointer to the corresponding target.
	**/
	HdlTexture* HdlFBO::operator[](int i)
	{
		if(i<0 || i>targets.size())
			throw Exception("HdlFBO::operator[] - Invalid index : " + to_string(i) + " of " + to_string(targets.size()), __FILE__, __LINE__);
		else
			return targets[i];
	}

	/**
	\fn    void HdlFBO::bind(void)
	\brief Bind this FBO.
	**/
	void HdlFBO::bind(void)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
	}

	/**
	\fn    int HdlFBO::getMaximumColorAttachment(void)
	\brief Get the maximum number of attachment points.
	\return The maximum number of attachment points.
	**/
	int HdlFBO::getMaximumColorAttachment(void)
	{
		GLint maxAttachments;

		glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttachments );

		return maxAttachments;
	}

	/**
	\fn    GLenum HdlFBO::getAttachment(int i)
	\brief Convert an attachment ID to the corresponding OpenGL constant.
	\param i The ID of the attachment point.
	\return The corresponding OpenGL constant.
	**/
	GLenum HdlFBO::getAttachment(int i)
	{
		return GL_COLOR_ATTACHMENT0_EXT + i;
	}

	/**
	\fn    int HdlFBO::getIndexFromAttachment(GLenum attachment)
	\brief Convert an OpenGL constant to the corresponding attachment ID.
	\param attachment The OpenGL constant representing the attachment point.
	\return The corresponding ID.
	**/
	int HdlFBO::getIndexFromAttachment(GLenum attachment)
	{
		return attachment - GL_COLOR_ATTACHMENT0_EXT;
	}

	/**
	\fn    void HdlFBO::unbind(void)
	\brief Unbind the FBO which is currently bound to the OpenGL context.
	**/
	void HdlFBO::unbind(void)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //unbind
	}

