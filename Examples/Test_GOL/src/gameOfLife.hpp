#ifndef __GAME_OF_LIFE__
#define __GAME_OF_LIFE__

	#include <iostream>
	#include <fstream>
	#include "WindowRendering.hpp"
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QTimer>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;
		using namespace Glip::Modules;

	// Object
		class GameOfLife : public QApplication
		{
			Q_OBJECT

			private :
				// Data :
				int w, h;
				std::fstream	log;
				WindowRenderer	window;
				QTimer 		timer;
				ProceduralInput *inp;
				Pipeline	*p1,
						*p2;
				HdlTexture	*t,
						*target;

				// Functions :
				void randomTexture(float alpha);

			public :
				// Functions :
				GameOfLife(int _w, int _h, int argc, char** argv);
				~GameOfLife(void);

			private slots :
				void compute(void);
				void show(void);
		};

#endif
