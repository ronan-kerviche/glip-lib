/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTexture.hpp                                                                            */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Texture Handle                                                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlTexture.hpp
 * \brief   OpenGL Texture Handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 7th 2010
**/

#ifndef __HDLTEXTURE_INCLUDE__
#define __HDLTEXTURE_INCLUDE__

// Include
    #include "OglInclude.hpp"

namespace Glip
{
    namespace CoreGL
    {

        // Texture Format Handle
                /**
                 \class __ReadOnly_HdlTextureFormat
                 \brief Object handle for OpenGL texture formats (Read Only!)
                **/
            class __ReadOnly_HdlTextureFormat
            {
                protected :
                    // Data
                        int    imgW, imgH, imgC, colSize, imgSize; // Image parameters
                        GLenum mode, depth, minFilter, magFilter;  // Image format and texture filtering
                        int    baseLevel, maxLevel;                // MipMap information
                        GLint  wraps, wrapt;                       // Wrapping modes
                    // Private tools :
                        int    getChannelCount(GLenum _mode);
                        int    getChannelSize(GLenum _depth);
                        GLenum getAliasMode(GLenum _mode);
                public :
                    __ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter);
                    __ReadOnly_HdlTextureFormat(const __ReadOnly_HdlTextureFormat& copy);
                    ~__ReadOnly_HdlTextureFormat(void);

                    int    getWidth    (void) const;
                    int    getHeight   (void) const;
                    int    getChannel  (void) const;
                    int    getSize     (void) const;
                    GLenum getGLMode   (void) const;
                    GLenum getGLDepth  (void) const;
                    GLenum getMinFilter(void) const;
                    GLenum getMagFilter(void) const;
                    int    getBaseLevel(void) const;
                    int    getMaxLevel (void) const;
                    GLint  getSWrapping(void) const;
                    GLint  getTWrapping(void) const;

                    bool   operator==(const __ReadOnly_HdlTextureFormat&) const;
                    bool   operator!=(const __ReadOnly_HdlTextureFormat&) const;
            };

        // Texture Format Handle
                /**
                 \class HdlTextureFormat
                 \brief Object handle for OpenGL texture formats
                **/
            class HdlTextureFormat : public __ReadOnly_HdlTextureFormat
            {
                public :
                    // reproduce constructor :
                        HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter);
                    // Writing Functions
                        void setWidth    (int w);
                        void setHeight   (int h);
                        void setSize     (int w, int h);
                        void setGLMode   (GLenum md);
                        void setGLDepth  (GLenum dp);
                        void setMinFilter(GLenum mf);
                        void setMagFilter(GLenum mf);
                        void setBaseLevel(int l);
                        void setMaxLevel (int l);
                        void setSWrapping(GLint m);
                        void setTWrapping(GLint m);

                        const __ReadOnly_HdlTextureFormat& operator=(const __ReadOnly_HdlTextureFormat&);
            };

        // Texture Handle
                /**
                 \class HdlTexture
                 \brief Object handle for OpenGL textures
                **/
            class HdlTexture : public __ReadOnly_HdlTextureFormat
            {
                private :
                    // Data
                        GLuint texID;

                public :
                    // Functions
                        HdlTexture(const __ReadOnly_HdlTextureFormat& fmt);
                        ~HdlTexture(void);

                        /*int    getWidth  (void)  const;
                        int    getHeight (void)  const;
                        int    getChannel(void)  const;
                        int    getSize   (void)  const;
                        GLenum getGLMode (void)  const;
                        GLenum getGLDepth(void)  const;*/
                        GLuint getID     (void)  const;

                        void   bind(GLenum unit=GL_TEXTURE0_ARB);
                        void   bind(int unit);
                        void   write(GLvoid *texData, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO);
                        void   fill(char dataByte);

                        // Static Textures tools
                        static void unbind(GLenum unit=GL_TEXTURE0_ARB);
                        static void unbind(int unit);
            };

        // Multi-texturing specification Handling, as a namespace but with private members
                /**
                 \class HdlMultiTexturing
                 \brief Singleton handle for OpenGL Multitexturing skills
                **/
            class HdlMultiTexturing
            {
                private :
                    // Data
                        HdlMultiTexturing(void); // No instance tolerated
                        static GLenum currentUnit;
                        static GLint  maxUnit;

                public :
                    // Functions
                        static void   init(void);
                        static void   setWorkingUnit(GLenum unit);
                        static GLenum unitName(int id);
                        static int    unitIndex(GLenum name);
                        static void   bindToUnit(HdlTexture& tex, GLenum unit = currentUnit);
                        static void   bindToCurrentUnit(HdlTexture& tex);
                        static void   unbindUnit(GLenum unit = currentUnit);
                        static void   releaseUnit(GLenum unit);
                        static void   releaseCurrentUnit(void);
                        static void   glTexCoord2d(float u, float v, GLenum unit = currentUnit);

                        static GLenum getCurrentUnit(void);
                        static GLint  getMaxUnit(void);
            };
    }
}

#endif

