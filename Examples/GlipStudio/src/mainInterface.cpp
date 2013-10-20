#include "mainInterface.hpp"
#include <QDateTime>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// MainWindow
	MainWindow::MainWindow(void)
	 : mainLayout(this), codeEditors(*this, this), libraryInterface(*this, this)
	{
		
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
		// Safe reparenting, we do not want the OpenGL context to be deleted upon deletion of the interface as some ressources can still be handled by higher managers.
		// Reparting prevent secondarySplitter from deleting its child &display.
		display.setParent(NULL);
	}

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		if( requireClose() )
			event->accept();
		else
			event->ignore();
	}

// GlipStudio
	GlipStudio::GlipStudio(int& argc, char** argv)
	 : QApplication(argc, argv), settingsManager(NULL), mainWindow(NULL)
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

		// Load settings : 
		settingsManager = new SettingsManager("settings.vnp");

		mainWindow = new MainWindow();
	}

	GlipStudio::~GlipStudio(void)
	{
		delete mainWindow;
		delete settingsManager;
	}

	bool GlipStudio::notify(QObject * receiver, QEvent * event) 
	{
		try 
		{
			return QApplication::notify(receiver, event);
		} 
		catch(std::exception& e) 
		{
			std::cerr << "Exception caught :" << std::endl;
			std::cout << e.what() << std::endl;

			// Save : 
			QFile file("exceptions.txt");

			if( file.open(QIODevice::WriteOnly | QIODevice::Text) )
			{
				QTextStream out(&file);

				QDateTime dateTime = QDateTime::currentDateTime();

				out << "On " << dateTime.toString() << ", the following exception was caught : \n";
				out << e.what() << "\n";
			
				file.close();		
			}

			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught. However, you might be able to continue execution."), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}

		return false;
	}
 
