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

	struct StreamData
	{
		bool 		isInput;
		int		maxMipMapLevel,
				bitRate,
				frameRate;
		GLenum 		minFilter,
				magFilter,
				sWrapping,
				tWrapping;
		PixelFormat	pixelFormat;
		float		seek,
				time,
				duration;
		std::string	filename,
				timeUniform,
				seekUniform,
				frameRateUniform,
				durationUniform;
		std::vector<std::string> ports; // Ids, Names or Ids.

		StreamData(void);
	};

	struct ProcessCommand
	{
		std::vector<StreamData>	inputs,
					outputs;
		std::string		uniformsFilenanme;
		int readInput(const std::string& portName, const std::string& str);
		int readOutput(const std::string& portName, const std::string& str);
	};

	template<typename T>
	struct AutoVector : public std::vector<T*>
	{
		private :
			AutoVector(const AutoVector<T>&);
		public :
			typedef typename std::vector<T*>::iterator iterator;
			typedef typename std::vector<T*>::const_iterator const_iterator;

			AutoVector(void)
			{ }

			virtual ~AutoVector(void)
			{
				for(iterator it=begin(); it!=end(); it++)
					delete *it;
				clear();
			}

			using std::vector<T*>::begin;
			using std::vector<T*>::end;
			using std::vector<T*>::clear;
			using std::vector<T*>::push_back;
			using std::vector<T*>::front;
			using std::vector<T*>::back;
			using std::vector<T*>::size;
	};

	extern int parseArguments(const int& argc, char** argv, std::string& pipelineFilename, std::string& inputFormatString, std::string& displayName, ProcessCommand& command); 
	extern int compute(const std::string& pipelineFilename, const std::string& inputFormatString, const std::string& displayName, const ProcessCommand& command);

#endif

