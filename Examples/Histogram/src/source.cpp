#include "header.hpp"
#include "WindowRendering.hpp"

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Code
	HistogramInterface::HistogramInterface(void)
	 : text(NULL), pipeline(NULL), layout(NULL), chImg(NULL), sav(NULL), window(NULL)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("HistogramInterface::HistogramInterface - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			// Main interface :
			window  = new WindowRenderer(this, 640, 480);

			// Info :
			log << "> Histogram" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			layout 	= new QVBoxLayout(this);
			chImg	= new QPushButton("Load an Image", this);
			sav	= new QPushButton("Save result (RGB888)", this);

			layout->addWidget(chImg);
			layout->addWidget(window);
			layout->addWidget(sav);
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


			QObject::connect(chImg, 	SIGNAL(released(void)), this, SLOT(loadImage(void)));
			QObject::connect(sav,		SIGNAL(released(void)), this, SLOT(save(void)));
			QObject::connect(window,	SIGNAL(resized(void)),  this, SLOT(requestUpdate(void)));

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

		delete text;
		delete layout;
		delete chImg;
		delete sav;
		delete window;
		delete pipeline;
	}

	void HistogramInterface::loadImage(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load an Image"), ".", "*.jpg *.JPG *.png");

		if (!filename.isEmpty())
		{
			log << "Loading : " << filename.toStdString() << std::endl;

			QImage* image = new QImage(filename);

			if (image->isNull())
			{
				log << "Failed to load file!" << std::endl;
				QMessageBox::information(this, tr("Image Content Info"), tr("Cannot load %1.").arg(filename));
				std::cout << "Cannot load : " << filename.toUtf8().constData() << std::endl;
				delete image;
			}
			else
			{
				log << "Image format : "<< image->width() << 'X' << image->height() << std::endl;

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
				delete image;

				std::cout << "Texture size : " << static_cast<int>(static_cast<float>(text->getSize())/(1024.0*1024.0)) << " MB " << std::endl;

				// Update the grid of the pipeline :
				(*pipeline)["instHistRed"].setGeometry(HdlVBO::generate2DGrid(text->getWidth(), text->getHeight()));
				(*pipeline)["instHistGreen"].setGeometry(HdlVBO::generate2DGrid(text->getWidth(), text->getHeight()));
				(*pipeline)["instHistBlue"].setGeometry(HdlVBO::generate2DGrid(text->getWidth(), text->getHeight()));

				float a = 1.0/static_cast<float>(text->getWidth()*text->getHeight());
				((*pipeline)["instHistRed"]).prgm().modifyVar("nrm",HdlProgram::Var,a);
				((*pipeline)["instHistGreen"]).prgm().modifyVar("nrm",HdlProgram::Var,a);
				((*pipeline)["instHistBlue"]).prgm().modifyVar("nrm",HdlProgram::Var,a);

				try
				{
					#ifdef __BURN_TEST__
						pipeline->enablePerfsMonitoring();
						float tmp = 0.0;

						std::cout << "Rendering..." << std::endl;

						for(int i = 0; i<100; i++)
						{
							(*pipeline) << (*text) << Pipeline::Process;
							tmp += pipeline->getTotalTiming();
						}
						std::cout << "...end rendering" << std::endl;

						std::cout << "Time : " << tmp/100.0f << "ms " << std::endl;
					#else
						pipeline->enablePerfsMonitoring();
						std::cout << "Rendering..." << std::endl;
						(*pipeline) << (*text) << Pipeline::Process;
						std::cout << "...end rendering" << std::endl;
						std::cout << "Time : " << pipeline->getTotalTiming() << "ms " << std::endl;
						log << "Rendering time : " << pipeline->getTotalTiming() << "ms " << std::endl;
					#endif
				}
				catch(Exception& e)
				{
					QMessageBox::information(this, tr("Error while computing : "), e.what());
					log << "Exception while computing : " << std::endl;
					log << e.what() << std::endl;
					log << "> Abort" << std::endl;
					log.close();
					std::cout << "Error while computing : " << e.what() << std::endl;
					throw e;
				}

				requestUpdate();
			}
		}
	}

	void HistogramInterface::save(void)
	{
		QString filename = QFileDialog::getSaveFileName(this);

		if (!filename.isEmpty())
		{
			std::cout << "Pipeline : " << pipeline << std::endl;
			std::cout << "Texture  : " << text << std::endl;
			if(pipeline!=NULL && text!=NULL)
			{
				try
				{
					std::cout << "Exporting image..." << std::endl;
					TextureReader reader("reader",pipeline->out(0));
					reader.yFlip = true;
					reader << pipeline->out(0);

					// Create an image with Qt
					QImage image(reader.getWidth(), reader.getHeight(), QImage::Format_RGB888);

					QRgb value;
					double r, g, b;
					for(int y=0; y<reader.getHeight(); y++)
					{
						for(int x=0; x<reader.getWidth(); x++)
						{
							r = static_cast<unsigned char>(reader(x,y,0)*255.0);
							g = static_cast<unsigned char>(reader(x,y,1)*255.0);
							b = static_cast<unsigned char>(reader(x,y,2)*255.0);
							value = qRgb(r, g, b);
							image.setPixel(x, y, value);
						}
					}

					if(!image.save(filename))
					{
						QMessageBox::information(this, tr("Error while writing : "), filename);
						std::cout << "Error while writing : " << filename.toUtf8().constData() << std::endl;
					}
				}
				catch(Exception& e)
				{
					QMessageBox::information(this, tr("Error while saving : "), e.what());
					log << "Exception while computing : " << std::endl;
					log << e.what() << std::endl;
					log << "> Abort" << std::endl;
					log.close();
					std::cout << "Error while saving : " << e.what() << std::endl;
				}
			}
		}
	}

	void HistogramInterface::requestUpdate(void)
	{
		if(pipeline!=NULL)
			(*window) << pipeline->out(0);
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
