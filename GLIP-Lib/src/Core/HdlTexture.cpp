/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
#include "Core/HdlTexture.hpp"
#include "Core/Exception.hpp"

using namespace Glip::CoreGL;

// HdlTextureFormat - Functions
	/**
	\fn    __ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps, GLenum _wrapt, int _baseLevel, int _maxLevel)
	\brief __ReadOnly_HdlTextureFormat Construtor.

	\param w           Width of the texture.
	\param h           Height of the texture.
	\param _mode       Mode for the texture (e.g. GL_RGB, GL_RGBA, GL_BGRA, etc.).
	\param _depth      Depth for the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, etc.).
	\param _minFilter  Minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR).
	\param _magFilter  Magnification filter (e.g. GL_NEAREST or GL_LINEAR, only these two options are accepted).
	\param _wraps      Wrapping S parameter (GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT).
	\param _wrapt      Wrapping T parameter (GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT).
	\param _baseLevel  Base for the mipmaps (default is 0).
	\param _maxLevel   Highest level for the mipmaps (default is 0, no other mipmaps than the original image).
	**/
	__ReadOnly_HdlTextureFormat::__ReadOnly_HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps, GLenum _wrapt, int _baseLevel, int _maxLevel)
	 : imgW(w), imgH(h), mode(_mode), depth(_depth), minFilter(_minFilter), magFilter(_magFilter), baseLevel(_baseLevel), maxLevel(_maxLevel), wraps(_wraps), wrapt(_wrapt)
	{ }

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
	}

	__ReadOnly_HdlTextureFormat::~__ReadOnly_HdlTextureFormat(void)
	{ }

	// Private tools
	int __ReadOnly_HdlTextureFormat::getChannelCount(GLenum _mode) const
	{
		return static_cast<int>(HdlTextureFormatDescriptorsList::get(_mode).numChannels);
	}

	int __ReadOnly_HdlTextureFormat::getChannelSize(GLenum _depth) const
	{
		return HdlTextureFormatDescriptorsList::getTypeDepth(_depth);
	}

	GLenum __ReadOnly_HdlTextureFormat::getAliasMode(GLenum _mode) const
	{
		return HdlTextureFormatDescriptorsList::get(_mode).aliasMode;
	}

	bool __ReadOnly_HdlTextureFormat::isCompressedMode(GLenum _mode) const
	{
		return HdlTextureFormatDescriptorsList::get(_mode).isCompressed;
	}

	bool __ReadOnly_HdlTextureFormat::isFloatingPointMode(GLenum _mode, GLenum _depth) const
	{
		return HdlTextureFormatDescriptorsList::get(_mode).isFloatting;
	}

	GLenum __ReadOnly_HdlTextureFormat::getCorrespondingCompressedMode(GLenum _mode) const
	{
		if(isCompressedMode(_mode))
			return _mode;
		else
			return HdlTextureFormatDescriptorsList::get(_mode).correspondingModeForCompressing;
	}

	GLenum __ReadOnly_HdlTextureFormat::getCorrespondingUncompressedMode(GLenum _mode) const
	{
		if(!isCompressedMode(_mode))
			return _mode;
		else
			return HdlTextureFormatDescriptorsList::get(_mode).correspondingModeForCompressing;
	}

// Public Tools
	/**
	\fn     int __ReadOnly_HdlTextureFormat::getWidth(void) const
	\return The texture's width.
	\fn     int __ReadOnly_HdlTextureFormat::getHeight(void) const
	\return The texture's height.
	\fn     int __ReadOnly_HdlTextureFormat::getNumPixels(void) const
	\return The texture's number of pixels (width x height).
	\fn     int __ReadOnly_HdlTextureFormat::getNumChannels(void) const
	\return The texture's channel count.
	\fn     int __ReadOnly_HdlTextureFormat::getChannelDepth(void) const
	\return The channel size in BYTE.
	\fn     int __ReadOnly_HdlTextureFormat::getNumElements(void) const
	\return The texture's number of elements (width x height x channels).
	\fn     size_t __ReadOnly_HdlTextureFormat::getSize(void) const
	\return The texture's size in BYTE.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getGLMode(void) const
	\return The texture's mode.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getGLDepth(void) const
	\return The texture's pixel depth.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getMinFilter(void) const
	\return The texture's minification parameter.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getMagFilter(void) const
	\return The texture's magnification parameter.
	\fn     int __ReadOnly_HdlTextureFormat::getBaseLevel(void) const
	\return The texture's base level for mipmaps.
	\fn     int __ReadOnly_HdlTextureFormat::getMaxLevel (void) const
	\return The texture's highest level for mipmaps.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getSWrapping(void) const
	\return The texture's S wrapping parameter.
	\fn     GLenum __ReadOnly_HdlTextureFormat::getTWrapping(void) const
	\return The texture's T wrapping parameter.
	\fn     bool __ReadOnly_HdlTextureFormat::isCompressed(void) const
	\return True if the texture is compressed.
	\fn     bool __ReadOnly_HdlTextureFormat::isFloatingPoint(void) const
	\return True if the texture is of floatting point type.
	**/
	int	__ReadOnly_HdlTextureFormat::getWidth   	(void) const { return imgW; }
	int	__ReadOnly_HdlTextureFormat::getHeight   	(void) const { return imgH; }
	int	__ReadOnly_HdlTextureFormat::getNumPixels	(void) const { return imgH*imgW; }
	int	__ReadOnly_HdlTextureFormat::getNumChannels  	(void) const { return getChannelCount(mode); }
	int	__ReadOnly_HdlTextureFormat::getChannelDepth  	(void) const { return getChannelSize(getGLDepth()); }
	int	__ReadOnly_HdlTextureFormat::getNumElements	(void) const { return imgH*imgW*getChannelCount(mode); }
	size_t	__ReadOnly_HdlTextureFormat::getSize     	(void) const { return static_cast<size_t>(getWidth()) * static_cast<size_t>(getHeight()) * static_cast<size_t>(getNumChannels()) * static_cast<size_t>(getChannelDepth()); }
	GLenum	__ReadOnly_HdlTextureFormat::getGLMode   	(void) const { return mode; }
	GLenum	__ReadOnly_HdlTextureFormat::getGLDepth  	(void) const { return depth; }
	GLenum	__ReadOnly_HdlTextureFormat::getMinFilter	(void) const { return minFilter; }
	GLenum	__ReadOnly_HdlTextureFormat::getMagFilter	(void) const { return magFilter; }
	int	__ReadOnly_HdlTextureFormat::getBaseLevel	(void) const { return baseLevel; }
	int	__ReadOnly_HdlTextureFormat::getMaxLevel 	(void) const { return maxLevel; }
	GLenum	__ReadOnly_HdlTextureFormat::getSWrapping	(void) const { return wraps; }
	GLenum	__ReadOnly_HdlTextureFormat::getTWrapping	(void) const { return wrapt; }
	bool	__ReadOnly_HdlTextureFormat::isCompressed	(void) const { return isCompressedMode(mode); }
	bool	__ReadOnly_HdlTextureFormat::isFloatingPoint	(void) const { return isFloatingPointMode(mode, depth); }

	/**
	\fn const HdlTextureFormatDescriptor& __ReadOnly_HdlTextureFormat::getFormatDescriptor(void) const
	\return A constatnt reference to the descriptor of the current mode.
	**/
	const HdlTextureFormatDescriptor& __ReadOnly_HdlTextureFormat::getFormatDescriptor(void) const
	{
		return HdlTextureFormatDescriptorsList::get(getGLMode());
	}

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::operator==(const __ReadOnly_HdlTextureFormat& f) const
	\param f Format to be compared with this.
	\return True if the two format have the same parameters.
	**/
	bool __ReadOnly_HdlTextureFormat::operator==(const __ReadOnly_HdlTextureFormat& f) const
	{
		return  (imgW			== f.imgW)		&&
			(imgH			== f.imgH)		&&
			(getNumChannels()	== f.getNumChannels())	&&
			(getChannelDepth()	== f.getChannelDepth())   &&
			(getSize()		== f.getSize())		&&
			(mode			== f.mode)		&&
			(depth			== f.depth)		&&
			(minFilter		== f.minFilter)		&&
			(magFilter		== f.magFilter)		&&
			(baseLevel		== f.baseLevel)		&&
			(maxLevel		== f.maxLevel)		&&
			(wraps			== f.wraps)		&&
			(wrapt			== f.wrapt);
	}

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::operator!=(const __ReadOnly_HdlTextureFormat& f) const
	\param f Format to be compared with this.
	\return True if the two format have at least one different parameter.
	**/
	bool __ReadOnly_HdlTextureFormat::operator!=(const __ReadOnly_HdlTextureFormat& f) const
	{
		return !(*this==f);
	}

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::isCompatibleWith(const __ReadOnly_HdlTextureFormat&) const
	\brief Check if the two formats share the same memory characteristics (sizes, number of channels, byte per pixel, internal GL mode, same mipmap levels).
	\param f Format to be compared with this.
	\return True if the two formats share the same memory characteristics (sizes, number of channels, byte per pixel, internal GL mode, same mipmap levels).
	**/
	bool __ReadOnly_HdlTextureFormat::isCompatibleWith(const __ReadOnly_HdlTextureFormat& f) const
	{
		return  (imgW			== f.imgW)		&&
			(imgH			== f.imgH)		&&
			(getNumChannels()	== f.getNumChannels())	&&
			(getChannelDepth()	== f.getChannelDepth())	&&
			(getSize()		== f.getSize())		&&
			(mode			== f.mode)		&&
			(depth			== f.depth)		&&
			(baseLevel		== f.baseLevel)		&&
			(maxLevel		== f.maxLevel);
	}

	/**
	\fn    __ReadOnly_HdlTextureFormat __ReadOnly_HdlTextureFormat::getCompressedFormat(void) const
	\brief Get the equivalent, but compressed, format.
	\return A __ReadOnly_HdlTextureFormat object instance identical to this, but mode is set to the corresponding compressed format.
	**/
	__ReadOnly_HdlTextureFormat __ReadOnly_HdlTextureFormat::getCompressedFormat(void) const
	{
		if(isCompressed())
			return *this;
		else
		{
			__ReadOnly_HdlTextureFormat res(*this);
			res.mode = getCorrespondingCompressedMode(mode);
			return res;
		}
	}

	/**
	\fn    __ReadOnly_HdlTextureFormat __ReadOnly_HdlTextureFormat::getUncompressedFormat(void) const
	\brief Get the equivalent, but uncompressed, format.
	\return A __ReadOnly_HdlTextureFormat object instance identical to this, but mode is set to the corresponding uncompressed format.
	**/
	__ReadOnly_HdlTextureFormat __ReadOnly_HdlTextureFormat::getUncompressedFormat(void) const
	{
		if(!isCompressed())
			return *this;
		else
		{
			__ReadOnly_HdlTextureFormat res(*this);
			res.mode = getCorrespondingUncompressedMode(mode);
			return res;
		}
	}

	/**
	\fn    bool __ReadOnly_HdlTextureFormat::isCorrespondingCompressedFormat(const __ReadOnly_HdlTextureFormat& f) const
	\brief Check if this and f share the same memory parameters except that the mode for f is the corresponding compressed mode of this.
	\param f Format to be compared with this.
	\return True if this and f share the same memory parameters except that the mode for f is the corresponding compressed mode of this.
	**/
	bool __ReadOnly_HdlTextureFormat::isCorrespondingCompressedFormat(const __ReadOnly_HdlTextureFormat& f) const
	{
		bool test = 	(imgW			== f.imgW)		&&
				(imgH			== f.imgH)		&&
				(getNumChannels()	== f.getNumChannels())	&&
				(getChannelDepth()	== f.getChannelDepth())	&&
				(getSize()		== f.getSize())		&&
				(depth			== f.depth)		&&
				(baseLevel		== f.baseLevel)		&&
				(maxLevel		== f.maxLevel);

		return test && (f.mode==getCorrespondingCompressedMode(mode));
	}

	/**
	\fn	unsigned int __ReadOnly_HdlTextureFormat::getSetting(GLenum param) const
	\brief	Obtain a setting value based on its GL name.
	\param param The GL name of the setting to be retreived.
	\return The value of the corresponding setting or raise an Exception if any error occur.
	
	Available paramaters : 
	Parameter       			| Description
	--------------------------------------- | --------------------------
	GL_TEXTURE_WIDTH			| Get the width (same as __ReadOnly_HdlTextureFormat::getWidth()).
	GL_TEXTURE_HEIGHT			| Get the height (same as __ReadOnly_HdlTextureFormat::getHeight()).
	GL_TEXTURE_RED_SIZE			| Get the depth of the red channel in bits (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().redDepthInBits).
	GL_TEXTURE_GREEN_SIZE			| Get the depth of the green channel in bits (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().greenDepthInBits).
	GL_TEXTURE_BLUE_SIZE			| Get the depth of the blue channel in bits (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().blueDepthInBits).
	GL_TEXTURE_ALPHA_SIZE			| Get the depth of the alpha channel in bits (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().alphaDepthInBits).
	GL_TEXTURE_LUMINANCE_SIZE		| Get the depth of the luminance channel in bits (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().luminanceDepthInBits).
	GL_TEXTURE_MIN_FILTER			| Get the minification parameter (same as __ReadOnly_HdlTextureFormat::getMinFilter()).
	GL_TEXTURE_MAG_FILTER			| Get the magnification parameter (same as __ReadOnly_HdlTextureFormat::getMagFilter()).
	GL_TEXTURE_WRAP_S			| Get the S wrapping parameter (same as __ReadOnly_HdlTextureFormat::getSWrapping()).
	GL_TEXTURE_WRAP_T			| Get the T wrapping parameter (same as __ReadOnly_HdlTextureFormat::getTWrapping()).
	GL_TEXTURE_BASE_LEVEL			| Get the base mipmap level (same as __ReadOnly_HdlTextureFormat::getBaseLevel()).
	GL_TEXTURE_MAX_LEVEL			| Get the base mipmap level (same as __ReadOnly_HdlTextureFormat::getMaxLevel()).
	GL_GENERATE_MIPMAP			| Return 1 if the mipmap will be generated.
	GL_TEXTURE_INTERNAL_FORMAT		| Get the mode (same as __ReadOnly_HdlTextureFormat::getGLMode()).
	GL_TEXTURE_COMPRESSED			| Return 1 if the texture is compressed, 0 otherwise (same as __ReadOnly_HdlTextureFormat::isCompressed()).
	GL_TEXTURE_COMPRESSED_IMAGE_SIZE	| Will raise an exception as the format cannot communicate with the display driver.
	GL_TEXTURE_DEPTH			| Get the depth (same as __ReadOnly_HdlTextureFormat::getGLDepth()).
	GL_TEXTURE_RED_TYPE			| Get the red type (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().redType).
	GL_TEXTURE_GREEN_TYPE			| Get the green type (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().greenType).
	GL_TEXTURE_BLUE_TYPE			| Get the blue type (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().blueType).
	GL_TEXTURE_ALPHA_TYPE			| Get the alpha type (same as __ReadOnly_HdlTextureFormat::getFormatDescriptor().alphaType).
	**/
	unsigned int __ReadOnly_HdlTextureFormat::getSetting(GLenum param) const
	{
		#define TEXTURE_CHANNEL_TYPE(bitName, typeName)	if(getFormatDescriptor(). bitName ) \
								{ \
									GLenum t = getFormatDescriptor(). typeName ; \
									if(t!=GL_NONE) \
										return t; \
									else \
										return getGLDepth(); \
								} \
								else \
									return GL_NONE;
		
		switch(param)
		{
			case GL_TEXTURE_WIDTH :			return getWidth();
			case GL_TEXTURE_HEIGHT : 		return getHeight();
			case GL_TEXTURE_RED_SIZE :		return getFormatDescriptor().redDepthInBits;
			case GL_TEXTURE_GREEN_SIZE :		return getFormatDescriptor().greenDepthInBits;
			case GL_TEXTURE_BLUE_SIZE :		return getFormatDescriptor().blueDepthInBits;
			case GL_TEXTURE_ALPHA_SIZE :		return getFormatDescriptor().alphaDepthInBits;
			case GL_TEXTURE_LUMINANCE_SIZE :	return getFormatDescriptor().luminanceDepthInBits;
			case GL_TEXTURE_MIN_FILTER :		return getMinFilter();
			case GL_TEXTURE_MAG_FILTER :		return getMagFilter();
			case GL_TEXTURE_WRAP_S :		return getSWrapping();
			case GL_TEXTURE_WRAP_T :		return getTWrapping();
			case GL_TEXTURE_BASE_LEVEL :		return getBaseLevel();
			case GL_TEXTURE_MAX_LEVEL :		return getMaxLevel();
			case GL_GENERATE_MIPMAP :		return getMaxLevel()>0;
			case GL_TEXTURE_INTERNAL_FORMAT :	return getGLMode();		
			case GL_TEXTURE_COMPRESSED :		return isCompressed() ? 1 : 0;
			case GL_TEXTURE_COMPRESSED_IMAGE_SIZE :	throw Exception("__ReadOnly_HdlTextureFormat::getSetting : Unable to forecast the size of a compressed texture.", __FILE__, __LINE__);
			case GL_TEXTURE_DEPTH :			return getGLDepth();
			case GL_TEXTURE_RED_TYPE :		TEXTURE_CHANNEL_TYPE( hasRedChannel, 	redType )
			case GL_TEXTURE_GREEN_TYPE :		TEXTURE_CHANNEL_TYPE( hasGreenChannel, 	greenType )
			case GL_TEXTURE_BLUE_TYPE :		TEXTURE_CHANNEL_TYPE( hasBlueChannel, 	blueType )
			case GL_TEXTURE_ALPHA_TYPE :		TEXTURE_CHANNEL_TYPE( hasAlphaChannel, 	alphaType )
			default : 				throw Exception("__ReadOnly_HdlTextureFormat::getSetting : Throw unable to get parameter \"" + glParamName(param) + "\".", __FILE__, __LINE__);
		}

		#undef TEXTURE_CHANNEL_TYPE
	}

	/**
	\fn    static int __ReadOnly_HdlTextureFormat::getMaxSize(void)
	\brief Returns the maximum size for a texture.
	\return The size, in pixels.
	**/
	int __ReadOnly_HdlTextureFormat::getMaxSize(void)
	{
		GLint s;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &s);
		return static_cast<int>(s);
	}

	/**
	\fn    HdlTextureFormat::HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps, GLenum _wrapt, int _baseLevel, int _maxLevel)
	\brief HdlTextureFormat Construtor.

	\param w           Width of the texture.
	\param h           Height of the texture.
	\param _mode       Mode for the texture (e.g. GL_RGB, GL_RGBA, GL_BGRA, etc.).
	\param _depth      Depth for the texture (e.g. GL_FLOAT, GL_UNSIGNED_BYTE, GL_INT, etc.).
	\param _minFilter  Minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR).
	\param _magFilter  Magnification filter (e.g. GL_NEAREST or GL_LINEAR, only these two options are accepted).
	\param _wraps      Wrapping S parameter (GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT).
	\param _wrapt      Wrapping T parameter (GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT).
	\param _baseLevel  Base for the mipmaps (default is 0).
	\param _maxLevel   Highest level for the mipmaps (default is 0, no other mipmaps than the original image).
	**/
	HdlTextureFormat::HdlTextureFormat(int w, int h, GLenum _mode, GLenum _depth, GLenum _minFilter, GLenum _magFilter, GLenum _wraps, GLenum _wrapt, int _baseLevel, int _maxLevel)
	 : __ReadOnly_HdlTextureFormat(w, h, _mode, _depth, _minFilter, _magFilter, _wraps, _wrapt, _baseLevel, _maxLevel)
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
	\param mf The new minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR).
	\fn    void HdlTextureFormat::setMagFilter(GLenum mf)
	\brief Sets the texture's magnification parameter.
	\param mf The new magnification filter (e.g. GL_NEAREST or GL_LINEAR, only these two options are accepted).
	\fn    void HdlTextureFormat::setBaseLevel(int l)
	\brief Sets the texture's base level for mipmaps.
	\param l The new base level (must be greater than 0).
	\fn    void HdlTextureFormat::setMaxLevel (int l)
	\brief Sets the texture's highest level for mipmaps.
	\param l The new highest level (must be greater than 0).
	\fn    void HdlTextureFormat::setSWrapping(GLenum m)
	\brief Sets the texture's S wrapping parameter.
	\param m The new S wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	\fn    void HdlTextureFormat::setTWrapping(GLenum m)
	\brief Sets the texture's T wrapping parameter.
	\param m The new T wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	**/
	void HdlTextureFormat::setWidth    (int w)		{ imgW      = w;  }
	void HdlTextureFormat::setHeight   (int h)		{ imgH      = h;  }
	void HdlTextureFormat::setSize     (int w, int h)	{ imgW     = w;
								  imgH     = h;   }
	void HdlTextureFormat::setGLMode   (GLenum md)		{ mode      = md; }
	void HdlTextureFormat::setGLDepth  (GLenum dp)		{ depth     = dp; }
	void HdlTextureFormat::setMinFilter(GLenum mf)		{ minFilter = mf; }
	void HdlTextureFormat::setMagFilter(GLenum mf)		{ magFilter = mf; }
	void HdlTextureFormat::setBaseLevel(int l)		{ baseLevel = l;  }
	void HdlTextureFormat::setMaxLevel (int l)		{ maxLevel  = l;  }
	void HdlTextureFormat::setSWrapping(GLenum m)		{ wraps     = m;  }
	void HdlTextureFormat::setTWrapping(GLenum m)		{ wrapt     = m;  }

	/**
	\fn const __ReadOnly_HdlTextureFormat& HdlTextureFormat::operator=(const __ReadOnly_HdlTextureFormat& copy)
	\brief Copy operator.
	\param copy The format to copy.
	\return This object.
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

		return *this;
	}

	/**
	\fn void HdlTextureFormat::setSetting(GLenum param, unsigned int value)
	\brief Set parameter, this function will raise an Exception if any errors occur.
	\param param The GL name of the paramater to set.
	\param value The value to assign to the parameter.
	
	Available paramaters : 
	Parameter       			| Description
	--------------------------------------- | --------------------------
	GL_TEXTURE_WIDTH			| Set the width (same as HdlTextureFormat::setWidth()).
	GL_TEXTURE_HEIGHT			| Set the height (same as HdlTextureFormat::setHeight()).
	GL_TEXTURE_MIN_FILTER			| Set the minification parameter (same as HdlTextureFormat::setMinFilter()).
	GL_TEXTURE_MAG_FILTER			| Set the magnification parameter (same as HdlTextureFormat::setMagFilter()).
	GL_TEXTURE_WRAP_S			| Set the S wrapping parameter (same as HdlTextureFormat::setSWrapping()).
	GL_TEXTURE_WRAP_T			| Set the T wrapping parameter (same as HdlTextureFormat::setTWrapping()).
	GL_TEXTURE_BASE_LEVEL			| Set the base mipmap level (same as HdlTextureFormat::setBaseLevel()).
	GL_TEXTURE_MAX_LEVEL			| Set the base mipmap level (same as HdlTextureFormat::setMaxLevel()).
	GL_TEXTURE_INTERNAL_FORMAT		| Set the mode (same as HdlTextureFormat::setGLMode()).
	GL_TEXTURE_DEPTH			| Set the depth (same as HdlTextureFormat::setGLDepth()).
	**/
	void HdlTextureFormat::setSetting(GLenum param, unsigned int value)
	{
		switch(param)
		{
			case GL_TEXTURE_WIDTH :			setWidth(value);	break;
			case GL_TEXTURE_HEIGHT : 		setHeight(value);	break;
			case GL_TEXTURE_MIN_FILTER :		setMinFilter(value);	break;
			case GL_TEXTURE_MAG_FILTER :		setMagFilter(value);	break;
			case GL_TEXTURE_WRAP_S :		setSWrapping(value);	break;
			case GL_TEXTURE_WRAP_T :		setTWrapping(value);	break;
			case GL_TEXTURE_BASE_LEVEL :		setBaseLevel(value);	break;
			case GL_TEXTURE_MAX_LEVEL :		setMaxLevel(value);	break;
			case GL_TEXTURE_INTERNAL_FORMAT :	setGLMode(value);	break;
			case GL_TEXTURE_DEPTH :			setGLDepth(value);	break;
			case GL_TEXTURE_RED_SIZE :		
			case GL_TEXTURE_GREEN_SIZE :		
			case GL_TEXTURE_BLUE_SIZE :		
			case GL_TEXTURE_ALPHA_SIZE :		
			case GL_TEXTURE_LUMINANCE_SIZE :	
			case GL_GENERATE_MIPMAP :		
			case GL_TEXTURE_COMPRESSED :		
			case GL_TEXTURE_COMPRESSED_IMAGE_SIZE :		
			case GL_TEXTURE_RED_TYPE :		
			case GL_TEXTURE_GREEN_TYPE :		
			case GL_TEXTURE_BLUE_TYPE :		
			case GL_TEXTURE_ALPHA_TYPE :		throw Exception("HdlTextureFormat::setSetting : Parameter \"" + glParamName(param) + "\" cannot be set.", __FILE__, __LINE__);
			default : 				throw Exception("HdlTextureFormat::setSetting : Throw unable to get parameter \"" + glParamName(param) + "\".", __FILE__, __LINE__);
		}
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

		if(isFloatingPoint())
			NEED_EXTENSION(GLEW_ARB_texture_float)

		if(isCompressed())
			NEED_EXTENSION(GLEW_ARB_texture_compression);

		// Test :
		if(getBaseLevel()>getMaxLevel())
			throw Exception("HdlTexture::HdlTexture - Texture can't be created : Base mipmap level (" + to_string(getBaseLevel()) + ") is greater than maximum mipmap level (" + to_string(getMaxLevel()) + ").", __FILE__, __LINE__);

		glEnable(GL_TEXTURE_2D);

		// Create the texture
		glGenTextures(1, &texID);

		// COMMON ERROR : USE OF MIPMAP : LINEAR_MIPMAP_NEAREST... when max level = 0 (leads to Invalid Enum)
		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlTexture::HdlTexture", "glGenTextures()")
		#endif

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
		{
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		}

		bool testAcceptanceFailed;
		std::string errString = glErrorToString(&testAcceptanceFailed);

		if(testAcceptanceFailed)
			throw Exception("HdlTexture::HdlTexture - One or more texture parameter cannot be set among : MinFilter = " + glParamName(getMinFilter()) + ", MagFilter = " + glParamName(getMagFilter()) + ", SWrapping = " + glParamName(getSWrapping()) + ", TWrapping = " + glParamName(getTWrapping()) + ", BaseLevel = " + to_string(getBaseLevel()) + ", MaxLevel = " + to_string(getMaxLevel()) + ". Last OpenGL error : " + errString + ".", __FILE__, __LINE__);

		HdlTexture::unbind();
	}

	HdlTexture::~HdlTexture(void)
	{
		HdlTexture::unbind();

		// delete the texture
		glDeleteTextures( 1, &texID);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlTexture::~HdlTexture", "glDeleteTextures()")
		#endif
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
	\fn void HdlTexture::getSizeOnGPU(int m)
	\brief Returns the size of the texture in bytes for mipmap m, this function returns the same value as __ReadOnly_HdlTextureFormat::getSize() for standard textures but gives the true size on the GPU for compressed textures. As this function requires a GL API call plus binding, this may be slow.
	\param m The target mipmap level.
	\return Returns the size of the texture in bytes.
	**/
	int HdlTexture::getSizeOnGPU(int m)
	{
		GLint s;

		bind();

		if(isCompressed())
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &s);
		else
		{
			GLint rs, gs, bs, as, ls, is, ds, _w, _h, bytesPerPixel;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_WIDTH,		&_w );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_HEIGHT,		&_h );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_RED_SIZE,		&rs );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_GREEN_SIZE,	&gs );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_BLUE_SIZE,	&bs );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_ALPHA_SIZE,	&as );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_LUMINANCE_SIZE,	&ls );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_INTENSITY_SIZE,	&is );
			glGetTexLevelParameteriv(GL_TEXTURE_2D, m, GL_TEXTURE_DEPTH_SIZE,	&ds );

			bytesPerPixel = (rs+gs+bs+as+ls+is+ds)/8;
			s = _w*_h*bytesPerPixel;
		}

		return static_cast<int>(s);
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
	\fn    void HdlTexture::setMinFilter(GLenum mf)
	\brief Sets the texture's minification parameter. WARNING : no error checking is performed within this function.
	\param mf The new minification filter (e.g. GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR).
	\fn    void HdlTexture::setMagFilter(GLenum mf)
	\brief Sets the texture's magnification parameter. WARNING : no error checking is performed within this function.
	\param mf The new magnification filter (e.g. GL_NEAREST or GL_LINEAR, only these two options are accepted).
	\fn    void HdlTexture::setSWrapping(GLenum m)
	\brief Sets the texture's S wrapping parameter. WARNING : no error checking is performed within this function.
	\param m The new S wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	\fn    void HdlTexture::setTWrapping(GLenum m)
	\brief Sets the texture's T wrapping parameter. WARNING : no error checking is performed within this function.
	\param m The new T wrapping parameter (e.g. GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT)
	**/
	void HdlTexture::setMinFilter(GLenum mf)	{ minFilter = mf; bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter() );}
	void HdlTexture::setMagFilter(GLenum mf)	{ magFilter = mf; bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter() );}
	void HdlTexture::setSWrapping(GLenum m)		{ wraps     = m;  bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     getSWrapping() );}
	void HdlTexture::setTWrapping(GLenum m)		{ wrapt     = m;  bind(); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     getTWrapping() );}

	/**
	\fn void HdlTexture::setSetting(GLenum param, unsigned int value)
	\brief Set parameter, this function will raise an Exception if any errors occur.
	\param param The GL name of the paramater to set.
	\param value The value to assign to the parameter.
	
	Available paramaters : 
	Parameter       			| Description
	--------------------------------------- | --------------------------
	GL_TEXTURE_MIN_FILTER			| Set the minification parameter (same as HdlTexture::setMinFilter()).
	GL_TEXTURE_MAG_FILTER			| Set the magnification parameter (same as HdlTexture::setMagFilter()).
	GL_TEXTURE_WRAP_S			| Set the S wrapping parameter (same as HdlTexture::setSWrapping()).
	GL_TEXTURE_WRAP_T			| Set the T wrapping parameter (same as HdlTexture::setTWrapping()).
	**/
	void HdlTexture::setSetting(GLenum param, unsigned int value)
	{
		switch(param)
		{
			case GL_TEXTURE_MIN_FILTER :		setMinFilter(value);	break;
			case GL_TEXTURE_MAG_FILTER :		setMagFilter(value);	break;
			case GL_TEXTURE_WRAP_S :		setSWrapping(value);	break;
			case GL_TEXTURE_WRAP_T :		setTWrapping(value);	break;
			case GL_TEXTURE_WIDTH :	
			case GL_TEXTURE_HEIGHT :
			case GL_TEXTURE_BASE_LEVEL :
			case GL_TEXTURE_MAX_LEVEL :
			case GL_TEXTURE_INTERNAL_FORMAT :
			case GL_TEXTURE_DEPTH :
			case GL_TEXTURE_RED_SIZE :		
			case GL_TEXTURE_GREEN_SIZE :		
			case GL_TEXTURE_BLUE_SIZE :		
			case GL_TEXTURE_ALPHA_SIZE :		
			case GL_TEXTURE_LUMINANCE_SIZE :	
			case GL_GENERATE_MIPMAP :		
			case GL_TEXTURE_COMPRESSED :		
			case GL_TEXTURE_COMPRESSED_IMAGE_SIZE :		
			case GL_TEXTURE_RED_TYPE :		
			case GL_TEXTURE_GREEN_TYPE :		
			case GL_TEXTURE_BLUE_TYPE :		
			case GL_TEXTURE_ALPHA_TYPE :		throw Exception("HdlTexture::setSetting : Parameter \"" + glParamName(param) + "\" cannot be set.", __FILE__, __LINE__);
			default : 				throw Exception("HdlTexture::setSetting : Throw unable to get parameter \"" + glParamName(param) + "\".", __FILE__, __LINE__);
		}
	}

	/**
	\fn void HdlTexture::write(GLvoid *texData, GLenum pixelFormat, GLenum pixelDepth)
	\brief Write data to a texture using classical glTexImage method. In the case that the texture is compressed and the input data is not of the compressed format, you MUST specify pixelFormat and pixelDepth. WARNING : this function does not perform error checking.
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

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlTexture::write", "glTexImage2D()")
		#endif

		if( getMaxLevel()>0 )
		{
			glGenerateMipmap(GL_TEXTURE_2D);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlTexture::write", "glGenerateMipmap()")
			#endif
		}
	}

	/**
	\fn void HdlTexture::writeCompressed(GLvoid *texData, int size, GLenum pixelFormat, GLenum pixelDepth)
	\brief Write compressed data to a texture using glCompressedTexImage2D method.
	\param texData The pointer to the data.
	\param size The size in bytes of the data.
	\param pixelFormat The pixel format of the input data (considered the same as the texture layout if not provided).
	\param pixelDepth The depth of the input data (considered the same as the texture layout if not provided).
	**/
	void HdlTexture::writeCompressed(GLvoid *texData, int size, GLenum pixelFormat, GLenum pixelDepth)
	{
		if(pixelFormat==GL_ZERO)
			pixelFormat = mode;

		if(pixelDepth==GL_ZERO)
			pixelDepth = depth;

		// Bind it
		glBindTexture(GL_TEXTURE_2D, texID);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//write
		//glTexImage2D(GL_TEXTURE_2D, 0, mode, imgW, imgH, 0, pixelFormat, pixelDepth, texData);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, mode, imgW, imgH, 0,  static_cast<GLsizei>(size), texData);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlTexture::writeCompressed", "glCompressedTexImage2D()")
		#endif

		if( getMaxLevel()>0 )
		{
			glGenerateMipmap(GL_TEXTURE_2D);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlTexture::writeCompressed", "glGenerateMipmap()")
			#endif
		}
	}

	/**
	\fn void HdlTexture::fill(char dataByte)
	\brief Fill a texture will a gray level.
	\param dataByte The gray level to apply.
	**/
	void HdlTexture::fill(char dataByte)
	{
		/*if(isCompressed())
			throw Exception("HdlTexture::fill - Cannot be called on a compressed texture.", __FILE__, __LINE__);*/

		bind();

		size_t imgSize = getSize();

		char* tmp = new char[imgSize];
		memset(tmp, dataByte, imgSize);

		write(tmp);

		delete[] tmp;
	}

	/**
	\fn GLenum HdlTexture::getInternalMode(void)
	\brief Get the internal format of the texture. For an uncompressed texture the result is the same as getGLMode() but for a compressed texture it returns the real compression mode used.
	\return The internal format of the texture.
	**/
	GLenum HdlTexture::getInternalMode(void)
	{
		bind();

		GLint param;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &param);

		return param;
	}

	bool HdlTexture::checkForConsistency(bool verbose)
	{
		GLint	glId;
		GLint 	vMagFilter, vMinFilter, vBaseLevel, vMaxLevel, vSWrap, vTWrap, vMipmapGen, vWidth, vHeight, vMode, vBorder, vCompressed;
		bool 	tMagFilter, tMinFilter, tBaseLevel, tMaxLevel, tSWrap, tTWrap, tMipmapGen, tWidth, tHeight, tMode, tBorder, tCompressed;

		bind();

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &glId);

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 		&vMagFilter);	tMagFilter	= vMagFilter==getMagFilter();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 		&vMinFilter);	tMinFilter	= vMinFilter==getMinFilter();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 		&vBaseLevel);	tBaseLevel	= vBaseLevel==getBaseLevel();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 		&vMaxLevel);	tMaxLevel	= vMaxLevel==getMaxLevel();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 			&vSWrap);	tSWrap		= vSWrap==getSWrapping();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 			&vTWrap);	tTWrap		= vTWrap==getTWrapping();
                glGetTexParameteriv(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, 			&vMipmapGen);	tMipmapGen	= (vMipmapGen>0 && getBaseLevel()>0) || (vMipmapGen==0 && getBaseLevel()==0);

                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, 		&vWidth);	tWidth		= vWidth==getWidth();
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, 		&vHeight);	tHeight		= vHeight==getHeight();
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, 	&vMode);	tMode		= vMode==getGLMode();
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BORDER, 		&vBorder);	tBorder		= vBorder==0;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, 	&vCompressed);	tCompressed	= (vCompressed>0 && isCompressed()) || (vCompressed==0 && !isCompressed());

                if(verbose)
		{
			std::cout << __HERE__ << "HdlTexture::checkForConsistency - Checking texture : " << getID() << " == " << glId << std::endl;

			std::cout << "    - Width       : ";
			if(tWidth)		std::cout << "OK (" << getWidth() << ')';
			else			std::cout << "FAILED (API : " << getWidth() << "; GL : " << vWidth << ')';
			std::cout << std::endl;

			std::cout << "    - Height      : ";
			if(tHeight)		std::cout << "OK (" << getHeight() << ')';
			else			std::cout << "FAILED (API : " << getHeight() << "; GL : " << vHeight << ')';
			std::cout << std::endl;

			std::cout << "    - Mode        : ";
			if(tMode)		std::cout << "OK (" << glParamName(getGLMode()) << ')';
			else			std::cout << "FAILED (API : " << glParamName(getGLMode()) << "; GL : " << glParamName(vMode) << ')';
			std::cout << std::endl;

			std::cout << "    - Compression : ";
			if(tCompressed)		std::cout << "OK (" << isCompressed() << ')';
			else			std::cout << "FAILED (API : " << isCompressed() << "; GL : " << vCompressed << ')';
			std::cout << std::endl;

			std::cout << "    - MagFilter   : ";
			if(tMagFilter)		std::cout << "OK (" << glParamName(getMagFilter()) << ')';
			else			std::cout << "FAILED (API : " << glParamName(getMagFilter()) << "; GL : " << glParamName(vMagFilter) << ')';
			std::cout << std::endl;

			std::cout << "    - MinFilter   : ";
			if(tMinFilter)		std::cout << "OK (" << glParamName(getMinFilter()) << ')';
			else			std::cout << "FAILED (API : " << glParamName(getMinFilter()) << "; GL : " << glParamName(vMinFilter) << ')';
			std::cout << std::endl;

			std::cout << "    - BaseLevel   : ";
			if(tBaseLevel)		std::cout << "OK (" << getBaseLevel() << ')';
			else			std::cout << "FAILED (API : " << getBaseLevel() << "; GL : " << vBaseLevel << ')';
			std::cout << std::endl;

			std::cout << "    - MaxLevel    : ";
			if(tMaxLevel)		std::cout << "OK (" << getMaxLevel() << ')';
			else			std::cout << "FAILED (API : " << getMaxLevel() << "; GL : " << vMaxLevel << ')';
			std::cout << std::endl;

			std::cout << "    - SWrap       : ";
			if(tSWrap)		std::cout << "OK (" << glParamName(getSWrapping()) << ')';
			else			std::cout << "FAILED (API : " << glParamName(getSWrapping()) << "; GL : " << glParamName(vSWrap) << ')';
			std::cout << std::endl;

			std::cout << "    - TWrap       : ";
			if(tTWrap)		std::cout << "OK (" << glParamName(getTWrapping()) << ')';
			else			std::cout << "FAILED (API : " << glParamName(getTWrapping()) << "; GL : " << glParamName(vTWrap) << ')';
			std::cout << std::endl;

			std::cout << "    - MipMapGen   : ";
			if(tMipmapGen)		std::cout << "OK (" << (getBaseLevel()>0) << ')';
			else			std::cout << "FAILED (API : " << (getBaseLevel()>0) << "; GL : " << vMipmapGen << ')';
			std::cout << std::endl;

			std::cout << "    - Borders     : ";
			if(tBorder)		std::cout << "OK (0)";
			else			std::cout << "FAILED (API : 0; GL : " << vBorder << ')';
			std::cout << std::endl;
		}

		return tMagFilter && tMinFilter && tBaseLevel && tMaxLevel && tSWrap && tTWrap && tMipmapGen && tWidth && tHeight && tMode && tBorder && tCompressed;
	}

	/**
	\fn const __ReadOnly_HdlTextureFormat& HdlTexture::format(void) const
	\brief Explicitly return a constant reference to a __ReadOnly_HdlTextureFormat object describing the format of this texture.
	\return A constant reference to a __ReadOnly_HdlTextureFormat object.
	**/
	const __ReadOnly_HdlTextureFormat& HdlTexture::format(void) const
	{
		return *this;
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

