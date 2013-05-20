#include "header.hpp"

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Code
	HistogramInterface::HistogramInterface(void)
	 : pipeline(NULL), window(NULL, 640, 480), layout(this), saveButton("Save result (RGB888)", this), scaleSlider(Qt::Horizontal)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("HistogramInterface::HistogramInterface - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			scaleSlider.setRange(1,100);

			// Info :
			log << "> Histogram" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			window.renderer().setMouseActions(true);
			window.renderer().setKeyboardActions(true);
			window.renderer().setPixelAspectRatio(1.0f);

			layout.addLayout(&imageLoaderInterface);
			layout.addWidget(&window);
			layout.addWidget(&scaleSlider);
			layout.addWidget(&saveButton);
			setGeometry(1000, 100, 800, 700);
			show();

			// Make the pipeline
			LayoutLoader	loader;

			pipeline = loader("./Filters/histogramPipeline.ppl", "instHistogramPipeline");

			// Set geometry and variables :
			const int 	numWPoints = 512,
					numHPoints = 512;
			((*pipeline)["histogramFilterInstance"]).setGeometry( HdlVBO::generate3DGrid(numWPoints, numHPoints, 3, 1.0f, 1.0f, 2.0f) );
			((*pipeline)["histogramFilterInstance"]).prgm().modifyVar("nrm", GL_FLOAT, 1.0f/static_cast<float>(numWPoints*numHPoints));

			QObject::connect(&imageLoaderInterface,		SIGNAL(currentTextureChanged(void)),	this, SLOT(process(void)));
			QObject::connect(&scaleSlider,			SIGNAL(sliderMoved(int)),		this, SLOT(process(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(actionReceived(void)),		this, SLOT(updateShow(void)));
			QObject::connect(&saveButton,			SIGNAL(released(void)), 		this, SLOT(save(void)));
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			QMessageBox::information(NULL, tr("Error while building program"), e.what());
			std::cout << "Error while setting variables : " << e.what() << std::endl;
			std::cout << "Will be rethrown!" << std::endl;
			throw e;
		}
	}

	HistogramInterface::~HistogramInterface(void)
	{
		log << "> End" << std::endl;
		log.close();

		delete pipeline;
	}

	void HistogramInterface::process(void)
	{
		if(pipeline!=NULL && imageLoaderInterface.getNumTextures()>0)
		{
			HdlTexture& texture = imageLoaderInterface.currentTexture();

			((*pipeline)["histogramShowFilterInstance"]).prgm().modifyVar("scale", GL_FLOAT, static_cast<float>(scaleSlider.value()));

			(*pipeline) << texture << Pipeline::Process;

			updateShow();
		}
	}

	void HistogramInterface::updateShow(void)
	{
		static bool lock = false;

		if(lock)
			return;

		lock = true;

		if(pipeline!=NULL && imageLoaderInterface.getNumTextures()>0)
		{
			try
			{
				float imageAspectRatio = static_cast<float>(pipeline->out(0).getWidth())/static_cast<float>(pipeline->out(0).getHeight());
				window.renderer().setImageAspectRatio(imageAspectRatio);
				window.renderer() << pipeline->out(0) << OutputDevice::Process;
			}
			catch(Exception& e)
			{
				log << "HistogramInterface::updateShow - Exception caught : " << std::endl;
				log << e.what() << std::endl;
				std::cerr << "HistogramInterface::updateShow - Exception caught : " << std::endl;
				std::cerr << e.what() << std::endl;
			}
		}
		else
			window.renderer().clearWindow();

		lock = false;
	}

	void HistogramInterface::save(void)
	{
		if(imageLoaderInterface.getNumTextures()<=0)
			QMessageBox::information(this, tr("Error while requesting image write"), tr("No available image."));
		else if(pipeline!=NULL)
		{
			try
			{
				imageLoaderInterface.saveTexture(pipeline->out(0));
			}
			catch(Exception& e)
			{
				QMessageBox::information(this, tr("IHM::save - Error while writing file : "), e.what());
				log << "IHM::save - Error while writing file :  " << std::endl;
				log << e.what() << std::endl;
			}
		}
		else
			QMessageBox::information(this, tr("Error while requesting image write"), tr("Pipeline is not defined, this is an internal error."));
	}

	HistogramApplication::HistogramApplication(int& argc, char** argv)
	 : QApplication(argc, argv)
	{
		ihm = new HistogramInterface;
	}

	HistogramApplication::~HistogramApplication(void)
	{
		delete ihm;
	}
