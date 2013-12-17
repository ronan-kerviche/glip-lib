#include "mainInterface.hpp"
#include <QDateTime>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// MainWindow
	MainWindow::MainWindow(void)
	 : /*mainLayout(this),*/ codeEditors(*this, this), libraryInterface(*this, this)
	{
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
			QMessageBox::information(NULL, tr("GlipStudio::GlipStudio - Error :"), tr("The style sheet \"%1\" could not be loaded.\nIn %2.").arg(stylesheetFile.fileName()).arg(path));
			throw Exception("GlipStudio::GlipStudio - The style sheet \"" + stylesheetFile.fileName().toStdString()  + "\" could not be loaded (" + path.toStdString() + ").", __FILE__, __LINE__); 
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
 
