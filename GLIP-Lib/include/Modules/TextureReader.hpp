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
	#include "../Core/HdlPBO.hpp"

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
		\brief Copy data back from GPU memory to CPU memory (textures must not be compressed on GPU side)
		**/
		class TextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				void* data;

			protected :
				void process(HdlTexture& t);

			public :
				// Data
					/// Set to true if you want to flip X axis while reading.
					bool 	xFlip,
					/// Set to true if you want to flip Y axis while reading.
						yFlip;
				// Functions
				TextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format);
				~TextureReader(void);

				double operator()(int x, int y, int c);
		};

		/**
		\class PBOTextureReader
		\brief Copy data back from GPU memory to CPU memory using a PBO (textures must not be compressed on GPU side)
		**/
		class PBOTextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat, protected HdlPBO
		{
			protected :
				void process(HdlTexture& t);

			public :
				// Functions
				PBOTextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format, GLenum freq);
				~PBOTextureReader(void);

				void* startReadingMemory(void);
				void endReadingMemory(void);

				using __ReadOnly_HdlTextureFormat::getWidth;
				using __ReadOnly_HdlTextureFormat::getHeight;
				using __ReadOnly_HdlTextureFormat::getSize;
		};

		/**
		\class CompressedTextureReader
		\brief Raw copy of data, back from GPU memory to CPU memory for compressed textures.
		**/
		class CompressedTextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				char* data;
				int size;

			protected :
				void process(HdlTexture& t);

			public :
				CompressedTextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format);
				~CompressedTextureReader(void);

				int	getSize(void) const;
				char*	getData(void) const;
				char&	operator[](int i);
		};

		/**
		\class TextureCopier
		\brief Copy data from texture to texture. Will also perform compression/decompression on the fly according to the input and output formats.
		**/
		class TextureCopier : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				HdlTexture	*tex;
				HdlPBO		*pbo;

			protected :
				void process(HdlTexture& t);

			public :
				TextureCopier(const std::string& name, const __ReadOnly_HdlTextureFormat& formatIn, const __ReadOnly_HdlTextureFormat& formatOut);
				~TextureCopier(void);

				HdlTexture& texture(void);
		};
	}
}


#endif

