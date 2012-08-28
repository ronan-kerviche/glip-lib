#include "imageContentInfo.hpp"
#include "WindowRendering.hpp"
#include <cstdlib>
#include <ctime>
#include <QPointer>

// Flags
	//#define __USE_PBO__

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Src :
	IHM::IHM(void)
	 : text(NULL), pipeline(NULL), computingSuccess(false), layout(NULL), chImg(NULL), chPpl(NULL), sav(NULL), window(NULL), box(NULL)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("My Application"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("IHM::IHM - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			// Main interface :
			window  = new WindowRenderer(this, 640, 480);

			// Info :
			log << "> ImageTest" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

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
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			QMessageBox::warning(NULL, tr("ImageTest"), e.what());
			throw e;
		}
	}

	IHM::~IHM(void)
	{
		log << "> End" << std::endl;
		log.close();
		delete text;
		delete layout;
		delete chImg;
		delete chPpl;
		delete sav;
		delete window;
		delete pipeline;
		delete box;
	}

	void IHM::loadImage(void)
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
				HdlTextureFormat fmt(image->width(),image->height(),GL_RGB,GL_UNSIGNED_BYTE,GL_LINEAR,GL_LINEAR);
				text = new HdlTexture(fmt);

				#ifndef __USE_PBO__
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
				#else
					try
					{
						//Example for texture streaming to the GPU (live video for example, but in that case, the PBO is only created once)
						HdlPBO pbo(image->width(),image->height(),3,1,GL_PIXEL_UNPACK_BUFFER_ARB,GL_STREAM_DRAW_ARB);

						// YOU MUST WRITE ONCE IN THE TEXTURE BEFORE USING PBO::copyToTexture ON IT.
						text->fill(0);

						unsigned char* ptr = reinterpret_cast<unsigned char*>(pbo.map());
						int t=0;

						for(int i=image->height()-1; i>=0; i--)
						{
							for(int j=0; j<image->width(); j++)
							{
								QRgb col 	= image->pixel(j,i);
								ptr[t+0] 	= static_cast<unsigned char>( qRed( col ) );
								ptr[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
								ptr[t+2] 	= static_cast<unsigned char>( qBlue( col ) );
								t += 3;
							}
						}

						HdlPBO::unmap(GL_PIXEL_UNPACK_BUFFER_ARB);
						pbo.copyToTexture(*text);
					}
					catch(Exception& e)
					{
						log << "Exception while loading image : " << std::endl;
						log << e.what() << std::endl;
						QMessageBox::information(NULL, tr("Error while build texture from PBO : "), e.what());
						std::cout << "Error while build texture from PBO : " << filename.toUtf8().constData() << std::endl;
						delete image;
						delete text;
						text = NULL;
						return ;
					}

				#endif

				delete image;

				std::cout << "Texture size (for non-compressed texture - getSize()) : " << static_cast<int>(static_cast<float>(text->getSize())/(1024.0*1024.0)) << " MB \t (" << static_cast<int>(static_cast<float>(text->getSize())/1024.0) << "kB)" << std::endl;
				std::cout << "Texture size (for all texture - getSizeOnGPU()) : " << static_cast<int>(static_cast<float>(text->getSizeOnGPU())/(1024.0*1024.0)) << " MB \t (" << static_cast<int>(static_cast<float>(text->getSizeOnGPU())/1024.0) << "kB)"  << std::endl;

				if(pipeline!=NULL)
					requestComputingUpdate();

				updateOutput();
			}
		}
	}

	void IHM::loadPipeline(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load a Pipeline"), ".", "*.ppl");

		if (!filename.isEmpty())
		{
			log << "Building pipeline from : " << filename.toStdString() << std::endl;
			std::cout << "Building : " << filename.toStdString() << std::endl;
			bool 		success = true;
			PipelineLayout* model 	= NULL;
			LayoutLoader	loader;

			try
			{
				model = loader(filename.toStdString());
			}
			catch(Exception& e)
			{
				success = false;
				log << "Exception while loading the pipeline : " << std::endl;
				log << e.what() << std::endl;
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
				catch(Exception& e)
				{
					success = false;
					log << "Exception while creating the pipeline : " << std::endl;
					log << e.what() << std::endl;
					QMessageBox::information(this, tr("Error while creating the pipeline : "), e.what());
					std::cout << "Error while creating the pipeline : " << e.what() << std::endl;

					delete pipeline;
					pipeline = NULL;
				}

				if(success)
				{
					//Test writing : loader.write(*pipeline, "./Filters/writingTest.ppl");
					log << "Pipeline size on the GPU : " << static_cast<int>(static_cast<float>(pipeline->getSize())/1024.0) << " kB" << std::endl;
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
		int port = box->currentIndex();
		QString filename = QFileDialog::getSaveFileName(this);

		if (!filename.isEmpty())
		{
			if(text!=NULL && (port==0 || pipeline!=NULL) )
			{
				// The image selected
				try
				{
					log << "Exporting image... (" << port << ") to " << filename.toStdString() << std::endl;
					std::cout << "Exporting image... (" << port << ')' << std::endl;

					HdlTextureFormat *fmt = NULL;

					// Read the target format :
					if(port==0)
						fmt = new HdlTextureFormat(*text);
					else
						fmt = new HdlTextureFormat(pipeline->out(port-1));

					#define __USE_PBO__
					#ifndef __USE_PBO__
						TextureReader reader("reader",*fmt);
						reader.yFlip = true;

						if(port==0)
							reader << *text;
						else
							reader << pipeline->out(port-1);

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
					#else
						// Create the reader on the right format (inheritance of the texture object)
						PBOTextureReader reader("reader", *fmt, GL_STREAM_READ_ARB);

						// Start copy to PBO :
						if(port==0)
							reader << *text;
						else
							reader << pipeline->out(port-1);

						// Get access to memory :
						unsigned char* ptr = reinterpret_cast<unsigned char*>(reader.startReadingMemory());

						// Create an image with Qt
						QImage image(reader.getWidth(), reader.getHeight(), QImage::Format_RGB888);

						QRgb value;
						double r, g, b;
						int p = 0;
						for(int y=reader.getHeight()-1; y>=0; y--)
						{
							for(int x=0; x<reader.getWidth(); x++)
							{
								r = ptr[p+0];
								g = ptr[p+1];
								b = ptr[p+2];
								value = qRgb(r, g, b);
								image.setPixel(x, y, value);
								p += 3;
							}
						}

						// Close access to memory :
						reader.endReadingMemory();
					#endif

					delete fmt;

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

					pipeline->enablePerfsMonitoring();
					(*pipeline) << (*text) << Pipeline::Process;

					log << "Rendering time : " << pipeline->getTotalTiming() << " ms" << std::endl;

					pipeline->disablePerfsMonitoring();

					std::cout << "...end rendering" << std::endl;
				#endif
			}
			catch(std::exception& e)
			{
				computingSuccess = false;

				log << "Exception while computing : " << std::endl;
				log << e.what() << std::endl;
				QMessageBox::information(this, tr("Error while computing : "), e.what());
				std::cout << "Error while computing : " << e.what() << std::endl;
			}
		}
	}

	void IHM::updateOutput(void)
	{
		try
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
		catch(std::exception& e)
		{
			log << "Exception while updating : " << std::endl;
			log << e.what() << std::endl;
		}
	}


	ImageContentInformation::ImageContentInformation(int _w, int _h, int& argc, char** argv)
	 : QApplication(argc,argv), w(_w), h(_h), ihm(NULL)
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
			QMessageBox::information(NULL, tr("Exception caught : "), e.what());
			throw e;
		}

		std::cout << "--- STARTING ---" << std::endl;
	}

	ImageContentInformation::~ImageContentInformation(void)
	{
		delete ihm;
	}
