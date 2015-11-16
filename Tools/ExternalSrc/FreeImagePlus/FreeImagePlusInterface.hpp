/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : FreeImagePlusInterface.hpp                                                                */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : FreeImage3 interface for image input/output.                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPCOMPUTE_FREEIMAGEPLUSINTERFACE__
#define __GLIPCOMPUTE_FREEIMAGEPLUSINTERFACE__

	#include "GLIPLib.hpp"

namespace FreeImagePlusInterface
{
	extern Glip::Modules::ImageBuffer* loadImage(const std::string& filename);
	extern Glip::CoreGL::HdlTexture* loadTexture(const std::string& filename);
	extern void saveTexture(Glip::CoreGL::HdlTexture& texture, const std::string& filename);
}

#endif

