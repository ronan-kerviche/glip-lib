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
		- Write the help.
	*/

// Includes : 
	#include "GlipCompute.hpp"

// Main :
	int main(int argc, char** argv)
	{
		int returnCode = 0;

		size_t				memorySize;	
		std::string 			pipelineFilename,
						inputFormatString,
						displayName;
		std::vector<ProcessCommand> 	commands;

		returnCode = parseArguments(argc, argv, pipelineFilename, memorySize, inputFormatString, displayName, commands);
	
		if(returnCode==0)
			returnCode = compute(pipelineFilename, memorySize, inputFormatString, displayName, commands);

		return returnCode;
	}

