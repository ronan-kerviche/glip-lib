#ifndef __GAME_OF_LIFE__
#define __GAME_OF_LIFE__

	#include <iostream>
	#include <fstream>
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
				std::fstream	log;
				bool 		computingSuccess;
				HdlTexture	*text;
				QVBoxLayout	*layout;
				QPushButton	*chImg, *chPpl, *sav;
				WindowRenderer	*window;
				Pipeline	*pipeline;
				QComboBox	*box;

			public :
				IHM(void);
				~IHM(void);

			public slots :
				void loadImage(void);
				void loadPipeline(void);
				void save(void);
				void requestComputingUpdate(void);
				void updateOutput(void);
		};

		class ImageContentInformation : public QApplication
		{
			Q_OBJECT

			private :
				int w, h;
				IHM	*ihm;

			public :
				ImageContentInformation(int _w, int _h, int& argc, char** argv);
				~ImageContentInformation(void);
		};

#endif
