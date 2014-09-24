#include "GlipStudio.hpp"
#include <QDateTime>
#include <QDebug>

// Special function, for redirection of qDebug, qCritical, etc. to a file : 
#if QT_VERSION >= 0x050000
	void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const char *msg)
#else
	void customMessageHandler(QtMsgType type, const char *msg)
#endif
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

// GlipStudio :
	GlipStudio::GlipStudio(int& argc, char** argv)
	 :	QApplication(argc, argv),
		window(NULL),
		editor(NULL),
		collection(NULL),
		pipelineManager(NULL)
	{
		#if QT_VERSION >= 0x050000
			qInstallMessageHandler(reinterpret_cast<QtMessageHandler>(customMessageHandler));
		#else
			qInstallMsgHandler(customMessageHandler);
		#endif

		loadStyleSheet();
		loadFonts();

		window		= new QVGL::MainWidget;
		editor		= new QGED::CodeEditorTabsSubWidget;
		collection	= new QGIC::ImageItemsCollectionSubWidget;
		pipelineManager	= new QGPM::PipelineManagerSubWidget(&const_cast<const QVGL::MainWidget*>(window)->getMouseState());

		// Add Subwidgets : 
		window->addSubWidget(editor);
		window->addSubWidget(collection);
		window->addViewsTable(collection->getMainViewsTablePtr());
		window->addSubWidget(pipelineManager);

		// Connections : 
		QObject::connect(collection, 				SIGNAL(addViewRequest(QVGL::View*)), 					window, 				SLOT(addView(QVGL::View*)));
		QObject::connect(editor->getCodeEditorPtr(), 		SIGNAL(compileSource(std::string, std::string, void*, const QObject*)), pipelineManager->getManagerPtr(), 	SLOT(compileSource(std::string, std::string, void*, const QObject*)));
		QObject::connect(collection->getCollectionPtr(), 	SIGNAL(imageItemAdded(QGIC::ImageItem*)), 				pipelineManager->getManagerPtr(), 	SLOT(addImageItem(QGIC::ImageItem*)));
		QObject::connect(pipelineManager->getManagerPtr(), 	SIGNAL(addViewRequest(QVGL::View*)), 					window, 				SLOT(addView(QVGL::View*)));
		QObject::connect(pipelineManager->getManagerPtr(), 	SIGNAL(addViewsTableRequest(QVGL::ViewsTable*)),			window, 				SLOT(addViewsTable(QVGL::ViewsTable*)));
		QObject::connect(pipelineManager->getManagerPtr(), 	SIGNAL(addImageItemRequest(QGIC::ImageItem*)),				collection->getCollectionPtr(),		SLOT(addImageItem(QGIC::ImageItem*)));

		window->resize(900, 600);
		window->show();
	}

	GlipStudio::~GlipStudio(void)
	{
		delete pipelineManager;
		delete collection;
		delete editor;
		delete window;
	}

	void GlipStudio::loadStyleSheet(void)
	{
		const QString 	stylesheetFilename = "stylesheet.css";
		QFile 		stylesheetFile(stylesheetFilename);
			
		if(!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString path = QDir::currentPath();

			Exception e("GlipStudio::GlipStudio - The style sheet \"" + stylesheetFile.fileName().toStdString()  + "\" could not be loaded (from " + path.toStdString() + ").", __FILE__, __LINE__); 

			qCritical() << e.what();

			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("The style sheet \"%1\" could not be loaded.\nIn %2. The execution will continue with default theme on your system.").arg(stylesheetFile.fileName()).arg(path), QMessageBox::Ok);
			messageBox.exec();
		}
		else
		{
			QTextStream 	stylesheetStream(&stylesheetFile);
			QString 	stylesheet = stylesheetStream.readAll();

			// Set style : 
			QApplication::setStyleSheet(stylesheet);
		}
	}

	void GlipStudio::loadFonts(void)
	{
		// Load fonts : 
		int fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Regular.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;
	
		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Black.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl; 

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Bold.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Light.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-ExtraLight.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Medium.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;

		fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Semibold.ttf");
		//if( fid < 0)
		//	std::cerr << "Could not locate the font!" << std::endl;

		/*std::cout << "Font list : " << std::endl;
		QFontDatabase db;
		foreach(const QString& family, QFontDatabase::applicationFontFamilies(fid))
		{
			std::cout << "    " << family.toStdString() << std::endl;
			foreach(const QString& style, db.styles(family))
				std::cout << "      " << style.toStdString() << std::endl;
		}
		std::cout << "End font list." << std::endl;*/
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

