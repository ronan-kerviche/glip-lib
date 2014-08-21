/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : main.cpp                                                                                  */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Main function.                                                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

	/*
	TODO 
		- Versions of OpenGL adapted to driver/hardware for maximum functionalities.
		- Alpha layer with freeimage library.
		- Uniforms variables to load from file.
		- Orientation//=coordinates inside the images (flip the horizontal axis?).
		- Load external modules.
	*/

// Includes : 
	#include "GlipCompute.hpp"

// Main :
	int main(int argc, char** argv)
	{
		int returnCode = 0;

		std::string 				pipelineFilename,
							uniformsFilename,
							inputFormatString;
		std::map<std::string, std::string>	inputs,
							outputs;

		returnCode = parseArguments(argc, argv, pipelineFilename, uniformsFilename, inputFormatString, inputs, outputs);
	
		if(returnCode==0)
			returnCode = compute(pipelineFilename, uniformsFilename, inputFormatString, inputs, outputs);

		return returnCode;
	}

