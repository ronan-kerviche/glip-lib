#ifndef __GAME_OF_LIFE__
#define __GAME_OF_LIFE__

	#include <iostream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QTimer>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;

	// Object
		class GameOfLife : public QApplication
		{
			Q_OBJECT

			private :
				// Data :
				int w, h;
				WindowRenderer	*window;
				QTimer 		*timer;
				Pipeline	*p1, *p2;
				HdlTexture	*t;

				// Functions :
				void randomTexture(float alpha);

			public :
				// Functions :
				GameOfLife(int _w, int _h, int argc, char** argv);

			private slots :
				void compute(void);
		};

#endif
