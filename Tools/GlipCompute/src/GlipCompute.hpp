/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
	#include "DeviceMemoryManager.hpp"

	enum GCFlags
	{
		NoFlag			= 0,
		ForcePreservePipeline	= 1
	};

	struct ProcessCommand
	{
		int							line;
		std::string						name;
		std::vector< std::pair<std::string, std::string> >	inputFilenames,		// First String is either the name or the index of the port.
									outputFilenames;	// The second is the name of the resource to plug there.
		std::vector<unsigned int>				inputMinFilterSettings,
									inputMagFilterSettings,
									inputWrapSSettings,
									inputWrapTSettings;
		int							uniformsLine;
		std::string						uniformVariables;

		ProcessCommand(void);
		void setSafeParameterSettings(void);
	};

extern int parseArguments(int argc, char** argv, std::string& pipelineFilename, size_t& memorySize, GCFlags& flags, std::string& inputFormatString, std::string& displayName, std::vector<ProcessCommand>& commands);
	extern int compute(const std::string& pipelineFilename, const size_t& memorySize, const GCFlags& flags, const std::string& inputFormatString, const std::string& displayName, std::vector<ProcessCommand>& commands);

#endif

