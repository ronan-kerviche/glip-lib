#include "mainInterface.hpp"
#include <QtDebug>
#include <QDateTime>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// MainWindow
	MainWindow::MainWindow(void)
	 : frame(this), codeEditors(*this, this), libraryInterface(*this, this)
	{
		// Transmit the close signal : 
		connect(&frame, SIGNAL(closeSignal()), this, SLOT(close()) );

		// Display tools : 
		display.sceneWidget().setKeyboardActions(true);
		display.sceneWidget().setMouseActions(true);

		secondarySplitter.setOrientation(Qt::Vertical);
		secondarySplitter.addWidget(&display);
		secondarySplitter.addWidget(&libraryInterface);
		containerLayout.addWidget(&secondarySplitter);
		container.setLayout(&containerLayout);

		mainSplitter.addWidget(&container);
		mainSplitter.addWidget(&codeEditors);
		setCentralWidget(&mainSplitter);

		frame.titleBar().setWindowTitle("GlipStudio");
		frame.resize(1280,720);
		frame.setMinimumWidth(512);
		frame.setMinimumHeight(512);
		frame.show();
	}

	MainWindow::~MainWindow(void)
	{
		// Safe reparenting, we do not want the OpenGL context to be deleted upon deletion of the interface as some ressources can still be handled by higher managers.
		// Reparting prevent secondarySplitter from deleting its child &display.
		display.setParent(NULL);
		frame.setParent(NULL);
		frame.removeContent();
	}

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		if( requireClose() )
		{
			codeEditors.close();
			display.close();
			libraryInterface.close();

			event->accept();
			frame.close();
		}
		else
			event->ignore();
	}

// Special function, for redirection of qDebug, qCritical, etc. to a file : 
	void customMessageHandler(QtMsgType type, const char *msg)
	{
		QDateTime dateTime = QDateTime::currentDateTime();
		

		QString txt;

		switch (type)
		{
			case QtDebugMsg:
				txt = QString("[%1] DEBUG :\n%2").arg(dateTime.toString()).arg(msg);
				break;
			case QtWarningMsg:
				txt = QString("[%1] WARNING :\n%2").arg(dateTime.toString()).arg(msg);
				break;
			case QtCriticalMsg:
				txt = QString("[%1] CRITICAL :\n%2").arg(dateTime.toString()).arg(msg);
				break;
			case QtFatalMsg:
				txt = QString("[%1] FATAL :\n%2").arg(dateTime.toString()).arg(msg);
				break;
			default :
				txt = QString("[%1] UNKNOWN :\n%2").arg(dateTime.toString()).arg(msg);
		}

		QFile outFile("errorLog.txt");
		outFile.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream stream(&outFile);
		stream << txt << "\n";

		if(type==QtFatalMsg)
			abort();
	}

// GlipStudio
	GlipStudio::GlipStudio(int& argc, char** argv)
	 : QApplication(argc, argv), settingsManager(NULL), mainWindow(NULL)
	{
		// Set the redirections of logs : 
		qInstallMsgHandler(customMessageHandler);

		// Load fonts : 
		int fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Regular.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;
	
		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Black.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl; 

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Bold.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Light.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-ExtraLight.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Medium.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Semibold.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		std::cout << "Font list : " << std::endl;
		QFontDatabase db;
		foreach(const QString& family, QFontDatabase::applicationFontFamilies(fid))
		{
			std::cout << "    " << family.toStdString() << std::endl;
			foreach(const QString& style, db.styles(family))
				std::cout << "      " << style.toStdString() << std::endl;
		}
		std::cout << "End font list." << std::endl;

		// Load Stylesheet : 
		const QString 	stylesheetFilename = "stylesheet.css";
		QFile 		stylesheetFile(stylesheetFilename);
			
		if(!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString path = QDir::currentPath();

			Exception e("GlipStudio::GlipStudio - The style sheet \"" + stylesheetFile.fileName().toStdString()  + "\" could not be loaded (" + path.toStdString() + ").", __FILE__, __LINE__); 

			qCritical() << e.what();

			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("The style sheet \"%1\" could not be loaded.\nIn %2. The execution will continue with default system theme (BAD).").arg(stylesheetFile.fileName()).arg(path), QMessageBox::Ok);
			messageBox.exec();
		}

		QTextStream 	stylesheetStream(&stylesheetFile);
		QString 	stylesheet = stylesheetStream.readAll();

		// Set style : 
		QApplication::setStyleSheet(stylesheet);

		// Load settings : 
		try
		{
			settingsManager = new SettingsManager("settings.vnp");
		}
		catch(Exception& e)
		{
			// Save : 
			qWarning() << e.what();

			// Show : 
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("Unable to load the settings from \"settings.vnp\". The settings will be reset."), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();

			// Create a blank settings bank : 
			settingsManager = new SettingsManager("settings.vnp", true);
		}

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
			// Save : 
			qWarning() << e.what();	

			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught. However, you might be able to continue execution."), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}

		return false;
	}
 
