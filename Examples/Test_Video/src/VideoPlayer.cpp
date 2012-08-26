#include "VideoPlayer.hpp"
#include "WindowRendering.hpp"

// Namespace
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Objects
	VideoModIHM::VideoModIHM(void)
	 : QWidget(), player(NULL), src(NULL), output(NULL), pbo(NULL), textureA(NULL), textureB(NULL), textureLatest(NULL), textureOldest(NULL), pipeline(NULL), videosLayout(NULL), streamControlsLayout(NULL), generalLayout(NULL), loadVideo(NULL), play(NULL), pause(NULL), loadPipeline(NULL), saveFrame(NULL), pipelineControl(NULL), box(NULL), timer(NULL), pixmap(NULL), image(NULL), fmt(NULL), buffer(NULL), timeLayout(NULL), timeSlider(NULL), timeLabel(NULL)
	{
		videosLayout		= new QHBoxLayout();
		timeLayout		= new QHBoxLayout();
		streamControlsLayout	= new QHBoxLayout();
		pipelineControl		= new QHBoxLayout();
		generalLayout		= new QVBoxLayout(this);

		player 			= new Phonon::VideoPlayer(Phonon::VideoCategory);
		output  		= new WindowRenderer(this, 640, 480);

		timeSlider		= new QSlider(this);
		timeSlider->setOrientation(Qt::Horizontal);
		timeSlider->setRange(0,1000);
		timeLabel		= new QLabel("00:00", this);
		timeLabel->setFixedWidth(100);
		timeLabel->setAlignment(Qt::AlignHCenter);

		loadVideo		= new QPushButton("Open a Video", this);
		play			= new QPushButton("Play", this);
		pause			= new QPushButton("Pause", this);
		loadPipeline		= new QPushButton("Open a Pipeline", this);
		saveFrame		= new QPushButton("Save Frame", this);

		box			= new QComboBox(this);
		box->addItem("<Input Stream>");

		pixmap			= new QPixmap();
		image			= new QImage();

		fmt			= new HdlTextureFormat(1,1,GL_RGB,GL_UNSIGNED_BYTE,GL_LINEAR,GL_LINEAR);

		videosLayout->addWidget(player);
		videosLayout->addWidget(output);
		timeLayout->addWidget(timeLabel);
		timeLayout->addWidget(timeSlider);
		streamControlsLayout->addWidget(loadVideo);
		streamControlsLayout->addWidget(play);
		streamControlsLayout->addWidget(pause);
		pipelineControl->addWidget(loadPipeline);
		pipelineControl->addWidget(box);
		pipelineControl->addWidget(saveFrame);

		generalLayout->addLayout(videosLayout);
		generalLayout->addLayout(timeLayout);
		generalLayout->addLayout(streamControlsLayout);
		generalLayout->addLayout(pipelineControl);

		setGeometry(0, 0, 1280, 720);
		show();

		// Timer :
		timer = new QTimer;
		timer->setInterval(50);

		// Connections :
		QObject::connect(loadVideo, 	SIGNAL(released(void)), 	this, 	SLOT(openVideo(void)));
		QObject::connect(play, 		SIGNAL(released(void)), 	player,	SLOT(play(void)));
		QObject::connect(pause, 	SIGNAL(released(void)), 	player,	SLOT(pause(void)));
		QObject::connect(loadPipeline,	SIGNAL(released(void)), 	this,	SLOT(openPipeline(void)));
		QObject::connect(saveFrame,	SIGNAL(released(void)), 	this,	SLOT(save(void)));
		QObject::connect(timer, 	SIGNAL(timeout(void)),		this, 	SLOT(grabFrame(void)));
		QObject::connect(timer, 	SIGNAL(timeout(void)),		this, 	SLOT(updateTime(void)));
		QObject::connect(timeSlider,	SIGNAL(sliderReleased()),	this, 	SLOT(seekPosition(void)));

		timer->start();
	}

	VideoModIHM::~VideoModIHM(void)
	{
		timer->stop();
		delete player;
		delete src;
		delete output;
		delete pbo;
		delete timeSlider;
		delete timeLabel;
		delete textureA;
		delete textureB;
		delete pipeline;
		delete videosLayout;
		delete timeLayout;
		delete streamControlsLayout;
		delete pipelineControl;
		delete generalLayout;
		delete loadVideo;
		delete play;
		delete pause;
		delete loadPipeline;
		delete saveFrame;
		delete box;
		delete pixmap;
		delete image;
		delete fmt;
		delete[] buffer;
	}

	void VideoModIHM::openVideo(void)
	{
		QStringList types = Phonon::BackendCapabilities::availableMimeTypes();
		for(int i = 0; i<types.size(); i++)
			std::cout << types[i].toStdString() << std::endl;

		// Get a filename :
		QString filename = QFileDialog::getOpenFileName(this, tr("Open a Video"), ".", "*");

		if (!filename.isEmpty())
		{
			if(player->isPlaying())
			{
				player->stop();
				delete src;
			}

			// Try to open stream :
			src = new Phonon::MediaSource(filename);
			player->play(*src);
		}
	}

	void VideoModIHM::openPipeline(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load a Pipeline"), ".", "*.ppl");

		if (!filename.isEmpty())
		{
			std::cout << "Building : " << filename.toStdString() << std::endl;
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

					QMessageBox::information(this, tr("Error while creating the pipeline : \n"), e.what());
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
				}

				delete model;
			}
		}
	}

	void VideoModIHM::grabFrame(void)
	{
		if(player->isPlaying() && src!=NULL)
		{
			try
			{
				std::cout << "Grabing image..." << std::endl;
				const QPixmap& snapshot = QPixmap::grabWindow(player->videoWidget()->winId());
				(*image) = snapshot.toImage();
				QImage& img = (*image);

				if(img.isNull())
					throw Exception("VideoModIHM::grabFrame - NULL Image.", __FILE__, __LINE__);


				// Check size :
				const int 	w = img.width(),
						h = img.height();

				// Check ressources :
				if(fmt->getWidth()!=w || fmt->getHeight()!=h)
				{
					std::cout << "Updating format to : " << w << "X" << h << " pixels." << std::endl;
					fmt->setWidth(w);
					fmt->setHeight(h);

					if(pbo!=NULL)
						delete pbo;
					pbo = new HdlPBO(*fmt,GL_PIXEL_UNPACK_BUFFER_ARB,GL_STREAM_DRAW_ARB);

					if(textureA!=NULL)
						delete textureA;
					textureA = new HdlTexture(*fmt);

					if(textureB!=NULL)
						delete textureB;
					textureB = new HdlTexture(*fmt);

					if(buffer!=NULL)
						delete[] buffer;
					buffer = new unsigned char[textureA->getSize()];

					textureOldest = textureA;
					textureLatest = textureB;

					std::cout << "Done" << std::endl;
				}

				// Transcode image :
				int t=0;

				for(int i=h-1; i>=0; i--)
				{
					for(int j=0; j<w; j++)
					{
						QRgb col 	= img.pixel(j,i);
						buffer[t+0] 	= static_cast<unsigned char>( qRed( col ) );
						buffer[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
						buffer[t+2] 	= static_cast<unsigned char>( qBlue( col ) );
						t += 3;
					}
				}
				textureOldest->write(buffer);

				// swap order :
				HdlTexture* tmp = textureLatest;
				textureLatest = textureOldest;
				textureOldest = tmp;

				// Update pipeline if needed :
				if(pipeline!=NULL)
				{
					if(pipeline->getNumInputPort()==1)
						(*pipeline) << (*textureLatest) << Pipeline::Process;
					else if(pipeline->getNumInputPort()==2)
						(*pipeline) << (*textureLatest) << (*textureOldest) << Pipeline::Process;

				}
			}
			catch(Exception& e)
			{
				QMessageBox::information(NULL, tr("Error while build texture from PBO : \n"), e.what());
				std::cout << "Error while build texture from PBO : \n" << e.what() << std::endl;
				delete pbo;
				delete textureA;
				delete textureB;
				textureLatest = NULL;
				textureOldest = NULL;
				pbo = NULL;
				textureA = NULL;
				textureB = NULL;
			}
		}

		drawOutput();
	}

	void VideoModIHM::drawOutput(void)
	{
		int port = box->currentIndex();

		if( (pipeline!=NULL) || (port==0 && textureLatest!=NULL) )
		{
			if(port==0)
				(*output) << (*textureLatest);
			else
				(*output) << pipeline->out(port-1);
		}
	}

	void VideoModIHM::save(void)
	{
		int port = box->currentIndex();
		QString filename = QFileDialog::getSaveFileName(this);

		if (!filename.isEmpty())
		{
			if(textureLatest!=NULL && (port==0 || pipeline!=NULL) )
			{
				// The image selected
				try
				{
					std::cout << "Exporting image... (" << port << ')' << std::endl;

					HdlTextureFormat *tmpfmt = NULL;

					// Read the target format :
					if(port==0)
						tmpfmt = new HdlTextureFormat(*textureLatest);
					else
						tmpfmt = new HdlTextureFormat(pipeline->out(port-1));

					TextureReader reader("reader",*tmpfmt);
					reader.yFlip = true;

					if(port==0)
						reader << *textureLatest;
					else if(pipeline!=NULL)
						reader << pipeline->out(port-1);
					else
						throw Exception("VideoModIHM::save - Cannot save NULL content.", __FILE__, __LINE__);

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

					delete tmpfmt;

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

	void VideoModIHM::updateTime(void)
	{
		if(player->isPlaying() && !timeSlider->isSliderDown())
		{
			double 	c = player->currentTime(),
				t = player->totalTime();

			timeSlider->setSliderPosition(static_cast<int>(c/t*1000.0));
			int 	m  = static_cast<int>(c/60000.0);
			c -= m*60000.0;
			int	s  = static_cast<int>(c/1000.0);

			timeLabel->setText(tr("%1:%2").arg(m).arg(s));
		}
	}

	void VideoModIHM::seekPosition(void)
	{
		if(player->mediaObject()->isSeekable())
		{
			double val = timeSlider->value();
			val /= 1000.0;
			qint64 p = static_cast<qint64>(val*player->totalTime());
			player->seek(p);
		}
	}

	VideoModApplication::VideoModApplication(int& argc, char** argv)
	 : QApplication(argc,argv)
	{
		setApplicationName("GlipLib-VideoPipeline");
		ihm = new VideoModIHM();
	}

	VideoModApplication::~VideoModApplication()
	{
		delete ihm;
	}
