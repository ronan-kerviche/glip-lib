#include "imageContentInfo.hpp"
#include "WindowRendering.hpp"
#include <cstdlib>
#include <ctime>
#include <QPointer>

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Src :
	IHM::IHM(void)
	 : text(NULL), pipeline(NULL), computingSuccess(false)
	{
		// Main interface :
		window  = new WindowRenderer(this, 640, 480);
		layout 	= new QVBoxLayout(this);
		chImg	= new QPushButton("Load an Image", this);
		chPpl	= new QPushButton("Load a Pipeline", this);
		sav	= new QPushButton("Save result (RGB888)", this);
		box	= new QComboBox(this);

		layout->addWidget(chImg);
		layout->addWidget(chPpl);
		layout->addWidget(box);
		layout->addWidget(window);
		layout->addWidget(sav);
		setGeometry(1000, 100, 800, 700);
		show();

		box->addItem("<Input Image>");

		QObject::connect(chImg, 	SIGNAL(released(void)), this, SLOT(loadImage(void)));
		QObject::connect(chPpl,		SIGNAL(released(void)), this, SLOT(loadPipeline(void)));
		QObject::connect(sav,		SIGNAL(released(void)), this, SLOT(save(void)));
		QObject::connect(window,	SIGNAL(resized(void)),  this, SLOT(updateOutput(void)));
		QObject::connect(box, 		SIGNAL(currentIndexChanged(int)),	this, SLOT(updateOutput(void)));
	}

	void IHM::loadImage(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load an Image"), ".", "*.jpg *.JPG *.png");

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
				delete image;

				std::cout << "Texture size : " << static_cast<int>(static_cast<float>(text->getSize())/(1024.0*1024.0)) << " MB " << std::endl;

				if(pipeline!=NULL)
					requestComputingUpdate();

				updateOutput();
			}
		}
	}

	void IHM::loadPipeline(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load an Pipeline"), ".", "*.ppl");

		if (!filename.isEmpty())
		{
			std::cout << "Building : " << filename.toUtf8().constData() << std::endl;
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

				QMessageBox::information(NULL, tr("Error while loading the pipeline : "), e.what());
				std::cout << "Error while building the pipeline : " << e.what() << std::endl;
			}

			if(success)
			{
				if(pipeline!=NULL)
				{
					delete pipeline;
					pipeline = NULL;

					// clean the box too...
					while(box->count()>1)
						box->removeItem(1);
				}

				try
				{
					pipeline = new Pipeline(*model, "LoadedPipeline");
				}
				catch(std::exception& e)
				{
					success = false;

					QMessageBox::information(this, tr("Error while creating the pipeline : "), e.what());
					std::cout << "Error while creating the pipeline : " << e.what() << std::endl;

					delete pipeline;
					pipeline = NULL;
				}

				if(success)
				{
					//Test writing : loader.write(*pipeline, "./Filters/writingTest.ppl");
					std::cout << "Pipeline size on the GPU : " << static_cast<int>(static_cast<float>(pipeline->getSize())/(1024.0*1024.0)) << " MB" << std::endl;

					// Update the box :
					for(int i=0; i<pipeline->getNumOutputPort(); i++)
						box->addItem(pipeline->getOutputPortName(i).c_str());

					requestComputingUpdate();
					updateOutput();
				}

				delete model;
			}
		}
	}

	void IHM::save(void)
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
				catch(std::exception& e)
				{
					QMessageBox::information(this, tr("Error while saving : "), e.what());
					std::cout << "Error while saving : " << e.what() << std::endl;
				}
			}
		}
	}

	void IHM::requestComputingUpdate(void)
	{
		if(pipeline!=NULL && text!=NULL)
		{
			computingSuccess = true;

			try
			{
				//#define __BENCH_MARK__
				#ifdef __BENCH_MARK__
					pipeline->enablePerfsMonitoring();
					float tmp = 0.0;

					std::cout << "Rendering..." << std::endl;

					for(int i = 0; i<100; i++)
					{
						(*pipeline) << (*text) << Pipeline::Process;
						tmp += pipeline->getTotalTiming();
					}
					std::cout << "...end rendering" << std::endl;
					sleep(1);

					std::cout << "Time : " << tmp/100.0f << "ms " << std::endl;
					for(int i=0; i<pipeline->getNumActions(); i++)
					{
						std::string name;
						float t = pipeline->getTiming(i, name);
						std::cout << "    Filter " << name << " \t : " << t << "ms" << std::endl;
					}
				#else
					std::cout << "Rendering..." << std::endl;
					(*pipeline) << (*text) << Pipeline::Process;
					std::cout << "...end rendering" << std::endl;
				#endif
			}
			catch(std::exception& e)
			{
				computingSuccess = false;

				QMessageBox::information(this, tr("Error while computing : "), e.what());
				std::cout << "Error while computing : " << e.what() << std::endl;
			}
		}
	}

	void IHM::updateOutput(void)
	{
		int port = box->currentIndex();

		if( (computingSuccess && pipeline!=NULL) || (port==0 && text!=NULL) )
		{
			if(port==0)
				(*window) << (*text);
			else
				(*window) << pipeline->out(port-1);
		}
	}


	ImageContentInformation::ImageContentInformation(int _w, int _h, int& argc, char** argv)
	 : QApplication(argc,argv), w(_w), h(_h)
	{
		try
		{
			// Interface :
			ihm = new IHM;
		}
		catch(std::exception& e)
		{
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			throw e;
		}

		std::cout << "--- STARTING ---" << std::endl;
	}
