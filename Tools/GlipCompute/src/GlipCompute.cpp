/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GlipCompute.cpp                                                                           */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : FreeImage3 interface for image input/output.                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Include : 
	#include "GlipCompute.hpp"
	#include <unistd.h>

// Constants : 
	const std::string versionString = 
	"GLIP-COMPUTE V1.0, Built on " __DATE__ ".";

	const std::string helpString = 
"GLIP-COMPUTE\n\
Use GLIP-Lib from the command line to process or generate images.\n\
glip-compute [-p FILENAME] [-u FILENAME] [-i {1, 2, 3, ...} FILENAME]\n\
	     [-o {1, 2, 3, ...} FILENAME] [-r FILENAME]\n\
\n\
Mandatory argument :\n\
-p, --pipeline	Pipeline filename. See the online documentation for more\n\
		information about pipeline scripts.\n\
		E.g. : -p pipeline.ppl\n\
\n\
Optional, passing the inputs/outputs as aguments (single processing\n\
command) :\n\
 -i, --input	Input to the pipeline, can be indexed by a number or by the\n\
		name of the input port (they do not need to be in the\n\
		correct order), followed by a filename. All the input\n\
		ports must receive an input image.\n\
		E.g. : -i 0 inputImage.ext\n\
		       -i inputPortName inputImage.ext\n\
 -o, --output	Output to the pipeline, can be indexed by a number or by the\n\
		name of the input port (they do not need to be in the\n\
		correct order), followed by a filename. Only the ouput\n\
		ports listed will be saved, others will be discarded\n\
		E.g. : -o 0 outputImage.ext\n\
		       -o outputPortName outputImage.ext\n\
 -u, --uniforms	Load a set of uniform variables script. See the online \n\
		documentation about these.\n\
		E.g. : -u uniformsFile.uvd\n\
\n\
Optional, passing the processing commands as a file (can be used to\n\
generate multiple processing commands using the same pipeline) :\n\
 -r, --process	Load a set of processing commands from a file. See\n\
		the section \"Processing Commands\" for more information\n\
		E.g. : -r commandsFile.txt\n\
\n\
Optional, passing the processing commands from stdin.\n\
\n\
Other options : \n\
 -f, --format	Set how the input format requirements are passed to the\n\
		pipeline. You can use C notation with either %d\n\
		indicating the use of the numerical indexing, or %s\n\
		indicating the use of the port name instead.\n\
		E.g. : -f inputFormat%d\n\
		       -f someName_%s\n\
 -m, --memory	Set the amount of memory (given in MB) available to \n\
		be reusable on the device. This helps reduce significantly\n\
		the overhead induced by loading the image files (textures\n\
		are conserved as long as possible on device, depending on\n\
		their usage frequency).\n\
		Default is 128 MB.\n\
 -h, --help	Show this help and stops.\n\
 -t, --template	Show a list of templates script (Pipeline, Uniforms and \n\
		Command) and stops.\n\
 -v, --version	Show the version and stops.\n\
\n\
PROCESSING COMMANDS\n\
  Processing commands describe which resource to use in order to repeat\n\
the operation with little overhead. This commands are given in a format\n\
close to the format of the Pipeline or Uniforms script. They should be :\n\
\n\
PROCESS\n\
{\n\
	INPUT( inFilename0.ext, inFilename1.ext, ...)\n\
	OUTPUT( outFilename0.ext, outFilename1.ext, ...)\n\
}\n\
\n\
  This description must match the number of input of the pipeline, and be\n\
given in the correct order. As for the outputs, it is possible to discard\n\
one by using the keyword VOID instead of a filename. Filtering can be\n\
specified per input with the following set of commands :\n\
\n\
GL_TEXTURE_MIN_FILTER( filterForInput0, filterForInput1, ...)\n\
GL_TEXTURE_MAG_FILTER( filterForInput0, filterForInput1, ...)\n\
GL_TEXTURE_WRAP_S( filterForInput0, filterForInput1, ...)\n\
GL_TEXTURE_WRAP_T( filterForInput0, filterForInput1, ...)\n\
\n\
It is also possible\n\
to supply specific Uniforms variables on a per command basis via two\n\
methods. The first, from a file, by adding the following line in the body\n\
of the command :\n\
\n\
UNIFORMS( filename.uvd )\n\
\n\
  The second adding the code directly via the body :\n\
\n\
UNIFORMS\n\
{\n\
	// Uniforms description goes here.\n\
}\n\
\n\
glip-compute is part of the GLIP-Lib project.\n\
Link : <http://glip-lib.sourceforge.net/>\
";

	const std::string templateString = 
"GLIP-Lib, Blank structures / Templates\n\
--------------------------------------------------------------------------------\n\
BLANK PIPELINE COMPONENTS\n\
\n\
REQUIRED_FORMAT: /* name to use */( /* required format name */ )\n\
\n\
TEXTURE_FORMAT: /* name */ ( /* width */, /* height */, GL_RGB, \n\
	GL_UNSIGNED_BYTE)\n\
\n\
SHADER_SOURCE: /* name */\n\
{\n\
	#version 130\n\
\n\
	uniform sampler2D /* input texture name */;\n\
	out vec4 /* output texture name */;\n\
\n\
	// uniform vec3 someVar= vec3(0.0, 0.0, 0.0);\n\
\n\
	void main()\n\
	{\n\
		// The current fragment position :\n\
		vec2 pos = gl_TexCoord[0].st;\n\
		// Read the base level of the texture :\n\
		vec4 col = textureLod(/* input texture name */, pos, 0);\n\
\n\
		/* output texture name */ = col;\n\
	}\n\
}\n\
\n\
FILTER_LAYOUT: /* name */ ( /* output format name */, /* shader name */)\n\
\n\
PIPELINE_MAIN: /* name */\n\
{\n\
	INPUT_PORTS( /* list of ports names */ )\n\
	OUTPUT_PORTS( /* list of ports names */ )\n\
\n\
	FILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\
}\n\
\n\
--------------------------------------------------------------------------------\n\
BLANK UNIFORMS : \n\
\n\
PIPELINE:/* pipeline name */\n\
{\n\
	FILTER:/* filter instance */\n\
	{\n\
		/* GL Type */ : /* name */ ( /*values */ )\n\
		// E.g. :\n\
		GL_FLOAT_MAT3:M(0.3, 0.3, 0.4, 0.3, 0.3, 0.4, 0.3, 0.3, 0.4)\n\
	}\n\
}\n\
\n\
--------------------------------------------------------------------------------\n\
BLANK PROCESSING COMMANDS : \n\
\n\
PROCESS : /* name, optional */\n\
{\n\
	INPUT( /* inputs */)\n\
	OUTPUT( /* outputs */)\n\
	UNIFORMS\n\
	{\n\
		/* Uniforms */\n\
	}\n\
}\n\
\n\
PROCESS : /* name, optional */\n\
{\n\
	INPUT( /* inputs */)\n\
	OUTPUT( /* outputs */)\n\
	UNIFORMS( /* filename */ )\n\
}\n\
\n\
--------------------------------------------------------------------------------\n\
glip-compute is part of the GLIP-Lib project.\n\
Link : <http://glip-lib.sourceforge.net/>\
";
	ProcessCommand::ProcessCommand(void)
	 : 	line(1),
		uniformsLine(1)
	{ }

	void ProcessCommand::setSafeParameterSettings(void)
	{
		#define TEST_AND_FILL( targetVector, defaultValue ) \
			if(! targetVector .empty() && targetVector.size()!=inputFilenames.size()) \
				throw Glip::Exception("ProcessCommand::setSafeParameterSettings - " STR(targetVector) " is not empty but does not contain the same number of elements as the inputs list (internal error).", __FILE__, __LINE__); \
			else if( targetVector .empty()) \
				targetVector .assign(inputFilenames.size(), defaultValue );
			
			TEST_AND_FILL( inputMinFilterSettings , GL_NEAREST )
			TEST_AND_FILL( inputMagFilterSettings ,GL_NEAREST )
			TEST_AND_FILL( inputWrapSSettings , GL_CLAMP )
			TEST_AND_FILL( inputWrapTSettings , GL_CLAMP )

		#undef TEST_AND_FILL
	}

// Tools : 
	std::string loadUniforms(const std::string& str)
	{
		if(str.find('\n')==std::string::npos)
		{
			std::fstream file;

			file.open(str.c_str(), std::fstream::in);

			if(!file.is_open())
				throw Glip::Exception("loadUniforms - Cannot open file \"" + str + "\".", __FILE__, __LINE__);

			std::stringstream sstr;
			sstr << file.rdbuf();

			file.close();

			return sstr.str();
		}
		else
			return str;
	}

	void readProcessCommandFile(const std::string& str, std::vector< ProcessCommand >& commands)
	{
		std::string 	conditionalFilename,
				content,
				defaultUniformVariables;

		// If str does not contain a new line, we assume it is a filename : 
		if(str.find('\n')==std::string::npos)
		{
			std::fstream file;

			file.open(str.c_str(), std::fstream::in);

			if(!file.is_open())
				throw Glip::Exception("readProcessCommandFile - Cannot open file \"" + str + "\".", __FILE__, __LINE__);

			std::stringstream sstr;
			sstr << file.rdbuf();
			content = sstr.str();

			file.close();

			conditionalFilename = ", in file \"" + str + "\"";
		}
		else
			content = str;

		// Read the structure : 
		Glip::Modules::VanillaParserSpace::VanillaParser parser(content);

		for(std::vector<Glip::Modules::VanillaParserSpace::Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
		{
			if(it->strKeyword=="PROCESS")
			{
				// Test : 
				if(!it->noArgument)
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" cannot have an argument list (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
				if(it->noBody || it->body.empty())
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" does not have a body (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

				ProcessCommand command;

				// Copy the name if needed : 
				if(!it->noName && !it->name.empty())
					command.name = it->name;

				// Read the body : 
				Glip::Modules::VanillaParserSpace::VanillaParser subParser(it->body, it->bodyLine);

				for(std::vector<Glip::Modules::VanillaParserSpace::Element>::iterator itSub=subParser.elements.begin(); itSub!=subParser.elements.end(); itSub++)
				{
					if(itSub->strKeyword=="INPUT" || itSub->strKeyword=="OUTPUT")
					{
						// Test : 
						if(itSub->strKeyword=="INPUT" && !command.inputFilenames.empty())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" already set (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(itSub->strKeyword=="OUTPUT" && !command.outputFilenames.empty())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" already set (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(!itSub->noName)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" cannot have a name (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(itSub->noArgument)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" must have an argument list (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(!itSub->noBody)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" cannot have a body (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

						// Load : 
						std::vector< std::pair<std::string, std::string> > filenamesList;
						
						for(std::vector<std::string>::iterator itArg=itSub->arguments.begin(); itArg!=itSub->arguments.end(); itArg++)
							filenamesList.push_back( std::pair<std::string, std::string>( "", *itArg) );			

						// Set : 
						if(itSub->strKeyword=="INPUT")
							command.inputFilenames = filenamesList;
						else
							command.outputFilenames = filenamesList;
					}
					else if(itSub->strKeyword==Glip::glParamName(GL_TEXTURE_MAG_FILTER) || itSub->strKeyword==Glip::glParamName(GL_TEXTURE_MIN_FILTER) || itSub->strKeyword==Glip::glParamName(GL_TEXTURE_WRAP_S) || itSub->strKeyword==Glip::glParamName(GL_TEXTURE_WRAP_T))
					{
						// Test :
						if(command.inputFilenames.empty())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" was defined before the INPUT command.", __FILE__, __LINE__);
						if(itSub->arguments.size()!=command.inputFilenames.size())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" contains " + Glip::toString(itSub->arguments.size()) + " arguments while " + Glip::toString(command.inputFilenames.size()) + " inputs were defined.", __FILE__, __LINE__);

						// Find the target :
						GLenum targetIndex = Glip::glFromString(itSub->strKeyword);
						std::vector<unsigned int>* target = NULL;

						// Test the target :		
						switch(targetIndex)
						{
							case GL_TEXTURE_MAG_FILTER :
								target = &command.inputMinFilterSettings;
								break;
							case GL_TEXTURE_MIN_FILTER :
								target = &command.inputMagFilterSettings;
								break;
							case GL_TEXTURE_WRAP_S :
								target = &command.inputWrapSSettings;
								break;
							case GL_TEXTURE_WRAP_T :
								target = &command.inputWrapTSettings;
								break;
							default :
								throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" is not referenced (internal error).", __FILE__, __LINE__);
						}
		
						if(!target->empty())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\", data was already set.", __FILE__, __LINE__);

						// Load the values :
						for(std::vector<std::string>::iterator itArg=itSub->arguments.begin(); itArg!=itSub->arguments.end(); itArg++)
						{
							GLenum glArg = Glip::glFromString(*itArg);
	
							if(glArg==GL_FALSE)
								throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\", cannot read argument : \"" + (*itArg) + "\".", __FILE__, __LINE__);
							else
								target->push_back(glArg);			
						}
					}
					else if(itSub->strKeyword=="UNIFORMS")
					{
						if(!command.uniformVariables.empty())
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" already set (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(!itSub->noName)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" cannot have a name (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(itSub->noArgument && itSub->noBody)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" misses content in body or filename in argument (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
						if(!itSub->noArgument && !itSub->noBody)
							throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" cannot have both body and arguments (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
							
						if(!itSub->noArgument) // Filename to load
						{
							if(itSub->arguments.size()!=1)
								throw Glip::Exception("readProcessCommandFile - Command \"" + itSub->strKeyword + "\" only take one argument (line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

							std::fstream file;

							file.open(itSub->arguments.front().c_str(), std::fstream::in);

							if(!file.is_open())
								throw Glip::Exception("readProcessCommandFile - Cannot open file \"" + str + "\" to read uniform variables (from line " + Glip::toString(itSub->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

							std::stringstream sstr;
							sstr << file.rdbuf();
							command.uniformVariables = sstr.str();
							command.uniformsLine = 1;

							file.close();
						}
						else // Body to copy : 
						{
							command.uniformVariables = itSub->body + "\n";
							command.uniformsLine = itSub->startLine;
						}
					}
					else
						throw Glip::Exception("readProcessCommandFile - Unknown property : \"" + itSub->strKeyword + "\" (line " + Glip::toString(itSub->startLine) + ").", __FILE__, __LINE__);
				}

				// Test command nature : 
				if(command.inputFilenames.empty())
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" misses inputs list (line " + Glip::toString(it->startLine) + ").", __FILE__, __LINE__);
				if(command.outputFilenames.empty())
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" misses outputs list (line " + Glip::toString(it->startLine) + ").", __FILE__, __LINE__);
				if(command.uniformVariables.empty())
					command.uniformVariables = defaultUniformVariables;

				commands.push_back(command);
			}
			else if(it->strKeyword=="DEFAULT_UNIFORMS")
			{
				if(!it->noName)
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" cannot have a name (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
				if(it->noArgument && it->noBody)
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" misses content in body or filename in argument (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
				if(!it->noArgument && !it->noBody)
					throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" cannot have both body and arguments (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);
				
				if(!it->noArgument) // Filename to load
				{
					if(it->arguments.size()!=1)
						throw Glip::Exception("readProcessCommandFile - Command \"" + it->strKeyword + "\" only take one argument (line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

					std::fstream file;

					file.open(it->arguments.front().c_str(), std::fstream::in);

					if(!file.is_open())
						throw Glip::Exception("readProcessCommandFile - Cannot open file \"" + str + "\" to read uniform variables (from line " + Glip::toString(it->startLine) + conditionalFilename + ").", __FILE__, __LINE__);

					std::stringstream sstr;
					sstr << file.rdbuf();
					defaultUniformVariables = sstr.str();

					file.close();
				}
				else // Body to copy : 
					defaultUniformVariables = it->body;
			}
			else
				throw Glip::Exception("readProcessCommandFile - Unknown command : \"" + it->strKeyword + "\" (line " + Glip::toString(it->startLine) + ").", __FILE__, __LINE__);
		}
	}

	bool isAKeyboard(FILE *fp)
	{
		//# ifdef __STDC__
		//	/* This dirty operation allows gcc -ansi -pedantic */
		//	extern int fileno(FILE *fp);
		//	extern int _isatty(int fn);
		//#endif
		return ((fp != NULL) && isatty(fileno(fp)));
	}

	int parseArguments(int argc, char** argv, std::string& pipelineFilename, size_t& memorySize, std::string& inputFormatString, std::vector<ProcessCommand>& commands)
	{
		#define RETURN_ERROR( code, str ) { std::cerr << str << std::endl; return code ; }

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
		memorySize = 134217728; // 128 MB
		ProcessCommand singleCommand; 
		pipelineFilename.clear();
		commands.clear();
		inputFormatString = "inputFormat%d";

		// Parse : 
		for(std::vector<std::string>::iterator it=(arguments.begin() + 1); it!=arguments.end(); it++)
		{
			const std::string arg = *it;

			if(arg=="-v" || arg=="--version")
			{
				std::cout << versionString << std::endl;
				return 1;
			}
			else if(arg=="-h" || arg=="--help")
			{
				std::cout << helpString << std::endl;
				return 1;
			}
			else if(arg=="-t" || arg=="--template")
			{
				std::cout << templateString << std::endl;
				return 1;
			}
			else if(arg=="-p" || arg=="--pipeline")
			{
				it++;
				if(it!=arguments.end())
					pipelineFilename = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")
			}
			else if(arg=="-u" || arg=="--uniforms")
			{
				it++;
				if(it!=arguments.end())
					singleCommand.uniformVariables = loadUniforms(*it);
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")
			}
			else if(arg=="-i" || arg=="--input" || arg=="-o" || arg=="--output")
			{
				std::string 	name,
						filename;

				it++;

				if(it!=arguments.end())
					name = *it;
				else
					RETURN_ERROR(-1, "Missing name for argument " << arg << ".")

				it++;

				if(it!=arguments.end())
					filename = *it;
				else
					RETURN_ERROR(-1, "Missing name for argument " << arg << ".")

				if(arg=="-i" || arg=="--input")
					singleCommand.inputFilenames.push_back( std::pair<std::string, std::string>(name, filename) );
				else if(arg=="-o" || arg=="--output")
					singleCommand.outputFilenames.push_back( std::pair<std::string, std::string>(name, filename) );
				else
					RETURN_ERROR(-1, "[INTERNAL] Missing name/filename description for argument " << arg << ".")
			}
			else if(arg=="-r" || arg=="--process")
			{
				std::string processCommandsFilename;

				it++;
				if(it!=arguments.end())
					processCommandsFilename = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")

				try
				{
					readProcessCommandFile(processCommandsFilename, commands);
				}
				catch(Glip::Exception& e)
				{
					RETURN_ERROR(-1, e.what());
				}
			}
			else if(arg=="-m" || arg=="--memory")
			{
				std::string memoryStr;

				it++;
				if(it!=arguments.end())
					memoryStr = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")

				if(!Glip::fromString(memoryStr, memorySize))
					RETURN_ERROR(-1, "Cannot read memory size : \"" << memoryStr << "\".")
				else
					memorySize = memorySize * 1024 * 1024; // Transfer to MB
			}
			else if(arg=="-f" || arg=="--format")
			{
				it++;
				if(it!=arguments.end())
					inputFormatString = *it;
				else
					RETURN_ERROR(-1, "Missing filename for argument " << arg << ".")
			}
			else
				RETURN_ERROR(-1, "Unknonwn argument : " << arg << ".")
		}

		// Read the stdin : 
		if(!isAKeyboard(stdin))
		{
			std::string 	stdinContent,
					line;
			while(std::getline(std::cin, line))
			    stdinContent += "\n" + line;
			stdinContent += "\n";
		
			try
			{
				readProcessCommandFile(stdinContent, commands);
			}
			catch(Glip::Exception& e)
			{
				RETURN_ERROR(-1, e.what());
			}
		}

		if(!singleCommand.inputFilenames.empty() || !singleCommand.outputFilenames.empty())
			commands.push_back(singleCommand);

		// Test : 
		if(commands.empty())
			RETURN_ERROR(-1, "No commands were defined.")

		if(inputFormatString.find("%s")==std::string::npos && inputFormatString.find("%d")==std::string::npos)
			RETURN_ERROR(-1, "Input format string format is invalid (missing %s or %d) : \"" << inputFormatString << "\".")	
		else if(inputFormatString.find("%s")!=std::string::npos && inputFormatString.find("%d")!=std::string::npos)
			RETURN_ERROR(-1, "Input format string format is invalid (having both %s and %d) : \"" << inputFormatString << "\".")

		// Ok : 
		return 0;
	}

	int getIndex(const std::vector<std::string>& stringList, const std::string& str)
	{
		std::vector<std::string>::const_iterator it = std::find(stringList.begin(), stringList.end(), str);

		if(it!=stringList.end())
			return std::distance(stringList.begin(), it);
		else
			return -1;
	}

	void sortPorts(const Glip::Modules::LayoutLoader::PipelineScriptElements& elements, const ProcessCommand& command,  std::vector<std::string>& inputsSorted, std::vector<std::string>& outputsSorted)
	{
		// Init : 
		inputsSorted.assign(elements.mainPipelineInputs.size(), "");
		outputsSorted.assign(elements.mainPipelineOutputs.size(), "");

		// Scan all given inputs : 
		int idInput = 0;
		for(std::vector< std::pair<std::string, std::string> >::const_iterator it=command.inputFilenames.begin(); it!=command.inputFilenames.end(); it++)
		{
			int id = -1;

			if(it->first.empty())
				id = idInput;
			else
			{
				// Try to identify it as a name : 
				id = getIndex(elements.mainPipelineInputs, it->first);

				if(id<0)
				{
					if(!Glip::fromString(it->first, id))	
						throw Glip::Exception("The pipeline " + elements.mainPipeline + " has no input ports named (or indexed as) \"" + it->first + "\".", __FILE__, __LINE__);
				}

				if(id<0 || id>=elements.mainPipelineInputs.size())
					throw Glip::Exception("Input port of index " + Glip::toString(id) + " is out of range (pipeline " + elements.mainPipeline + " has " + Glip::toString(elements.mainPipelineInputs.size()) + " input port(s), indexing start at 0).", __FILE__, __LINE__);
			}
			
			// Set : 
			if(id>=0 && id<inputsSorted.size())
				inputsSorted[id] = it->second;
			idInput++;
		}

		// Test the inputs : 
		for(std::vector<std::string>::iterator it=inputsSorted.begin(); it!=inputsSorted.end(); it++)
		{
			if(it->empty())
				throw Glip::Exception("Filename for input " + elements.mainPipelineInputs[std::distance(inputsSorted.begin(), it)] + " was not defined (is empty).", __FILE__, __LINE__);
		}

		// Scan all given outputs : 
		int idOutput = 0;
		for(std::vector< std::pair<std::string, std::string> >::const_iterator it=command.outputFilenames.begin(); it!=command.outputFilenames.end(); it++)
		{
			int id = -1;

			if(it->first.empty())
				id = idOutput;
			else if(it->second=="VOID") // Discarded
				id = -1;
			else
			{
				id = getIndex(elements.mainPipelineOutputs, it->first);

				if(id<0)
				{
					// Try to identify it as a number : 
					if(!Glip::fromString(it->first, id))	
						throw Glip::Exception("The pipeline " + elements.mainPipeline + " has no output ports named (or indexed as) \"" + it->first + "\".", __FILE__, __LINE__);
				}

				if(id<0 || id>=elements.mainPipelineOutputs.size())
					throw Glip::Exception("Output port of index " + Glip::toString(id) + " is out of range (pipeline " + elements.mainPipeline + " has " + Glip::toString(elements.mainPipelineOutputs.size()) + " output port(s), indexing start at 0).", __FILE__, __LINE__);
			}
			
			// Set : 
			if(id>=0 && id<outputsSorted.size())
				outputsSorted[id] = it->second;
			idOutput++;
		}
	}

	int compute(const std::string& pipelineFilename, const size_t& memorySize, const std::string& inputFormatString, std::vector<ProcessCommand>& commands)
	{
		int returnCode = 0;

		Glip::CorePipeline::Pipeline* pipeline = NULL;
		std::vector<Glip::CoreGL::HdlTexture*> inputTextures;
		DeviceMemoryManager* deviceMemoryManager = NULL;

		try
		{
			// Create the GL context : 
			createWindowlessContext();

			// Start GL : 
			Glip::HandleOpenGL::init();

			// Create the loader, load the standard modules : 
			Glip::Modules::LayoutLoader lloader;
			Glip::Modules::LayoutLoaderModule::addBasicModules(lloader);

			// Create the uniforms loader : 
			Glip::Modules::UniformsVarsLoader uloader;

			// Analyze the pipeline : 
			Glip::Modules::LayoutLoader::PipelineScriptElements elements = lloader.listElements(pipelineFilename);

			deviceMemoryManager = new DeviceMemoryManager(memorySize);
	
			for(std::vector<ProcessCommand>::iterator itCommand = commands.begin(); itCommand!=commands.end(); itCommand++)
			{
				std::string commandName;
	
				if(!itCommand->name.empty())
					commandName = " in command " + itCommand->name;

				// Fill in the filter settings :
				itCommand->setSafeParameterSettings();

				// Test number of outputs : 
				if(elements.mainPipelineInputs.size()>itCommand->inputFilenames.size())
					throw Glip::Exception("The pipeline " + elements.mainPipeline + " has " + Glip::toString(elements.mainPipelineInputs.size()) + " input port(s) but only " + Glip::toString(itCommand->inputFilenames.size()) + " input filenames were given" + commandName + ".", __FILE__, __LINE__);
				if(elements.mainPipelineOutputs.size()>itCommand->outputFilenames.size())
					throw Glip::Exception("The pipeline " + elements.mainPipeline + " has " + Glip::toString(elements.mainPipelineOutputs.size()) + " output port(s) but only " + Glip::toString(itCommand->outputFilenames.size()) + " output filenames were given" + commandName + ".", __FILE__, __LINE__);

				// Sort : 
				std::vector<std::string> 	inputsSorted,
								outputsSorted;
				sortPorts(elements, *itCommand, inputsSorted, outputsSorted);

				// Load the input images in the correct order :
				for(int k=0; k<inputsSorted.size(); k++)
				{
					Glip::CoreGL::HdlTexture* texture = deviceMemoryManager->get(inputsSorted[k]); 
					inputTextures.push_back(texture);
					texture->setSetting(GL_TEXTURE_MIN_FILTER, 	itCommand->inputMinFilterSettings[k]);
					texture->setSetting(GL_TEXTURE_MAG_FILTER, 	itCommand->inputMagFilterSettings[k]);
					texture->setSetting(GL_TEXTURE_WRAP_S, 		itCommand->inputWrapSSettings[k]);
					texture->setSetting(GL_TEXTURE_WRAP_T, 		itCommand->inputWrapTSettings[k]);
				}

				// Set the variables :
				const int maxSize = 1024;
				for(int k=0; k<elements.mainPipelineInputs.size(); k++)
				{
					// Generate the name : 
					char buffer[maxSize];
					std::memset(buffer, 0, maxSize);

					if(inputFormatString.find("%s")!=std::string::npos)
						snprintf( buffer, maxSize, inputFormatString.c_str(), elements.mainPipelineInputs[k].c_str());
					else if(inputFormatString.find("%d")!=std::string::npos)
						snprintf( buffer, maxSize, inputFormatString.c_str(), k);
					else
						throw Glip::Exception("Cannot generate input format name from string format : \"" + inputFormatString + "\".", __FILE__, __LINE__);

					lloader.addRequiredElement(std::string(buffer), inputTextures[k]->format());
				}

				// Uniforms : 
				if(!itCommand->uniformVariables.empty())
					uloader.load(itCommand->uniformVariables, false, itCommand->uniformsLine);

				// Load : 
				Glip::CorePipeline::__ReadOnly_PipelineLayout pLayout = lloader(pipelineFilename);
				
				// Prepare the pipeline : 
				pipeline = new Glip::CorePipeline::Pipeline(pLayout, "GlipComputePipeline");

				// Connect the inputs :  
				for(std::vector<Glip::CoreGL::HdlTexture*>::iterator it=inputTextures.begin(); it!=inputTextures.end(); it++)
					(*pipeline) << *(*it);

				// Set the uniforms : 
				if(!uloader.empty())
					uloader.applyTo(*pipeline);

				// Compute :
				(*pipeline) << Glip::CorePipeline::Pipeline::Process;

				// Save the outputs : 
				for(int k=0; k<pipeline->getNumOutputPort(); k++)
				{
					if(!outputsSorted[k].empty())
						saveImage(pipeline->out(k), outputsSorted[k]);
				}

				// Clean : 
				delete pipeline;
				pipeline = NULL;
				inputTextures.clear();

				lloader.clearRequiredElements();
				uloader.clear();
			}
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			returnCode = -1;
		}

		delete deviceMemoryManager;
		delete pipeline;

		return returnCode;
	}

