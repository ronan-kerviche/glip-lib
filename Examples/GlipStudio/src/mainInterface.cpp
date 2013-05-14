#include "mainInterface.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// MainWindow
	MainWindow::MainWindow(void)
	 : mainLayout(this), codeEditors(this), libraryInterface(this), display(this,640,480), mainPipeline(NULL)
	{
		QObject::connect(&(display.renderer()),		SIGNAL(resized(void)),  		this, SLOT(updateOutput(void)));
		QObject::connect(&(display.renderer()),		SIGNAL(actionReceived(void)),		this, SLOT(updateOutput(void)));
		QObject::connect(&codeEditors, 			SIGNAL(requireRefresh(void)), 		this, SLOT(refreshPipeline(void)));
		QObject::connect(&libraryInterface,		SIGNAL(requireRedraw(void)),		this, SLOT(updateOutput(void)));
		
		// Display tools : 
		display.renderer().setKeyboardActions(true);
		display.renderer().setMouseActions(true);
		display.renderer().setPixelAspectRatio(1.0f);

		secondarySplitter.setOrientation(Qt::Vertical);
		secondarySplitter.addWidget(&display);
		secondarySplitter.addWidget(&libraryInterface);
		containerLayout.addWidget(&secondarySplitter);
		container.setLayout(&containerLayout);

		mainSplitter.addWidget(&container);
		mainSplitter.addWidget(&codeEditors);
		mainLayout.addWidget(&mainSplitter);

		resize(1280,720);
		show();
	}

	MainWindow::~MainWindow(void)
	{
		delete mainPipeline;
		mainPipeline = NULL;
	}

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		if( codeEditors.canBeClosed() )
			event->accept();
		else
			event->ignore();
	}

	void MainWindow::refreshPipeline(void)
	{
		bool success = true;

		try
		{
			delete mainPipeline;
			mainPipeline = NULL;
			mainPipeline = pipelineLoader(codeEditors.getCurrentCode(), "MainPipeline");
		}
		catch(Exception& e)
		{
			success = false;

			std::cerr << e.what() << std::endl;
			delete mainPipeline;
			mainPipeline = NULL;
		}

		if(success)
			updateOutput();
	}

	void MainWindow::updateOutput(void)
	{
		static bool initBackground = true;

		if(initBackground)
		{
			QColor darkbg = palette().background().color().lighter(70);

			float 	r = static_cast<float>(darkbg.red()) / 255.0f,
				g = static_cast<float>(darkbg.green()) / 255.0f,
				b = static_cast<float>(darkbg.blue()) / 255.0f;
		
			display.renderer().setClearColor(r, g, b);

			initBackground = false;
		}
		
		if(mainPipeline!=NULL && libraryInterface.hasOutput())
		{
			(*mainPipeline)	<<  libraryInterface.currentOutput() << Pipeline::Process;
			display.renderer() << mainPipeline->out(0) << OutputDevice::Process;
		}
		else if(libraryInterface.hasOutput())
		{
			float 	imageAspectRatio = static_cast<float>(libraryInterface.currentOutput().getWidth())/static_cast<float>(libraryInterface.currentOutput().getHeight());
			display.renderer().setImageAspectRatio(imageAspectRatio);

			display.renderer() << libraryInterface.currentOutput() << OutputDevice::Process;
		}
		else
			display.renderer().clearWindow();
	}

// GlipStudio
	GlipStudio::GlipStudio(int& argc, char** argv)
	 : QApplication(argc, argv), mainWindow(NULL)
	{
		const QString 	stylesheetFilename = "stylesheet.css";
		QFile 		stylesheetFile(stylesheetFilename);
			
		if(!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::information(NULL, tr("GlipStudio::GlipStudi - Error :"), tr("The style sheet \"%1\" could not be loaded.").arg(stylesheetFile.fileName()));
			throw Exception("GlipStudio::GlipStudio - The style sheet \"" + stylesheetFile.fileName().toStdString()  + "\" could not be loaded.", __FILE__, __LINE__); 
		}

		QTextStream 	stylesheetStream(&stylesheetFile);
		QString 	stylesheet = stylesheetStream.readAll();

		// Set style : 
		QApplication::setStyleSheet(stylesheet);

		mainWindow = new MainWindow();
	}

	GlipStudio::~GlipStudio(void)
	{
		delete mainWindow;
	}
 
