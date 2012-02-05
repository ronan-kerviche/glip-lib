/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTexture.cpp                                                                            */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Texture Handle                                                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlTexture.cpp
 * \brief   OpenGL Texture Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
**/

#include <cstring>
#include "HdlTexture.hpp"
#include "Exception.hpp"
#include "../include/GLIPLib.hpp"


using namespace Glip::CoreGL;

// HdlTextureFormat - Functions
	/**
	\fn    __ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter)
	\brief __ReadOnly_HdlTextureFormat Construtor.

	\param w           Width of the texture.
	\param h           Height of the texture.
	\param _mode       Mode for the texture (e.g. GL_RGB, GL_RGBA, GL_BGRA, etc.).
	\param _depth      Depth for the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, etc.).
	\param _minFilter  Minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	\param _magFilter  Magnification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	**/
	__ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter)
	 : imgW(w), imgH(h), mode(_mode), depth(_depth), minFilter(_minFilter), magFilter(_magFilter)
	{
		imgC      = getChannelCount(mode);
		colSize   = getChannelSize(depth);
		imgSize   = imgW*imgH*imgC*colSize;
		baseLevel = 0;
		maxLevel  = 0;
		wraps     = GL_CLAMP;
		wrapt     = GL_CLAMP;
	}

	/**
	\fn    __ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(const __ReadOnly_HdlTextureFormat& copy)
	\brief __ReadOnly_HdlTextureFormat Construtor.

	\param copy Copy this format.
	**/
	__ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(const __ReadOnly_HdlTextureFormat& copy)
	{
		imgW      = copy.imgW;
		imgH      = copy.imgH;
		mode      = copy.mode;
		depth     = copy.depth;
		minFilter = copy.minFilter;
		magFilter = copy.magFilter;
		baseLevel = copy.baseLevel;
		maxLevel  = copy.maxLevel;
		wraps     = copy.wraps;
		wrapt     = copy.wrapt;

		imgC      = getChannelCount(mode);
		colSize   = getChannelSize(depth);
		imgSize   = imgW*imgH*imgC*colSize;
	}

	__ReadOnly_HdlTextureFormat::~__ReadOnly_HdlTextureFormat(void)
	{ }

	// Private tools
	int __ReadOnly_HdlTextureFormat::getChannelCount(GLenum _mode) const
	{
		// implicit : get channel count
		switch(_mode)
		{
			case GL_RED		:
			case GL_GREEN		:
			case GL_LUMINANCE	:
			case GL_LUMINANCE_ALPHA	:
			case GL_ALPHA		: return 1;
			case GL_RG		: return 2;
			case GL_RGB32F		:
			case GL_RGB32I		:
			case GL_RGB32UI		:
			case GL_RGB16_SNORM	:
			case GL_RGB16F		:
			case GL_RGB16I		:
			case GL_RGB16UI		:
			case GL_RGB16		:
			case GL_RGB8_SNORM	:
			case GL_RGB8		:
			case GL_RGB8I		:
			case GL_RGB8UI		:
			case GL_SRGB8		:
			case GL_RGB9_E5		:
			case GL_RGB		:
			case GL_BGR		: return 3;
			case GL_RGBA32F		:
			case GL_RGBA32I		:
			case GL_RGBA32UI	:
			case GL_RGBA16		:
			case GL_RGBA16F		:
			case GL_RGBA16I		:
			case GL_RGBA16UI	:
			case GL_RGBA8		:
			case GL_RGBA8UI		:
			case GL_RGBA		:
			case GL_BGRA		: return 4;
			default :
				throw Exception("HdlTextureFormat - cannot recognize channel count for " + glParamName(_mode), __FILE__, __LINE__);
		}
	}

	int __ReadOnly_HdlTextureFormat::getChannelSize(GLenum _depth) const
	{
		switch(_depth)
		{
			#define TMP_SIZE(X, Type)    case X : return sizeof(Type);
				TMP_SIZE(GL_BYTE,           GLbyte )
				TMP_SIZE(GL_UNSIGNED_BYTE,  GLubyte )
				TMP_SIZE(GL_SHORT,          GLshort )
				TMP_SIZE(GL_UNSIGNED_SHORT, GLushort )
				TMP_SIZE(GL_INT,            GLint )
				TMP_SIZE(GL_UNSIGNED_INT,   GLuint )
				TMP_SIZE(GL_FLOAT,          GLfloat )
				TMP_SIZE(GL_DOUBLE,         GLdouble )
			#undef TMP_SIZE
			default :
				//colSize = 0;
				throw Exception("HdlTextureFormat - cannot recognize color channel type " + glParamName(_depth), __FILE__, __LINE__);
		}
	}

	GLenum __ReadOnly_HdlTextureFormat::getAliasMode(GLenum _mode) const
	{
		switch(_mode)
		{
			case GL_RED		:
			case GL_GREEN		:
			case GL_LUMINANCE	:
			case GL_LUMINANCE_ALPHA	:
			case GL_ALPHA		: return GL_RED;
			case GL_RG		: return GL_RG;
			case GL_RGB32F		:
			case GL_RGB32I		:
			case GL_RGB32UI		:
			case GL_RGB16_SNORM	:
			case GL_RGB16F		:
			case GL_RGB16I		:
			case GL_RGB16UI		:
			case GL_RGB16		:
			case GL_RGB8_SNORM	:
			case GL_RGB8		:
			case GL_RGB8I		:
			case GL_RGB8UI		:
			case GL_SRGB8		:
			case GL_RGB9_E5		:
			case GL_RGB		: return GL_RGB;
			case GL_BGR		: return GL_BGR;
			case GL_RGBA32F		:
			case GL_RGBA32I		:
			case GL_RGBA32UI	:
			case GL_RGBA16		:
			case GL_RGBA16F		:
			case GL_RGBA16I		:
			case GL_RGBA16UI	:
			case GL_RGBA8		:
			case GL_RGBA8UI		:
			case GL_RGBA		: return GL_RGBA;
			case GL_BGRA		: return GL_BGRA;
			default :
				throw Exception("HdlTextureFormat - cannot mode alias for " + glParamName(_mode), __FILE__, __LINE__);
		}
	}

// Public Tools
	/**
	\fn    int __ReadOnly_HdlTextureFormat::getWidth(void) const
	\brief Returns the texture's width.
	\fn    int __ReadOnly_HdlTextureFormat::getHeight(void) const
	\brief Returns the texture's height.
	\fn    int __ReadOnly_HdlTextureFormat::getChannel(void) const
	\brief Returns the texture's channel count.
	\fn    int __ReadOnly_HdlTextureFormat::getSize(void) const
	\brief Returns the texture's size in BYTE.
	\fn    int __ReadOnly_HdlTextureFormat::getChannelDepth(void) const
	\brief Returns the channel size in BYTE.
	\fn    GLenum __ReadOnly_HdlTextureFormat::getGLMode(void) const
	\brief Returns the texture's mode.
	\fn    GLenum __ReadOnly_HdlTextureFormat::getGLDepth(void) const
	\brief Returns the texture's pixel depth.
	\fn    GLenum __ReadOnly_HdlTextureFormat::getMinFilter(void) const
	\brief Returns the texture's minification parameter.
	\fn    GLenum __ReadOnly_HdlTextureFormat::getMagFilter(void) const
	\brief Returns the texture's magnification parameter.
	\fn    int __ReadOnly_HdlTextureFormat::getBaseLevel(void) const
	\brief Returns the texture's base level for mipmaps.
	\fn    int __ReadOnly_HdlTextureFormat::getMaxLevel (void) const
	\brief Returns the texture's highest level for mipmaps.
	\fn    GLint __ReadOnly_HdlTextureFormat::getSWrapping(void) const
	\brief Returns the texture's S wrapping parameter.
	\fn    GLint __ReadOnly_HdlTextureFormat::getTWrapping(void) const
	\brief Returns the texture's T wrapping parameter.
	**/
	int    __ReadOnly_HdlTextureFormat::getWidth   		(void) const { return imgW; }
	int    __ReadOnly_HdlTextureFormat::getHeight   	(void) const { return imgH; }
	int    __ReadOnly_HdlTextureFormat::getChannel  	(void) const { return imgC; }
	int    __ReadOnly_HdlTextureFormat::getChannelDepth  	(void) const { return getChannelSize(getGLDepth()); }
	int    __ReadOnly_HdlTextureFormat::getSize     	(void) const { return imgSize; }
	GLenum __ReadOnly_HdlTextureFormat::getGLMode   	(void) const { return mode; }
	GLenum __ReadOnly_HdlTextureFormat::getGLDepth  	(void) const { return depth; }
	GLenum __ReadOnly_HdlTextureFormat::getMinFilter	(void) const { return minFilter; }
	GLenum __ReadOnly_HdlTextureFormat::getMagFilter	(void) const { return magFilter; }
	int    __ReadOnly_HdlTextureFormat::getBaseLevel	(void) const { return baseLevel; }
	int    __ReadOnly_HdlTextureFormat::getMaxLevel 	(void) const { return maxLevel; }
	GLint  __ReadOnly_HdlTextureFormat::getSWrapping	(void) const { return wraps; }
	GLint  __ReadOnly_HdlTextureFormat::getTWrapping	(void) const { return wrapt; }

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::operator==(const __ReadOnly_HdlTextureFormat& f) const
	\brief Returns true if the two format have the same parameters.
	\param f Format to be compared with this.
	**/
	bool __ReadOnly_HdlTextureFormat::operator==(const __ReadOnly_HdlTextureFormat& f) const
	{
		return  (imgW      == f.imgW)      &&
			(imgH      == f.imgH)      &&
			(imgC      == f.imgC)      &&
			(colSize   == f.colSize)   &&
			(imgSize   == f.imgSize)   &&
			(mode      == f.mode)      &&
			(depth     == f.depth)     &&
			(minFilter == f.minFilter) &&
			(magFilter == f.magFilter) &&
			(baseLevel == f.baseLevel) &&
			(maxLevel  == f.maxLevel)  &&
			(wraps     == f.wraps)     &&
			(wrapt     == f.wrapt);
	}

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::operator!=(const __ReadOnly_HdlTextureFormat& f) const
	\brief Returns true if the two format have at least one different parameter.
	\param f Format to be compared with this.
	**/
	bool __ReadOnly_HdlTextureFormat::operator!=(const __ReadOnly_HdlTextureFormat& f) const
	{
		return !(*this==f);
	}

	/**
	\fn    HdlTextureFormat::HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter)
	\brief HdlTextureFormat Construtor.

	\param w           Width of the texture.
	\param h           Height of the texture.
	\param _mode       Mode for the texture (e.g. GL_RGB, GL_RGBA, GL_BGRA, etc.).
	\param _depth      Depth for the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, etc.).
	\param _minFilter  Minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	\param _magFilter  Magnification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	**/
	HdlTextureFormat::HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter)
	 : __ReadOnly_HdlTextureFormat(w, h, _mode, _depth, _minFilter, _magFilter)
	{ }

	/**
	\fn    HdlTextureFormat::HdlTextureFormat(const __ReadOnly_HdlTextureFormat& fmt)7
	\brief HdlTextureFormat Construtor.
	\param fmt Source format.
	**/
	HdlTextureFormat::HdlTextureFormat(const __ReadOnly_HdlTextureFormat& fmt)
	 : __ReadOnly_HdlTextureFormat(fmt)
	{ }

	/**
	\fn    void HdlTextureFormat::setWidth(int w)
	\brief Sets the texture's width.
	\param w The new width.
	\fn    void HdlTextureFormat::setHeight(int h)
	\brief Sets the texture's height.
	\param h The new height.
	\fn    void HdlTextureFormat::setSize(int w, int h)
	\brief Sets the format size.
	\param w The new width.
	\param h The new height.
	\fn    void HdlTextureFormat::setGLMode(GLenum md)
	\brief Sets the texture's mode.
	\param md The new mode for the texture (e.g. GL_RGB, GL_RGBA, GL_BGRA, etc.).
	\fn    void HdlTextureFormat::setGLDepth(GLenum dp)
	\brief Sets the texture's pixel depth.
	\param dp The new depth for the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, etc.).
	\fn    void HdlTextureFormat::setMinFilter(GLenum mf)
	\brief Sets the texture's minification parameter.
	\param mf The new minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	\fn    void HdlTextureFormat::setMagFilter(GLenum mf)
	\brief Sets the texture's magnification parameter.
	\param mf The new magnification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.).
	\fn    void HdlTextureFormat::setBaseLevel(int l)
	\brief Sets the texture's base level for mipmaps.
	\param l The new base level (must be greater than 0).
	\fn    void HdlTextureFormat::setMaxLevel (int l)
	\brief Sets the texture's highest level for mipmaps.
	\param l The new highest level (must be greater than 0).
	\fn    void HdlTextureFormat::setSWrapping(GLint m)
	\brief Sets the texture's S wrapping parameter.
	\param m The new S wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT)
	\fn    void HdlTextureFormat::setTWrapping(GLint m)
	\brief Sets the texture's T wrapping parameter.
	\param m The new T wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT)
	**/
	void HdlTextureFormat::setWidth    (int w)                   { imgW      = w;  }
	void HdlTextureFormat::setHeight   (int h)                   { imgH      = h;  }
	void HdlTextureFormat::setSize     (int w, int h)            { 	imgW      = w;
									imgH      = h;  }
	void HdlTextureFormat::setGLMode   (GLenum md)               { mode      = md; }
	void HdlTextureFormat::setGLDepth  (GLenum dp)               { depth     = dp; }
	void HdlTextureFormat::setMinFilter(GLenum mf)               { minFilter = mf; }
	void HdlTextureFormat::setMagFilter(GLenum mf)               { magFilter = mf; }
	void HdlTextureFormat::setBaseLevel(int l)                   { baseLevel = l;  }
	void HdlTextureFormat::setMaxLevel (int l)                   { maxLevel  = l;  }
	void HdlTextureFormat::setSWrapping(GLint m)                 { wraps     = m;  }
	void HdlTextureFormat::setTWrapping(GLint m)                 { wrapt     = m;  }

	/**
	\fn const __ReadOnly_HdlTextureFormat& HdlTextureFormat::operator=(const __ReadOnly_HdlTextureFormat& copy)
	\brief Copy operator.
	\param copy The format to copy.
	\return This.
	**/
	const __ReadOnly_HdlTextureFormat& HdlTextureFormat::operator=(const __ReadOnly_HdlTextureFormat& copy)
	{
		imgW      = copy.getWidth();
		imgH      = copy.getHeight();
		mode      = copy.getGLMode();
		depth     = copy.getGLDepth();
		minFilter = copy.getMinFilter();
		magFilter = copy.getMagFilter();
		baseLevel = copy.getBaseLevel();
		maxLevel  = copy.getMaxLevel();
		wraps     = copy.getSWrapping();
		wrapt     = copy.getTWrapping();

		imgC      = getChannelCount(mode);
		colSize   = getChannelSize(depth);
		imgSize   = imgW*imgH*imgC*colSize;

		return *this;
	}

// HdlTexture - Functions
	/**
	\fn HdlTexture::HdlTexture(const __ReadOnly_HdlTextureFormat& fmt)
	\brief HdlTexture constructor.
	\param fmt The format to use.
	**/
	HdlTexture::HdlTexture(const __ReadOnly_HdlTextureFormat& fmt)
	 : __ReadOnly_HdlTextureFormat(fmt), texID(0)
	{
		// Testing hardware :
		NEED_EXTENSION(GLEW_ARB_multitexture)
		NEED_EXTENSION(GLEW_ARB_texture_border_clamp)
		NEED_EXTENSION(GLEW_ARB_texture_non_power_of_two)
		NEED_EXTENSION(GLEW_ARB_texture_rectangle)
		NEED_EXTENSION(GLEW_ARB_texture_float)

		glEnable(GL_TEXTURE_2D);

		// Create the texture
		glGenTextures(1, &texID);

		// COMMON ERROR : USE OF MIPMAP : LINEAR_MIPMAP_NEAREST... when max level = 0 (leads to Invalid Enum)

		if(texID==0)
			throw Exception("HdlTexture::HdlTexture - Texture can't be created. Last OpenGL error : " + glErrorToString(), __FILE__, __LINE__);

		// Set it up
		glBindTexture(GL_TEXTURE_2D,texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getSWrapping() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getTWrapping() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, getBaseLevel() );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  getMaxLevel()  );

		if( getMaxLevel()>0 )
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		HdlTexture::unbind();
	}

	HdlTexture::~HdlTexture(void)
	{
		HdlTexture::unbind();

		// delete the texture
		glDeleteTextures( 1, &texID);
	}

// Public tools
	/**
	\fn GLuint HdlTexture::getID(void) const
	\brief Get the ID of the texture.
	\return The ID of the OpenGL texture handled by the driver.
	**/
	GLuint HdlTexture::getID(void) const
	{
		return texID;
	}

	/**
	\fn void HdlTexture::bind(GLenum unit)
	\brief Bind the texture to a unit.
	\param unit The target unit.
	**/
	void HdlTexture::bind(GLenum unit)
	{
		glActiveTextureARB(unit);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
		glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_REPLACE);
	}

	/**
	\fn void HdlTexture::bind(int unit)
	\brief Bind the texture to a unit.
	\param unit The target unit.
	**/
	void HdlTexture::bind(int unit)
	{
		bind(GL_TEXTURE0_ARB+static_cast<GLenum>(unit));
	}

	/**
	\fn void HdlTexture::write(GLvoid *texData, GLenum pixelFormat, GLenum pixelDepth)
	\brief Write data to a texture using classical glTexImage method.
	\param texData The pointer to the data.
	\param pixelFormat The pixel format of the input data (considered the same as the texture layout if not provided).
	\param pixelDepth The depth of the input data (considered the same as the texture layout if not provided).
	**/
	void HdlTexture::write(GLvoid *texData, GLenum pixelFormat, GLenum pixelDepth)
	{
		if(pixelFormat==GL_ZERO)
			pixelFormat = mode;

		if(pixelDepth==GL_ZERO)
			pixelDepth = depth;

		pixelFormat = getAliasMode(pixelFormat);

		// Bind it
		glBindTexture(GL_TEXTURE_2D, texID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//write
		glTexImage2D(GL_TEXTURE_2D, 0, mode, imgW, imgH, 0, pixelFormat, pixelDepth, texData);

		if( getMaxLevel()>0 )
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	/**
	\fn void HdlTexture::fill(char dataByte)
	\brief Fill a texture will a gray level.
	\param dataByte The gray level to apply.
	**/
	void HdlTexture::fill(char dataByte)
	{
		bind();

		char* tmp = new char[imgSize];
		memset(tmp, dataByte, imgSize);

		write(tmp);

		delete[] tmp;
	}

// Static tools
	/**
	\fn void HdlTexture::unbind(GLenum unit)
	\brief Unbind a unit from any texture.
	\param unit The target unit.
	**/
	void HdlTexture::unbind(GLenum unit)
	{
		glActiveTextureARB(unit);
		glBindTexture(GL_TEXTURE_2D, 0); //unBind
	}

	/**
	\fn void HdlTexture::unbind(int unit)
	\brief Unbind a unit from any texture.
	\param unit The target unit.
	**/
	void HdlTexture::unbind(int unit)
	{
		unbind(GL_TEXTURE0_ARB+static_cast<GLenum>(unit));
	}

	/**
	\fn int HdlTexture::getMaxImageUnits(void)
	\brief Get the maximum number of texture that can be bound at the same time (ie. the maximum number of input port of a filter).
	\return The maximum number of texture that can be bound at the same time.
	**/
	int HdlTexture::getMaxImageUnits(void)
	{
		int maxTextureImageUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);

		return maxTextureImageUnits;
	}
