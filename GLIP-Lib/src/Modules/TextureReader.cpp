/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : TextureReader.cpp                                                                        **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Texture Reader                                                                           **/
/**                                                                                                             **/
/*****************************************************************************************************************/

#include "HdlTexture.hpp"
#include "IOModules.hpp"
#include "StreamManager.hpp"
#include "TextureReader.hpp"

	TextureReader::TextureReader(int _nSocket) : InputModule(_nSocket, "Texture reader")
	{}

	__HdlTextureFormat_OnlyData& TextureReader::getFormat(int i)
	{
		return reinterpret_cast<__HdlTextureFormat_OnlyData&>(texture(i));
	}

	bool TextureReader::read(int i, GLint level, GLenum format, GLenum type, GLvoid* img)
	{
		if( connected(i) )
		{
			texture(i).bind();
			glGetTexImage(GL_TEXTURE_2D, level, format, type, img);
			HdlTexture::unbind();
			return true;
		}
		else
			return false;
	}
