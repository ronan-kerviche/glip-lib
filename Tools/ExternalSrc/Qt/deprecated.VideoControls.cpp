/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : VideoControls.cpp                                                                         */
/*     Original Date : December 31th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for loading and playing videos with FFMPEG library.                          */
/*                     Require ResourceLoader.*pp and VideoStream.*pp.					         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "VideoControls.hpp"
#include <QFileDialog>
#include <QMessageBox>

// VideoOptionsDialog
	VideoOptionsDialog::VideoOptionsDialog(QWidget* parent, GLenum defMinFilter, GLenum defMagFilter, GLenum defSWrapping, GLenum defTWrapping, int defMaxLevel, int defNumFrameBuffered)
	 : QDialog(parent), options(this, defMinFilter, defMagFilter, defSWrapping, defTWrapping, defMaxLevel), numFrameBufferedLabel("Number of frames buffered"), okButton("Ok"), cancelButton("Cancel")
	{
		numFrameBufferedSpinBox.setRange(1,10);
		numFrameBufferedSpinBox.setValue(defNumFrameBuffered);

		options.addWidget(&numFrameBufferedLabel,	2, 2);
		options.addWidget(&numFrameBufferedSpinBox,	2, 3);
		options.addWidget(&cancelButton,		3, 2);
		options.addWidget(&okButton,			3, 3);

		QObject::connect(&cancelButton, SIGNAL(released(void)), this, SLOT(reject(void)));
		QObject::connect(&okButton, 	SIGNAL(released(void)), this, SLOT(accept(void)));
	}

	VideoOptionsDialog::~VideoOptionsDialog(void)
	{ }

	int VideoOptionsDialog::getNumFrameBuffered(void) const
	{
		return numFrameBufferedSpinBox.value();
	}

// VideoControls
	VideoControls::VideoControls(QWidget* parent)
	 : QVBoxLayout(parent), timeLabel("--:--"), frameRateLabel("Frame rate : "), playButton("Play"), stopButton("Stop"), loadVideoButton("Load video..."), optionsButton("Options"), infoLine("(No video)"), timeSlider(Qt::Horizontal), nextFrameButton(">"),
	   minFilter(GL_NEAREST), magFilter(GL_NEAREST), sWrapping(GL_CLAMP), tWrapping(GL_CLAMP), maxMipmapLevel(0), numFrameBuffered(1), playing(false), stream(NULL)
	{
		frameRateLabel.setAlignment(Qt::AlignRight);
		frameRateLabel.setFixedSize(90, 20);
		nextFrameButton.setFixedSize(20, 20);
		timeLabel.setAlignment(Qt::AlignCenter);
		timeLabel.setFixedSize(80, 20);
		frameRateSpinBox.setRange(1, 1000);
		infoLine.setReadOnly(true);
		timeSlider.setRange(0,0);

		layout1.addWidget(&timeLabel);
		layout1.addWidget(&timeSlider);
		layout1.addWidget(&nextFrameButton);

		layout2.addWidget(&playButton);
		layout2.addWidget(&stopButton);
		layout2.addWidget(&infoLine);

		layout3.addWidget(&loadVideoButton);
		layout3.addWidget(&optionsButton);
		layout3.addWidget(&frameRateLabel);
		layout3.addWidget(&frameRateSpinBox);

		addLayout(&layout1);
		addLayout(&layout2);
		addLayout(&layout3);

		updateLoadToolTip();
		updateInfoTool();

		timer.setInterval(1000);

		QObject::connect(&loadVideoButton,	SIGNAL(released(void)), 	this, SLOT(loadVideoFile(void)));
		QObject::connect(&optionsButton,	SIGNAL(released(void)), 	this, SLOT(changeOptions(void)));
		QObject::connect(&playButton,		SIGNAL(released(void)),		this, SLOT(togglePlayPause(void)));
		QObject::connect(&stopButton,		SIGNAL(released(void)),		this, SLOT(stop(void)));
		QObject::connect(&timer, 		SIGNAL(timeout(void)),		this, SLOT(timerTick(void)));
		QObject::connect(&timeSlider,		SIGNAL(sliderMoved(int)),	this, SLOT(seek(void)));
		QObject::connect(&frameRateSpinBox,	SIGNAL(valueChanged(int)),	this, SLOT(changeFrameRate(void)));
		QObject::connect(&nextFrameButton,	SIGNAL(released(void)),		this, SLOT(oneFrameForward(void)));

		frameRateSpinBox.setValue(30);
	}

	VideoControls::~VideoControls(void)
	{
		timer.stop();
		delete stream;
	}

	void VideoControls::timerTick(void)
	{
		if(playing && !timeSlider.isSliderDown() && stream!=NULL)
		{
			stream->readNextFrame();
			timeSlider.setValue(stream->getCurrentTimeSec());

			int 	c = stream->getCurrentTimeSec(),
				t = stream->getVideoDurationSec();
			timeLabel.setText(tr("%1 : %2").arg(c).arg(t));

			// stop if over :
			if(stream->isOver())
				stop();

			emit frameUpdated();
		}
	}

	void VideoControls::updateFrame(void)
	{
		if(videoStreamIsValid())
		{
			stream->readNextFrame();

			// stop if over :
			if(stream->isOver())
				stop();

			emit frameUpdated();
		}
	}

	void VideoControls::updateInfoTool(const QString& filename)
	{
		if(stream!=NULL)
		{
			std::string tip = "Current video have the following parameters :\n    Minification filter \t : " + getGLEnumName(stream->out(0).getMinFilter()) + "\n    Magnification filter \t : " + getGLEnumName(stream->out(0).getMagFilter()) + "\n    S wrapping mode \t : " + getGLEnumName(stream->out(0).getSWrapping()) + "\n    T wrapping mode \t : " + getGLEnumName(stream->out(0).getTWrapping()) + "\n    Max Mipmap level \t : " + to_string(stream->out(0).getMaxLevel()) + "\n    Number of buffers \t : " + to_string(stream->getNumOutputPort());
			infoLine.setToolTip(tip.c_str());

			std::string info = "File : " + filename.toStdString() + " (" + to_string(stream->out(0).getWidth()) + "x" + to_string(stream->out(0).getHeight()) + ")";
			infoLine.setText(info.c_str());
		}
		else
		{
			infoLine.setToolTip("");
			infoLine.setText("(No video)");
		}
	}

	void VideoControls::updateLoadToolTip(void)
	{
		std::string tip = "Videos will be loaded with the following parameters :\n    Minification filter \t : " + getGLEnumName(minFilter) + "\n    Magnification filter \t : " + getGLEnumName(magFilter) + "\n    S wrapping mode \t : " + getGLEnumName(sWrapping) + "\n    T wrapping mode \t : " + getGLEnumName(tWrapping) + "\n    Max Mipmap level \t : " + to_string(maxMipmapLevel) + "\n    Number of buffers \t : " + to_string(numFrameBuffered);

		loadVideoButton.setToolTip(tip.c_str());
	}

	void VideoControls::changeFrameRate(void)
	{
		timer.stop();
		timer.setInterval(1000.0f/static_cast<float>(frameRateSpinBox.value()));
		timer.start();
	}

	void VideoControls::seek(void)
	{
		if(timeSlider.isSliderDown())
		{
			stream->seek( timeSlider.value() );

			int 	c = stream->getCurrentTimeSec(),
				t = stream->getVideoDurationSec();
			timeLabel.setText(tr("%1 : %2").arg(c).arg(t));

			updateFrame();
		}
	}

	bool VideoControls::videoStreamIsValid(void) const
	{
		return stream!=NULL;
	}

	bool VideoControls::isPlaying(void) const
	{
		return playing && !timeSlider.isSliderDown() && stream!=NULL;
	}

	VideoStream& VideoControls::videoStream(void)
	{
		if(!videoStreamIsValid())
			throw Exception("VideoControls::videoStream - Video stream is invalid (no stream loaded).", __FILE__, __LINE__);
		else
			return *stream;
	}

	void VideoControls::loadVideoFile(void)
	{
		QString filename = QFileDialog::getOpenFileName(parentWidget(), QObject::tr("Open a video..."), ".", "*.mp4 *.avi *.mov *.flv *.webm");

		if (!filename.isEmpty())
		{
			pause();

			try
			{
				std::string stdFilename = filename.toStdString();
				VideoStream* tmp = new VideoStream(stdFilename, numFrameBuffered, minFilter, magFilter, sWrapping, tWrapping, maxMipmapLevel);

				if(stream!=NULL)
					delete stream;

				stream = tmp;

				timeSlider.setRange(0, stream->getVideoDurationSec());

				QFileInfo pathInfo( filename );
				QString strippedFilename( pathInfo.fileName() );
				updateInfoTool(strippedFilename);

				emit newVideoLoaded();

				play();
			}
			catch(Exception& e)
			{
				QMessageBox::information(parentWidget(), tr("VideoControls::loadVideoFile - Error while loading the video : "), e.what());
			}
		}
	}

	void VideoControls::changeOptions(void)
	{
		VideoOptionsDialog optionsDialog(parentWidget(), minFilter, magFilter, sWrapping, tWrapping, maxMipmapLevel, numFrameBuffered);

		int result = optionsDialog.exec();

		if(result==QDialog::Accepted)
		{
			minFilter	= optionsDialog.options.getMinFilter();
			magFilter	= optionsDialog.options.getMagFilter();
			sWrapping	= optionsDialog.options.getSWrapping();
			tWrapping	= optionsDialog.options.getTWrapping();
			maxMipmapLevel	= optionsDialog.options.getMaxLevel();
			numFrameBuffered= optionsDialog.getNumFrameBuffered();

			updateLoadToolTip();
		}
	}

	void VideoControls::play(void)
	{
		if(!playing && stream!=NULL)
		{
			playing = true;
			playButton.setText("Pause");
			timer.start();
		}
	}

	void VideoControls::pause(void)
	{
		if(playing)
		{
			playing = false;
			playButton.setText("Play");
			timer.stop();
		}
	}

	void VideoControls::stop(void)
	{
		if(stream!=NULL)
		{
			playing = false;
			timer.stop();

			playButton.setText("Play");
			timeSlider.setValue(0);

			int 	t = stream->getVideoDurationSec();
			timeLabel.setText(tr("0 : %1").arg(t));

			stream->seek(0.0f);
			stream->readNextFrame();

			emit frameUpdated();
		}
	}

	void VideoControls::togglePlayPause(void)
	{
		if(playing)
			pause();
		else
			play();
	}

	void VideoControls::oneFrameForward(void)
	{
		if(videoStreamIsValid())
		{
			stream->readNextFrame();

			// stop if over :
			if(stream->isOver())
				stop();

			emit frameUpdated();
		}
	}

// VideoRecordingDialog
	VideoRecordingDialog::VideoRecordingDialog(QWidget* parent)
	 : QWidget(parent), layout(this), recordingStateLabel("Status :"), filenameLabel("Filename :"), portNameLabel("Recording port :"), formatLabel1("Format :"),formatLabel2("Filtering (min/mag) :"), formatLabel3("Wrapping (S/T) :"), numFramesLabel("Number of frames recorded :"), durationLabel("Recording time :"), pixelFormatLabel("Pixel format : "), frameRateLabel("Frame rate : "), videoBitRateLabel("Video bit rate :"), finishRecording("End recording")
	{
		recordingStateLabel.setAlignment(Qt::AlignRight);
		filenameLabel.setAlignment(Qt::AlignRight);
		portNameLabel.setAlignment(Qt::AlignRight);
		formatLabel1.setAlignment(Qt::AlignRight);
		formatLabel2.setAlignment(Qt::AlignRight);
		formatLabel3.setAlignment(Qt::AlignRight);
		numFramesLabel.setAlignment(Qt::AlignRight);
		durationLabel.setAlignment(Qt::AlignRight);
		pixelFormatLabel.setAlignment(Qt::AlignRight);
		frameRateLabel.setAlignment(Qt::AlignRight);
		videoBitRateLabel.setAlignment(Qt::AlignRight);

		recordingStateInfo.setReadOnly(true);
		filenameInfo.setReadOnly(true);
		portNameInfo.setReadOnly(true);
		formatInfo1.setReadOnly(true);
		formatInfo2.setReadOnly(true);
		formatInfo3.setReadOnly(true);
		numFramesInfo.setReadOnly(true);
		durationInfo.setReadOnly(true);
		pixelFormatInfo.setReadOnly(true);
		frameRateInfo.setReadOnly(true);
		videoBitRateInfo.setReadOnly(true);

		layout.addWidget(&recordingStateLabel,	0, 0);	layout.addWidget(&recordingStateInfo,	0, 1);
		layout.addWidget(&filenameLabel,	1, 0);	layout.addWidget(&filenameInfo,		1, 1);
		layout.addWidget(&portNameLabel,	2, 0);	layout.addWidget(&portNameInfo,		2, 1);
		layout.addWidget(&formatLabel1,		3, 0);	layout.addWidget(&formatInfo1,		3, 1);
		layout.addWidget(&formatLabel2,		4, 0);	layout.addWidget(&formatInfo2,		4, 1);
		layout.addWidget(&formatLabel3,		5, 0);	layout.addWidget(&formatInfo3,		5, 1);
		layout.addWidget(&numFramesLabel,	6, 0);	layout.addWidget(&numFramesInfo,	6, 1);
		layout.addWidget(&durationLabel,	7, 0);	layout.addWidget(&durationInfo,		7, 1);
		layout.addWidget(&pixelFormatLabel,	8, 0);	layout.addWidget(&pixelFormatInfo,	8, 1);
		layout.addWidget(&frameRateLabel,	9, 0);	layout.addWidget(&frameRateInfo,	9, 1);
		layout.addWidget(&videoBitRateLabel,   10, 0);	layout.addWidget(&videoBitRateInfo,    10, 1);
								layout.addWidget(&finishRecording,     11, 1);

		layout.setColumnMinimumWidth(1, 400);

		QObject::connect(&finishRecording, 	SIGNAL(released(void)), this, SIGNAL(stopRecording(void)));
	}

	VideoRecordingDialog::~VideoRecordingDialog(void)
	{ }

	void VideoRecordingDialog::updateStatus(bool isRecording)
	{
		if(isRecording)
			recordingStateInfo.setText("Recording...");
		else
			recordingStateInfo.setText("Not Recording.");
	}

	void VideoRecordingDialog::updateFilename(const std::string& filename)
	{
		filenameInfo.setText(QString(filename.c_str()));
	}

	void VideoRecordingDialog::updatePortName(const std::string& name)
	{
		portNameInfo.setText(QString(name.c_str()));
	}

	void VideoRecordingDialog::updateFormat(const HdlAbstractTextureFormat& fmt)
	{
		int weight 	= fmt.getSize()/1024 + 1;
		QString mode 	= getGLEnumName(fmt.getGLMode()).c_str(),
			depth 	= getGLEnumName(fmt.getGLDepth()).c_str(),
			minF	= getGLEnumName(fmt.getMinFilter()).c_str(),
			magF	= getGLEnumName(fmt.getMagFilter()).c_str(),
			sWrap	= getGLEnumName(fmt.getSWrapping()).c_str(),
			tWrap	= getGLEnumName(fmt.getTWrapping()).c_str();

		formatInfo1.setText(tr("%1x%2, %3, %4, %5 kB").arg(fmt.getWidth()).arg(fmt.getHeight()).arg(mode).arg(depth).arg(weight));
		formatInfo2.setText(tr("%1 / %2").arg(minF).arg(magF));
		formatInfo3.setText(tr("%1 / %2").arg(sWrap).arg(tWrap));
	}

	void VideoRecordingDialog::updateFrameRate(int f_fps)
	{
		frameRateInfo.setText(tr("%1 FPS").arg(f_fps));
	}

	void VideoRecordingDialog::updatePixelFormat(PixelFormat pixFmt)
	{
		std::string str = InterfaceFFMPEG::getPixFormatName(pixFmt);
		pixelFormatInfo.setText(str.c_str());
	}

	void VideoRecordingDialog::updateBitRate(int b_bitPerSec)
	{
		videoBitRateInfo.setText(tr("%1 kB/s").arg(b_bitPerSec/1024));
	}

	void VideoRecordingDialog::updateFrameCount(int c)
	{
		numFramesInfo.setText(tr("%1").arg(c));
	}

	void VideoRecordingDialog::updateDuration(float d_sec)
	{
		durationInfo.setText(tr("%1 second(s)").arg(static_cast<int>(d_sec)));
	}

	VideoRecordingOptionsDialog::VideoRecordingOptionsDialog(int defFrameRate, int defVideoBitRate_bitPerSec, PixelFormat defPixFmt, QWidget* parent)
	 : QDialog(parent),layout(this), pixelFormatLabel("Pixel format :"), frameRateLabel("Frame rate :"), videoBitRateLabel("Video bit rate (kB/s) :"), okButton("Ok"), cancelButton("Cancel")
	{
		pixelFormatLabel.setAlignment(Qt::AlignRight);
		frameRateLabel.setAlignment(Qt::AlignRight);
		videoBitRateLabel.setAlignment(Qt::AlignRight);

		pixelFormatBox.addItem("PIX_FMT_YUV410P", PIX_FMT_YUV410P);
		pixelFormatBox.addItem("PIX_FMT_YUV411P", PIX_FMT_YUV411P);
		pixelFormatBox.addItem("PIX_FMT_YUV420P", PIX_FMT_YUV420P);
		pixelFormatBox.addItem("PIX_FMT_YUV422P", PIX_FMT_YUV422P);
		pixelFormatBox.addItem("PIX_FMT_YUV440P", PIX_FMT_YUV440P);
		pixelFormatBox.addItem("PIX_FMT_YUV444P", PIX_FMT_YUV444P);

		frameRateBox.setRange(1,1000);
		videoBitRateBox.setRange(1,1024);

		frameRateBox.setValue(defFrameRate);
		videoBitRateBox.setValue(defVideoBitRate_bitPerSec/1024);

		switch(defPixFmt)
		{
			case PIX_FMT_YUV411P :	pixelFormatBox.setCurrentIndex(1); break;
			case PIX_FMT_YUV420P :	pixelFormatBox.setCurrentIndex(2); break;
			case PIX_FMT_YUV422P :	pixelFormatBox.setCurrentIndex(3); break;
			case PIX_FMT_YUV440P :	pixelFormatBox.setCurrentIndex(4); break;
			case PIX_FMT_YUV444P :	pixelFormatBox.setCurrentIndex(5); break;
			case PIX_FMT_YUV410P :
			default :		pixelFormatBox.setCurrentIndex(0); break;
		}

		layout.addWidget(&pixelFormatLabel,	0, 0); layout.addWidget(&pixelFormatBox, 	0, 1);
		layout.addWidget(&frameRateLabel,	1, 0); layout.addWidget(&frameRateBox, 		1, 1);
		layout.addWidget(&videoBitRateLabel,	2, 0); layout.addWidget(&videoBitRateBox, 	2, 1);
		layout.addWidget(&cancelButton, 	3, 0); layout.addWidget(&okButton, 		3, 1);

		QObject::connect(&okButton, 	SIGNAL(released(void)), this, SLOT(accept(void)));
		QObject::connect(&cancelButton, SIGNAL(released(void)), this, SLOT(reject(void)));
	}

	VideoRecordingOptionsDialog::~VideoRecordingOptionsDialog(void)
	{ }

	PixelFormat VideoRecordingOptionsDialog::getPixelFormat(void) const
	{
		return static_cast<PixelFormat>(pixelFormatBox.itemData(pixelFormatBox.currentIndex()).toInt());
	}

	int VideoRecordingOptionsDialog::getVideoBitRate(void) const
	{
		return videoBitRateBox.value()*1024;
	}

	int VideoRecordingOptionsDialog::getFrameRate(void) const
	{
		return frameRateBox.value();
	}

	VideoRecorderControls::VideoRecorderControls(QWidget* parent)
	 : QHBoxLayout(parent), startRecordingButton("Start recording..."), optionsButton("Options"), recorder(NULL), recordedPort(0), frameRate(30), bitRate_bitPerSec(400000), pixFmt(PIX_FMT_YUV420P)
	{
		addWidget(&startRecordingButton);
		addWidget(&optionsButton);

		QObject::connect(&startRecordingButton, SIGNAL(released(void)), 	this, SIGNAL(aboutToStartRecording(void)));
		QObject::connect(&optionsButton, 	SIGNAL(released(void)), 	this, SLOT(changeOptions(void)));
		QObject::connect(&recordingDialog,	SIGNAL(stopRecording(void)),	this, SLOT(stopRecording(void)));
	}

	VideoRecorderControls::~VideoRecorderControls(void)
	{
		stopRecording();
	}

	void VideoRecorderControls::changeOptions(void)
	{
		VideoRecordingOptionsDialog optionsDialog(frameRate, bitRate_bitPerSec, pixFmt, parentWidget());

		int result = optionsDialog.exec();

		if(result==QDialog::Accepted)
		{
			frameRate		= optionsDialog.getFrameRate();
			bitRate_bitPerSec	= optionsDialog.getVideoBitRate();
			pixFmt			= optionsDialog.getPixelFormat();
		}
	}

	void VideoRecorderControls::startRecording(unsigned int portID, const std::string& portName, const HdlAbstractTextureFormat& fmt)
	{
		if(!isRecording())
		{
			QString filename = QFileDialog::getSaveFileName(parentWidget());

			if (!filename.isEmpty())
			{
				try
				{
					// Create recorder :
					std::string filenameStd = filename.toStdString();
					recorder = new VideoRecorder(filenameStd, fmt, frameRate, bitRate_bitPerSec, pixFmt);

					QFileInfo pathInfo( filename );
					QString strippedFilename( pathInfo.fileName() );

					// Save info :
					recordedPort = portID;

					// Show the info box :
					recordingDialog.updateFilename(strippedFilename.toStdString());
					recordingDialog.updatePortName(portName);
					recordingDialog.updateFormat(fmt);
					recordingDialog.updateStatus(true);
					recordingDialog.updateFrameRate(frameRate);
					recordingDialog.updatePixelFormat(pixFmt);
					recordingDialog.updateBitRate(bitRate_bitPerSec);
					recordingDialog.updateFrameCount(0);
					recordingDialog.updateDuration(0.0f);
					recordingDialog.show();

				}
				catch(Exception& e)
				{
					QMessageBox::information(parentWidget(), "VideoRecorderControls::startRecording - Error : ", e.what());
				}
			}
		}
		else
			QMessageBox::information(parentWidget(), "VideoRecorderControls::startRecording", "A recording session is already in place.");
	}

	void VideoRecorderControls::stopRecording(void)
	{
		if(isRecording())
		{
			recordingDialog.hide();
			delete recorder;
			recorder = NULL;

			emit endRecording();
		}
	}

	bool VideoRecorderControls::isRecording(void) const
	{
		return recorder!=NULL;
	}

	unsigned int VideoRecorderControls::getRecordedPort(void) const
	{
		return recordedPort;
	}

	void VideoRecorderControls::submitNewFrame(HdlTexture& texture)
	{
		static bool lock = false;

		if(lock)
			return;

		lock = true;

		if(!isRecording())
		{
			lock = false;
			throw Exception("VideoRecorderControls::submitNewFrame - Not recording.", __FILE__, __LINE__);
		}
		else
		{
			try
			{
				(*recorder) << texture << OutputDevice::Process;

				recordingDialog.updateFrameCount(recorder->getNumEncodedFrames());
				recordingDialog.updateDuration(recorder->getTotalVideoDurationSec());
			}
			catch(Exception& e)
			{
				lock = false;
				throw e;
			}
		}

		lock = false;
	}
