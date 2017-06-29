/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-FILTERVIDEO                                                                                          */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : GlipFilterVideo.cpp                                                                       */
/*     Original Date : January 22nd 2017                                                                         */
/*                                                                                                               */
/*     Description   : glip-filtervideo utility. Command-line tool for video filtering and processing.           */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Includes :
	#include <cmath>
	#include "CreateWindowlessContext.hpp"
	#include "GlipFilterVideo.hpp"
	#include "VideoStream.hpp"
	#include "VideoRecorder.hpp"
	#include <unistd.h>
	
// Constants :
	const std::string versionString = 
	"GLIP-FILTERVIDEO V1.0, Built on " __DATE__ ".";

const std::string helpString =
//  ----    ----    ----    ----    ----    ----    ----    ----    ----    ----    ----<- 80
"GLIP-FILTERVIDEO\n\
Use GLIP-Lib from the command line to process or generate videos.\n\
glip-filtervideo [-p FILENAME] [-u FILENAME] [-i {1, 2, portName, ...} FILENAME]\n\
		 [-o {1, 2, portName, ...} FILENAME] [-r FILENAME]\n\
 -p, --pipeline	Filename of the processing pipeline to user.\n\
 -i, --input	Input to the pipeline, can be indexed by a number or by the\n\
		name of the input port (they do not need to be in the\n\
		correct order), followed by a filename. All the input\n\
		ports must receive an input video.\n\
		You can also set the beginning time (seek) and change the framerate\n\
		(framerate) which are flotting point numbers. You can specify the name\n\
		of the uniform variables receiving these informations (pipeline wide).\n\
		For instance, the timeVariable will be updated for every frame with the\n\
		current time code (in seconds).\n\
		E.g. : -i 0 inputVideo.ext\n\
		       -i inputPortName inputVideo.ext\n\
		       -i BUNDLE(portName, 0, ...) ...\n\
		       -i ... \"inputVideo.ext(seek, framerate, timeVariable, ...\n\
		          seekVariable, frameRateVariable, totalDurationVariable)\"\n\
 -o, --output	Output to the pipeline, can be indexed by a number or by the\n\
		name of the input port (they do not need to be in the\n\
		correct order), followed by a filename. Only the ouput\n\
		ports listed will be saved, others will be discarded\n\
		E.g. : -o 0 outputVideo.ext\n\
		       -o outputPortName outputVideo.ext\n\
		       -o \"outputVideo.ext(duration, seek, framerate, ...\n\
		          timeVariable, seekVariable, frameRateVariable, ...\n\
		          durationVariable, bitRate, pixelFormat)\"\n\
 -u, --uniforms	Load a set of uniform variables script. See the online \n\
		documentation about these.\n\
		E.g. : -u uniformsFile.uvd\n\
\n\
Other options : \n\
 -f, --format	Set how the input format requirements are passed to the\n\
		pipeline. You can use C notation with either %d\n\
		indicating the use of the numerical indexing, or %s\n\
		indicating the use of the port name instead.\n\
		E.g. : -f inputFormat%d\n\
		       -f someName_%s\n\
 -d, --display	Name of the host, X server and display to target for the\n\
		context.\n\
		E.g. : -d host:xServer.screenId\n\
		       -d localhost:0.0\n\
 -h, --help	Show this help and stops.\n\
 -v, --version	Show the version and stops.\n\
 -V, --Version  Start a context, show the informations and stop.\n\
		You can set the display variable before using this option.\n\
\n\
glip-filtervideo is part of the GLIP-Lib project.\n\
Link : http://glip-lib.net/\
";

#define RETURN_ERROR( code, str ) { std::cerr << str << std::endl; return code ; }

// StreamData and derivatives :
	StreamData::StreamData(void)
	 :	seek(0.0),
		time(0.0),
		duration(0.0)
	{ }

	StreamData::StreamData(const StreamData& c)
	 :	seek(c.seek),
		time(c.time),
		duration(c.duration),
		filename(c.filename),
		timeUniform(c.timeUniform),
		seekUniform(c.seekUniform),
		frameRateUniform(c.frameRateUniform),
		durationUniform(c.durationUniform)
	{ }

	StreamData::~StreamData(void)
	{ }

	InputStreamData::InputStreamData(void)
	 :	maxMipMapLevel(0),
		minFilter(GL_NEAREST),
		magFilter(GL_NEAREST),
		sWrapping(GL_CLAMP),
		tWrapping(GL_CLAMP),
		stream(NULL)
	{ }

	InputStreamData::InputStreamData(const InputStreamData& c)
	 :	StreamData(c),
		maxMipMapLevel(c.maxMipMapLevel),
		minFilter(c.minFilter),
		magFilter(c.magFilter),
		sWrapping(c.sWrapping),
		tWrapping(c.tWrapping),
		ports(c.ports),
		stream(NULL)
	{
		if(c.stream!=NULL)
			throw Glip::Exception("InputStreamData::InputStreamData - Internal error, stream attached to the original object.", __FILE__, __LINE__, Glip::Exception::ClientException);
	}

	InputStreamData::~InputStreamData(void)
	{
		delete stream;
		stream = NULL;
	}

	void InputStreamData::openStreamAtSeek(void)
	{
		stream = new FFMPEGInterface::VideoStream(filename, ports.size(), minFilter, magFilter, sWrapping, tWrapping, maxMipMapLevel);
		stream->seek(seek);
		stream->readNextFrame();
		time = 0.0f;
	}

	OutputStreamData::OutputStreamData(void)
	 :	bitRate(400000),
		pixelFormat(PIX_FMT_YUV420P),
		frameRate(0.0),
		recorder(NULL)
	{ }

	OutputStreamData::OutputStreamData(const OutputStreamData& c)
	 :	StreamData(c),
		bitRate(c.bitRate),
		pixelFormat(c.pixelFormat),
		port(c.port),	
		recorder(NULL)
	{
		if(c.recorder!=NULL)
			throw Glip::Exception("OutputStreamData::OutputStreamData - Internal error, recorder attached to the original object.", __FILE__, __LINE__, Glip::Exception::ClientException);
	}

	OutputStreamData::~OutputStreamData(void)
	{
		delete recorder;
		recorder = NULL;
	}

	void OutputStreamData::startRecorder(const Glip::CoreGL::HdlAbstractTextureFormat& fmt)
	{
		frameRate = FFMPEGInterface::FFMPEGContext::roundFrameRate(frameRate);
		recorder = new FFMPEGInterface::VideoRecorder(filename, fmt, frameRate, bitRate, pixelFormat);
		if(frameRate!=recorder->getFrameRate())
			throw Glip::Exception("OutputStreamData::startRecorder - Internal error, framerate rounding failed.", __FILE__, __LINE__, Glip::Exception::ClientException);
	}
	
// ProcessCommand :
	int ProcessCommand::readInput(const std::string& portStr, const std::string& str)
	{
		const std::string dArg = "*";
		try
		{
			InputStreamData inputData;
			// Read the ports :
			Glip::Modules::VanillaParserSpace::VanillaParser portParser(portStr);
			if(portParser.elements.size()!=1)
				RETURN_ERROR( -1, "Too many elements in argument " << portStr << ".")
			if(portParser.elements.front().noArgument)
				inputData.ports.push_back(portParser.elements.front().strKeyword);
			else
			{
				for(std::vector<std::string>::const_iterator it=portParser.elements.front().arguments.begin(); it!=portParser.elements.front().arguments.end(); it++)
					inputData.ports.push_back(*it);
			}

			// Read the file arguments :
			Glip::Modules::VanillaParserSpace::VanillaParser fileParser(str);
			if(fileParser.elements.size()!=1)
				RETURN_ERROR( -1, "Too many elements in argument " << str << ".")
			const Glip::Modules::VanillaParserSpace::Element& e = fileParser.elements.front();
			const std::vector<std::string>& arguments = e.arguments;	
			inputData.filename = e.strKeyword;
			#define READ_SCALAR(argId, member, testValue, defaultValue) \
				if(arguments.size()>argId && arguments[argId]!=dArg) \
				{ \
					if(!Glip::fromString(arguments[argId], inputData. member ) || inputData. member < testValue) \
						RETURN_ERROR( -1, "Invalid \"" #member "\" argument : " << arguments[argId] << ".") \
				} \
				else \
					inputData. member = defaultValue;

			READ_SCALAR(0, seek, 0.0f, 0.0f)
			READ_SCALAR(9, maxMipMapLevel, 0, 0)
			#undef READ_SCALAR

			#define READ_TEXT(argId, member, defaultValue) \
				if(arguments.size()>argId && arguments[argId]!=dArg) \
				{ \
					inputData. member = arguments[argId]; \
					if(inputData. member .empty()) \
						RETURN_ERROR( -1, "Invalid \"" #member "\" argument : " << arguments[argId] << ".") \
				} \
				else \
					inputData. member = defaultValue;
			
			READ_TEXT(1, timeUniform, 	"")
			READ_TEXT(2, seekUniform, 	"")
			READ_TEXT(3, frameRateUniform, 	"")
			READ_TEXT(4, durationUniform, 	"")
			#undef READ_TEXT

			#define READ_GLENUM(argId, member, defaultValue) \
				if(arguments.size()>argId && arguments[argId]!=dArg) \
				{ \
					inputData. member = Glip::CoreGL::getGLEnum(arguments[argId]); \
				} \
				else \
					inputData. member = defaultValue;

			READ_GLENUM(5, minFilter, GL_NEAREST)
			READ_GLENUM(6, magFilter, GL_NEAREST)
			READ_GLENUM(7, sWrapping, GL_CLAMP)
			READ_GLENUM(8, tWrapping, GL_CLAMP)
			#undef READ_GLENUM
			// Save :
			inputs.push_back(inputData);
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			return -1;
		}
		return 0;
	}

	int ProcessCommand::readOutput(const std::string& portName, const std::string& str)
	{
		const std::string dArg = "*";
		try
		{
			Glip::Modules::VanillaParserSpace::VanillaParser parser(str);
			if(parser.elements.size()!=1)
				RETURN_ERROR( -1, "Too many elements in argument " << str << ".")
			const Glip::Modules::VanillaParserSpace::Element& e = parser.elements.front();
			const std::vector<std::string>& arguments = e.arguments;
			OutputStreamData outputData;
			outputData.port = portName;
			outputData.filename = e.strKeyword;

			#define READ_SCALAR(argId, member, testValue, defaultValue) \
				if(arguments.size()>argId && arguments[argId]!=dArg) \
				{ \
					if(!Glip::fromString(arguments[argId], outputData. member ) || outputData. member < testValue) \
						RETURN_ERROR( -1, "Invalid \"" #member "\" argument : " << arguments[argId] << ".") \
				} \
				else \
					outputData. member = defaultValue;

			READ_SCALAR(0, duration, 0.0f, 0.0f)
			READ_SCALAR(1, seek, 0.0f, 0.0f)
			READ_SCALAR(2, frameRate, 1, 0)
			outputData.frameRate = FFMPEGInterface::FFMPEGContext::roundFrameRate(outputData.frameRate);
			READ_SCALAR(7, bitRate, 1, 400000)
			#undef READ_POSITIVE_SCALAR	

			#define READ_TEXT(argId, member, defaultValue) \
				if(arguments.size()>argId && arguments[argId]!=dArg) \
				{ \
					outputData. member = arguments[argId]; \
					if(outputData. member .empty()) \
						RETURN_ERROR( -1, "Invalid \"" #member "\" argument : " << arguments[argId] << ".") \
				} \
				else \
					outputData. member = defaultValue;
			
			READ_TEXT(3, timeUniform, 	"")
			READ_TEXT(4, seekUniform, 	"")
			READ_TEXT(5, frameRateUniform, 	"")
			READ_TEXT(6, durationUniform, 	"")
			#undef READ_TEXT

			const int argId = 8;
			if(arguments.size()>argId && arguments[argId]!=dArg)
			{
				outputData.pixelFormat = FFMPEGInterface::FFMPEGContext::getPixFormat(arguments[argId]);
				if(outputData.pixelFormat==AV_PIX_FMT_NONE)
					RETURN_ERROR( -1, "Invalid pixelFormat argument : " << arguments[argId] << ".")
			}
			else
				outputData.pixelFormat = PIX_FMT_YUV420P;

			// Save :
			outputs.push_back(outputData);
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			return -1;
		}
		return 0;
	}

// Tools :
	bool isAKeyboard(FILE *fp)
	{
		//# ifdef __STDC__
		//	/* This dirty operation allows gcc -ansi -pedantic */
		//	extern int fileno(FILE *fp);
		//	extern int _isatty(int fn);
		//#endif
		return ((fp != NULL) && isatty(fileno(fp)));
	}

	int printInfos(const std::string& displayName)
	{
		try
		{
			// Create the GL context : 
			createWindowlessContext(displayName);

			// Start GL : 
			Glip::HandleOpenGL::init();

			// Create the loader, load the standard modules : 
			Glip::Modules::LayoutLoader lloader;
			Glip::Modules::LayoutLoaderModule::addBasicModules(lloader);
	
			// Print :
			std::cout << versionString << std::endl;
			std::cout << "Vendor       : " << Glip::HandleOpenGL::getVendorName() << std::endl;
			std::cout << "Renderer     : " << Glip::HandleOpenGL::getRendererName() << std::endl;
			std::cout << "Version      : " << Glip::HandleOpenGL::getVersion() << std::endl;
			std::cout << "GLSL Version : " << Glip::HandleOpenGL::getGLSLVersion() << std::endl;
			std::cout << std::endl;	
	
			std::vector<Glip::Modules::LayoutLoaderModule*> modules = lloader.listModules();
			std::cout << modules.size() << " loaded modules : " << std::endl;
			for(std::vector<Glip::Modules::LayoutLoaderModule*>::const_iterator it=modules.begin(); it!=modules.end(); it++)
				std::cout << (*it)->getManual() << std::endl;
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			return -1;
		}
		return 1;
	}

	int parseArguments(const int& argc, char** argv, std::string& pipelineFilename, std::string& inputFormatString, std::string& displayName, ProcessCommand& command)
	{
		const std::string executable = argv[0];
		// Simple test : 
		if(argc==1)
		{
			std::cerr << "Missing arguments!" << std::endl;
			std::cerr << "Use " << executable << " -h to obtain help." << std::endl;
			return -1;
		}
		
		// Fill arguments array :
		std::vector<std::string> arguments; 
		for(int k=0; k<argc; k++)
			arguments.push_back(argv[k]);

		// Init and defaults :
		pipelineFilename.clear();
		inputFormatString = "inputFormat%d";
		displayName.clear();

		// Parse : 
		for(std::vector<std::string>::iterator it=(arguments.begin() + 1); it!=arguments.end(); it++)
		{
			const std::string arg = *it;

			if(arg=="-v" || arg=="--version")
			{
				std::cout << versionString << std::endl;
				return 1;
			}
			else if(arg=="-V" || arg=="--Version")
			{
				printInfos(displayName);
				return 1;	
			}
			else if(arg=="-h" || arg=="--help")
			{
				std::cout << helpString << std::endl;
				return 1;
			}
			else if(arg=="-t" || arg=="--template")
			{
				std::cout << "TEMPLATE UNAVAILABLE - TODO" << std::endl;
				return 1;
			}
			else if(arg=="-p" || arg=="--pipeline")
			{
				it++;
				if(!pipelineFilename.empty())
					RETURN_ERROR(-1, "Pipeline was already declared.")
				else if(it!=arguments.end())
					pipelineFilename = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")
			}
			else if(arg=="-u" || arg=="--uniforms")
			{
				/*it++;
				if(!singleCommand.uniformVariables.empty())
					RETURN_ERROR(-1, "Uniform was already declared.")
				else if(it!=arguments.end())
					singleCommand.uniformVariables = loadUniforms(*it);
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")*/
				RETURN_ERROR(-1, "Not Supported.")
			}
			else if(arg=="-i" || arg=="--input" || arg=="-o" || arg=="--output")
			{
				std::string 	name,
						filename;
				it++;
				if(it!=arguments.end())
					name = *it;
				else
					RETURN_ERROR(-1, "Missing port name for argument " << arg << ".")
				it++;
				if(it!=arguments.end())
					filename = *it;
				else
					RETURN_ERROR(-1, "Missing file name for argument " << arg << ".")

				int returnCode = 0;
				if(arg=="-i" || arg=="--input")
					returnCode = command.readInput(name, filename);
				else if(arg=="-o" || arg=="--output")
					returnCode = command.readOutput(name, filename);
				else
					RETURN_ERROR(-1, "[INTERNAL] Missing name/filename description for argument " << arg << ".")
				if(returnCode!=0)
					return returnCode;
			}	
			else if(arg=="-f" || arg=="--format")
			{
				it++;
				if(it!=arguments.end())
					inputFormatString = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")
			}	
			else if(arg=="-d" || arg=="--display")
			{
				it++;
				if(it!=arguments.end())
					displayName = *it;
				else
					RETURN_ERROR(-1, "Missing display name for argument " << arg << ".")
			}
			else
				RETURN_ERROR(-1, "Unknonwn argument : " << arg << ".")
		}

		// Test :
		if(pipelineFilename.empty())
			RETURN_ERROR(-1, "No pipeline was defined.")
		if(command.outputs.empty())
			RETURN_ERROR(-1, "No output was defined.")
		if(inputFormatString.find("%s")==std::string::npos && inputFormatString.find("%d")==std::string::npos)
			RETURN_ERROR(-1, "Input format string format is invalid (missing %s or %d) : \"" << inputFormatString << "\".")	
		else if(inputFormatString.find("%s")!=std::string::npos && inputFormatString.find("%d")!=std::string::npos)
			RETURN_ERROR(-1, "Input format string format is invalid (having both %s and %d) : \"" << inputFormatString << "\".")

		// Ok : 
		return 0;
	}
	
	int findPort(const std::string& portName, const std::vector<std::string>& ports)
	{
		for(int k=0; k<static_cast<int>(ports.size()); k++)
		{
			int id = -1;
			if(portName==ports[k] || (Glip::fromString(portName, id) && id==k))
				return k;
		}
		return -1;
	}

	std::vector<Glip::CorePipeline::Filter*> getFiltersDependentOnUniform(Glip::CorePipeline::Pipeline& pipeline, const std::string& name, const GLenum& type)
	{
		std::vector<Glip::CorePipeline::Filter*> filters;
		if(!name.empty())
		{
			for(int f=0; f<pipeline.getNumElements(); f++)
			{
				if(pipeline.getElementKind(f)==Glip::CorePipeline::AbstractPipelineLayout::FILTER)
				{
					Glip::CorePipeline::Filter* ptr = &pipeline[pipeline.getElementID(f)];
					const std::vector<std::string>& uniformsNames = ptr->program().getUniformsNames();
					const std::vector<GLenum>& uniformsTypes = ptr->program().getUniformsTypes();
					std::vector<std::string>::const_iterator it = std::find(uniformsNames.begin(), uniformsNames.end(), name);
					if(it!=uniformsNames.end() && uniformsTypes[std::distance(uniformsNames.begin(), uniformsNames.end())]!=type)
						filters.push_back(ptr);
				}
			}
		}
		return filters;	
	}

	int compute(const std::string& pipelineFilename, const std::string& inputFormatString, const std::string& displayName, ProcessCommand& command)
	{
		int returnCode = 0;
		try
		{
			// Create the GL context : 
			createWindowlessContext(displayName);
			// Start GL : 
			Glip::HandleOpenGL::init();
			// Pipeline loader :
			Glip::Modules::LayoutLoader lloader;
			Glip::Modules::LayoutLoaderModule::addBasicModules(lloader);
			// Uniforms loader :
			Glip::Modules::UniformsLoader uloader;
	
			// Preparse the pipeline data and prepare the required data :
			Glip::Modules::LayoutLoader::PipelineScriptElements elements = lloader.listElements(pipelineFilename);

			// Map the inputs :
			std::vector<std::pair<int,int> > inputSourcesMap(elements.mainPipelineInputs.size(), std::pair<int,int>(-1,-1));
			for(int streamId=0; streamId<static_cast<int>(command.inputs.size()); streamId++)
			{
				for(int ageId=0; ageId<static_cast<int>(command.inputs[streamId].ports.size()); ageId++)
				{
					const int inputPortId = findPort(command.inputs[streamId].ports[ageId], elements.mainPipelineInputs);
					if(inputPortId<0)
						throw Glip::Exception("The input port " + command.inputs[streamId].ports[ageId] + " for the stream " + command.inputs[streamId].filename + " does not exist in the pipeline " + elements.mainPipeline + ".", __FILE__, __LINE__, Glip::Exception::ClientException);
					else
						inputSourcesMap[inputPortId] = std::pair<int,int>(streamId, ageId);
				}
			}
			// Verify all the connections :
			for(int k=0; k<static_cast<int>(elements.mainPipelineInputs.size()); k++)
			{
				if(inputSourcesMap[k].first<0 || inputSourcesMap[k].second<0)
					throw Glip::Exception("The input port " + elements.mainPipelineInputs[k] + " of the pipeline " + elements.mainPipeline + " is not connected.", __FILE__, __LINE__, Glip::Exception::ClientException);
			}
	
			// Map the outputs :
			std::vector<int> outputStreamsMap(elements.mainPipelineOutputs.size(), -1);
			for(int streamId=0; streamId<static_cast<int>(command.outputs.size()); streamId++)
			{
				const int outputPortId = findPort(command.outputs[streamId].port, elements.mainPipelineOutputs);
				if(outputPortId<0)
					throw Glip::Exception("The input port " + command.outputs[streamId].port + " for the stream " + command.outputs[streamId].filename + " does not exist in the pipeline " + elements.mainPipeline + ".", __FILE__, __LINE__, Glip::Exception::ClientException);
				else
					outputStreamsMap[outputPortId] = streamId;
			}
			// Verify all the connections :
			for(int k=0; k<static_cast<int>(elements.mainPipelineOutputs.size()); k++)
			{
				if(outputStreamsMap[k]<0)
					throw Glip::Exception("The output port " + elements.mainPipelineOutputs[k] + " of the pipeline " + elements.mainPipeline + " is not connected.", __FILE__, __LINE__, Glip::Exception::ClientException);
			}

			// Load/open all the input sources :
			for(std::vector<InputStreamData>::iterator it=command.inputs.begin(); it!=command.inputs.end(); it++)
				it->openStreamAtSeek();
	
			// Specify the required format data :
			for(int k=0; k<static_cast<int>(elements.mainPipelineInputs.size()); k++)
			{
				const int maxSize = 65536;
				char buffer[maxSize];
				std::memset(buffer, 0, maxSize);
				int actualLength = 0;

				if(inputFormatString.find("%s")!=std::string::npos)
					actualLength = snprintf( buffer, maxSize, inputFormatString.c_str(), elements.mainPipelineInputs[k].c_str());
				else if(inputFormatString.find("%d")!=std::string::npos)
					actualLength = snprintf( buffer, maxSize, inputFormatString.c_str(), k);
				else
					throw Glip::Exception("Cannot generate input format name from string format : \"" + inputFormatString + "\".", __FILE__, __LINE__, Glip::Exception::ClientException);
				if(actualLength>=maxSize)
					throw Glip::Exception("Cannot generate input format name from string format : \"" + inputFormatString + "\", string is too long.", __FILE__, __LINE__, Glip::Exception::ClientException);

				// Set the required value :
				const std::string name(buffer, actualLength);
				const std::pair<int, int>& mapPair = inputSourcesMap[k];
				lloader.addRequiredElement(name, command.inputs[mapPair.first].stream->format());
			}

			// Load the pipeline :
			Glip::CorePipeline::AbstractPipelineLayout pLayout = lloader.getPipelineLayout(pipelineFilename);
			Glip::CorePipeline::Pipeline pipeline(pLayout, "GlipComputePipeline");

			// Find the master duration and framerate :
			float 	masterFrameRate = 0.0f,
				masterDuration = 0.0f;
			for(std::vector<InputStreamData>::const_iterator it=command.inputs.begin(); it!=command.inputs.end(); it++)
			{
				masterFrameRate = std::max(masterFrameRate, it->stream->getFrameRate());
				masterDuration = std::max(masterDuration, it->stream->getDurationSec());
			}
			for(std::vector<OutputStreamData>::const_iterator it=command.outputs.begin(); it!=command.outputs.end(); it++)
			{
				masterFrameRate = std::max(masterFrameRate, static_cast<float>(it->frameRate));
				masterDuration = std::max(masterDuration, it->duration);
			}
			if(masterFrameRate<=0.0f)
				masterFrameRate = 25.0f; // 25 FPS default.
			if(masterDuration<=0.0f)
				masterDuration = 10.0f; // 10 Seconds default.
			std::cout << "Master framerate : " << masterFrameRate << " FPS." << std::endl;
			std::cout << "Master duration  : " << masterDuration << " seconds." << std::endl;
			// Complete the output streams durations and framerate :
			for(std::vector<OutputStreamData>::iterator it=command.outputs.begin(); it!=command.outputs.end(); it++)
			{
				if(it->duration<=0.0f)
					it->duration = masterDuration;
				if(it->frameRate<=0)
					it->frameRate = masterFrameRate;
			}

			// Create the destination files :
			for(std::vector<int>::iterator it=outputStreamsMap.begin(); it!=outputStreamsMap.end(); it++)
				command.outputs[*it].startRecorder(pipeline.out(*it).format());
	
			// Set the static uniforms data (seek, duration, frameRate, etc.) :
			std::vector<std::pair<std::string, std::vector<Glip::CorePipeline::Filter*> > > inputsTimeDependentFilters,
													outputsTimeDependentFilters;
			for(std::vector<InputStreamData>::const_iterator it=command.inputs.begin(); it!=command.inputs.end(); it++)
			{
				std::vector<Glip::CorePipeline::Filter*> filters;
				#define SET_UNIFORM(uniformString, value) \
					filters = getFiltersDependentOnUniform(pipeline, uniformString, GL_FLOAT); \
					if(!uniformString.empty() && filters.empty()) \
						std::cout << "[WARNING] Could not find variable " << uniformString << " for input " << it->filename << " in any of the filter(s) in pipeline " << pipeline.getLayoutName() << "." << std::endl; \
					for(std::vector<Glip::CorePipeline::Filter*>::iterator itPtr=filters.begin(); itPtr!=filters.end(); itPtr++) \
						(*itPtr)->program().setVar(uniformString, GL_FLOAT, value);
				
				SET_UNIFORM(it->seekUniform, it->seek)
				SET_UNIFORM(it->frameRateUniform, it->stream->getFrameRate())
				SET_UNIFORM(it->durationUniform, it->stream->getDurationSec())
				SET_UNIFORM(it->timeUniform, it->stream->getCurrentTimeSec())
				inputsTimeDependentFilters.push_back(std::pair<std::string, std::vector<Glip::CorePipeline::Filter*> >(it->timeUniform, filters));
				#undef SET_UNIFORM
			}
			for(std::vector<OutputStreamData>::const_iterator it=command.outputs.begin(); it!=command.outputs.end(); it++)
			{
				std::vector<Glip::CorePipeline::Filter*> filters;
				#define SET_UNIFORM(uniformString, value) \
					filters = getFiltersDependentOnUniform(pipeline, uniformString, GL_FLOAT); \
					if(!uniformString.empty() && filters.empty()) \
						std::cout << "[WARNING] Could not find variable " << uniformString << " for output " << it->filename << " in any of the filter(s) in pipeline " << pipeline.getLayoutName() << "." << std::endl; \
					for(std::vector<Glip::CorePipeline::Filter*>::iterator itPtr=filters.begin(); itPtr!=filters.end(); itPtr++) \
						(*itPtr)->program().setVar(uniformString, GL_FLOAT, value);
				
				SET_UNIFORM(it->seekUniform, it->seek)
				SET_UNIFORM(it->frameRateUniform, it->recorder->getFrameRate())
				SET_UNIFORM(it->durationUniform, it->duration)
				SET_UNIFORM(it->timeUniform, 0.0f)
				outputsTimeDependentFilters.push_back(std::pair<std::string, std::vector<Glip::CorePipeline::Filter*> >(it->timeUniform, filters));
				#undef SET_UNIFORM
			}

			// Process :
			float masterClock = 0.0f;
			while(masterClock<=masterDuration)
			{
				for(std::vector<InputStreamData>::iterator it=command.inputs.begin(); it!=command.inputs.end(); it++)
				{
					while(!it->stream->isOver() && masterClock-it->time>0.5f/it->stream->getFrameRate())
					{
						it->stream->readNextFrame();
						it->time += 1.0f/it->stream->getFrameRate();
					}
					// Set time :
					std::pair<std::string, std::vector<Glip::CorePipeline::Filter*> >& filters = inputsTimeDependentFilters[std::distance(command.inputs.begin(), it)];
					for(std::vector<Glip::CorePipeline::Filter*>::iterator itF=filters.second.begin(); itF!=filters.second.end(); itF++)
						(*itF)->program().setVar(filters.first, GL_FLOAT, it->time);
				}
				// Set the outputs uniform timing :
				for(std::vector<std::pair<std::string, std::vector<Glip::CorePipeline::Filter*> > >::iterator it=outputsTimeDependentFilters.begin(); it!=outputsTimeDependentFilters.end(); it++)
					for(std::vector<Glip::CorePipeline::Filter*>::iterator itF=it->second.begin(); itF!=it->second.end(); itF++)
						(*itF)->program().setVar(it->first, GL_FLOAT, command.outputs[std::distance(it->second.begin(), itF)].time);

				// Connect and render :
				for(int k=0; k<pipeline.getNumInputPort(); k++)
				{
					const std::pair<int, int>& p = inputSourcesMap[k];
					pipeline << command.inputs[p.first].stream->frame(p.second);
				}
				pipeline << Glip::CorePipeline::Pipeline::Process;

				// Save the streams for the correct clocks and update the time :
				for(int k=0; k<pipeline.getNumOutputPort(); k++)
				{
					OutputStreamData& output = command.outputs[outputStreamsMap[k]];
					const int 	k1 = static_cast<int>(output.time*output.recorder->getFrameRate()),
							k2 = static_cast<int>((output.time+1.0f/masterFrameRate)*output.recorder->getFrameRate());
					if(k1!=k2)
						output.recorder->record(pipeline.out(k));
					output.time += 1.0f/masterFrameRate;
				}

				if(static_cast<int>(masterClock/10.0f)!=static_cast<int>((masterClock+1.0f/masterFrameRate)/10.0f))
					std::cout << "Time : " << (std::floor(masterClock/10.0f)*10.0f+10.0f) << " seconds ..." << std::endl;
				// Move on :
				masterClock += 1.0f/masterFrameRate;
			}
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			returnCode = -1;
		}
		// Close all streams :

		return returnCode;
	}

