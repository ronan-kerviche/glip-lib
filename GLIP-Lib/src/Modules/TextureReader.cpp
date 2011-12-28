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
 * \version 0.6
 * \date    October 17th 2010
**/

#include "TextureReader.hpp"
#include "Exception.hpp"

	/*TextureReader::TextureReader(const std::string& name) : OutputDevice(name)
	{}*/

	/*__HdlTextureFormat_OnlyData& TextureReader::getFormat(void)
	{
		if(readTexture()!=NULL)
			return reinterpret_cast<__HdlTextureFormat_OnlyData&>(*readTexture());
		else
			throw Exception("TextureReader::getFormat - no texture bound to " + getNameExtended(), __FILE__, __LINE__);
	}

	bool TextureReader::read(int i, GLint level, GLenum format, GLenum type, GLvoid* img)
	{
		if(readTexture()!=NULL)
		{
			readTexture()->bind();
			glGetTexImage(GL_TEXTURE_2D, level, format, type, img);
			HdlTexture::unbind();
			return true;
		}
		else
			throw Exception("TextureReader::read - no texture bound to " + getNameExtended(), __FILE__, __LINE__);
	}*/
