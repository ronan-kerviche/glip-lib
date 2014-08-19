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
		- Filtering of inputs.
		- Connect inputs and outputs directly by name.
		- Alpha layer with freeimage library.
		- Uniforms variables to load from file.
		- Orientation//=coordinates inside the images (flip the horizontal axis?).
		- Input variables.
		- Load standard modules.
		- Load external modules.
		- Input format variables.
	*/

// Includes : 
	#include "CreateWindowlessContext.hpp"
	#include "FreeImagePlusInterface.hpp"

// Constants : 
	const std::string versionString = 
	"GLIP-COMPUTE V1.0, Built on " __DATE__ ".";

	const std::string helpString = 
	"GLIP-COMPUTE\n\
	 Use GLIP-Lib from the command line to process or generate images .\n\
	 glip-compute [-p pipeline] [-u uniforms] [-i {1, 2, 3, ...} filename] [-o {1, 2, 3, ...} filename]\
	";

// Arguments tools : 
	bool getArgument(const std::string& arg, int& k, const int& argc, char** argv, std::string& output)
	{
		k++;
	
		if(k>=argc)
		{
			std::cerr << "Missing filename for argument " << arg << "." << std::endl;
			return false;
		}
		else
		{
			output = argv[k];
			return true;
		}
	}

	bool getIndexedArgument(const std::string& arg, int& k, const int& argc, char** argv, std::vector<std::string>& output)
	{
		int idx = 0;

		k++;
	
		if(k>=argc)
		{
			std::cerr << "Missing index for argument " << arg << "." << std::endl;
			return false;
		}
		else
		{
			if(!Glip::from_string(argv[k], idx))
			{
				std::cerr << "Cannot read index : \"" << argv[k] << "\"." << std::endl;
				return false;
			}
		}

		// Next : 
		k++;
	
		if(k>=argc)
		{
			std::cerr << "Missing filename for argument " << arg << " of index " << idx << "." << std::endl;
			return false;
		}
		else
		{
			// Make sure we can access the right element :
			while(output.size()<=idx)
				output.push_back("");
	
			if(!output[idx].empty())
			{
				std::cout << "Cannot replace argument " << arg << " of index " << idx << " from \"" << output[idx] << "\" to \"" << argv[k] << "\"." << std::endl;
			}
			else
				output[idx] = argv[k];
		}

		return true;
	}

// Main :
	int main(int argc, char** argv)
	{
		int returnCode = 0;

		// Data used for the computation : 
		int				verbosity = 0;
		std::string			pipelineFilename,
						uniformsFilename;

		std::vector<std::string>	inputFilenames,
						outputFilenames;

		// Start by sorting and testing the arguments : 
		if(argc==1)
		{
			std::cerr << "Missing arguments!" << std::endl;
			std::cerr << "Use -h argument to obtain help." << std::endl;
			return -1;
		}
	
		for(int k=1; k<argc; k++)
		{
			std::string arg = argv[k];
		
			if(arg=="-v" || arg=="--version")
			{
				std::cout << versionString << std::endl;
				return 0;
			}
			else if(arg=="-h" || arg=="--help")
			{
				std::cout << helpString << std::endl;
				return 0;
			}
			else if(arg=="-p" || arg=="--pipeline")
			{
				if(!getArgument(arg, k, argc, argv, pipelineFilename))
					return -1;
			}
			else if(arg=="-u" || arg=="--uniforms")
			{
				if(!getArgument(arg, k, argc, argv, uniformsFilename))
					return -1;
			}
			else if(arg=="-i" || arg=="--input")
			{
				if(!getIndexedArgument(arg, k, argc, argv, inputFilenames))
					return -1;
			}
			else if(arg=="-o" || arg=="--output")
			{
				if(!getIndexedArgument(arg, k, argc, argv, outputFilenames))
					return -1;
			}
			else
			{
				std::cerr << "Unknonwn argument : " << arg << std::endl;
				return -1;
			}
		} 

		// Test presence of main arguments : 
		if(pipelineFilename.empty())
		{
			std::cerr << "Missing pipeline filename." << std::endl;
			return -1;
		}

		if(outputFilenames.empty())
		{
			std::cerr << "Output filenames list is empty." << std::endl;
			return -1;
		}

		for(std::vector<std::string>::iterator it=inputFilenames.begin(); it!=inputFilenames.end(); it++)
		{
			if(it->empty())
			{
				std::cerr << "Input " << std::distance(inputFilenames.begin(), it) << " was not defined (is empty)." << std::endl;
				return -1;
			}
		}

		for(std::vector<std::string>::iterator it=outputFilenames.begin(); it!=outputFilenames.end(); it++)
		{
			if(it->empty())
			{
				std::cerr << "Output " << std::distance(outputFilenames.begin(), it) << " was not defined (is empty)." << std::endl;
				return -1;
			}
		}

		// Start deeper processing : 
		try
		{
			// Create the GL context : 
			createWindowlessContext();

			// Start GL : 
			Glip::HandleOpenGL::init();

			// Analyze the pipeline : 
			Glip::Modules::LayoutLoader lloader;

			Glip::CorePipeline::__ReadOnly_PipelineLayout pLayout = lloader(pipelineFilename);
	
			if(pLayout.getNumInputPort()>inputFilenames.size())
				throw Glip::Exception("The pipeline " + pLayout.getFullName() + " has " + Glip::to_string(pLayout.getNumInputPort()) + " input port(s) but only " + Glip::to_string(inputFilenames.size()) + " input filenames were given.", __FILE__, __LINE__);
			if(pLayout.getNumOutputPort()>outputFilenames.size())
				throw Glip::Exception("The pipeline " + pLayout.getFullName() + " has " + Glip::to_string(pLayout.getNumOutputPort()) + " output port(s) but only " + Glip::to_string(outputFilenames.size()) + " output filenames were given.", __FILE__, __LINE__);

			// Load the input images, if any : 
			std::vector<Glip::CoreGL::HdlTexture*> inputs;
			unsigned char* buffer = new unsigned char[16*16*3];
			Glip::CoreGL::HdlTextureFormat textureFormat(16, 16, GL_RGB, GL_UNSIGNED_BYTE);

			for(std::vector<std::string>::iterator it=inputFilenames.begin(); it!=inputFilenames.end(); it++)
				inputs.push_back(loadImage(*it));

			// Prepare the processing : 
			Glip::CorePipeline::Pipeline* pipeline = new Glip::CorePipeline::Pipeline(pLayout, "ComputePipeline");

			for(std::vector<Glip::CoreGL::HdlTexture*>::iterator it=inputs.begin(); it!=inputs.end(); it++)
				(*pipeline) << (*(*it));

			(*pipeline) << Glip::CorePipeline::Pipeline::Process;

			// Save the outputs : 
			for(int k=0; k<pipeline->getNumOutputPort(); k++)
				saveImage(pipeline->out(k), outputFilenames[k]);

			// Clean : 
			delete pipeline;
			for(std::vector<Glip::CoreGL::HdlTexture*>::iterator it=inputs.begin(); it!=inputs.end(); it++)
				delete (*it);
		}
		catch(Glip::Exception& e)
		{
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;
			returnCode = -1;
		}

		return returnCode;
	}

