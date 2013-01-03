/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : WindowRendering.cpp                                                                       */
/*     Original Date : September 1st 2011                                                                        */
/*                                                                                                               */
/*     Description   : Qt Widget for rendering textures.                                                         */
/*                     (see WindowRendering.hpp for more information)                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

// Includes
	#include "WindowRendering.hpp"

// Class
	WindowRenderer::WindowRenderer(QWidget* _parent, int w, int h)
	 : QGLWidget(_parent), parent(_parent), vbo(NULL), OutputDevice("Display"), mouseMovementsEnabled(false), keyboardMovementsEnabled(false),
	   doubleLeftClick(false), doubleRightClick(false), leftClick(false), rightClick(false), mouseWheelTurned(false), wheelSteps(0), deltaX(0), deltaY(0), lastPosX(-1), lastPosY(-1),
	   fullscreenModeEnabled(false), currentCenterX(0.0f), currentCenterY(0.0f), currentRotationDegrees(0.0f), currentRotationCos(1.0f), currentRotationSin(0.0f), currentScale(1.0f),
	   currentStepRotationDegrees(180.0f), currentStepScale(1.1f), keyPressIncr(0.04f),
	   currentPixelAspectRatio(1.0f), currentImageAspectRatio(1.0f), currentWindowAspectRatio(1.0f),
	   clearColorRed(0.1f), clearColorGreen(0.1f), clearColorBlue(0.1f)
	{
		QWidget::setGeometry(10,10,w,h);

		makeCurrent();
		HandleOpenGL::init();

		glViewport(0, 0, w, h);

		try
		{
			vbo = HdlVBO::generate2DStandardQuad();
		}
		catch(std::exception& e)
		{
			Exception m("WindowRenderer::WindowRenderer - Error while creating geometry", __FILE__, __LINE__);
			throw m+e;
		}

		// Init keys :
		setKeyForAction(KeyUp,				Qt::Key_Up);
		setKeyForAction(KeyDown,			Qt::Key_Down);
		setKeyForAction(KeyLeft,			Qt::Key_Left);
		setKeyForAction(KeyRight,			Qt::Key_Right);
		setKeyForAction(KeyZoomIn,			Qt::Key_Plus);
		setKeyForAction(KeyZoomOut,			Qt::Key_Minus);
		setKeyForAction(KeyRotationClockWise,		Qt::Key_F);
		setKeyForAction(KeyRotationCounterClockWise,	Qt::Key_D);
		setKeyForAction(KeyToggleFullscreen,		Qt::Key_Return);
		setKeyForAction(KeyExitOnlyFullscreen,		Qt::Key_Escape);
		setKeyForAction(KeyResetView,			Qt::Key_Space);

		for(int i=0; i<NumActionKey; i++)
		{
			keyPressed[i]		= false;
			keyJustPressed[i]	= false;
			keyJustReleased[i]	= false;
		}

		//show();
	}

	WindowRenderer::~WindowRenderer(void)
	{
		this->hide();
		delete vbo;
		vbo = NULL;
	}

	void WindowRenderer::initializeGL(void)
	{
		currentWindowAspectRatio = static_cast<float>(width())/static_cast<float>(height());
		glViewport(0, 0, width(), height());
	}

	WindowRenderer::KeyAction WindowRenderer::corresponding(const Qt::Key& k) const
	{
		for(int i=0; i<NumActionKey; i++)
			if(keyAssociation[i]==k)
				return static_cast<WindowRenderer::KeyAction>(i);

		// else
		return NoAction;
	}

	void WindowRenderer::updateActions(void)
	{
		// From mouse :
		if(mouseMovementsEnabled)
		{
			// Translation?
			if(leftClick)
			{
				// Method 2 :
				float 	dx 		= 2.0f * deltaX / static_cast<float>(width()),
					dy 		= 2.0f * deltaY / static_cast<float>(height());

				translation(dx, dy);

				// Reset mouse translation :
				deltaX 			= 0;
				deltaY 			= 0;
			}
			else if(rightClick) // Rotation?
			{
				rotation(deltaX / static_cast<float>(width()));

				// Reset mouse translation :
				deltaX 			= 0;
				deltaY 			= 0;
			}

			// Scaling :
			if(mouseWheelTurned)
			{
				currentScale 		= std::pow(currentStepScale,wheelSteps);
				mouseWheelTurned	= false;
			}

			// Toggle fullscreen :
			if(doubleLeftClick)
			{
				toggleFullscreenMode();
				doubleLeftClick 	= false;
			}

			if(doubleRightClick)
			{
				resetTransformation();
				doubleRightClick	= false;
			}
		}
		else
		{
			#ifdef __VERBOSE__
				std::cout << "WindowRenderer::updateActions - Widget is not taking mouse input into account." << std::endl;
			#endif
		}

		// From keyboard :
		if(keyboardMovementsEnabled)
		{
			// Translation?
			float 	dx = 0.0f,
				dy = 0.0f;

			if(pressed(KeyUp))	dy -= keyPressIncr;
			if(pressed(KeyDown))	dy += keyPressIncr;
			if(pressed(KeyLeft))	dx -= keyPressIncr;
			if(pressed(KeyRight))	dx += keyPressIncr;

			translation(dx, dy);

			// Rotation?
			if(pressed(KeyRotationClockWise))
				rotation(keyPressIncr);

			if(pressed(KeyRotationCounterClockWise))
				rotation(-keyPressIncr);

			// Scaling?
			if(pressed(KeyZoomIn))				wheelSteps++;
			if(pressed(KeyZoomOut))				wheelSteps--;

			currentScale = std::pow(currentStepScale,wheelSteps);

			// Toggle fullscreen :
			if(justPressed(KeyToggleFullscreen))		toggleFullscreenMode();
			if(justPressed(KeyExitOnlyFullscreen))		setFullscreenMode(false);

			// Reset view :
			if(justPressed(KeyResetView))			resetTransformation();

		}
		else
		{
			#ifdef __VERBOSE__
				std::cout << "WindowRenderer::updateActions - Widget is not taking keyboard input into account." << std::endl;
			#endif
		}
	}

	bool WindowRenderer::pressed(const KeyAction& a) const
	{
		return keyPressed[static_cast<int>(a)];
	}

	bool WindowRenderer::justPressed(const WindowRenderer::KeyAction& a)
	{
		if(keyJustPressed[static_cast<int>(a)])
		{
			keyJustPressed[static_cast<int>(a)] = false;
			return true;
		}
		else
			return false;
	}

	bool WindowRenderer::justReleased(const KeyAction& a)
	{
		if(keyJustReleased[static_cast<int>(a)])
		{
			keyJustReleased[static_cast<int>(a)] = false;
			return true;
		}
		else
			return false;
	}

	void WindowRenderer::keyPressEvent(QKeyEvent* event)
	{
		if(!event->isAutoRepeat())
		{
			WindowRenderer::KeyAction a = corresponding(static_cast<Qt::Key>(event->key()));

			#ifdef __VERBOSE__
				std::cout << "WindowRenderer::keyPressEvent - Event : Key pressed" << std::endl;
			#endif

			if(a!=NoAction)
			{
				keyPressed[static_cast<int>(a)] = true;
				keyJustPressed[static_cast<int>(a)] = true;
				keyJustReleased[static_cast<int>(a)] = false;

				emit actionReceived();
			}
			else
				std::cerr << "WindowRenderer::keyPressEvent - Warning : Key not associated" << std::endl;
		}
		else
		{
			WindowRenderer::KeyAction a = corresponding(static_cast<Qt::Key>(event->key()));

			if(a!=NoAction)
				emit actionReceived();
		}
	}

	void WindowRenderer::keyReleaseEvent(QKeyEvent* event)
	{
		if(!event->isAutoRepeat())
		{
			WindowRenderer::KeyAction a = corresponding(static_cast<Qt::Key>(event->key()));

			#ifdef __VERBOSE__
				std::cout << "WindowRenderer::keyReleaseEvent - Event : Key released" << std::endl;
			#endif

			if(a!=NoAction)
			{
				keyPressed[static_cast<int>(a)] = false;
				keyJustPressed[static_cast<int>(a)] = false;
				keyJustReleased[static_cast<int>(a)] = true;

				emit actionReceived();
			}
			else
				std::cerr << "WindowRenderer::keyReleaseEvent - Warning : Key not associated" << std::endl;
		}
	}

	void WindowRenderer::mouseMoveEvent(QMouseEvent* event)
	{
		#ifdef __VERBOSE__
			std::cout <<"WindowRenderer::mouseMoveEvent - Mouse move event." << std::endl;
		#endif

		if(lastPosX==-1) lastPosX = event->x();
		if(lastPosY==-1) lastPosY = event->y();

		deltaX += event->x()-lastPosX,
		deltaY += event->y()-lastPosY;

		lastPosX = event->x();
		lastPosY = event->y();

		emit actionReceived();
	}

	void WindowRenderer::wheelEvent(QWheelEvent *event)
	{
		#ifdef __VERBOSE__
			std::cout <<"WindowRenderer::wheelEvent - Wheel event." << std::endl;
		#endif

		if(event->delta()!=0)
			wheelSteps += event->delta()/(8*15);

		mouseWheelTurned = true;

		emit actionReceived();

		// wheelSteps 	> 0 : away of the user
		// 		< 0 : toward the user
	}

	void WindowRenderer::mousePressEvent(QMouseEvent *event)
	{
		#ifdef __VERBOSE__
			std::cout << "WindowRenderer::mousePressEvent - Click event." << std::endl;
		#endif

		if(event->buttons() & Qt::LeftButton)
			leftClick = true;
		else if(event->buttons() & Qt::RightButton)
			rightClick = true;

		lastPosX = -1;
		lastPosY = -1;

		emit actionReceived();
	}

	void WindowRenderer::mouseReleaseEvent(QMouseEvent *event)
	{
		#ifdef __VERBOSE__
			std::cout << "WindowRenderer::mouseReleaseEvent - End click event." << std::endl;
		#endif

		if(!(event->buttons() & Qt::LeftButton))
			leftClick = false;
		else if(!(event->buttons() & Qt::RightButton))
			rightClick = false;

		lastPosX = -1;
		lastPosY = -1;

		emit actionReceived();
	}

	void WindowRenderer::mouseDoubleClickEvent(QMouseEvent * event)
	{
		#ifdef __VERBOSE__
			std::cout <<"WindowRenderer::mouseDoubleClickEvent - Double click event." << std::endl;
		#endif

		if(event->buttons() & Qt::LeftButton)
			doubleLeftClick = true;
		else if(event->buttons() & Qt::RightButton)
			doubleRightClick = true;

		emit actionReceived();
	}

	void WindowRenderer::resizeGL(int width, int height)
	{
		#ifdef __VERBOSE__
			std::cout <<"WindowRenderer::resizeGL - Resizing to " << width << "x" << height << std::endl;
		#endif

		glViewport(0, 0, width, height);

		currentWindowAspectRatio = static_cast<float>(QWidget::width())/static_cast<float>(QWidget::height());

		emit resized();
	}

	void WindowRenderer::setPixelAspectRatio(float ratio)
	{
		currentPixelAspectRatio = ratio;
	}

	void WindowRenderer::setImageAspectRatio(float ratio)
	{
		currentImageAspectRatio = ratio;
	}

	void WindowRenderer::setClearColor(float red, float green, float blue)
	{
		clearColorRed	= red;
		clearColorGreen	= green;
		clearColorBlue	= blue;
	}

	void WindowRenderer::clearWindow(bool swapNow)
	{
		// Clear screen :
		glClearColor( clearColorRed, clearColorGreen, clearColorBlue, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		if(swapNow)
			swapBuffers();
	}

	void WindowRenderer::process(HdlTexture& t)
	{
		// Check keys :
		updateActions();

		clearWindow(false); // false = don't swap now

		float 	scaleForCurrentWindowAspectRatioX,
			scaleForCurrentWindowAspectRatioY,
			scaleForCurrentSurfaceAspectRatioX,
			scaleForCurrentSurfaceAspectRatioY,
			scaleFitting;

		getScalingCoefficients(scaleForCurrentWindowAspectRatioX, scaleForCurrentWindowAspectRatioY,scaleForCurrentSurfaceAspectRatioX,scaleForCurrentSurfaceAspectRatioY, scaleFitting);

		glScalef( currentScale*scaleForCurrentWindowAspectRatioX, currentScale*scaleForCurrentWindowAspectRatioY, 1.0f);

		glRotatef( -currentRotationDegrees, 0.0f, 0.0f, 1.0f);

		glTranslatef( currentCenterX, -currentCenterY, 0.0);

		glScalef( scaleForCurrentSurfaceAspectRatioX/scaleFitting, -scaleForCurrentSurfaceAspectRatioY/scaleFitting, 1.0f);

		// Bind texture to surface :
		t.bind();

		// Draw surface :
		vbo->draw();

		HdlTexture::unbind();

		// Show axis :
		/*glBegin(GL_LINES);
			glColor3f(1.0f,1.0f,0.0f);
			glVertex2f(0.0f,0.0f);
			glVertex2f(1.0f,0.0f);
			glColor3f(1.0f,0.0f,1.0f);
			glVertex2f(0.0f,0.0f);
			glVertex2f(0.0f,1.0f);
		glEnd();*/

		// Done!
		swapBuffers();
	}

	bool WindowRenderer::isKeyboardActionsEnabled(void) const
	{
		return keyboardMovementsEnabled;
	}

	void WindowRenderer::setKeyboardActions(bool enabled)
	{
		keyboardMovementsEnabled = enabled;

		// Activate focus via mouse click :
		if(keyboardMovementsEnabled)
			setFocusPolicy(Qt::ClickFocus);
		else
			setFocusPolicy(Qt::NoFocus);
	}

	bool WindowRenderer::isMouseActionsEnabled(void) const
	{
		return mouseMovementsEnabled;
	}

	void WindowRenderer::setMouseActions(bool enabled)
	{
		mouseMovementsEnabled = enabled;
	}

	void WindowRenderer::setKeyForAction(const WindowRenderer::KeyAction& action, const Qt::Key& key)
	{
		if(action<0 || action>=NumActionKey)
			throw Exception("WindowRenderer::setKeyForAction - Action id = " + to_string(static_cast<int>(action)) + " is out of bounds.", __FILE__, __LINE__);
		else
			keyAssociation[static_cast<int>(action)] = key;
	}

	void WindowRenderer::removeKeyForAction(const WindowRenderer::KeyAction& action)
	{
		setKeyForAction(action, static_cast<Qt::Key>(0));
	}

	void WindowRenderer::getScalingCoefficients(float& scaleForCurrentWindowAspectRatioX, float& scaleForCurrentWindowAspectRatioY, float& scaleForCurrentSurfaceAspectRatioX, float& scaleForCurrentSurfaceAspectRatioY, float& scaleFitting)
	{
		float surfaceAspectRatio 		= currentImageAspectRatio*currentPixelAspectRatio;
		scaleForCurrentWindowAspectRatioX	= (currentWindowAspectRatio>1.0f) ? (1.0f/currentWindowAspectRatio) : (1.0f);
		scaleForCurrentWindowAspectRatioY	= (currentWindowAspectRatio>1.0f) ? (1.0f) : (currentWindowAspectRatio);
		scaleForCurrentSurfaceAspectRatioX	= (surfaceAspectRatio>1.0f) ? (1.0f) : (surfaceAspectRatio);
		scaleForCurrentSurfaceAspectRatioY	= (surfaceAspectRatio>1.0f) ? (1.0f/surfaceAspectRatio) : (1.0f);

		if( (currentWindowAspectRatio>=1.0f && surfaceAspectRatio<=1.0f) || (currentWindowAspectRatio<=1.0f && surfaceAspectRatio>=1.0f) )
			scaleFitting = 1.0f;
		else if( (currentWindowAspectRatio<=1.0f && surfaceAspectRatio<=currentWindowAspectRatio) )
			scaleFitting = scaleForCurrentWindowAspectRatioY;
		else if( (currentWindowAspectRatio<=1.0f && surfaceAspectRatio>currentWindowAspectRatio) )
			scaleFitting = surfaceAspectRatio;
		else if( (currentWindowAspectRatio>=1.0f && surfaceAspectRatio>=currentWindowAspectRatio) )
			scaleFitting = scaleForCurrentWindowAspectRatioX;
		else if( (currentWindowAspectRatio>=1.0f && surfaceAspectRatio<currentWindowAspectRatio) )
			scaleFitting = 1.0f/surfaceAspectRatio;
		else
			scaleFitting = 1.0f;
	}

	void WindowRenderer::translation(float dx, float dy)
	{
		float	scaleForCurrentWindowAspectRatioX,
			scaleForCurrentWindowAspectRatioY,
			dummy;

		getScalingCoefficients(scaleForCurrentWindowAspectRatioX, scaleForCurrentWindowAspectRatioY,dummy,dummy,dummy);

		dx /= scaleForCurrentWindowAspectRatioX;
		dy /= scaleForCurrentWindowAspectRatioY;

		currentCenterX 	+= (+dx*currentRotationCos + dy*currentRotationSin) / currentScale;
		currentCenterY 	+= (-dx*currentRotationSin + dy*currentRotationCos) / currentScale;
	}

	void WindowRenderer::rotation(float d)
	{
		currentRotationDegrees 	+= d * currentStepRotationDegrees;

		currentRotationCos 	= cos(currentRotationDegrees*M_PI/180.0f);
		currentRotationSin 	= sin(currentRotationDegrees*M_PI/180.0f);
	}

	void WindowRenderer::zoom(int incr)
	{
		wheelSteps += incr;
	}

	void WindowRenderer::setFullscreenMode(bool enabled)
	{
		if(enabled!=fullscreenModeEnabled)
		{
			if(enabled)
			{
				parent = parentWidget();
				setParent(NULL);
				showFullScreen();
			}
			else
			{
				setParent(parent);
				showNormal();
				emit requireResize();
			}

			fullscreenModeEnabled = enabled;
		}
	}

	void WindowRenderer::toggleFullscreenMode(void)
	{
		setFullscreenMode(!fullscreenModeEnabled);
	}

	bool WindowRenderer::isFullscreenModeEnabled(void) const
	{
		return fullscreenModeEnabled;
	}

	void WindowRenderer::resetTransformation(void)
	{
		wheelSteps		= 0;
		deltaX			= 0;
		deltaY			= 0;
		lastPosX		= -1;
		lastPosY		= -1;
		currentCenterX		= 0.0f;
		currentCenterY		= 0.0f;
		currentRotationDegrees	= 0.0f;
		currentScale		= 1.0f;
	}

	WindowRendererContainer::WindowRendererContainer(QWidget* _parent, int w, int h)
	 : QWidget(_parent), childRenderer(this, w, h)
	{
		QObject::connect(&childRenderer, SIGNAL(requireResize(void)), 	this, SLOT(handleResizeRequirement(void)));
	}

	WindowRendererContainer::~WindowRendererContainer(void)
	{ }

	void WindowRendererContainer::resizeEvent(QResizeEvent* event)
	{
		childRenderer.resize(event->size().width(),event->size().height());
	}

	WindowRenderer& WindowRendererContainer::renderer(void)
	{
		return childRenderer;
	}

	void WindowRendererContainer::handleResizeRequirement(void)
	{
		childRenderer.resize(width(),height());
	}
