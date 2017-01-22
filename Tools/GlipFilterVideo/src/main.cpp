#include <iostream>
#include "GLIPLib.hpp"
#include "CreateWindowlessContext.hpp"
#include "VideoStream.hpp"
#include "VideoRecorder.hpp"

int main(int argc, char** argv)
{
	if(argc!=2)
	{
		std::cerr << "Invalid/missing argument." << std::endl;
		std::cerr << argv[0] << " videoFilename" << std::endl;
		return -1;
	}
	else
	{
		Glip::CorePipeline::Pipeline* pipeline = NULL;
		try
		{
			createWindowlessContext();
			Glip::CoreGL::HandleOpenGL();

			Glip::Modules::LayoutLoader loader;
			Glip::Modules::LayoutLoaderModule::addBasicModules(loader);
			pipeline = loader.getPipeline("testPipeline.ppl");

			if(pipeline->getNumInputPort()>0 || pipeline->getNumOutputPort()!=1)
				throw Glip::Exception("Invalid pipeline layout.");
			// Find the first filter :
			Glip::CorePipeline::Filter* filter = NULL;
			for(int f=0; f<pipeline->getNumElements(); f++)
			{
				if(pipeline->getElementKind(f)==Glip::CorePipeline::AbstractPipelineLayout::FILTER)
				{
					std::cout << "Found filter \"" << pipeline->getElementName(f) << "\" in pipeline " << pipeline->getFullName() << std::endl;
					Glip::CorePipeline::Filter* tmp = &(*pipeline)[pipeline->getElementID(f)];
					const std::vector<std::string>& uniformsNames = tmp->program().getUniformsNames();
					const std::vector<GLenum>& uniformsTypes = tmp->program().getUniformsTypes();
					for(int k=0; k<uniformsNames.size(); k++)
						std::cout << "    " << Glip::getGLEnumNameSafe(uniformsTypes[k]) << " - " << uniformsNames[k] << std::endl;
					if(tmp->program().isUniformVariableValid("time"))
					{
						std::cout << " The filter has the time variable ..." << std::endl;
						filter = tmp;
						break;
					}
					else
						std::cout << " The filter does not have the time variable ..." << std::endl;
				}
			}

			// Read :
			//VideoStream stream(argv[1], 1);

			// Record :
			const float frameRate = 24.0;
			FFMPEGInterface::VideoRecorder recorder("test.avi", pipeline->out(0).format(), frameRate);
			std::cout << "Recording ..." << std::endl;
			for(float time=0.0; time<=10.0; time+=1.0f/frameRate)
			{
				// Update the time :
				if(filter!=NULL)
				{
					const GLenum err = glGetError();
					if(err!=GL_NO_ERROR)
						std::cout << "(" << time << ") Error : " << err << std::endl;
					filter->program().setVar("time", GL_FLOAT, time);
				}
				// Generate :
				(*pipeline) << Glip::CorePipeline::Pipeline::Process;
				// Record :
				recorder.record(pipeline->out());
			}
		}
		catch(Glip::Exception& e)
		{
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;
		}
		delete pipeline;
		pipeline = NULL;
		return 0;
	}
}
