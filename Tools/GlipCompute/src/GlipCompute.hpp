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

	struct ProcessCommand
	{
		std::string						name;
		std::vector< std::pair<std::string, std::string> >	inputFilenames,
									outputFilenames;
		std::string						uniformVariables;
	};

	extern int parseArguments(int argc, char** argv, std::string& pipelineFilename, std::string& inputFormatString, std::vector<ProcessCommand>& commands);
	extern int compute(const std::string& pipelineFilename, const std::string& inputFormatString, const std::vector<ProcessCommand>& commands);

#endif

