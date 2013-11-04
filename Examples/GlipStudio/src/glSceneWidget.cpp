// Includes :
	#include "glSceneWidget.hpp"
	#include <limits>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// ViewLink
	int 		ViewLink::viewCounter = 1;
	GLSceneWidget*	ViewLink::quietSceneTarget = NULL;

	ViewLink::ViewLink(GLSceneWidget* _scene)
	 : 	__ReadOnly_ComponentLayout(getLayout()), 
		OutputDevice(getLayout(), "GLViewLink_" + to_string(viewCounter)), 
		scene(_scene), 
		target(NULL)
	{
		viewCounter++;
		setHaloColor(1.0f, 1.0f, 1.0f);
		centerCoords[0] = 0.0f;
		centerCoords[1] = 0.0f;
		angleRadians	= 0.0f;
		scale		= 1.0f;
	}

	ViewLink::~ViewLink(void)
	{
		target = NULL;

		if(scene!=NULL)
			scene->removeView(this);
	}

	OutputDevice::OutputDeviceLayout ViewLink::getLayout(void) const
	{
		OutputDevice::OutputDeviceLayout tmp("GLViewLink_" + to_string(viewCounter));

		// Add input ports :
		tmp.addInputPort("viewInput");

		return tmp;
	}

	void ViewLink::process(void)
	{
		target 	= &in(0);

		bringUp();
	}

	bool ViewLink::preparedToDraw(void)
	{
		if(target!=NULL)
		{
			target->bind();
			return true;
		}
		else
			return false;
	}

	const __ReadOnly_HdlTextureFormat& ViewLink::format(void)
	{
		if(target==NULL)
			throw Exception("ViewLink::format - Viewer not connected.", __FILE__, __LINE__);
		else
			return *target;
	}

	void ViewLink::getScalingRatios(float* imageScaling, float* haloScaling, float haloSize, float currentPixelX, float currentPixelY)
	{
		float	width 	= format().getWidth(),
			height	= format().getHeight();

		if(width>=height)
		{
			imageScaling[0] = 1.0f * scale;
			imageScaling[1] = height / width * scale;

			/*haloScaling[0]	= imageScaling[0] * (1.0f + haloSize);
			haloScaling[1]	= imageScaling[1] * (1.0f + haloSize);*/
			haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
			haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
		}
		else
		{
			imageScaling[0] = width / height * scale;
			imageScaling[1] = 1.0f * scale;

			/*haloScaling[0]	= imageScaling[0] * (1.0f + haloSize);
			haloScaling[1]	= imageScaling[1] * (1.0f + haloSize);*/
			haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
			haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
		}
	}

	void ViewLink::setHaloColor(float r, float g, float b)
	{
		haloColorRGB[0]	= r;
		haloColorRGB[1]	= g;
		haloColorRGB[2]	= b;
	}

	void ViewLink::clear(void)
	{
		target = NULL;

		if(scene!=NULL)
			scene->updateScene();
	}

	void ViewLink::bringUp(void)
	{
		if(scene!=NULL)
		{
			if( quietSceneTarget!=scene )
			{
				scene->bringUpView(this);
				scene->updateScene();
			}
			// else : wait for ViewLink::endQuietUpdate to push the updates.
		}
		else
			throw Exception("ViewLink::bringUp - This view is closed.", __FILE__, __LINE__);
	}

	bool ViewLink::isVisible(void) const
	{
		if(scene==NULL)
			return false;
		else
			return scene->viewIsVisible(this);
	}

	bool ViewLink::isSelected(void) const
	{
		if(scene==NULL)
			return false;
		else
			return scene->viewIsSelected(this);
	}

	bool ViewLink::isClosed(void) const
	{
		return scene==NULL;
	}

	void ViewLink::selectView(bool dropCurrentSelection)
	{
		if(scene!=NULL)
		{
			scene->selectView(this, dropCurrentSelection);
			scene->updateScene();
		}
	}

	void ViewLink::beginQuietUpdate(void)
	{
		if(scene!=NULL)
		{
			if(quietSceneTarget==NULL)
				quietSceneTarget = scene;
			else
				throw Exception("ViewLink::beginQuietUpdate - Quiet mode already enabled.", __FILE__, __LINE__);
		}
	}

	void ViewLink::endQuietUpdate(void)
	{
		if(quietSceneTarget!=NULL)
		{
			quietSceneTarget->updateScene();
			quietSceneTarget = NULL;
		}
		else
			throw Exception("ViewLink::endQuietUpdate - Not in quiet mode.", __FILE__, __LINE__);
	}

// View Manager : 
	int ViewManager::managerCount = 0;

	ViewManager::ViewManager(GLSceneWidget* _scene, QWidget* parent)
	 : 	QMenu("Views", parent),
		scene(_scene),
		currentManagerID(managerCount),
		createNewViewAction("Display in a new View", this),
		closeAllViewAction("Close all Views", this)
	{
		managerCount++;

		addAction(&createNewViewAction);
		addAction(&closeAllViewAction);

		connect(&createNewViewAction, 			SIGNAL(triggered()), 	this, SIGNAL(createNewView()));
		connect(&closeAllViewAction,			SIGNAL(triggered()), 	this, SLOT(closeAllViews()));
		connect(this,					SIGNAL(closed()),	this, SLOT(closeAllViews()));

		enableCreationAction(false);

		// Build halo color from manager index : 
		float hue = currentManagerID * 125.0f;	// The maximum number of groups is given by lcm(delta, 360)/delta (FR : ppcm(delta, 360)/delta)
		genColor(hue, r, g, b);
	}

	ViewManager::~ViewManager(void)
	{
		closeAllViews();

		if(scene!=NULL)
			scene->removeManager(this);
	}
	
	void ViewManager::genColor(float hue, float& red, float& green, float& blue)
	{
		hue = static_cast<int>(hue) % 360;
		hue /= 60.0f;
		int 	i = std::floor(hue);
		float	f = hue - i;		
		
		switch( i )
		{
			case 0:
				red	= 1.0f;
				green	= f;
				blue	= 0.0f;
				break;
			case 1:
				red	= 1.0f - f;
				green	= 1.0f;
				blue	= 0.0f;
				break;
			case 2:
				red	= 0.0f;
				green	= 1.0f;
				blue	= f;
				break;
			case 3:
				red	= 0.0f;
				green	= 1.0f - f;
				blue	= 1.0f;
				break;
			case 4:
				red	= f;
				green	= 0.0f;
				blue	= 1.0f;
				break;
			default:		// case 5:
				red	= 1.0f;
				green	= 0.0f;
				blue	= 1.0f - f;
				break;
		}
	}

	void ViewManager::viewClosed(void)
	{
		ViewLink* link = reinterpret_cast<ViewLink*>( QObject::sender() );

		std::vector<ViewLink*>::iterator it = std::find(viewLinks.begin(), viewLinks.end(), link);
		
		if(it!=viewLinks.end())
		{
			int k = std::distance(viewLinks.begin(), it);

			delete (*it);
			viewLinks.erase(it);
			recordIDs.erase( recordIDs.begin() + k);
		}
	}

	void ViewManager::closeAllViews(void)
	{
		while(!viewLinks.empty())
			removeRecord(recordIDs.back());
	}

	void ViewManager::enableCreationAction(bool s)
	{
		createNewViewAction.setEnabled(s);
	}

	void ViewManager::show(int recordID, HdlTexture& texture, bool newView)
	{
		if( viewLinks.empty() || newView )
		{
			if(scene==NULL)
				return ;

			// Create a new view : 
			ViewLink* link = scene->createView();

			if(link==NULL)
				return ;

			viewLinks.push_back( link );
			recordIDs.push_back(recordID);

			connect(viewLinks.back(), SIGNAL(closed()), this, SLOT(viewClosed()));

			viewLinks.back()->setHaloColor(r, g, b);
			(*viewLinks.back()) << texture << OutputDevice::Process;
			viewLinks.back()->selectView();
		}
		else if(isLinkedToAView(recordID))
		{
			update(recordID, texture);

			// Force Selection : 
			for(int k=0; k<recordIDs.size(); k++)
			{
				if(recordIDs[k]==recordID)
					viewLinks[k]->selectView();
			}
		}
		else
		{
			// Find the first selected view :	
			int k;
			for(k = 0; k<viewLinks.size(); k++)
			{
				if(viewLinks[k]->isSelected())
					break;
			}

			// ... Or use the last created one otherwise : 
			if(k>=viewLinks.size())
				k--;
		
			recordIDs[k] = recordID;
			(*viewLinks[k]) << texture << OutputDevice::Process;
			viewLinks[k]->selectView();
		}
	}
		
	void ViewManager::update(int recordID, HdlTexture& texture)
	{
		for(int k=0; k<recordIDs.size(); k++)
		{
			if(recordIDs[k]==recordID)
				(*viewLinks[k]) << texture << OutputDevice::Process;
		}
	}

	bool ViewManager::isLinkedToAView(int recordID) const
	{
		std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		
		return it!=recordIDs.end();
	}

	bool ViewManager::isOnDisplay(int recordID) const
	{
		std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

		while(it!=recordIDs.end())
		{
			int k = std::distance(recordIDs.begin(), it);

			if(viewLinks[k]->isVisible())
				return true;

			it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		}

		return false;
	}

	bool ViewManager::hasViews(void) const
	{
		return !viewLinks.empty();
	}

	void ViewManager::removeRecord(int recordID)
	{
		std::vector<int>::iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

		while(it!=recordIDs.end())
		{
			int k = std::distance(recordIDs.begin(), it);	

			// Remove :
			delete viewLinks[k];
			viewLinks.erase( viewLinks.begin() + k );
			recordIDs.erase(it);

			// Next : 
			it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		}
	}

	void ViewManager::beginQuietUpdate(void)
	{
		if(!viewLinks.empty())
			viewLinks.front()->beginQuietUpdate();
	}

	void ViewManager::endQuietUpdate(void)
	{
		if(!viewLinks.empty())
			ViewLink::endQuietUpdate();
	}

// Special shaders : 
	const std::string placementVertexShaderSource = 	"#version 130															\n"
								"																\n"
								"in vec2 vertexInput;														\n"
								"																\n"
								"uniform float 	angle 			= 0.0,											\n"
								"		homothetieRapport	= 1.0;											\n"
								"uniform vec2	screenScaling 		= vec2(1.0, 1.0),									\n"
								"		imageScaling		= vec2(1.0, 1.0),									\n"
								"		centerCoords		= vec2(0.0, 0.0),									\n"
								"		screenCenter		= vec2(0.0, 0.0),									\n"
								"		homothetieCentre	= vec2(0.0, 0.0);									\n"
								"																\n"
								"void main()															\n"
								"{																\n"
								"	// Generate rotation matrix : 												\n"
								"	mat2 rot = mat2(cos(angle), 	sin(angle), 										\n"
								"			-sin(angle), 	cos(angle));										\n"
								"																\n"
								"	// Scale : 														\n"
								"	vec2 tmpVertex	= vertexInput;												\n"
								"																\n"
								"	tmpVertex.x	= tmpVertex.x * imageScaling.x;										\n"
								"	tmpVertex.y	= tmpVertex.y * imageScaling.y;										\n"
								"	tmpVertex	= rot * tmpVertex;											\n"
								"	tmpVertex	= tmpVertex + centerCoords;										\n"
								"																\n"
								"	tmpVertex	= tmpVertex + screenCenter;										\n"
								"																\n"
								"	if(homothetieRapport==0.0f) // Translation										\n"
								"		tmpVertex	= tmpVertex + homothetieCentre;									\n"
								"	else // Homothetie													\n"
								"		tmpVertex	= (tmpVertex - homothetieCentre) * homothetieRapport + homothetieCentre;			\n"
								"																\n"
								"	tmpVertex.x	= tmpVertex.x * screenScaling.x;									\n"
								"	tmpVertex.y	= tmpVertex.y * screenScaling.y;									\n"
								"																\n"
								"	// Write : 														\n"
								"	gl_Position 	= vec4(tmpVertex, 0.0, 1.0);										\n"
								"																\n"
								"	gl_TexCoord[0] 	= gl_MultiTexCoord0;											\n"
								"}																\n";

	const std::string placementFragmentShaderSource = 	"#version 130															\n"
								"																\n"
								"uniform sampler2D	viewTexture;												\n"
								"out     vec4 		displayOutput;												\n"
								"																\n"
								"uniform int		selectionMode	= 0,	// 0 for drawing, 1 for making a selection, 2 for drawing a halo.		\n"	
								"			objectID	= 0;											\n"
								"uniform vec3		haloColor	= vec3(1.0, 1.0, 1.0);									\n"
								"																\n"
								"void main()															\n"
								"{																\n"
								"	if(selectionMode==0)													\n"
								"	{															\n"
								"		// Get the input data :												\n"
								"		vec4 col  = textureLod(viewTexture, vec2(gl_TexCoord[0].s, 1.0f-gl_TexCoord[0].t), 0.0);			\n"
								"																\n"
								"		// Write the output data :											\n"
								"		displayOutput = col;												\n"
								"	}															\n"
								"	else if(selectionMode==1)												\n"
								"		displayOutput = vec4( vec3(1.0f,1.0f,1.0f)*float(objectID)/255.0f, 1.0f);					\n"
								"	else //if(selectionMode==2)												\n"
								"	{															\n"
								"		// Compute the transparency : 											\n"
								"		float a = 5.0f*pow( 1.0f - 2.0f * max( abs(gl_TexCoord[0].s-0.5f), abs(gl_TexCoord[0].t-0.5f) ), 0.7f);		\n"
								"		displayOutput = vec4(haloColor, a);										\n"
								"	}															\n"
								"}																\n";

// GLSceneWidget
	GLSceneWidget::GLSceneWidget(int width, int height, QWidget* _parent)
	 : 	QGLWidget(_parent),
		parent(_parent),
		quad(NULL),
		placementProgram(NULL),
		mouseMovementsEnabled(false), 
		keyboardMovementsEnabled(false), 
		doubleLeftClick(false), 
		doubleRightClick(false), 
		leftClick(false), 
		rightClick(false), 
		mouseJustLeftClicked(false),
		mouseJustRightClicked(false),
		mouseJustLeftClickReleased(false),
		mouseJustRightClickReleased(false),
		mouseWheelJustTurned(false),
		wheelSteps(0), 
		deltaX(0), 
		deltaY(0), 
		lastPosX(-1), 
		lastPosY(-1),
		deltaWheelSteps(0), 
		wheelRotationAtX(0), 
		wheelRotationAtY(0),
		fullscreenModeEnabled(false),
		clearColorRed(0.1f),
		clearColorGreen(0.1f),
		clearColorBlue(0.1f),
		handMode(true),
		contextMenu(this)
	{
		screenCenter[0]		= 0.0f;
		screenCenter[1]		= 0.0f;
		homothetieCentre[0]	= 0.0f;
		homothetieCentre[1]	= 0.0f;
		homothetieRapport	= 1.0f;

		QWidget::setGeometry(10,10,width,height);
		makeCurrent();
		setAutoBufferSwap(false);

		try
		{
			// Glip-lib specifics : 
			HandleOpenGL::init();

			// Load Placement shader : 
			//reloadPlacementShader();

			ShaderSource 		sourceVertex(placementVertexShaderSource),
						sourcePixel(placementFragmentShaderSource);

			HdlShader 		vertexShader(GL_VERTEX_SHADER, sourceVertex),
						pixelShader(GL_FRAGMENT_SHADER, sourcePixel);

			placementProgram	= new HdlProgram( vertexShader, pixelShader);
		}
		catch(Exception& e)
		{
			// Show Error : 
			std::cout << "Caught an exception : " << std::endl;
			std::cout << e.what() << std::endl;

			QMessageBox::information(parentWidget(), tr("GlipStudio - Error during initialization : "), e.what());

			// re-throw :
			throw e; 
		}

		glViewport(0, 0, width, height);

		// Contextual Menu : 
		selectAllAction			= contextMenu.addAction("Select All", 				this, SLOT(selectAll()));
		selectAllVisibleAction		= contextMenu.addAction("Select All Visible", 			this, SLOT(selectAllVisible()));
		resetSelectedAngleAction	= contextMenu.addAction("Reset angles of selected views", 	this, SLOT(resetSelectionAngle()));
		resetSelectedScaleAction	= contextMenu.addAction("Reset scales of selected views", 	this, SLOT(resetSelectionAngle()));
		resetSelectedPositionAction 	= contextMenu.addAction("Reset positions of selected views",	this, SLOT(resetSelectionPosition()));
		resetSelectionAction		= contextMenu.addAction("Reset selected views", 		this, SLOT(resetSelection()));
		hideSelectedAction		= contextMenu.addAction("Hide selected views", 			this, SLOT(hideCurrentSelection()));
		closeSelectedAction		= contextMenu.addAction("Close selected views", 		this, SLOT(closeSelection()));
		hideAllAction			= contextMenu.addAction("Hide All", 				this, SLOT(hideAll()));
		showAllAction			= contextMenu.addAction("Show All", 				this, SLOT(showAll()));
		resetGlobalPositionAction	= contextMenu.addAction("Reset Global Position", 		this, SLOT(resetGlobalPosition()));
		resetGlobalZoomAction		= contextMenu.addAction("Reset Global Zoom", 			this, SLOT(resetGlobalZoom()));
		resetGlobalAction		= contextMenu.addAction("Reset Global",				this, SLOT(resetGlobal()));
		handModeAction 			= contextMenu.addAction("Hand Mode",				this, SLOT(switchSelectionMode()));
		toggleFullscreenAction		= contextMenu.addAction("Fullscreen",				this, SLOT(toggleFullscreenMode()));

		handModeAction->setCheckable(true);
		toggleFullscreenAction->setCheckable(true);

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
		setKeyForAction(KeyResetView,			Qt::Key_Backspace);
		setKeyForAction(KeyCloseView,			Qt::Key_Delete);
		setKeyForAction(KeyControl,			Qt::Key_Control);
		setKeyForAction(KeyShiftRotate,			Qt::Key_Shift);
		setKeyForAction(KeyToggleHandMode,		Qt::Key_Space);
		setKeyForAction(KeySelectAll,			QKeySequence::SelectAll);

		for(int i=0; i<NumActionKey; i++)
		{
			keyPressed[i]		= false;
			keyJustPressed[i]	= false;
			keyJustReleased[i]	= false;
		}

		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setMinimumSize(QSize(256,256));

		quad = new GeometryInstance( GeometryPrimitives::StandardQuad(), GL_STATIC_DRAW_ARB );

		handMode = false;
		switchSelectionMode(); // Will turn hand mode to true and set the correct initialization.
	}


	GLSceneWidget::~GLSceneWidget(void)
	{
		while(!links.empty())
			removeView(links.back(), true);

		while(!managers.empty())
			removeManager(managers.back(), true);

		makeCurrent();
		delete quad;
		HandleOpenGL::deinit();
	}

	// Tools : 
		GLSceneWidget::KeyAction GLSceneWidget::correspondingAction(const QKeySequence& k) const
		{
			for(int i=0; i<NumActionKey; i++)
				if(keyAssociation[i]==k)
					return static_cast<GLSceneWidget::KeyAction>(i);

			// else
			return NoAction;
		}

		GLSceneWidget::KeyAction GLSceneWidget::correspondingAction(const QKeyEvent& e) const
		{
			const int s = (e.key() | e.modifiers());

			for(int i=0; i<NumActionKey; i++)
			{
				for(int k=0; k<keyAssociation[i].count(); k++)
				{
					if(s==keyAssociation[i][k])
						return static_cast<GLSceneWidget::KeyAction>(i);
				}
			}

			// else
			return NoAction;
		}

		int GLSceneWidget::getViewID(const ViewLink* view) const
		{
			std::vector<ViewLink*>::const_iterator it = std::find(links.begin(), links.end(), view);

			if(it==links.end())
				return -1;
			else
				return std::distance(links.begin(), it);
		}

	// Qt events interception : 
		void GLSceneWidget::keyPressEvent(QKeyEvent* event)
		{
			if(!event->isAutoRepeat())
			{
				GLSceneWidget::KeyAction a = correspondingAction(event->key());

				#ifdef __VERBOSE__
					std::cout << "GLSceneWidget::keyPressEvent - Event : Key pressed" << std::endl;
				#endif

				if(a!=NoAction)
				{
					keyPressed[static_cast<int>(a)] = true;
					keyJustPressed[static_cast<int>(a)] = true;
					keyJustReleased[static_cast<int>(a)] = false;

					processAction();
				}
				else
					std::cerr << "GLSceneWidget::keyPressEvent - Warning : Key not associated" << std::endl;
			}
			else
			{
				GLSceneWidget::KeyAction a = correspondingAction(event->key());

				if(a!=NoAction)
					processAction();
			}
		}

		void GLSceneWidget::keyReleaseEvent(QKeyEvent* event)
		{
			if(!event->isAutoRepeat())
			{
				GLSceneWidget::KeyAction a = correspondingAction(event->key());

				#ifdef __VERBOSE__
					std::cout << "GLSceneWidget::keyReleaseEvent - Event : Key released" << std::endl;
				#endif

				if(a!=NoAction)
				{
					keyPressed[static_cast<int>(a)] = false;
					keyJustPressed[static_cast<int>(a)] = false;
					keyJustReleased[static_cast<int>(a)] = true;

					processAction();
				}
				else
					std::cerr << "GLSceneWidget::keyReleaseEvent - Warning : Key not associated" << std::endl;
			}
		}

		void GLSceneWidget::mouseMoveEvent(QMouseEvent* event)
		{
			#ifdef __VERBOSE__
				std::cout <<"GLSceneWidget::mouseMoveEvent - Mouse move event." << std::endl;
			#endif

			if(lastPosX==-1) lastPosX = event->x();
			if(lastPosY==-1) lastPosY = event->y();

			deltaX += event->x()-lastPosX,
			deltaY += event->y()-lastPosY;

			lastPosX = event->x();
			lastPosY = event->y();

			processAction();
		}

		void GLSceneWidget::wheelEvent(QWheelEvent *event)
		{
			#ifdef __VERBOSE__
				std::cout <<"GLSceneWidget::wheelEvent - Wheel event." << std::endl;
			#endif

			if(event->delta()!=0)
			{
				wheelSteps 		+= event->delta()/(8*15);
				deltaWheelSteps 	= event->delta()/(8*15);
				mouseWheelJustTurned 	= true;
			}

			lastPosX = event->x();
			lastPosY = event->y();

			processAction();

			// wheelSteps 	> 0 : away of the user
			// 		< 0 : toward the user
		}

		void GLSceneWidget::mousePressEvent(QMouseEvent *event)
		{
			#ifdef __VERBOSE__
				std::cout << "GLSceneWidget::mousePressEvent - Click event." << std::endl;
			#endif

			if( (event->buttons() & Qt::LeftButton) )
			{
				leftClick 			= true;
				mouseJustLeftClicked		= true;
				mouseJustLeftClickReleased	= false;
			}
			else if( (event->buttons() & Qt::RightButton) )
			{
				rightClick 			= true;
				mouseJustRightClicked		= true;
				mouseJustRightClickReleased	= false;
			}

			/*if( (event->buttons() & Qt::RightButton) || ((event->buttons() & Qt::LeftButton) && pressed(KeyControl)) )
				originalOrientationBeforeRightClick = currentRotationDegrees - atan2( event->y() - height()/2, event->x() - width()/2 ) * 180.0f / M_PI;*/

			lastPosX = event->x();
			lastPosY = event->y();

			processAction();
		}

		void GLSceneWidget::mouseReleaseEvent(QMouseEvent *event)
		{
			#ifdef __VERBOSE__
				std::cout << "GLSceneWidget::mouseReleaseEvent - End click event." << std::endl;
			#endif

			if(!(event->buttons() & Qt::LeftButton))
			{
				leftClick 			= false;
				mouseJustLeftClicked		= false;
				mouseJustLeftClickReleased	= true;
		
			}			
			else if(!(event->buttons() & Qt::RightButton))
			{
				rightClick 			= false;
				mouseJustRightClicked		= false;
				mouseJustRightClickReleased	= true;
			}

			lastPosX = event->x();
			lastPosY = event->y();

			processAction();
		}

		void GLSceneWidget::mouseDoubleClickEvent(QMouseEvent * event)
		{
			#ifdef __VERBOSE__
				std::cout <<"GLSceneWidget::mouseDoubleClickEvent - Double click event." << std::endl;
			#endif

			if(event->buttons() & Qt::LeftButton)
				doubleLeftClick = true;
			else if(event->buttons() & Qt::RightButton)
				doubleRightClick = true;

			lastPosX = event->x();
			lastPosY = event->y();

			processAction();
		}

		bool GLSceneWidget::pressed(const KeyAction& a) const
		{
			return keyPressed[static_cast<int>(a)];
		}

		bool GLSceneWidget::justPressed(const GLSceneWidget::KeyAction& a)
		{
			if(keyJustPressed[static_cast<int>(a)])
			{
				keyJustPressed[static_cast<int>(a)] = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justReleased(const KeyAction& a)
		{
			if(keyJustReleased[static_cast<int>(a)])
			{
				keyJustReleased[static_cast<int>(a)] = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justLeftClicked(void)
		{
			if(mouseJustLeftClicked)
			{
				mouseJustLeftClicked = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justLeftClickReleased(void)
		{
			if(mouseJustLeftClickReleased)
			{
				mouseJustLeftClickReleased = false;
				return true;
			}
			else
				return false;
		}
	
		bool GLSceneWidget::justRightClicked(void)
		{
			if(mouseJustRightClicked)
			{
				mouseJustRightClicked = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justRightClickReleased(void)
		{
			if(mouseJustRightClickReleased)
			{
				mouseJustRightClickReleased = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justDoubleLeftClicked(void)
		{
			if(doubleLeftClick)
			{
				doubleLeftClick = false;
				return true;
			}
			else
				return false;
		}
	
		bool GLSceneWidget::justDoubleRightClicked(void)
		{
			if(doubleRightClick)
			{
				doubleRightClick = false;
				return true;
			}
			else
				return false;
		}

		bool GLSceneWidget::justMouseWheelTurned(void)
		{
			if(mouseWheelJustTurned)
			{
				mouseWheelJustTurned = false;
				return true;
			}
			else
				return false;
		}

	// Process Action function : 
		ViewLink* GLSceneWidget::updateSelection(bool dropSelection)
		{
			ViewLink* under = getObjectIDUnder(lastPosX, lastPosY);

			bool test = viewIsSelected(under);

			if(under==NULL && !selectionList.empty() && !pressed(KeyControl) && dropSelection)
				selectionList.clear();
			else if(under!=NULL && !pressed(KeyControl) && !test)
			{
				selectionList.clear();
				selectionList.push_back(under);
			}
			else if(under!=NULL && pressed(KeyControl) && !test)
			{
				selectionList.push_back(under);
			}

			if(under!=NULL)
				emit under->selected();

			return under;
		}
	
		void GLSceneWidget::processAction(void)
		{
			bool needUpdate = false;

			// Mouse : 
				if(justDoubleLeftClicked())
				{
					toggleFullscreenMode();
					doubleLeftClick = false;
					needUpdate = true;
				}

				if(justLeftClicked())
				{
					if(handMode)
						setCursor(Qt::ClosedHandCursor);
					else
					{
						ViewLink* under = updateSelection();
		
						if(under!=NULL)
							bringUpView(under);
					}
					
					needUpdate = true; // Need update for deselection anymway.
				}

				if(justLeftClickReleased())
				{
					if(handMode)
						setCursor(Qt::OpenHandCursor);
				}

				if(justRightClicked())
				{
					if(!handMode)
						updateSelection(false);

					QPoint globalPos = mapToGlobal(QPoint(lastPosX, lastPosY));

					updateContextMenu();

					contextMenu.exec(globalPos);
				}

				if(deltaX!=0 || deltaY!=0)
				{
					// Convert to screen size : 
					float	dx = 0.0f, 
						dy = 0.0f;

					getGLCoordinatesRelative(deltaX, deltaY, dx, dy);

					if(!pressed(KeyShiftRotate))
					{
						// Apply to selection as translation :
						if(!selectionList.empty() && !handMode)
						{
							for(std::vector<ViewLink*>::iterator it = selectionList.begin(); it!=selectionList.end(); it++)
							{
								(*it)->centerCoords[0] += dx;
								(*it)->centerCoords[1] += dy;
							}
						}
						else if(handMode)
						{
							screenCenter[0] += dx;
							screenCenter[1] += dy;
						}
					}
					else
					{
						ViewLink* under = updateSelection(false);

						if(under!=NULL)
						{
							// Compute angle for current center :
							float 	ox	= 0.0f,
								oy	= 0.0f,
								nx	= 0.0f,
								ny	= 0.0f,
								dAngle 	= 0.0f;

							getGLCoordinatesAbsolute(lastPosX, lastPosY, ox, oy);
							getGLCoordinatesAbsolute(lastPosX - deltaX, lastPosY - deltaY, nx, ny);

							ox	= under->centerCoords[0] - ox;
							oy	= under->centerCoords[1] - oy;
							nx 	= under->centerCoords[0] - nx;
							ny	= under->centerCoords[1] - ny;

							//std::cout << "New angle : " << std::atan2(oy, ox) << std::endl;
							dAngle	= std::atan2(oy, ox) - std::atan2(ny, nx);

							// Apply to selection as rotation :
							for(std::vector<ViewLink*>::iterator it = selectionList.begin(); it!=selectionList.end(); it++)
								(*it)->angleRadians += dAngle;
						}
					}

					needUpdate 	= true;
					deltaX		= 0;
					deltaY		= 0;
				}

				if( deltaWheelSteps!=0 )
				{
					if(!selectionList.empty() && !handMode)
					{
						for(std::vector<ViewLink*>::iterator it = selectionList.begin(); it!=selectionList.end(); it++)
							(*it)->scale *= std::pow(1.2f, deltaWheelSteps);
					}
					else if(handMode)
					{
						float xc, yc;
						getGLCoordinatesAbsoluteRaw(lastPosX, lastPosY, xc, yc);
						homothetieComposition(xc, yc, static_cast<float>(deltaWheelSteps>0.0f)*2.0f - 1.0f);
					}

					needUpdate 	= true;
					deltaWheelSteps = 0;
				}

			// Keyboard : 
				if( justPressed(KeyToggleHandMode) )
					switchSelectionMode();

				if( justPressed(KeyResetView) )
				{
					if(handMode)
						resetGlobal();
					else
						resetSelection();
				
					needUpdate 	= true;
				}

				if( justPressed(KeyToggleFullscreen) )
					toggleFullscreenMode();

				if( justPressed(KeyExitOnlyFullscreen) )
					setFullscreenMode(false);

				if( justPressed(KeyCloseView) )
				{
					closeSelection();
					needUpdate 	= true;
				}

				if( justPressed(KeySelectAll) )
				{
					selectAll();
					needUpdate 	= true;
				}

				if( pressed(KeyRotationClockWise) )
				{
					for(std::vector<ViewLink*>::iterator it = selectionList.begin(); it!=selectionList.end(); it++)
						(*it)->angleRadians -= 0.05f;
					needUpdate 	= true;
				}				

				if( pressed(KeyRotationCounterClockWise) )
				{
					for(std::vector<ViewLink*>::iterator it = selectionList.begin(); it!=selectionList.end(); it++)
						(*it)->angleRadians += 0.05f;
					needUpdate 	= true;
				}

			// Finally : 
			if(needUpdate)
				paintGL();
		}

	// QGL Tools : 
		void GLSceneWidget::initializeGL(void)
		{
			makeCurrent();
			glViewport(0, 0, width(), height());
		}

		void GLSceneWidget::resizeGL(int width, int height)
		{
			#ifdef __VERBOSE__
				std::cout <<"GLSceneWidget::resizeGL - Resizing to " << width << "x" << height << std::endl;
			#endif

			if(height==width)
				std::cout << "WIDTH == HEIGHT" << std::endl;

			glViewport(0, 0, width, height);
		}

		void GLSceneWidget::drawScene(bool forSelection)
		{
			// Use the placement shader : 
			if(placementProgram!=NULL && !displayList.empty())
			{
				placementProgram->use();

				// Set screen scaling variable : 
				float screenScaling[2];

				float sPixelX, sPixelY;
				getGLCoordinatesRelative(1.0, 1.0, sPixelX, sPixelY);

				// Some might be negative due to directions of GL axis : 
				sPixelX = std::abs(sPixelX);
				sPixelY = std::abs(sPixelY);

				// The real ones : 
				if(width()>=height())
				{
					screenScaling[0] = static_cast<float>(height())/static_cast<float>(width());
					screenScaling[1] = 1.0f;
				}
				else
				{
					screenScaling[0] = 1.0f;
					screenScaling[1] = static_cast<float>(width())/static_cast<float>(height());
				}
				//std::cout << "Screen scaling : " << screenScaling[0] << " x " << screenScaling[1] << std::endl;

				placementProgram->modifyVar("screenCenter", GL_FLOAT_VEC2, screenCenter);
				placementProgram->modifyVar("screenScaling", GL_FLOAT_VEC2, screenScaling);
				placementProgram->modifyVar("homothetieCentre", GL_FLOAT_VEC2, homothetieCentre);
				placementProgram->modifyVar("homothetieRapport", GL_FLOAT, homothetieRapport);

				// Set the mode :
				if(forSelection)
					placementProgram->modifyVar("selectionMode", GL_INT, 1);
				else
					placementProgram->modifyVar("selectionMode", GL_INT, 0);

				// Go through the list of view in order to draw some of them :
				for(std::list<ViewLink*>::iterator it = displayList.begin(); it!=displayList.end(); it++)
				{
					if((*it)->preparedToDraw())
					{
						// Set positions and rotation : 
						placementProgram->modifyVar("centerCoords", 	GL_FLOAT_VEC2, 	(*it)->centerCoords);
						placementProgram->modifyVar("angle",		GL_FLOAT,	(*it)->angleRadians);

						// Set screen scaling variable : 
						float 	imageScaling[2],
							haloScaling[2];

						(*it)->getScalingRatios(imageScaling, haloScaling, 10.0f, sPixelX, sPixelY); // 4.0 is the border layout in pixels, when selected.

						if(forSelection)
							placementProgram->modifyVar("objectID", GL_INT, getViewID(*it)+1);					
						else if(viewIsSelected(*it))
						{
							placementProgram->modifyVar("selectionMode", GL_INT, 2);
							placementProgram->modifyVar("imageScaling", GL_FLOAT_VEC2, haloScaling);
							placementProgram->modifyVar("haloColor", GL_FLOAT_VEC3, (*it)->haloColorRGB);
							
							quad->draw();

							// Restore : 
							placementProgram->modifyVar("selectionMode", GL_INT, 0);
						}	
						
						// Scaling of the image : 
						placementProgram->modifyVar("imageScaling", GL_FLOAT_VEC2, imageScaling);
				
						// Draw the image :
						quad->draw();
					}
				}

				// Axis : 
				/*{
					float centerCoords[2];
					centerCoords[0] = 0.0f;
					centerCoords[1] = 0.0f;
					float imageScaling[2];
					imageScaling[0] = 1.0f;
					imageScaling[1] = 1.0f;
					placementProgram->modifyVar("centerCoords", 	GL_FLOAT_VEC2, 	centerCoords);
					placementProgram->modifyVar("imageScaling", GL_FLOAT_VEC2, imageScaling);

					placementProgram->modifyVar("selectionMode", GL_INT, 2);

					glBegin(GL_LINES);
						glColor3f(1.0f,0.0f,0.0f);
						glVertex2f(0.0f,0.0f);
						glVertex2f(1.0f,0.0f);
						glColor3f(1.0f,0.0f,1.0f);
						glVertex2f(0.05f,0.05f);
						glVertex2f(0.5f,0.05f);
						glColor3f(0.0f,1.0f,0.0f);
						glVertex2f(0.0f,0.0f);
						glVertex2f(0.0f,1.0f);
						glColor3f(0.0f,1.0f,1.0f);
						glVertex2f(0.05f,0.05f);
						glVertex2f(0.05f,0.5f);
					glEnd();
				}*/
				// Clean : 
				HdlTexture::unbind();
				HdlProgram::stopProgram();
			}
		}

		void GLSceneWidget::paintGL(void)
		{
			// Clear framebuffer : 
			glClearColor( clearColorRed, clearColorGreen, clearColorBlue, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			// Allow blending : 
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			drawScene(false);

			// Stop blending : 
			glDisable(GL_BLEND);
	
			// Print on screen : 
			swapBuffers();
		}

		float GLSceneWidget::getGlobalScale(void) const
		{
			if(homothetieRapport==0.0f)
				return 1.0f;
			else
				return homothetieRapport;
		}

		void GLSceneWidget::getGLCoordinatesAbsoluteRaw(float x, float y, float& glX, float& glY)
		{
			float	w	= width(),
				h	= height();

			getGLCoordinatesRelativeRaw(x, y, glX, glY);

			if(w>=h)
			{
				glX -= (w/h);
				glY += 1.0f;
			}
			else
			{
				glX -= 1.0f;
				glY += h/w;
			}

			// std::cout << "GL coordinates RAW: " << glX << " x " << glY << "   (pixel size : " << (1.0/ height() * 2.0f / homothetieRapport) << ')' << std::endl;
		}

		void GLSceneWidget::getGLCoordinatesRelativeRaw(float x, float y, float& glX, float& glY)
		{
			float	w	= width(),
				h	= height();

			if(w>=h)
			{
				glX = x / h * 2.0f; // X 2 because the OpenGL axis goes from -1 to 1.
				glY = -y / h * 2.0f; // - because the Y axis is upward (respect to the screen) for GL while it is downward for Qt.
			}
			else
			{
				glX = x / w * 2.0f;
				glY = -y / w * 2.0f;
			}
		}

		void GLSceneWidget::getGLCoordinatesAbsolute(float x, float y, float& glX, float& glY)
		{
			float	w	= width(),
				h	= height(),
				z	= getGlobalScale();

			getGLCoordinatesRelative(x, y, glX, glY);

			// To GL center : 
			if(w>=h)
			{
				glX -= (w/h) / z;
				glY += 1.0f / z;
			}
			else
			{
				glX -= 1.0f / z;
				glY += h/w / z;
			}

			// Offset for current zoom : 
			glX += screenCenter[0];
			glY += screenCenter[1];

			//std::cout << "GL coordinates    : " << glX << " x " << glY << "   (pixel size : " << (1.0/ height() * 2.0f / z) << ')' << std::endl;
		}

		void GLSceneWidget::getGLCoordinatesRelative(float x, float y, float& glX, float& glY)
		{	// OK!
			float	w	= width(),
				h	= height(),
				z	= getGlobalScale();

			if(w>=h)
			{
				glX = x / h * 2.0f / z; // X 2 because the OpenGL axis goes from -1 to 1.
				glY = -y / h * 2.0f / z; // - because the Y axis is upward (respect to the screen) for GL while it is downward for Qt.
			}
			else
			{
				glX = x / w * 2.0f / z;
				glY = -y / w * 2.0f / z;
			}
		}

		void GLSceneWidget::homothetieComposition(float xc, float yc, float zoomDirection)
		{
			float 	newRapport	= (zoomDirection>0) ? (1.2f) : (1.0f/1.2f),
				a		= homothetieRapport * newRapport - newRapport,
				b		= newRapport - 1.0f,
				c		= homothetieRapport * newRapport - 1.0f;

			/*std::cout << "Before : " << std::endl;
			std::cout << "    hx = " << homothetieCentre[0] << "; hy = " << homothetieCentre[1] << std::endl;
			std::cout << "    r  = " << homothetieRapport << std::endl;
			std::cout << "    xc = " << xc << "; yc = " << yc << std::endl;
			std::cout << "    rn = " << newRapport << std::endl;*/

			if(std::abs(c) <= 100.0f * std::numeric_limits<float>::epsilon()) // La composition est une translation
			{
				homothetieCentre[0]	= (1.0f - newRapport) * (xc - homothetieCentre[0]);
				homothetieCentre[1]	= (1.0f - newRapport) * (yc - homothetieCentre[1]);
				homothetieRapport	= 0.0f;
			}
			else if( homothetieRapport==0.0f ) // La composition est entre une translation et une homothetie.
			{
				homothetieCentre[0]	= (xc * (1.0f - newRapport) + newRapport * homothetieCentre[0]) / (1.0f - newRapport);
				homothetieCentre[1]	= (yc * (1.0f - newRapport) + newRapport * homothetieCentre[1]) / (1.0f - newRapport);
				homothetieRapport	= newRapport;
			}
			else // Cas général : 
			{
				homothetieCentre[0]	= (a * homothetieCentre[0] + b * xc)/c;
				homothetieCentre[1]	= (a * homothetieCentre[1] + b * yc)/c;
				homothetieRapport 	= homothetieRapport * newRapport;
			}

			/*std::cout << "After : " << std::endl;
			std::cout << "    hx = " << homothetieCentre[0] << "; hy = " << homothetieCentre[1] << std::endl;
			std::cout << "    r  = " << homothetieRapport << std::endl;*/
		}

		ViewLink* GLSceneWidget::getObjectIDUnder(int x, int y)
		{
			// Clear to black first : 
			glClearColor( 0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			// Draw the scene : 
			drawScene(true);
			
			// Set the read buffer : 
			glReadBuffer( GL_BACK );

			unsigned char id = 0;

			// Subtle point here : the frame buffer is verticaly flipped!
			glReadPixels(x, height()-(y+1), 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

			int rid = id;
			rid--;

			#ifdef __VERBOSE__
				std::cout << "Picking id = " << rid << " under x=" << x << " y=" << y << std::endl;
			#endif

			if(rid<0 || rid>=links.size())
				return NULL;
			else
				return links[rid];
		}

	// Actions (for the contextual menu): 
		void GLSceneWidget::updateContextMenu(void)
		{
			// Per actions : 
			if(links.empty())
				selectAllAction->setEnabled(false);
			else
				selectAllAction->setEnabled(true);

			if(displayList.empty())
				selectAllVisibleAction->setEnabled(false);
			else
				selectAllVisibleAction->setEnabled(true);
	
			if(selectionList.empty())
			{
				resetSelectedAngleAction->setEnabled(false);
				resetSelectedScaleAction->setEnabled(false);
				resetSelectedPositionAction->setEnabled(false);
				resetSelectionAction->setEnabled(false);
				hideSelectedAction->setEnabled(false);
				closeSelectedAction->setEnabled(false);
			}
			else
			{
				resetSelectedAngleAction->setEnabled(true);
				resetSelectedScaleAction->setEnabled(true);
				resetSelectedPositionAction->setEnabled(true);
				resetSelectionAction->setEnabled(true);

				if(displayList.empty())
					hideSelectedAction->setEnabled(false);
				else
					hideSelectedAction->setEnabled(true);

				closeSelectedAction->setEnabled(true);
			}

			if(displayList.empty())
				hideAllAction->setEnabled(false);
			else
				hideAllAction->setEnabled(true);

			if(links.size()==displayList.size())
				showAllAction->setEnabled(false);
			else
				showAllAction->setEnabled(true);

			if(handMode)
				handModeAction->setChecked(true);
			else
				handModeAction->setChecked(false);

			if(fullscreenModeEnabled)
				toggleFullscreenAction->setChecked(true);
			else
				toggleFullscreenAction->setChecked(false);
		}

		void GLSceneWidget::selectAll(void)
		{
			selectionList = links;
		}

		void GLSceneWidget::selectAllVisible(void)
		{
			selectionList.clear();
			for(std::list<ViewLink*>::iterator it=displayList.begin(); it!=displayList.end(); it++)
				selectionList.push_back(*it);
		}

		void GLSceneWidget::hideAll(void)
		{
			std::list<ViewLink*> copyDisplayList = displayList;

			for(std::list<ViewLink*>::iterator it=copyDisplayList.begin(); it!=copyDisplayList.end(); ++it)
				hideView(*it);
		}

		void GLSceneWidget::showAll(void)
		{
			for(std::vector<ViewLink*>::iterator it=links.begin(); it!=links.end(); it++)
			{
				if(!viewIsVisible(*it))
					pushBackView(*it);
			}
		}

		void GLSceneWidget::hideCurrentSelection(void)
		{
			for(std::vector<ViewLink*>::iterator it=selectionList.begin(); it!=selectionList.end(); it++)
				hideView(*it);
		}

		void GLSceneWidget::closeSelection(void)
		{
			while(!selectionList.empty())
				removeView( selectionList.back(), true );
		}

		void GLSceneWidget::resetSelectionAngle(void)
		{
			for(std::vector<ViewLink*>::iterator it=selectionList.begin(); it!=selectionList.end(); it++)
				(*it)->angleRadians = 0.0;
		}

		void GLSceneWidget::resetSelectionScale(void)
		{
			for(std::vector<ViewLink*>::iterator it=selectionList.begin(); it!=selectionList.end(); it++)
				(*it)->scale = 1.0;
		}

		void GLSceneWidget::resetSelectionPosition(void)
		{
			for(std::vector<ViewLink*>::iterator it=selectionList.begin(); it!=selectionList.end(); it++)
			{
				(*it)->centerCoords[0] = 0.0;
				(*it)->centerCoords[1] = 0.0;
			}
		}

		void GLSceneWidget::resetSelection(void)
		{
			resetSelectionAngle();
			resetSelectionScale();
			resetSelectionPosition();
		}

		void GLSceneWidget::resetGlobalPosition(void)
		{
			screenCenter[0]		= 0.0f;
			screenCenter[1]		= 0.0f;
		}

		void GLSceneWidget::resetGlobalZoom(void)
		{
			homothetieCentre[0]	= 0.0f;
			homothetieCentre[1]	= 0.0f;
			homothetieRapport	= 1.0f;
		}

		void GLSceneWidget::resetGlobal(void)
		{
			resetGlobalPosition();
			resetGlobalZoom();
		}

		void GLSceneWidget::switchSelectionMode(void)
		{
			handMode = !handMode;

			if(handMode && leftClick)
				setCursor(Qt::ClosedHandCursor);
			else if(handMode)
				setCursor(Qt::OpenHandCursor);
			else
				setCursor(Qt::ArrowCursor);
		}

		void GLSceneWidget::setFullscreenMode(bool enabled)
		{
			if(enabled!=fullscreenModeEnabled && !displayList.empty()) // Do not toggle (or save toggle) if empty
			{
				makeCurrent();

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
					emit requireContainerCatch();
				}

				fullscreenModeEnabled = enabled;
			}
		}

		void GLSceneWidget::toggleFullscreenMode(void)
		{
			setFullscreenMode(!fullscreenModeEnabled);
		}

	// Main mechanics : 
		void GLSceneWidget::updateScene(void)
		{
			paintGL();
		}

		ViewLink* GLSceneWidget::createView(void)
		{
			ViewLink* link = new ViewLink(this);

			// Lists : 
			links.push_back(link);

			return link;
		}

		bool GLSceneWidget::viewExists(ViewLink* view, bool throwException)
		{
			int id = getViewID(view);

			if(id==-1)
			{
				if(throwException)
					throw Exception("GLSceneWidget::viewExists - View does not exist.", __FILE__, __LINE__);
				else
					return false;
			}
			else
				return true;
		}

		void GLSceneWidget::bringUpView(ViewLink* view)
		{
			viewExists(view, true);
			hideView(view);
			displayList.push_back(view);
		}

		void GLSceneWidget::pushBackView(ViewLink* view)
		{
			viewExists(view, true);
			hideView(view);
			displayList.push_front(view);
		}

		void GLSceneWidget::hideView(ViewLink* view)
		{
			std::list<ViewLink*>::iterator it = std::find(displayList.begin(), displayList.end(), view);

			if(it!=displayList.end())
				displayList.erase(it);
		}

		void GLSceneWidget::unselectView(ViewLink* view)
		{
			std::vector<ViewLink*>::iterator it = std::find(selectionList.begin(), selectionList.end(), view);

			if(it!=selectionList.end())
				selectionList.erase(it);
		}

		void GLSceneWidget::selectView(ViewLink* view, bool dropCurrentSelection)
		{
			viewExists(view, true);

			if(dropCurrentSelection)
				selectionList.clear();
			else
				unselectView(view);

			selectionList.push_back(view);
		}

		bool GLSceneWidget::viewIsVisible(const ViewLink* view) const
		{
			std::list<ViewLink*>::const_iterator it = std::find(displayList.begin(), displayList.end(), view);
			
			return it!=displayList.end();
		}

		bool GLSceneWidget::viewIsSelected(const ViewLink* view) const
		{
			std::vector<ViewLink*>::const_iterator it = std::find(selectionList.begin(), selectionList.end(), view);

			return it!=selectionList.end();
		}

		void GLSceneWidget::removeView(ViewLink* view, bool sendSignal)
		{
			viewExists(view, true);
			
			unselectView(view);
			hideView(view);

			std::vector<ViewLink*>::iterator it = std::find(links.begin(), links.end(), view);
			(*it)->scene = NULL;
			links.erase(it);

			if(sendSignal)
				emit view->closed();
			else
				updateScene();		// If this doesn't have to send the signal, it means that the request was external and, thus, it needs to update the display.
		}

		ViewManager* GLSceneWidget::createManager(void)
		{
			ViewManager* manager = new ViewManager(this);

			// Lists : 
			managers.push_back(manager);

			return manager;
		}

		void GLSceneWidget::removeManager(ViewManager* manager, bool sendSignal)
		{
			std::vector<ViewManager*>::iterator it = std::find(managers.begin(), managers.end(), manager);

			if(it!=managers.end())
			{
				(*it)->scene = NULL;

				managers.erase(it);

				if(sendSignal)
					emit manager->closed();
			}
		}

	// Enable/Disable/Set keys :
		bool GLSceneWidget::isKeyboardActionsEnabled(void) const
		{
			return keyboardMovementsEnabled;
		}

		void GLSceneWidget::setKeyboardActions(bool enabled)
		{
			keyboardMovementsEnabled = enabled;

			// Activate focus via mouse click :
			if(keyboardMovementsEnabled)
				setFocusPolicy(Qt::ClickFocus);
			else
				setFocusPolicy(Qt::NoFocus);
		}

		bool GLSceneWidget::isMouseActionsEnabled(void) const
		{
			return mouseMovementsEnabled;
		}

		void GLSceneWidget::setMouseActions(bool enabled)
		{
			mouseMovementsEnabled = enabled;
		}

		void GLSceneWidget::setKeyForAction(const GLSceneWidget::KeyAction& action, const QKeySequence& key)
		{
			if(action<0 || action>=NumActionKey)
				throw Exception("GLSceneWidget::setKeyForAction - Action id = " + to_string(static_cast<int>(action)) + " is out of bounds.", __FILE__, __LINE__);
			else
			{
				keyAssociation[static_cast<int>(action)] = key;

				// Special notifications : 
				switch(action)
				{
					case KeyResetView:		resetGlobalAction->setShortcut( key );		break;
					case KeyCloseView:		closeSelectedAction->setShortcut( key );	break;
					case KeySelectAll:		selectAllAction->setShortcut( key );		break;
					case KeyToggleHandMode:		handModeAction->setShortcut( key );		break;
					case KeyToggleFullscreen:	toggleFullscreenAction->setShortcut( key );	break;
					default:									break;
				}
			}
		}

		void GLSceneWidget::removeKeyForAction(const GLSceneWidget::KeyAction& action)
		{
			setKeyForAction(action, QKeySequence());
		}

	// Other settings : 
		void GLSceneWidget::setClearColor(float red, float green, float blue)
		{
			clearColorRed	= red;
			clearColorGreen	= green;
			clearColorBlue	= blue;
		}

	// Temporary : 
		/*void GLSceneWidget::reloadPlacementShader(void)
		{
			delete placementProgram;
			placementProgram = NULL;

			try
			{
				ShaderSource 	sourceVertex("./placementShaderVertex.glsl"),
						sourcePixel("./placementShaderPixel.glsl");

				HdlShader 	vertexShader(GL_VERTEX_SHADER, sourceVertex),
						pixelShader(GL_FRAGMENT_SHADER, sourcePixel);

				placementProgram	= new HdlProgram( vertexShader, pixelShader);
			}
			catch(Exception& e)
			{
				std::cerr << "Error while replacing Placement Shader : " << std::endl;
				std::cerr << e.what() << std::endl;

				delete placementProgram;
				placementProgram = NULL;

				throw e;
			}
		}*/

// GLSceneWidgetContainer
	GLSceneWidgetContainer::GLSceneWidgetContainer(int width, int height, QWidget* parent)
	 : QWidget(parent), container(this), scene(width, height, this)
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setMinimumSize(QSize(256,256));

		container.addWidget(&scene);

		QObject::connect(&scene, SIGNAL(requireContainerCatch(void)), 	this, SLOT(handleCatch(void)));
	}

	GLSceneWidgetContainer::~GLSceneWidgetContainer(void)
	{ }

	GLSceneWidget& GLSceneWidgetContainer::sceneWidget(void)
	{
		return scene;
	}

	void GLSceneWidgetContainer::handleCatch(void)
	{
		container.removeWidget(&scene);
		container.addWidget(&scene);
	}

