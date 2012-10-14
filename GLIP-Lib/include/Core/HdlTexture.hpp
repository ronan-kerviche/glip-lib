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
	#include "OglInclude.hpp"
	#include "HdlTextureTools.hpp"

	namespace Glip
	{
		namespace CoreGL
		{

			// Texture Format Handle
			/**
			\class __ReadOnly_HdlTextureFormat
			\brief Object handle for OpenGL texture formats (Read Only)
			**/
			class __ReadOnly_HdlTextureFormat
			{
				protected :
					// Data
					int	imgW, imgH, imgC, colSize, imgSize; // Image parameters
					GLenum	mode, depth, minFilter, magFilter;  // Image format and texture filtering
					int	baseLevel, maxLevel;                // MipMap information
					GLint	wraps, wrapt;                       // Wrapping modes

					// Protected tools :
					int	getChannelCount(GLenum _mode)				const;
					int	getChannelSize(GLenum _depth)				const;
					GLenum	getAliasMode(GLenum _mode)				const;
					bool	isCompressedMode(GLenum _mode)				const;
					bool	isFloatingPointMode(GLenum _mode, GLenum _depth)	const;
					GLenum	getCorrespondingCompressedMode(GLenum _mode)		const;
					GLenum 	getCorrespondingUncompressedMode(GLenum _mode) 		const;
					__ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps = GL_CLAMP, GLenum _wrapt = GL_CLAMP, int _baseLevel = 0, int _maxLevel = 0);
					__ReadOnly_HdlTextureFormat(const __ReadOnly_HdlTextureFormat& copy);

				public :
					~__ReadOnly_HdlTextureFormat(void);

					int	getWidth    	(void) const;
					int	getHeight   	(void) const;
					int	getNumPixels	(void) const;
					int	getNumChannels  (void) const;
					int	getNumElements	(void) const;
				virtual int	getSize     	(void) const;
					int	getChannelDepth (void) const;
					GLenum	getGLMode   	(void) const;
					GLenum	getGLDepth  	(void) const;
					GLenum	getMinFilter	(void) const;
					GLenum	getMagFilter	(void) const;
					int	getBaseLevel	(void) const;
					int	getMaxLevel	(void) const;
					GLint	getSWrapping	(void) const;
					GLint	getTWrapping	(void) const;
					bool	isCompressed	(void) const;
					bool	isFloatingPoint	(void) const;

					bool	operator==(const __ReadOnly_HdlTextureFormat&) const;
					bool	operator!=(const __ReadOnly_HdlTextureFormat&) const;

					bool	isCompatibleWith(const __ReadOnly_HdlTextureFormat&) const;

					__ReadOnly_HdlTextureFormat getCompressedFormat(void) const;
					__ReadOnly_HdlTextureFormat getUncompressedFormat(void) const;
					bool	isCorrespondingCompressedFormat(const __ReadOnly_HdlTextureFormat&) const;

					// Static Tools :
					static 	int	getMaxSize(void);
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
					HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps = GL_CLAMP, GLenum _wrapt = GL_CLAMP, int _baseLevel = 0, int _maxLevel = 0);
					HdlTextureFormat(const __ReadOnly_HdlTextureFormat& fmt);

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

					// Functions
					HdlTexture(const HdlTexture&);

				public :
					// Functions
					HdlTexture(const __ReadOnly_HdlTextureFormat& fmt);
					~HdlTexture(void);

					GLuint	getID     (void)  const;
					int	getSizeOnGPU(int m=0);
					void	bind(GLenum unit=GL_TEXTURE0_ARB);
					void	bind(int unit);
					void	write(GLvoid *texData, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO);
					void	writeCompressed(GLvoid *texData, int size, GLenum pixelFormat = GL_ZERO, GLenum pixelDepth = GL_ZERO);
					void	fill(char dataByte);
					GLenum	getInternalMode(void);
					bool	checkForConsistency(bool verbose = false);
					void	setMinFilter(GLenum mf);
					void	setMagFilter(GLenum mf);
					void	setSWrapping(GLint m);
					void	setTWrapping(GLint m);

					const __ReadOnly_HdlTextureFormat& format(void) const;

					// Static Textures tools
					static void unbind(GLenum unit=GL_TEXTURE0_ARB);
					static void unbind(int unit);
					static int  getMaxImageUnits(void);
			};
		}
	}

#endif

