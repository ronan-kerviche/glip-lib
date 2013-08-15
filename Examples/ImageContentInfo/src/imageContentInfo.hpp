#ifndef __IMAGE_CONTENT_INFO__
#define __IMAGE_CONTENT_INFO__

	#include <iostream>
	#include <fstream>
	#include "WindowRendering.hpp"
	#include "ResourceLoader.hpp"
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QWidget>
	#include <QPushButton>
	#include <QVBoxLayout>
	#include <QFileDialog>
	#include <QMessageBox>
	#include <QComboBox>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;
		using namespace Glip::Modules;

	// Object
		class IHM : public QWidget
		{
			Q_OBJECT

			private :
				std::fstream		log;
				QVBoxLayout		layout;
				QPushButton		saveButton;
				ImageLoaderInterface	imageLoaderInterface;
				PipelineLoaderInterface pipelineLoaderInterface;
				WindowRendererContainer	window;

			public :
				IHM(void);
				~IHM(void);

			public slots :
				void save(void);
				void requestComputingUpdate(void);
				void updateOutput(void);
		};

		class ImageContentInformation : public QApplication
		{
			Q_OBJECT

			private :
				IHM	*ihm;

			public :
				ImageContentInformation(int& argc, char** argv);
				~ImageContentInformation(void);
		};

#endif
