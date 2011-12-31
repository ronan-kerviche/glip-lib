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
 * \date    October 17th 2010
**/

#ifndef __TEXTURE_READER_INCLUDE__
#define __TEXTURE_READER_INCLUDE__

	/* Info :
		Implement a slow readback of GL textures
		Based on glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* img);
	*/

	// Include :
	#include "../Core/OglInclude.hpp"
	#include "../Core/HdlTexture.hpp"
	#include "../Core/Devices.hpp"


namespace Glip
{
	// Prototypes
	namespace CoreGL
	{
		class __HdlTextureFormat_OnlyData;
	}

	namespace CorePipeline
	{
		class OutputDevice;
	}

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		// Structure
		/**
		\class TextureReader
		\brief Copy data back from GPU memory to CPU memory
		**/
		class TextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				void* data;

			protected :
				void process(HdlTexture& t);

			public :
				// data
					/// Set to true if you want to flip X axis while reading.
					bool 	xFlip,
					/// Set to true if you want to flip Y axis while reading.
						yFlip;
				// Functions
				TextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format);
				~TextureReader(void);

				double operator()(int x, int y, int c);
		};
	}
}


#endif

