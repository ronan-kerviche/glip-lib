/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ResourceLoader.cpp                                                                        */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for visualization with OpenGL.                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "GLSceneWidget.hpp"

// QVGLView :
	QVGLView::QVGLView(HdlTexture* _texture, const QString& _name)
	 : 	texture(_texture),
		name(_name),
		qvglParent(NULL),
		angle(0.0f),
		homothecyScale(1.0f),
		flipUpDown(false),
		flipLeftRight(false)
	{
		reset();

		if(texture==NULL)
			throw Exception("QVGLView::QVGLView - Texture pointer is NULL.", __FILE__, __LINE__);
	}

	QVGLView::~QVGLView(void)
	{
		emit closed();
	}

	void QVGLView::prepareToDraw(void)
	{
		texture->bind();
	}

	const __ReadOnly_HdlTextureFormat& QVGLView::getFormat(void) const
	{
		return *texture; // TODO : fix the return temporary warning.
	}

	const QString& QVGLView::getName(void) const
	{
		return name;
	}

	void QVGLView::setName(const QString& newName)
	{
		name = newName;

		emit nameChanged();
	}

	float QVGLView::getAngle(void) const
	{
		return angle;
	}

	void QVGLView::setAngle(const float& a)
	{
		angle = a;
		
		emit updated();
	}

	void QVGLView::rotate(const float& a)
	{
		angle += a;

		emit updated();
	}

	void QVGLView::getViewCenter(float& x, float& y) const
	{
		x = viewCenter[0];
		y = viewCenter[1];
	}

	void QVGLView::setViewCenter(const float& x, const float& y)
	{
		viewCenter[0] = x;
		viewCenter[1] = y;

		emit updated();
	}

	void QVGLView::move(const float& x, const float& y)
	{
		viewCenter[0] += x;
		viewCenter[1] += y;

		emit updated();
	}

	bool QVGLView::isMirrored(void) const
	{
		return flipLeftRight;
	}

	void QVGLView::setMirror(bool enabled)
	{
		flipLeftRight = enabled;

		emit updated();
	}

	bool QVGLView::isUpsideDown(void) const
	{
		return flipUpDown;	
	}

	void QVGLView::setUpsideDown(bool enabled)
	{
		flipUpDown = enabled;

		emit updated();
	}

	void QVGLView::zoom(const float& xCenter, const float& yCenter, const float& factor)
	{
		// factor > 1.0 => zoom in
		float 	a		= homothecyScale * factor - factor,
			b		= factor - 1.0f,
			c		= homothecyScale * factor - 1.0f;

		if(std::abs(c) <= 100.0f * std::numeric_limits<float>::epsilon()) // This composition is a translation
		{
			homothecyCenter[0]	= (1.0f - factor) * (xCenter - homothecyCenter[0]);
			homothecyCenter[1]	= (1.0f - factor) * (yCenter - homothecyCenter[1]);
			homothecyScale		= 0.0f;
		}
		else if( homothecyScale==0.0f ) // From a translation to a homothecy
		{
			homothecyCenter[0]	= (xCenter * (1.0f - factor) + factor * homothecyCenter[0]) / (1.0f - factor);
			homothecyCenter[1]	= (yCenter * (1.0f - factor) + factor * homothecyCenter[1]) / (1.0f - factor);
			homothecyScale		= factor;
		}
		else // CGeneric case
		{
			homothecyCenter[0]	= (a * homothecyCenter[0] + b * xCenter)/c;
			homothecyCenter[1]	= (a * homothecyCenter[1] + b * yCenter)/c;
			homothecyScale	 	= homothecyScale * factor;
		}

		emit updated();
	}

	void QVGLView::reset(void)
	{
		angle			= 0.0f;
		homothecyScale		= 1.0f;
		flipUpDown		= false;
		flipLeftRight		= false;
		viewCenter[0]		= 0.0f;
		viewCenter[1]	 	= 0.0f;
		homothecyCenter[0]	= 0.0f;
		homothecyCenter[1]	= 0.0f;

		emit updated();
	}

	void QVGLView::show(void)
	{
		emit requireDisplay();
	}

	void QVGLView::close(void)
	{
		emit closed();
	}

	bool QVGLView::isClosed(void)
	{
		return qvglParent==NULL;
	}

	float QVGLView::getImageRatio(void) const
	{
		if(texture==NULL)
			return 0.0f;
		else
			return static_cast<float>(texture->getWidth()) / static_cast<float>(texture->getHeight());
	}
	
	void QVGLView::getAspectRatioScaling(float& xImgScale, float& yImgScale) const
	{
		const float imgRatio = getImageRatio();

		xImgScale = std::min(1.0f, imgRatio);
		yImgScale = std::min(1.0f, 1.0f/imgRatio);

		if(flipLeftRight)	xImgScale *= -1.0f;
		if(flipUpDown) 		yImgScale *= -1.0f;
	}

	QString QVGLView::getSizeString(void) const
	{
		if(texture!=NULL)
		{
			const unsigned int sizeInBytes = texture->getSize();

			const float	GB = 1024.0f*1024.0f*1024.0f,
					MB = 1024.0f*1024.0f,
					KB = 1024.0f;

			if(sizeInBytes>=GB)
				return tr("%1 GB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(sizeInBytes)/GB)) );
			else if(sizeInBytes>=MB)
				return tr("%1 MB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(sizeInBytes)/MB)) );
			else if(sizeInBytes>=KB)
				return tr("%1 KB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(sizeInBytes)/KB)) );
			else 
				return tr("%1 B").arg( sizeInBytes );
		}
		else
			return "0 B";
	}

// QVGLSubWidget :
	QVGLSubWidget::QVGLSubWidget(QWidget* parent)
	 : 	QWidget(parent),
		layout(this),
		titleLabel(this),
		hideButton(this),
		invalidMotion(true),
		qvglParent(NULL),
		graphicsProxy(NULL)
	{
		// But first : 
		QWidget::hide();

		// Activate focus via mouse click :
		setFocusPolicy(Qt::ClickFocus);

		// Create title bar : 
		titleLabel.setOpenExternalLinks(false);
		setTitle("Title");

		// Buttons : 
		const int 	w = 32,
				h = 16;
		hideButton.setMinimumSize(w, h);
		hideButton.setMaximumSize(w, h);

		titleBar.addWidget(&titleLabel);
		titleBar.addWidget(&hideButton);
		titleBar.setMargin(0);
		titleBar.setSpacing(0);

		// Create final layout : 
		layout.addLayout(&titleBar);
		layout.setMargin(1);
		layout.setSpacing(1);

		// Connections : 
		QObject::connect(&hideButton, SIGNAL(released(void)), this, SLOT(hide(void)));

		// Background color : 
		/*QPalette pal;
		QBrush brush(Qt::NoBrush);
		QColor color(Qt::transparent);
		pal.setBrush(QPalette::Window, brush);
		pal.setColor(QPalette::Window, color);
		setPalette(pal);*/
	}

	QVGLSubWidget::~QVGLSubWidget(void)
	{ }

	void QVGLSubWidget::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
		{
			event->accept(); // Do not propagate the event hereafter
			
			offset = event->pos();

			invalidMotion = false;
		}

		//std::cout << "GLSubWidget : Mouse click event" << std::endl;

		// Send selection signal : 
		emit selected(this);
	}

	void QVGLSubWidget::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted() && !invalidMotion && qvglParent!=NULL)
		{
			event->accept(); // Do not propagate the event hereafter.

			// Get the new position : 
			QPoint v = mapToParent(event->pos() - offset);

			// Prevent the widget from moving out of the scene : 
			v.setX( std::min(static_cast<int>(qvglParent->sceneRect().width()-width()), v.x()) );
			v.setY( std::min(static_cast<int>(qvglParent->sceneRect().height()-titleBar.geometry().height()), v.y()) );

			v.setX( std::max(0, v.x()) );
			v.setY( std::max(0, v.y()) );
			
			// Move : 
			move(v);
		}

		//std::cout << "GLSubWidget : Mouse move event" << std::endl;
	}

	void QVGLSubWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted())
		{
			event->accept(); // Do not propagate the event hereafter.
			
			invalidMotion = true;
		}
	}

	void QVGLSubWidget::setLayout(QLayout* subLayout)
	{
		if(layout.count()>=2)
			layout.takeAt(1);
		
		layout.addLayout(subLayout);
	}

	QString QVGLSubWidget::getTitle(void)
	{
		return titleLabel.text();
	}

	void QVGLSubWidget::setTitle(QString title)
	{
		// Do not put rich text!
		title.remove(QRegExp("<[^>]*>"));

		titleLabel.setText(title);
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		emit titleChanged();
	}

	QVGLWidget* QVGLSubWidget::getQVGLParent(void)
	{
		return qvglParent;
	}

	void QVGLSubWidget::show(void)
	{
		QWidget::show();

		emit showRequest(this);
	}

	void QVGLSubWidget::hide(void)
	{
		QWidget::hide();

		emit hideRequest(this);
	}

// QVGLTopBar
	QVGLTopBar::QVGLTopBar(void)
	 : 	graphicsProxy(NULL),
		bar(this),
		mainMenuButton("Menu"),
		viewsMenuButton("Views"),
		widgetsMenuButton("Widgets"),
		mainMenu("Menu", this),
		viewsMenu("Views", this),
		widgetsMenu("Widgets", this),
		hideAllWidgets("Hide all widgets", this),
		closeAllWidgets("Close all widgets", this)
	{
		// Activate focus via mouse click :
		setFocusPolicy(Qt::ClickFocus);

		// Menus : 
		mainMenuButton.setMenu(&mainMenu);
		viewsMenuButton.setMenu(&viewsMenu);
		widgetsMenuButton.setMenu(&widgetsMenu);

		mainMenuButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		viewsMenuButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		widgetsMenuButton.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		bar.addWidget(&mainMenuButton);
		bar.addWidget(&viewsMenuButton);
		bar.addWidget(&widgetsMenuButton);
		bar.addWidget(&titleLabel);

		bar.setMargin(1);
		bar.setSpacing(1);

		// Signals connections : 
		QObject::connect(&viewsSignalMapper, 	SIGNAL(mapped(QObject*)), this, SLOT(castViewPointer(QObject*)));
		QObject::connect(&widgetsSignalMapper,	SIGNAL(mapped(QObject*)), this, SLOT(castSubWidgetPointer(QObject*)));

		// Reset : 
		setTitle();
		updateViewsList(QList<QVGLView*>());
		updateSubWidgetsList(QList<QVGLSubWidget*>());

		// TEST :
		mainMenu.addAction("Action 1");
		mainMenu.addAction("Action 2");
		mainMenu.addAction("Action 3");
	}

	QVGLTopBar::~QVGLTopBar(void)
	{ }

	void QVGLTopBar::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		emit selected(this);
	}

	void QVGLTopBar::stretch(const QRectF& rect)
	{
		setGeometry(0, 0, rect.width(), height());
	}

	void QVGLTopBar::castViewPointer(QObject* ptr)
	{
		emit changeViewRequest(reinterpret_cast<QVGLView*>(ptr));
	}

	void QVGLTopBar::castSubWidgetPointer(QObject* ptr)
	{
		emit showSubWidgetRequest(reinterpret_cast<QVGLSubWidget*>(ptr));
	}

	void QVGLTopBar::setTitle(void)
	{
		titleLabel.setText("(No Title)");
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// No tooltip : 
		titleLabel.setToolTip("");
	}

	void QVGLTopBar::setTitle(QString title)
	{
		// Do not put rich text!
		title.remove(QRegExp("<[^>]*>"));

		titleLabel.setText(title);
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// No tooltip : 
		titleLabel.setToolTip("");
	}

	void QVGLTopBar::setTitle(const QVGLView& view)
	{
		// Set title : 
		titleLabel.setText(view.getName());
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// Set tooltip info :
		QString toolTip;

		toolTip += "<table>";
			toolTip += tr("<tr><td><i>Size</i></td><td>:</td><td>%1x%2 (%3)</td></tr>").arg(view.getFormat().getWidth()).arg(view.getFormat().getHeight()).arg(view.getSizeString());
			toolTip += tr("<tr><td><i>Mode</i></td><td>:</td><td>%1</td></tr>").arg(glParamName( view.getFormat().getGLMode() ).c_str());
			toolTip += tr("<tr><td><i>Depth</i></td><td>:</td><td>%1</td></tr>").arg(glParamName( view.getFormat().getGLDepth() ).c_str());
			toolTip += tr("<tr><td><i>Filtering</i></td><td>:</td><td>%1 / %2</td></tr>").arg(glParamName( view.getFormat().getMinFilter() ).c_str()).arg(glParamName( view.getFormat().getMagFilter() ).c_str());
			toolTip += tr("<tr><td><i>Wrapping</i></td><td>:</td><td>%1 / %2</td></tr>").arg(glParamName( view.getFormat().getSWrapping() ).c_str()).arg(glParamName( view.getFormat().getTWrapping() ).c_str());
			toolTip += tr("<tr><td><i>Mipmap</i></td><td>:</td><td>%1 / %2</td></tr>").arg(view.getFormat().getBaseLevel()).arg(view.getFormat().getMaxLevel());
		toolTip += "</table>"; 

		titleLabel.setToolTip(toolTip);
	}

	void QVGLTopBar::updateViewsList(const QList<QVGLView*>& viewsList)
	{
		// Clear previous list : 
		viewsMenu.clear();

		if(!viewsList.isEmpty())
		{
			for(QList<QVGLView*>::const_iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			{
				// Create the action and set the mapping : 	
				QAction* tmpAction = viewsMenu.addAction((*it)->getName(), &viewsSignalMapper, SLOT(map()));
				viewsSignalMapper.setMapping(tmpAction, reinterpret_cast<QObject*>(*it));
			}
		}
		else
			viewsMenu.addAction("(No Views)");
	}

	void QVGLTopBar::updateSubWidgetsList(const QList<QVGLSubWidget*>& subWidgetsList)
	{
		// Clear the previous list : 
		widgetsMenu.clear();

		if(!subWidgetsList.isEmpty())
		{
			// Add the default action : 
			widgetsMenu.addAction(&hideAllWidgets);
			widgetsMenu.addAction(&closeAllWidgets);
			hideAllWidgets.setEnabled(true);
			closeAllWidgets.setEnabled(true);

			widgetsMenu.addSeparator();

			for(QList<QVGLSubWidget*>::const_iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			{
				// Create the action and set the mapping : 	
				QAction* tmpAction = widgetsMenu.addAction((*it)->getTitle(), &widgetsSignalMapper, SLOT(map()));
				widgetsSignalMapper.setMapping(tmpAction, reinterpret_cast<QObject*>(*it));
			}
		}
		else
		{
			// Add the default action : 
			widgetsMenu.addAction(&hideAllWidgets);
			widgetsMenu.addAction(&closeAllWidgets);
			hideAllWidgets.setEnabled(false);
			closeAllWidgets.setEnabled(false);
		}
	}

	void QVGLTopBar::setWindowOpacity(qreal level)
	{
		QWidget::setWindowOpacity(level);
		mainMenu.setWindowOpacity(level);
		viewsMenu.setWindowOpacity(level);
		widgetsMenu.setWindowOpacity(level);
	}

// QVGLContextWidget :
	QVGLContextWidget::QVGLContextWidget(QGLContext* ctx, QWidget* parent)
	 : 	QGLWidget(ctx, parent),
		glipOwnership(false),
		clearColorRed(0.1f),
		clearColorGreen(0.1f),
		clearColorBlue(0.1f)
	{
		try
		{
			makeCurrent();

			// Glip-lib specifics : 
			if(!HandleOpenGL::isInitialized())
			{
				glipOwnership = true;
				HandleOpenGL::init();
			}
		}
		catch(Exception& e)
		{
			// Show Error : 
			std::cerr << "GLSceneWidget::GLSceneWidget - Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;

			QMessageBox errorBox(QMessageBox::Critical, "QVGLContextWidget::QVGLContextWidget", "An error occurred during initialization.", QMessageBox::Ok);
			errorBox.setDetailedText(e.what());
			errorBox.exec();

			// re-throw :
			throw e;
		}
		
		/*setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_OpaquePaintEvent);*/
	}

	QVGLContextWidget::~QVGLContextWidget(void)
	{
		if(glipOwnership)
			HandleOpenGL::deinit();
	}

	void QVGLContextWidget::initializeGL(void)
	{
		makeCurrent();
		glViewport(0, 0, width(), height());
		setAutoBufferSwap(false);
	}

	void QVGLContextWidget::resizeGL(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

// QVGLKeyboardState :
	QVGLKeyboardState::QVGLKeyboardState(void)
	{
		for(int k=0; k<QVGLNumActions; k++)
			actionPressed[k] = false;

		resetActionsKeySequences();
	}

	QVGLKeyboardState::~QVGLKeyboardState(void)
	{ }

	void QVGLKeyboardState::keyPressed(QKeyEvent* event)
	{
		QVGLActionID a = getActionAssociatedToKey(event);

		if(a!=QVGLNoAction)
		{
			event->accept();
			actionPressed[a] = true;

			emit actionReceived(a);
		}
	}

	void QVGLKeyboardState::keyReleased(QKeyEvent* event)
	{
		QVGLActionID a = getActionAssociatedToKey(event);

		if(a!=QVGLNoAction)
		{
			event->accept();
			actionPressed[a] = false;
		}
	}

	QVGLActionID QVGLKeyboardState::getActionAssociatedToKey(const QKeySequence& keySequence) const
	{
		if(keysActionsAssociations.contains(keySequence))
			return keysActionsAssociations[keySequence];
		else
			return QVGLNoAction;
	}

	QVGLActionID QVGLKeyboardState::getActionAssociatedToKey(const QKeyEvent* event) const
	{
		const int s = (event->key() | event->modifiers());

		return getActionAssociatedToKey(QKeySequence(s));
	}

	QKeySequence QVGLKeyboardState::getKeysAssociatedToAction(const QVGLActionID& a)
	{
		bool addComa = false;
		QString keysString;

		for(QMap<QKeySequence, QVGLActionID>::iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
		{
			if(it.value()==a)
			{
				if(addComa)
					keysString += ", ";

				keysString += it.key().toString();
				addComa = true;
			}
		}
	
		return QKeySequence(keysString);
	}

	void QVGLKeyboardState::setActionKeySequence(const QVGLActionID& a, const QKeySequence& keySequence)
	{
		// Remove all the previous sequences associated with the action : 
		QList<QKeySequence> previousKeySequence = keysActionsAssociations.keys(a);

		for(QList<QKeySequence>::iterator it=previousKeySequence.begin(); it!=previousKeySequence.end(); it++)
			keysActionsAssociations.remove(*it);

		// Insert the sequences separately : 
		for(int k=0; k<keySequence.count(); k++)
		{
			QKeySequence ks(keySequence[k]);

			// Remove all the sequences interfering with other actions :
			keysActionsAssociations.remove(ks);

			// Insert the new association : 
			keysActionsAssociations[ks] = a;
		}
	}

	void QVGLKeyboardState::resetActionsKeySequences(void)
	{
		setActionKeySequence(QVGLActionUp,				Qt::Key_Up);
		setActionKeySequence(QVGLActionDown,				Qt::Key_Down);
		setActionKeySequence(QVGLActionLeft,				Qt::Key_Left);
		setActionKeySequence(QVGLActionRight,				Qt::Key_Right);
		setActionKeySequence(QVGLActionZoomIn,				QKeySequence(Qt::Key_Plus, Qt::SHIFT + Qt::Key_Plus, Qt::KeypadModifier + Qt::Key_Plus));	// Support for keypad and shifts.
		setActionKeySequence(QVGLActionZoomOut,				QKeySequence(Qt::Key_Minus, Qt::SHIFT + Qt::Key_Minus, Qt::KeypadModifier + Qt::Key_Minus));	// Support for keypad and shifts.
		setActionKeySequence(QVGLActionRotationClockWise,		Qt::Key_F);
		setActionKeySequence(QVGLActionRotationCounterClockWise,	Qt::Key_D);
		setActionKeySequence(QVGLActionToggleFullscreen,		Qt::Key_Return);
		setActionKeySequence(QVGLActionExitFullscreen,			Qt::Key_Escape);
		setActionKeySequence(QVGLActionResetView,			Qt::Key_Space);
		setActionKeySequence(QVGLActionPreviousView,			QKeySequence(Qt::CTRL + Qt::Key_Left));
		setActionKeySequence(QVGLActionNextView,			QKeySequence(Qt::CTRL + Qt::Key_Right));
		setActionKeySequence(QVGLActionCloseView,			Qt::Key_Delete);
		setActionKeySequence(QVGLActionCloseAllViews,			QKeySequence(Qt::SHIFT + Qt::Key_Delete));
		setActionKeySequence(QVGLActionMotionModifier,			QKeySequence(Qt::CTRL + Qt::Key_Control, Qt::Key_Control)); 	// The first correspond the press event, the second to the release.
		setActionKeySequence(QVGLActionRotationModifier,		QKeySequence(Qt::SHIFT + Qt::Key_Shift, Qt::Key_Shift));	// (the same)
	}

// QVGLMouseState :
	QVGLMouseState::QVGLMouseState(void)
	 :	functionMode(ModeMotion),
		wheelDelta(0.0f)
	{
		// Create the maps : 
		#define ADD_VECTOR( ID ) vectors.insert( ID , QPair<DataStatus, QPointF>(NotModified, QPointF(0.0, 0.0)));
			ADD_VECTOR( VectorLastLeftClick )
			ADD_VECTOR( VectorLastLeftClickGl )
			ADD_VECTOR( VectorLastLeftClickQuad )
			ADD_VECTOR( VectorLastLeftClickImage )

			ADD_VECTOR( VectorLastLeftPosition )
			ADD_VECTOR( VectorLastLeftPositionGl )
			ADD_VECTOR( VectorLastLeftPositionQuad )
			ADD_VECTOR( VectorLastLeftPositionImage )

			ADD_VECTOR( VectorLastLeftShift )
			ADD_VECTOR( VectorLastLeftShiftGl )
			ADD_VECTOR( VectorLastLeftShiftQuad )
			ADD_VECTOR( VectorLastLeftShiftImage )

			ADD_VECTOR( VectorLastLeftRelease )
			ADD_VECTOR( VectorLastLeftReleaseGl )
			ADD_VECTOR( VectorLastLeftReleaseQuad )
			ADD_VECTOR( VectorLastLeftReleaseImage )

			ADD_VECTOR( VectorLastLeftCompletedVector )
			ADD_VECTOR( VectorLastLeftCompletedVectorGl )
			ADD_VECTOR( VectorLastLeftCompletedVectorQuad )
			ADD_VECTOR( VectorLastLeftCompletedVectorImage )

			ADD_VECTOR( VectorLastRightClick )
			ADD_VECTOR( VectorLastRightClickGl )
			ADD_VECTOR( VectorLastRightClickQuad )
			ADD_VECTOR( VectorLastRightClickImage )

			ADD_VECTOR( VectorLastRightPosition )
			ADD_VECTOR( VectorLastRightPositionGl )
			ADD_VECTOR( VectorLastRightPositionQuad )
			ADD_VECTOR( VectorLastRightPositionImage )

			ADD_VECTOR( VectorLastRightShift )
			ADD_VECTOR( VectorLastRightShiftGl )
			ADD_VECTOR( VectorLastRightShiftQuad	)
			ADD_VECTOR( VectorLastRightShiftImage )

			ADD_VECTOR( VectorLastRightRelease )
			ADD_VECTOR( VectorLastRightReleaseGl )
			ADD_VECTOR( VectorLastRightReleaseQuad )
			ADD_VECTOR( VectorLastRightReleaseImage )

			ADD_VECTOR( VectorLastRightCompletedVector )
			ADD_VECTOR( VectorLastRightCompletedVectorGl )
			ADD_VECTOR( VectorLastRightCompletedVectorQuad )
			ADD_VECTOR( VectorLastRightCompletedVectorImage )

			ADD_VECTOR( VectorLastWheelUp )
			ADD_VECTOR( VectorLastWheelUpGl )
			ADD_VECTOR( VectorLastWheelUpQuad )
			ADD_VECTOR( VectorLastWheelUpImage )

			ADD_VECTOR( VectorLastWheelDown )
			ADD_VECTOR( VectorLastWheelDownGl )
			ADD_VECTOR( VectorLastWheelDownQuad )
			ADD_VECTOR( VectorLastWheelDownImage )
		#undef ADD_VECTOR

		#define ADD_COLOR( ID ) colors.insert( ID , QPair<DataStatus, QColor>(NotModified, QColor(Qt::black)));
			ADD_COLOR( ColorUnderLastLeftClick )
			ADD_COLOR( ColorUnderLastLeftPosition )
			ADD_COLOR( ColorUnderLastLeftRelease )
			ADD_COLOR( ColorUnderLastRightClick )
			ADD_COLOR( ColorUnderLastRightPosition )
			ADD_COLOR( ColorUnderLastRightRelease )
		#undef ADD_COLOR

		vectorIDs 	= vectors.keys();
		colorIDs 	= colors.keys();
	}

	QVGLMouseState::~QVGLMouseState(void)
	{ }
	
	const QPointF& QVGLMouseState::invisibleGetVector(const VectorID& id) const
	{
		QMap<VectorID, QPair<DataStatus, QPointF> >::const_iterator it = vectors.find(id);

		if(it!=vectors.end())
			return it->second;
		else
			throw Exception("QVGLMouseState::getVector - Unknown VectorID (invisible access).", __FILE__, __LINE__);
	}

	const QColor& QVGLMouseState::invisibleGetColor(const ColorID& id) const
	{
		QMap<ColorID,  QPair<DataStatus, QColor> >::const_iterator it = colors.find(id);

		if(it!=colors.end())
			return it->second;
		else
			throw Exception("QVGLMouseState::getColor - Unknown ColorID (invisible access).", __FILE__, __LINE__);
	}

	bool QVGLMouseState::doesVectorRequireUpdate(const VectorID& id) const
	{
		return vectors[id].first == RequireUpdate;
	}

	bool QVGLMouseState::doesColorRequirepdate(const ColorID& id) const
	{
		return colors[id].first == RequireUpdate;
	}

	void QVGLMouseState::setVector(const VectorID& id, const QPointF& v, const bool requireUpdate)
	{
		if(vectors.contains(id))
		{
			vectors[id].first 	= (requireUpdate ? RequireUpdate : Modified);
			vectors[id].second 	= v;
		}
		else
			throw Exception("QVGLMouseState::setVector - Unknown VectorID.", __FILE__, __LINE__);
	}

	void QVGLMouseState::setColor(const ColorID& id, const QColor& c)
	{
		if(colors.contains(id))
		{
			colors[id].first 	= Modified;
			colors[id].second	= c;
		}
		else
			throw Exception("QVGLMouseState::setColor - Unknown ColorID.", __FILE__, __LINE__);
	}

	void QVGLMouseState::processEvent(QGraphicsSceneWheelEvent* event)
	{
		if(event->delta()!=0 && event->orientation()==Qt::Vertical)
		{
			wheelDelta += static_cast<float>(event->delta())/(8.0f*15.0f);

			// wheelSteps 	> 0 : away of the user
			// 		< 0 : toward the user

			if(wheelDelta>0)
				setVector(VectorLastWheelUp, event->scenePos(), true);
			else
				setVector(VectorLastWheelDown, event->scenePos(), true);

			event->accept();
			emit requestExternalUpdate();
		}
	}

	void QVGLMouseState::processEvent(QGraphicsSceneMouseEvent* event, const bool clicked, const bool moved, const bool released)
	{
		// event->button()  : The button which triggered the event (empty during drag).
		// event->buttons() : The buttons currently pressed (might not contain the previous in the case of a click).

		if(event->button()==Qt::LeftButton || (event->buttons() & Qt::LeftButton)!=0)
		{	
			if(clicked)
			{
				setVector(VectorLastLeftClick, event->buttonDownScenePos(Qt::LeftButton), true);
				setVector(VectorLastLeftPosition, event->scenePos(), true);
				//setVector(VectorLastLeftShift, QPointF(0.0, 0.0), true);
			}			

			if(moved)
			{
				setVector(VectorLastLeftPosition, event->scenePos(), true);
				setVector(VectorLastLeftShift, event->scenePos() - event->lastScenePos(), true);
			}

			if(released)
			{
				setVector(VectorLastLeftRelease, event->scenePos(), true);
				setVector(VectorLastLeftCompletedVector, event->scenePos() - invisibleGetVector(VectorLastLeftClick), true);
				//setVector(VectorLastLeftShift, QPointF(0.0, 0.0), true);
			}

			if(clicked || moved || released)
				event->accept();
		}
		
		if(event->button()==Qt::RightButton || (event->buttons() & Qt::RightButton)!=0)
		{
			if(clicked)
			{
				setVector(VectorLastRightClick, event->buttonDownScenePos(Qt::RightButton), true);
				setVector(VectorLastRightPosition, event->scenePos(), true);
			}			

			if(moved)
			{
				setVector(VectorLastRightPosition, event->scenePos(), true);
				setVector(VectorLastRightShift, event->scenePos() - event->lastScenePos(), true);
			}

			if(released)
			{
				setVector(VectorLastRightRelease, event->scenePos(), true);
				setVector(VectorLastRightCompletedVector, event->scenePos() - invisibleGetVector(VectorLastRightClick), true);
			}

			if(clicked || moved || released)
				event->accept();
		}

		// Qt::MidButton ?

		// Require the widget to update all the coordinates.
		if(event->isAccepted())
			emit requestExternalUpdate();
	}

	void QVGLMouseState::updateProcessCompleted(void)
	{
		emit updated();
	}

	void QVGLMouseState::clear(void)
	{
		for(QMap<VectorID, QPair<DataStatus, QPointF> >::Iterator it=vectors.begin(); it!=vectors.end(); it++)
		{
			it.value().first 	= NotModified;
			it.value().second 	= QPointF(0.0, 0.0);
		}

		for(QMap<ColorID, QPair<DataStatus, QColor> >::Iterator it=colors.begin(); it!=colors.end(); it++)
		{
			it.value().first 	= NotModified;
			it.value().second 	= Qt::black;
		}
	}

	const QList<QVGLMouseState::VectorID>& QVGLMouseState::getVectorIDs(void) const
	{
		return vectorIDs;
	}

	const QList<QVGLMouseState::ColorID>& QVGLMouseState::getColorIDs(void) const
	{
		return colorIDs;
	}

	bool QVGLMouseState::isVectorModified(const VectorID& id) const
	{
		return (vectors[id].first==Modified);
	}

	bool QVGLMouseState::isColorModified(const ColorID& id) const 
	{
		return (colors[id].first==Modified);
	}

	const QPointF& QVGLMouseState::getVector(const VectorID& id)
	{
		if(vectors.contains(id))
		{
			vectors[id].first = ((vectors[id].first==Modified) ? NotModified : vectors[id].first);
			return vectors[id].second;
		}
		else
			throw Exception("QVGLMouseState::getVector - Unknown VectorID.", __FILE__, __LINE__);
	}

	const QColor& QVGLMouseState::getColor(const ColorID& id)
	{
		if(colors.contains(id))
		{
			colors[id].first = ((colors[id].first==Modified) ? NotModified : colors[id].first);
			return colors[id].second;
		}
		else
			throw Exception("QVGLMouseState::getColor - Unknown ColorID.", __FILE__, __LINE__);
	}

	bool QVGLMouseState::isWheelDeltaModified(void) const
	{
		return (wheelDelta!=0.0f);
	}	
	
	float QVGLMouseState::getWheelDelta(void)
	{
		float c = wheelDelta;
		wheelDelta = 0.0f;
		return c;
	}

	const QVGLMouseState::FunctionMode& QVGLMouseState::getFunctionMode(void) const
	{
		return functionMode;
	}

	void QVGLMouseState::setFunctionMode(const FunctionMode& m)
	{
		functionMode = m;
	}

	QVGLMouseState::BasisID QVGLMouseState::getVectorBasis(const VectorID& vID)
	{
		return static_cast<QVGLMouseState::BasisID>(vID % NumBasis);
	}

	bool QVGLMouseState::isBasisRelative(const BasisID& bID)
	{
		return (bID==PixelRelativeBasis) || (bID==GlRelativeBasis) || (bID==QuadRelativeBasis) || (bID==ImageRelativeBasis);
	}

// Shader code : 
	const std::string vertexShaderSource = 		"#version 130															\n"
							"																\n"
							"in vec2 vertexInput;														\n"
							"																\n"
							"uniform vec2	imageScale 	= vec2(1.0, 1.0),										\n"
							"		sceneScale 	= vec2(1.0, 1.0),										\n"
							"		viewCenter 	= vec2(0.0, 0.0),										\n"
							"		homothecyCenter	= vec2(0.0, 0.0);										\n"
							"uniform float	angle		= 0.0,												\n"
							"		homothecyScale	= 1.0,												\n"
							"		adaptationScale = 1.0;												\n"
							"																\n"
							"void main()															\n"
							"{																\n"
							"	// Generate rotation matrix : 												\n"
							"	mat2 rot 	= mat2(	 cos(angle), 	sin(angle), 									\n"
							"				-sin(angle), 	cos(angle));									\n"
							"																\n"
							"	vec2 tmpVertex	= vertexInput;												\n"
							"																\n"
							"	// Scale : 														\n"
							"	tmpVertex.x	= tmpVertex.x * imageScale.x;										\n"
							"	tmpVertex.y	= tmpVertex.y * imageScale.y;										\n"
							"																\n"
							"	// Rotate : 														\n"
							"	tmpVertex	= rot * tmpVertex;											\n"
							"																\n"
							"	// Translate : 														\n"
							"	tmpVertex	= tmpVertex + viewCenter;										\n"
							"																\n"
							"	// Homotothecy : 													\n"
							"	if(homothecyScale==0.0f) // Translation	only										\n"
							"		tmpVertex	= tmpVertex + homothecyCenter;									\n"
							"	else // Homothecy													\n"
							"		tmpVertex	= (tmpVertex - homothecyCenter) * homothecyScale + homothecyCenter;				\n"
							"																\n"
							"	// Scene Scaling :													\n"
							"	tmpVertex.x	= tmpVertex.x * sceneScale.x;										\n"
							"	tmpVertex.y	= tmpVertex.y * sceneScale.y;										\n"
							"																\n"
							"	// Adaptation Scaling :													\n"
							"	tmpVertex	= tmpVertex * adaptationScale;										\n"
							"																\n"
							"	// Write : 														\n"
							"	gl_Position 	= vec4(tmpVertex, 0.0, 1.0);										\n"
							"																\n"
							"	gl_TexCoord[0] 	= gl_MultiTexCoord0;											\n"
							"}																\n";

	const std::string fragmentShaderSource = 	"#version 130															\n"
							"																\n"
							"uniform sampler2D	viewTexture;												\n"
							"out     vec4 		displayOutput;												\n"
							"																\n"
							"void main()															\n"
							"{																\n"
							"	// Get the input data :													\n"
							"	displayOutput = textureLod(viewTexture, gl_TexCoord[0].st, 0.0);							\n"
							"}																\n";

// QVGLSceneWidget :
	QVGLSceneWidget::QVGLSceneWidget(QVGLWidget* _qvglParent, QVGLTopBar* _topBar)
	 : 	qvglParent(_qvglParent),
		quad(NULL),
		shaderProgram(NULL),
		topBar(_topBar)
	{
		// Activate focus via mouse click :
		//setFocusPolicy(Qt::ClickFocus);

		try
		{	
			quad = new GeometryInstance(GeometryPrimitives::StandardQuad(), GL_STATIC_DRAW_ARB);

			ShaderSource 	sourceVertex(vertexShaderSource),
					sourcePixel(fragmentShaderSource);

			HdlShader 	vertexShader(GL_VERTEX_SHADER, sourceVertex),
					pixelShader(GL_FRAGMENT_SHADER, sourcePixel);

			shaderProgram	= new HdlProgram( vertexShader, pixelShader);
		}
		catch(Exception& e)
		{
			delete quad;
			delete shaderProgram;
			quad = NULL;
			shaderProgram = NULL;
	
			// re-throw :
			throw e; 
		}

		// Bars (here because of the stretch signal) : 
		if(topBar!=NULL)
		{
			topBar->graphicsProxy = addWidget(topBar);
			QObject::connect(this, SIGNAL(sceneRectChanged(const QRectF&)), topBar, SLOT(stretch(const QRectF&)));
		}
	}

	QVGLSceneWidget::~QVGLSceneWidget(void)
	{
		delete quad;
		delete shaderProgram;
	}

	void QVGLSceneWidget::drawBackground(QPainter* painter, const QRectF& rect)
	{
		HdlTexture::unbind();

		glClearColor( 0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		QVGLView* currentView = qvglParent->getCurrentView();

		if(currentView!=NULL)
		{
			shaderProgram->use();

			float 	imageScale[2],
				sceneScale[2],
				adaptationScale;

			// Get the various scales : 
			currentView->getAspectRatioScaling(imageScale[0], imageScale[1]);
			qvglParent->getSceneRatioScaling(sceneScale[0], sceneScale[1]);
			adaptationScale = qvglParent->getAdaptationScaling(currentView->getImageRatio());

			// Load the data : 
			shaderProgram->modifyVar("imageScale", 	GL_FLOAT_VEC2,	imageScale);
			shaderProgram->modifyVar("sceneScale", 		GL_FLOAT_VEC2,	sceneScale);
			shaderProgram->modifyVar("adaptationScale", 	GL_FLOAT,	adaptationScale);
			shaderProgram->modifyVar("viewCenter",		GL_FLOAT_VEC2,	currentView->viewCenter);
			shaderProgram->modifyVar("homothecyCenter",	GL_FLOAT_VEC2,	currentView->homothecyCenter);
			shaderProgram->modifyVar("angle",		GL_FLOAT,	currentView->angle);
			shaderProgram->modifyVar("homothecyScale",	GL_FLOAT,	currentView->homothecyScale);

			currentView->prepareToDraw();

			quad->draw();

			//currentView->setAngle( currentView->getAngle() + 1.57079f); //0.174f);

			//std::cout << "Image      : " << currentView->getImageRatio() << std::endl;
			//std::cout << "Scene      : " << qvglParent->getSceneRatio() << std::endl;
			//std::cout << "Adaptation : " << adaptationScale << std::endl;
			//std::cout << "Angle      : " << currentView->getAngle() << std::endl;
		}		
		//else
		//	std::cout << "Nothing to draw!" << std::endl;

		/*{
			HdlProgram::stopProgram();
			HdlTexture::unbind();

			glBegin(GL_LINES);
				// X
				//glColor3f(1.0f,0.0f,1.0f);	// magenta
				//glVertex2f(-0.05f,-0.05f);
				//glVertex2f(5.0f,-0.05f);
				glColor3f(1.0f,0.0f,0.0f);	// red, ONE
				glVertex2f(0.0f,0.0f);
				glVertex2f(1.0f,0.0f);
				//glColor3f(1.0f,0.0f,1.0f);	// magenta
				//glVertex2f(0.05f,0.05f);
				//glVertex2f(0.5f,0.05f);

				// Y
				//glColor3f(0.0f,1.0f,1.0f);	// cyan
				//glVertex2f(-0.05f,-0.05f);
				//glVertex2f(-0.05f,5.0f);
				glColor3f(0.0f,1.0f,0.0f);	// green, ONE
				glVertex2f(0.0f,0.0f);
				glVertex2f(0.0f,1.0f);
				//glColor3f(0.0f,1.0f,1.0f);	// magenta
				//glVertex2f(0.05f,0.05f);
				//glVertex2f(0.05f,0.5f);
			glEnd();

			const float a = 0.3f;
			glBegin(GL_QUADS);
				glColor3f(1.0f,0.0f,0.0f);
				glVertex2f(-a,-a);
				glColor3f(0.0f,1.0f,0.0f);
				glVertex2f(-a,a);
				glColor3f(0.0f,0.0f,1.0f);
				glVertex2f(a,a);
				glColor3f(1.0f,1.0f,1.0f);
				glVertex2f(a,-a);
			glEnd();
		}*/
	}

	void QVGLSceneWidget::keyPressEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyPressEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getKeyboardState().keyPressed(event);

		//std::cout << "Pressed : " << qvglParent->getKeyboardState().getActionAssociatedToKey(event) << std::endl;
	}

	void QVGLSceneWidget::keyReleaseEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyReleaseEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getKeyboardState().keyReleased(event);

		//std::cout << "Released : " << qvglParent->getKeyboardState().getActionAssociatedToKey(event) << std::endl;
	}

	void QVGLSceneWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseMoveEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, false, true, false);
	}

	void QVGLSceneWidget::wheelEvent(QGraphicsSceneWheelEvent* event)
	{
		QGraphicsScene::wheelEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event);
	}

	void QVGLSceneWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mousePressEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, true, false, false);

		// TMP : 
		{
			float xGl, yGl;
			qvglParent->toGlCoordinates(event->scenePos().x(), event->scenePos().y(), xGl, yGl, false);

			std::cout << "MousePressed - Gl Coordinates    : " << xGl << ", " << yGl << std::endl;

			float xQuad, yQuad;
			qvglParent->toQuadCoordinates(xGl, yGl, xQuad, yQuad, false);
			
			std::cout << "MousePressed - Quad Coordinates  : " << xQuad << ", " << yQuad << std::endl;

			float xImg, yImg;
			qvglParent->toImageCoordinates(xQuad, yQuad, xImg, yImg, false);
			std::cout << "MousePressed - Image Coordinates : " << xImg << ", " << yImg << std::endl;
		}
	}

	void QVGLSceneWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseReleaseEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, false, false, true);
	}

	void QVGLSceneWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseDoubleClickEvent(event);

		if(!event->isAccepted())
			event->accept();

		// TODO
			//qvglParent->getMouseState().processEvent(event);
		std::cout << "TODO - line " << __LINE__ << " : add double click event process." << std::endl;
	}

	/*void QVGLSceneWidget::addSubWidget(QVGLSubWidget* subWidget)
	{
		subWidget->graphicsProxy = addWidget(subWidget);

		// Connect : 
		// TODO : TO REMOVE
		QObject::con//nect(subWidget, SIGNAL(selected(QVGLSubWidget*)), this, SLOT(reorderItems(QVGLSubWidget*)));
	}

	void QVGLSceneWidget::putItemOnTop(QGraphicsProxyWidget* graphicsProxy)
	{
		if(graphicsProxy!=NULL)
		{
			QList<QGraphicsItem*> itemsList = items();

			qreal k = itemsList.count()-2;

			for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
			{
				if((*it)!=reinterpret_cast<QGraphicsItem*>(selectedWidget->graphicsProxy))
				{
					(*it)->setZValue(k);
					k--;
				}
			}

			// Raise to top : 
			graphicsProxy->setZValue(itemsList.count()-1.0);
		}
	}*/

// QVGLSceneViewWidget :
	QVGLSceneViewWidget::QVGLSceneViewWidget(QVGLWidget* _qvglParent, QVGLTopBar* _topBar)
	 : 	contextWidget(NULL),
		sceneWidget(NULL),
		qvglParent(_qvglParent)
	{
		// Create the GL widget : 
		QGLFormat glFormat(QGL::DepthBuffer | QGL::DoubleBuffer);
		contextWidget = new QVGLContextWidget(new QGLContext(glFormat), this);
		setViewport(contextWidget);

		// Create the scene : 
		sceneWidget = new QVGLSceneWidget(qvglParent, _topBar);
		setScene(sceneWidget);

		// Other parameters : 	
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);	// For container catching.
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);	// Because GL has to redraw the complete area.
	}

	QVGLSceneViewWidget::~QVGLSceneViewWidget(void)
	{ }

	void QVGLSceneViewWidget::resizeEvent(QResizeEvent *event)
	{
		if(scene())
			scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));

		QGraphicsView::resizeEvent(event);
   	}

	void QVGLSceneViewWidget::addSubWidget(QVGLSubWidget* subWidget)
	{
		subWidget->graphicsProxy = sceneWidget->addWidget(subWidget);
	}

	void QVGLSceneViewWidget::putItemOnTop(QGraphicsProxyWidget* graphicsProxy)
	{
		if(graphicsProxy!=NULL)
		{
			QList<QGraphicsItem*> itemsList = sceneWidget->items();

			qreal k = itemsList.count()-2;

			for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
			{
				if((*it)!=reinterpret_cast<QGraphicsItem*>(graphicsProxy))
				{
					(*it)->setZValue(k);
					k--;
				}
			}

			// Raise to top : 
			graphicsProxy->setZValue(itemsList.count()-1.0);
		}
	}

	void QVGLSceneViewWidget::putItemOnBottom(QGraphicsProxyWidget* graphicsProxy)
	{
		if(graphicsProxy!=NULL)
		{
			QList<QGraphicsItem*> itemsList = sceneWidget->items();

			qreal k = itemsList.count()-1;

			for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
			{
				if((*it)!=reinterpret_cast<QGraphicsItem*>(graphicsProxy))
				{
					(*it)->setZValue(k);
					k--;
				}
			}

			// Raise to top : 
			graphicsProxy->setZValue(0.0);
		}
	}

	void QVGLSceneViewWidget::makeGLContextAvailable(void)
	{
		contextWidget->makeCurrent();
	}

	void QVGLSceneViewWidget::update(void)
	{
		sceneWidget->update();
		//QGraphicsView::update(); // not required.
	}

// QVGLWidget :
	QVGLWidget::QVGLWidget(QWidget* parent)
	 :	QWidget(parent), 
		container(QBoxLayout::LeftToRight, this),
		sceneViewWidget(this, &topBar),
		currentViewIndex(-1),
		opacityActiveSubWidget(0.8),
		opacityIdleSubWidget(0.2),
		opacityActiveBar(0.8),
		opacityIdleBar(0.4)
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		container.addWidget(&sceneViewWidget);
		container.setMargin(0);
		container.setSpacing(0);

		// Reset bars views : 
		barSelected(&topBar);

		QObject::connect(&mouseState,		SIGNAL(requestExternalUpdate(void)),		this, SLOT(updateMouseStateData(void)));
		QObject::connect(&sceneViewWidget, 	SIGNAL(requireContainerCatch(void)), 		this, SLOT(handleCatch(void)));
		QObject::connect(&keyboardState,	SIGNAL(actionReceived(QVGLActionID)),		this, SLOT(processAction(QVGLActionID)));
		QObject::connect(&mouseState,		SIGNAL(updated(void)),				this, SLOT(performMouseAction(void)));
		QObject::connect(&topBar,		SIGNAL(selected(QVGLTopBar*)),			this, SLOT(barSelected(QVGLTopBar*)));
		QObject::connect(&topBar,		SIGNAL(changeViewRequest(QVGLView*)),		this, SLOT(viewRequireDisplay(QVGLView*)));
		QObject::connect(&topBar,		SIGNAL(showSubWidgetRequest(QVGLSubWidget*)),	this, SLOT(showSubWidget(QVGLSubWidget*)));
	}

	QVGLWidget::~QVGLWidget(void)
	{
		disconnect();

		for(QList<QVGLView*>::Iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			(*it)->qvglParent = NULL;
	}

	void QVGLWidget::updateMouseStateData(void)
	{
		const QList<QVGLMouseState::VectorID>& vectorIDs = mouseState.getVectorIDs();

		for(QList<QVGLMouseState::VectorID>::const_iterator it=vectorIDs.begin(); it!=vectorIDs.end(); it++)
		{
			if( mouseState.doesVectorRequireUpdate(*it) && ((QVGLMouseState::getVectorBasis(*it)==QVGLMouseState::PixelBasis) || (QVGLMouseState::getVectorBasis(*it)==QVGLMouseState::PixelRelativeBasis))) // use the first one to populate the others : 
			{
				const bool isBasisRelative = (QVGLMouseState::getVectorBasis(*it)==QVGLMouseState::PixelRelativeBasis);

				float 	xGl	= 0.0f, 
					yGl	= 0.0f, 
					xQuad	= 0.0f, 
					yQuad	= 0.0f,
					xImg	= 0.0f,
					yImg	= 0.0f;

				QPointF vPixel = mouseState.invisibleGetVector(*it);

				toGlCoordinates(vPixel.x(), vPixel.y(), xGl, yGl, isBasisRelative);
				toQuadCoordinates(xGl, yGl, xQuad, yQuad, isBasisRelative);
				toImageCoordinates(xQuad, yQuad, xImg, yImg, isBasisRelative);

				mouseState.setVector(*it, vPixel); // Clear the require update flag.
				mouseState.setVector(static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::GlBasis), 	QPointF(xGl, yGl));
				mouseState.setVector(static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::QuadBasis),	QPointF(xQuad, yQuad));
				mouseState.setVector(static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::ImageBasis),	QPointF(xImg, yImg));

				//std::cout << "Updating from " << *it << " to " << static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::GlBasis) << ", " << static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::QuadBasis) << ", " << static_cast<QVGLMouseState::VectorID>(*it + QVGLMouseState::ImageBasis) << std::endl;
				//std::cout << "Vector : " << vPixel.x() << ", " << vPixel.y() << std::endl;

				// Need to update associated color : 

				// Set the read buffer : 
				// makeGLContextAvailable();
				//glReadBuffer( GL_BACK );

				// Subtle point here : the frame buffer is verticaly flipped!
				//glReadPixels(x, height()-(y+1), 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);
			}
		}

		mouseState.updateProcessCompleted();
	}

	void QVGLWidget::performMouseAction(void)
	{
		QVGLView* currentView = getCurrentView();

		if(currentView!=NULL && mouseState.getFunctionMode()==QVGLMouseState::ModeMotion)
		{
			if(mouseState.isVectorModified(QVGLMouseState::VectorLastLeftShiftGl))
			{
				QPointF v = mouseState.getVector(QVGLMouseState::VectorLastLeftShiftGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				// Scene scaling : see toGlCoordinates

				// Homothecy : 
				if(currentView->homothecyScale!=0.0f)
				{
					v.rx() /= currentView->homothecyScale;
					v.ry() /= currentView->homothecyScale;
				}

				currentView->move(v.x(), v.y());
			}

			if(mouseState.isVectorModified(QVGLMouseState::VectorLastWheelUpGl) && mouseState.isWheelDeltaModified())
			{
				QPointF v = mouseState.getVector(QVGLMouseState::VectorLastWheelUpGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				currentView->zoom(v.x(), v.y(), std::pow(1.2f, mouseState.getWheelDelta()));
			}

			if(mouseState.isVectorModified(QVGLMouseState::VectorLastWheelDownGl) && mouseState.isWheelDeltaModified())
			{
				QPointF v = mouseState.getVector(QVGLMouseState::VectorLastWheelDownGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				currentView->zoom(v.x(), v.y(), std::pow(1.2f, mouseState.getWheelDelta()));
			}
		}
	}

	/*void QVGLWidget::handleCatch(void)
	{
		container.removeWidget(&sceneViewWidget);
		container.addWidget(&sceneViewWidget);
	}*/

	void QVGLWidget::viewRequireDisplay(QVGLView* view)
	{
		int idx = viewsList.indexOf(view);

		if(idx>=0 && idx<viewsList.size())
			changeCurrentView(idx);
	}

	void QVGLWidget::viewRequireDisplay(void)
	{
		// Get the emitter : 
		QVGLView* view = reinterpret_cast<QVGLView*>(QObject::sender());

		viewRequireDisplay(view);
	}

	void QVGLWidget::viewUpdated(QVGLView* view)
	{
		if(view==getCurrentView() && view!=NULL)
			sceneViewWidget.update();
	}

	void QVGLWidget::viewUpdated(void)
	{
		// Get the emitter : 
		QVGLView* view = reinterpret_cast<QVGLView*>(QObject::sender());

		viewUpdated(view);
	}

	void QVGLWidget::viewClosed(QVGLView* view)
	{
		int idx = viewsList.indexOf(view);

		// If this view is in the list : 
		if(idx!=-1)
		{
			// Remove this view from the list :
			viewsList.removeAt(idx);

			// Disconnect : 
			view->disconnect(this);
			view->qvglParent = NULL;
				
			// Change current view if needed :
			if(currentViewIndex==idx)
				changeCurrentView(currentViewIndex);

			// Update views list : 
			topBar.updateViewsList(viewsList);
		}
	}

	void QVGLWidget::viewClosed(void)
	{
		// Get the emitter : 
		QVGLView* view = reinterpret_cast<QVGLView*>(QObject::sender());
		
		viewClosed(view);
	}

	void QVGLWidget::subWidgetSelected(QVGLSubWidget* subWidget)
	{
		if(subWidgetsList.contains(subWidget) && subWidget->qvglParent==this && subWidget->graphicsProxy!=NULL)
		{
			// Change opacity of all other subWidgets : 
			for(QList<QVGLSubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityIdleBar);

			// Change the opacity of the current subWidget : 
			subWidget->setWindowOpacity(opacityActiveSubWidget);

			// Raise the current subWidget : 
			sceneViewWidget.putItemOnTop(subWidget->graphicsProxy);
		}
	}

	void QVGLWidget::subWidgetSelected(void)
	{
		// Get the emitter : 
		QVGLSubWidget* subWidget = reinterpret_cast<QVGLSubWidget*>(QObject::sender());

		subWidgetSelected(subWidget);
	}

	void QVGLWidget::showSubWidget(QVGLSubWidget* subWidget)
	{
		if(!subWidget->isVisible())
			subWidget->show();		// implement the Show (1st part)
		else
			subWidgetSelected(subWidget);	// implement the raise (2nd part)
	}

	void QVGLWidget::showSubWidget(void)
	{
		// Get the emitter : 
		QVGLSubWidget* subWidget = reinterpret_cast<QVGLSubWidget*>(QObject::sender());

		showSubWidget(subWidget);
	}
	
	void QVGLWidget::hideSubWidget(QVGLSubWidget* subWidget)
	{
		if(subWidgetsList.contains(subWidget) && subWidget->qvglParent==this && subWidget->graphicsProxy!=NULL)
		{
			// Lower the current subWidget : 
			sceneViewWidget.putItemOnBottom(subWidget->graphicsProxy);

			// Raise the top bar : 
			sceneViewWidget.putItemOnTop(topBar.graphicsProxy);
		}
	}

	void QVGLWidget::hideSubWidget(void)
	{
		// Get the emitter : 
		QVGLSubWidget* subWidget = reinterpret_cast<QVGLSubWidget*>(QObject::sender());

		hideSubWidget(subWidget);
	}

	void QVGLWidget::subWidgetClosed(QVGLSubWidget* subWidget)
	{
		int idx = subWidgetsList.indexOf(subWidget);

		// If this view is in the list : 
		if(idx!=-1)
		{
			// Remove this view from the list :
			subWidgetsList.removeAt(idx);

			// Disconnect : 
			subWidget->disconnect(this);
			subWidget->qvglParent = NULL;

			// Update widgets list : 
			topBar.updateSubWidgetsList(subWidgetsList);
		}
	}

	void QVGLWidget::subWidgetClosed(void)
	{
		// Get the emitter : 
		QVGLSubWidget* subWidget = reinterpret_cast<QVGLSubWidget*>(QObject::sender());
		
		subWidgetClosed(subWidget);
	}

	void QVGLWidget::barSelected(QVGLTopBar* bar)
	{
		if(bar==&topBar)
		{
			// Change opacity of all other subWidgets : 
			for(QList<QVGLSubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityActiveBar);

			// Raise the bar : 
			sceneViewWidget.putItemOnTop(topBar.graphicsProxy);
		}	
	}

	QVGLKeyboardState& QVGLWidget::getKeyboardState(void)
	{
		return keyboardState;
	}

	QVGLMouseState&	QVGLWidget::getMouseState(void)
	{
		return mouseState;
	}

	const QVGLKeyboardState& QVGLWidget::getKeyboardState(void) const
	{
		return keyboardState;
	}

	const QVGLMouseState& QVGLWidget::getMouseState(void) const
	{
		return mouseState;
	}

	QVGLView* QVGLWidget::getCurrentView(void) const
	{
		if(currentViewIndex>=0 && currentViewIndex<viewsList.size())
			return viewsList[currentViewIndex];
		else
			return NULL;
	}

	void QVGLWidget::changeCurrentView(int targetID)
	{
		currentViewIndex = std::min(std::max(targetID, 0), viewsList.size()-1);

		// Change title : 
		QVGLView* currentView = getCurrentView();

		if(currentView!=NULL)
		{
			topBar.setTitle(*currentView);

			// Show : 
			sceneViewWidget.update();
		}
	}

	void QVGLWidget::getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const
	{
		const float sceneRatio = getSceneRatio();

		xSceneScale = std::min(1.0f, 1.0f/sceneRatio);
		ySceneScale = std::min(1.0f, sceneRatio); 	// The minus sign set the orientation of the GL axis to be the same as Qt.
	}

	float QVGLWidget::getAdaptationScaling(const float& imageRatio) const
	{
		const float sceneRatio = getSceneRatio();

		if((sceneRatio<=1.0f && imageRatio>=1.0f) || (sceneRatio>=1.0f && imageRatio<=1.0f))	// Opposite aspect ratios : do not do anything.
			return 1.0f;
		else if(imageRatio<=1.0f)	// Portrait
		{
			if(sceneRatio>imageRatio)
				return std::max(sceneRatio, 1.0f/sceneRatio);
			else
				return std::max(imageRatio, 1.0f/imageRatio);
		}
		else //if(imageRatio>1.0f)	// Landscape
		{
			if(sceneRatio>imageRatio)
				return std::max(imageRatio, 1.0f/imageRatio);
			else
				return std::max(sceneRatio, 1.0f/sceneRatio);
		}
	}

	void QVGLWidget::toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const
	{
		QRectF rect = sceneRect();

		if(!isRelative)
		{
			xGl = (static_cast<float>(x) - rect.x()) * 2.0f / rect.width() - 1.0f;
			yGl = 1.0f - (static_cast<float>(y) - rect.y()) * 2.0f / rect.height();
		}
		else
		{
			xGl = static_cast<float>(x) * 2.0f / rect.width();
			yGl = - static_cast<float>(y) * 2.0f / rect.height();
		}

		// Scene scaling : 
		float xSceneScale, ySceneScale;
		getSceneRatioScaling(xSceneScale, ySceneScale);	

		xGl /= xSceneScale;
		yGl /= ySceneScale;
	}

	void QVGLWidget::toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, QVGLView* view) const
	{
		// Input coordinates are assumed relative in the window.

		if(view==NULL)
			view = getCurrentView();

		if(view==NULL)
		{
			xQuad = 0.0f;
			yQuad = 0.0f;
		}
		else if(!isRelative)
		{
			// Adaptation scaling : 
			const float adaptationScale = getAdaptationScaling(view->getImageRatio());
			float 	x = xGl / adaptationScale,
				y = yGl / adaptationScale;

			// Scene scaling : see toGlCoordinates
			
			// Homothecy : 
			x = (x - view->homothecyCenter[0])/view->homothecyScale + view->homothecyCenter[0];
			y = (y - view->homothecyCenter[1])/view->homothecyScale + view->homothecyCenter[1];

			// Translation : 
			x -= view->viewCenter[0];
			y -= view->viewCenter[1];

			// Rotation : 
			const float 	c  = std::cos(view->angle),
					s  = std::sin(view->angle),
			 		x2 = c * x - s * y,
					y2 = s * x + c * y;

			// Image scaling : 
			float xImgScale, yImgScale;
			view->getAspectRatioScaling(xImgScale, yImgScale);

			xQuad = x2 / xImgScale;
			yQuad = y2 / yImgScale;
		}
		else
		{
			// Adaptation scaling : 
			const float adaptationScale = getAdaptationScaling(view->getImageRatio());
			float 	x = xGl / adaptationScale,
				y = yGl / adaptationScale;

			// Scene scaling : see toGlCoordinates

			// Homothecy : 
			x /= view->homothecyScale;
			y /= view->homothecyScale;

			// Rotation : 
			const float 	c  = std::cos(view->angle),
					s  = std::sin(view->angle),
			 		x2 = c * x - s * y,
					y2 = s * x + c * y;

			// Image scaling : 
			float xImgScale, yImgScale;
			view->getAspectRatioScaling(xImgScale, yImgScale);

			xQuad = x2 / xImgScale;
			yQuad = y2 / yImgScale;
		}
	}

	void QVGLWidget::toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, QVGLView* view) const
	{
		if(view==NULL)
			view = getCurrentView();

		if(view==NULL)
		{
			xImg = 0.0f;
			yImg = 0.0f;
		}
		else if(!isRelative)
		{
			xImg = (xQuad + 1.0f)*view->getFormat().getWidth()/2.0f;
			yImg = (1.0f - yQuad)*view->getFormat().getHeight()/2.0f;
		}
		else
		{
			xImg = xQuad*view->getFormat().getWidth()/2.0f;
			yImg = -yQuad*view->getFormat().getHeight()/2.0f;
		}
	}

	void QVGLWidget::addView(QVGLView* view)
	{
		if(!viewsList.contains(view) && view->qvglParent==NULL)
		{
			viewsList.append(view);

			view->qvglParent = this;

			// Connect actions : 
			QObject::connect(view, SIGNAL(requireDisplay()),	this, SLOT(viewRequireDisplay()));
			QObject::connect(view, SIGNAL(updated()),		this, SLOT(viewUpdated()));
			QObject::connect(view, SIGNAL(closed()), 		this, SLOT(viewClosed()));

			// Update views list : 
			topBar.updateViewsList(viewsList);
		}
	}

	void QVGLWidget::addSubWidget(QVGLSubWidget* subWidget)
	{
		if(!subWidgetsList.contains(subWidget) && subWidget->qvglParent==NULL)
		{
			sceneViewWidget.addSubWidget(subWidget);
			
			// Connect : 
			QObject::connect(subWidget, SIGNAL(selected(QVGLSubWidget*)), 		this, SLOT(subWidgetSelected(QVGLSubWidget*)));
			QObject::connect(subWidget, SIGNAL(showRequest(QVGLSubWidget*)),	this, SLOT(showSubWidget(QVGLSubWidget*)));
			QObject::connect(subWidget, SIGNAL(hideRequest(QVGLSubWidget*)),	this, SLOT(hideSubWidget(QVGLSubWidget*)));
			QObject::connect(subWidget, SIGNAL(destroyed(void)),			this, SLOT(subWidgetClosed(void)));

			// Save link : 
			subWidget->qvglParent = this;
			subWidgetsList.append(subWidget);

			// Update widgets list : 
			topBar.updateSubWidgetsList(subWidgetsList);

			// Show : 
			subWidget->show();
		}
	}

	float QVGLWidget::getSceneRatio(void) const
	{
		return sceneRect().width() / sceneRect().height();
	}

	QRectF QVGLWidget::sceneRect(void) const
	{
		return sceneViewWidget.sceneRect();
	}

	void QVGLWidget::processAction(QVGLActionID action)
	{
		QVGLView* currentView = getCurrentView();

		switch(action)
		{
			case QVGLActionUp :
				if(currentView!=NULL) currentView->move(0.0f, -0.1f);
				break;
			case QVGLActionDown :
				if(currentView!=NULL) currentView->move(0.0f, +0.1f);
				break;
			case QVGLActionLeft :
				if(currentView!=NULL) currentView->move(-0.1f, 0.0f);
				break;
			case QVGLActionRight :
				if(currentView!=NULL) currentView->move(+0.1f, 0.0f);
				break;
			case QVGLActionZoomIn :
				if(currentView!=NULL)
				{
					float xCenter, yCenter;
					currentView->getViewCenter(xCenter, yCenter);

					// TODO - MISSING : SCREEN DEPENDENT SCALING
					std::cout << "TODO - line " << __LINE__ << " : add dependent screen scaling to solve bad centering bug." << std::endl;

					// Adaptation scaling : 
					//const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());
					//xCenter *= adaptationScale;
					//yCenter *= adaptationScale;

					currentView->zoom(xCenter, yCenter, 1.2f);
				}
				break;
			case QVGLActionZoomOut :
				if(currentView!=NULL)
				{
					float xCenter, yCenter;
					currentView->getViewCenter(xCenter, yCenter);

					// TODO - MISSING : SCREEN DEPENDENT SCALING
					std::cout << "TODO - line " << __LINE__ << " : add dependent screen scaling to solve bad centering bug." << std::endl;

					// Adaptation scaling : 
					//const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());
					//xCenter *= adaptationScale;
					//yCenter *= adaptationScale;

					currentView->zoom(xCenter, yCenter, 1.0f/1.2f);
				}
				break;
			case QVGLActionRotationClockWise :
				if(currentView!=NULL) currentView->rotate(-0.17453f);
				break;
			case QVGLActionRotationCounterClockWise :
				if(currentView!=NULL) currentView->rotate(+0.17453f);
				break;
			case QVGLActionToggleFullscreen :
				if(!sceneViewWidget.isFullScreen())
				{
					// Enter fullscreen : 
					sceneViewWidget.setParent(NULL);
					sceneViewWidget.showFullScreen();
					break;
				}
				// else : 
			case QVGLActionExitFullscreen :
				// Re-attach :
				sceneViewWidget.setParent(this);
				sceneViewWidget.showNormal();
				container.removeWidget(&sceneViewWidget);
				container.addWidget(&sceneViewWidget);
				break;
			case QVGLActionResetView :
				if(currentView!=NULL) currentView->reset();
				break;
			case QVGLActionPreviousView :
				//currentViewIndex = std::max(currentViewIndex - 1, std::min(viewsList.size()-1, 0));
				changeCurrentView(currentViewIndex - 1);
				break;
			case QVGLActionNextView :
				//currentViewIndex = std::min(currentViewIndex + 1, viewsList.size()-1);
				changeCurrentView(currentViewIndex + 1);
				break;
			case QVGLActionCloseView :
				if(currentView!=NULL) currentView->close();
				break;
			case QVGLActionCloseAllViews :
			case QVGLActionMotionModifier :
			case QVGLActionRotationModifier :
			case QVGLNoAction :
				break;
			default : 
				throw Exception("QVGLWidget::processAction - Unknown action (code : " + to_string(action) + ").", __FILE__, __LINE__);
		}
	}

