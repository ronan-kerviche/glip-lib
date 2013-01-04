/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : VideoControls.hpp                                                                         */
/*     Original Date : December 31th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for loading and playing videos with FFMPEG library.                          */
/*                     Require RessourceLoader.*pp and VideoStream.*pp.					         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_VIDEO_CONTROLS__
#define __GLIPLIB_VIDEO_CONTROLS__

	#include "VideoStream.hpp"
	#include "VideoRecorder.hpp"
	#include "RessourceLoader.hpp"
	#include <QTimer>
	#include <QSlider>

	class VideoOptionsDialog : public QDialog
	{
		Q_OBJECT

		private :
			QPushButton	okButton,
					cancelButton;
			QLabel		numFrameBufferedLabel;
			QSpinBox	numFrameBufferedSpinBox;

		public :
			ImageLoaderOptions options;

			VideoOptionsDialog(QWidget* parent=NULL, GLenum defMinFilter=GL_NEAREST, GLenum defMagFilter=GL_NEAREST, GLenum defSWrapping=GL_CLAMP, GLenum defTWrapping=GL_CLAMP, int defMaxLevel=0, int _numFrameBuffered=1);
			~VideoOptionsDialog(void);

			int getNumFrameBuffered(void) const;
	};

	class VideoControls : public QVBoxLayout
	{
		Q_OBJECT

		private :
			QHBoxLayout 	layout1,
					layout2,
					layout3;
			QLabel		timeLabel,
					frameRateLabel;
			QPushButton	playButton,
					stopButton,
					loadVideoButton,
					optionsButton,
					nextFrameButton;
			QSlider		timeSlider;
			QLineEdit	infoLine;
			QSpinBox	frameRateSpinBox;
			QTimer		timer;

			// Video parameters for loading stage :
			GLenum 		minFilter,
					magFilter,
					sWrapping,
					tWrapping;
			int 		maxMipmapLevel,
					numFrameBuffered;

			// Player data :
			bool		isPlaying;
			VideoStream	*stream;

		private slots :
			void		timerTick(void);
			void		updateFrame(void);
			void		updateInfoTool(const QString& filename="");
			void		updateLoadToolTip(void);
			void 		changeFrameRate(void);
			void		seek(void);

		public :
			VideoControls(QWidget* parent = NULL);
			~VideoControls(void);

			bool		videoStreamIsValid(void) const;
			VideoStream& 	videoStream(void);

		public slots :
			void 		loadVideoFile(void);
			void 		changeOptions(void);
			void 		play(void);
			void 		pause(void);
			void 		stop(void);
			void 		togglePlayPause(void);
			void		oneFrameForward(void);

		signals :
			void		newVideoLoaded(void);
			void 		frameUpdated(void);
	};

	class VideoRecordingDialog : public QWidget
	{
		Q_OBJECT

		private :
			QGridLayout	layout;
			QLabel		recordingStateLabel,
					filenameLabel,
					portNameLabel,
					formatLabel1,
					formatLabel2,
					formatLabel3,
					numFramesLabel,
					durationLabel,
					pixelFormatLabel,
					frameRateLabel,
					videoBitRateLabel;
			QLineEdit	recordingStateInfo,
					filenameInfo,
					portNameInfo,
					formatInfo1,
					formatInfo2,
					formatInfo3,
					numFramesInfo,
					durationInfo,
					pixelFormatInfo,
					frameRateInfo,
					videoBitRateInfo;
			QPushButton	finishRecording;

		public :
			VideoRecordingDialog(QWidget* parent=NULL);
			~VideoRecordingDialog(void);

			void updateStatus(bool isRecording);
			void updateFilename(const std::string& filename);
			void updatePortName(const std::string& name);
			void updateFormat(const __ReadOnly_HdlTextureFormat& fmt);
			void updateFrameRate(int f_fps);
			void updatePixelFormat(PixelFormat pixFmt);
			void updateBitRate(int b_bitPerSec);
			void updateFrameCount(int c);
			void updateDuration(float d_sec);

		signals :
			void stopRecording(void);
	};

	class VideoRecordingOptionsDialog : public QDialog
	{
		Q_OBJECT

		private :
			QGridLayout	layout;
			QLabel		pixelFormatLabel,
					frameRateLabel,
					videoBitRateLabel;
			QComboBox	pixelFormatBox;
			QSpinBox	frameRateBox,
					videoBitRateBox;
			QPushButton	okButton,
					cancelButton;

		public :
			VideoRecordingOptionsDialog(int defFrameRate, int defVideoBitRate_bitPerSec, PixelFormat defPixFmt, QWidget* parent=NULL);
			~VideoRecordingOptionsDialog(void);

			PixelFormat getPixelFormat(void) const;
			int getVideoBitRate(void) const;
			int getFrameRate(void) const;
	};

	class VideoRecorderControls : public QHBoxLayout
	{
		Q_OBJECT

		private :
			// Current parameters :
			int		frameRate,
					bitRate_bitPerSec,
					recordedPort;
			PixelFormat	pixFmt;

			QPushButton	startRecordingButton,
					optionsButton;

			VideoRecorder	*recorder;

			VideoRecordingDialog recordingDialog;

		private slots :
			void changeOptions(void);

		public :
			VideoRecorderControls(QWidget* parent=NULL);
			~VideoRecorderControls(void);

			bool isRecording(void) const;
			unsigned int getRecordedPort(void) const;
			void submitNewFrame(HdlTexture& texture);

		public slots :
			void startRecording(unsigned int portID, const std::string& portName, const __ReadOnly_HdlTextureFormat& fmt);
			void stopRecording(void);

		signals :
			void aboutToStartRecording(void);
			void endRecording(void);
	};

#endif
