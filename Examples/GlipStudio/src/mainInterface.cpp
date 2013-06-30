#include "mainInterface.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// MainWindow
	MainWindow::MainWindow(void)
	 : mainLayout(this), codeEditors(this), libraryInterface(this), display(this,640,480), mainPipeline(NULL)
	{
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
		libraryInterface.compile( codeEditors.getCurrentCode(), codeEditors.getCodePath() );
	}

	void MainWindow::updateOutput(void)
	{
		static bool initBackground = true;

		if(libraryInterface.hasOutput())
		{
			display.renderer().setImageAspectRatio(libraryInterface.currentOutput());

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

	bool GlipStudio::notify(QObject * receiver, QEvent * event) 
	{
		try 
		{
			return QApplication::notify(receiver, event);
		} 
		catch(std::exception& e) 
		{
			qDebug() << "Exception caught :" << e.what();
		}

		return false;
	}
 
