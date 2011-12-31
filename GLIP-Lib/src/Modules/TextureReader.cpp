/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : TextureReader.cpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Module : Texture Reader                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    TextureReader.cpp
 * \brief   Module : Texture Reader
 * \author  R. KERVICHE
 * \date    October 17th 2010
**/

	#include<limits>
	#include "TextureReader.hpp"
	#include "../Core/Exception.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	/**
	\fn TextureReader::TextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format)
	\brief TextureReader constructor.
	\param name Name of the component.
	\param format Format expected.
	**/
	TextureReader::TextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format)
	 : OutputDevice(name), __ReadOnly_HdlTextureFormat(format), xFlip(false), yFlip(false)
	{
		char* tmp = new char[getSize()];
		data = reinterpret_cast<void*>(tmp);
	}

	TextureReader::~TextureReader(void)
	{
		char* tmp = reinterpret_cast<char*>(data);
		data = NULL;
		delete[] tmp;
	}

	void TextureReader::process(HdlTexture& t)
	{
		if(t!=*this)
			throw Exception("TextureReader::process - Can not read texture having different layout format", __FILE__, __LINE__);

		t.bind();

		glGetTexImage(GL_TEXTURE_2D, 0, getGLMode(), getGLDepth(), reinterpret_cast<GLvoid*>(data));

		HdlTexture::unbind();
	}

	/**
	\fn double TextureReader::operator()(int x, int y, int c)
	\brief Read data in the texture (after calling << on a texture object reference).
	\param x The X position.
	\param y The y position.
	\param c The channel to be read (0,1,2[,3]).
	\return A normalized intensity (in [0.0;1.0]), except for floatting point textures.
	**/
	double TextureReader::operator()(int x, int y, int c)
	{
		int 	xt = x,
			yt = y;
		if(xFlip) x = getWidth()-x-1;
		if(yFlip) y = getHeight()-y-1;
		int pos = (y*getWidth()+x)*getChannel()+c;
		double res = 0.0;

		GLenum d = getGLDepth();

		if(d==GL_BYTE)
		{
			char* s = reinterpret_cast<char*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<char>::max());
		}

		if(d==GL_UNSIGNED_BYTE)
		{
			unsigned char* s = reinterpret_cast<unsigned char*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<unsigned char>::max());
		}

		if(d==GL_SHORT)
		{
			short* s = reinterpret_cast<short*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<short>::max());
		}

		if(d==GL_UNSIGNED_SHORT)
		{
			unsigned short* s = reinterpret_cast<unsigned short*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<unsigned short>::max());
		}

		if(d==GL_INT)
		{
			int* s = reinterpret_cast<int*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<int>::max());
		}

		if(d==GL_UNSIGNED_INT)
		{
			unsigned int* s = reinterpret_cast<unsigned int*>(data);
			res = static_cast<double>(s[pos])/static_cast<double>(std::numeric_limits<unsigned int>::max());
		}

		if(d==GL_FLOAT)
		{
			float* s = reinterpret_cast<float*>(data);
			res = static_cast<double>(s[pos]);
		}

		if(d==GL_DOUBLE)
		{
			double* s = reinterpret_cast<double*>(data);
			res = s[pos];
		}

		return res;
	}


