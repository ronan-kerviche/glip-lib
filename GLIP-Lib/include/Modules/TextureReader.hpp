/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
	#include "Core/OglInclude.hpp"
	#include "Core/HdlTexture.hpp"
	#include "Core/Devices.hpp"
	#include "Core/HdlPBO.hpp"

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
\brief Copy data back from GPU memory (device) to CPU memory/RAM (host) (textures must not be compressed on GPU side).

Example of how to use TextureReader :
\code
	// For reading a known HdlTextureFormat format fmt :
	TextureReader firstReader("FirstReader",fmt);

	// Or directly from a texture :
	TextureReader secondReader("SecondReader",texture.format());

	// Or directly from a pipeline output (which is a reference to a texture) :
	TextureReader thirdReader("ThirdReader",myPipeline.out(0).format());

	// How to use them :
	secondReader << texture << OutputDevice::Process;
	thirdReader << myPipeline.out(0) << OutputDevice::Process;

	// Read the red component of the pixel on second column, third line :
	double value;

	value = secondReader(2,3,1);
	//and value = thirdReader(2,3,1);
\endcode

**/
		class TextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				char* originalPointer;
				void* data;

				OutputDevice::OutputDeviceLayout getLayout(void) const;

			protected :
				void process(void);

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
				const void* buffer(void) const;
				void copy(void* ptr) const;
		};

/**
\class PBOTextureReader
\brief Copy data back from GPU memory (device) to CPU memory/RAM (host) using a PBO (textures must not be compressed on GPU side).

Example of how to use PBOTextureReader :
\code
	// For reading, frequently, a known HdlTextureFormat format fmt :
	PBOTextureReader firstPBOReader("FirstPBOReader",fmt, GL_STREAM_READ);

	// Or directly from a texture :
	PBOTextureReader secondPBOReader("SecondPBOReader",texture.format(), GL_STREAM_READ);

	// Or directly from a pipeline output (which is a reference to a texture) :
	PBOTextureReader thirdPBOReader("ThirdPBOReader",myPipeline.out(0).format(), GL_STREAM_READ);

	// How to use them :
	secondPBOReader << texture << OutputDevice::Process;
	thirdPBOReader << myPipeline.out(0) << OutputDevice::Process;

	// Map the internal PBO to the memory for GL_RGB, GL_UNSIGNED_BYTE :
	unsigned char* ptr = reinterpret_cast<unsigned char*>(secondPBOReader.startReadingMemory());

	// Copy to your buffer :
	memcpy(yourBuffer, ptr, secondPBOReader.getNumPixels()*3);

	// Release the mapped memory :
	secondPBOReader.endReadingMemory();
\endcode
**/
		class PBOTextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat, protected HdlPBO
		{
			private : 
				OutputDevice::OutputDeviceLayout getLayout(void) const;
		
			protected :
				void process(void);

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
\brief Raw copy of data, back from GPU memory (device) to CPU memory/RAM (host) for compressed textures.

This class is working exacly as TextureReader except that you will access the data in a compressed format (no direct channels/pixels access).
**/
		class CompressedTextureReader : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				char* data;
				int size;

				OutputDevice::OutputDeviceLayout getLayout(void) const;

			protected :
				void process(void);

			public :
				CompressedTextureReader(const std::string& name, const __ReadOnly_HdlTextureFormat& format);
				~CompressedTextureReader(void);

				size_t	getSize(void) const;
				char*	getData(void) const;
				char&	operator[](int i);
		};

/**
\class TextureCopier
\brief Copy data from texture to texture. Will also perform compression/decompression on the fly according to the input and output formats.

Example :
\code
	// With identical format :
	HdlTextureFormat fmt(...);
	HdlTexture someTexture(fmt);

	// Create the object, specify that the target texture has to be allocated and managed by the object :
	TextureCopier copier("Copier",fmt,fmt);

	// Copy :
	copier << someTexture << OutputDevice::Process;

	// Now you can use copier.texture() which will be a perfect copy of someTexture.

	// You can also specify another target :
	TextureCopier copier("Copier2",fmt,fmt,false); // false = custom target, do not make allocation for copy target
	copier.provideTexture(&someTargetTexture);

	// Copy :
	copier << someTexture << OutputDevice::Process;

	// Use : 
	somePipeline << someTargetTexture << ... ;
\endcode
**/
		class TextureCopier : public OutputDevice, public __ReadOnly_HdlTextureFormat
		{
			private :
				bool 		customTexture;
				HdlTexture	*targetTexture;
				HdlPBO		*pbo;

				OutputDevice::OutputDeviceLayout getLayout(void) const;

			protected :
				void process(void);

			public :
				TextureCopier(const std::string& name, const __ReadOnly_HdlTextureFormat& formatIn, const __ReadOnly_HdlTextureFormat& formatOut, bool _customTexture=false);
				~TextureCopier(void);

				bool isUsingCustomTargetTexture(void) const;
				void provideTexture(HdlTexture* texture);
				HdlTexture& texture(void);
		};
	}
}


#endif

