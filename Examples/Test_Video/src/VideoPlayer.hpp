#ifndef __VIDEO_PLAYER__
#define __VIDEO_PLAYER__

	// gstreamer + ffmpeg!!!
	#include <iostream>
	#include <fstream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QWidget>
	#include <QPushButton>
	#include <QVBoxLayout>
	#include <QHBoxLayout>
	#include <QFileDialog>
	#include <QMessageBox>
	#include <QComboBox>
	#include <QSlider>
	#include <QLabel>
	#include <Phonon/BackendCapabilities>
	#include <Phonon/MediaSource>
	#include <Phonon/MediaObject>
	#include <Phonon/VideoPlayer>
	#include <Phonon/AudioOutput>
	#include <Phonon/VideoWidget>
	#include <QTimer>
	#include <QPixmap>
	#include <QImage>
	#include <QDialog>
	#include <QScrollArea>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;
		using namespace Glip::Modules;

	class VideoModIHM : public QWidget
	{
		Q_OBJECT

		private :
			std::fstream		log;
			QTimer 			*timer;
			Phonon::VideoPlayer 	*player;
			Phonon::MediaSource 	*src;
			WindowRenderer		*output;

			HdlTextureFormat 	*fmt;
			HdlPBO			*pbo;
			HdlTexture		*textureA,
						*textureB,
						*textureLatest,
						*textureOldest;
			Pipeline		*pipeline;

			QHBoxLayout		*videosLayout,
						*timeLayout,
						*streamControlsLayout,
						*pipelineControl;
			QVBoxLayout		*generalLayout;

			QPushButton		*loadVideo,
						*play,
						*pause,
						*loadPipeline,
						*saveFrame;

			QSlider			*timeSlider;
			QLabel			*timeLabel;
			QComboBox		*timeBox;

			QComboBox		*box;
			QPixmap			*pixmap;
			QImage			*image;
			unsigned char		*buffer;

		public :
			VideoModIHM(void);
			~VideoModIHM(void);

		public slots :
			void openVideo(void);
			void openPipeline(void);
			void grabFrame(void);
			void drawOutput(void);
			void save(void);
			void updateTime(void);
			void seekPosition(void);
			void changeFPS(void);

		signals :
			//void close(void);
	};

	class VideoModApplication : public QApplication
	{
		Q_OBJECT

		private :
			VideoModIHM		*ihm;

		public :
			VideoModApplication(int& argc, char** argv);
			~VideoModApplication();

		public slots :
			//void close(void);
	};

#endif
