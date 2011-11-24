
// Includes
	#include "WindowRendering.hpp"

// Class
	WindowRenderer::WindowRenderer(int w, int h, double _fmtImg) : QGLWidget(), InputModule(1, "QTWidgetRenderer")
	{
		QWidget::setGeometry(10,10,w,h);

		if(_fmtImg==-1)
			fmtImg = static_cast<double>(w)/static_cast<double>(h);
		else
			fmtImg = _fmtImg;

		makeCurrent();
        glViewport(0, 0, w, h);

		show();

		timer = new QTimer;
        timer->setInterval(50);
		connect(timer, SIGNAL(timeout()),this, SLOT(draw()));
        timer->start();
	}

	WindowRenderer::~WindowRenderer(void)
	{
		this->hide();
	}

	void WindowRenderer::resizeGL(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	bool WindowRenderer::process(void)
	{	
		return true; 
	}

	void WindowRenderer::draw(void)
	{	 
		if(!allConnected()) 
		{
			std::cout << "WindowRenderer::draw - Not connected!" << std::endl; 
			return ; 
		}
  
		glLoadIdentity(); 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

		bindAll();

		#ifdef DISP_MIROR
			drawGeometry(1.0, 1.0, -1.0, -1.0);
		#else
			drawGeometry(-1.0, 1.0, 1.0, -1.0);
		#endif

		unbindAll();

        swapBuffers();
	}

