#include "header.hpp"
#include <cmath>	
    
	// Code
	ApplicationWebcam::ApplicationWebcam(int id, FOURCC code) : OutputModule(2, "MainApplication"), enabled(false)
	{   	
		// Create the Camera
		std::cout << "Creating the camera..." << std::endl;
		const int camW = 640,
				  camH = 480;
		cam = new UnicapCamera(id,code,camW,camH);

		// Set the geometry :
		geo = HdlVBO::generate2DGrid(640, 480); 
		HdlTextureFormat fmt(256, 1, GL_RGB32F_ARB, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		ShaderSource srcPixel, srcVertex;
		srcPixel.loadSourceFile("./Filters/srcPixel.glsl");
		srcVertex.loadSourceFile("./Filters/srcVertex.glsl");

		histogram = new HdlPipeline("HistogramPipeline", 1, 1);
		histogram->addFilter(new HdlFilter("HistogramFilter", srcPixel, fmt, 1, 1, HdlFilter::MODE_HISTORY, &srcVertex));
		histogram->filter("HistogramFilter").setGeometry(geo);
		histogram->filter("HistogramFilter").program().modifyVar("tex0",HdlProgram::SHADER_VAR,0);
		histogram->filter("HistogramFilter").setBlendingStack(true);

		StreamManager::connect(histogram->in(0),  cam->out(0)); 
		StreamManager::connect(histogram->filter("HistogramFilter").in(0), histogram->in(0));
		StreamManager::connect(histogram->out(0), histogram->filter("HistogramFilter").out(0)); 
		
		// OverAll connections
		StreamManager::connect(out(0), cam->out(0)); 
		StreamManager::connect(out(1), histogram->out(0)); 

		// Start camera
		if(cam->isEnabled())
			cam->run();
		else
		{
			std::cout << "Can't use the camera" << std::endl;
			return ;
		}
 
		// Start QT loop
		std::cout << "Initializing QT loop" << std::endl;
		timer = new QTimer;   
        timer->setInterval(50);    
		connect(timer, SIGNAL(timeout()),this, SLOT(loop()));  
        timer->start();

		// Done!
		std::cout << "==================== Appli started ======================" << std::endl;
		enabled = true; 
	}

	ApplicationWebcam::~ApplicationWebcam(void)
	{
		timer->stop();
		delete cam;
		delete timer;
	}

	bool ApplicationWebcam::isEnabled(void)
	{
		return enabled;
	}

	void ApplicationWebcam::loop(void)
	{
		if(cam->isNewFrame())
		{
			histogram->process();
		}
	}

