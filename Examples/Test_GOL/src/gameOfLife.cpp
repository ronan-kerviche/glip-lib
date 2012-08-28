#include "gameOfLife.hpp"
#include "WindowRendering.hpp"
#include <cstdlib>
#include <ctime>
#include <QMessageBox>

// Src :
	GameOfLife::GameOfLife(int _w, int _h, int argc, char** argv)
	 : QApplication(argc,argv), w(_w), h(_h), window(NULL), timer(NULL), inp(NULL), p1(NULL), p2(NULL), t(NULL)
	{
		log.open("./log.txt", std::fstream::out | std::fstream::trunc);

		if(!log.is_open())
		{
			QMessageBox::warning(NULL, tr("TestGOL"), tr("Unable to write to the log file log.txt.\n"));
			throw Exception("GameOfLife::GameOfLife - Cannot open log file.", __FILE__, __LINE__);
		}

		try
		{
			window = new WindowRenderer(w, h);

			// Info :
			log << "> Test_GOL" << std::endl;
			log << "Vendor name   : " << HandleOpenGL::getVendorName() << std::endl;
			log << "Renderer name : " << HandleOpenGL::getRendererName() << std::endl;
			log << "GL version    : " << HandleOpenGL::getVersion() << std::endl;
			log << "GLSL version  : " << HandleOpenGL::getGLSLVersion() << std::endl;

			HdlTextureFormat fmt(w, h, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);

			// Create the pipeline :
				#define __LOAD_FROM_FILE__
				#ifndef __LOAD_FROM_FILE__
					ShaderSource src("./Filters/game.glsl");

					FilterLayout fl("GameOfLife_Layout", fmt, src);

					PipelineLayout pl("MainPipeline_2Steps");
					pl.addInput("Input");
					pl.addOutput("Output");
					pl.add(fl, "GameOfLife_1");
					pl.add(fl, "GameOfLife_2");

					pl.connectToInput("Input", "GameOfLife_1", "inText");
					pl.connect("GameOfLife_1", "outText", "GameOfLife_2", "inText");
					pl.connectToOutput("GameOfLife_2", "outText", "Output");

					p1 = new Pipeline(pl, "Ping");
					p2 = new Pipeline(pl, "Pong");
				#else
					// Load the same settings but from a file :
					LayoutLoader loader;
					PipelineLayout* model = loader("./Filters/pipelineGOL.ppl");
					p1 = new Pipeline(*model, "Ping");
					p2 = new Pipeline(*model, "Pong");
					delete model;
				#endif

			// Create a random starting point :
				fmt.setSWrapping(GL_REPEAT); // pac-man repetition
				fmt.setTWrapping(GL_REPEAT);
				t = new HdlTexture(fmt);

				randomTexture(0.3);
				(*window) << (*t);

			// Create another weird random (mathematical) point :
			ShaderSource inputSrc("./Filters/gameInput.glsl");
			inp 	= new ProceduralInput("InputPattern", fmt, inputSrc);
			inp->prgm().modifyVar("t",HdlProgram::Var,2);
			inp->generateNewFrame();

			// Do the first pass :
				(*p1) << (*t) << Pipeline::Process;
				(*p2) << (*t) << Pipeline::Process;

			timer = new QTimer;
			timer->setInterval(50);
			connect(timer, SIGNAL(timeout()),this, SLOT(compute()));
			timer->start();
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			QMessageBox::warning(NULL, tr("TestGOL"), e.what());
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			throw e;
		}
	}

	GameOfLife::~GameOfLife(void)
	{
		timer->stop();

		log << "> End" << std::endl;
		log.close();

		delete timer;
		delete inp;
		delete p1;
		delete p2;
		delete t;
		delete window;
	}

	void GameOfLife::randomTexture(float alpha)
	{
		srand(time(NULL));
		unsigned char *tmp = new unsigned char [3*w*h];
		for(int i=0; i<3*w*h; i++)
		{
			float a = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
			tmp[i] = 255*(a<alpha);
		}

		t->write(tmp);
		delete[] tmp;
	}

	void GameOfLife::compute(void)
	{
		static int i = 0;
		static int j = 3;

		try
		{
			if(i==100)
			{
				//throw Exception("Stop");
				std::cout << "> Reset" << std::endl;
				//reset :
				#define __RADOM_GEN__
				#ifdef __RADOM_GEN__
					randomTexture(0.3);
					(*p1) << (*t) << Pipeline::Process;
					(*p2) << (*t) << Pipeline::Process;
				#else
					inp->prgm().modifyVar("t",HdlProgram::SHADER_VAR,j);
					(*p1) << inp->texture() << Pipeline::Process;
					(*p2) << inp->texture() << Pipeline::Process;
					j++;
				#endif

				i = 0;
			}
			else
			{
				if(i%2==0)
				{
					(*p2) << p1->out(0) << Pipeline::Process;
					(*window) << p2->out(0);
				}
				else
				{
					(*p1) << p2->out(0) << Pipeline::Process;
					(*window) <<  p1->out(0);
				}
			}

			i++;
		}
		catch(Exception& e)
		{
			log << "Exception caught : " << std::endl;
			log << e.what() << std::endl;
			log << "> Abort" << std::endl;
			log.close();
			std::cout << "Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			std::cout << "(Will be rethrown)" << std::endl;
			throw e;
		}
	}
