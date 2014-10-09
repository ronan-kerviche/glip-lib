/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GLSceneWidget.cpp                                                                         */
/*     Original Date : May 26th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for visualization with OpenGL.                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "GLSceneWidget.hpp"

using namespace QVGL;

// View :
	View::View(const QString& _name)
	 : 	texture(NULL),
		name(_name),
		qvglParent(NULL),
		angle(0.0f),
		homothecyScale(1.0f),
		flipUpDown(false),
		flipLeftRight(false)
	{
		reset();
	}

	View::View(HdlTexture* _texture, const QString& _name)
	 : 	texture(_texture),
		name(_name),
		qvglParent(NULL),
		angle(0.0f),
		homothecyScale(1.0f),
		flipUpDown(false),
		flipLeftRight(false)
	{
		reset();
	}

	View::~View(void)
	{
		qvglParent = NULL;
		texture = NULL;
	}

	void View::prepareToDraw(void)
	{
		if(texture!=NULL)
			texture->bind();
	}

	bool View::isValid(void) const
	{
		return (texture!=NULL);
	}

	const HdlAbstractTextureFormat& View::getFormat(void) const
	{
		if(texture==NULL)
			throw Exception("View::getFormat - View is invalid.", __FILE__, __LINE__);
		else
			return *texture;
	}

	const QString& View::getName(void) const
	{
		return name;
	}

	void View::setName(const QString& newName)
	{
		name = newName;

		emit nameChanged();
	}

	void View::setTexture(HdlTexture* _texture)
	{
		texture = _texture;
	
		emit updated();
	}

	float View::getAngle(void) const
	{
		return angle;
	}

	void View::setAngle(const float& a)
	{
		angle = a;
		
		emit updated();
	}

	void View::rotate(const float& a)
	{
		angle += a;

		emit updated();
	}

	void View::getViewCenter(float& x, float& y) const
	{
		x = viewCenter[0];
		y = viewCenter[1];
	}

	void View::setViewCenter(const float& x, const float& y)
	{
		viewCenter[0] = x;
		viewCenter[1] = y;

		emit updated();
	}

	void View::move(const float& x, const float& y)
	{
		viewCenter[0] += x;
		viewCenter[1] += y;

		emit updated();
	}

	bool View::isMirrored(void) const
	{
		return flipLeftRight;
	}

	void View::setMirror(bool enabled)
	{
		flipLeftRight = enabled;

		emit updated();
	}

	bool View::isUpsideDown(void) const
	{
		return flipUpDown;	
	}

	void View::setUpsideDown(bool enabled)
	{
		flipUpDown = enabled;

		emit updated();
	}

	void View::zoom(const float& xCenter, const float& yCenter, const float& factor)
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

	void View::reset(void)
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

	void View::show(void)
	{
		emit requireDisplay();
	}

	void View::close(void)
	{
		emit closed();
	}

	bool View::isClosed(void)
	{
		return qvglParent==NULL;
	}

	float View::getImageRatio(void) const
	{
		if(texture==NULL)
			return 0.0f;
		else
			return static_cast<float>(texture->getWidth()) / static_cast<float>(texture->getHeight());
	}
	
	void View::getAspectRatioScaling(float& xImgScale, float& yImgScale) const
	{
		const float imgRatio = getImageRatio();

		xImgScale = std::min(1.0f, imgRatio);
		yImgScale = std::min(1.0f, 1.0f/imgRatio);

		if(flipLeftRight)	xImgScale *= -1.0f;
		if(flipUpDown) 		yImgScale *= -1.0f;
	}

	QString View::getSizeString(void) const
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

// Vignette :
		QColor	Vignette::frameColor		= QColor(128, 	128, 	128),
			Vignette::selectedFrameColor	= QColor(192, 	192, 	192),
			Vignette::titleBarColor		= QColor(32, 	 32, 	 32, static_cast<unsigned char>(titleBarOpacity * 255.0f)),
			Vignette::titleColor		= QColor(224, 	224, 	224),
			Vignette::infosColor		= QColor(160, 	160,	160);
		float	Vignette::frameThickness	= 3.0f,
			Vignette::titleBarOpacity	= 0.75f;

		Vignette::Vignette(View* _view)
		 : 	QObject(NULL),
			QGraphicsItemGroup(NULL),
			selected(false),			
			view(_view),
			frame(0, 0, 1, 1),
			titleBar(0, 0, 1, 1)
		{
			setFiltersChildEvents(true);
			setAcceptHoverEvents(true);

			addToGroup(&titleBar);
			addToGroup(&frame);			
			addToGroup(&title);
			addToGroup(&infos);

			// Title positions : 
			title.setPos(6, 0);

			// Set colors : 
			setSelection(false);
			
			QBrush brush(titleBarColor);
			titleBar.setBrush(brush);
			titleBar.setPen(Qt::NoPen);

			title.setBrush(titleColor);
			infos.setBrush(infosColor);

			// Update content : 
			updateTitle();
			updateInfos();
	
			// Connect : 
			QObject::connect(view, SIGNAL(nameChanged(void)), 	this, SLOT(updateTitle(void)));
			QObject::connect(view, SIGNAL(updated(void)), 		this, SLOT(updateInfos(void)));
		}

		Vignette::~Vignette(void)
		{
			removeFromGroup(&frame);
			removeFromGroup(&titleBar);
			removeFromGroup(&title);
		}

		void Vignette::setTitleBarHeight(void)
		{
			QRectF titleRect = titleBar.rect();

			int hRect = std::max(title.boundingRect().height(), infos.boundingRect().height());

			titleRect.setHeight(hRect);
			titleBar.setRect(titleRect);
		}

		void Vignette::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
		{
			if(view!=NULL)
			{
				event->accept();
				emit showView(view);
			}
			else
				event->ignore();
		}

		/*void Vignette::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
		{
			std::cout << "Vignette::mouseMoveEvent" << std::endl;
			event->ignore();
		}*/

		void Vignette::mousePressEvent(QGraphicsSceneMouseEvent* event)
		{
			const QRectF rectF = boundingRect();

			// It seems Qt is sending event to items for which the event is actually outside the bounding rectangle.
			// Not sure if a bug, but it is simple to test and prevent : 
			if(!rectF.contains(event->scenePos()))
				event->ignore();
			else if(!selected)
			{
				emit selection();
				event->accept();
			}
			else
				event->ignore();
		}

		/*void Vignette::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
		{
			std::cout << "Vignette::mouseReleaseEvent" << std::endl;
			event->ignore();
		}*/

		View* Vignette::getView(void)
		{
			return view;
		}

		int Vignette::getWidth(void) const
		{
			return frame.rect().width();
		}
	
		int Vignette::getHeight(void) const
		{
			return frame.rect().height();
		}

		void Vignette::resize(const QSize& size)
		{
			prepareGeometryChange();

			QRectF frameRect = frame.rect();
			frameRect.setSize(size);
			frame.setRect(frameRect);

			QRectF titleBarRect = titleBar.rect();
			titleBarRect.setWidth(size.width());
			titleBar.setRect(titleBarRect);

			// Force the update of title and infos to fit the news sizes :
			updateTitle();
			updateInfos();
		}

		bool Vignette::isSelected(void) const
		{
			return selected;
		}

		void Vignette::setSelection(bool enabled)
		{
			selected = enabled;

			if(enabled)
				frame.setPen(QPen(selectedFrameColor, frameThickness));
			else
				frame.setPen(QPen(frameColor, frameThickness));
		}

		QRectF Vignette::boundingRect(void) const
		{
			QRectF rect = frame.rect();
			rect.moveTo(scenePos());
			return rect;
		}

		void Vignette::updateTitle(void)
		{
			QString titleStr = view->getName();

			// Compute the maximum number of characters to fill 80% max :
			QFontMetrics metrics(title.font());
			int maxCharacters = static_cast<int>(0.8f * static_cast<float>(getWidth())/static_cast<float>(metrics.maxWidth()));

			if(titleStr.size()>maxCharacters)
				titleStr = tr("%1...").arg(titleStr.left(maxCharacters-3));

			title.setText(titleStr);
			setTitleBarHeight();
		}

		void Vignette::updateInfos(void)
		{
			QString text;
			if(view->isValid())
				text = tr("%1x%2").arg(view->getFormat().getWidth()).arg(view->getFormat().getHeight());
			else
				text = "(Invalid View)";
			
			QFontMetrics metrics(infos.font());
			infos.setText(text);
			infos.setPos(getWidth() - metrics.width(text) - 6, 0);

			setTitleBarHeight();
		}
	
// ViewsTable :
	float ViewsTable::rho = 0.04;

	ViewsTable::ViewsTable(const QString& tableName)
	 :	QGraphicsItemGroup(NULL),
		name(tableName),
		emptyNotification("(Empty)")//,
		//visible(true)
	{
		// Let its children handle their own notification : 
		setHandlesChildEvents(false);

		addToGroup(&emptyNotification);
		emptyNotification.setBrush(Qt::white);

		setVisible(false);
	}

	ViewsTable::~ViewsTable(void)
	{
		clear();
		emit closed();
	}

	void ViewsTable::computeTableParameters(const QRectF& size, int N)
	{
		// IN
		// W 	: width of the scene.
		// H	: height of the scene.
		// N 	: number of elements to display.
		// rho 	: percentage of spacing.
		// OUT
		// a 	: number of columns of the table.
		// b 	: number of rows of the table.
		// w	: width of each vignette.
		// h 	: height of each vignette.
		// u 	: horizontal spacing, adjusted.
		// v 	: vertical spacing, adjusted.

		if(N<1)
			N = vignettesList.size();

		topBarHeight = TopBar::getHeight();

		const float 	W = size.width();
				H = size.height() - static_cast<float>(topBarHeight);

		const float sceneRatio = W / H;

		// ROUND REQUIRES : 
		// QT <= 4 : QMAKE_CXXFLAGS += -std=c++11
		// QT >= 5 : CONFIG += c++11

		a = std::round(std::sqrt(sceneRatio*static_cast<float>(N)));
		b = std::ceil(static_cast<float>(N)/static_cast<float>(a));

		w = static_cast<int>((1.0f - rho) * W / static_cast<float>(a));
		h = static_cast<int>((1.0f - rho) * H / static_cast<float>(b));

		u = (W - static_cast<float>(a*w)) / static_cast<float>(a + 1);
		v = (H - static_cast<float>(b*h)) / static_cast<float>(b + 1);
	}

	void ViewsTable::getIndices(const Vignette* vignette, int& i, int& j) const
	{
		int 	idx 	= -1,
			k 	= 0;

		// Find the index of the Vignette as a value of the map : 
		for(QMap<View*, Vignette*>::const_iterator it=vignettesList.begin(); it!=vignettesList.end(); it++)
		{
			if(it.value()==vignette)
			{
				idx = k;
				break;
			}
			else
				k++;
		}			

		if(idx<0)
			throw Exception("ViewsTable::getIndices - Vignette not listed.", __FILE__, __LINE__);
		else
		{
			j = idx % a;
			i = (idx - j) / a;
		}
	}

	QPoint ViewsTable::getScenePosition(const int& i, const int& j) const
	{
		const int 	x = static_cast<int>(static_cast<float>(j) * (static_cast<float>(w) + u) + u),
				y = static_cast<int>(static_cast<float>(i) * (static_cast<float>(h) + v) + v) + topBarHeight;

		return QPoint(x, y);
	}

	QPoint ViewsTable::getScenePosition(const Vignette* vignette) const
	{
		int i, j;

		getIndices(vignette, i, j);

		return getScenePosition(i, j);
	}

	QVariant ViewsTable::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
	{
		if((change==QGraphicsItem::ItemSceneChange || change==QGraphicsItem::ItemSceneHasChanged) && scene()!=NULL)
		{
			resize();
			QObject::connect(scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(resize(const QRectF&)));

			return value;
		}
		else
			return QGraphicsItem::itemChange(change, value);
	}

	/*void ViewsTable::setVisible(bool enabled)
	{
		visible = enabled;
		for(QMap<View*, Vignette*>::iterator it=vignettesList.begin(); it!=vignettesList.end(); it++)
			it.value()->setVisible(enabled);
	}*/

	void ViewsTable::resize(const QRectF& size)
	{
		// Recompute parameters : 
		computeTableParameters(size);

		// Propagate to the vignettes : 
		const QSize vignetteSize(w, h);
		for(QMap<View*, Vignette*>::iterator it=vignettesList.begin(); it!=vignettesList.end(); it++)
		{
			(*it)->resize(vignetteSize);
			(*it)->setPos(getScenePosition(*it));
		}

		// Reset the right position for the the label :
		emptyNotification.setPos(size.width()/2.0, size.height()/2.0);
	}

	void ViewsTable::resize(void)
	{
		if(scene()!=NULL)
			resize(scene()->sceneRect());
	}

	void ViewsTable::updateSelection(void)
	{
		Vignette* sender = reinterpret_cast<Vignette*>(QObject::sender());

		for(QMap<View*, Vignette*>::iterator it=vignettesList.begin(); it!=vignettesList.end(); it++)
		{
			if(it.value()->isSelected() && it.value()!=sender)
				it.value()->setSelection(false);
			else if(it.value()==sender)
				it.value()->setSelection(true);
		}

		emit viewSelection(sender->getView());
	}

	void ViewsTable::viewClosed(void)
	{
		View* view = reinterpret_cast<View*>(QObject::sender());

		removeView(view);
	}

	QMap<View*, Vignette*>::iterator	ViewsTable::begin(void)		{ return vignettesList.begin(); }
	QMap<View*, Vignette*>::const_iterator	ViewsTable::begin(void) const	{ return vignettesList.begin(); }
	QMap<View*, Vignette*>::iterator	ViewsTable::end(void)		{ return vignettesList.end(); }
	QMap<View*, Vignette*>::const_iterator	ViewsTable::end(void) const	{ return vignettesList.end(); }

	const QString& ViewsTable::getName(void) const
	{
		return name;
	}

	void ViewsTable::setName(const QString& newName)
	{
		name = newName;

		emit nameChanged();
	}

	void ViewsTable::getGLPositionOfVignette(const Vignette* vignette, int& x, int& y) const
	{
		if(vignette==NULL)
		{
			x = 0;
			y = 0;
		}
		else
		{
			x = vignette->x();
			y = H - vignette->y() - h + topBarHeight;
		}
	}
	
	QRectF ViewsTable::getVignetteFrame(View* view) const
	{
		QMap<View*, Vignette*>::const_iterator it = vignettesList.find(view);

		if(it!=vignettesList.end())
			return it.value()->boundingRect();
		else
			return QRectF(0.0f, 0.0f, 0.0f, 0.0f);
	}

	/*bool ViewsTable::isVisible(void) const
	{
		return visible;
	}*/
	
	bool ViewsTable::isClosed(void) const
	{
		return (scene()==NULL);
	}

	/*void ViewsTable::setVisible(bool enabled) // TEST, TO REMOVE
	{
		std::cout << "ViewsTable::setVisible - \"" << getName().toStdString() << "\" : " << enabled << std::endl;

		QGraphicsItemGroup::setVisible(enabled);
	}*/

	void ViewsTable::addView(View* view, bool resizeNow)
	{
		if(view==NULL)
			return ;
		
		if(!vignettesList.contains(view))
		{
			Vignette* v = new Vignette(view);
			addToGroup(v);
			vignettesList[view] = v;

			//useless? v->setVisible(isVisible());

			QObject::connect(v, 	SIGNAL(selection()), 		this, SLOT(updateSelection()));
			QObject::connect(v, 	SIGNAL(showView(View*)),	this, SIGNAL(showView(View*)));
			QObject::connect(view, 	SIGNAL(closed()),		this, SLOT(viewClosed()));
			QObject::connect(view, 	SIGNAL(destroyed()),		this, SLOT(viewClosed()));

			if(resizeNow)
				resize();

			emptyNotification.hide();
		}
	}

	void ViewsTable::addViews(const QList<View*>& viewsList)
	{
		// Build the vignettes :
		for(QList<View*>::const_iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			addView(*it, false);

		resize();
	}

	void ViewsTable::removeView(View* view, bool resizeNow)
	{
		if(vignettesList.contains(view))
		{
			if(scene()!=NULL)
				scene()->removeItem(vignettesList[view]);

			delete vignettesList[view];
			vignettesList.remove(view);

			if(resizeNow)
				resize();

			if(vignettesList.empty())
				emptyNotification.show();
		}
	}

	void ViewsTable::removeViews(const QList<View*>& viewsList)
	{
		for(QList<View*>::const_iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			removeView(*it, false);

		resize();
	}

	void ViewsTable::clear(void)
	{
		while(!vignettesList.isEmpty())
			removeView(vignettesList.begin().key());
	}

	void ViewsTable::show(void)
	{
		emit requireDisplay();
	}

	void ViewsTable::close(void)
	{
		emit closed();
	}

// SubWidget :
	SubWidget::SubWidget(void)
	 : 	layout(this),
		titleBar(&titleWidget),
		titleLabel(this),
		hideButton(this),
		widget(NULL),
		visible(false),
		motionActive(false),
		resizeActive(false),
		resizeHorizontalLock(false),
		resizeVerticalLock(false),
		qvglParent(NULL),
		graphicsProxy(NULL),
		anchorMode(AnchorFree)
	{
		// But first : 
		QWidget::hide();

		// Activate focus via mouse click :
		setFocusPolicy(Qt::ClickFocus);

		// Create title bar : 
		titleLabel.setTextFormat(Qt::PlainText);
		setTitle("Title");

		// Buttons : 
		const int 	w = 32,
				h = 16;
		hideButton.setMinimumSize(w, h);
		hideButton.setMaximumSize(w, h);
		hideButton.setArrowType(Qt::UpArrow);
		hideButton.setToolTip("Hide");

		titleBar.addWidget(&titleLabel);
		titleBar.addWidget(&hideButton);
		titleBar.setMargin(0);
		titleBar.setSpacing(0);

		titleWidget.setFixedHeight(h);

		// Create final layout : 
		layout.addWidget(&titleWidget, 0);
		layout.setMargin(4);
		layout.setSpacing(1);

		// Connections : 
		QObject::connect(&hideButton, SIGNAL(released(void)), this, SLOT(hide(void)));
	}

	SubWidget::~SubWidget(void)
	{
		if(graphicsProxy!=NULL)
		{
			graphicsProxy->setWidget(NULL);
			delete graphicsProxy;
			graphicsProxy = NULL;
		}
	}

	void SubWidget::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
		{
			const QPoint 	cw 		 = mapFromGlobal(event->globalPos());
					mousePress  	 = mapToParent(cw);
					sizeAtMousePress = size();

			// Vector from the initial widget position to the start click position. After that, move the window to the current cursor position plus this vector : 
			offset = mousePress - pos(); 

			const int borderResize = 2 * layout.margin();
			resizeActive = (cw.x()<borderResize) || ((width()-cw.x())<borderResize) || ((height()-cw.y())<borderResize); // All borders except the title.
			motionActive = !resizeActive;

			// Further test on the resize action : 
			if(resizeActive)
			{
				resizeHorizontalLock 	= ((height()-cw.y())>borderResize) || anchorMode!=AnchorFree; // All anchors lock the vertical resize
				resizeVerticalLock	= ((cw.x()>borderResize) && ((width()-cw.x())>borderResize)) || anchorMode==AnchorMaximized; // Only the maximized anchor lock the horizontal resize.
				resizeActive		= !(resizeHorizontalLock && resizeVerticalLock);	
			}
		}

		// Absorb all : 
		event->accept();

		// Send selection signal : 
		emit selected(this);
	}

	void SubWidget::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		QRectF sceneRect;
		if(graphicsProxy!=NULL && graphicsProxy->scene()!=NULL)
			sceneRect = graphicsProxy->scene()->sceneRect();

		if(!event->isAccepted())
		{
			// Protect from moving a maximized widget :
			if(anchorMode!=AnchorFree && motionActive)	// Detach?
			{
				const double	unsnapSafeMarginPercent = 0.03,
						smallestMarginPixel = 64.0;

				// Get the new position : 
				const QPoint	c = mapToParent(mapFromGlobal(event->globalPos())),
						p = c - offset;
				const double	d = (p.x() - x())*(p.x() - x()) + (p.y() - y())*(p.y() - y());

				// Unsnap / demaximized only if needed.
				if((anchorMode==AnchorLeft && c.x()>std::max(unsnapSafeMarginPercent*sceneRect.width(), smallestMarginPixel)) || (anchorMode==AnchorRight && c.x()<std::min((1.0-unsnapSafeMarginPercent)*sceneRect.width(), sceneRect.width()-smallestMarginPixel)) || (anchorMode==AnchorMaximized && d>(smallestMarginPixel*smallestMarginPixel) && c.y()>0 && c.y()<(sceneRect.height()-1)))
				{
					const bool test = (anchorMode==AnchorMaximized);

					setAnchor(AnchorFree);

					// Reset the offset, if needed : 
					if(!test)
						move(p);
					else
					{
						move(c - QPoint(width()/2, titleLabel.height()/2));
						offset = c - pos();
					}
				}
			}
			else if(motionActive)
			{
				// Get the new position : 
				const QPoint	c = mapToParent(mapFromGlobal(event->globalPos())),
						p = c - offset;

				if(c.x()<=0)
					setAnchor(AnchorLeft);
				else if(c.x()>=(sceneRect.width()-1))
 					setAnchor(AnchorRight);
				else if(c.y()<=0 || c.y()>=(sceneRect.height()-1))
				{
					setAnchor(AnchorMaximized);
					offset = c - pos();
				}				
				else
					move(p);
			}
			else if(resizeActive)
			{
				const QPoint	c = mapToParent(mapFromGlobal(event->globalPos()));

				if(std::abs(offset.x())<2*layout.margin())
				{
					// Special case were the resize is mixed with a change in the position in X.
					QPoint d = c - mousePress;

					if(resizeHorizontalLock)
						d.setY(0);

					if(resizeVerticalLock)
						d.setX(0);

					// Prevent right-shift if the width is smaller than the minimumWidgth :
					int newWidth = sizeAtMousePress.width()-d.x();

					if(newWidth<minimumWidth())
					{
						newWidth = minimumWidth();
						d.setX(sizeAtMousePress.width() - minimumWidth()); // Solve for the correct displacement.
					}

					setGeometry(mousePress.x()+d.x()-offset.x(), mousePress.y()-offset.y(), newWidth, sizeAtMousePress.height()+d.y());
				}
				else
				{
					// General case : 
					QPoint d = c - mousePress;

					if(resizeHorizontalLock)
						d.setY(0);

					if(resizeVerticalLock)
						d.setX(0);

					resize(sizeAtMousePress.width()+d.x(), sizeAtMousePress.height()+d.y());
				}
			}
		}

		// Absorb all : 
		event->accept();
	}

	void SubWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted())
		{
			motionActive = false;
			resizeActive = false;
		}

		// Absorb all : 
		event->accept();
	}

	void SubWidget::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QWidget::mouseDoubleClickEvent(event);

		// It seems that the previous line always accepts the event. 
		// We assume the titleBar discrimination is sufficient.

		if(qvglParent!=NULL && titleWidget.underMouse())
		{
			if(anchorMode!=AnchorMaximized)
				setAnchor(AnchorMaximized);
			else
				setAnchor(AnchorFree);
		}

		// Absorb all : 
		event->accept();
	}

	void SubWidget::wheelEvent(QWheelEvent* event)
	{
		QWidget::wheelEvent(event);

		// Absorb all : 
		event->accept();
	}

	void SubWidget::addChild(QObject* pObject)
	{
		if(pObject && pObject->isWidgetType())
		{
			pObject->installEventFilter(this);

			const QObjectList& childList = pObject->children();
			for(QObjectList::const_iterator it = childList.begin(); it!=childList.end(); it++)
				addChild(*it);
		}
	}

	void SubWidget::removeChild(QObject* pObject)
	{
		if(pObject && pObject->isWidgetType())
		{
			pObject->removeEventFilter(this);

			const QObjectList& childList = pObject->children();
			for(QObjectList::const_iterator it = childList.begin(); it!=childList.end(); it++)
				removeChild(*it);
		}
	}

	void SubWidget::childEvent(QChildEvent* e)
	{
		if (e->child()->isWidgetType())
		{
			if(e->type() == QEvent::ChildAdded)
				addChild(e->child());
			else if (e->type() == QEvent::ChildRemoved)
				removeChild(e->child());
		}

		QWidget::childEvent(e);
	}

	bool SubWidget::eventFilter(QObject* target, QEvent* e)
	{
		if(e->type()==QEvent::ChildAdded)
			addChild(reinterpret_cast<QChildEvent*>(e)->child());	
		else if(e->type()==QEvent::ChildRemoved)
			removeChild(reinterpret_cast<QChildEvent*>(e)->child());
		else if(e->type()==QEvent::KeyPress || e->type()==QEvent::KeyRelease || e->type()==QEvent::MouseButtonDblClick || e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease || e->type()==QEvent::FocusIn)
			emit selected(this);

		return QWidget::eventFilter(target, e);
	}

	void SubWidget::sceneRectChanged(const QRectF& sceneRect)
	{
		switch(anchorMode)
		{
			case AnchorLeft:
				move(0, TopBar::getHeight());
				resize(width(), sceneRect.height()-TopBar::getHeight());
				break;
			case AnchorRight: 
				move(sceneRect.width()-width(), TopBar::getHeight());
				resize(width(), sceneRect.height()-TopBar::getHeight());
				break;
			case AnchorMaximized:
				move(0, TopBar::getHeight());
				resize(sceneRect.width(), sceneRect.height()-TopBar::getHeight());
				break;
		}
	}

	void SubWidget::sceneRectChanged(void)
	{
		if(graphicsProxy!=NULL && graphicsProxy->scene()!=NULL)
			sceneRectChanged(graphicsProxy->scene()->sceneRect());
	}

	void SubWidget::graphicsProxyDestroyed(void)
	{
		graphicsProxy = NULL;
		anchorMode = AnchorFree;
	}

	void SubWidget::setInnerWidget(QWidget* _widget)
	{
		if(widget!=NULL)
			throw Exception("SubWidget::setInnerWidget - A QWidget was already bound.", __FILE__, __LINE__);
		else if(_widget!=NULL)
		{
			widget = _widget;
			widget->setParent(this);
			layout.addWidget(widget, 1);

			// Scan widget and install the event filter : 
			widget->installEventFilter(this);

			const QObjectList& childList = widget->children();
			for(QObjectList::const_iterator it = childList.begin(); it!=childList.end(); it++)
				addChild(*it);
		}
	}

	QWidget* SubWidget::getInnerWidget(void)
	{
		return widget;
	}

	void SubWidget::setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy)
	{
		if(_graphicsProxy!=NULL)
		{
			graphicsProxy = _graphicsProxy;

			if(graphicsProxy->scene()!=NULL)
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectChanged(const QRectF&)));

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
		}
	}

	QGraphicsProxyWidget* SubWidget::getGraphicsProxy(void)
	{
		return graphicsProxy;
	}

	QString SubWidget::getTitle(void)
	{
		return titleLabel.text();
	}

	void SubWidget::setTitle(QString title)
	{
		titleLabel.setText(title);
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		emit titleChanged();
	}

	void SubWidget::setQVGLParent(MainWidget* _qvglParent)
	{
		qvglParent = _qvglParent;
	}

	MainWidget* SubWidget::getQVGLParent(void)
	{
		return qvglParent;
	}

	void SubWidget::setAnchor(AnchorMode mode)
	{
		switch(mode)
		{
			case AnchorFree :
				move(originalPosition);
				resize(originalSize);
				anchorMode = AnchorFree;
				break;
			case AnchorLeft :
			case AnchorRight :
			case AnchorMaximized :
				originalPosition = pos();
				originalSize = size();
				anchorMode = mode;
				sceneRectChanged();
				break;
		}
	}

	const SubWidget::AnchorMode& SubWidget::getAnchor(void) const
	{
		return anchorMode;
	}

	bool SubWidget::shoudBeVisible(void) const
	{
		return visible;
	}

	void SubWidget::show(void)
	{
		QWidget::show();

		visible = true;

		emit showRequest(this);
	}

	void SubWidget::temporaryHide(bool enabled)
	{
		if(enabled)
			QWidget::hide();
		else if(!enabled && visible)
			QWidget::show();
	}

	void SubWidget::hide(void)
	{
		QWidget::hide();

		visible = false;

		emit hideRequest(this);
	}

// PositionColorInfoMini : 
	PositionColorInfoMini::PositionColorInfoMini(void)
	 : bar(this)
	{
		colorBox.setAutoFillBackground(true);
		QFontMetrics fontMetrics(positionLabel.font());

		positionLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		positionLabel.setFixedWidth(fontMetrics.width("00000x00000"));
		
		colorBox.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		colorBox.setFixedWidth(fontMetrics.width("####"));

		bar.addWidget(&positionLabel);
		bar.addWidget(&colorBox);
		bar.addSpacing(2);

		bar.setMargin(0);
		bar.setSpacing(0);

		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		// Clear : 
		updatePosition();
		updateColor();
	}

	PositionColorInfoMini::~PositionColorInfoMini(void)
	{ }

	void PositionColorInfoMini::setWindowOpacity(qreal level)
	{
		QWidget::setWindowOpacity(level);
		colorBox.setWindowOpacity(1.0);		// TODO : to fix missing solid color.
	}

	void PositionColorInfoMini::updatePosition(const QPointF& pos)
	{
		//positionLabel.setText("00000x00000");
		positionLabel.setText(tr("%1x%2").arg(std::floor(pos.x())).arg(std::floor(pos.y())));
	}

	void PositionColorInfoMini::updateColor(const QColor& color)
	{
		QPalette palette = colorBox.palette();
		palette.setColor(QPalette::Window, color);
		colorBox.setPalette(palette);
	}

// TopBar
	TopBar* TopBar::singleton = NULL;

	TopBar::TopBar(void)
	 : 	graphicsProxy(NULL),
		bar(this),
		mainMenu("Menu", this),
		viewsMenu("Views", this),
		subWidgetsMenu("Widgets", this),
		viewsSeparator(NULL),
		closeCurrentViewAction("Close view", this),	
		closeAllViewsAction("Close all views", this),
		viewsTablesSeparator(NULL),
		closeCurrentViewsTableAction("Close table", this), 
		closeAllViewsTableAction("Close all tables", this),
		temporaryHideAllSubWidgetsAction("Hide all widgets", this),
		hideAllSubWidgetsAction("Close all widgets", this),
		subWidgetsSeparator(NULL),
		signalMapper(this),
		menuBar(this)
	{
		if(singleton==NULL)
			singleton = this;
		else
			throw Glip::Exception("TopBar::TopBar - Only one instance of TopBar is tolerated.", __FILE__, __LINE__);

		// Activate focus via mouse click :
		setFocusPolicy(Qt::ClickFocus);

		// Menu :
		menuBar.addMenu(&mainMenu);
		menuBar.addMenu(&viewsMenu);
		menuBar.addMenu(&subWidgetsMenu);
		menuBar.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		bar.addWidget(&menuBar);
		bar.addWidget(&titleLabel);
		bar.addWidget(&positionColorInfo);

		bar.setMargin(0);
		bar.setSpacing(0);

		// Signals connections : 
		QObject::connect(&viewsSignalMapper, 			SIGNAL(mapped(QObject*)), 	this, SLOT(castViewPointer(QObject*)));
		QObject::connect(&viewsTablesSignalMapper,		SIGNAL(mapped(QObject*)), 	this, SLOT(castViewsTablePointer(QObject*)));
		QObject::connect(&widgetsSignalMapper,			SIGNAL(mapped(QObject*)), 	this, SLOT(castSubWidgetPointer(QObject*)));
		QObject::connect(&mainMenu,				SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&viewsMenu,				SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&subWidgetsMenu,			SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&signalMapper,				SIGNAL(mapped(int)),		this, SLOT(transferActionSignal(int)));

		// Signals mapping : 
		QObject::connect(&closeCurrentViewAction,		SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&closeAllViewsAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&temporaryHideAllSubWidgetsAction,	SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&hideAllSubWidgetsAction,		SIGNAL(triggered()),		&signalMapper, SLOT(map()));

		signalMapper.setMapping(&closeCurrentViewAction, 		ActionCloseView);
		signalMapper.setMapping(&closeAllViewsAction,			ActionCloseAllViews);
		signalMapper.setMapping(&closeCurrentViewsTableAction, 		ActionCloseViewsTable);
		signalMapper.setMapping(&closeAllViewsTableAction,		ActionCloseAllViewsTables);
		signalMapper.setMapping(&temporaryHideAllSubWidgetsAction,	ActionTemporaryHideAllSubWidgets);
		signalMapper.setMapping(&hideAllSubWidgetsAction,		ActionHideAllSubWidgets);

		// Init views menu : 
		viewsSeparator = viewsMenu.addSeparator();
		viewsMenu.addAction(&closeCurrentViewAction);	
		viewsMenu.addAction(&closeAllViewsAction);
		viewsMenu.addSeparator();
		viewsTablesSeparator = viewsMenu.addSeparator();
		viewsMenu.addAction(&closeCurrentViewsTableAction);
		viewsMenu.addAction(&closeAllViewsTableAction);

		subWidgetsMenu.addAction(&temporaryHideAllSubWidgetsAction);
		subWidgetsMenu.addAction(&hideAllSubWidgetsAction);
		subWidgetsSeparator = subWidgetsMenu.addSeparator();

		// Reset : 
		setTitle();

		// TEST :
		mainMenu.addAction("Action 1");
		mainMenu.addAction("Action 2");
		mainMenu.addAction("Action 3");
	}

	TopBar::~TopBar(void)
	{
		// Clear the menus : 
		viewsActions.clear();
		viewsTablesActions.clear();

		viewsMenu.removeAction(&closeCurrentViewAction);
		viewsMenu.removeAction(&closeAllViewsAction);
		subWidgetsMenu.removeAction(&temporaryHideAllSubWidgetsAction);
		subWidgetsMenu.removeAction(&hideAllSubWidgetsAction);

		if(graphicsProxy!=NULL)
		{
			graphicsProxy->setWidget(NULL);
			delete graphicsProxy;
			graphicsProxy = NULL;
		}

		if(singleton==this)
			singleton = NULL;
	}

	void TopBar::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		// Absorb :
		if(!event->isAccepted())
			event->accept();

		emit selected(this);
	}

	void TopBar::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QWidget::mouseDoubleClickEvent(event);

		// Absorb :
		if(!event->isAccepted())
			event->accept();
	}

	void TopBar::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mouseMoveEvent(event);

		// Absorb :
		if(!event->isAccepted())
			event->accept();
	}

	void TopBar::mouseReleaseEvent(QMouseEvent* event)
	{
		QWidget::mouseReleaseEvent(event);

		// Absorb :
		if(!event->isAccepted())
			event->accept();
	}

	void TopBar::stretch(const QRectF& rect)
	{
		setGeometry(0, 0, rect.width(), height());
	}

	void TopBar::castViewPointer(QObject* ptr)
	{
		emit changeViewRequest(reinterpret_cast<View*>(ptr));
	}

	void TopBar::castViewsTablePointer(QObject* ptr)
	{
		emit changeViewsTableRequest(reinterpret_cast<ViewsTable*>(ptr));
	}

	void TopBar::castSubWidgetPointer(QObject* ptr)
	{
		emit showSubWidgetRequest(reinterpret_cast<SubWidget*>(ptr));
	}

	void TopBar::sendSelectedSignal(void)
	{
		emit selected(this);
	}
	
	void TopBar::transferActionSignal(int actionID)
	{
		emit requestAction(static_cast<ActionID>(actionID));
	}

	void TopBar::viewClosed(void)
	{
		View* view = reinterpret_cast<View*>(QObject::sender());
	
		QMap<View*, QAction*>::iterator it = viewsActions.find(view);

		if(view!=NULL && it!=viewsActions.end())
		{
			viewsMenu.removeAction(it.value());
			delete it.value();
			viewsActions.erase(it);
		}
	}

	void TopBar::viewsTableClosed(void)
	{
		ViewsTable* viewTable = reinterpret_cast<ViewsTable*>(QObject::sender());
	
		QMap<ViewsTable*, QAction*>::iterator it = viewsTablesActions.find(viewTable);

		if(viewTable!=NULL && it!=viewsTablesActions.end())
		{
			viewsMenu.removeAction(it.value());
			delete it.value();
			viewsTablesActions.erase(it);
		}
	}

	void TopBar::subWidgetClosed(void)
	{
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		QMap<SubWidget*, QAction*>::iterator it = subWidgetsActions.find(subWidget);
		
		if(subWidget!=NULL && it!=subWidgetsActions.end())
		{
			subWidgetsMenu.removeAction(it.value());
			delete it.value();
			subWidgetsActions.erase(it);
		}
	}

	void TopBar::graphicsProxyDestroyed(void)
	{
		graphicsProxy = NULL;
	}

	void TopBar::setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy)
	{
		if(_graphicsProxy!=NULL)
		{
			graphicsProxy = _graphicsProxy;

			if(graphicsProxy->scene()!=NULL)
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(stretch(const QRectF&)));

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
		}
	}

	QGraphicsProxyWidget* TopBar::getGraphicsProxy(void)
	{
		return graphicsProxy;
	}

	void TopBar::setTitle(void)
	{
		titleLabel.setText("(No Title)");
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// No tooltip : 
		titleLabel.setToolTip("");
	}

	void TopBar::setTitle(QString title)
	{
		// Do not put rich text!
		title.remove(QRegExp("<[^>]*>"));

		titleLabel.setText(title);
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// No tooltip : 
		titleLabel.setToolTip("");
	}

	void TopBar::setTitle(const View& view)
	{
		// Set title : 
		titleLabel.setText(view.getName());
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// Set tooltip info :
		QString toolTip;

		if(view.isValid())
		{
			toolTip += "<table>";
				toolTip += tr("<tr><td><i>Size</i></td><td>:</td><td>%1x%2 (%3)</td></tr>").arg(view.getFormat().getWidth()).arg(view.getFormat().getHeight()).arg(view.getSizeString());
				toolTip += tr("<tr><td><i>Mode</i></td><td>:</td><td>%1</td></tr>").arg(glParamName( view.getFormat().getGLMode() ).c_str());
				toolTip += tr("<tr><td><i>Depth</i></td><td>:</td><td>%1</td></tr>").arg(glParamName( view.getFormat().getGLDepth() ).c_str());
				toolTip += tr("<tr><td><i>Filtering</i></td><td>:</td><td>%1 / %2</td></tr>").arg(glParamName( view.getFormat().getMinFilter() ).c_str()).arg(glParamName( view.getFormat().getMagFilter() ).c_str());
				toolTip += tr("<tr><td><i>Wrapping</i></td><td>:</td><td>%1 / %2</td></tr>").arg(glParamName( view.getFormat().getSWrapping() ).c_str()).arg(glParamName( view.getFormat().getTWrapping() ).c_str());
				toolTip += tr("<tr><td><i>Mipmap</i></td><td>:</td><td>%1 / %2</td></tr>").arg(view.getFormat().getBaseLevel()).arg(view.getFormat().getMaxLevel());
			toolTip += "</table>";
		}
		else
			toolTip = "<i>Invalid View</i>";

		titleLabel.setToolTip(toolTip);
	}

	void TopBar::setTitle(const ViewsTable& table)
	{
		// Set title : 
		titleLabel.setText(table.getName());
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		// No tooltip : 
		titleLabel.setToolTip("");
	}

	void TopBar::updatePositionAndColor(const QPointF& pos, const QColor& color)
	{
		positionColorInfo.updatePosition(pos);
		positionColorInfo.updateColor(color);
	}

	void TopBar::setWindowOpacity(qreal level)
	{
		QWidget::setWindowOpacity(level);
		mainMenu.setWindowOpacity(level);
		viewsMenu.setWindowOpacity(level);
		subWidgetsMenu.setWindowOpacity(level);
		positionColorInfo.setWindowOpacity(level);
	}

	int TopBar::getHeight(void)
	{
		if(singleton==NULL)
			return 0;
		else
			return singleton->height();
	}

	void TopBar::addView(View* view)
	{
		if(view!=NULL && !viewsActions.contains(view))
		{
			QAction* action = new QAction(view->getName(), this);
			viewsActions[view] = action;
			viewsMenu.insertAction(viewsSeparator, action);
			
			// Signals : 
			viewsSignalMapper.setMapping(action, reinterpret_cast<QObject*>(view));
			QObject::connect(action,	SIGNAL(triggered(void)), 	&viewsSignalMapper, 	SLOT(map()));
			QObject::connect(view, 		SIGNAL(closed(void)), 		this, 			SLOT(viewClosed(void)));
			QObject::connect(view, 		SIGNAL(destroyed(void)), 	this, 			SLOT(viewClosed(void)));
		}
	}

	void TopBar::addViewsTable(ViewsTable* viewsTable)
	{
		if(viewsTable!=NULL && !viewsTablesActions.contains(viewsTable))
		{
			QAction* action = new QAction(viewsTable->getName(), this);
			viewsTablesActions[viewsTable] = action;
			viewsMenu.insertAction(viewsTablesSeparator, action);
			
			// Signals : 
			viewsTablesSignalMapper.setMapping(action, reinterpret_cast<QObject*>(viewsTable));
			QObject::connect(action,	SIGNAL(triggered(void)), 	&viewsTablesSignalMapper, 	SLOT(map()));
			QObject::connect(viewsTable, 	SIGNAL(closed(void)), 		this, 				SLOT(viewsTableClosed(void)));
			QObject::connect(viewsTable, 	SIGNAL(destroyed(void)), 	this, 				SLOT(viewsTableClosed(void)));
		}
	}

	void TopBar::addSubWidget(SubWidget* subWidget)
	{
		if(subWidget!=NULL && !subWidgetsActions.contains(subWidget))
		{
			QAction* action = new QAction(subWidget->getTitle(), this);
			subWidgetsActions[subWidget] = action;
			subWidgetsMenu.addAction(action);

			// Signals :
			widgetsSignalMapper.setMapping(action, reinterpret_cast<QObject*>(subWidget));
			QObject::connect(action,	SIGNAL(triggered(void)), 	&widgetsSignalMapper, 	SLOT(map()));
			QObject::connect(subWidget,	SIGNAL(closed(void)), 		this, 			SLOT(subWidgetClosed(void)));
			QObject::connect(subWidget,	SIGNAL(destroyed(void)), 	this, 			SLOT(subWidgetClosed(void)));
		}
	}

// BottomBar : 
	BottomBar::BottomBar(void)
	 : 	graphicsProxy(NULL),
		bar(this)
	{
		// Activate focus via mouse click :
		setFocusPolicy(Qt::ClickFocus);

		// Bar : 
		bar.addWidget(&toolBar);

		bar.setMargin(0);
		bar.setSpacing(0);

		// Hide : 
		hide();
	}

	BottomBar::~BottomBar(void)
	{
		if(graphicsProxy!=NULL)
		{
			graphicsProxy->setWidget(NULL);
			delete graphicsProxy;
			graphicsProxy = NULL;
		}
	}

	void BottomBar::mousePressEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		emit selected(this);
	}

	void BottomBar::stretch(const QRectF& rect)
	{
		setGeometry(0, rect.height()-height(), rect.width(), height());
	}

	void BottomBar::graphicsProxyDestroyed(void)
	{
		graphicsProxy = NULL;
	}

	void BottomBar::setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy)
	{
		if(_graphicsProxy!=NULL)
		{
			graphicsProxy = _graphicsProxy;

			if(graphicsProxy->scene()!=NULL)
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(stretch(const QRectF&)));

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
		}
	}

	QGraphicsProxyWidget* BottomBar::getGraphicsProxy(void)
	{
		return graphicsProxy;
	}

// ContextWidget :
	ContextWidget::ContextWidget(QGLContext* ctx, QWidget* parent)
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

			QMessageBox errorBox(QMessageBox::Critical, "ContextWidget::ContextWidget", "An error occurred during initialization.", QMessageBox::Ok);
			errorBox.setDetailedText(e.what());
			errorBox.exec();

			// re-throw :
			throw e;
		}
	}

	ContextWidget::~ContextWidget(void)
	{
		if(glipOwnership)
			HandleOpenGL::deinit();
	}

	void ContextWidget::initializeGL(void)
	{
		makeCurrent();
		glViewport(0, 0, width(), height());
		setAutoBufferSwap(false);
	}

	void ContextWidget::resizeGL(int width, int height)
	{
		// TODO : this is never called, to be removed?.
		glViewport(0, 0, width, height);
	}

// KeyboardState :
	KeyboardState::KeyboardState(void)
	{
		for(int k=0; k<NumActions; k++)
		{
			actionPressed[k] = false;
			takeBackEnabled[k] = false;
		}

		resetActionsKeySequences();
	}

	KeyboardState::~KeyboardState(void)
	{ }

	void KeyboardState::keyPressed(QKeyEvent* event)
	{
		ActionID a = getActionAssociatedToKey(event);

		if(a!=NoAction)
		{
			event->accept();
			actionPressed[a] = true;

			emit actionReceived(a);
		}
	}

	void KeyboardState::keyReleased(QKeyEvent* event)
	{
		ActionID a = getActionAssociatedToKey(event);

		if(a!=NoAction)
		{
			event->accept();
			actionPressed[a] = false;

			if(isActionTakeBackEnabled(a))
				emit actionReceived(a, true);
		}
	}

	void KeyboardState::forceRelease(void)
	{
		for(QMap<QKeySequence, ActionID>::iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
		{
			if(isActionTakeBackEnabled(it.value()))
				emit actionReceived(it.value(), true);
		}
	}

	ActionID KeyboardState::getActionAssociatedToKey(const QKeySequence& keySequence) const
	{
		if(keysActionsAssociations.contains(keySequence))
			return keysActionsAssociations[keySequence];
		else
			return NoAction;
	}

	ActionID KeyboardState::getActionAssociatedToKey(const QKeyEvent* event) const
	{
		const int s = (event->key() | event->modifiers());

		return getActionAssociatedToKey(QKeySequence(s));
	}

	QKeySequence KeyboardState::getKeysAssociatedToAction(const ActionID& a)
	{
		bool addComa = false;
		QString keysString;

		for(QMap<QKeySequence, ActionID>::iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
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

	bool KeyboardState::isActionTakeBackEnabled(const ActionID& a) const
	{
		if(a>=0 && a<NumActions)
			return takeBackEnabled[a];
		else
			throw Exception("KeyboardState::isActionTakenBack - Invalid ActionID.", __FILE__, __LINE__);
	}

	void KeyboardState::setTakeBack(const ActionID& a, bool enabled)
	{
		if(a>=0 && a<NumActions)
			takeBackEnabled[a] = enabled;
		else
			throw Exception("KeyboardState::isActionTakenBack - Invalid ActionID.", __FILE__, __LINE__);
	}

	void KeyboardState::setActionKeySequence(const ActionID& a, const QKeySequence& keySequence, bool enableTakeBack)
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

		// Take back : 
		setTakeBack(a, enableTakeBack);
	}

	void KeyboardState::resetActionsKeySequences(void)
	{
		setActionKeySequence(ActionUp,					Qt::Key_Up);
		setActionKeySequence(ActionDown,				Qt::Key_Down);
		setActionKeySequence(ActionLeft,				Qt::Key_Left);
		setActionKeySequence(ActionRight,				Qt::Key_Right);
		setActionKeySequence(ActionZoomIn,				QKeySequence(Qt::Key_Plus, Qt::SHIFT + Qt::Key_Plus, Qt::KeypadModifier + Qt::Key_Plus));	// Support for keypad and shifts.
		setActionKeySequence(ActionZoomOut,				QKeySequence(Qt::Key_Minus, Qt::SHIFT + Qt::Key_Minus, Qt::KeypadModifier + Qt::Key_Minus));	// Support for keypad and shifts.
		setActionKeySequence(ActionRotationClockWise,			Qt::Key_F);
		setActionKeySequence(ActionRotationCounterClockWise,		Qt::Key_D);
		setActionKeySequence(ActionToggleFullscreen,			Qt::Key_Return);
		setActionKeySequence(ActionExitFullscreen,			Qt::Key_Escape);
		setActionKeySequence(ActionResetView,				Qt::Key_Space);
		setActionKeySequence(ActionPreviousView,			QKeySequence(Qt::CTRL + Qt::Key_Left));
		setActionKeySequence(ActionNextView,				QKeySequence(Qt::CTRL + Qt::Key_Right));
		setActionKeySequence(ActionCloseView,				Qt::Key_Delete);
		setActionKeySequence(ActionCloseAllViews,			QKeySequence(Qt::SHIFT + Qt::Key_Delete));
		setActionKeySequence(ActionMotionModifier,			QKeySequence(Qt::CTRL + Qt::Key_Control, Qt::Key_Control), true); 	// The first correspond the press event, the second to the release.
		setActionKeySequence(ActionRotationModifier,			QKeySequence(Qt::SHIFT + Qt::Key_Shift, Qt::Key_Shift), true);		// (the same)
		setActionKeySequence(ActionNextSubWidget,			QKeySequence(Qt::ALT + Qt::Key_F));
		setActionKeySequence(ActionPreviousSubWidget,			QKeySequence(Qt::ALT + Qt::Key_G));
		setActionKeySequence(ActionTemporaryHideAllSubWidgets,		QKeySequence(Qt::CTRL + Qt::Key_T));
		setActionKeySequence(ActionTemporaryUnhideAllSubWidgets,	QKeySequence(Qt::CTRL + Qt::Key_Y));
		setActionKeySequence(ActionHideAllSubWidgets,			QKeySequence(Qt::CTRL + Qt::Key_U));
	}

// MouseState::VectorData
	MouseState::VectorData::VectorData(const QString& name, QObject* parent)
	 : 	modification(1)
		#ifdef __MAKE_VARIABLES__
		, record(NULL)
		#endif
	{
		#ifdef __MAKE_VARIABLES__
			record = new QGUI::VariableRecord(name, GL_FLOAT_VEC2, parent);
		#endif
	}

	MouseState::VectorData::~VectorData(void)
	{
		#ifdef __MAKE_VARIABLES__
			delete record;
			record = NULL;
		#endif	
	}

// MouseState::ColorData
	MouseState::ColorData::ColorData(const QString& name, QObject* parent)
	 : 	modification(1)
		#ifdef __MAKE_VARIABLES__
		, record(NULL)
		#endif
	{
		#ifdef __MAKE_VARIABLES__
			record = new QGUI::VariableRecord(name, GL_FLOAT_VEC3, parent);
		#endif
	}

	MouseState::ColorData::~ColorData(void)
	{
		#ifdef __MAKE_VARIABLES__
			delete record;
			record = NULL;
		#endif	
	}

// MouseState :
	const QMap<MouseState::VectorID, QString>	MouseState::vectorsNameMap = initVectorsNameMap();
	const QMap<MouseState::ColorID, QString>	MouseState::colorsNameMap = initColorsNameMap();

	// Static Function :
	QMap<MouseState::VectorID, QString> MouseState::initVectorsNameMap(void)
	{
		QMap<VectorID, QString> _vectorsNameMap;

		#define NAME_MAP( id ) _vectorsNameMap[ id ] = STR( id );

			NAME_MAP( VectorLastLeftClick )
			NAME_MAP( VectorLastLeftClickGl )
			NAME_MAP( VectorLastLeftClickQuad )
			NAME_MAP( VectorLastLeftClickImage )

			NAME_MAP( VectorLastLeftPosition )
			NAME_MAP( VectorLastLeftPositionGl )
			NAME_MAP( VectorLastLeftPositionQuad )
			NAME_MAP( VectorLastLeftPositionImage )

			NAME_MAP( VectorLastLeftShift )
			NAME_MAP( VectorLastLeftShiftGl )
			NAME_MAP( VectorLastLeftShiftQuad )
			NAME_MAP( VectorLastLeftShiftImage )

			NAME_MAP( VectorLastLeftRelease )
			NAME_MAP( VectorLastLeftReleaseGl )
			NAME_MAP( VectorLastLeftReleaseQuad )
			NAME_MAP( VectorLastLeftReleaseImage )

			NAME_MAP( VectorLastLeftCompletedVector )
			NAME_MAP( VectorLastLeftCompletedVectorGl )
			NAME_MAP( VectorLastLeftCompletedVectorQuad )
			NAME_MAP( VectorLastLeftCompletedVectorImage )

			NAME_MAP( VectorLastRightClick )
			NAME_MAP( VectorLastRightClickGl )
			NAME_MAP( VectorLastRightClickQuad )
			NAME_MAP( VectorLastRightClickImage )

			NAME_MAP( VectorLastRightPosition )
			NAME_MAP( VectorLastRightPositionGl )
			NAME_MAP( VectorLastRightPositionQuad )
			NAME_MAP( VectorLastRightPositionImage )

			NAME_MAP( VectorLastRightShift )
			NAME_MAP( VectorLastRightShiftGl )
			NAME_MAP( VectorLastRightShiftQuad )
			NAME_MAP( VectorLastRightShiftImage )

			NAME_MAP( VectorLastRightRelease )
			NAME_MAP( VectorLastRightReleaseGl )
			NAME_MAP( VectorLastRightReleaseQuad )
			NAME_MAP( VectorLastRightReleaseImage )

			NAME_MAP( VectorLastRightCompletedVector )
			NAME_MAP( VectorLastRightCompletedVectorGl )
			NAME_MAP( VectorLastRightCompletedVectorQuad )
			NAME_MAP( VectorLastRightCompletedVectorImage )

			NAME_MAP( VectorLastWheelUp )
			NAME_MAP( VectorLastWheelUpGl )
			NAME_MAP( VectorLastWheelUpQuad )
			NAME_MAP( VectorLastWheelUpImage )

			NAME_MAP( VectorLastWheelDown )
			NAME_MAP( VectorLastWheelDownGl )
			NAME_MAP( VectorLastWheelDownQuad )
			NAME_MAP( VectorLastWheelDownImage )
			
		#undef NAME_MAP

		return _vectorsNameMap;
	}

	// Static Function :
	QMap<MouseState::ColorID, QString> MouseState::initColorsNameMap(void)
	{
		QMap<ColorID, QString> _colorsNameMap;

		#define NAME_MAP( id ) _colorsNameMap[ id ] = STR( id );

			NAME_MAP( ColorUnderLastLeftClick )
			NAME_MAP( ColorUnderLastLeftPosition )
			NAME_MAP( ColorUnderLastLeftRelease )
			NAME_MAP( ColorUnderLastRightClick )
			NAME_MAP( ColorUnderLastRightPosition )
			NAME_MAP( ColorUnderLastRightRelease )

		#undef NAME_MAP

		return _colorsNameMap;
	}

	MouseState::MouseState(void)
	 :	functionMode(ModeCollection),
		wheelDelta(0.0f)
	{
		// Create the maps : 
		for(QMap<VectorID, QString>::const_iterator it=vectorsNameMap.begin(); it!=vectorsNameMap.end(); it++)
			vectors.insert( it.key(), new VectorData(getVectorIDName(it.key()), this));

		for(QMap<ColorID, QString>::const_iterator it=colorsNameMap.begin(); it!=colorsNameMap.end(); it++)
			colors.insert( it.key(), new ColorData(getColorIDName(it.key()), this));

		vectorIDs 	= vectors.keys();
		colorIDs 	= colors.keys();

		setFunctionMode(ModeCollection);
	}

	MouseState::~MouseState(void)
	{ 
		for(QMap<VectorID, VectorData*>::iterator it=vectors.begin(); it!=vectors.end(); it++)
			delete it.value();
		vectors.clear();

		for(QMap<ColorID, ColorData*>::iterator it=colors.begin(); it!=colors.end(); it++)
			delete it.value();
		colors.clear();
	}	

	void MouseState::incrementEventCounters(void)
	{
		// Increment only if the value is not waiting for an update (0) :
		for(QMap<VectorID, VectorData*>::iterator it=vectors.begin(); it!=vectors.end(); it++)
			it.value()->modification += (it.value()->modification>=1) ? 1 : 0;

		for(QMap<ColorID, ColorData*>::iterator it=colors.begin(); it!=colors.end(); it++)
			it.value()->modification += (it.value()->modification>=1) ? 1 : 0;
	}

	bool MouseState::doesVectorRequireUpdate(const VectorID& id) const
	{
		if(id==InvalidVectorID)
			return false;
		else
			return vectors[id]->modification==0;
	}

	bool MouseState::doesColorRequirepdate(const ColorID& id) const
	{
		if(id==InvalidColorID)
			return false;
		else
			return colors[id]->modification==0;
	}

	void MouseState::setVector(const VectorID& id, const QPointF& v, const bool requireUpdate)
	{
		QMap<VectorID, VectorData*>::iterator it = vectors.find(id);
	
		if(it!=vectors.end())
		{
			it.value()->modification = requireUpdate ? 0 : 1; //(requireUpdate ? RequireUpdate : Modified);
			it.value()->vector 	 = v;
			
			#ifdef __MAKE_VARIABLES__
			if(!requireUpdate && getFunctionMode()==ModeCollection)
			{
				it.value()->record->data().set(v.x(), 0);
				it.value()->record->data().set(v.y(), 1);
				it.value()->record->declareUpdate();
			}
			#endif
		}
		else
			throw Exception("MouseState::setVector - Unknown VectorID.", __FILE__, __LINE__);
	}

	void MouseState::setColor(const ColorID& id, const QColor& c)
	{
		QMap<ColorID, ColorData*>::iterator it = colors.find(id);

		if(it!=colors.end())
		{
			it.value()->modification = 1; // Modified
			it.value()->color	= c;

			#ifdef __MAKE_VARIABLES__
			if(getFunctionMode()==ModeCollection)
			{
				it.value()->record->data().set(c.red(), 0);
				it.value()->record->data().set(c.green(), 1);
				it.value()->record->data().set(c.blue(), 2);
				it.value()->record->declareUpdate();
			}
			#endif
		}
		else
			throw Exception("MouseState::setColor - Unknown ColorID.", __FILE__, __LINE__);
	}

	void MouseState::processEvent(QGraphicsSceneWheelEvent* event)
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
			incrementEventCounters();
			emit requestExternalUpdate();
		}
	}

	void MouseState::processEvent(QGraphicsSceneMouseEvent* event, const bool clicked, const bool moved, const bool released)
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

				if(getFunctionMode()==ModeMotion)
					emit mustSetMouseCursor(Qt::ClosedHandCursor);
			}			

			if(moved)
			{
				setVector(VectorLastLeftPosition, event->scenePos(), true);
				setVector(VectorLastLeftShift, event->scenePos() - event->lastScenePos(), true);
			}

			if(released)
			{
				setVector(VectorLastLeftRelease, event->scenePos(), true);
				setVector(VectorLastLeftCompletedVector, event->scenePos() - getVector(VectorLastLeftClick), true);
				//setVector(VectorLastLeftCompletedVector, event->scenePos() - invisibleGetVector(VectorLastLeftClick), true);
				//setVector(VectorLastLeftShift, QPointF(0.0, 0.0), true);

				if(getFunctionMode()==ModeMotion)
					emit mustSetMouseCursor(Qt::OpenHandCursor);
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
				setVector(VectorLastRightCompletedVector, event->scenePos() - getVector(VectorLastRightClick), true);
				//setVector(VectorLastRightCompletedVector, event->scenePos() - invisibleGetVector(VectorLastRightClick), true);
			}

			if(clicked || moved || released)
				event->accept();
		}

		// Qt::MidButton ?

		// Require the widget to update all the coordinates and colors.
		if(event->isAccepted())
		{
			incrementEventCounters();
			emit requestExternalUpdate();
		}
	}

	void MouseState::updateProcessCompleted(void)
	{
		emit updated();
	}

	void MouseState::clear(void)
	{
		for(QMap<VectorID, VectorData*>::Iterator it=vectors.begin(); it!=vectors.end(); it++)
		{
			it.value()->modification 	= 1;
			it.value()->vector 		= QPointF(0.0, 0.0);
			#ifdef __MAKE_VARIABLES__
			it.value()->record->data().set(0.0, 0);
			it.value()->record->data().set(0.0, 1);
			#endif
		}

		for(QMap<ColorID, ColorData*>::Iterator it=colors.begin(); it!=colors.end(); it++)
		{
			it.value()->modification 	= 1;
			it.value()->color		= Qt::black;
			#ifdef __MAKE_VARIABLES__
			it.value()->record->data().set(0.0, 0);
			it.value()->record->data().set(0.0, 1);
			it.value()->record->data().set(0.0, 2);
			#endif
		}
	}

	const QList<MouseState::VectorID>& MouseState::getVectorIDs(void) const
	{
		return vectorIDs;
	}

	const QList<MouseState::ColorID>& MouseState::getColorIDs(void) const
	{
		return colorIDs;
	}

	bool MouseState::isVectorModified(const VectorID& id) const
	{
		QMap<VectorID, VectorData*>::const_iterator it=vectors.find(id);

		if(it!=vectors.end())
			return (it.value()->modification==1);
		else
			return false;
	}

	bool MouseState::isColorModified(const ColorID& id) const 
	{
		QMap<ColorID, ColorData*>::const_iterator it=colors.find(id);
	
		if(it!=colors.end())
			return (it.value()->modification==1);
		else
			return false;
	}

	const QPointF& MouseState::getVector(const VectorID& id) const
	{
		QMap<VectorID, VectorData*>::const_iterator it=vectors.find(id);

		if(it!=vectors.end())
			return it.value()->vector;
		else
			throw Exception("MouseState::getVector - Unknown VectorID.", __FILE__, __LINE__);
	}

	const QColor& MouseState::getColor(const ColorID& id) const
	{
		QMap<ColorID, ColorData*>::const_iterator it=colors.find(id);	
	
		if(it!=colors.end())
			return it.value()->color;
		else
			throw Exception("MouseState::getColor - Unknown ColorID.", __FILE__, __LINE__);
	}

	bool MouseState::isWheelDeltaModified(void) const
	{
		return (wheelDelta!=0.0f);
	}	
	
	float MouseState::getWheelDelta(void)
	{
		float c = wheelDelta;
		wheelDelta = 0.0f;
		return c;
	}

	const MouseState::FunctionMode& MouseState::getFunctionMode(void) const
	{
		return functionMode;
	}

	void MouseState::setFunctionMode(const FunctionMode& m)
	{
		functionMode = m;

		// See http://qt-project.org/doc/qt-4.8/qt.html#CursorShape-enum for more cursors shapes : 
		switch(m)
		{
			case ModeMotion :
				emit mustSetMouseCursor(Qt::OpenHandCursor);
				break;
			case ModeCollection :
				emit mustSetMouseCursor(Qt::CrossCursor);
				break;
			default : 
				throw Exception("MouseState::setFunctionMode - Unknown function mode.", __FILE__, __LINE__);
		}
	}

	MouseState::VectorID MouseState::validate(const VectorID& vID)
	{
		if(vectorsNameMap.contains(vID))
			return vID;
		else
			return InvalidVectorID;
	}	

	MouseState::ColorID MouseState::validate(const ColorID& cID)
	{
		if(colorsNameMap.contains(cID))
			return cID;
		else
			return InvalidColorID;
	}
	
	QString MouseState::getVectorIDName(const VectorID& vID)
	{
		QMap<VectorID, QString>::const_iterator it=vectorsNameMap.find(vID);

		if(it==vectorsNameMap.end())
			return "InvalidVectorID";
		else
			return it.value();
	}

	QString MouseState::getColorIDName(const ColorID& cID)
	{
		QMap<ColorID, QString>::const_iterator it=colorsNameMap.find(cID);		

		if(it==colorsNameMap.end())
			return "InvalidColorID";
		else
			return it.value();
	}

	MouseState::VectorID MouseState::getVectorIDFromName(const QString& name)
	{
		for(QMap<VectorID, QString>::const_iterator it=vectorsNameMap.begin(); it!=vectorsNameMap.end(); it++)
		{
			if(it.value()==name)
				return it.key();
		}
	
		return InvalidVectorID;
	}	

	MouseState::ColorID MouseState::getColorIDFromName(const QString& name)
	{
		for(QMap<ColorID, QString>::const_iterator it=colorsNameMap.begin(); it!=colorsNameMap.end(); it++)
		{
			if(it.value()==name)
				return it.key();
		}
	
		return InvalidColorID;
	}	

	MouseState::VectorID MouseState::getPixelVectorID(const VectorID& vID)
	{
		return validate(static_cast<VectorID>(vID - (vID % NumBasis)));
	}

	MouseState::BasisID MouseState::getVectorBasis(const VectorID& vID)
	{
		return static_cast<BasisID>(vID % NumBasis);
	}

	MouseState::ColorID MouseState::getCorrespondingColorID(const VectorID& vID)
	{
		return validate(static_cast<ColorID>(vID - (vID % NumBasis)));
	}

	bool MouseState::isBasisRelative(const BasisID& bID)
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

// SceneWidget :
	SceneWidget::SceneWidget(MainWidget* _qvglParent, TopBar* _topBar, BottomBar* _bottomBar)
	 : 	qvglParent(_qvglParent),
		quad(NULL),
		shaderProgram(NULL),
		topBar(_topBar),
		bottomBar(_bottomBar)
	{
		// Activate focus via mouse click :
		//setFocusPolicy(Qt::ClickFocus);

		try
		{	
			quad = new GeometryInstance(GeometryPrimitives::ReversedQuad(), GL_STATIC_DRAW_ARB);

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
			QGraphicsProxyWidget* graphicsProxy = addWidget(topBar);
			topBar->setGraphicsProxy(graphicsProxy);
			//QObject::connect(this, SIGNAL(sceneRectChanged(const QRectF&)), topBar, SLOT(stretch(const QRectF&)));
		}

		if(bottomBar!=NULL)
		{
			QGraphicsProxyWidget* graphicsProxy = addWidget(bottomBar);
			bottomBar->setGraphicsProxy(graphicsProxy);
			//QObject::connect(this, SIGNAL(sceneRectChanged(const QRectF&)), bottomBar, SLOT(stretch(const QRectF&)));
		}
	}

	SceneWidget::~SceneWidget(void)
	{
		delete quad;
		delete shaderProgram;
	}

	void SceneWidget::drawView(View* view)
	{
		float 	imageScale[2],
			sceneScale[2],
			adaptationScale;

		// Get the various scales : 
		view->getAspectRatioScaling(imageScale[0], imageScale[1]);
		qvglParent->getSceneRatioScaling(sceneScale[0], sceneScale[1]);
		adaptationScale = qvglParent->getAdaptationScaling(view->getImageRatio());

		// Load the data : 
		shaderProgram->modifyVar("imageScale", 		GL_FLOAT_VEC2,	imageScale);
		shaderProgram->modifyVar("sceneScale", 		GL_FLOAT_VEC2,	sceneScale);
		shaderProgram->modifyVar("adaptationScale", 	GL_FLOAT,	adaptationScale);
		shaderProgram->modifyVar("viewCenter",		GL_FLOAT_VEC2,	view->viewCenter);
		shaderProgram->modifyVar("homothecyCenter",	GL_FLOAT_VEC2,	view->homothecyCenter);
		shaderProgram->modifyVar("angle",		GL_FLOAT,	view->angle);
		shaderProgram->modifyVar("homothecyScale",	GL_FLOAT,	view->homothecyScale);

		// Draw : 
		view->prepareToDraw();

		quad->draw();
	}

	void SceneWidget::drawView(View* view, const int& x, const int& y, const int& w, const int& h)
	{
		glViewport(x, y, w, h);

		const float sceneRatio = static_cast<float>(w) / static_cast<float>(h);
		float 	imageScale[2],
			sceneScale[2],
			adaptationScale;

		// Get the various scales : 
		view->getAspectRatioScaling(imageScale[0], imageScale[1]);
		qvglParent->getSceneRatioScaling(sceneRatio, sceneScale[0], sceneScale[1]);
		adaptationScale = qvglParent->getAdaptationScaling(sceneRatio, view->getImageRatio());

		// Load the data : 
		shaderProgram->modifyVar("imageScale", 		GL_FLOAT_VEC2,	imageScale);
		shaderProgram->modifyVar("sceneScale", 		GL_FLOAT_VEC2,	sceneScale);
		shaderProgram->modifyVar("adaptationScale", 	GL_FLOAT,	adaptationScale);
		shaderProgram->modifyVar("viewCenter",		GL_FLOAT_VEC2,	view->viewCenter);
		shaderProgram->modifyVar("homothecyCenter",	GL_FLOAT_VEC2,	view->homothecyCenter);
		shaderProgram->modifyVar("angle",		GL_FLOAT,	view->angle);
		shaderProgram->modifyVar("homothecyScale",	GL_FLOAT,	view->homothecyScale);

		// Draw : 
		view->prepareToDraw();

		quad->draw();
	}

	void SceneWidget::drawViewsTable(ViewsTable* viewsTable)
	{
		//std::cout << "Drawing table : " << std::endl;
		for(QMap<View*, Vignette*>::iterator it=viewsTable->begin(); it!=viewsTable->end(); it++)
		{
			int x, y;
			viewsTable->getGLPositionOfVignette(*it, x, y);
			drawView((*it)->getView(), x, y, (*it)->getWidth(), (*it)->getHeight());

			//std::cout << "    At : " << (*it)->x() << 'x' << (*it)->y() << ", size : " << (*it)->getWidth() << 'x' << (*it)->getHeight() << std::endl;
		}

		// Restore view port : 
		glViewport(0, 0, width(), height());
	}

	/*void SceneWidget::getTableParameters(const int& W, const int& H, const int& N, const float& rho, int& a, int& b, int& w, int& h, float& u, float& v) const
	{
		// W 	: width of the scene.
		// H	: height of the scene.
		// N 	: number of elements to display.
		// rho 	: percentage of spacing.
		// a 	: number of columns of the table.
		// b 	: number of rows of the table.
		// w	: width of each vignette.
		// h 	: height of each vignette.
		// u 	: horizontal spacing, adjusted.
		// v 	: vertical spacing, adjusted.
		
		const float sceneRatio = static_cast<float>(W)/static_cast<float>(H);

		// ROUND REQUIRES : 
		// QT <= 4 : QMAKE_CXXFLAGS += -std=c++11
		// QT >= 5 : CONFIG += c++11

		a = std::round(std::sqrt(sceneRatio*static_cast<float>(N)));
		b = std::ceil(static_cast<float>(N)/static_cast<float>(a));

		w = static_cast<int>((1.0f - rho) * static_cast<float>(W) / static_cast<float>(a));
		h = static_cast<int>((1.0f - rho) * static_cast<float>(H) / static_cast<float>(b));

		u = static_cast<float>(W - a*w) / static_cast<float>(a + 1);
		v = static_cast<float>(H - b*h) / static_cast<float>(b + 1);
	}*/

	void SceneWidget::drawBackground(QPainter* painter, const QRectF& rect)
	{
		HdlTexture::unbind();

		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// Enable transparency on the background :
		glEnable(GL_BLEND);
		// TODO : reset blend function.

		ViewsTable* viewsTable = qvglParent->getCurrentViewsTable();

		if(viewsTable!=NULL)
			drawViewsTable(viewsTable);
		else
		{
			View* currentView = qvglParent->getCurrentView();

			if(currentView!=NULL)
				drawView(currentView);
		}

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

		HdlProgram::stopProgram();
		HdlTexture::unbind();
	}

	void SceneWidget::keyPressEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyPressEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getKeyboardState().keyPressed(event);

		//std::cout << "Pressed : " << qvglParent->getKeyboardState().getActionAssociatedToKey(event) << std::endl;
	}

	void SceneWidget::keyReleaseEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyReleaseEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getKeyboardState().keyReleased(event);

		//std::cout << "Released : " << qvglParent->getKeyboardState().getActionAssociatedToKey(event) << std::endl;
	}

	void SceneWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseMoveEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, false, true, false);
	}

	void SceneWidget::wheelEvent(QGraphicsSceneWheelEvent* event)
	{
		QGraphicsScene::wheelEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event);
	}

	void SceneWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mousePressEvent(event);

		// TMP : 
		if(!event->isAccepted() && qvglParent!=NULL)
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

			unsigned char red, green, blue;
			qvglParent->getColorAt(event->scenePos().x(), event->scenePos().y(), red, green, blue);
			std::cout << "MousePressed - Color             : (" << static_cast<int>(red) << "; " << static_cast<int>(green) << ", " << static_cast<int>(blue) << ")" << std::endl;
		}

		// Send the event :
		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, true, false, false);
	}

	void SceneWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseReleaseEvent(event);

		if(!event->isAccepted() && qvglParent!=NULL)
			qvglParent->getMouseState().processEvent(event, false, false, true);
	}

	/*void SceneWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
	{
		QGraphicsScene::mouseDoubleClickEvent(event);

		if(!event->isAccepted())
		{
			std::cout << "TODO - line " << __LINE__ << " : add double click event process." << std::endl;

			// TODO
			//qvglParent->getMouseState().processEvent(event);

			event->accept();
		}
	}*/

// SceneViewWidget :
	SceneViewWidget::SceneViewWidget(MainWidget* _qvglParent, TopBar* _topBar, BottomBar* _bottomBar)
	 : 	contextWidget(NULL),
		sceneWidget(NULL),
		qvglParent(_qvglParent)
	{
		// Create the GL widget : 
		QGLFormat glFormat(QGL::DepthBuffer | QGL::DoubleBuffer);
		contextWidget = new ContextWidget(new QGLContext(glFormat), this);
		setViewport(contextWidget);

		// Create the scene : 
		sceneWidget = new SceneWidget(qvglParent, _topBar, _bottomBar);
		setScene(sceneWidget);

		// Other parameters : 	
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);	// For container catching.
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);	// Because GL has to redraw the complete area.
	}

	SceneViewWidget::~SceneViewWidget(void)
	{ }

	void SceneViewWidget::resizeEvent(QResizeEvent *event)
	{
		if(scene()!=NULL)
		{
			// Force the size of the scene to be exactly equal to the size of the widget :
			scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
			//emit resized(event->size());
		}

		QGraphicsView::resizeEvent(event);
   	}

	void SceneViewWidget::addSubWidget(SubWidget* subWidget)
	{
		QGraphicsProxyWidget* proxy = sceneWidget->addWidget(subWidget);
		subWidget->setGraphicsProxy(proxy);
	}

	void SceneViewWidget::addItem(QGraphicsItem* item)
	{
		sceneWidget->addItem(item);
	}

	void SceneViewWidget::removeItem(QGraphicsItem* item)
	{
		sceneWidget->removeItem(item);
	}

	SubWidget* SceneViewWidget::getUppermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible) const
	{
		if(list.isEmpty())
			return NULL;
		else
		{
			SubWidget* highest = list.front();
			QGraphicsProxyWidget* highestGraphicsProxy = highest->getGraphicsProxy();

			for(QList<SubWidget*>::const_iterator it=list.begin(); it!=list.end(); it++)
			{
				QGraphicsProxyWidget* graphicsProxy = (*it)->getGraphicsProxy();

				if(graphicsProxy!=NULL)
				{
					if(	(!highest->isVisible() && (*it)->isVisible() && onlyIfVisible) || 					// First case.
						(graphicsProxy->zValue() > highestGraphicsProxy->zValue() && ((*it)->isVisible() || !onlyIfVisible)) )	// Generic case.
					{
						highest = (*it);
						highestGraphicsProxy = graphicsProxy;
					}
				}
			}

			if(!highest->isVisible() && onlyIfVisible)
				return NULL;
			else			
				return highest;
		}
	}

	SubWidget* SceneViewWidget::getLowermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible) const
	{
		if(list.isEmpty())
			return NULL;
		else
		{
			SubWidget* lowest = list.front();
			QGraphicsProxyWidget* lowestGraphicsProxy = lowest->getGraphicsProxy();

			for(QList<SubWidget*>::const_iterator it=list.begin(); it!=list.end(); it++)
			{
				QGraphicsProxyWidget* graphicsProxy = (*it)->getGraphicsProxy();

				if(graphicsProxy!=NULL)
				{
					if(	(!lowest->isVisible() && (*it)->isVisible() && onlyIfVisible) ||				// First case.
						(graphicsProxy->zValue() < lowestGraphicsProxy->zValue() && ((*it)->isVisible() || !onlyIfVisible)) )	// Generic case.
					{
						lowest = (*it);
						lowestGraphicsProxy = graphicsProxy;
					}
				}
			}

			if(!lowest->isVisible() && onlyIfVisible)
				return NULL;
			else			
				return lowest;
		}
	}

	void SceneViewWidget::orderSubWidgetsList(QList<SubWidget*>& list, bool onlyIfVisible) const
	{
		if(onlyIfVisible)
		{
			// Remove non-visible widgets : 
			for(QList<SubWidget*>::iterator it=list.begin(); it!=list.end(); )
			{
				if(!(*it)->isVisible())
					it = list.erase(it);
				else
					it++;
			}
		}

		// Sort widget regarding their zValue : 
		for(int p=0; p<list.size(); p++)
		{
			for(int q=0; q<(list.size()-1); q++)
			{
				QGraphicsProxyWidget	*currentGraphicsProxy = list[q]->getGraphicsProxy(),
							*nextGraphicsProxy = list[q+1]->getGraphicsProxy();

				if(currentGraphicsProxy!=NULL && nextGraphicsProxy!=NULL)
				{
					if(nextGraphicsProxy->zValue() > currentGraphicsProxy->zValue())
						list.swap(q+1,q);
				}
			}
		}
	}

	void SceneViewWidget::putItemOnTop(QGraphicsProxyWidget* graphicsProxy)
	{
		if(graphicsProxy!=NULL)
		{
			QList<QGraphicsItem*> itemsList = sceneWidget->items();

			qreal k = itemsList.count()-2;

			for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
			{
				if((*it)!=graphicsProxy->graphicsItem())
				{
					(*it)->setZValue(k);
					k--;
				}
			}

			// Raise to top : 
			graphicsProxy->setZValue(itemsList.count()-1);
		}
	}

	void SceneViewWidget::putItemOnBottom(QGraphicsProxyWidget* graphicsProxy)
	{
		if(graphicsProxy!=NULL)
		{
			QList<QGraphicsItem*> itemsList = sceneWidget->items();

			qreal k = itemsList.count()-1;

			for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
			{
				if((*it)!=graphicsProxy->graphicsItem())
				{
					(*it)->setZValue(k);
					k--;
				}
			}

			// Raise to top : 
			graphicsProxy->setZValue(0.0);
		}
	}

	void SceneViewWidget::makeGLContextAvailable(void)
	{
		contextWidget->makeCurrent();
	}

	void SceneViewWidget::getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue)
	{
		unsigned char rgb[3];

		glReadBuffer( GL_BACK );

		// Subtle point here : the frame buffer is verticaly flipped!
		glReadPixels(x, contextWidget->height()-(y+1), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rgb);

		// Split : 
		red 	= rgb[0];
		green	= rgb[1];
		blue	= rgb[2];
	}

	void SceneViewWidget::getColorAt(int x, int y, QColor& c)
	{
		unsigned char rgb[3];

		glReadBuffer( GL_BACK );

		// Subtle point here : the frame buffer is verticaly flipped!
		glReadPixels(x, contextWidget->height()-(y+1), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rgb);

		// Split : 
		c.setRed(rgb[0]);
		c.setGreen(rgb[1]);
		c.setBlue(rgb[2]);
	}

	void SceneViewWidget::update(void)
	{
		sceneWidget->update();
		//QGraphicsView::update(); // not required.
	}

// MainWidget :
	MainWidget::MainWidget(QWidget* parent)
	 :	QWidget(parent), 
		container(QBoxLayout::LeftToRight, this),
		sceneViewWidget(this, &topBar, &bottomBar),
		currentViewIndex(-1),
		currentViewsTableIndex(-1),
		mainViewsTable(NULL),
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
		barSelected(&bottomBar);

		QObject::connect(&mouseState,		SIGNAL(requestExternalUpdate(void)),		this, 		SLOT(updateMouseStateData(void)));
		QObject::connect(&keyboardState,	SIGNAL(actionReceived(ActionID, bool)),		this, 		SLOT(processAction(ActionID, bool)));
		QObject::connect(&mouseState,		SIGNAL(updated(void)),				this, 		SLOT(performMouseAction(void)));
		QObject::connect(&mouseState,		SIGNAL(mustSetMouseCursor(Qt::CursorShape)),	this, 		SLOT(setMouseCursor(Qt::CursorShape)));
		QObject::connect(this,			SIGNAL(viewAdded(View*)),			&topBar,	SLOT(addView(View*)));
		QObject::connect(this,			SIGNAL(viewsTableAdded(ViewsTable*)),		&topBar,	SLOT(addViewsTable(ViewsTable*)));
		QObject::connect(this,			SIGNAL(subWidgetAdded(SubWidget*)),		&topBar,	SLOT(addSubWidget(SubWidget*)));
		QObject::connect(&topBar,		SIGNAL(selected(TopBar*)),			this, 		SLOT(barSelected(TopBar*)));
		QObject::connect(&topBar,		SIGNAL(changeViewRequest(View*)),		this, 		SLOT(viewRequireDisplay(View*)));
		QObject::connect(&topBar,		SIGNAL(changeViewsTableRequest(ViewsTable*)),	this, 		SLOT(viewsTableRequireDisplay(ViewsTable*)));
		QObject::connect(&topBar,		SIGNAL(requestAction(ActionID)),		this, 		SLOT(processAction(ActionID)));
		QObject::connect(&topBar,		SIGNAL(showSubWidgetRequest(SubWidget*)),	this, 		SLOT(showSubWidget(SubWidget*)));
		QObject::connect(&bottomBar,		SIGNAL(selected(BottomBar*)),			this, 		SLOT(barSelected(BottomBar*)));

		// Create main table view : 
		mainViewsTable = new ViewsTable("Main Table");
		addViewsTable(mainViewsTable);

		std::cout << '[' << __LINE__ << "] Temporary, always showing table." << std::endl;
		changeCurrentViewsTable(0);

		// Add new views to the main table : 
		QObject::connect(this,			SIGNAL(viewAdded(View*)),			mainViewsTable,	SLOT(addView(View*)));
	}

	MainWidget::~MainWidget(void)
	{
		disconnect();

		delete mainViewsTable;

		for(QList<View*>::Iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			(*it)->qvglParent = NULL;

		viewsList.clear();
	}

	void MainWidget::updateMouseStateData(void)
	{
		const QList<MouseState::VectorID>& vectorIDs = mouseState.getVectorIDs();

		for(QList<MouseState::VectorID>::const_iterator it=vectorIDs.begin(); it!=vectorIDs.end(); it++)
		{
			const MouseState::BasisID basisID = MouseState::getVectorBasis(*it);

			if(mouseState.doesVectorRequireUpdate(*it) && ((basisID==MouseState::PixelBasis) || (basisID==MouseState::PixelRelativeBasis))) // use the first one to populate the others : 
			{
				const bool isBasisRelative = (basisID==MouseState::PixelRelativeBasis);

				float 	xGl	= 0.0f, 
					yGl	= 0.0f, 
					xQuad	= 0.0f, 
					yQuad	= 0.0f,
					xImg	= 0.0f,
					yImg	= 0.0f;

				QPointF vPixel = mouseState.getVector(*it); //mouseState.invisibleGetVector(*it);

				View* currentView = getCurrentView();
				ViewsTable* currentViewsTable = getCurrentViewsTable();

				if(currentView!=NULL && currentViewsTable==NULL)
				{
					// Full scene : 
					toGlCoordinates(vPixel.x(), vPixel.y(), xGl, yGl, isBasisRelative);
					toQuadCoordinates(xGl, yGl, xQuad, yQuad, isBasisRelative);
					toImageCoordinates(xQuad, yQuad, xImg, yImg, isBasisRelative);
				}
				else if(currentView!=NULL)
				{
					// Restricted to the vignette frame : 
					QRectF rect = currentViewsTable->getVignetteFrame(currentView);

					toGlCoordinates(vPixel.x(), vPixel.y(), xGl, yGl, isBasisRelative, rect);
					toQuadCoordinates(xGl, yGl, xQuad, yQuad, isBasisRelative, rect, currentView);
					toImageCoordinates(xQuad, yQuad, xImg, yImg, isBasisRelative);
				}

				mouseState.setVector(*it, vPixel); // Clear the require update flag.
				mouseState.setVector(static_cast<MouseState::VectorID>(*it + MouseState::GlBasis), 	QPointF(xGl, yGl));
				mouseState.setVector(static_cast<MouseState::VectorID>(*it + MouseState::QuadBasis),	QPointF(xQuad, yQuad));
				mouseState.setVector(static_cast<MouseState::VectorID>(*it + MouseState::ImageBasis),	QPointF(xImg, yImg));

				// Update the corresponding color : 
				if(mouseState.getFunctionMode()==MouseState::ModeCollection)
				{
					MouseState::ColorID colorID = mouseState.getCorrespondingColorID(*it);
					QColor color;

					if(colorID!=MouseState::InvalidColorID)
					{
						getColorAt(vPixel.x(), vPixel.y(), color);

						mouseState.setColor(colorID, color);
					}

					// Give the information to TopBar in the case of last left position (ONLY) : 
					if((*it)==MouseState::VectorLastLeftPosition)
						topBar.updatePositionAndColor(QPointF(xImg, yImg), color);
				}

				//std::cout << "Updating from " << *it << " to " << static_cast<MouseState::VectorID>(*it + MouseState::GlBasis) << ", " << static_cast<MouseState::VectorID>(*it + MouseState::QuadBasis) << ", " << static_cast<MouseState::VectorID>(*it + MouseState::ImageBasis) << std::endl;
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

	void MainWidget::performMouseAction(void)
	{
		View* 		currentView 		= getCurrentView();
		ViewsTable* 	currentViewsTable 	= getCurrentViewsTable();

		if(currentView!=NULL && mouseState.getFunctionMode()==MouseState::ModeMotion)
		{
			QRectF sceneRect;
	
			if(currentViewsTable!=NULL)
				sceneRect = currentViewsTable->getVignetteFrame(currentView);

			// Get adaptation scale (full screen or frame) :
			float adaptationScale;

			if(currentViewsTable==NULL)
				adaptationScale = getAdaptationScaling(currentView->getImageRatio());
			else
				adaptationScale = getAdaptationScaling(sceneRect.width() / sceneRect.height(), currentView->getImageRatio());


			if(mouseState.isVectorModified(MouseState::VectorLastLeftShiftGl))
			{
				QPointF v = mouseState.getVector(MouseState::VectorLastLeftShiftGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				// ORIGINAL : const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());

				/*float adaptationScale;

				if(currentViewsTable==NULL)
					adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				else
					adaptationScale = getAdaptationScaling(sceneRect.width() / sceneRect.height(), currentView->getImageRatio());*/

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

			if(mouseState.isVectorModified(MouseState::VectorLastWheelUpGl) && mouseState.isWheelDeltaModified())
			{
				QPointF v = mouseState.getVector(MouseState::VectorLastWheelUpGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				// ORIGINAL : const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());

				/*float adaptationScale;

				if(currentViewsTable==NULL)
					adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				else
					adaptationScale = getAdaptationScaling(sceneRect.width() / sceneRect.height(), currentView->getImageRatio());*/

				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				currentView->zoom(v.x(), v.y(), std::pow(1.2f, mouseState.getWheelDelta()));
			}

			if(mouseState.isVectorModified(MouseState::VectorLastWheelDownGl) && mouseState.isWheelDeltaModified())
			{
				QPointF v = mouseState.getVector(MouseState::VectorLastWheelDownGl);

				// Go from Gl coordinates to Translation (before quad, do not use QuadBasis).

				// Adaptation scaling : 
				// ORIGINAL : const float adaptationScale = getAdaptationScaling(currentView->getImageRatio());

				/*float adaptationScale;

				if(currentViewsTable==NULL)
					adaptationScale = getAdaptationScaling(currentView->getImageRatio());
				else
					adaptationScale = getAdaptationScaling(sceneRect.width() / sceneRect.height(), currentView->getImageRatio());*/

				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				currentView->zoom(v.x(), v.y(), std::pow(1.2f, mouseState.getWheelDelta()));
			}
		}
		else
		{
			// Clear wheel state : 
			if(mouseState.isWheelDeltaModified())
				mouseState.getWheelDelta();
		}
	}

	void MainWidget::setMouseCursor(Qt::CursorShape cursorShape)
	{
		setCursor(cursorShape);
		sceneViewWidget.setCursor(cursorShape);
		sceneViewWidget.viewport()->setCursor(cursorShape);
	}

	void MainWidget::viewRequireDisplay(View* view)
	{
		std::cout << "MainWidget::viewRequireDisplay : " << view << std::endl;	

		int idx = viewsList.indexOf(view);

		if(idx>=0 && idx<viewsList.size())
			changeCurrentView(idx);
	}

	void MainWidget::viewRequireDisplay(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());

		viewRequireDisplay(view);
	}

	void MainWidget::viewChangeSelection(View* view)
	{
		int idx = viewsList.indexOf(view);

		if(idx>=0 && idx<viewsList.size())
			changeCurrentView(idx, false);
	}

	void MainWidget::viewUpdated(View* view)
	{
		if(view==getCurrentView() && view!=NULL)
			sceneViewWidget.update();
	}

	void MainWidget::viewUpdated(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());

		viewUpdated(view);
	}

	void MainWidget::viewClosed(View* view)
	{
		int idx = viewsList.indexOf(view);

		// If this view is in the list : 
		if(idx>=0)
		{
			// Remove this view from the list :
			viewsList.removeAt(idx);

			// Disconnect : 
			view->disconnect(this);
			view->qvglParent = NULL;
			view->close();
				
			// Change current view if needed :
			if(currentViewIndex==idx)
			{
				if(getCurrentViewsTable()!=NULL)
					hideCurrentView();
				else
					changeCurrentView(currentViewIndex);
			}
		}
	}

	void MainWidget::viewClosed(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());
		
		viewClosed(view);
	}

	void MainWidget::viewDestroyed(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());

		int idx = viewsList.indexOf(view);

		// If this view is in the list : 
		if(idx>=0)
		{
			// Remove this view from the list :
			viewsList.removeAt(idx);

			// Change current view if needed :
			if(currentViewIndex==idx)
			{
				if(getCurrentViewsTable()!=NULL)
					hideCurrentView();
				else
					changeCurrentView(currentViewIndex);
			}
		}
	}

	void MainWidget::closeAllViews(void)
	{
		while(!viewsList.isEmpty())
		{
			View* view = viewsList.front();
			viewsList.removeFirst();
			
			view->disconnect(this);
			view->qvglParent = NULL;
			view->close();
		}

		// Update : 
		hideCurrentView();
	}

	void MainWidget::viewsTableRequireDisplay(ViewsTable* viewsTable)
	{
		int idx = viewsTablesList.indexOf(viewsTable);

		if(idx>=0 && idx<viewsTablesList.size())
			changeCurrentViewsTable(idx);
	}

	void MainWidget::viewsTableRequireDisplay(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		viewsTableRequireDisplay(viewsTable);
	}

	void MainWidget::viewsTableClosed(ViewsTable* viewsTable)
	{
		int idx = viewsTablesList.indexOf(viewsTable);

		if(idx>=0 && viewsTable!=mainViewsTable) // Do not remove main table
		{
			viewsTablesList.removeAt(idx);
			sceneViewWidget.removeItem(viewsTable);
			viewsTable->disconnect(this);		
		}
	}

	void MainWidget::viewsTableClosed(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		viewsTableClosed(viewsTable);
	}

	void MainWidget::viewsTableDestroyed(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		int idx = viewsTablesList.indexOf(viewsTable);

		if(idx>=0)
			viewsTablesList.removeAt(idx);
	}

	void MainWidget::closeAllViewsTables(void)
	{
		while(!viewsTablesList.isEmpty())
		{
			ViewsTable* viewsTable = viewsTablesList.back();

			if(viewsTable!=mainViewsTable)
				viewsTableClosed(viewsTable);
		}
	}

	void MainWidget::subWidgetSelected(SubWidget* subWidget)
	{
		if(subWidgetsList.contains(subWidget) && subWidget->getQVGLParent()==this && subWidget->getGraphicsProxy()!=NULL)
		{
			// Change opacity of all other subWidgets : 
			for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityIdleBar);
			bottomBar.setWindowOpacity(opacityIdleBar);

			// Change the opacity of the current subWidget : 
			subWidget->setWindowOpacity(opacityActiveSubWidget);

			// Raise the current subWidget : 
			sceneViewWidget.putItemOnTop(subWidget->getGraphicsProxy());

			// Release all buttons from the main interface : 
			keyboardState.forceRelease();
		}
	}

	void MainWidget::subWidgetSelected(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		subWidgetSelected(subWidget);
	}

	void MainWidget::showSubWidget(SubWidget* subWidget)
	{
		if(!subWidget->isVisible())
			subWidget->show();		// implement the Show (1st part)
		else
			subWidgetSelected(subWidget);	// implement the raise (2nd part)
	}

	void MainWidget::showSubWidget(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		showSubWidget(subWidget);
	}
	
	void MainWidget::hideSubWidget(SubWidget* subWidget)
	{
		if(subWidgetsList.contains(subWidget) && subWidget->getQVGLParent()==this && subWidget->getGraphicsProxy()!=NULL)
		{
			// Lower the current subWidget : 
			sceneViewWidget.putItemOnBottom(subWidget->getGraphicsProxy());

			// Raise the top bar : 
			sceneViewWidget.putItemOnTop(topBar.getGraphicsProxy());
		}
	}

	void MainWidget::hideSubWidget(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		hideSubWidget(subWidget);
	}

	void MainWidget::subWidgetClosed(SubWidget* subWidget)
	{
		int idx = subWidgetsList.indexOf(subWidget);

		if(idx!=-1)
		{
			// Remove this view from the list :
			subWidgetsList.removeAt(idx);

			// Disconnect : 
			subWidget->disconnect(this);
			subWidget->setQVGLParent(NULL);
		}
	}

	void MainWidget::subWidgetClosed(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());
		
		subWidgetClosed(subWidget);
	}

	void MainWidget::subWidgetDestroyed(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		int idx = subWidgetsList.indexOf(subWidget);

		if(idx!=-1)
			subWidgetsList.removeAt(idx);
	}

	void MainWidget::nextSubWidget(void)
	{
		// Get the ordered list of widgets : 
		QList<SubWidget*> list = subWidgetsList;
		sceneViewWidget.orderSubWidgetsList(list, true);

		if(list.count()>=2)
		{
			sceneViewWidget.putItemOnBottom(list[0]->getGraphicsProxy());
			sceneViewWidget.putItemOnTop(list[1]->getGraphicsProxy());

			// Change opacities : 
			for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			topBar.setWindowOpacity(opacityIdleBar);
			bottomBar.setWindowOpacity(opacityIdleBar);

			list[1]->setWindowOpacity(opacityActiveSubWidget);			
		}
	}

	void MainWidget::previousSubWidget(void)
	{
		// Get the ordered list of widgets : 
		QList<SubWidget*> list = subWidgetsList;
		sceneViewWidget.orderSubWidgetsList(list, true);

		if(list.count()>=2)
		{
			sceneViewWidget.putItemOnTop(list.back()->getGraphicsProxy());
			
			// Change opacities : 
			for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			topBar.setWindowOpacity(opacityIdleBar);
			bottomBar.setWindowOpacity(opacityIdleBar);

			list.back()->setWindowOpacity(opacityActiveSubWidget);			
		}
	}

	void MainWidget::temporaryHideAllSubWidgets(bool enabled)
	{
		for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			(*it)->temporaryHide(enabled);
	}

	void MainWidget::hideAllSubWidgets(void)
	{
		for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			(*it)->hide();
	}

	void MainWidget::barSelected(TopBar* bar)
	{
		if(bar==&topBar)
		{
			// Change opacity of all other subWidgets : 
			for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityActiveBar);
			bottomBar.setWindowOpacity(opacityActiveBar);

			// Raise the bar : 
			sceneViewWidget.putItemOnTop(bottomBar.getGraphicsProxy());
			sceneViewWidget.putItemOnTop(topBar.getGraphicsProxy());		// Raise TOP on top.

			// Release all buttons from the main interface : 
			keyboardState.forceRelease();
		}	
	}

	void MainWidget::barSelected(BottomBar* bar)
	{
		if(bar==&bottomBar)
		{
			// Change opacity of all other subWidgets : 
			for(QList<SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				(*it)->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityActiveBar);
			bottomBar.setWindowOpacity(opacityActiveBar);

			// Raise the bar : 
			sceneViewWidget.putItemOnTop(topBar.getGraphicsProxy());
			sceneViewWidget.putItemOnTop(bottomBar.getGraphicsProxy());		// Raise BOTTOM on top.

			// Release all buttons from the main interface : 
			keyboardState.forceRelease();
		}
	}

	KeyboardState& MainWidget::getKeyboardState(void)
	{
		return keyboardState;
	}

	MouseState& MainWidget::getMouseState(void)
	{
		return mouseState;
	}

	const KeyboardState& MainWidget::getKeyboardState(void) const
	{
		return keyboardState;
	}

	const MouseState& MainWidget::getMouseState(void) const
	{
		return mouseState;
	}

	View* MainWidget::getCurrentView(void) const
	{
		if(currentViewIndex>=0 && currentViewIndex<viewsList.size())
			return viewsList[currentViewIndex];
		else
			return NULL;
	}

	QList<View*>& MainWidget::getCurrentViewList(void)
	{
		return viewsList;
	}

	ViewsTable* MainWidget::getCurrentViewsTable(void)
	{
		if(currentViewsTableIndex>=0 && currentViewsTableIndex<viewsTablesList.size())
			return viewsTablesList[currentViewsTableIndex];
		else
			return NULL;
	}

	void MainWidget::changeCurrentView(int targetID, bool showNow)
	{
		currentViewIndex = std::min(std::max(targetID, 0), viewsList.size()-1);

		View* currentView = getCurrentView();

		if(currentView!=NULL && showNow)
		{
			// Hide current Table : 
			hideCurrentViewsTable();

			// Change title : 
			topBar.setTitle(*currentView);

			// Show : 
			sceneViewWidget.update();
		}
		else if(currentView==NULL)
			topBar.setTitle("(No View)");
	}

	void MainWidget::hideCurrentView(void)
	{
		topBar.setTitle("(No View)");
		currentViewIndex = -1;
		sceneViewWidget.update();		
	}

	void MainWidget::changeCurrentViewsTable(int targetID)
	{
		// Hide previous table :
		hideCurrentViewsTable();

		currentViewsTableIndex = std::min(std::max(targetID, 0), viewsTablesList.size()-1);

		ViewsTable* currentTable = getCurrentViewsTable();

		if(currentTable!=NULL)
		{
			// Change title : 
			topBar.setTitle(*currentTable);

			// Show : 
			currentTable->setVisible(true);
			sceneViewWidget.update();
		}
	}

	void MainWidget::hideCurrentViewsTable(void)
	{
		ViewsTable* currentTable = getCurrentViewsTable();

		if(currentTable!=NULL)
		{	
			topBar.setTitle("(No View)");
			currentViewsTableIndex = -1;
			currentTable->setVisible(false);
		}
	}

	void MainWidget::getSceneRatioScaling(const float& sceneRatio, float& xSceneScale, float& ySceneScale) const
	{
		xSceneScale = std::min(1.0f, 1.0f/sceneRatio);
		ySceneScale = std::min(1.0f, sceneRatio); 	// The minus sign set the orientation of the GL axis to be the same as Qt.
	}

	void MainWidget::getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const
	{
		/* ORIGINAL
		const float sceneRatio = getSceneRatio();

		xSceneScale = std::min(1.0f, 1.0f/sceneRatio);
		ySceneScale = std::min(1.0f, sceneRatio); 	// The minus sign set the orientation of the GL axis to be the same as Qt.*/

		getSceneRatioScaling(getSceneRatio(), xSceneScale, ySceneScale);
	}

	float MainWidget::getAdaptationScaling(const float& sceneRatio, const float& imageRatio) const
	{
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

	float MainWidget::getAdaptationScaling(const float& imageRatio) const
	{
		/* ORIGINAL
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
		}*/

		return getAdaptationScaling(getSceneRatio(), imageRatio);
	}

	void MainWidget::toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative, const QRectF& rect) const
	{
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
		// ORIGINAL : getSceneRatioScaling(xSceneScale, ySceneScale);
		getSceneRatioScaling(rect.width() / rect.height(), xSceneScale, ySceneScale);

		xGl /= xSceneScale;
		yGl /= ySceneScale;
	}

	void MainWidget::toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const
	{
		/* ORIGINAL
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
		yGl /= ySceneScale;*/

		toGlCoordinates(x, y, xGl, yGl, isRelative, sceneRect());
	}

	void MainWidget::toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const QRectF& rect, View* view) const
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
			// ORIGINAL : const float adaptationScale = getAdaptationScaling(view->getImageRatio());
			const float adaptationScale = getAdaptationScaling(rect.width() / rect.height(), view->getImageRatio());
			float 	x = xGl / adaptationScale,
				y = yGl / adaptationScale;

			// Scene scaling : see toGlCoordinates
			
			// Homothecy :
			if( view->homothecyScale!=0.0f)
			{
				x = (x - view->homothecyCenter[0])/view->homothecyScale + view->homothecyCenter[0];
				y = (y - view->homothecyCenter[1])/view->homothecyScale + view->homothecyCenter[1];
			}
			// else : simplified.

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
			// ORIGINAL : const float adaptationScale = getAdaptationScaling(view->getImageRatio());
			const float adaptationScale = getAdaptationScaling(rect.width() / rect.height(), view->getImageRatio());
			float 	x = xGl / adaptationScale,
				y = yGl / adaptationScale;

			// Scene scaling : see toGlCoordinates

			// Homothecy : 
			if( view->homothecyScale!=0.0f)
			{
				x /= view->homothecyScale;
				y /= view->homothecyScale;
			}

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

	void MainWidget::toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, View* view) const
	{
		toQuadCoordinates(xGl, yGl, xQuad, yQuad, isRelative, sceneRect(), view);
	}

	void MainWidget::toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, View* view) const
	{
		if(view==NULL)
			view = getCurrentView();

		if(view==NULL || !view->isValid())
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

	float MainWidget::getSceneRatio(void) const
	{
		QRectF rect = sceneRect();
		return rect.width() / rect.height();
	}

	QRectF MainWidget::sceneRect(void) const
	{
		return sceneViewWidget.sceneRect();
	}

	void MainWidget::getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue)
	{
		MainWidget::sceneViewWidget.getColorAt(x, y, red, green, blue);
	}

	void MainWidget::getColorAt(int x, int y, QColor& c)
	{
		MainWidget::sceneViewWidget.getColorAt(x, y, c);
	}

	void MainWidget::addView(View* view)
	{
		if(!viewsList.contains(view) && view->qvglParent==NULL)
		{
			viewsList.append(view);

			view->qvglParent = this;

			std::cout << "QVGL::MainWidget::addView : " << view << std::endl;

			// Connect actions : 
			QObject::connect(view, SIGNAL(requireDisplay()),	this, SLOT(viewRequireDisplay()));
			QObject::connect(view, SIGNAL(updated()),		this, SLOT(viewUpdated()));
			QObject::connect(view, SIGNAL(closed()), 		this, SLOT(viewClosed()));
			QObject::connect(view, SIGNAL(destroyed()), 		this, SLOT(viewDestroyed()));

			// Update :
			emit viewAdded(view);
		}
	}

	void MainWidget::addViewsTable(ViewsTable* viewsTable)
	{
		if(!viewsTablesList.contains(viewsTable))
		{
			sceneViewWidget.addItem(viewsTable);
			viewsTablesList.append(viewsTable);

			QObject::connect(viewsTable, SIGNAL(requireDisplay()),		this, SLOT(viewsTableRequireDisplay()));
			QObject::connect(viewsTable, SIGNAL(showView(View*)), 		this, SLOT(viewRequireDisplay(View*)));
			QObject::connect(viewsTable, SIGNAL(viewSelection(View*)),	this, SLOT(viewChangeSelection(View*)));
			QObject::connect(viewsTable, SIGNAL(closed()),			this, SLOT(viewsTableClosed()));
			QObject::connect(viewsTable, SIGNAL(destroyed()),		this, SLOT(viewsTableDestroyed()));

			// Update :
			emit viewsTableAdded(viewsTable);
		}
	}

	void MainWidget::addSubWidget(SubWidget* subWidget)
	{
		if(!subWidgetsList.contains(subWidget) && subWidget->getQVGLParent()==NULL)
		{
			sceneViewWidget.addSubWidget(subWidget);
			
			// Connect : 
			QObject::connect(subWidget, SIGNAL(selected(SubWidget*)), 	this, SLOT(subWidgetSelected(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(showRequest(SubWidget*)),	this, SLOT(showSubWidget(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(hideRequest(SubWidget*)),	this, SLOT(hideSubWidget(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(destroyed()),		this, SLOT(subWidgetDestroyed()));

			// Save link : 
			subWidget->setQVGLParent(this);
			subWidgetsList.append(subWidget);

			// Move and show : 
			subWidget->move(0, topBar.height());
			subWidget->show();

			// Update : 
			emit subWidgetAdded(subWidget);
		}
		else if(subWidgetsList.contains(subWidget) && subWidget->getQVGLParent()==this)
			subWidgetSelected(subWidget);
	}

	void MainWidget::processAction(ActionID action, bool takenBack)
	{
		View* 		currentView 		= getCurrentView();
		ViewsTable* 	currentViewsTable	= getCurrentViewsTable();

		switch(action)
		{
			case ActionUp :
				if(currentView!=NULL) currentView->move(0.0f, -0.1f);
				break;
			case ActionDown :
				if(currentView!=NULL) currentView->move(0.0f, +0.1f);
				break;
			case ActionLeft :
				if(currentView!=NULL) currentView->move(-0.1f, 0.0f);
				break;
			case ActionRight :
				if(currentView!=NULL) currentView->move(+0.1f, 0.0f);
				break;
			case ActionZoomIn :
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
			case ActionZoomOut :
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
			case ActionRotationClockWise :
				if(currentView!=NULL) currentView->rotate(-0.17453f);
				break;
			case ActionRotationCounterClockWise :
				if(currentView!=NULL) currentView->rotate(+0.17453f);
				break;
			case ActionToggleFullscreen :
				if(!sceneViewWidget.isFullScreen())
				{
					// Enter fullscreen : 
					sceneViewWidget.setParent(NULL);
					sceneViewWidget.showFullScreen();
					break;
				}
				// else : 
			case ActionExitFullscreen :
				// Re-attach :
				sceneViewWidget.setParent(this);
				sceneViewWidget.showNormal();
				container.removeWidget(&sceneViewWidget);
				container.addWidget(&sceneViewWidget);
				break;
			case ActionResetView :
				if(currentView!=NULL) currentView->reset();
				break;
			case ActionPreviousView :
				changeCurrentView(currentViewIndex - 1);
				break;
			case ActionNextView :
				changeCurrentView(currentViewIndex + 1);
				break;
			case ActionCloseView :
				if(currentView!=NULL) currentView->close();
				break;
			case ActionCloseAllViews :
				closeAllViews();
				break;
			case ActionCloseViewsTable : 
				if(currentViewsTable!=NULL) currentViewsTable->close();
				break;
			case ActionCloseAllViewsTables : 
				closeAllViewsTables();
				break;
			case ActionMotionModifier :
				if(!takenBack)
					mouseState.setFunctionMode(MouseState::ModeMotion);
				else
					mouseState.setFunctionMode(MouseState::ModeCollection);
				break;
			case ActionRotationModifier :
				std::cout << '[' << __LINE__ << "] ActionRotationModifier not available." << std::endl;
				break;
			case ActionNextSubWidget :
				nextSubWidget();
				break;
			case ActionPreviousSubWidget :
				previousSubWidget();
				break;
			case ActionTemporaryHideAllSubWidgets :
				temporaryHideAllSubWidgets(true);
				break;
			case ActionTemporaryUnhideAllSubWidgets :
				temporaryHideAllSubWidgets(false);
				break;
			case ActionHideAllSubWidgets :
				hideAllSubWidgets();
				break;
			case NoAction :
				break;
			default : 
				throw Exception("MainWidget::processAction - Unknown action (code : " + toString(action) + ").", __FILE__, __LINE__);
		}
	}

