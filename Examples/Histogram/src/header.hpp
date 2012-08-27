#ifndef __HEADER_LIFE__
#define __HEADER_LIFE__

	// Includes
	#include <iostream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QWidget>
	#include <QPushButton>
	#include <QVBoxLayout>
	#include <QFileDialog>
	#include <QMessageBox>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;

	class HistogramInterface : public QWidget
	{
		Q_OBJECT

		private :
			HdlTexture*	text;
			QVBoxLayout* 	layout;
			QPushButton	*chImg, *sav;
			WindowRenderer	*window;
			Pipeline	*pipeline;

		public :
			HistogramInterface(void);

		public slots :
			void loadImage(void);
			void save(void);
			void requestUpdate(void);
	};

	class HistogramApplication : public QApplication
	{
		Q_OBJECT

		private :
			HistogramInterface* ihm;

		public :
			HistogramApplication(int& argc, char** argv);
	};

#endif
