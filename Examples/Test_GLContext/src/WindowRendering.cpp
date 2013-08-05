
// Includes
	#include "WindowRendering.hpp"

// Class
	WindowRenderer::WindowRenderer(int w, int h, double _fmtImg)
	 : QGLWidget(), geometry(NULL)
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
			//vbo = HdlVBO::generate2DGrid(50, 35, 1.0, 1.0, 0.0, 0.0);
			//vbo = HdlVBO::generate2DStandardQuad();
			geometry = new GeometryInstance( GeometryPrimitives::PointsGrid2D(50, 35), GL_STATIC_DRAW_ARB );
		}
		catch(std::exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while creating geometry", __FILE__, __LINE__);
			throw m+e;
		}

		// Prepare Texture
			glErrors(true, false);
			glEnable(GL_TEXTURE_2D);
			HdlTextureFormat fmt(2, 2, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
			texture = new HdlTexture(fmt);
			//texture->fill(0);
			unsigned char* t = new unsigned char[12];
			/* B&W
			t[0] = t[ 1] = t[ 2] = 255;
			t[3] = t[ 4] = t[ 5] = 0;
			t[6] = t[ 7] = t[ 8] = 0;
			t[9] = t[10] = t[11] = 255;
			*/
			t[0] 	= 255;
			t[1] 	= 0;
			t[2] 	= 0;
			t[3] 	= 0;
			t[4] 	= 255;
			t[5] 	= 0;
			t[6] 	= 0;
			t[7] 	= 0;
			t[8] 	= 255;
			t[9] 	= 255;
			t[10] 	= 255;
			t[11] 	= 0;
			texture->write((GLvoid*)t, GL_RGB, GL_UNSIGNED_BYTE);
			//texture->bind();
			delete[] t;
		// end TMP
		glErrors(true, false);

		show();

		timer = new QTimer;
		timer->setInterval(500);
			connect(timer, SIGNAL(timeout()),this, SLOT(draw()));
		timer->start();
	}

	WindowRenderer::~WindowRenderer(void)
	{
		this->hide();
		delete geometry;
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
		static unsigned int i=0;
		/*static GLuint texID=0;

		if(texID==0)
		{
			glEnable(GL_TEXTURE_2D);

			// create the texture
			glGenTextures(1, &texID);

			glBindTexture(GL_TEXTURE_2D, texID);
			std::cout << "Creation : "; glErrors(true,false);
			//glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
			//glPixelStorei(GL_UNPACK_LSB_FIRST,  0);
			//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			//glPixelStorei(GL_UNPACK_SKIP_ROWS,  0);
			//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			std::cout << "Alignement: "; glErrors(true,false);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glErrors(true,false);

			static unsigned char* t = new unsigned char[12];
			t[0] = t[ 1] = t[ 2] = 255;
			t[3] = t[ 4] = t[ 5] = 0;
			t[6] = t[ 7] = t[ 8] = 0;
			t[9] = t[10] = t[11] = 255;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)t);
			delete[] t;
			glErrors(true,false);
		}*/

		glLoadIdentity();
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture->bind();

		if(i%2==0)
		{
			glBegin(GL_QUADS);
				glVertex2f( -0.5, -0.5); glTexCoord2f(0, 0);
				glVertex2f( -0.5,  0.5); glTexCoord2f(0, 1);
				glVertex2f(  0.5,  0.5); glTexCoord2f(1, 1);
				glVertex2f(  0.5, -0.5); glTexCoord2f(1, 0);
			glEnd();
			std::cout << "Direct rendering : "; glErrors(true,false);
		}
		else
		{
			std::cout << "Before VBO : "; glErrors(true,false);
			geometry->draw();
		}

		swapBuffers();
		i++;
	}

