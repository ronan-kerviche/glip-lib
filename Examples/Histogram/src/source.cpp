#include "header.hpp"

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Code
	HistogramInterface::HistogramInterface(void)
	 : pipeline(NULL), window(NULL, 640, 480), layout(this), saveButton("Save result (RGB888)", this)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("HistogramInterface::HistogramInterface - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
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
			layout.addWidget(&saveButton);
			setGeometry(1000, 100, 800, 700);
			show();

			// Make the pipeline
			PipelineLayout* model = NULL;
			LayoutLoader	loader;

			model = loader("./Filters/histogramPipeline.ppl");

			pipeline = new Pipeline(*model, "instHistogramPipeline");
			std::cout << "Name of the pipeline : " << pipeline->getNameExtended() << std::endl;
			delete model;

			// Set variables :
			((*pipeline)["instHistRed"]).prgm().modifyVar("c",HdlProgram::Var,0);
			((*pipeline)["instHistGreen"]).prgm().modifyVar("c",HdlProgram::Var,1);
			((*pipeline)["instHistBlue"]).prgm().modifyVar("c",HdlProgram::Var,2);
			const float scale = 10.0f;
			((*pipeline)["instHistRed"]).prgm().modifyVar("scale",HdlProgram::Var,scale);
			((*pipeline)["instHistGreen"]).prgm().modifyVar("scale",HdlProgram::Var,scale);
			((*pipeline)["instHistBlue"]).prgm().modifyVar("scale",HdlProgram::Var,scale);

			QObject::connect(&imageLoaderInterface,		SIGNAL(currentTextureChanged(void)),	this, SLOT(process(void)));
			QObject::connect(&imageLoaderInterface,		SIGNAL(currentTextureChanged(void)),	this, SLOT(updateShow(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(resized(void)),  		this, SLOT(updateShow(void)));
			QObject::connect(&(window.renderer()),		SIGNAL(actionReceived(void)),		this, SLOT(updateShow(void)));
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
		if(pipeline!=NULL)
		{
			// Update the grid of the pipeline :
			HdlTexture& texture = imageLoaderInterface.currentTexture();
			(*pipeline)["instHistRed"].setGeometry(HdlVBO::generate2DGrid(texture.getWidth(), texture.getHeight()));
			(*pipeline)["instHistGreen"].setGeometry(HdlVBO::generate2DGrid(texture.getWidth(), texture.getHeight()));
			(*pipeline)["instHistBlue"].setGeometry(HdlVBO::generate2DGrid(texture.getWidth(), texture.getHeight()));

			float a = 1.0f/static_cast<float>(texture.getNumPixels());
			((*pipeline)["instHistRed"]).prgm().modifyVar("nrm",HdlProgram::Var,a);
			((*pipeline)["instHistGreen"]).prgm().modifyVar("nrm",HdlProgram::Var,a);
			((*pipeline)["instHistBlue"]).prgm().modifyVar("nrm",HdlProgram::Var,a);

			(*pipeline) << texture << Pipeline::Process;
		}
	}

	void HistogramInterface::updateShow(void)
	{
		if(pipeline!=NULL)
		{
			float imageAspectRatio = static_cast<float>(pipeline->out(0).getWidth())/static_cast<float>(pipeline->out(0).getHeight());
			window.renderer().setImageAspectRatio(imageAspectRatio);
			window.renderer() << pipeline->out(0);
		}
		else
			window.renderer().clearWindow();
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
