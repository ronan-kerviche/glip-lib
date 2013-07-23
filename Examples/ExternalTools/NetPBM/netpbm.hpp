/*
	Small NetPBM RAWBITS Reader/Writer
	GLIPLIB Project.

	This code is also changing the endianness of 16 bits RAW files.
*/

	#include <iostream>
	#include <fstream>
	#include <cmath>
	#include <locale>
	#include "GLIPLib.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

namespace NetPBM
{
	class Image
	{
		private : 
			unsigned int 	width,
					height,
					maxval,
					numLayers;
			unsigned char*	bytes;

			bool nextInteger(unsigned int& val, char* buffer, unsigned int& p, const unsigned int bufferLength);

		public : 
			Image(unsigned int w, unsigned int h, unsigned int m, unsigned int layers);
			Image(HdlTexture& texture);
			Image(const std::string& filename);

			~Image(void);

			bool isValid(void) const;
			unsigned int getWidth(void) const;
			unsigned int getHeight(void) const;
			unsigned int getMaximum(void) const;
			unsigned int getNumLayers(void) const;
			unsigned int getSize(void) const;
			unsigned int getBytePerValue(void) const;

			template<typename T>
			T& value(unsigned int x, unsigned int y, int layer=0);

			template<typename T>
			const T& value(unsigned int x, unsigned int y, int layer=0) const;

			bool writeToFile(const std::string& filename);
			bool copyToTexture(HdlTexture& texture);
			HdlTexture* createTexture(void);
	};
}

