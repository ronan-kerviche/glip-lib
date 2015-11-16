/* ************************************************************************************************************* */
/*                                                                                                               */
/*     LIBRAW INTERFACE                                                                                          */
/*     Interface to the LibRaw library for the OpenGL Image Processing LIBrary                                   */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : LibRawInterface.pp                                                                        */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : LibRaw interface for image input/output.                                                  */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_LIBRAWINTERFACE__
#define __GLIPLIB_LIBRAWINTERFACE__

	#include "GLIPLib.hpp"

namespace LibRawInterface
{
	extern Glip::Modules::ImageBuffer* loadImage(const std::string& filename);
	extern Glip::CoreGL::HdlTexture* loadTexture(const std::string& filename);
}

#endif

