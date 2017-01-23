/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : main.cpp                                                                                  */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Main function.                                                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */
	
// Includes : 
	#include "GlipCompute.hpp"

// Main :
	int main(int argc, char** argv)
	{
		int				returnCode = 0;
		size_t				memorySize = 0;	
		GCFlags				flags;
		std::string 			pipelineFilename,
						inputFormatString,
						displayName;
		std::vector<ProcessCommand> 	commands;
		returnCode = parseArguments(argc, argv, pipelineFilename, memorySize, flags, inputFormatString, displayName, commands);	
		if(returnCode==0)
			returnCode = compute(pipelineFilename, memorySize, flags, inputFormatString, displayName, commands);
		return returnCode;
	}

