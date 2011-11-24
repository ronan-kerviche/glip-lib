/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlPBO.hpp                                                                                */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlPBO.hpp
 * \brief   OpenGL Pixel Buffer Object Handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 7th 2010
*/

/* ************************************************************************************************************* */
/*                                                                                                               */
/*         Tips : UNPACK <-> UPLOAD   Data to   GPU                                                              */
/*                PACK   <-> DOWNLOAD Data from GPU                                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __HDLPBO_INCLUDE__
#define __HDLPBO_INCLUDE__

    // GL include
        #include "OglInclude.hpp"
		#include "HdlGeBO.hpp"

namespace Glip
{
    namespace CoreGL
    {

        // prototypes :
            class HdlTexture;
            class HdlFBO;

        // Pixel Buffer Object Handle
        class HdlPBO : public HdlGeBO
        {
            private :
                // Data
                    int w, h, c, cs;
            public :
                // Tools
                    HdlPBO(int _w, int _h, int _c, int _cs, GLenum aim, GLenum freq);
                    ~HdlPBO(void);

                    int  getWidth(void);
                    int  getHeight(void);
                    int  getChannelCount(void);
                    int  getChannelSize(void);
                    void copyToTexture(HdlTexture& texture, GLenum mode, GLenum depth, int oX=0, int oY=0, int w=-1, int h=-1);

                //Static tools
                    static void unbind(GLenum target=0);
                    static void unmap(GLenum target=0);
        };

        /*enum
        {
            PBO_NOMODE        = 0x00000000,
            PBO_PACK          = 0x00000001,
            PBO_UNPACK        = 0x00000010,
            PBO_PACK_MAPPED   = 0x00000100,
            PBO_UNPACK_MAPPED = 0x00001000,
            PBO_UPLOAD        = 0x10000000,
            PBO_DOWNLOAD      = 0x01000000,
            PBO_BOTHWAY       = 0x11000000,
            PBO_MASK          = 0x11111111
        };

        // PBO Handle
        class HdlPBO
        {
            protected :
                // Data
                    int imgW, imgH, imgC, byteSize, imgSize;
                    GLuint texBuffer; // Buffer Index for OGL
                    GLenum usage;
                    int aim;
                    static int workingMode;

                    friend void unbindPBO(int);
                    friend void readActivePBO(bool);

            public :
                // Tools
                    HdlPBO(int _aim, int w, int h, int channels, int Bsize = 1); // Bsize : taille en octet du support (char -> 1 octet!)
                    ~HdlPBO(void);

                    int getImgW(void);
                    int getImgH(void);
                    int getImgC(void);
                    int getImgSize(void);

                    void  copyToTexture(GLuint texInd,   int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);
                    void  copyToTexture(HdlTexture& tex, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);
                        /// The Texture will be bound after the call
                        /// The PBO will be bound after the call

                    void  copyFromTexture(HdlTexture& tex, GLenum mode, GLenum depth);

                    void* getData(void);
                    void* readData(void);
                        /// The modification will be stored only if unbindPBO() is called after that
                        /// The PBO will be bound after the call
                        /// The PBO will be maped after the call

                    void  bind(void);

                    void* readBuffer(GLenum bufferInd, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);
                        /// The optionnal modifications on the data located by the ouput buffer will be stored only if unbindPBO() is called after that
                        /// The read buffer will be bufferInd after this call
                        /// The PBO will be bound after the call
                        /// The PBO will be maped after the call

                    //void  onlyReadFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);
                    //void  onlyReadFBO2(HdlFBO& fbo, GLenum mode, GLenum depth);
                    //void* readFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);

                    //void  startReadingFBO(HdlFBO& fbo, int offsetX, int offsetY, int w, int h, GLenum mode, GLenum depth);
                    //void* endReadingFBO(void);
        };

        // General PBO tools
        void unbindPBO(int mode = PBO_BOTHWAY);
        void readActivePBO(bool bin = false);*/
    }
}

#endif

