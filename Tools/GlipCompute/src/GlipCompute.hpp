/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GlipCompute.hpp                                                                           */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : FreeImage3 interface for image input/output.                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPCOMPUTE__
#define __GLIPCOMPUTE__

	// Includes : 
	#include "CreateWindowlessContext.hpp"
	#include "FreeImagePlusInterface.hpp"

	extern int parseArguments(int argc, char** argv, std::string& pipelineFilename, std::string& uniformsFilename, std::string& inputFormatString, std::map<std::string, std::string>& inputs, std::map<std::string, std::string>& outputs);
	extern int compute(const std::string& pipelineFilename, const std::string& uniformsFilename, const std::string& inputFormatString, const std::map<std::string, std::string>& inputs, const std::map<std::string, std::string>& outputs);

#endif

