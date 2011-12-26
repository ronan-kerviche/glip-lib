#include "imageContentInfo.hpp"
#include "WindowRendering.hpp"
#include <cstdlib>
#include <ctime>

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Src :
	Interface::Interface(void)
	 : text(NULL), pipeline(NULL)
	{
		// Main interface :
		window  = new WindowRenderer(this, 640, 480);
		layout 	= new QVBoxLayout(this);
		chImg	= new QPushButton("Load an Image", this);
		chPpl	= new QPushButton("Load a Pipeline", this);


		layout->addWidget(chImg);
		layout->addWidget(chPpl);
		layout->addWidget(window);
		setGeometry(100, 100, 640, 600);
		show();

		QObject::connect(chImg, 	SIGNAL(released()), this, SLOT(loadImage()));
		QObject::connect(chPpl,		SIGNAL(released()), this, SLOT(loadPipeline()));
		QObject::connect(window,	SIGNAL(resized()),  this, SLOT(requestUpdate()));
	}

	void Interface::loadImage(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

		if (!filename.isEmpty())
		{
			QImage* image = new QImage(filename);

			if (image->isNull())
			{
				QMessageBox::information(this, tr("Image Content Info"), tr("Cannot load %1.").arg(filename));
				std::cout << "Cannot load : " << filename.toUtf8().constData() << std::endl;
				delete image;
			}
			else
			{
				if(text!=NULL)
				{
					delete text;
					text = NULL;
				}

				// Build new header :
				HdlTextureFormat fmt(image->width(),image->height(),GL_RGB,GL_UNSIGNED_BYTE,GL_NEAREST,GL_NEAREST);
				text = new HdlTexture(fmt);

				unsigned char* temp = new unsigned char[image->width()*image->height()*3];
				int t=0;

				for(int i=image->height()-1; i>=0; i--)
				{
					for(int j=0; j<image->width(); j++)
					{
						QRgb col 	= image->pixel(j,i);
						temp[t+0] 	= static_cast<unsigned char>( qRed( col ) );
						temp[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
						temp[t+2] 	= static_cast<unsigned char>( qBlue( col ) );
						t += 3;
					}
				}

				text->write(temp);

				delete[] temp;

				requestUpdate();
			}
		}
	}

	void Interface::loadPipeline(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

		if (!filename.isEmpty())
		{
			bool 		success = true;
			PipelineLayout* model 	= NULL;
			LayoutLoader	loader;

			try
			{
				model = loader(filename.toUtf8().constData());
			}
			catch(std::exception& e)
			{
				success = false;

				// Open error window with the message :
				QMessageBox::information(this, tr("Error while loading the pipeline : "), e.what());
				std::cout << "Error while building the pipeline : " << e.what() << std::endl;
			}

			if(success)
			{
				if(pipeline!=NULL)
				{
					delete pipeline;
					pipeline = NULL;
				}

				pipeline = new Pipeline(*model, "LoadedPipeline");
				requestUpdate();
			}
		}
	}

	void Interface::requestUpdate(void)
	{
		if(pipeline!=NULL && text!=NULL)
		{
			bool success = true;

			try
			{
				(*pipeline) << (*text) << Process;
			}
			catch(std::exception& e)
			{
				success = false;

				// Open error window with the message :
				QMessageBox::information(this, tr("Error while computing : "), e.what());
				std::cout << "Error while computing : " << e.what() << std::endl;
			}

			if(success)
			{
				(*window) << pipeline->out(0);
			}
		}
	}


	ImageContentInformation::ImageContentInformation(int _w, int _h, int argc, char** argv)
	 : QApplication(argc,argv), w(_w), h(_h)
	{
		try
		{
			// Interface :
			interface = new Interface;
		}
		catch(Exception& e)
		{
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			throw e;
		}

		std::cout << "--- STARTING ---" << std::endl;
	}
