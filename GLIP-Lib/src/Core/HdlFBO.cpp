/* ***************************************************************************************************************/
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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

// HdlRenderBuffer
	/**
	\fn HdlRenderBuffer::HdlRenderBuffer(const GLenum& _internalFormat, int _width, int _height)
	\brief Constructor.
	\param _internalFormat Internal format of this render buffer (GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX, GL_STENCIL_INDEX8, ...).
	\param _width Width of the buffer.
	\param _height Height of the buffer.
	**/
	HdlRenderBuffer::HdlRenderBuffer(const GLenum& _internalFormat, int _width, int _height)
	 : 	rboID(0),
		internalFormat(_internalFormat),
		width(_width),
		height(_height)
	{
		glGenRenderbuffers(1, &rboID);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlRenderBuffer::HdlRenderBuffer", "glGenRenderbuffers()")
		#endif

		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlRenderBuffer::HdlRenderBuffer", "glRenderbufferStorage()")
		#endif
		unbind();
	}

	HdlRenderBuffer::~HdlRenderBuffer(void)
	{
		glDeleteRenderbuffers(1, &rboID);		
	}

	/**
	\fn GLuint HdlRenderBuffer::getID(void) const
	\brief Get the ID of the render buffer object.
	\return The GL id of this objet.
	**/
	GLuint HdlRenderBuffer::getID(void) const
	{
		return rboID;
	}

	/**
	\fn GLenum HdlRenderBuffer::getInternalFormat(void) const
	\brief Get the internal format of this render buffer.
	\return The internal format of this object (GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX, GL_STENCIL_INDEX8, ...).
	**/
	GLenum HdlRenderBuffer::getInternalFormat(void) const
	{
		return internalFormat;
	}

	/**
	\fn int HdlRenderBuffer::getWidth(void) const
	\brief Get the width of this render buffer object.
	\return The width of this object.
	**/
	int HdlRenderBuffer::getWidth(void) const
	{
		return width;
	}

	/**
	\fn int HdlRenderBuffer::getHeight(void) const
	\brief Get the height of this render buffer object.
	\return The height of this object.
	**/
	int HdlRenderBuffer::getHeight(void) const
	{
		return height;
	}

	/**
	\fn void HdlRenderBuffer::bind(void)
	\brief Bind this render buffer object.
	**/
	void HdlRenderBuffer::bind(void)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, rboID);
	}
	
	/**
	\fn void HdlRenderBuffer::unbind(void)
	\brief Unbind any render buffer object.
	**/
	void HdlRenderBuffer::unbind(void)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

// HdlFBO
	/**
	\fn    HdlFBO::HdlFBO(const HdlAbstractTextureFormat& f, int numTarget)
	\brief HdlFBO Construtor.
	\param f Format of the textures attached to the rendering point.
	\param numTarget Number of targets to be built by the constructor.
	**/
	HdlFBO::HdlFBO(const HdlAbstractTextureFormat& f, int numTarget)
	 : 	HdlAbstractTextureFormat(f),
		fboID(0),
		firstRendering(true),
		depthBuffer(NULL),
		depthBufferAttached(false)
	{
		#ifdef GLIP_USE_GL
		NEED_EXTENSION(GL_ARB_framebuffer_object)
		FIX_MISSING_GLEW_CALL(glGenFramebuffers, glGenFramebuffersEXT)
		FIX_MISSING_GLEW_CALL(glBindFramebuffer, glBindFramebufferEXT)
		FIX_MISSING_GLEW_CALL(glDeleteFramebuffers, glDeleteFramebuffersEXT)
		FIX_MISSING_GLEW_CALL(glFramebufferTexture2D, glFramebufferTexture2DEXT)
		FIX_MISSING_GLEW_CALL(glDrawBuffers, glDrawBuffersARB)
		FIX_MISSING_GLEW_CALL(glGenerateMipmap, glGenerateMipmapEXT)
		#endif

		if(isCompressed())
			throw Exception("HdlFBO::HdlFBO - Cannot render to compressed texture of format : " + getGLEnumNameSafe(getGLMode()) + ".", __FILE__, __LINE__, Exception::GLException);

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlFBO::HdlFBO - Disabling Depth buffer." << std::endl;
		#endif

		glDisable(GL_DEPTH_TEST);

		glGenFramebuffers(1, &fboID);

		if(fboID==0)
		{
			GLenum err = glGetError();
			throw Exception("HdlFBO::HdlFBO - FBO can't be created. Last OpenGL error : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
		}
		else
		{
			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlFBO::HdlFBO", "glGenFramebuffers(1, &fboID)")
			#endif
		}


		for(int i=0; i<numTarget; i++)
			addTarget();

		// check FBO status
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
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

		delete depthBuffer;
	}

	void HdlFBO::bindTextureToFBO(int i)
	{
		// Entering safe zone :
		glGetError();

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		glFramebufferTexture2D(GL_FRAMEBUFFER, getAttachment(i), GL_TEXTURE_2D, targets[i]->getID(), 0);

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlFBO::bindTextureToFBO - Texture can't be bound to the FBO, its format might be incompatible. OpenGL error " + getGLEnumNameSafe(err) + " : " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	void HdlFBO::unbindTextureFromFBO(int i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		glFramebufferTexture2D(GL_FRAMEBUFFER, getAttachment(i), GL_TEXTURE_2D, 0, 0);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::unbindTextureFromFBO", "glFramebufferTexture2D()")
		#endif
	}

	/**
	\fn int HdlFBO::addTarget(void)
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
	\fn int HdlFBO::getNumTargets(void) const
	\brief Get the number of targets.
	\return The current number of targets.
	**/
	int HdlFBO::getNumTargets(void) const
	{
		return targets.size();
	}

	/**
	\fn int HdlFBO::getAttachmentCount(void) const
	\brief Return the number of attachment points.
	\return The number of attached textures.
	**/
	int HdlFBO::getAttachmentCount(void) const
	{
		return targets.size();
	}

	/**
	\fn void HdlFBO::addDepthBuffer(void)
	\brief Add a depth buffer to this Framebuffer object.
	**/
	void HdlFBO::addDepthBuffer(void)
	{
		if(depthBuffer==NULL)
			depthBuffer = new HdlRenderBuffer(GL_DEPTH_COMPONENT, getWidth(), getHeight());
	}

	/**
	\fn bool HdlFBO::hasDepthBuffer(void)
	\brief Test if a depth buffer is currently attached to this object.
	\return True if a depth buffer is currently attached.
	**/
	bool HdlFBO::hasDepthBuffer(void)
	{
		return (depthBuffer!=NULL);
	}

	/**
	\fn void HdlFBO::removeDepthBuffer(void)
	\brief Remove the depth buffer currently attached to this frame buffer.
	**/
	void HdlFBO::removeDepthBuffer(void)
	{
		if(depthBuffer!=NULL)
		{
			bind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			delete depthBuffer;
			depthBuffer = NULL;
		}
	}

	/**
	\fn void HdlFBO::beginRendering(int usedTarget, bool useExistingDepthBuffer)
	\param usedTarget The number of targets to be used. It must be greater or equal to 1 and less or equal to getAttachmentCount(). Default is 0 which means all targets.
	\brief Prepare the FBO for rendering in it.
	\param usedTarget Number of targets used in this FBO.
	\param useExistingDepthBuffer If true and if a depth buffer is currently attached, it will be used (see HdlFBO::addDepthBuffer).
	**/
	void HdlFBO::beginRendering(int usedTarget, bool useExistingDepthBuffer)
	{
		static const GLenum attachmentsList[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
							GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
							GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15};


		if(usedTarget==0)
			usedTarget = targets.size();

		if(usedTarget>static_cast<int>(targets.size()))
			throw Exception("HdlFBO::beginRendering - Can't render to " + toString(usedTarget) + " textures because the current number of targets is " + toString(targets.size()), __FILE__, __LINE__, Exception::GLException);

		// First run test : 
		if(firstRendering) 
		{
			GLenum t = test();
			if(t!=GL_FRAMEBUFFER_COMPLETE)
				throw Exception("HdlFBO::beginRendering - FBO is incomplete, cannot render to target : " + getGLEnumNameSafe(t) + ".", __FILE__, __LINE__, Exception::GLException);
			
			firstRendering = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		// Attach a depth buffer (uncommon?) : 
		if(depthBuffer!=NULL && useExistingDepthBuffer)
		{	
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer->getID());
			depthBufferAttached = true;
		}

		glDrawBuffers(usedTarget, attachmentsList);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::beginRendering", "glDrawBuffers()")
		#endif

		// Save viewport configuration (removed for GLES compatibility, impact?) :
		//glPushAttrib(GL_VIEWPORT_BIT);

		//#ifdef __GLIPLIB_TRACK_GL_ERRORS__
		//	OPENGL_ERROR_TRACKER("HdlFBO::beginRendering", "glPushAttrib()")
		//#endif

		// Create a new viewport configuration
		glViewport(0,0,getWidth(),getHeight());
	}

	/**
	\fn void HdlFBO::endRendering(void)
	\brief End the rendering in the FBO, build the mipmaps for the target.
	**/
	void HdlFBO::endRendering(void)
	{
		if(depthBufferAttached)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			depthBufferAttached = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind

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

		// restore viewport setting (removed for GLES compatibility, impact?) :
		//glPopAttrib();

		//#ifdef __GLIPLIB_TRACK_GL_ERRORS__
		//	OPENGL_ERROR_TRACKER("HdlFBO::endRendering", "glPopAttrib()")
		//#endif
	}

	/**
	\fn HdlTexture* HdlFBO::operator[](int i)
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
	\fn void HdlFBO::bind(void)
	\brief Bind this FBO.
	**/
	void HdlFBO::bind(void)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	}

	/**
	\fn size_t HdlFBO::getSize(bool askDriver)
	\brief Gets the size of the FBO in bytes.
	\param askDriver If true, it will use HdlTexture::getSizeOnGPU() to determine the real size (might be slower).
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
	\fn int HdlFBO::test(void)
	\brief Test the validity of a FBO object
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
	\fn int HdlFBO::getMaximumColorAttachment(void)
	\brief Get the maximum number of attachment points.
	\return The maximum number of attachment points.
	**/
	int HdlFBO::getMaximumColorAttachment(void)
	{
		GLint maxAttachments;

		glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &maxAttachments );

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlFBO::getMaximumColorAttachment", "glGetIntegerv()")
		#endif

		return maxAttachments;
	}

	/**
	\fn GLenum HdlFBO::getAttachment(int i)
	\brief Convert an attachment ID to the corresponding OpenGL constant.
	\param i The ID of the attachment point.
	\return The corresponding OpenGL constant.
	**/
	GLenum HdlFBO::getAttachment(int i)
	{
		return GL_COLOR_ATTACHMENT0 + i;
	}

	/**
	\fn int HdlFBO::getIndexFromAttachment(GLenum attachment)
	\brief Convert an OpenGL constant to the corresponding attachment ID.
	\param attachment The OpenGL constant representing the attachment point.
	\return The corresponding ID.
	**/
	int HdlFBO::getIndexFromAttachment(GLenum attachment)
	{
		return attachment - GL_COLOR_ATTACHMENT0;
	}

	/**
	\fn void HdlFBO::unbind(void)
	\brief Unbind the FBO which is currently bound to the OpenGL context.
	**/
	void HdlFBO::unbind(void)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
	}

