#include "imageContentInfo.hpp"
#include <cstdlib>
#include <ctime>
#include <QPointer>

// Flags
	//#define __USE_PBO__

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Src :
	IHM::IHM(void)
	 : layout(this), saveButton("Save result (RGB888)", this), window(this, 640, 480)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("IHM::IHM - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			// Main interface :
			window.renderer().setKeyboardActions(true);
			window.renderer().setMouseActions(true);
			window.renderer().setPixelAspectRatio(1.0f);

			// Info :
			log << "> ImageTest" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			layout.addLayout(&imageLoaderInterface);
			layout.addLayout(&pipelineLoaderInterface);
			layout.addWidget(&window);
			layout.addWidget(&saveButton);

			setGeometry(1000, 100, 800, 700);
			show();

			QObject::connect(&saveButton,			SIGNAL(released(void)), 		this, SLOT(save(void)));
			QObject::connect(&imageLoaderInterface,		SIGNAL(currentTextureChanged(void)),	this, SLOT(requestComputingUpdate(void)));
			QObject::connect(&imageLoaderInterface,		SIGNAL(currentTextureChanged(void)),	this, SLOT(updateOutput(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(resized(void)),  		this, SLOT(updateOutput(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(actionReceived(void)),		this, SLOT(updateOutput(void)));
			QObject::connect(&pipelineLoaderInterface, 	SIGNAL(outputIndexChanged(void)),	this, SLOT(updateOutput(void)));
			QObject::connect(&pipelineLoaderInterface, 	SIGNAL(requestComputingUpdate(void)),	this, SLOT(requestComputingUpdate(void)));
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			QMessageBox::warning(NULL, tr("ImageTest - Error : "), e.what());
			throw e;
		}
	}

	IHM::~IHM(void)
	{
		log << "> End" << std::endl;
		log.close();
	}

	void IHM::save(void)
	{
		if(pipelineLoaderInterface.currentChoiceIsOriginal())
			QMessageBox::information(this, tr("Error while requesting image write"), tr("Cannot write the original file (no modification made)."));
		else if(!pipelineLoaderInterface.isPipelineValid())
			QMessageBox::information(this, tr("Error while requesting image write"), tr("No available pipeline or last computing operation failed."));
		else
		{
			try
			{
				imageLoaderInterface.saveTexture(pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ));
			}
			catch(Exception& e)
			{
				QMessageBox::information(this, tr("IHM::save - Error while writing file : "), e.what());
				std::cout << "IHM::save - Error while writing file :  " << std::endl;
				std::cout << e.what() << std::endl;
			}
		}
	}

	void IHM::requestComputingUpdate(void)
	{
		if(pipelineLoaderInterface.isPipelineValid() && imageLoaderInterface.getNumTextures()>0)
		{
			if(pipelineLoaderInterface.pipeline().getNumInputPort()!=1)
			{
				QMessageBox::information(this, tr("IHM::requestComputingUpdate - Error :"), "The pipeline must have only one input.");
				pipelineLoaderInterface.revokePipeline();
			}
			else
				pipelineLoaderInterface.pipeline() << imageLoaderInterface.currentTexture() << Pipeline::Process;
		}
	}

	void IHM::updateOutput(void)
	{
		if(imageLoaderInterface.getNumTextures()==0)
			window.renderer().clearWindow();
		else
		{
			try
			{
				int 	w = pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ).getWidth(),
					h = pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ).getHeight();
				float 	imageAspectRatio = static_cast<float>(w)/static_cast<float>(h);
				window.renderer().setImageAspectRatio(imageAspectRatio);

				window.renderer() << pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ) << OutputDevice::Process;
			}
			catch(std::exception& e)
			{
				std::cout << "IHM::updateOutput - Exception while updating : " << std::endl;
				std::cout << e.what() << std::endl;
				log << "IHM::updateOutput - Exception while updating : " << std::endl;
				log << e.what() << std::endl;
			}
		}
	}


	ImageContentInformation::ImageContentInformation(int& argc, char** argv)
	 : QApplication(argc,argv), ihm(NULL)
	{
		try
		{
			// Interface :
			ihm = new IHM;
		}
		catch(std::exception& e)
		{
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			QMessageBox::information(NULL, tr("Exception caught : "), e.what());
			throw e;
		}

		std::cout << "--- STARTING ---" << std::endl;
	}

	ImageContentInformation::~ImageContentInformation(void)
	{
		delete ihm;
	}
