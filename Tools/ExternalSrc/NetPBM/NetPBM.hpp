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
	ImageBuffer* loadNetPBMFile(const std::string& filename);
	void saveNetPBMToFile(const ImageBuffer& image, const std::string& filename);
}

