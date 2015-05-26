/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
 * \date    August 7th 2010
**/

#ifndef __HDLTEXTURE_INCLUDE__
#define __HDLTEXTURE_INCLUDE__

	// Include
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Core/HdlTextureTools.hpp"

	namespace Glip
	{
		namespace CoreGL
		{

			// Texture Format Handle
			/**
			\class HdlAbstractTextureFormat
			\brief Object handle for OpenGL texture formats (Read Only).
			**/
			class GLIP_API HdlAbstractTextureFormat
			{
				private :
					int	alignment;						// Alignment in memory (on 1, 4 or 8 bytes).

				protected :
					// Data
					int	width, height; 						// Image parameters.
					GLenum	mode, depth, minFilter, magFilter, wraps, wrapt;  	// Image format, texture filtering and wrapping modes.
					int	baseLevel, maxLevel;                			// MipMap information.	

					// Protected constructors : 
					HdlAbstractTextureFormat(int _width, int _height, GLenum _mode, GLenum _depth, GLenum _minFilter = GL_NEAREST, GLenum _magFilter = GL_NEAREST, GLenum _wraps = GL_CLAMP, GLenum _wrapt = GL_CLAMP, int _baseLevel = 0, int _maxLevel = 0);
					HdlAbstractTextureFormat(const HdlAbstractTextureFormat& copy);

					// Protected function : 
					void setAlignment(int _alignment);

				public :
				virtual ~HdlAbstractTextureFormat(void);

					int	getWidth    	(void) const;
					int	getHeight   	(void) const;
					int	getNumPixels	(void) const;
					int	getNumChannels  (void) const;
					int	getNumElements	(void) const;
					int	getAlignment	(void) const;
					int	getPixelSize	(void) const;
				virtual size_t	getRowSize	(void) const;
				virtual size_t	getSize     	(void) const;
					int	getChannelDepth (void) const;
					GLenum	getGLMode   	(void) const;
					GLenum	getGLDepth  	(void) const;
					GLenum	getMinFilter	(void) const;
					GLenum	getMagFilter	(void) const;
					int	getBaseLevel	(void) const;
					int	getMaxLevel	(void) const;
					GLenum	getSWrapping	(void) const;
					GLenum	getTWrapping	(void) const;
					bool	isCompressed	(void) const;
					bool	isFloatingPoint	(void) const;

					const HdlTextureFormatDescriptor& getFormatDescriptor(void) const;

					bool	operator==(const HdlAbstractTextureFormat&) const;
					bool	operator!=(const HdlAbstractTextureFormat&) const;

					bool	isCompatibleWith(const HdlAbstractTextureFormat&) const;

					HdlAbstractTextureFormat getCompressedFormat(void) const;
					HdlAbstractTextureFormat getUncompressedFormat(void) const;
					bool	isCorrespondingCompressedFormat(const HdlAbstractTextureFormat&) const;

				virtual	unsigned int getSetting(GLenum param) const;

					// Static Tools :
					static int getMaxSize(void);
			};

			// Texture Format Handle
			/**
			\class HdlTextureFormat
			\brief Object handle for OpenGL texture formats.
			**/
			class GLIP_API HdlTextureFormat : public HdlAbstractTextureFormat
			{
				public :
					// reproduce constructor :
					HdlTextureFormat(int _width, int _height, GLenum _mode, GLenum _depth, GLenum _minFilter = GL_NEAREST, GLenum _magFilter = GL_NEAREST, GLenum _wraps = GL_CLAMP, GLenum _wrapt = GL_CLAMP, int _baseLevel = 0, int _maxLevel = 0);
					HdlTextureFormat(const HdlAbstractTextureFormat& fmt);

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
					void setSWrapping(GLenum m);
					void setTWrapping(GLenum m);

					const HdlAbstractTextureFormat& operator=(const HdlAbstractTextureFormat&);

					void setSetting(GLenum param, unsigned int value);

					static HdlTextureFormat getTextureFormat(GLuint texID);
			};

			// Texture Handle
			/**
			\class HdlTexture
			\brief Object handle for OpenGL textures.
			**/
			class GLIP_API HdlTexture : public HdlAbstractTextureFormat
			{
				private :
					// Data
					GLuint texID;
					bool proxy;

					// Functions
					HdlTexture(const HdlTexture&); // No-copy

				public :
					// Functions
					HdlTexture(const HdlAbstractTextureFormat& fmt);
					HdlTexture(GLuint proxyTexID);
					virtual ~HdlTexture(void);

					GLuint	getID(void) const;
					bool	isProxy(void) const;
					int	getSizeOnGPU(int m=0);
					void	bind(GLenum unit=GL_TEXTURE0_ARB);
					void	bind(int unit);
					void	write(GLvoid *texData, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO, int alignment=-1);
					void	writeCompressed(GLvoid *texData, int size, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO, int alignment=-1);
					void	fill(char dataByte);
					void	read(GLvoid *data, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO, int alignment=-1);
					GLenum	getInternalMode(void);
					bool	checkForConsistency(bool verbose = false);
					void	setMinFilter(GLenum mf);
					void	setMagFilter(GLenum mf);
					void	setSWrapping(GLenum m);
					void	setTWrapping(GLenum m);
					void 	setSetting(GLenum param, unsigned int value);

					const HdlAbstractTextureFormat& format(void) const;

					// Static Textures tools
					static void unbind(GLenum unit=GL_TEXTURE0_ARB);
					static void unbind(int unit);
					static int  getMaxImageUnits(void);
			};
		}
	}

#endif

