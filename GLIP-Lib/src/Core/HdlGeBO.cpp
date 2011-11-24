/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlGeBO.cpp                                                                               */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlGeBO.cpp
 * \brief   OpenGL Generic handle for Buffer Objects (VBO, PBO, VAO, ...)
 * \author  R. KERVICHE
 * \version 0.6
 * \date    February 6th 2011
*/

#include "HdlGeBO.hpp"
#include "../include/GLIPLib.hpp"

using namespace Glip::CoreGL;

// Functions
    /**
     \fn    HdlGeBO::HdlGeBO(unsigned int _size, GLenum infoTarget, GLenum infoUsage) : size(_size)
     \brief HdlGeBO Construtor.

     \param _size      Size of the buffer, in bytes.
     \param infoTarget Target kind, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
     \param infoUsage  Usage kind among GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
    **/
    HdlGeBO::HdlGeBO(unsigned int _size, GLenum infoTarget, GLenum infoUsage) : size(_size)
    {
        // Generate the buffer
        glGenBuffers(1, &bufferId);

        // Bind it
        glBindBuffer(infoTarget, bufferId);

        // Allocate some space
        glBufferData(infoTarget, size, NULL, infoUsage);

        // Release point
        HdlGeBO::unbind(infoTarget);

        buildTarget = infoTarget;
        buildUsage  = infoUsage;
    }

    /**
     \fn    HdlGeBO::HdlGeBO(GLuint id, unsigned int _size) : size(_size)
     \brief HdlGeBO Construtor.

     \param id    GLuint id of the Buffer Object to be mapped in.
     \param _size Size of the buffer, in bytes.
     \param infoTarget Target kind, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
     \param infoUsage  Usage kind among GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
    **/
    HdlGeBO::HdlGeBO(GLuint id, unsigned int _size, GLenum infoTarget, GLenum infoUsage) : size(_size)
    {
        // Just copy the link
        bufferId = id;

        // And data
        buildTarget = infoTarget;
        buildUsage  = infoUsage;
    }

    HdlGeBO::~HdlGeBO(void)
    {
        // Delete the object
        glDeleteBuffers(1, &bufferId);
    }

    /**
     \fn    unsigned int HdlGeBO::getSize(void)
     \brief Get the size of the Buffer Object.

     \return Size of the BO in bytes.
    **/
    unsigned int HdlGeBO::getSize(void)
    {
        return size;
    }

    /**
     \fn    GLuint HdlGeBO::getID(void)
     \brief Get the ID of the Buffer Object.

     \return ID of the Buffer Object
    **/
    GLuint HdlGeBO::getID(void)
    {
        return bufferId;
    }

    /**
     \fn    GLenum HdlGeBO::getTarget(void)
     \brief Get the target of the Buffer Object.

     \return Target of the Buffer Object, among : GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
    **/
    GLenum HdlGeBO::getTarget(void)
    {
        return buildTarget;
    }

    /**
     \fn    GLenum HdlGeBO::getUsage(void)
     \brief Get the usage of the Buffer Object.

     \return ID of the Buffer Object, among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
    **/
    GLenum HdlGeBO::getUsage(void)
    {
        return buildUsage;
    }

    /**
     \fn    void HdlGeBO::bind(GLenum target)
     \brief Bind the Buffer Object.
    **/
    void HdlGeBO::bind(GLenum target)
    {
        glBindBuffer(target, bufferId);
    }

    /**
     \fn    void* HdlGeBO::map(GLenum target, GLenum access)
     \brief Map the Buffer Object into the CPU memory.

     \param target Target mapping point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
     \param access Kind of access, among GL_READ_ONLY_ARB, GL_WRITE_ONLY_ARB, GL_READ_WRITE_ARB.

     \return Pointer in CPU memory.
    **/
    void* HdlGeBO::map(GLenum target, GLenum access)
    {
        HdlGeBO::unmap(target);
        bind(target);
        return glMapBufferARB(target, access);
    }

// Static tools
    /**
     \fn    void HdlGeBO::unbind(GLenum target)
     \brief Unbind any Buffer Object.

     \param target Target binding point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
    **/
    void HdlGeBO::unbind(GLenum target)
    {
        glBindBuffer(target, 0);
    }

    /**
     \fn    void HdlGeBO::unbind(GLenum target)
     \brief Unmap any Buffer Object.

     \param target Target binding point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
    **/
    void HdlGeBO::unmap(GLenum target)
    {
        glUnmapBufferARB(target);
    }

    /*
    std::string HdlGeBO::getTargetName(GLenum target)
    {
        #define NAMEIT(X) case X : return #X;
        switch(target)
        {
            NAMEIT( GL_ARRAY_BUFFER_ARB )
            NAMEIT( GL_ELEMENT_ARRAY_BUFFER_ARB )
            NAMEIT( GL_PIXEL_UNPACK_BUFFER_ARB )
            NAMEIT( GL_PIXEL_PACK_BUFFER_ARB )
            default : return "Taget not recognized";
        }
        #undef NAMEIT
    }

    std::string HdlGeBO::getUsageName(GLenum usage)
    {
        #define NAMEIT(X) case X : return #X;
        switch(usage)
        {
            NAMEIT( GL_STATIC_DRAW_ARB )
            NAMEIT( GL_STATIC_READ_ARB )
            NAMEIT( GL_STATIC_COPY_ARB )
            NAMEIT( GL_DYNAMIC_DRAW_ARB )
            NAMEIT( GL_DYNAMIC_READ_ARB )
            NAMEIT( GL_DYNAMIC_COPY_ARB )
            NAMEIT( GL_STREAM_DRAW_ARB )
            NAMEIT( GL_STREAM_READ_ARB )
            NAMEIT( GL_STREAM_COPY_ARB )
            default : return "Usage not recognized";
        }
        #undef NAMEIT
    }
    */
