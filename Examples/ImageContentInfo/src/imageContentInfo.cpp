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

	TmpWidget::TmpWidget(void)
	 : 	layout(&widget),
		a("Button A", &widget),
		b("Button B", &widget),
		c("Button C", &widget),
		//img("/home/arkh/Pictures/mire.bmp"),
		//img("/home/arkh/Pictures/the_general_problem.png"),
		//img("/home/arkh/Pictures/2048.png"),
		img("/home/arkh/Pictures/Wallpaper/winter_process_v2_1680x1050.jpg"),
		texture(NULL),
		view(NULL)
	{
		std::cout << "TmpWidget    : " << this << std::endl;
		std::cout << "    layout   : " << (&layout) << std::endl;
		std::cout << "    button a : " << (&a) << std::endl;
		std::cout << "    button b : " << (&b) << std::endl;
		std::cout << "    button c : " << (&c) << std::endl;

		static int count = 1;
		
		layout.addWidget(&a);
		layout.addWidget(&b);
		layout.addWidget(&c);
		layout.setMargin(0);
		layout.setSpacing(1);

		// Create the view : 
		ImageBuffer* buffer = createImageBufferFromQImage(img);
		texture = new HdlTexture(*buffer);

		(*buffer) >> (*texture);

		view = new QVGL::View(texture, tr("View %1").arg(count));

		//view->setAngle(0.5465f);
		//view->setViewCenter(0.3, 0.1);

		delete buffer;

		setTitle(tr("Widget %1").arg(count));

		count++;

		QObject::connect(&a, SIGNAL(released(void)), this, SLOT(buttonAPressed(void)));
		QObject::connect(&b, SIGNAL(released(void)), this, SLOT(buttonBPressed(void)));
		QObject::connect(&c, SIGNAL(released(void)), this, SLOT(buttonCPressed(void)));

		std::cout << "Subwidget created." << std::endl;

		setInnerWidget(&widget);

		show();

		move(128, 128);
	}

	TmpWidget::~TmpWidget(void)
	{
		delete texture;
		delete view;
	}

	void TmpWidget::buttonAPressed(void)
	{
		std::cout << "TmpWidget : Button A, add and show" << std::endl;

		getQVGLParent()->addView(view);
		view->show();
	}
	
	void TmpWidget::buttonBPressed(void)
	{
		std::cout << "TmpWidget : Button B, close" << std::endl;

		view->close();
	}

	void TmpWidget::buttonCPressed(void)
	{
		std::cout << "TmpWidget : Button C" << std::endl;
	}

// TestEditorWidget ;
	CodeEditorSubWidget::CodeEditorSubWidget(void)
	{
		setInnerWidget(&mainWidget);
		setTitle("Test Code Editor");
	}
	
	CodeEditorSubWidget::~CodeEditorSubWidget(void)
	{ }

// Src :
	IHM::IHM(void)
	 : layout(this), saveButton("Save result (RGB888)", this), window(this)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("IHM::IHM - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			if(!HandleOpenGL::isInitialized())
				throw Exception("OpenGL Context not initialized.", __FILE__, __LINE__);

			// Main interface :
			//window.renderer().setKeyboardActions(true);
			//window.renderer().setMouseActions(true);
			//window.renderer().setPixelAspectRatio(1.0f);

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
			//QObject::connect(&(window.renderer()),		SIGNAL(actionReceived(void)),		this, SLOT(updateOutput(void)));
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

		TmpWidget* tmp = new TmpWidget;
		window.addSubWidget(tmp);

		tmp = new TmpWidget;
		window.addSubWidget(tmp);

		tmp = new TmpWidget;
		window.addSubWidget(tmp);

		CodeEditorSubWidget* editor = new CodeEditorSubWidget;
		window.addSubWidget(editor);
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

			updateOutput();
		}
	}

	void IHM::updateOutput(void)
	{
		static bool lock = false;

		if(lock)
			return;

		lock = true;

		//if(imageLoaderInterface.getNumTextures()==0)
			//window.renderer().clearWindow();
		//else
		/*{
			try
			{
				int 	w = pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ).getWidth(),
					h = pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ).getHeight();
				float 	imageAspectRatio = static_cast<float>(w)/static_cast<float>(h);
				//window.renderer().setImageAspectRatio(imageAspectRatio);

				pipelineLoaderInterface.loader().clearRequiredElements("InputFormat");
				pipelineLoaderInterface.loader().addRequiredElement("InputFormat", imageLoaderInterface.currentTexture());

				//window.renderer() << pipelineLoaderInterface.currentOutput( imageLoaderInterface.currentTexture() ) << OutputDevice::Process;
			}
			catch(std::exception& e)
			{
				std::cout << "IHM::updateOutput - Exception while updating : " << std::endl;
				std::cout << e.what() << std::endl;
				log << "IHM::updateOutput - Exception while updating : " << std::endl;
				log << e.what() << std::endl;
			}
		}*/

		lock = false;
	}


	ImageContentInformation::ImageContentInformation(int& argc, char** argv)
	 : QApplication(argc,argv), ihm(NULL)
	{
		try
		{
			// Interface :
			ihm = new IHM;

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

