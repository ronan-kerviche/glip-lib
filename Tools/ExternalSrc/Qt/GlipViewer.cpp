#include "GlipViewer.hpp"
#include <QMenuBar>
#include <QMenu>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

using namespace QGlip;

// TopBar :
	TopBar::TopBar(QGraphicsItem* parent)
	 :	QGraphicsProxyWidget(parent),
		layout(&widget)
	{
		setObjectName("TopBar");
		widget.setObjectName("TopBar");

		QGraphicsProxyWidget::setWidget(&widget);

		// Test :
		layout.setMargin(0);
		layout.setSpacing(0);
		widget.setWindowOpacity(0.75f);
	}
	
	TopBar::~TopBar(void)
	{
		// Prevent the layout to delete subWidgets :
		for(int k=0; k<NumPositions; k++)
			takeWidget(static_cast<Position>(k));
		// Prevent the proxy to delete the widget :
		QGraphicsProxyWidget::setWidget(NULL);
	}

	QVariant TopBar::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
	{
		// If the item is attached to a new scene, make sure the top bar can resize :
		if((change==QGraphicsItem::ItemSceneChange || change==QGraphicsItem::ItemSceneHasChanged) && scene()!=NULL)
		{
			QObject::connect(scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(fitToScene(const QRectF&)));
			fitToScene(scene()->sceneRect());
			return value;
		}
		else
			return QGraphicsItem::itemChange(change, value);
	}

	void TopBar::fitToScene(const QRectF& newRect)
	{
		setGeometry(QRectF(0.0f, 0.0f, newRect.width(), static_cast<float>(getHeight())));
	}

	QWidget* TopBar::getWidget(const Position& p)
	{
		return layout.itemAtPosition(0, static_cast<int>(p))->widget();
	}

	QWidget* TopBar::takeWidget(const Position& p)
	{
		QLayoutItem* item = layout.itemAtPosition(0, static_cast<int>(p));
		if(item!=NULL && item->widget()!=NULL)
		{
			QWidget* widget = item->widget();
			layout.removeWidget(widget);
			widget->hide();
			widget->setParent(NULL);
			return widget;
		}
		else
			return NULL;
	}

	void TopBar::setWidget(const Position& p, QWidget* ptr)
	{
		// This will not delete a widget previously added at the same location.
		takeWidget(p);
		layout.addWidget(ptr, 0, static_cast<int>(p));
	}

	int TopBar::getHeight(void)
	{
		return 24;
	}

// ViewTransform :
	ViewTransform::ViewTransform(void)
	 : 	center(0.0f, 0.0f),
		angle_rad(0.0f),
		scale(1.0f),
		verticalFlip(false),
		horizontalFlip(false),
		c(1.0f),
		s(0.0f)
	{
		setObjectName("ViewTransform");
	}

	ViewTransform::ViewTransform(const ViewTransform& t)
	 :	QObject(NULL),	// No parent copy.
		center(t.center),
		angle_rad(t.angle_rad),
		scale(t.scale),
		verticalFlip(t.verticalFlip),
		horizontalFlip(t.horizontalFlip),
		c(t.c),
		s(t.s)
	{
		setObjectName("ViewTransform");
	}

	ViewTransform::~ViewTransform(void)
	{ }

	ViewTransform& ViewTransform::operator=(const ViewTransform& t)
	{
		center = t.center;
		angle_rad = t.angle_rad;
		c = t.c;
		s = t.s;
		scale = t.scale;
		verticalFlip = t.verticalFlip;
		horizontalFlip = t.horizontalFlip;
		emit transformModified();
		return (*this);
	}

	QPointF ViewTransform::getCenter(void) const
	{
		return center;
	}

	void ViewTransform::setCenter(const QPointF& c)
	{
		center = c;
		emit transformModified();
	}

	float ViewTransform::getAngle_rad(void) const
	{
		return angle_rad;
	}

	float ViewTransform::getAngleCosine(void) const
	{
		return c;
	}

	float ViewTransform::getAngleSine(void) const
	{
		return s;
	}

	void ViewTransform::setAngle_rad(const float& a)
	{
		angle_rad = a;
		c = std::cos(angle_rad);
		s = std::sin(angle_rad);
		emit transformModified();
	}

	float ViewTransform::getScale(void) const
	{
		return scale;
	}

	void ViewTransform::setScale(const float& s)
	{
		scale = s;
		emit transformModified();
	}
	
	bool ViewTransform::getVerticalFlip(void) const
	{
		return verticalFlip;
	}

	void ViewTransform::setVerticalFlip(const bool& e)
	{
		verticalFlip = e;
		emit transformModified();
	}

	bool ViewTransform::getHorizontalFlip(void) const
	{
		return horizontalFlip;
	}

	void ViewTransform::setHorizontalFlip(const bool& e)
	{
		horizontalFlip = e;
		emit transformModified();
	}

	QPointF ViewTransform::standardQuadVertexToGLCoordinates(const QPointF& vertex, const QSize& sceneSize, const QSize& textureSize) const
	{
		QPointF p, x;
		p.rx() = ((horizontalFlip ? -1.0f : 1.0f) * vertex.x() + 1.0f) / 2.0f * textureSize.width() - center.x();
		p.ry() = ((verticalFlip ? -1.0f : 1.0f) * vertex.y() + 1.0f) / 2.0f * textureSize.height() - center.y();
		x.rx() = (c*p.x() + s*p.y()) * (scale * getBaseScale(sceneSize, textureSize)) / static_cast<float>(sceneSize.width()) * 2.0f;
		x.ry() = (-s*p.x() + c*p.y()) * (scale * getBaseScale(sceneSize, textureSize)) / static_cast<float>(sceneSize.height()) * 2.0f;
		return x;
	}

	QPointF ViewTransform::pixelToFragmentCoordinates(const QPointF& pixel, const QRectF& sceneRect, const QSize& textureSize, const bool& omitFlips) const
	{
		const QSize sceneSize = sceneRect.size().toSize();
		QPointF p, f;
		p.rx() = ((pixel.x() - sceneRect.left())/sceneRect.width()*2.0f - 1.0f) / 2.0f * static_cast<float>(sceneRect.width()) / (scale * getBaseScale(sceneSize, textureSize));
		p.ry() = ((sceneRect.bottom() - pixel.y())/sceneRect.height()*2.0f - 1.0f) / 2.0f * static_cast<float>(sceneRect.height()) / (scale * getBaseScale(sceneSize, textureSize));
		f.rx() = (c*p.x() - s*p.y()) + center.x();
		f.ry() = (s*p.x() + c*p.y()) + center.y();
		if(horizontalFlip && !omitFlips)
			f.rx() = textureSize.width() - f.x();
		if(verticalFlip && !omitFlips)
			f.ry() = textureSize.height() - f.y();
		return f;
	}	

	void ViewTransform::reset(const QSize& textureSize, const int& duration)
	{
		if(duration<=0)
		{
			center = QPointF(textureSize.width(), textureSize.height()) / 2.0f;
			angle_rad = 0.0f;
			c = 1.0f;
			s = 0.0f;
			scale = 1.0f;
			horizontalFlip = false;
			verticalFlip = false;
			emit transformModified();
		}
		else
		{
			// Simplify the rotation :
			angle_rad = angle_rad - std::floor(angle_rad/(2.0f*M_PI)+0.5f)*2.0f*M_PI;
			angle_rad = (angle_rad>=M_PI) ? -angle_rad : angle_rad;
			c = std::cos(angle_rad);
			s = std::sin(angle_rad);
			// Create the animations :
			QParallelAnimationGroup* animation = new QParallelAnimationGroup(this);
			QPropertyAnimation* centerAnimation = new QPropertyAnimation(this, "center");
			centerAnimation->setEndValue(QPointF(textureSize.width(), textureSize.height()) / 2.0f);
			centerAnimation->setDuration(duration);
			QPropertyAnimation* angleAnimation = new QPropertyAnimation(this, "angle_rad");
			angleAnimation->setEndValue(0.0f);
			angleAnimation->setDuration(duration);
			QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
			scaleAnimation->setEndValue(1.0f);
			scaleAnimation->setDuration(duration);
			// Flip on first step :
			horizontalFlip = false;
			verticalFlip = false;
			animation->addAnimation(centerAnimation);
			animation->addAnimation(angleAnimation);
			animation->addAnimation(scaleAnimation);
			// Automatic delete upon completion :
			QObject::connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
			animation->start();
		}
	}

	void ViewTransform::translate(const QPointF& t)
	{
		center -= t;
		emit transformModified();
	}

	void ViewTransform::rotate(const float& a)
	{
		angle_rad -= a;
		c = std::cos(angle_rad);
		s = std::sin(angle_rad);
		emit transformModified();
	}

	void ViewTransform::zoom(const float& r, const QPointF& p)
	{
		scale *= r;
		center = (center + p * (r - 1.0f)) / r;
		emit transformModified();
	}

	void ViewTransform::zoom(const float& r)
	{
		zoom(r, center);
	}

	void ViewTransform::scaleOneToOne(void)
	{
		scale = 1.0f;
		emit transformModified();
	}	

	float ViewTransform::getBaseScale(const QSize& sceneSize, const QSize& textureSize)
	{
		return std::min(static_cast<float>(sceneSize.width())/static_cast<float>(textureSize.width()), static_cast<float>(sceneSize.height())/static_cast<float>(textureSize.height()));
	}

	float ViewTransform::getCompoundScale(const QSize& sceneSize, const QSize& textureSize) const
	{
		return scale * getBaseScale(sceneSize, textureSize);
	}
	
	void ViewTransform::flipHorizontal(void)
	{
		horizontalFlip = !horizontalFlip;
		emit transformModified();
	}

	void ViewTransform::flipVertical(void)
	{
		verticalFlip = !verticalFlip;
		emit transformModified();
	}

// AbstractGLDrawableObject :
	AbstractGLDrawableObject::AbstractGLDrawableObject(QObject* parent)
	 :	QObject(parent)
	{ }

	void AbstractGLDrawableObject::processMousePressEvent(const QRectF& sceneRect, QGraphicsSceneMouseEvent* event, const View* view)
	{
		const QPointF p = view->pixelToFragmentCoordinates(event->scenePos(), sceneRect, false);
		std::cout << "Click : " << event << std::endl;
		std::cout << "    at : " << event->scenePos().x() << ", " << event->scenePos().y() << std::endl;
		std::cout << "    at : " << p.x() << ", " << p.y() << std::endl;
		event->accept();
	}

	void AbstractGLDrawableObject::processMouseMoveEvent(const QRectF& sceneRect, QGraphicsSceneMouseEvent* event, View* view, View** subViews, const int numSubViews)
	{
		if(view==NULL)
			return ;	
		if((event->buttons() & Qt::LeftButton)!=0)
		{
			if((event->modifiers() & Qt::ControlModifier)!=0)
			{
				const QPointF t = view->pixelToFragmentCoordinates(event->scenePos(), sceneRect, true) - view->pixelToFragmentCoordinates(event->lastScenePos(), sceneRect, true);
				view->transform.translate(t);
				for(int k=0; k<numSubViews; k++)
					subViews[k]->transform.translate(t);
				event->accept();
			}
			else if((event->modifiers() & Qt::ShiftModifier)!=0)
			{
				const QPointF	last(event->lastScenePos().x() - sceneRect.left() - sceneRect.width()/2.0f, sceneRect.height()/2.0f - event->lastScenePos().y() + sceneRect.top()),
						pos(event->scenePos().x() - sceneRect.left() - sceneRect.width()/2.0f, sceneRect.height()/2.0f - event->scenePos().y() + sceneRect.top());
				const float 	a = std::acos((pos.x()*last.x()+pos.y()*last.y())/(sqrt(last.x()*last.x() + last.y()*last.y())*sqrt(pos.x()*pos.x() + pos.y()*pos.y()))),
						b = ((pos.y()*last.x()-pos.x()*last.y()) >= 0.0f) ? a : -a;
				view->transform.rotate(b);
				for(int k=0; k<numSubViews; k++)
					subViews[k]->transform.rotate(b);
				event->accept();
			}
		}
	}

	void AbstractGLDrawableObject::processWheelEvent(const QRectF& sceneRect, QGraphicsSceneWheelEvent* event, View* view, View** subViews, const int numSubViews)
	{
		if(view==NULL)
			return ;
		if((event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))!=0)
		{
			const float scale = std::pow(1.01f, event->delta()/8.0f);	
			const QPointF pos = view->pixelToFragmentCoordinates(event->scenePos(), sceneRect, true);
			view->transform.zoom(scale, pos);
			for(int k=0; k<numSubViews; k++)
				subViews[k]->transform.zoom(scale, pos);
			event->accept();
		}	
	}

	void AbstractGLDrawableObject::processKeyPressEvent(const QRectF& sceneRect, QKeyEvent* event, View* view, View** subViews, const int numSubViews)
	{
		if(view==NULL)
			return ;
		switch(event->key())
		{
			case Qt::Key_Space :
				view->resetTransform(300);
				for(int k=0; k<numSubViews; k++)
					subViews[k]->resetTransform(300);
				event->accept();
				break;
			case Qt::Key_Left :
				if((event->modifiers() & Qt::AltModifier)!=0)
				{
					view->transform.flipHorizontal();
					for(int k=0; k<numSubViews; k++)
						subViews[k]->transform.flipHorizontal();
					event->accept();
				}
				else if((event->modifiers() & Qt::ShiftModifier)!=0)
				{
					view->transform.rotate(+M_PI/2.0f);
					for(int k=0; k<numSubViews; k++)
						subViews[k]->transform.rotate(+M_PI/2.0f);
					event->accept();
				}
				break;
			case Qt::Key_Right :
				if((event->modifiers() & Qt::AltModifier)!=0)
				{
					view->transform.flipHorizontal();
					for(int k=0; k<numSubViews; k++)
						subViews[k]->transform.flipHorizontal();
					event->accept();
				}
				else if((event->modifiers() & Qt::ShiftModifier)!=0)
				{
					view->transform.rotate(-M_PI/2.0f);
					for(int k=0; k<numSubViews; k++)
						subViews[k]->transform.rotate(-M_PI/2.0f);
					event->accept();
				}
				break;
			case Qt::Key_Up :
			case Qt::Key_Down :
				if((event->modifiers() & Qt::AltModifier)!=0)
				{
					view->transform.flipVertical();
					for(int k=0; k<numSubViews; k++)
						subViews[k]->transform.flipVertical();
					event->accept();
				}
			case Qt::Key_Backspace :
				emit view->gotoParentObject();
				event->accept();
				break;
			case Qt::Key_PageUp :
				emit view->gotoNextObject();
				break;
			case Qt::Key_PageDown :
				emit view->gotoPreviousObject();
				break;
			default:
				std::cerr << "View::keyPressEvent - Unknown key : " << event->key() << std::endl;
				break;
		}
	}

	void AbstractGLDrawableObject::showObject(void)
	{
		emit showRequest();
	}

	QList<AbstractGLDrawableObject*> AbstractGLDrawableObject::getSubObjects(void) const
	{
		return QList<AbstractGLDrawableObject*>();
	}

// View :
	View::View(TextureResource* ptr, QObject* parent)
	 :	AbstractGLDrawableObject(parent),
		textureResource(ptr),
		controlsLayout(&controlsWidget)
	{
		if(textureResource==NULL)
			qFatal("View::View - Cannot create a NULL view.");
		// Item :
		QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, true);
		QGraphicsItem::hide();
		// Create controls :
		controlsLayout.addWidget(&previousButton);
		controlsLayout.addWidget(&parentButton);
		controlsLayout.addWidget(&nextButton);
		controlsLayout.addWidget(&closeButton);
		previousButton.setText("Prev");
		parentButton.setText("Parent");
		nextButton.setText("Next");
		closeButton.setText("Close");
		controlsLayout.setMargin(0);
		controlsLayout.setSpacing(0);
		// Title :
		updateTitle();
		// Transform :
		QObject::connect(&transform, SIGNAL(transformModified()), this, SIGNAL(updateScene()));
		// Resource :
		QObject::connect(textureResource, SIGNAL(contentChanged()), this, SIGNAL(updateScene()));
		QObject::connect(textureResource, SIGNAL(formatChanged()), this, SIGNAL(updateScene()));
		QObject::connect(textureResource, SIGNAL(formatChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(textureChanged()), this, SIGNAL(updateScene()));
		QObject::connect(textureResource, SIGNAL(textureChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(nameChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(informationChanged(const QString)), this, SLOT(updateTitle()));
	}
	
	View::~View(void)
	{
		// Do not delete the resource :
		textureResource = NULL;
	}

	void View::updateTitle(void)
	{
		if(textureResource!=NULL)
		{
			title.setText(textureResource->getName());
			title.setToolTip(textureResource->getHTMLInformation());
		}
		else
		{
			title.setText("<Invalid View>");
			title.setToolTip("");
		}
	}

	void View::resourceDestroyed(void)
	{
		textureResource = NULL;
		emit closed();
	}

	void View::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		if(scene()!=NULL)
			processMousePressEvent(scene()->sceneRect(), event, this);
	}

	void View::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
	{
		if(scene()!=NULL)
			processMouseMoveEvent(scene()->sceneRect(), event, this);
	}
	
	void View::wheelEvent(QGraphicsSceneWheelEvent* event)
	{
		if(scene()!=NULL)
			processWheelEvent(scene()->sceneRect(), event, this);	
	}

	void View::keyPressEvent(QKeyEvent* event)
	{
		if(scene()!=NULL)
			processKeyPressEvent(scene()->sceneRect(), event, this);	
	}

	const QSize View::getTextureSize(void) const
	{
		return (textureResource!=NULL && textureResource->getTexture()!=NULL) ? QSize(textureResource->getTexture()->getWidth(), textureResource->getTexture()->getHeight()) : QSize(1, 1);
	}

	TextureResource* View::getTextureResource(void) const
	{
		return textureResource;
	}	

	QPointF View::standardQuadVertexToGLCoordinates(const QPointF& vertex, const QSize& rect) const
	{
		return transform.standardQuadVertexToGLCoordinates(vertex, rect, getTextureSize());
	}

	QPointF View::standardQuadVertexToGLCoordinates(const QPointF& vertex) const
	{
		const QSize sceneSize = (scene()!=NULL) ? scene()->sceneRect().size().toSize() : QSize(1, 1);
		return standardQuadVertexToGLCoordinates(vertex, sceneSize);
	}

	QPointF View::pixelToFragmentCoordinates(const QPointF& pixel, const QRectF& rect, const bool& omitFlips) const
	{
		return transform.pixelToFragmentCoordinates(pixel, rect, getTextureSize(), omitFlips);
	}

	QPointF View::pixelToFragmentCoordinates(const QPointF& pixel, const bool& omitFlips) const
	{
		const QRectF sceneRect = (scene()!=NULL) ? scene()->sceneRect() : QRectF(0.0f, 0.0f, 1.0f, 1.0f);
		return pixelToFragmentCoordinates(pixel, sceneRect, omitFlips);
	}

	void View::resetTransform(const int& duration)
	{
		if(textureResource!=NULL && textureResource->getTexture()!=NULL)
			transform.reset(getTextureSize(), duration);
		else
			qWarning("View::resetTransform - Not attached to texture, reset skipped.");
	}	

	float View::getBaseScale(const QSize& sceneSize) const
	{
		if(textureResource!=NULL && textureResource->getTexture()!=NULL)
			return transform.getBaseScale(sceneSize, getTextureSize());
		else
			return 1.0f;
	}

	float View::getCompoundScale(const QSize& sceneSize) const
	{
		if(textureResource!=NULL && textureResource->getTexture()!=NULL)
			return transform.getCompoundScale(sceneSize, getTextureSize());
		else
			return 1.0f;
	}

	QGraphicsItem* View::getUnderlyingItem(void)
	{
		return this;
	}

	void View::populateTopBar(TopBar& bar)
	{
		bar.setWidget(TopBar::Center, &title);
		bar.setWidget(TopBar::Right, &controlsWidget);
	}

	void View::drawBackground(const QRectF& rect, Glip::CoreGL::HdlProgram& program, Glip::CorePipeline::GeometryInstance& quad, ViewTransform* t)
	{
		if(textureResource!=NULL && textureResource->getTexture()!=NULL && scene()!=NULL)
		{
			if(t==NULL)
				t = &transform;
			// Set the variables :
			glViewport(rect.left(), scene()->sceneRect().height() - rect.top() - rect.height(), rect.width(), rect.height());
			program.setVar("center", GL_FLOAT_VEC2, static_cast<float>(t->getCenter().x()), static_cast<float>(t->getCenter().y()));
			program.setVar("sceneSize", GL_FLOAT_VEC2, static_cast<float>(rect.width()), static_cast<float>(rect.height()));
			program.setVar("textureSize", GL_FLOAT_VEC2, static_cast<float>(textureResource->getTexture()->getWidth()), static_cast<float>(textureResource->getTexture()->getHeight()));
			program.setVar("c", GL_FLOAT, t->getAngleCosine());
			program.setVar("s", GL_FLOAT, t->getAngleSine());
			program.setVar("scale", GL_FLOAT, getCompoundScale(rect.size().toSize()));
			program.setVar("horizontalFlip", GL_INT, t->getHorizontalFlip() ? 1 : 0);
			program.setVar("verticalFlip", GL_INT, t->getVerticalFlip() ? 1 : 0);

			// Bind the texture :
			textureResource->getTexture()->bind();

			// Draw :
			quad.draw();
		}
	}

	void View::drawBackground(const QRectF& rect, Glip::CoreGL::HdlProgram& program, Glip::CorePipeline::GeometryInstance& quad)
	{
		if(isVisible())
			drawBackground(rect, program, quad, &transform);
		else
			std::cerr << "View::drawBackground - is not visible." << std::endl;
	}
	
	QRectF View::boundingRect(void) const
	{
		return (scene()!=NULL) ? scene()->sceneRect() : QRectF(0.0f, 0.0f, 1.0f, 1.0f);
	}
	
	void View::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{ }

// Vignette :
	Vignette::Vignette(View* ptr, QObject* parent)
	 :	QObject(parent),
		view(ptr),
		textureResource(NULL),
		frame(0.0f, 0.0f, 1.0f, 1.0f),
		titleBar(0.0f, 0.0f, 1.0f, TopBar::getHeight()),
		frameWidth(2.0f),
		frameHighlightedWidth(3.0f),
		frameColor(128, 128, 128),
		frameHighlightedColor(255, 255, 255),
		titleBackgroundColor(32, 32, 32, static_cast<unsigned char>(0.75f * 255.0f))
	{
		setObjectName("Vignette");
		if(view==NULL)
			qFatal("Vignette::Vignette - Cannot create a NULL view.");
		textureResource = view->getTextureResource();
		QObject::connect(view, SIGNAL(closed()), this, SLOT(viewDestroyed()));
		QObject::connect(view, SIGNAL(destroyed()), this, SLOT(viewDestroyed()));
		QObject::connect(textureResource, SIGNAL(destroyed()), this, SLOT(resourceDestroyed()));
		QObject::connect(textureResource, SIGNAL(formatChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(textureChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(nameChanged()), this, SLOT(updateTitle()));
		QObject::connect(textureResource, SIGNAL(informationChanged(const QString)), this, SLOT(updateTitle()));		
		addToGroup(&titleBar);
		addToGroup(&frame);
		addToGroup(&title);
		// Style :
		title.setPos(4, 6);
		titleBar.setBrush(titleBackgroundColor);
		titleBar.setPen(Qt::NoPen);
		title.setBrush(QColor(224, 224, 224));
		frame.setPen(QPen(frameColor, frameWidth));
		updateTitle();
	}

	Vignette::~Vignette(void)
	{
		view = NULL;
		textureResource = NULL;
	}

	void Vignette::updateTitle(void)
	{
		if(view!=NULL && textureResource!=NULL)
		{
			title.setText(textureResource->getName());
			titleBar.setToolTip(textureResource->getHTMLInformation());
		}
	}

	void Vignette::viewDestroyed(void)
	{
		view = NULL;
		textureResource = NULL;
	}

	void Vignette::resourceDestroyed(void)
	{
		view = NULL;
		textureResource = NULL;
	}

	View* Vignette::getView(void) const
	{
		return view;
	}

	QRectF Vignette::boundingRect(void) const
	{
		QRectF rect = frame.rect();
		rect.moveTo(scenePos());
		return rect;
	}

	void Vignette::resize(const QSize& s)
	{
		prepareGeometryChange();
		QRectF frameRect = frame.rect();
		frameRect.setSize(s);
		frame.setRect(frameRect);
		QRectF titleBarRect = titleBar.rect();
		titleBarRect.setWidth(s.width());
		titleBar.setRect(titleBarRect);
	}

	void Vignette::enableSelectionHighlight(const bool& enable)
	{
		if(enable)
			frame.setPen(QPen(frameHighlightedColor, frameHighlightedWidth));
		else
			frame.setPen(QPen(frameColor, frameWidth));
	}

// Gallery :
	Gallery::Gallery(QObject* parent)
	 :	AbstractGLDrawableObject(parent),
		numColumns(1),
		numRows(1),
		vignetteSize(1,1),
		horizontalSpacing(1.0f),
		verticalSpacing(1.0f)
	{
		QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, true);
		QGraphicsItemGroup::hide();
		std::cout << "Gallery : " << this << std::endl;
	}

	Gallery::~Gallery(void)
	{
		while(!views.empty())
		{
			QList<QPair<View*, Vignette*> >::iterator it = views.begin();
			View* view = it->first;
			Vignette* vignette = it->second;
			views.erase(it);
			delete view;
			delete vignette;
		}
	}

	QList<QPair<View*, Vignette*> >::iterator Gallery::getIterator(const View* view)
	{
		QList<QPair<View*, Vignette*> >::iterator it = views.begin();
		for(; it!=views.end(); it++)
		{
			if(it->first==view)
				break;
		}
		return it;
	}

	bool Gallery::contains(const TextureResource* resource) const
	{
		for(QList<QPair<View*, Vignette*> >::const_iterator it=views.begin(); it!=views.end(); it++)
		{
			if(it->first->getTextureResource()==resource)
				return true;
		}
		return false;
	}

	void Gallery::computeTableParameters(const QSize& size, int N)
	{
		const float rho = 0.04f; // percentage of spacing.
		if(N<1)
			N = views.size();
		const int topBarHeight = TopBar::getHeight();
		const float 	W = size.width(),
				H = size.height() - static_cast<float>(topBarHeight);
		const float sceneRatio = W / H;
		numColumns = std::ceil(std::sqrt(sceneRatio*static_cast<float>(N)) - 0.5); // std::round(x) \approx std::ceil(x-0.5)
		numRows = std::ceil(static_cast<float>(N)/static_cast<float>(numColumns));
		vignetteSize.setWidth(static_cast<int>((1.0f - rho) * W / static_cast<float>(numColumns)));
		vignetteSize.setHeight(static_cast<int>((1.0f - rho) * H / static_cast<float>(numRows)));
		horizontalSpacing = (W - static_cast<float>(numColumns*vignetteSize.width())) / static_cast<float>(numColumns + 1);
		verticalSpacing = (H - static_cast<float>(numRows*vignetteSize.height())) / static_cast<float>(numRows + 1);
	}	

	QVariant Gallery::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
	{
		if((change==QGraphicsItem::ItemSceneChange || change==QGraphicsItem::ItemSceneHasChanged) && scene()!=NULL)
		{	
			// Resize to the scene rectangle :
			for(QList<QPair<View*, Vignette*> >::iterator it=views.begin(); it!=views.end(); it++)
				it->first->resetTransform();
			resize();
			QObject::connect(scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(resize(const QRectF&)));
			return value;
		}
		else
			return QGraphicsItem::itemChange(change, value);
	}

	void Gallery::updateSelection(const QPointF& pos, const bool& add, const bool& remove, const bool& clear)
	{
		if(!add && clear)
		{
			selection.clear();
			selectionViews.clear();
		}
		for(QList<QPair<View*, Vignette*> >::iterator it=views.begin(); it!=views.end(); it++)
		{
			// Vignette under the mouse :
			if(it->second->boundingRect().contains(pos))
			{
				const int id = selection.indexOf(it->second);
				if(id<0)
				{
					selection.push_back(it->second);
					selectionViews.push_back(it->first);
					it->second->enableSelectionHighlight(true);
				}
				else if(remove)
				{
					selection.remove(id);
					selectionViews.remove(id);
					it->second->enableSelectionHighlight(false);
				}
				else // Put at the back :
				{
					std::swap(selection[id], selection.back());
					std::swap(selectionViews[id], selectionViews.back());
				}
			}
			else if(!add)
				it->second->enableSelectionHighlight(false);
		}
	}

	void Gallery::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		const bool modifier = (event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))!=0;
		updateSelection(event->scenePos(), modifier, false, true);
	}

	//void Gallery::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
	//{ }

	void Gallery::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
	{
		for(QList<QPair<View*, Vignette*> >::iterator it=views.begin(); it!=views.end(); it++)
		{
			if(it->second->boundingRect().contains(event->scenePos()))
			{
				//hide();
				it->first->showObject();
			}
		}
	}

	void Gallery::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
	{
		if(!selection.empty() && scene()!=NULL)
			processMouseMoveEvent(selection.back()->boundingRect(), event, selectionViews.back(), selectionViews.data(), selectionViews.size()-1);
	}

	void Gallery::wheelEvent(QGraphicsSceneWheelEvent* event)
	{
		if(scene()!=NULL)
		{
			updateSelection(event->scenePos(), true, false, false);
			processWheelEvent(selection.back()->boundingRect(), event, selectionViews.back(), selectionViews.data(), selectionViews.size()-1);
		}
	}

	void Gallery::keyPressEvent(QKeyEvent* event)
	{
		if(!selection.empty() && scene()!=NULL)
			processKeyPressEvent(selection.back()->boundingRect(), event, selectionViews.back(), selectionViews.data(), selectionViews.size()-1);
	}

	void Gallery::processGotoNextView(void)
	{
		View* view = reinterpret_cast<View*>(QObject::sender());
		
	}

	void Gallery::processGotoPreviousView(void)
	{
		View* view = reinterpret_cast<View*>(QObject::sender());
	}

	void Gallery::processGotoParentGallery(void)
	{
		showObject();
	}

	void Gallery::viewClosed(void)
	{
		// Delete the view and the vignette :
		View* view = reinterpret_cast<View*>(QObject::sender());
		QList<QPair<View*, Vignette*> >::iterator it = getIterator(view);
		if(it!=views.end())
		{
			Vignette* vignette = it->second;
			views.erase(it);
			delete view;
			delete vignette;
			// Remove from selection :
			const int k = selection.indexOf(vignette);
			if(k>=0)
			{
				selection.remove(k);
				selectionViews.remove(k);
			}
			// Reform the array :
			resize();
		}
	}
	
	void Gallery::resize(const QRectF& rect)
	{
		// Update the vignette positions :
		computeTableParameters(rect.size().toSize());
		int j = 0,
		    i = 0;
		for(QList<QPair<View*, Vignette*> >::iterator it=views.begin(); it!=views.end(); it++)
		{
			QPointF pos(	static_cast<int>(static_cast<float>(j) * (static_cast<float>(vignetteSize.width()) + horizontalSpacing) + horizontalSpacing),
					static_cast<int>(static_cast<float>(i) * (static_cast<float>(vignetteSize.height()) + verticalSpacing) + verticalSpacing) + TopBar::getHeight());
			it->second->resize(vignetteSize);
			it->second->setPos(pos);
			// Next Coordinate :
			j++;
			if(j>=numColumns)
			{
				j = 0;
				i++;
			}
		}
	}

	void Gallery::resize(void)
	{
		if(scene()!=NULL)
			resize(scene()->sceneRect());
	}	

	void Gallery::addView(TextureResource* resource, const bool& allowDuplicate)
	{
		if(resource!=NULL && (!contains(resource) || allowDuplicate))
		{
			View* view = new View(resource);
			Vignette* vignette = new Vignette(view, this);
			views.push_back(QPair<View*, Vignette*>(view, vignette));
			addToGroup(vignette);
			vignette->show();
			QObject::connect(view, SIGNAL(updateScene()), this, SIGNAL(updateScene()));
			QObject::connect(view, SIGNAL(closed()), this, SLOT(viewClosed()));
			QObject::connect(view, SIGNAL(gotoPreviousObject()), this, SLOT(processGotoPreviousView()));
			QObject::connect(view, SIGNAL(gotoNextObject()), this, SLOT(processGotoNextView()));
			QObject::connect(view, SIGNAL(gotoParentObject()), this, SLOT(processGotoParentGallery()));
			emit addSubObject(view);
			// Update the array :
			resize();
			view->resetTransform();
		}
	}	

	QGraphicsItem* Gallery::getUnderlyingItem(void)
	{
		return this;
	}

	void Gallery::populateTopBar(TopBar& bar)
	{

	}

	void Gallery::drawBackground(const QRectF& rect, Glip::CoreGL::HdlProgram& program, Glip::CorePipeline::GeometryInstance& quad)
	{
		if(isVisible())
		{
			for(QList<QPair<View*, Vignette*> >::iterator it=views.begin(); it!=views.end(); it++)
				it->first->drawBackground(it->second->boundingRect(), program, quad, &it->first->transform);
		}
	}

	QList<AbstractGLDrawableObject*> Gallery::getSubObjects(void) const
	{
		QList<AbstractGLDrawableObject*> result;
		for(QList<QPair<View*, Vignette*> >::const_iterator it=views.begin(); it!=views.end(); it++)
			result.push_back(it->first);
		return result;
	}

	QRectF Gallery::boundingRect(void) const
	{
		return (scene()!=NULL) ? scene()->sceneRect() : QRectF(0.0f, 0.0f, 1.0f, 1.0f);
	}
	
// GLContext :
	GLContext::GLContext(QGLContext* ctx, QWidget* parent)
	 :	QGLWidget(ctx, parent),
		glipOwnership(false)
	{
		try
		{
			makeCurrent();
			if(!Glip::HandleOpenGL::isInitialized())
			{
				Glip::HandleOpenGL::init();
				glipOwnership = true;
			}
		}
		catch(Glip::Exception& e)
		{
			qFatal("GLContext::GLContext - Error while creating context :\n%s", e.what()); 
		}
	}

	GLContext::~GLContext(void)
	{
		if(glipOwnership)
		{
			try
			{
				Glip::HandleOpenGL::deinit();
			}
			catch(...)
			{ }
		}
	}	

// GLScene :
	const std::string GLScene::displayVertexShaderSource = 		"#version 130\n"
									"uniform vec2	center = vec2(0.5, 0.5),\n"
									"		textureSize = vec2(1.0, 1.0),\n"
									"		sceneSize = vec2(1.0, 1.0);\n"
									"uniform float	c = 1.0, s=0.0,\n"
									"		scale = 1.0;\n"
									"uniform int	verticalFlip = 0,\n"
									"		horizontalFlip = 0;\n"
									"void main(void)\n"
									"{\n"
									"	vec2 p = gl_Vertex.xy;\n"
									"	if(horizontalFlip>0) p.x = (1.0-p.x) / 2.0 * textureSize.x - center.x;\n"
									"	else                 p.x = (p.x+1.0) / 2.0 * textureSize.x - center.x;\n"
									"	if(verticalFlip>0)   p.y = (1.0-p.y) / 2.0 * textureSize.y - center.y;\n"
									"	else                 p.y = (p.y+1.0) / 2.0 * textureSize.y - center.y;\n"
									"	p = mat2(c, -s, s, c) * p;\n"
									"	p.x = p.x * scale / sceneSize.x * 2.0f;\n"
									"	p.y = p.y * scale / sceneSize.y * 2.0f;\n"
									"	gl_Position = vec4(p, 0.0, 1.0);\n"
									"	gl_TexCoord[0] = gl_MultiTexCoord0;\n"
									"}\n";
	
	const std::string GLScene::displayFragmentShaderSource = 	"#version 130\n"
									"uniform sampler2D t;\n"
									"void main(void)\n"
									"{\n"
									//"	gl_FragColor = textureLod(t, gl_TexCoord[0].st, 0.0);\n"
									"	gl_FragColor = 0.5*textureLod(t, gl_TexCoord[0].st, 0.0) + 0.5*vec4(gl_TexCoord[0].st, 0.0, 0.0);\n"
									"}\n";

	GLScene::GLScene(QObject* parent)
	 :	QGraphicsScene(parent),
		quad(NULL),
		program(NULL),
		backgroundColor(32, 32, 64)
	{
		try
		{
			// Create Glip Resources :
			quad = new Glip::CorePipeline::GeometryInstance(Glip::CorePipeline::GeometryPrimitives::StandardQuad(), GL_STATIC_DRAW_ARB);
			Glip::CoreGL::HdlShader	vertexShader(GL_VERTEX_SHADER, displayVertexShaderSource),
						fragmentShader(GL_FRAGMENT_SHADER, displayFragmentShaderSource);
			program = new Glip::CoreGL::HdlProgram;
			program->updateShader(vertexShader, false);
			program->updateShader(fragmentShader, false);
			program->link();	
		}
		catch(Glip::Exception& e)
		{
			qFatal("GLScene::GLScene - Error while creating the scene :\n%s", e.what());
		}
		addItem(&topBar);
	}

	GLScene::~GLScene(void)
	{
		// Clean :
		objects.clear();
		delete quad;
		delete program;
	}

	void GLScene::drawBackground(QPainter* painter, const QRectF& rect)
	{
		UNUSED_PARAMETER(painter)
		UNUSED_PARAMETER(rect)	
		glViewport(0, 0, width(), height());
		glClearColor(backgroundColor.red()/255.0f, backgroundColor.green()/255.0f, backgroundColor.blue()/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable transparency on the background :
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw :	
		for(QVector<AbstractGLDrawableObject*>::iterator it=objects.begin(); it!=objects.end(); it++)
		{
			if((*it)->getUnderlyingItem()->isVisible())
			{
				(*it)->drawBackground(sceneRect(), *program, *quad);
				glViewport(0, 0, width(), height());
			}
		}
	}	

	void GLScene::showObject(void)
	{
		AbstractGLDrawableObject* obj = reinterpret_cast<AbstractGLDrawableObject*>(QObject::sender());
		for(QVector<AbstractGLDrawableObject*>::iterator it=objects.begin(); it!=objects.end(); it++)
		{
			if(*it!=obj) // Avoid flicker.
				(*it)->getUnderlyingItem()->hide();
		}
		// Show :
		obj->populateTopBar(topBar);
		obj->getUnderlyingItem()->show();
		// Make sure the top bar is on top :
		obj->getUnderlyingItem()->stackBefore(&topBar);
	}

	void GLScene::objectDestroyed(void)
	{
		AbstractGLDrawableObject* ptr = reinterpret_cast<AbstractGLDrawableObject*>(QObject::sender());
		const int id = objects.indexOf(ptr);
		if(id>=0)
			objects.remove(id);
	}	

	const QColor& GLScene::getBackgroundColor(void) const
	{
		return backgroundColor;
	}

	void GLScene::setBackgroundColor(const QColor& c)
	{
		backgroundColor = c;
	}

	void GLScene::addObject(AbstractGLDrawableObject* ptr)
	{
		if(!objects.contains(ptr))
		{
			// Add this object :
			objects.push_back(ptr);
			addItem(ptr->getUnderlyingItem());
			QObject::connect(ptr, SIGNAL(destroyed()), this, SLOT(objectDestroyed()));
			// Add all of its sub-objects :
			QList<AbstractGLDrawableObject*> subObjects = ptr->getSubObjects();
			for(QList<AbstractGLDrawableObject*>::iterator it=subObjects.begin(); it!=subObjects.end(); it++)
				addObject(*it);
			// Connect the required signals :
			QObject::connect(ptr, SIGNAL(addSubObject(AbstractGLDrawableObject*)), this, SLOT(addObject(AbstractGLDrawableObject*)));
			QObject::connect(ptr, SIGNAL(showRequest()), this, SLOT(showObject()));
		}
	}

// Viewer :
	Viewer::Viewer(QWidget* parent)
	 :	QGraphicsView(parent),
		glContext(NULL),
		glScene(NULL)
	{
		// Create the underlying objects :
		const QGLFormat glFormat(QGL::DepthBuffer | QGL::DoubleBuffer);
		QGLContext* _glContext = new QGLContext(glFormat);
		glContext = new GLContext(_glContext, this);
		setViewport(glContext);
		glScene = new GLScene(this);
		setScene(glScene);
		glScene->setSceneRect(QRect(0, 0, width(), height()));

		// Settings :
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // Because GL has to redraw the complete area.

		// Main gallery :
		galleries.push_back(createGallery());
		galleries.front()->showObject();

		// TESTS :
		/*TopBar* bar = new TopBar;
		glScene->addItem(bar);	
		bar->setWidget(TopBar::Left, new QLabel("Left"));
		bar->setWidget(TopBar::Center, new QLabel("Center"));
		bar->setWidget(TopBar::Right, new QLabel("Right"));*/

		// More TESTS :
		/*Glip::Modules::LayoutLoader loader;
		Glip::Modules::LayoutLoaderModule::addBasicModules(loader);
		Glip::CorePipeline::Pipeline* pipeline = loader.getPipeline("../Filters/glipLogo.ppl");
		(*pipeline) << Glip::CorePipeline::Pipeline::Process;
		TextureResource* rsc = new TextureResource(&pipeline->out(), "Test Texture");*/
		
		// Single View :
		/*View* view = new View(rsc);
		glScene->addItem(view);
		view->resetTransform();
		view->transform.setHorizontalFlip(true);
		view->transform.setVerticalFlip(true);
		view->show();
		QObject::connect(view, SIGNAL(updateScene()), this, SLOT(updateScene()));*/

		// Gallery :
		/*Gallery* gallery = new Gallery(this);
		gallery->addView(rsc, true);
		gallery->addView(rsc, true);
		gallery->addView(rsc, true);
		glScene->addObject(gallery);
		gallery->showObject();
		//QObject::connect(gallery, SIGNAL(updateScene()), this, SLOT(updateScene()));
		QObject::connect(gallery, SIGNAL(updateScene()), glScene, SLOT(update()));*/
	}
	
	Viewer::~Viewer(void)
	{
		while(!galleries.empty())
			delete galleries.front();
		delete glScene;
		delete glContext;
	}	

	void Viewer::resizeEvent(QResizeEvent* event)
	{
		glScene->setSceneRect(QRect(QPoint(0, 0), event->size()));
		QGraphicsView::resizeEvent(event);
	}
	
	void Viewer::galleryDestroyed(void)
	{
		Gallery* gallery = reinterpret_cast<Gallery*>(QObject::sender());
		const int k = galleries.indexOf(gallery);
		if(k>=0)
			galleries.remove(k);
	}

	Gallery* Viewer::createGallery(void)
	{
		Gallery* gallery = new Gallery(this);
		glScene->addObject(gallery);
		QObject::connect(gallery, SIGNAL(updateScene()), glScene, SLOT(update()));
		QObject::connect(gallery, SIGNAL(destroyed()), this, SLOT(galleryDestroyed()));
		return gallery;
	}

	void Viewer::addView(TextureResource* ptr, const bool allowDuplicate)
	{
		if(!galleries.empty())
			galleries.front()->addView(ptr, allowDuplicate);
	}

