#ifndef __TEST_FFMPEG__
#define __TEST_FFMPEG__

	#include <iostream>
	#include <fstream>
	#include "WindowRendering.hpp"
	#include "ResourceLoader.hpp"
	#include "VideoStream.hpp"
	#include "VideoControls.hpp"
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QTimer>

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;
		using namespace Glip::Modules;

	class TestFFMPEGInterface : public QWidget
	{
		Q_OBJECT

		private :
			std::fstream		log;

			QVBoxLayout		layout;
			PipelineLoaderInterface pipelineLoaderInterface;
			WindowRendererContainer	window;
			VideoControls		videoControls;
			VideoRecorderControls	videoRecorderControls;

		public :
			TestFFMPEGInterface(void);
			~TestFFMPEGInterface(void);

		private slots :
			void resetProcessTest(void);
			void process(void);
			void updateOutput(void);
			void aboutToStartRecording(void);
	};

	class TestFFMPEGApplication : public QApplication
	{
		Q_OBJECT

		private :
			TestFFMPEGInterface	*interface;

		public :
			TestFFMPEGApplication(int& argc, char** argv);
			~TestFFMPEGApplication(void);
	};

#endif
