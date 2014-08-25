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

// Constants : 
	const std::string versionString = 
	"GLIP-COMPUTE V1.0, Built on " __DATE__ ".";

	const std::string helpString = 
	"GLIP-COMPUTE\n\
	 Use GLIP-Lib from the command line to process or generate images .\n\
	 glip-compute [-p pipeline] [-u uniforms] [-i {1, 2, 3, ...} filename] [-o {1, 2, 3, ...} filename]\
	";

// Tools : 
	int parseArguments(int argc, char** argv, std::string& pipelineFilename, std::string& uniformsFilename, std::string& inputFormatString, std::map<std::string, std::string>& inputs, std::map<std::string, std::string>& outputs)
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
		pipelineFilename.clear();
		uniformsFilename.clear();
		inputs.clear();	
		outputs.clear();
		inputFormatString = "inputFormat%d";

		// Parse : 
		for(std::vector<std::string>::iterator it=(arguments.begin() + 1); it!=arguments.end(); it++)
		{
			const std::string arg = *it;

			if(arg=="-v" || arg=="--version")
			{
				std::cout << versionString << std::endl;
				return 0;
			}
			else if(arg=="-h" || arg=="--help")
			{
				std::cout << helpString << std::endl;
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
					uniformsFilename = *it;
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
					inputs[name] = filename;
				else if(arg=="-o" || arg=="--output")
					outputs[name] = filename;
				else
					RETURN_ERROR(-1, "[INTERNAL] Missing name/filename description for argument " << arg << ".")
			}
			else if(arg=="-ifmt" || arg=="--inputFormatStringFormat")
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

		// Test presence of main arguments : 
		if(pipelineFilename.empty())
			RETURN_ERROR(-1, "Missing pipeline filename.")

		if(outputs.empty())
			RETURN_ERROR(-1, "Output filenames list is empty.")

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

	void sortPorts(const Glip::Modules::LayoutLoader::PipelineScriptElements& elements, const std::map<std::string, std::string>& inputs, const std::map<std::string, std::string>& outputs, std::vector<std::string>& inputsSorted, std::vector<std::string>& outputsSorted)
	{
		// Init : 
		inputsSorted.assign(elements.mainPipelineInputs.size(), "");
		outputsSorted.assign(elements.mainPipelineOutputs.size(), "");

		// Scan all given inputs : 
		for(std::map<std::string, std::string>::const_iterator it=inputs.begin(); it!=inputs.end(); it++)
		{
			// Try to identify it as a name : 
			int id = getIndex(elements.mainPipelineInputs, it->first);

			if(id<0)
			{
				if(!Glip::from_string(it->first, id))	
					throw Glip::Exception("The pipeline " + elements.mainPipeline + " has no input ports named (or indexed as) \"" + it->first + "\".", __FILE__, __LINE__);
			}

			if(id<0 || id>=elements.mainPipelineInputs.size())
				throw Glip::Exception("Input port index " + Glip::to_string(id) + " is out of range (pipeline " + elements.mainPipeline + " has " + Glip::to_string(elements.mainPipelineInputs.size()) + " input port(s), indexing start at 0).", __FILE__, __LINE__);
			
			// Set : 
			inputsSorted[id] = it->second;
		}

		// Test the inputs : 
		for(std::vector<std::string>::iterator it=inputsSorted.begin(); it!=inputsSorted.end(); it++)
		{
			if(it->empty())
				throw Glip::Exception("Filename for input " + elements.mainPipelineInputs[std::distance(inputsSorted.begin(), it)] + " was not defined (is empty).", __FILE__, __LINE__);
		}

		// Scan all given outputs : 
		for(std::map<std::string, std::string>::const_iterator it=outputs.begin(); it!=outputs.end(); it++)
		{
			int id = getIndex(elements.mainPipelineOutputs, it->first);

			if(id<0)
			{
				// Try to identify it as a number : 
				if(!Glip::from_string(it->first, id))	
					throw Glip::Exception("The pipeline " + elements.mainPipeline + " has no output ports named (or indexed as) \"" + it->first + "\".", __FILE__, __LINE__);
			}

			if(id<0 || id>=elements.mainPipelineOutputs.size())
				throw Glip::Exception("Output port index " + Glip::to_string(id) + " is out of range (pipeline " + elements.mainPipeline + " has " + Glip::to_string(elements.mainPipelineOutputs.size()) + " output port(s), indexing start at 0).", __FILE__, __LINE__);
			
			// Set : 
			outputsSorted[id] = it->second;
		}
	}

	int compute(const std::string& pipelineFilename, const std::string& uniformsFilename, const std::string& inputFormatString, const std::map<std::string, std::string>& inputs, const std::map<std::string, std::string>& outputs)
	{
		int returnCode = 0;

		Glip::CorePipeline::Pipeline* pipeline = NULL;
		std::vector<Glip::CoreGL::HdlTexture*> inputTextures;

		try
		{
			// Create the GL context : 
			createWindowlessContext();

			// Start GL : 
			Glip::HandleOpenGL::init();

			// Create the loader, load the standard modules : 
			Glip::Modules::LayoutLoader lloader;
			Glip::Modules::LayoutLoaderModule::addBasicModules(lloader);

			// Analyze the pipeline : 
			Glip::Modules::LayoutLoader::PipelineScriptElements elements = lloader.listElements(pipelineFilename);
	
			// Test number of outputs : 
			if(elements.mainPipelineInputs.size()>inputs.size())
				throw Glip::Exception("The pipeline " + elements.mainPipeline + " has " + Glip::to_string(elements.mainPipelineInputs.size()) + " input port(s) but only " + Glip::to_string(inputs.size()) + " input filenames were given.", __FILE__, __LINE__);
			if(elements.mainPipelineOutputs.size()>outputs.size())
				throw Glip::Exception("The pipeline " + elements.mainPipeline + " has " + Glip::to_string(elements.mainPipelineOutputs.size()) + " output port(s) but only " + Glip::to_string(outputs.size()) + " output filenames were given.", __FILE__, __LINE__);

			// Sort : 
			std::vector<std::string> 	inputsSorted,
							outputsSorted;
			sortPorts(elements, inputs, outputs, inputsSorted, outputsSorted);

			// Load the input images in the correct order :
			for(std::vector<std::string>::iterator it=inputsSorted.begin(); it!=inputsSorted.end(); it++)
				inputTextures.push_back(loadImage(*it));

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

			// Load : 
			Glip::CorePipeline::__ReadOnly_PipelineLayout pLayout = lloader(pipelineFilename);
				
			// Prepare the pipeline : 
			pipeline = new Glip::CorePipeline::Pipeline(pLayout, "GlipComputePipeline");

			// Connect the inputs :  
			for(std::vector<Glip::CoreGL::HdlTexture*>::iterator it=inputTextures.begin(); it!=inputTextures.end(); it++)
				(*pipeline) << *(*it);

			// Compute :
			(*pipeline) << Glip::CorePipeline::Pipeline::Process;

			// Save the outputs : 
			for(int k=0; k<pipeline->getNumOutputPort(); k++)
			{
				if(!outputsSorted[k].empty())
					saveImage(pipeline->out(k), outputsSorted[k]);
			}
		}
		catch(Glip::Exception& e)
		{
			std::cerr << e.what();
			returnCode = -1;
		}

		delete pipeline;
		for(std::vector<Glip::CoreGL::HdlTexture*>::iterator it=inputTextures.begin(); it!=inputTextures.end(); it++)
			delete (*it);

		return returnCode;
	}

