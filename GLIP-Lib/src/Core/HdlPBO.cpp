/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlPBO.cpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlPBO.cpp
 * \brief   OpenGL Pixel Buffer Object Handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 7th 2010
*/

#include "HdlPBO.hpp"
#include "../include/GLIPLib.hpp"

using namespace Glip::CoreGL;

// Tools
	/**
	\fn HdlPBO::HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq)
	\brief HdlPBO constructor.
	\param _w The width of the buffer.
	\param _h The height of the buffer.
	\param _c The number of channels.
	\param _cs The size of each channel (in bytes).
	\param aim The target (GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB).
	\param freq The frequency (GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB).
	**/
	HdlPBO::HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq)
	 : h(_h), w(_w), c(_c), cs(_cs), HdlGeBO(_w*_h*_c*_cs, aim, freq)
	{ }

	HdlPBO::~HdlPBO(void)
	{ }

	/**
	\fn int HdlPBO::getWidth(void)
	\return The width of the buffer.
	\fn int HdlPBO::getHeight(void)
	\return The height of the buffer.
	\fn int HdlPBO::getChannelCount(void)
	\return The number of channels of the buffer.
	\fn int HdlPBO::getChannelSize(void)
	\return The channel size (in bytes) of the buffer.
	**/
	int HdlPBO::getWidth(void)        { return w;  }
	int HdlPBO::getHeight(void)       { return h;  }
	int HdlPBO::getChannelCount(void) { return c;  }
	int HdlPBO::getChannelSize(void)  { return cs; }

	/**
	\fn void HdlPBO::copyToTexture(HdlTexture& texture, GLenum mode, GLenum depth, int oX, int oY, int w, int h)
	\param texture The target texture.
	\param mode The channel layout of the input data.
	\param depth The depth of the input data.
	\param oX The offset in X direction for the destination target.
	\param oY The offset in Y direction for the destination target.
	\param w The width to be copied.
	\param h The height to be copied.
	**/
	void HdlPBO::copyToTexture(HdlTexture& texture, GLenum mode, GLenum depth, int oX, int oY, int w, int h)
	{
		if(w<=0) w = texture.getWidth();
		if(h<=0) h = texture.getHeight();

		texture.bind();
		bind(GL_PIXEL_UNPACK_BUFFER_ARB);

		glTexSubImage2D(GL_TEXTURE_2D, 0, oX, oY, w, h, mode, depth, 0);
	}

//Other tools
	/**
	\fn void HdlPBO::unbind(GLenum target)
	\brief Unbind the PBO bound to target.
	\param target The target binding point.
	**/
	void HdlPBO::unbind(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unbind(GL_PIXEL_UNPACK_BUFFER_ARB);
			HdlGeBO::unbind(GL_PIXEL_PACK_BUFFER_ARB  );
		}
		else
			HdlGeBO::unbind(target);
	}

	/**
	\fn void HdlPBO::unmap(GLenum target)
	\brief Unmap the PBO mapped from/to target.
	\param target The target binding point.
	**/
	void HdlPBO::unmap(GLenum target)
	{
		if(target==0)
		{
			HdlGeBO::unmap(GL_PIXEL_UNPACK_BUFFER_ARB);
			HdlGeBO::unmap(GL_PIXEL_PACK_BUFFER_ARB  );
		}
		else
			HdlGeBO::unmap(target);
	}

/*
// Data
    int HdlPBO::workingMode = PBO_NOMODE;

// Functions
    HdlPBO::HdlPBO(int _aim, int w, int h, int channels, int Bsize)
    {
        HandleOpenGL::init();

        // Init
        imgW     = w;
        imgH     = h;
        imgC     = channels;
        byteSize = Bsize;
        imgSize  = imgW*imgH*imgC*Bsize;
        aim      = _aim;

        // Generate the Buffer for streaming
        glGenBuffersARB(1, &texBuffer);
        /// SEEMS NOT USEFULL
        //glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);
        //glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, imgSize, 0, usage);

        //Unbind it
        //glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

        if(aim==PBO_UPLOAD)
        {
            usage = GL_STREAM_DRAW;

            //allocating the data
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, imgSize, 0, usage);
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

            return ;
        }

        if(aim==PBO_DOWNLOAD)
        {
            usage = GL_STREAM_READ;

            //allocating the data
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
            glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);

            return ;
        }

        std::cout << "PBO working situation not recognized on constructor" << std::endl;

    }

    HdlPBO::~HdlPBO(void)
    {
        unbindPBO();
        glDeleteBuffersARB(1, &texBuffer);
    }

    int HdlPBO::getImgW(void)
    {
        return imgW;
    }

    int HdlPBO::getImgH(void)
    {
        return imgH;
    }

    int HdlPBO::getImgC(void)
    {
        return imgC;
    }

    int HdlPBO::getImgSize(void)
    {
        return imgSize;
    }

    void HdlPBO::copyToTexture(GLuint texInd, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_DOWNLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        glBindTexture(GL_TEXTURE_2D, texInd);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);

        // copy pixels from PBO to texture object
        // Use offset instead of pointer.
        /// MODIFICATION : GL_RGB means imgC = 3, update this
        glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, w, h, mode, depth, 0);

        workingMode = workingMode | PBO_UNPACK;
    }

    void HdlPBO::copyToTexture(HdlTexture& tex, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_DOWNLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        glBindTexture(GL_TEXTURE_2D, tex.getID());
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);

        // copy pixels from PBO to texture object
        // Use offset instead of pointer.
        /// MODIFICATION : GL_RGB means imgC = 3, update this
        glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, offsetY, w, h, mode, depth, 0);

        workingMode = workingMode | PBO_UNPACK;
    }

    void* HdlPBO::getData(void)
    {
        if(aim==PBO_DOWNLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, texBuffer);

        // map the buffer object into client's memory
        // Note that glMapBufferARB() causes sync issue.
            // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
            // for GPU to finish its job. To avoid waiting (stall), you can call
            // first glBufferDataARB() with NULL pointer before glMapBufferARB().
            // If you do that, the previous data in PBO will be discarded and
            // glMapBufferARB() returns a new allocated pointer immediately
            // even if GPU is still working with the previous data.
        ///glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, imgSize, 0, usage);
        void* ptr = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

        workingMode = workingMode | PBO_UNPACK | PBO_UNPACK_MAPPED;

        return ptr; /// CAST PREFERENCE : (GLubyte*)
                    /// ptr could be NULL
                    /// The modification will be stored only if unbindPBO() is called after that
    }

    void* HdlPBO::readData(void)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        ///glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);
        void* ptr = glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

        workingMode = workingMode | PBO_PACK | PBO_PACK_MAPPED;

        return ptr; /// CAST PREFERENCE : (GLubyte*)
                    /// ptr could be NULL
                    /// The modification will be stored only if unbindPBO() is called after that
    }

    void* HdlPBO::readBuffer(GLenum bufferInd, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        /// Verification : OK

        // set the target framebuffer to read
        glReadBuffer(bufferInd);

        // read pixels from framebuffer to PBO
        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        glReadPixels( offsetX, offsetY, w, h, mode, depth, 0);

        //Send back the location of the data
        /// Seems not useful : glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
        ///glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);
        void* ptr = glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

        workingMode = workingMode | PBO_PACK | PBO_PACK_MAPPED;

        return ptr; /// CAST PREFERENCE : (GLubyte*)
                    /// ptr could be NULL
                    /// The modification will be stored only if unbindPBO() is called after that
    }

    /*void HdlPBO::onlyReadFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        //GLenum bufAttachment = fbo.getColAttachment();

        //bind the FBO
        fbo.bind();

        ///useless glReadBuffer(bufAttachment);
        glReadBuffer(fbo.getColAttachment());

        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        // read pixels from framebuffer to PBO
        // glReadPixels() should return immediately.
        glReadPixels( offsetX, offsetY, w, h, mode, depth, 0);

        //Send back the location of the data
        /// Seems not useful : glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
        /// Seems not useful : glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);

        workingMode = workingMode | PBO_PACK;

        return ;
    }

    void HdlPBO::onlyReadFBO2(HdlFBO& fbo, GLenum mode, GLenum depth)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        //GLenum bufAttachment = fbo.getColAttachment();

        //bind the FBO
        fbo.bindTexture();

        ///useless glReadBuffer(bufAttachment);
        glReadBuffer(fbo.getColAttachment());

        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        // read pixels from framebuffer to PBO
        // glReadPixels() should return immediately.
        glGetTexImage(GL_TEXTURE_2D, 0, mode, depth, 0);

        //Send back the location of the data
        /// Seems not useful : glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
        /// Seems not useful : glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);

        workingMode = workingMode | PBO_PACK;

        return ;
    }

    void* HdlPBO::readFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        GLenum bufAttachment = fbo.getColAttachment();

        //bind the FBO
        fbo.bind();

        glReadBuffer(bufAttachment);

        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        // read pixels from framebuffer to PBO
        // glReadPixels() should return immediately.
        glReadPixels( offsetX, offsetY, w, h, mode, depth, 0);

        //Send back the location of the data
        /// Seems not useful : glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
        ///glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);

        void* ptr = glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

        workingMode = workingMode | PBO_PACK | PBO_PACK_MAPPED;

        return ptr;
    }

    void HdlPBO::startReadingFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        GLenum bufAttachment = fbo.getColAttachment();

        //bind the FBO
        fbo.bind();

        glReadBuffer(bufAttachment);

        // read pixels from framebuffer to PBO
        // glReadPixels() should return immediately.
        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        glReadPixels( offsetX, offsetY, w, h, mode, depth, 0);

        workingMode = workingMode | PBO_PACK;
    }

    void* HdlPBO::endReadingFBO(void)
    {
        if(aim==PBO_UPLOAD) std::cout << "You are using the PBO the wrong way" << std::endl;

        //Send back the location of the data
        /// Seems not useful : glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);
        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, texBuffer);

        ///glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, imgSize, 0, usage);

        void* ptr = glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

        workingMode = workingMode | PBO_PACK | PBO_PACK_MAPPED;

        return ptr;
    }

// General PBO tools
    void unbindPBO(int mode)
    {
        // From GPU to CPU
        if((mode & PBO_DOWNLOAD) && (HdlPBO::workingMode & PBO_PACK) )
        {
            if( HdlPBO::workingMode & PBO_PACK_MAPPED ) //we need to unmap the content
            {
                glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
                HdlPBO::workingMode = HdlPBO::workingMode & (~PBO_PACK_MAPPED); //reset
            }

            // now we can unbind :
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);

            //reset
            HdlPBO::workingMode = HdlPBO::workingMode & (~PBO_PACK);
        }

        glErrors(false);

        // From CPU to GPU
        if((mode & PBO_UPLOAD) && (HdlPBO::workingMode & PBO_UNPACK) )
        {
            if( HdlPBO::workingMode & PBO_UNPACK_MAPPED ) //we need to unmap the content
            {
                glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
                HdlPBO::workingMode = HdlPBO::workingMode & (~PBO_UNPACK_MAPPED); //reset
                //mode -= PBO_UNPACK_MAPPED;
            }

            // now we can unbind :
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

            //reset
            HdlPBO::workingMode = HdlPBO::workingMode & (~PBO_UNPACK);
        }

        // clean end!
        HdlPBO::workingMode = HdlPBO::workingMode & PBO_MASK;
    }

    void readActivePBO(bool bin)
    {
        std::cout << "Reading active PBO states : " << std::endl;

        if(bin)
        {
            std::cout << "Binaries : |";
            for(unsigned int i = 0x10000000; i>=1; i/=16)
                std::cout << ((HdlPBO::workingMode & i)>0); //to get a 0 or 1
            std::cout << '|' << std::endl;
        }

        if(HdlPBO::workingMode & PBO_PACK)
        {
            std::cout << "  PBO - Packing/Downloading ";
            if(HdlPBO::workingMode & PBO_PACK_MAPPED) std::cout << "(ressource mapped)";
            std::cout << std::endl;
        }
        else
            if(HdlPBO::workingMode & PBO_PACK_MAPPED) std::cout << "  ERROR - ressource still mapped but not bound anymore (for downloading)" << std::endl;

        if(HdlPBO::workingMode & PBO_UNPACK)
        {
            std::cout << "  PBO - Unpacking/Uploading ";
            if(HdlPBO::workingMode & PBO_UNPACK_MAPPED) std::cout << "(ressource mapped)";
            std::cout << std::endl;
        }
        else
            if(HdlPBO::workingMode & PBO_UNPACK_MAPPED) std::cout << "  ERROR - ressource still mapped but not bound anymore (for uploading)" << std::endl;

        if( HdlPBO::workingMode & PBO_BOTHWAY )
            std::cout << "  ERROR - transfering mode bit set outside unbindPBO!" << std::endl;

        if( HdlPBO::workingMode & (~PBO_MASK) )
            std::cout << "  ERROR - bit not attributed but set anyway!" << std::endl;

        std::cout << "End reading" << std::endl;
    }

*/
