/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : WindowRendering.cpp                                                                       */
/*     Original Date : September 1st 2011                                                                        */
/*                                                                                                               */
/*     Description   : Qt Widget for rendering textures                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Includes
	#include "WindowRendering.hpp"

// Class
	WindowRenderer::WindowRenderer(QWidget* parent, int w, int h, double _fmtImg)
	 : QGLWidget(parent), OutputDevice("Display"), xFlip(false), yFlip(false)
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
			vbo = HdlVBO::generate2DStandardQuad();
		}
		catch(Exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while creating geometry", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while creating geometry", __FILE__, __LINE__);
			throw m+e;
		}

		show();
	}

	WindowRenderer::~WindowRenderer(void)
	{
		this->hide();
		delete vbo;
	}

	void WindowRenderer::resizeGL(int width, int height)
	{
		glViewport(0, 0, width, height);
		emit resized();
	}

	void WindowRenderer::process(HdlTexture& t)
	{
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		if(xFlip)	glScalef(-1.0f,  1.0f, 1.0f);
		if(yFlip)	glScalef( 1.0f, -1.0f, 1.0f);

		t.bind();
		vbo->draw();
		swapBuffers();
	}

