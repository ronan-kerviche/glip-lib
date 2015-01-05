/* ***************************************************************************************************************/
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
 * \date    August 7th 2010
*/

#include "Core/HdlFBO.hpp"
#include "Core/Exception.hpp"

using namespace Glip::CoreGL;

// Functions
	/**
	\fn    HdlFBO::HdlFBO(const HdlAbstractTextureFormat& f, int numTarget)
	\brief HdlFBO Construtor.

	\param f Format of the textures attached to the rendering point.
	\param numTarget Number of targets to be built by the constructor.
	**/
	HdlFBO::HdlFBO(const HdlAbstractTextureFormat& f, int numTarget)
	 : 	HdlAbstractTextureFormat(f),
		firstRendering(true)
	{
		NEED_EXTENSION(GL_ARB_framebuffer_object)
		FIX_MISSING_GLEW_CALL(glGenFramebuffers, glGenFramebuffersEXT)
		FIX_MISSING_GLEW_CALL(glBindFramebuffer, glBindFramebufferEXT)
		FIX_MISSING_GLEW_CALL(glDeleteFramebuffers, glDeleteFramebuffersEXT)
		FIX_MISSING_GLEW_CALL(glFramebufferTexture2D, glFramebufferTexture2DEXT)
		FIX_MISSING_GLEW_CALL(glDrawBuffers, glDrawBuffersARB)
		FIX_MISSING_GLEW_CALL(glGenerateMipmap, glGenerateMipmapEXT)

		if(isCompressed())
			throw Exception("HdlFBO::HdlFBO - Cannot render to compressed texture of format : " + glParamName(getGLMode()) + ".", __FILE__, __LINE__, Exception::GLException);

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlFBO::HdlFBO - Disabling Depth buffer." << std::endl;
		#endif

		glDisable(GL_DEPTH_TEST);

		glGenFramebuffers(1, &fboID);

		if(fboID==0)
			throw Exception("HdlFBO::HdlFBO - FBO can't be created. Last OpenGL error : " + glParamName(glGetError()) + ".", __FILE__, __LINE__, Exception::GLException);
		else
		{
			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlFBO::HdlFBO", "glGenFramebuffers(1, &fboID)")
			#endif
		}


		for(int i=0; i<numTarget; i++)
			addTarget();

		// check FBO status
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0); //unbind
	}

	HdlFBO::~HdlFBO(void)
	{
		// Remove all textures :
		for(unsigned int i=0; i<targets.size(); i++)
			unbindTextureFromFBO(i);

		glFlush();

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::~HdlFBO", "glFlush()")
		#endif

		glDeleteFramebuffers( 1, &fboID);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::~HdlFBO", "glDeleteFramebuffers()")
		#endif

		// Delete all textures :
		for(std::vector<HdlTexture*>::iterator it=targets.begin(); it!=targets.end(); it++)
			delete (*it);
	}

	void HdlFBO::bindTextureToFBO(int i)
	{
		// Entering safe zone :
		glGetError();

		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboID);

		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, getAttachment(i), GL_TEXTURE_2D, targets[i]->getID(), 0);

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlFBO::bindTextureToFBO - Texture can't be bound to the FBO, its format might be incompatible. (OpenGL error : " + glParamName(err) + ").", __FILE__, __LINE__, Exception::GLException);
	}

	void HdlFBO::unbindTextureFromFBO(int i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboID);

		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, getAttachment(i), GL_TEXTURE_2D, 0, 0);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::unbindTextureFromFBO", "glFramebufferTexture2D()")
		#endif
	}

	/**
	\fn    int HdlFBO::addTarget(void)
	\brief Add a new texture to target list.
	**/
	int HdlFBO::addTarget(void)
	{
		if(static_cast<int>(targets.size())>=getMaximumColorAttachment())
			throw Exception("HdlFBO::addTarget - Can't add more target, limit reached : " + toString(getMaximumColorAttachment()) + " textures.", __FILE__, __LINE__, Exception::GLException);
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
	\fn    void HdlFBO::beginRendering(int usedTarget)
	\param usedTarget The number of targets to be used. It must be greater or equal to 1 and less or equal to getAttachmentCount(). Default is 0 which means all targets.
	\brief Prepare the FBO for rendering in it
	**/
	void HdlFBO::beginRendering(int usedTarget)
	{
		static const GLenum attachmentsList[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT,
							GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT, GL_COLOR_ATTACHMENT8_EXT, GL_COLOR_ATTACHMENT9_EXT,
							GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT, GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT,GL_COLOR_ATTACHMENT14_EXT, GL_COLOR_ATTACHMENT15_EXT};


		if(usedTarget==0)
			usedTarget = targets.size();

		if(usedTarget>static_cast<int>(targets.size()))
			throw Exception("HdlFBO::beginRendering - Can't render to " + toString(usedTarget) + " textures because the current number of targets is " + toString(targets.size()), __FILE__, __LINE__, Exception::GLException);

		// First run test : 
		if(firstRendering) 
		{
			GLenum t = test();
			if(t!=GL_FRAMEBUFFER_COMPLETE)
				throw Exception("HdlFBO::beginRendering - FBO is incomplete, cannot render to target : " + glParamName(t) + ".", __FILE__, __LINE__, Exception::GLException);
			
			firstRendering = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboID);

		glDrawBuffers(usedTarget, attachmentsList);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::beginRendering", "glDrawBuffers()")
		#endif

		// Save viewport configuration
		glPushAttrib(GL_VIEWPORT_BIT);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::beginRendering", "glPushAttrib()")
		#endif

		// Create a new viewport configuration
		glViewport(0,0,getWidth(),getHeight());
	}

	/**
	\fn    void HdlFBO::endRendering(void)
	\brief End the rendering in the FBO, build the mipmaps for the target.
	**/
	void HdlFBO::endRendering(void)
	{
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0); // unbind

		// trigger mipmaps generation explicitly
		for(std::vector<HdlTexture*>::iterator it=targets.begin(); it!=targets.end(); it++)
		{
			if((*it)->getMaxLevel()>0)
			{
				glBindTexture(GL_TEXTURE_2D, (*it)->getID());

				glGenerateMipmap(GL_TEXTURE_2D);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("HdlFBO::endRendering", "glGenerateMipmap()")
				#endif
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// restore viewport setting
		glPopAttrib();

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::endRendering", "glPopAttrib()")
		#endif
	}

	/**
	\fn    HdlTexture* HdlFBO::operator[](int i)
	\brief Get a target of the FBO
	\param i Index of the target.
	\return Pointer to the corresponding target.
	**/
	HdlTexture* HdlFBO::operator[](int i)
	{
		if(i<0 || i>static_cast<int>(targets.size()))
			throw Exception("HdlFBO::operator[] - Invalid index : " + toString(i) + " of " + toString(targets.size()), __FILE__, __LINE__, Exception::CoreException);
		else
			return targets[i];
	}

	/**
	\fn    void HdlFBO::bind(void)
	\brief Bind this FBO.
	**/
	void HdlFBO::bind(void)
	{
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboID);
	}

	/**
	\fn     size_t HdlFBO::getSize(bool askDriver)
	\brief  Gets the size of the FBO in bytes.
	\param  askDriver If true, it will use HdlTexture::getSizeOnGPU() to determine the real size (might be slower).
	\return The size in byte of the multiple targets.
	**/
	size_t HdlFBO::getSize(bool askDriver)
	{
		if(!askDriver)
			return getAttachmentCount() * HdlAbstractTextureFormat::getSize();
		else
		{
			if(getAttachmentCount()>0)
				return getAttachmentCount() * targets.front()->getSizeOnGPU();
			else
				return 0;
		}
	}

	/**
	\fn     int HdlFBO::test(void)
	\brief  Test the validity of a FBO object
	\return An error code among : GL_FRAMEBUFFER_COMPLETE, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, GL_FRAMEBUFFER_UNSUPPORTED, GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS. See https://www.opengl.org/wiki/GLAPI/glCheckFramebufferStatus for more information.
	**/
	GLenum HdlFBO::test(void)
	{
		bind();

		GLenum returnCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		HdlFBO::unbind();

		return returnCode;
	}

	/**
	\fn     int HdlFBO::getMaximumColorAttachment(void)
	\brief  Get the maximum number of attachment points.
	\return The maximum number of attachment points.
	**/
	int HdlFBO::getMaximumColorAttachment(void)
	{
		GLint maxAttachments;

		glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttachments );

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::getMaximumColorAttachment", "glGetIntegerv()")
		#endif

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
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0); //unbind
	}

