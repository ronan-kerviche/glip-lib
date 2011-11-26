
// Includes
	#include "WindowRendering.hpp"

// Class
	WindowRenderer::WindowRenderer(int w, int h, double _fmtImg) : QGLWidget()
	{
		QWidget::setGeometry(10,10,w,h);

		if(_fmtImg==-1)
			fmtImg = static_cast<double>(w)/static_cast<double>(h);
		else
			fmtImg = _fmtImg;

		makeCurrent();
		HandleOpenGL::init();

		glViewport(0, 0, w, h);

		try
		{
			//vbo = HdlVBO::generate2DStandardQuad();
		}
		catch(std::exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while creating geometry", __FILE__, __LINE__);
			throw m+e;
		}

		// Prepare Texture
			HdlTextureFormat fmt(2, 2, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
			texture = new HdlTexture(fmt);
			unsigned char* t = new unsigned char[12];
			t[0] = t[ 1] = t[ 2] = 255;
			t[3] = t[ 4] = t[ 5] = 255;
			t[6] = t[ 7] = t[ 8] = 255;
			t[9] = t[10] = t[11] = 255;
			texture->write((GLvoid*)t);
			delete[] t;
		// end TMP

		show();

		timer = new QTimer;
		timer->setInterval(500);
			connect(timer, SIGNAL(timeout()),this, SLOT(draw()));
		timer->start();
	}

	WindowRenderer::~WindowRenderer(void)
	{
		this->hide();
		delete vbo;
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
		glLoadIdentity();
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture->bind();
		glBegin(GL_QUADS);
			glColor3ub(255, 255, 255);
			glVertex2f( 0.5,  0.5); glTexCoord2f(1.0, 1.0);
                        glVertex2f( 0.5, -0.5); glTexCoord2f(0.1, 0.0);
                        glVertex2f(-0.5, -0.5); glTexCoord2f(0.0, 0.0);
                        glVertex2f(-0.5,  0.5); glTexCoord2f(0.0, 1.0);
                glEnd();

                glDebug();

		/*std::cout << "Rendering..." << std::endl;
		try
		{
			//
			//vbo->draw();
		}
		catch(std::exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while rendering", __FILE__, __LINE__);
			throw m+e;
		}
		catch(...)
		{
			throw Exception("WindowRenderer::draw - Unknown exception received", __FILE__, __LINE__);
		}
		std::cout << "...end rendering" << std::endl << std::endl;*/

		swapBuffers();
	}

