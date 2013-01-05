#ifndef __GAME_OF_LIFE__
#define __GAME_OF_LIFE__

	#include <iostream>
	#include <fstream>
	#include "WindowRendering.hpp"
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QTimer>

	//#define __RADOM_GEN__

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

				#ifdef __RADOM_GEN__
					HdlTexture	*tInput;
				#else
					ProceduralInput *inp;
				#endif

				Pipeline	*p1,
						*p2;
				HdlTexture	*target;

				// Functions :
				#ifdef __RADOM_GEN__
					void randomTexture(float alpha);
				#endif

			public :
				// Functions :
				GameOfLife(int _w, int _h, int argc, char** argv);
				~GameOfLife(void);

			private slots :
				void compute(void);
				void show(void);
		};

#endif
