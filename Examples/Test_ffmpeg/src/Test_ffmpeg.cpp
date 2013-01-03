#include "Test_ffmpeg.hpp"
#include <QMessageBox>
#include <QFileDialog>

// Interface :
	TestFFMPEGInterface::TestFFMPEGInterface(void)
	 : layout(this), window(this, 640, 480)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("TestFFMPEGInterface::TestFFMPEGInterface - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			// Main interface :
			window.renderer().setKeyboardActions(true);
			window.renderer().setMouseActions(true);
			window.renderer().setPixelAspectRatio(1.0f);

			// Info :
			log << "> Test FFMPEG" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			layout.addLayout(&pipelineLoaderInterface);
			layout.addWidget(&window);
			layout.addLayout(&videoControls);
			layout.addLayout(&videoRecorderControls);

			setGeometry(1000, 100, 800, 700);
			show();

			QObject::connect(&(window.renderer()),		SIGNAL(actionReceived(void)),		this, 			SLOT(updateOutput(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(resized(void)),			this, 			SLOT(updateOutput(void)));
			QObject::connect(&pipelineLoaderInterface,	SIGNAL(outputIndexChanged(void)),	this, 			SLOT(updateOutput(void)));
			QObject::connect(&pipelineLoaderInterface,	SIGNAL(requestComputingUpdate(void)),	this, 			SLOT(process(void)));
			QObject::connect(&pipelineLoaderInterface,	SIGNAL(pipelineChanged(void)),		this,			SLOT(resetProcessTest(void)));
			QObject::connect(&pipelineLoaderInterface,	SIGNAL(pipelineChanged(void)),		&videoRecorderControls,	SLOT(stopRecording(void)));
			QObject::connect(&videoControls,		SIGNAL(newVideoLoaded(void)),		this,			SLOT(resetProcessTest(void)));
			QObject::connect(&videoControls,		SIGNAL(frameUpdated(void)),		this, 			SLOT(process(void)));
			QObject::connect(&videoRecorderControls,	SIGNAL(aboutToStartRecording(void)),	this, 			SLOT(aboutToStartRecording(void)));
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			QMessageBox::warning(NULL, tr("Test_ffmpeg - Error : "), e.what());
			throw e;
		}
	}

	TestFFMPEGInterface::~TestFFMPEGInterface(void)
	{
		log << "> End" << std::endl;
		log.close();
	}

	void TestFFMPEGInterface::resetProcessTest(void)
	{
		if(videoControls.videoStreamIsValid() && pipelineLoaderInterface.isPipelineValid())
		{
			if(pipelineLoaderInterface.pipeline().getNumInputPort() >  videoControls.videoStream().getNumBuffers())
				QMessageBox::warning(NULL, tr("Test_ffmpeg - Warning : "), "Pipeline has more input ports than video frames buffered. Last port will receive a duplicate of the oldest frame in the buffer queue.");
			else if(pipelineLoaderInterface.pipeline().getNumInputPort() <  videoControls.videoStream().getNumBuffers())
				QMessageBox::warning(NULL, tr("Test_ffmpeg - Warning : "), "Pipeline has less input ports than video frames buffered.");
		}

		process();
	}

	void TestFFMPEGInterface::process(void)
	{
		if(videoControls.videoStreamIsValid() && pipelineLoaderInterface.isPipelineValid())
		{
			for(int i=0; i<pipelineLoaderInterface.pipeline().getNumInputPort(); i++)
				pipelineLoaderInterface.pipeline() << videoControls.videoStream().texture( std::min(i, videoControls.videoStream().getNumBuffers()-1) );

			pipelineLoaderInterface.pipeline() << Pipeline::Process;

			if(videoRecorderControls.isRecording())
				videoRecorderControls.submitNewFrame( pipelineLoaderInterface.pipeline().out( videoRecorderControls.getRecordedPort() ) );
		}

		updateOutput();
	}

	void TestFFMPEGInterface::updateOutput(void)
	{
		if(videoControls.videoStreamIsValid())
		{
			int 	w = pipelineLoaderInterface.currentOutput( videoControls.videoStream().texture() ).getWidth(),
				h = pipelineLoaderInterface.currentOutput( videoControls.videoStream().texture() ).getHeight();
			float 	imageAspectRatio = static_cast<float>(w)/static_cast<float>(h);
			window.renderer().setImageAspectRatio(imageAspectRatio);

			window.renderer() << pipelineLoaderInterface.currentOutput( videoControls.videoStream().texture() );
		}
		else
			window.renderer().clearWindow();
	}

	void TestFFMPEGInterface::aboutToStartRecording(void)
	{
		int id = pipelineLoaderInterface.currentOutputId();

		if(id<0)
			QMessageBox::information(NULL, tr("TestFFMPEGInterface::aboutToStartRecording"), "You cannot record the orignal stream.");
		else if(!pipelineLoaderInterface.isPipelineValid())
			QMessageBox::information(NULL, tr("TestFFMPEGInterface::aboutToStartRecording"), "No valid pipeline for processing.");
		else
			videoRecorderControls.startRecording(id, pipelineLoaderInterface.pipeline().getOutputPortNameExtended(id), pipelineLoaderInterface.pipeline().out(id).format());
	}

// Application :
	TestFFMPEGApplication::TestFFMPEGApplication(int& argc, char** argv)
	 : QApplication(argc, argv), interface(NULL)
	{
		try
		{
			// Interface :
			interface = new TestFFMPEGInterface;
		}
		catch(std::exception& e)
		{
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			QMessageBox::information(NULL, tr("TestFFMPEGApplication::TestFFMPEGApplication - Exception caught : "), e.what());
			throw e;
		}
	}

	TestFFMPEGApplication::~TestFFMPEGApplication(void)
	{
		delete interface;
	}
