/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-FILTERVIDEO                                                                                          */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : GlipFilterVideo.hpp                                                                       */
/*     Original Date : January 22nd 2017                                                                         */
/*                                                                                                               */
/*     Description   : glip-filtervideo utility. Command-line tool for video filtering and processing.           */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIP_FILTERVIDEO__
#define __GLIP_FILTERVIDEO__

// Includes :
	#include <string>
	#include <vector>
	#include "GLIPLib.hpp"
	#include "FFMPEGInterface.hpp"

// Prototypes :
namespace FFMPEGInterface
{
	class VideoStream;
	class VideoRecorder;
}

// Tools and classes :
	struct StreamData
	{
		float		seek,
				time,
				duration;
		std::string	filename,
				timeUniform,
				seekUniform,
				frameRateUniform,
				durationUniform;

		StreamData(void);
		StreamData(const StreamData& c);
		virtual ~StreamData(void);
	};


	struct InputStreamData : public StreamData
	{
		int				maxMipMapLevel;
		GLenum 				minFilter,
						magFilter,
						sWrapping,
						tWrapping;
		std::vector<std::string> 	ports; // Names or Id's.
		FFMPEGInterface::VideoStream*	stream;

		InputStreamData(void);
		InputStreamData(const InputStreamData& c);
		virtual ~InputStreamData(void);
		void openStreamAtSeek(void);
		float getDurationSec(void) const; 
	};

	struct OutputStreamData : public StreamData
	{
		int				bitRate;
		PixelFormat     		pixelFormat;
		float				frameRate;
		std::string			port,
						frameRateUniform;
		FFMPEGInterface::VideoRecorder* recorder;

		OutputStreamData(void);
		OutputStreamData(const OutputStreamData& c);
		virtual ~OutputStreamData(void);
		void startRecorder(const Glip::CoreGL::HdlAbstractTextureFormat& fmt);
	};

	struct ProcessCommand
	{
		std::vector<InputStreamData>	inputs;
		std::vector<OutputStreamData>	outputs;
		std::string			uniformsFilenanme;
		int readInput(const std::string& portName, const std::string& str);
		int readOutput(const std::string& portName, const std::string& str);
	};	

	extern int parseArguments(const int& argc, char** argv, std::string& pipelineFilename, std::string& inputFormatString, std::string& displayName, ProcessCommand& command); 
	extern int compute(const std::string& pipelineFilename, const std::string& inputFormatString, const std::string& displayName, ProcessCommand& command);

#endif

