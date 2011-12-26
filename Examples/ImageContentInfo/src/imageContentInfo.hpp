#ifndef __GAME_OF_LIFE__
#define __GAME_OF_LIFE__

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

	// Object
		class Interface : public QWidget
		{
			Q_OBJECT

			private :
				HdlTexture*	text;
				QVBoxLayout* 	layout;
				QPushButton	*chImg, *chPpl;
				WindowRenderer	*window;
				Pipeline	*pipeline;

			public :
				Interface(void);

			private slots :
				void loadImage(void);
				void loadPipeline(void);
				void requestUpdate(void);
		};

		class ImageContentInformation : public QApplication
		{
			Q_OBJECT

			private :
				int w, h;
				Interface	*interface;

			public :
				ImageContentInformation(int _w, int _h, int argc, char** argv);
		};

#endif
