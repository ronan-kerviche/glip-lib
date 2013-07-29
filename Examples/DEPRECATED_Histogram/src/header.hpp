#ifndef __HEADER_LIFE__
#define __HEADER_LIFE__

	// Includes
	#include <iostream>
	#include <fstream>
	#include "WindowRendering.hpp"
	#include "RessourceLoader.hpp"
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
			std::fstream		log;
			QVBoxLayout		layout;
			QSlider			scaleSlider;
			QPushButton		saveButton;
			ImageLoaderInterface	imageLoaderInterface;
			WindowRendererContainer	window;
			Pipeline		*pipeline;

		public :
			HistogramInterface(void);
			~HistogramInterface(void);

		public slots :
			void process(void);
			void updateShow(void);
			void save(void);
	};

	class HistogramApplication : public QApplication
	{
		Q_OBJECT

		private :
			HistogramInterface* ihm;

		public :
			HistogramApplication(int& argc, char** argv);
			~HistogramApplication(void);
	};

#endif
