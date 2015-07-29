/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : NetPBM.hpp                                                                                */
/*     Original Date : July 6th 2014                                                                             */
/*                                                                                                               */
/*     Description   : NetPBM interface for GLIPLIB.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */


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

