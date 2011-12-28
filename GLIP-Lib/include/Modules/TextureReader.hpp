/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : TextureReader.hpp                                                                         */
/*     Original Date : October 17th 2010                                                                         */
/*                                                                                                               */
/*     Description   : Module : Texture Reader                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    TextureReader.hpp
 * \brief   Module : Texture Reader
 * \author  R. KERVICHE
 * \version 0.6
 * \date    October 17th 2010
**/

#ifndef __TEXTURE_READER_INCLUDE__
#define __TEXTURE_READER_INCLUDE__

	/* Info :
		Implement a slow readback of GL textures
		Based on glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* img);
	*/

	// Include :
/*	#include "OglInclude.hpp"
	#include "HdlTexture.hpp"
	#include "Devices.hpp"

	// Prototypes
	namespace Glip
	{
		namespace CoreGL
		{
			class __HdlTextureFormat_OnlyData;
		}

		namespace CorePipeline
		{
			class OutputDevice;
		}
	}

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	// Structure
	class TextureReader : public OutputDevice
	{
		private :
		public :
			// Functions
			TextureReader(const std::string& name);

			__HdlTextureFormat_OnlyData& getFormat(void);
			bool read(int i, GLint level, GLenum format, GLenum type, GLvoid* img);
	};
*/

#endif

