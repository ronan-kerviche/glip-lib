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

	#include <FreeImagePlus.h>
	#include "GLIPLib.hpp"

	extern Glip::CoreGL::HdlTexture* loadImage(const std::string& filename);
	extern void saveImage(Glip::CoreGL::HdlTexture& texture, const std::string& filename);

#endif

