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
#include <QApplication>
#include <QColorDialog>

using namespace QVGL;

// Tools : 
	const QString getVisualPartName(const VisualPart& part)
	{
		#define PART( id ) case id : return GLIP_STR(id);

		switch(part)
		{
			PART( ViewVisualPart )
			PART( VignetteVisualPart )
			PART( ViewsTableVisualPart )
			PART( SubWidgetVisualPart )
			PART( TopBarVisualPart )
			PART( BottomBarVisualPart )
			PART( NotAVisualPart )
			default : 
				return "<UnknownVisualPart>";
		}

		#undef PART
	}

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
		QObject::connect(this, SIGNAL(internalDataUpdated()), this, SIGNAL(updated())); // signal equivalence
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
		QObject::connect(this, SIGNAL(internalDataUpdated()), this, SIGNAL(updated())); // signal equivalence
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
	
		emit internalDataUpdated();
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

	void View::getImageCenter(float& x, float& y) const
	{
		if(texture!=NULL)
		{
			x = static_cast<float>(texture->getWidth())/2.0f;
			y = static_cast<float>(texture->getHeight())/2.0f;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
		}
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

	QString View::getSizeString(size_t sizeInBytes)
	{
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

	QString View::getSizeString(void) const
	{
		if(texture!=NULL)
			return getSizeString(texture->getSize());
		else
			return "0 B";
	}

	QString View::getDescriptionToolTip(void)
	{
		if(texture!=NULL)
		{
			QString toolTip;

			toolTip += "<table>";

			if(!name.isEmpty())
				toolTip += tr("<tr><td><i>Name</i></td><td>:</td><td>%1</td></tr>").arg(name);

			toolTip += tr("<tr><td><i>Size</i></td><td>:</td><td>%1x%2 (%3)</td></tr>").arg(texture->getWidth()).arg(texture->getHeight()).arg(getSizeString(texture->getSize()));
			toolTip += tr("<tr><td><i>Size (Driver)</i></td><td>:</td><td>%1</td></tr>").arg(getSizeString(texture->getSizeOnGPU(0)));
			toolTip += tr("<tr><td><i>Mode</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(texture->getGLMode() ).c_str());
			toolTip += tr("<tr><td><i>Depth</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(texture->getGLDepth() ).c_str());
			toolTip += tr("<tr><td><i>Filtering</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(texture->getMinFilter() ).c_str()).arg(getGLEnumName(texture->getMagFilter() ).c_str());
			toolTip += tr("<tr><td><i>Wrapping</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(texture->getSWrapping() ).c_str()).arg(getGLEnumName(texture->getTWrapping() ).c_str());
			toolTip += tr("<tr><td><i>Mipmap</i></td><td>:</td><td>%1 / %2</td></tr>").arg(texture->getBaseLevel()).arg(texture->getMaxLevel());

			for(QMap<QString, QString>::const_iterator it=infos.begin(); it!=infos.end(); it++)
				toolTip += tr("<tr><td><i>%1</i></td><td>:</td><td>%2</td></tr>").arg(it.key()).arg(it.value());

			toolTip += "</table>";

			return toolTip;
		}
		else
			return "<i>Description not available.</i>";
	}

// Vignette :
		const QColor	Vignette::frameColor		= QColor(128, 	128, 	128),
				Vignette::selectedFrameColor	= QColor(192, 	192, 	192),
				Vignette::titleBarColor		= QColor(32, 	 32, 	 32, static_cast<unsigned char>(titleBarOpacity * 255.0f)),
				Vignette::titleColor		= QColor(224, 	224, 	224),
				Vignette::infosColor		= QColor(160, 	160,	160);
		const float	Vignette::frameThickness	= 3.0f,
				Vignette::titleBarOpacity	= 0.75f;

		Vignette::Vignette(View* _view)
		 : 	QObject(NULL),
			QGraphicsItemGroup(NULL),
			selected(false),			
			view(_view),
			frame(0, 0, 1, 1),
			titleBar(0, 0, 1, 1)
		{
			// Set user data of parts : 
			setData(QGraphicsItemVisualPartKey, QVariant(VignetteVisualPart));
			frame.setData(QGraphicsItemVisualPartKey, QVariant(VignetteVisualPart));
			titleBar.setData(QGraphicsItemVisualPartKey, QVariant(VignetteVisualPart));
			title.setData(QGraphicsItemVisualPartKey, QVariant(VignetteVisualPart));
			infos.setData(QGraphicsItemVisualPartKey, QVariant(VignetteVisualPart));

			setFiltersChildEvents(true);
			//setAcceptHoverEvents(true); // not needed anymore ?

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
			QObject::connect(view, SIGNAL(nameChanged()), 		this, SLOT(updateTitle()));
			QObject::connect(view, SIGNAL(internalDataUpdated()), 	this, SLOT(updateInfos()));
		}

		Vignette::~Vignette(void)
		{
			view = NULL;
			removeFromGroup(&frame);
			removeFromGroup(&titleBar);
			removeFromGroup(&title);
			removeFromGroup(&infos);
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

		void Vignette::enforceOrdering(void)
		{
			/*frame.stackBefore(&titleBar);
			title.stackBefore(&frame);
			infos.stackBefore(&frame);*/

			// To be removed.
		}

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
			if(view!=NULL)
			{
				updateTitle();
				updateInfos();
			}

			enforceOrdering();
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
			titleBar.setToolTip(view->getDescriptionToolTip());

			QString titleStr = view->getName();

			// Compute the maximum number of characters to fill 80% max :
			QFontMetrics metrics(title.font());
			int maxCharacters = static_cast<int>(0.8f * static_cast<float>(getWidth())/static_cast<float>(metrics.width(' ')));

			if(titleStr.size()>maxCharacters)
				titleStr = tr("%1...").arg(titleStr.left(maxCharacters-3));

			title.setText(titleStr);
			setTitleBarHeight();
			enforceOrdering();
		}

		void Vignette::updateInfos(void)
		{
			titleBar.setToolTip(view->getDescriptionToolTip());

			QString text;
			if(view->isValid())
				text = tr("%1x%2").arg(view->getFormat().getWidth()).arg(view->getFormat().getHeight());
			else
				text = "(Invalid View)";
			
			QFontMetrics metrics(infos.font());
			infos.setText(text);
			infos.setPos(getWidth() - metrics.width(text) - 6, 0);
			setTitleBarHeight();
			enforceOrdering();
		}
	
// ViewsTable :
	float ViewsTable::rho = 0.04; // Percentage of spacing

	ViewsTable::ViewsTable(const QString& tableName)
	 :	QGraphicsItemGroup(NULL),
		name(tableName),
		emptyNotification("(Empty)")
	{
		// Set user data of parts : 
		setData(QGraphicsItemVisualPartKey, QVariant(ViewsTableVisualPart));
		emptyNotification.setData(QGraphicsItemVisualPartKey, QVariant(ViewsTableVisualPart));

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

		a = std::ceil(std::sqrt(sceneRatio*static_cast<float>(N)) - 0.5); // std::round(x) \approx std::ceil(x-0.5)
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
	
	QRectF ViewsTable::getVignetteFrame(const View* view) const
	{
		QMap<View*, Vignette*>::const_iterator it = vignettesList.find(const_cast<View*>(view));

		if(it!=vignettesList.end())
			return it.value()->boundingRect();
		else
			return QRectF(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	bool ViewsTable::isClosed(void) const
	{
		return (scene()==NULL);
	}

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

			vignettesList[view]->deleteLater();
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
	SubWidget::SubWidget(const Flag _flags, QWidget* parent)
	 : 	QWidget(parent),
		layout(this),
		titleBar(&titleWidget),
		titleLabel(this),
		hideButton(this),
		widget(NULL),
		qvglParent(NULL),
		graphicsProxy(NULL),
		flags(_flags),
		motionActive(false),
		resizeActive(false),
		resizeHorizontalLock(false),
		resizeVerticalLock(false),
		anchorMode(AnchorFree)
	{
		installEventFilter(this);

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
		hideButton.setObjectName("SubWidgetHideButton"); // Load for this specific name from the stylesheet

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
			const bool resizeable = (flags & NotResizeable)==0;
			const QPoint 	cw 		 = mapFromGlobal(event->globalPos());
					mousePress  	 = mapToParent(cw);
					sizeAtMousePress = size();

			// Vector from the initial widget position to the start click position. After that, move the window to the current cursor position plus this vector : 
			offset = mousePress - pos(); 

			const int borderResize = 2 * layout.margin();
			resizeActive = resizeable && ((cw.x()<borderResize) || ((width()-cw.x())<borderResize) || ((height()-cw.y())<borderResize)); // All borders except the title.
			motionActive = !resizeActive;

			// Further test on the resize action : 
			if(resizeActive)
			{
				resizeHorizontalLock 	= ((height()-cw.y())>borderResize) || anchorMode!=AnchorFree; // All anchors lock the vertical resize
				resizeVerticalLock	= ((cw.x()>borderResize) && ((width()-cw.x())>borderResize)) || anchorMode==AnchorMaximized; // Only the maximized anchor lock the horizontal resize.
				resizeActive		= !(resizeHorizontalLock && resizeVerticalLock);	
			}

			// Accept the event : 
			event->accept();

			// Send selection signal : 
			emit selected(this);
		}
		// Let QWidget::mousePressEvent decide of acceptance.
	}

	void SubWidget::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mouseMoveEvent(event);

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
				const bool anchorable = (flags & NotAnchorable)==0;

				// Get the new position : 
				const QPoint	c = mapToParent(mapFromGlobal(event->globalPos())),
						p = c - offset;

				if(anchorable && c.x()<=0)
					setAnchor(AnchorLeft);
				else if(anchorable && c.x()>=(sceneRect.width()-1))
 					setAnchor(AnchorRight);
				else if(anchorable && (c.y()<=0 || c.y()>=(sceneRect.height()-1)))
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

			// And accept the event (overwrite acceptance) : 
			event->accept();
		}
		// Let QWidget::mouseMoveEvent decide of acceptance.
	}

	void SubWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		QWidget::mousePressEvent(event);

		if(!event->isAccepted())
		{
			motionActive = false;
			resizeActive = false;

			// And accept the event (overwrite acceptance) : 
			event->accept();
		}
		// Let QWidget::mousePressEvent decide of acceptance
	}

	void SubWidget::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QWidget::mouseDoubleClickEvent(event);

		// It seems that the previous line always accepts the event. 
		// We assume the titleBar discrimination is sufficient.

		if(qvglParent!=NULL && titleWidget.underMouse())
		{
			const bool maximizable = (flags & NotMaximizable)==0;

			if(maximizable)
			{
				if(anchorMode!=AnchorMaximized)
					setAnchor(AnchorMaximized);
				else
					setAnchor(AnchorFree);
			}

			// And accept the event (overwrite acceptance) : 
			event->accept();
		}
		// Let QWidget::mouseDoubleClickEvent decide of acceptance.
	}

	void SubWidget::wheelEvent(QWheelEvent* event)
	{
		QWidget::wheelEvent(event);

		// And absorb all : 
		event->accept();
	}

	void SubWidget::showEvent(QShowEvent* event)
	{
		UNUSED_PARAMETER(event)

		// Make sure the widget is visible on the current portion of the scene :
		resetPosition(false);
	}

	void SubWidget::closeEvent(QCloseEvent* event)
	{
		if(widget!=NULL)
		{
			QCoreApplication::sendEvent(widget, event);
			if(event->isAccepted())
				QWidget::closeEvent(event);
		}
	}

	void SubWidget::addChild(QObject* pObject)
	{
		if(pObject && pObject->isWidgetType())
		{
			pObject->installEventFilter(this);

			const QObjectList& childList = pObject->children();
			for(QObjectList::const_iterator it=childList.begin(); it!=childList.end(); it++)
				addChild(*it);
		}
	}

	void SubWidget::removeChild(QObject* pObject)
	{
		if(pObject && pObject->isWidgetType())
		{
			pObject->removeEventFilter(this);

			const QObjectList& childList = pObject->children();
			for(QObjectList::const_iterator it=childList.begin(); it!=childList.end(); it++)
				removeChild(*it);
		}
	}

	void SubWidget::childEvent(QChildEvent* e)
	{
		if(e->child()->isWidgetType())
		{
			if(e->type()==QEvent::ChildAdded)
				addChild(e->child());
			else if (e->type()==QEvent::ChildRemoved)
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
		else if(/*e->type()==QEvent::KeyPress || e->type()==QEvent::KeyRelease ||*/ e->type()==QEvent::MouseButtonDblClick || e->type()==QEvent::MouseButtonPress || e->type()==QEvent::MouseButtonRelease || e->type()==QEvent::FocusIn)
			emit selected(this);
	
		// Test inner widget actions : 
		if(getInnerWidget()!=NULL && e->type()==QEvent::KeyPress)
		{
			/*
				This following piece of code takes all the actions attached to the inner widget, test if they are restricted (remember to setShortcutContext(Qt::WidgetShortcut) or setShortCut(Qt::WidgetWithChildrenShortcut)) and if the keyPress matches the shortcut sequences. If true, the specific action is triggered (can happen to multiple actions) and the event is discarded. This is only true if the subWidget is on top of the stack.
			*/

			bool test = false;
			QKeyEvent* keyEvent = reinterpret_cast<QKeyEvent*>(e);
			const QKeySequence received = QKeySequence(keyEvent->key() | keyEvent->modifiers());
			QList<QAction*> innerWidgetActions = getInnerWidget()->actions();
			for(QList<QAction*>::iterator it=innerWidgetActions.begin(); it!=innerWidgetActions.end(); it++)
			{
				if((*it)->isEnabled() && ((*it)->shortcutContext()==Qt::WidgetShortcut || (*it)->shortcutContext()==Qt::WidgetWithChildrenShortcut) && (*it)->shortcut().matches(received)>QKeySequence::PartialMatch)
				{
					(*it)->trigger();
					test = true;
				}
			}

			if(test)
				return true;
			//else continue.
		}

		if(target==this)
			return QWidget::eventFilter(target, e); // Give to the base class.
		else
			return false; // Agree
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
			default :
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
			graphicsProxy->setData(QGraphicsItemVisualPartKey, 		QVariant(SubWidgetVisualPart));
			graphicsProxy->setData(QGraphicsItemSubWidgetPointerKey,	QVariant::fromValue<SubWidget*>(this));

			if(graphicsProxy->scene()!=NULL)
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectChanged(const QRectF&)));

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
		}
	}

	QGraphicsProxyWidget* SubWidget::getGraphicsProxy(void)
	{
		return graphicsProxy;
	}

	QPoint SubWidget::mapItemCoordinatesToGlobal(const QPoint& p)
	{
		return SubWidget::mapItemCoordinatesToGlobal(p, this);
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

	void SubWidget::setQVGLParent(GlipViewWidget* _qvglParent)
	{
		qvglParent = _qvglParent;
	}

	GlipViewWidget* SubWidget::getQVGLParent(void)
	{
		return qvglParent;
	}

	void SubWidget::setAnchor(AnchorMode mode)
	{
		switch(mode)
		{
			case AnchorFree :
				move(originalPosition);

				if(anchorMode==AnchorLeft || anchorMode==AnchorRight)
					originalSize.setWidth(width());

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

	void SubWidget::resetPosition(bool force)
	{
		if(force)
		{
			setAnchor(AnchorFree);
			move(0, TopBar::getHeight()); 
		}
		else if(anchorMode==AnchorFree && graphicsProxy!=NULL && graphicsProxy->scene()!=NULL)
		{
			const QRectF sceneRect = graphicsProxy->scene()->sceneRect();
			const int safeBorder = 64; // pixels.
			int 	currentX = x(),
				currentY = y();

			if((x()+safeBorder)>=sceneRect.width())
				currentX = sceneRect.width() - safeBorder;

			if((x()+width())<=safeBorder)
				currentX = safeBorder - width();
			
			if((y()+safeBorder)>=sceneRect.height())
				currentY = sceneRect.height()-safeBorder;

			if((y()+height())<=safeBorder)
				currentY = safeBorder - height();

			move(currentX, currentY);
		}
	}

	void SubWidget::show(void)
	{
		QWidget::show();
		emit showRequest(this);
	}

	void SubWidget::hide(void)
	{
		if((flags & CloseOnHideRequest) > 0)
			close();
		else
		{
			QWidget::hide();
			emit hideRequest(this);
		}
	}

	void SubWidget::close(void)
	{
		QWidget::close();
		emit closed();
	}

	SubWidget* SubWidget::getPtrFromProxyItem(QGraphicsItem* item)
	{
		if(item==NULL)
			return NULL;
		else
			return item->data(QGraphicsItemSubWidgetPointerKey).value<SubWidget*>();
	}

	SubWidget* SubWidget::getPtrFromProxyItem(QGraphicsProxyWidget* proxy)
	{
		if(proxy==NULL)
			return NULL;
		else
			return proxy->data(QGraphicsItemSubWidgetPointerKey).value<SubWidget*>();
	}

	QPoint SubWidget::mapItemCoordinatesToGlobal(const QPoint& p, QGraphicsProxyWidget* ptr)
	{
		if(ptr!=NULL && ptr->scene()!=NULL && !ptr->scene()->views().empty() && ptr->scene()->views().first()!=NULL && ptr->scene()->views().first()->viewport()!=NULL)
		{
			QGraphicsView *view = ptr->scene()->views().first();
    			QPointF scenePos = ptr->mapToScene(p);
    			QPoint viewPos = view->mapFromScene(scenePos);
    			return view->viewport()->mapToGlobal(viewPos);
		}
		else
			return QPoint(0,0);
	}

	QPoint SubWidget::mapItemCoordinatesToGlobal(const QPoint& p, QWidget* ptr)
	{
		if(ptr!=NULL)
			return mapItemCoordinatesToGlobal(p, ptr->graphicsProxyWidget());
		else
			return QPoint(0,0);
	}

	QPoint SubWidget::mapItemCoordinatesToGlobal(const QPoint& p, SubWidget* ptr)
	{
		if(ptr!=NULL)
			return mapItemCoordinatesToGlobal(p, ptr->graphicsProxy);
		else
			return QPoint(0,0);
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
		positionLabel.setText(tr("%1x%2").arg(std::floor(pos.x())).arg(std::floor(pos.y())));
	}

	void PositionColorInfoMini::updateColor(const QColor& color)
	{
		QPalette palette = colorBox.palette();
		palette.setColor(QPalette::Window, color);
		colorBox.setPalette(palette);
		colorBox.setToolTip(tr("<table><tr><td><i>Code</i></td><td>:</td><td>%1</td></tr><tr><td><i>RGB</i></td><td>:</td><td>(%2, %3, %4)</td></tr></table>").arg(color.name()).arg(color.red()).arg(color.green()).arg(color.blue()));
	}

// TopBar
	TopBar* TopBar::singleton = NULL;

	TopBar::TopBar(QWidget* parent)
	 : 	QWidget(parent),
		graphicsProxy(NULL),
		bar(this),
		menuBar(this),
		mainMenu("Menu", this),
		viewsMenu("Views", this),
		viewsTablesMenu("Tables", this),
		subWidgetsMenu("Widgets", this),
		toggleFullscreenAction("Toggle Fullscreen", this),
		openSettingsInterfaceAction("Settings", this),
		openInfosAction("Infos", this),
		quitAction("Quit", this),
		viewsSeparator(NULL),
		closeCurrentViewAction("Close view", this),
		closeAllViewsAction("Close all views", this),
		viewsTablesSeparator(NULL),
		closeCurrentViewsTableAction("Close table", this),
		closeAllViewsTableAction("Close all tables", this),
		subWidgetsSeparator(NULL),
		toggleTemporaryHideAllSubWidgetsAction("Hide all widgets", this),
		hideAllSubWidgetsAction("Close all widgets", this),
		signalMapper(this)
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
		menuBar.addMenu(&viewsTablesMenu);
		menuBar.addMenu(&subWidgetsMenu);
		menuBar.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		bar.addWidget(&menuBar);
		bar.addWidget(&titleLabel);
		bar.addWidget(&positionColorInfo);

		bar.setMargin(0);
		bar.setSpacing(0);

		// Signals connections : 
		QObject::connect(&viewsSignalMapper, 				SIGNAL(mapped(QObject*)), 	this, SLOT(castViewPointer(QObject*)));
		QObject::connect(&viewsTablesSignalMapper,			SIGNAL(mapped(QObject*)), 	this, SLOT(castViewsTablePointer(QObject*)));
		QObject::connect(&widgetsSignalMapper,				SIGNAL(mapped(QObject*)), 	this, SLOT(castSubWidgetPointer(QObject*)));
		QObject::connect(&mainMenu,					SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&viewsMenu,					SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&viewsTablesMenu,				SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&subWidgetsMenu,				SIGNAL(aboutToShow()),		this, SLOT(sendSelectedSignal()));
		QObject::connect(&signalMapper,					SIGNAL(mapped(int)),		this, SLOT(transferActionSignal(int)));
		QObject::connect(&openInfosAction,				SIGNAL(triggered()), 		this, SIGNAL(requestOpenInfos()));
		QObject::connect(&openSettingsInterfaceAction,			SIGNAL(triggered()), 		this, SIGNAL(requestOpenSettingsInterface()));

		// Signals mapping : 
		QObject::connect(&toggleFullscreenAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&quitAction,					SIGNAL(triggered()), 		&signalMapper, SLOT(map()));
		QObject::connect(&closeCurrentViewAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&closeAllViewsAction,				SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&closeCurrentViewsTableAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&closeAllViewsTableAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&toggleTemporaryHideAllSubWidgetsAction,	SIGNAL(triggered()),		&signalMapper, SLOT(map()));
		QObject::connect(&hideAllSubWidgetsAction,			SIGNAL(triggered()),		&signalMapper, SLOT(map()));

		signalMapper.setMapping(&toggleFullscreenAction,		ActionToggleFullscreen);
		signalMapper.setMapping(&quitAction,				ActionQuit);
		signalMapper.setMapping(&closeCurrentViewAction, 		ActionCloseView);
		signalMapper.setMapping(&closeAllViewsAction,			ActionCloseAllViews);
		signalMapper.setMapping(&closeCurrentViewsTableAction, 		ActionCloseViewsTable);
		signalMapper.setMapping(&closeAllViewsTableAction,		ActionCloseAllViewsTables);
		signalMapper.setMapping(&toggleTemporaryHideAllSubWidgetsAction,ActionToggleTemporaryHideAllSubWidgets);
		signalMapper.setMapping(&hideAllSubWidgetsAction,		ActionHideAllSubWidgets);

		// Init menus : 
		mainMenu.addAction(&toggleFullscreenAction);
		mainMenu.addAction(&openSettingsInterfaceAction);
		mainMenu.addAction(&openInfosAction); 
		mainMenu.addAction(&quitAction);

		viewsSeparator = viewsMenu.addSeparator();
		viewsMenu.addAction(&closeCurrentViewAction);	
		viewsMenu.addAction(&closeAllViewsAction);
		
		viewsTablesSeparator = viewsTablesMenu.addSeparator();
		viewsTablesMenu.addAction(&closeCurrentViewsTableAction);
		viewsTablesMenu.addAction(&closeAllViewsTableAction);

		subWidgetsSeparator = subWidgetsMenu.addSeparator();
		subWidgetsMenu.addAction(&toggleTemporaryHideAllSubWidgetsAction);
		subWidgetsMenu.addAction(&hideAllSubWidgetsAction);

		// Reset : 
		setTitle();

		hide();
	}

	TopBar::~TopBar(void)
	{
		// Clear the menus : 
		viewsActions.clear();
		viewsTablesActions.clear();

		mainMenu.removeAction(&toggleFullscreenAction);
		mainMenu.removeAction(&openSettingsInterfaceAction);
		mainMenu.removeAction(&openInfosAction);
		mainMenu.removeAction(&quitAction);
		viewsMenu.removeAction(&closeCurrentViewAction);
		viewsMenu.removeAction(&closeAllViewsAction);
		viewsTablesMenu.removeAction(&closeCurrentViewsTableAction);
		viewsTablesMenu.removeAction(&closeAllViewsTableAction);
		subWidgetsMenu.removeAction(&toggleTemporaryHideAllSubWidgetsAction);
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

		if(!event->isAccepted())
		{			
			event->accept();
			emit selected(this);
		}
		// Let QWidget::mousePressEvent decide.
	}

	void TopBar::mouseDoubleClickEvent(QMouseEvent* event)
	{
		QWidget::mouseDoubleClickEvent(event);

		if(!event->isAccepted())
			event->accept();
		// Let QWidget::mouseDoubleClickEvent decide.
	}

	void TopBar::mouseMoveEvent(QMouseEvent* event)
	{
		QWidget::mouseMoveEvent(event);

		if(!event->isAccepted())
			event->accept();
		// Let QWidget::mouseMoveEvent decide.
	}

	void TopBar::mouseReleaseEvent(QMouseEvent* event)
	{
		QWidget::mouseReleaseEvent(event);

		if(!event->isAccepted())
			event->accept();
		// Let QWidget::mouseReleaseEvent decide.
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
			it.value()->deleteLater();
			viewsActions.erase(it);
		}
	}

	void TopBar::viewsTableClosed(void)
	{
		ViewsTable* viewTable = reinterpret_cast<ViewsTable*>(QObject::sender());
	
		QMap<ViewsTable*, QAction*>::iterator it = viewsTablesActions.find(viewTable);

		if(viewTable!=NULL && it!=viewsTablesActions.end())
		{
			viewsTablesMenu.removeAction(it.value());
			it.value()->deleteLater();
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
			it.value()->deleteLater();
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
			graphicsProxy->setData(QGraphicsItemVisualPartKey, QVariant(TopBarVisualPart));

			if(graphicsProxy->scene()!=NULL)
			{
				stretch(graphicsProxy->scene()->sceneRect());
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(stretch(const QRectF&)));
			}

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
			show();
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

	void TopBar::setTitle(View& view)
	{
		// Set title :
		titleLabel.setText(view.getName());
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		titleLabel.setToolTip(view.getDescriptionToolTip());
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
		viewsTablesMenu.setWindowOpacity(level);
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
			viewsTablesMenu.insertAction(viewsTablesSeparator, action);
			
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
			subWidgetsMenu.insertAction(subWidgetsSeparator, action);

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
			graphicsProxy->setData(QGraphicsItemVisualPartKey, QVariant(BottomBarVisualPart));

			if(graphicsProxy->scene()!=NULL)
			{
				stretch(graphicsProxy->scene()->sceneRect());
				QObject::connect(graphicsProxy->scene(), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(stretch(const QRectF&)));
			}

			QObject::connect(graphicsProxy, SIGNAL(destroyed(void)), this, SLOT(graphicsProxyDestroyed(void)));
			// show();
		}
	}

	QGraphicsProxyWidget* BottomBar::getGraphicsProxy(void)
	{
		return graphicsProxy;
	}

// InfosDialog :
	InfosDialog::InfosDialog(void)
	 :	SubWidget(static_cast<SubWidget::Flag>(SubWidget::NotResizeable | SubWidget::NotAnchorable | SubWidget::NotMaximizable | SubWidget::CloseOnHideRequest))
	{
		if(!HandleOpenGL::isInitialized())
			throw Glip::Exception("InfosDialog::InfosDialog - GlipLib is not initialized.");

		// Test : 
		const QString PBMSupportString = 
						#ifdef __USE_NETPBM__
							"Yes";
						#else
							"No";
						#endif
		
		const QString LibRawSupportString = 
						#ifdef __USE_LIBRAW__
							"Yes";
						#else
							"No";
						#endif

		// Create the message : 
		int pointSize = message.font().pointSize();
		message.setTextFormat(Qt::RichText);
		message.setTextInteractionFlags(Qt::TextBrowserInteraction);
		message.setOpenExternalLinks(true);
		message.setText( tr(	"<center><p style=\"font-size:%2pt; font-style:bold\">GlipStudio</p></center>"
					"<center><p style=\"font-size:%1pt; font-style:bold\">GlipStudio is the IDE for GlipLib (OpenGL Image Processing Library).<br>\n"
					"Find more information, documentation and examples at : <a href='http://glip-lib.sourceforge.net/'>http://glip-lib.sourceforge.net/</a>.</p></center>"
					"<center><p style=\"font-size:%3pt; font-style:italic\">Copyright &copy; 2010-2015, Ronan Kerviche, MIT License</p></center>"
					"<p style=\"margin-left:1em; margin-right:1em; font-size:%4pt; color:#AAAAAA;\"> Permission is hereby granted, free of charge, to any person obtaining a copy<br>\n"
					"of this software and associated documentation files (the \"Software\"), to deal<br>\n"
					"in the Software without restriction, including without limitation the rights<br>\n"
					"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell<br>\n"
					"copies of the Software, and to permit persons to whom the Software is<br>\n"
					"furnished to do so, subject to the following conditions:<br>\n"
					"<br>\n"
					"The above copyright notice and this permission notice shall be included in<br>\n"
					"all copies or substantial portions of the Software.<br>\n"
					"<br>\n"
					"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR<br>\n"
					"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,<br>\n"
					"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE<br>\n"
					"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER<br>\n"
					"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,<br>\n"
					"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN<br>\n"
					"THE SOFTWARE.\n</p>"
					"<center><table><tr><td><b><i>Binary build date</i></b> : </td><td>%5; %6</td></tr><tr><td><b><i>Hardware vendor : </i></b></td><td>%7</td></tr><tr><td><b><i>Renderer : </i></b></td><td>%8</td></tr><tr><td><b><i>OpenGL version : </i></b></td><td>%9</td></tr><tr><td><b><i>GLSL version : </i></b></td><td>%10</td></tr><tr><td><b><i>PBM Support</i></b> : </td><td>%11</td></tr><tr><td><b><i>LibRaw Support</i></b> : </td><td>%12</td></tr></table></center>"
					).arg(pointSize).arg(pointSize+5).arg(pointSize+2).arg(pointSize).arg(__DATE__).arg(__TIME__).arg(QString::fromStdString(HandleOpenGL::getVendorName())).arg(QString::fromStdString(HandleOpenGL::getRendererName())).arg(QString::fromStdString(HandleOpenGL::getVersion())).arg(QString::fromStdString(HandleOpenGL::getGLSLVersion())).arg(PBMSupportString).arg(LibRawSupportString) );

		// Add the inner widget :
		setInnerWidget(&message);
		setTitle("About");
	}

	InfosDialog::~InfosDialog(void)
	{ }

#ifdef __MAKE_VARIABLES__ 
// VariablesTrackerSubWidget 
	VariablesTrackerSubWidget::VariablesTrackerSubWidget(void)
	 :	innerTreeWidget(NULL)
	{
		// Add the inner widget :
		setInnerWidget(&innerTreeWidget);
		setTitle("Variables Tracker");

		QStringList headersList;
		headersList.append("Name");
		headersList.append("Value (transposed)");
		innerTreeWidget.setHeaderLabels(headersList);
		innerTreeWidget.setColumnCount(2);
		innerTreeWidget.setIndentation(16);
		innerTreeWidget.setContextMenuPolicy(Qt::CustomContextMenu);
		innerTreeWidget.setSelectionMode(QAbstractItemView::ExtendedSelection);

		// Add the already existing variables :
		const QVector<QGUI::VariableRecord*>& variables = QGUI::VariableRecord::getRecords();
		for(QVector<QGUI::VariableRecord*>::const_iterator it=variables.begin(); it!=variables.end(); it++)
			variableAdded(*it);	

		// Be notified if other variables are added, and add other signals :
		QObject::connect(QGUI::VariableRecord::getReferenceRecord(), SIGNAL(recordAdded(const QGUI::VariableRecord*)), this, SLOT(variableAdded(const QGUI::VariableRecord*)));
		QObject::connect(&innerTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(execCustomContextMenu(const QPoint&)));

		const QFontInfo fontInfo(font());
		const int em = fontInfo.pixelSize();
		resize(40*em, height());
	}

	VariablesTrackerSubWidget::~VariablesTrackerSubWidget(void)
	{ }

	void VariablesTrackerSubWidget::updateAlternateColors(void)
	{
		QBrush	original	= innerTreeWidget.palette().background().color(),
			darker		= QBrush(original.color().lighter(90)),
			lighter		= QBrush(original.color().lighter(110));

		QBrush* ptr = NULL;
		int c = 0;
		for(int k=0; k<innerTreeWidget.topLevelItemCount(); k++)
		{
			QTreeWidgetItem* item = innerTreeWidget.topLevelItem(k);

			// This line :
			ptr = (c%2==0) ? (&lighter) : (&darker);
			for(int p=0; p<innerTreeWidget.columnCount(); p++)
				item->setBackground(p, QBrush(*ptr));
			c++;

			// All the sublines :
			for(int l=0; l<item->childCount(); l++)
			{
				ptr = (c%2==0) ? (&lighter) : (&darker);
				for(int p=0; p<innerTreeWidget.columnCount(); p++)
					item->child(l)->setBackground(p, QBrush(*ptr));
				c++;
			}
		}
	}

	void VariablesTrackerSubWidget::variableAdded(const QGUI::VariableRecord* ptr)
	{
		// Check if the type already exists :
		QMap<GLenum, QTreeWidgetItem*>::iterator itRoot = typeRoots.find(ptr->data().getGLType());
		if(itRoot==typeRoots.end())
		{
			// If not, create it :
			QTreeWidgetItem* rootItem = new QTreeWidgetItem(static_cast<int>(ptr->data().getGLType()));
			rootItem->setText(0, QString::fromStdString(getGLEnumName(ptr->data().getGLType())));			

			innerTreeWidget.addTopLevelItem(rootItem);
			typeRoots[ptr->data().getGLType()] = rootItem;

			itRoot = typeRoots.find(ptr->data().getGLType());
		}

		// Create the variable :
		QTreeWidgetItem* variableItem = new QTreeWidgetItem(static_cast<int>(GL_NONE));
		variableItem->setText(0, ptr->getName());

		itRoot.value()->addChild(variableItem);
		items[const_cast<QGUI::VariableRecord*>(ptr)] = variableItem;

		// Update the content of the newly created content :
		variableUpdated(ptr);

		// Connect :
		QObject::connect(ptr, SIGNAL(updated(void)), 	this, SLOT(variableUpdated(void)));
		QObject::connect(ptr, SIGNAL(lockChanged(bool)),this, SLOT(variableLockChanged(bool)));
		QObject::connect(ptr, SIGNAL(destroyed(void)), 	this, SLOT(variableDeleted(void)));

		// Resize :
		itRoot.value()->setExpanded(true);
		innerTreeWidget.resizeColumnToContents(0);
		updateAlternateColors();
	}

	void VariablesTrackerSubWidget::variableUpdated(const QGUI::VariableRecord* ptr)
	{
		// Find the variable :
		QMap<QGUI::VariableRecord*, QTreeWidgetItem*>::iterator it = items.find(const_cast<QGUI::VariableRecord*>(ptr));
	
		if(it!=items.end())
		{
			QString str;
			const HdlDynamicData& data = ptr->data();

			for(int j=0; j<data.getNumColumns(); j++)
			{
				for(int i=0; i<data.getNumRows(); i++)
				{
					const double value = data.get(i, j);

					if(data.isBooleanType())
					{
						if(value==0.0)
							str += "false";
						else
							str += "true";
					}
					else if(data.isIntegerType())
						str += tr("%1").arg(static_cast<long long>(value));
					else
						str += tr("%1").arg(value);

					if(i<(data.getNumRows()-1))
						str += ", ";
				}

				if(j<(data.getNumColumns()-1))
					str +="\n";
			}

			it.value()->setText(1, str);
		}
	}

	void VariablesTrackerSubWidget::variableUpdated(void)
	{
		const QGUI::VariableRecord* ptr = reinterpret_cast<QGUI::VariableRecord*>(QObject::sender());
		variableUpdated(ptr);
	}

	void VariablesTrackerSubWidget::variableLockChanged(const QGUI::VariableRecord* ptr, bool locked)
	{
		// Find the variable :
		QMap<QGUI::VariableRecord*, QTreeWidgetItem*>::iterator it = items.find(const_cast<QGUI::VariableRecord*>(ptr));
	
		if(it!=items.end())
		{
			if(locked)
				it.value()->setText(0, tr("%1 [Locked]").arg(ptr->getName()));
			else
				it.value()->setText(0, ptr->getName());
		}
	}

	void VariablesTrackerSubWidget::variableLockChanged(bool locked)
	{
		const QGUI::VariableRecord* ptr = reinterpret_cast<QGUI::VariableRecord*>(QObject::sender());
		variableLockChanged(ptr, locked);
	}

	void VariablesTrackerSubWidget::variableDeleted(void)
	{
		// Find the variable :
		QGUI::VariableRecord* ptr = reinterpret_cast<QGUI::VariableRecord*>(QObject::sender());
		QMap<QGUI::VariableRecord*, QTreeWidgetItem*>::iterator it = items.find(ptr);
		
		if(it!=items.end())
		{
			delete it.value();
			items.erase(it);
		}
	}

	void VariablesTrackerSubWidget::lockSelection(void)
	{
		QList<QTreeWidgetItem*> selection = innerTreeWidget.selectedItems();
		for(QMap<QGUI::VariableRecord*, QTreeWidgetItem*>::iterator it=items.begin(); it!=items.end(); it++)
		{
			if(selection.contains(it.value()))
				it.key()->lock(true);
		}
	}
	
	void VariablesTrackerSubWidget::unlockSelection(void)
	{
		QList<QTreeWidgetItem*> selection = innerTreeWidget.selectedItems();
		for(QMap<QGUI::VariableRecord*, QTreeWidgetItem*>::iterator it=items.begin(); it!=items.end(); it++)
		{
			if(selection.contains(it.value()))
				it.key()->lock(false);
		}
	}

	void VariablesTrackerSubWidget::execCustomContextMenu(const QPoint& pos)
	{
		QMenu contextMenu(&innerTreeWidget);
		contextMenu.addAction("Lock", this, SLOT(lockSelection(void)));
		contextMenu.addAction("Unlock", this, SLOT(unlockSelection(void)));
		contextMenu.exec(innerTreeWidget.viewport()->mapToGlobal(pos));
	}
#endif

// GlipViewSettings :
	GlipViewSettings::GlipViewSettings(void)
	 : 	takeBackEnabled(NumActions, false)
	{
		resetSettings();
	}

	GlipViewSettings::GlipViewSettings(const GlipViewSettings& c)
	 : 	takeBackEnabled(c.takeBackEnabled),
		keysActionsAssociations(c.keysActionsAssociations),
		backgroundColor(c.backgroundColor),
		translationStep(c.translationStep),
		rotationStep(c.rotationStep),
		zoomFactor(c.zoomFactor)
	{ }

	GlipViewSettings::~GlipViewSettings(void)
	{ }

	QKeySequence GlipViewSettings::getKeysAssociatedToAction(const ActionID& a) const
	{
		/*bool addComa = false;
		QString keysString;

		for(QMap<QKeySequence, ActionID>::const_iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
		{
			if(it.value()==a)
			{
				if(addComa)
					keysString += ", ";

				keysString += it.key().toString();
				addComa = true;
			}
		}
	
		return QKeySequence(keysString);*/

		for(QMap<QKeySequence, ActionID>::const_iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
		{
			if(it.value()==a)
				return it.key();
		}
		return QKeySequence();
	}

	const QMap<QKeySequence, ActionID>& GlipViewSettings::getKeysActionsAssociations(void) const
	{
		return keysActionsAssociations;
	}

	const QVector<bool>& GlipViewSettings::getTakeBackEnabled(void) const
	{
		return takeBackEnabled;
	}

	void GlipViewSettings::setActionKeySequence(const ActionID& a, const QKeySequence& keySequence, bool enableTakeBack)
	{
		// Remove all the previous sequences associated with the action : 
		QList<QKeySequence> previousKeySequence = keysActionsAssociations.keys(a);

		for(QList<QKeySequence>::iterator it=previousKeySequence.begin(); it!=previousKeySequence.end(); it++)
			keysActionsAssociations.remove(*it);

		// Insert the sequences separately : 
		for(unsigned int k=0; k<keySequence.count(); k++)
		{
			QKeySequence ks(keySequence[k]);

			// Remove all the sequences interfering with other actions :
			keysActionsAssociations.remove(ks);

			// Insert the new association : 
			keysActionsAssociations[ks] = a;
		}

		// Take back : 
		takeBackEnabled[a] = enableTakeBack;
	}

	void GlipViewSettings::resetActionsKeySequences(void)
	{
		setActionKeySequence(ActionUp,					Qt::Key_Up);
		setActionKeySequence(ActionDown,				Qt::Key_Down);
		setActionKeySequence(ActionLeft,				Qt::Key_Left);
		setActionKeySequence(ActionRight,				Qt::Key_Right);
		setActionKeySequence(ActionZoomIn,				QKeySequence(Qt::Key_Plus, Qt::SHIFT + Qt::Key_Plus, Qt::KeypadModifier + Qt::Key_Plus));	// Support for keypad and shifts.
		setActionKeySequence(ActionZoomOut,				QKeySequence(Qt::Key_Minus, Qt::SHIFT + Qt::Key_Minus, Qt::KeypadModifier + Qt::Key_Minus));	// Support for keypad and shifts.
		setActionKeySequence(ActionRotationClockWise,			Qt::Key_F);
		setActionKeySequence(ActionRotationCounterClockWise,		Qt::Key_D);
		setActionKeySequence(ActionToggleFullscreen,			Qt::Key_F5);
		setActionKeySequence(ActionExitFullscreen,			Qt::Key_Escape);
		setActionKeySequence(ActionResetView,				Qt::Key_Space);
		setActionKeySequence(ActionPreviousView,			QKeySequence(Qt::CTRL + Qt::Key_Left));
		setActionKeySequence(ActionNextView,				QKeySequence(Qt::CTRL + Qt::Key_Right));
		setActionKeySequence(ActionCloseView,				Qt::Key_Delete);
		setActionKeySequence(ActionCloseAllViews,			QKeySequence(Qt::SHIFT + Qt::Key_Delete));
		setActionKeySequence(ActionMotionModifier,			QKeySequence(Qt::CTRL + Qt::Key_Control, Qt::Key_Control), true); 	// The first correspond the press event, the second to the release.
		setActionKeySequence(ActionRotationModifier,			QKeySequence(Qt::SHIFT + Qt::Key_Shift, Qt::Key_Shift), true);		// (the same)
		setActionKeySequence(ActionNextSubWidget,			QKeySequence(Qt::Key_F7));
		setActionKeySequence(ActionPreviousSubWidget,			QKeySequence(Qt::Key_F8));
		setActionKeySequence(ActionToggleTemporaryHideAllSubWidgets,	QKeySequence(Qt::Key_F6));
		setActionKeySequence(ActionHideAllSubWidgets,			QKeySequence(Qt::Key_F9));
	}

	void GlipViewSettings::resetSettings(void)
	{
		backgroundColor = QColor(25, 25, 25);
		translationStep	= 0.1f;
		rotationStep	= 0.17453f;
		zoomFactor 	= 1.2f;

		keysActionsAssociations.clear();
		takeBackEnabled.clear();
		takeBackEnabled.fill(false, NumActions);
		resetActionsKeySequences();
	}
	
	GlipViewSettings& GlipViewSettings::operator=(const GlipViewSettings& c)
	{
		backgroundColor		= c.backgroundColor;
		takeBackEnabled		= c.takeBackEnabled;
		keysActionsAssociations	= c.keysActionsAssociations;
		translationStep		= c.translationStep;
		rotationStep		= c.rotationStep;
		zoomFactor		= c.zoomFactor;

		return (*this);
	}

// KeyGrabber :
	KeyGrabber::KeyGrabber(const ActionID& _actionID, const QKeySequence& _currentKey, QWidget* parent)
	 : 	QPushButton(getKeyName(_currentKey), parent),
		currentKey(_currentKey),
		actionID(_actionID)
	{ }

	KeyGrabber::~KeyGrabber(void)
	{ }

	QString KeyGrabber::getKeyName(const QKeySequence& key)
	{
		if(key.isEmpty())
			return "(undefined)";
		else
			return key.toString(QKeySequence::NativeText);
	}

	void KeyGrabber::keyPressEvent(QKeyEvent* event)
	{
		if(event->key()!=Qt::Key_Return && event->key()!=Qt::Key_Escape)
		{
			const int s = (event->key() | event->modifiers());
			currentKey = QKeySequence(s);
			setText(getKeyName(currentKey));
			clearFocus();

			emit modified();
		}
		else
			clearFocus();
	}

	const QKeySequence& KeyGrabber::getKey(void) const
	{
		return currentKey;
	}
	
	void KeyGrabber::setKey(const QKeySequence& key)
	{
		currentKey = key;
		setText(getKeyName(currentKey));
	}

	void KeyGrabber::removeKey(void)
	{
		currentKey = QKeySequence();
		setText(getKeyName(currentKey));
	}

// GlipViewSettingsInterface :
	GlipViewSettingsInterface::GlipViewSettingsInterface(const GlipViewSettings& _settings)
	 :	SubWidget(static_cast<SubWidget::Flag>(SubWidget::NotResizeable | SubWidget::NotAnchorable | SubWidget::NotMaximizable | SubWidget::CloseOnHideRequest)), 
		settings(_settings),
		keysGroupBox(&innerWidget),
		othersGroupBox(&innerWidget),
		layout(&innerWidget),
		keysLayout(&keysGroupBox),
		othersLayout(&othersGroupBox),
		//decisionLayout(NULL),
		translationStepLabel("Translation : ", &othersGroupBox),
		rotationStepLabel("Rotation : ", &othersGroupBox),
		zoomFactorLabel("Zoom Factor : ", &othersGroupBox),
		backgroundColorLabel("Background color : ", &othersGroupBox),
		translationStepSpin(&othersGroupBox),
		rotationStepSpin(&othersGroupBox),
		zoomFactorSpin(&othersGroupBox),
		backgroundColorButton(&innerWidget),
		dialogButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Reset | QDialogButtonBox::Cancel, Qt::Horizontal, &innerWidget)
	{
		// Add the inner widget :
		setInnerWidget(&innerWidget);
		setTitle("Settings");

		// Build layout for the actions : 
		const int numColumns = 3;
		for(int k=0; k<NumActions; k++)
		{
			const int p = k/numColumns,
				  q = k - p*numColumns;
			const ActionID actionID = static_cast<ActionID>(k);

			QLabel* label = new QLabel(tr("%1 : ").arg(GlipViewWidget::getActionName(actionID)), &keysGroupBox);
			label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			keysLayout.addWidget(label, p, q*2+0);
			keyGrabbers.push_back(new KeyGrabber(actionID, settings.getKeysAssociatedToAction(actionID), &keysGroupBox));
			keysLayout.addWidget(keyGrabbers.back(), p, q*2+1);

			connect(keyGrabbers.back(), SIGNAL(modified()), this, SLOT(checkKeys(void)));
		}
		keysLayout.setMargin(2);
		keysLayout.setSpacing(2);
		keysGroupBox.setTitle("Controls");
		layout.addWidget(&keysGroupBox);
			
		// Add other settings : 
		translationStepLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		rotationStepLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		zoomFactorLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		backgroundColorLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		translationStepSpin.setValue(settings.translationStep);
		translationStepSpin.setRange(0.0, 1.0);
		rotationStepSpin.setValue(settings.rotationStep);
		rotationStepSpin.setRange(0.0, 1.0);
		zoomFactorSpin.setValue(settings.zoomFactor);
		zoomFactorSpin.setRange(1.0, 2.0);
		updateBackgroundColorButton();

		othersLayout.addWidget(&translationStepLabel);
		othersLayout.addWidget(&translationStepSpin);
		othersLayout.addWidget(&rotationStepLabel);
		othersLayout.addWidget(&rotationStepSpin);
		othersLayout.addWidget(&zoomFactorLabel);
		othersLayout.addWidget(&zoomFactorSpin);
		othersLayout.addWidget(&backgroundColorLabel);
		othersLayout.addWidget(&backgroundColorButton);

		othersGroupBox.setTitle("Miscellaneous");
		layout.addWidget(&othersGroupBox);

		// Add commands : 
		dialogButtons.setCenterButtons(true);
		layout.addWidget(&dialogButtons);

		layout.setMargin(2);
		layout.setSpacing(4);

		// Signals and slots :
		connect(&backgroundColorButton,	SIGNAL(released()), 			this, SLOT(changeBackgroundColor()));
		connect(&dialogButtons, 	SIGNAL(clicked(QAbstractButton*)),	this, SLOT(processDialogButtonPressed(QAbstractButton*)));
	}

	GlipViewSettingsInterface::~GlipViewSettingsInterface(void)
	{
		for(QVector<KeyGrabber*>::iterator it=keyGrabbers.begin(); it!=keyGrabbers.end(); it++)
			delete (*it);
	}

	void GlipViewSettingsInterface::updateBackgroundColorButton(void)
	{
		backgroundColorButton.setStyleSheet(tr("background:%1;").arg(settings.backgroundColor.name()));
	}
	
	void GlipViewSettingsInterface::updateInterface(void)
	{
		for(QVector<KeyGrabber*>::iterator it=keyGrabbers.begin(); it!=keyGrabbers.end(); it++)
			(*it)->setKey(settings.getKeysAssociatedToAction((*it)->actionID));
		translationStepSpin.setValue(settings.translationStep);
		rotationStepSpin.setValue(settings.rotationStep);
		zoomFactorSpin.setValue(settings.zoomFactor);
		updateBackgroundColorButton();
	}

	void GlipViewSettingsInterface::updateData(void)
	{
		const QVector<bool>& originalTakeBack = settings.getTakeBackEnabled();
		for(QVector<KeyGrabber*>::iterator it=keyGrabbers.begin(); it!=keyGrabbers.end(); it++)
			settings.setActionKeySequence((*it)->actionID, (*it)->getKey(), originalTakeBack[(*it)->actionID]);
		settings.translationStep = translationStepSpin.value();
		settings.rotationStep = rotationStepSpin.value();
		settings.zoomFactor = zoomFactorSpin.value();
		settings.backgroundColor = backgroundColorButton.palette().color(QPalette::Window);

		emit applySettings();
	}

	void GlipViewSettingsInterface::checkKeys(const KeyGrabber* ptr)
	{
		for(QVector<KeyGrabber*>::iterator it=keyGrabbers.begin(); it!=keyGrabbers.end(); it++)
		{
			if((*it)->actionID!=ptr->actionID && (*it)->getKey()==ptr->getKey())
				(*it)->removeKey();
		}
	}

	void GlipViewSettingsInterface::checkKeys(void)
	{
		const KeyGrabber* ptr = reinterpret_cast<KeyGrabber*>(QObject::sender());
		if(ptr!=NULL)
			checkKeys(ptr);
	}

	void GlipViewSettingsInterface::changeBackgroundColor(void)
	{
		QColor result = QColorDialog::getColor(settings.backgroundColor, NULL, "Choose background color");

		// If the user pressed 'Ok' : 
		if(result.isValid())
		{
			settings.backgroundColor = result;
			updateBackgroundColorButton();
		}
	}

	void GlipViewSettingsInterface::processDialogButtonPressed(QAbstractButton* button)
	{
		const QDialogButtonBox::StandardButton standardButton = dialogButtons.standardButton(button);

		switch(standardButton)
		{
			case QDialogButtonBox::Ok :
				updateData();
				close();
				break;
			case QDialogButtonBox::Apply : 
				updateData();
				break;
			case QDialogButtonBox::Reset :
				resetSettings();
				break;
			case QDialogButtonBox::Cancel :
				updateInterface();
				close();
				break;
			default : 
				// Nothing to do.
				break;
		}
	}

	void GlipViewSettingsInterface::resetSettings(void)
	{
		settings.resetSettings();
		updateInterface();
		emit applySettings();
	}

	const GlipViewSettings& GlipViewSettingsInterface::getSettings(void) const
	{
		return settings;
	}

// KeyboardState :
	KeyboardState::KeyboardState(const GlipViewSettings& settings)
	 : 	takeBackEnabled(NumActions, false),
		actionPressed(NumActions, false)
	{
		setKeyboardSettings(settings);

		for(int k=0; k<NumActions; k++)
			actionPressed[k] = false;
	}

	KeyboardState::~KeyboardState(void)
	{ }

	ActionID KeyboardState::getActionAssociatedToKey(const QKeySequence& keySequence) const
	{
		QMap<QKeySequence, ActionID>::const_iterator it = keysActionsAssociations.find(keySequence);
		if(it!=keysActionsAssociations.end())
			return (it.value());
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

	void KeyboardState::setKeyboardSettings(const GlipViewSettings& settings)
	{
		keysActionsAssociations = settings.getKeysActionsAssociations();
		takeBackEnabled		= settings.getTakeBackEnabled();
	}

	void KeyboardState::processKeyEvent(QKeyEvent* event, bool pressed)
	{
		ActionID a = getActionAssociatedToKey(event);

		if(!event->isAccepted() && a!=NoAction)
		{
			event->accept();

			actionPressed[a] = pressed;

			if(pressed)
				processAction(a, false);
			else if(takeBackEnabled[a])
				processAction(a, true);
			
		}
	}
	
	void KeyboardState::forceKeyRelease(void)
	{
		for(QMap<QKeySequence, ActionID>::iterator it=keysActionsAssociations.begin(); it!=keysActionsAssociations.end(); it++)
		{
			if(takeBackEnabled[it.value()])
				processAction(it.value(), true);
		}
	}

// MouseState::VectorData
	MouseState::VectorData::VectorData(const VectorID _id, const QString& name, QObject* parent)
	 : 	
		#ifdef __MAKE_VARIABLES__
		record(NULL),
		#endif
		id(_id)
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

	const QPointF& MouseState::VectorData::getVector(void) const
	{
		return vector;
	}

	void MouseState::VectorData::setVector(const QPointF& v, bool setRecord)
	{
		vector = v;
		#ifdef __MAKE_VARIABLES__
		if(setRecord && !record->isLocked())
		{
			record->data().set(v.x(), 0);
			record->data().set(v.y(), 1);
			record->declareUpdate();
		}
		#endif
	}

// MouseState::ColorData
	MouseState::ColorData::ColorData(const ColorID _id, const QString& name, QObject* parent)
	 : 	
		#ifdef __MAKE_VARIABLES__
		record(NULL),
		#endif
		id(_id)
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

	const QColor& MouseState::ColorData::getColor(void) const
	{
		return color;
	}

	void MouseState::ColorData::setColor(const QColor& c, bool setRecord)
	{
		color = c;
		#ifdef __MAKE_VARIABLES__
		if(setRecord && !record->isLocked())
		{
			record->data().set(c.red(), 0);
			record->data().set(c.green(), 1);
			record->data().set(c.blue(), 2);
			record->declareUpdate();
		}
		#endif
	}

// MouseState :
	MouseState::MouseState(QObject* parent)
	 :	functionMode(ModeCollection)
	{
		// Create the maps : 
		vectors.fill(NULL, MaxNumVectors);
		for(unsigned int k=0; k<MaxNumVectors; k++)
		{
			const VectorID vID = static_cast<VectorID>(k);
			if(isVectorIDValid(vID))
				vectors[k] = new VectorData(vID, getVectorIDName(vID), parent);
		}

		colors.fill(NULL, MaxNumColors);
		for(unsigned int k=0; k<MaxNumColors; k++)
		{
			const ColorID cID = static_cast<ColorID>(k);
			if(isColorIDValid(cID))
				colors[k] = new ColorData(cID, getColorIDName(cID), parent);
		}
	}

	MouseState::~MouseState(void)
	{ 
		for(QVector<VectorData*>::iterator it=vectors.begin(); it!=vectors.end(); it++)
			delete (*it);
		vectors.clear();

		for(QVector<ColorData*>::iterator it=colors.begin(); it!=colors.end(); it++)
			delete (*it);
		colors.clear();
	}

	#define UPDATE_VECTOR( vectorName ) updateMouseVectorAndColor(*vectors[vectorName], *vectors[vectorName##Gl], *vectors[vectorName##Quad], *vectors[vectorName##Image], *vectors[vectorName##Fragment], isBasisRelative(getVectorBasis(vectorName)));
	#define UPDATE_VECTOR_AND_COLOR( vectorName, colorName ) updateMouseVectorAndColor(*vectors[vectorName], *vectors[vectorName##Gl], *vectors[vectorName##Quad], *vectors[vectorName##Image], *vectors[vectorName##Fragment], isBasisRelative(getVectorBasis(vectorName)), colors[colorName]);

	void MouseState::processMouseEvent(QGraphicsSceneWheelEvent* event)
	{
		if(event->delta()!=0 && event->orientation()==Qt::Vertical)
		{
			const bool collection = (getMouseFunctionMode()==ModeCollection);
			const float wheelSteps = static_cast<float>(event->delta())/(8.0f*15.0f);
			//wheelDelta += wheelSteps;

			// wheelSteps 	> 0 : away of the user
			// 		< 0 : toward the user

			if(wheelSteps>0)
			{
				//setVector(VectorLastWheelUp, event->scenePos(), true);
				vectors[VectorLastWheelUp]->setVector(event->scenePos(), collection);
				UPDATE_VECTOR(VectorLastWheelUp);
				
				applyMouseAction(*vectors[VectorLastWheelUpGl], NULL, wheelSteps);
			}
			else
			{
				//setVector(VectorLastWheelDown, event->scenePos(), true);
				vectors[VectorLastWheelDown]->setVector(event->scenePos(), collection);
				UPDATE_VECTOR(VectorLastWheelDown);

				applyMouseAction(*vectors[VectorLastWheelDownGl], NULL, wheelSteps);
			}

			event->accept();
		}
		else
			event->ignore();
	}

	void MouseState::processMouseEvent(QGraphicsSceneMouseEvent* event, const bool clicked, const bool moved, const bool released)
	{
		const bool collection = (getMouseFunctionMode()==ModeCollection);
		// event->button()  : The button which triggered the event (empty during drag).
		// event->buttons() : The buttons currently pressed (might not contain the previous in the case of a click).

		if(event->button()==Qt::LeftButton || (event->buttons() & Qt::LeftButton)!=0)
		{	
			if(clicked)
			{
				vectors[VectorLastLeftClick]->setVector(event->scenePos(), collection);
				vectors[VectorLastLeftPosition]->setVector(event->scenePos(), collection);
				
				UPDATE_VECTOR_AND_COLOR(VectorLastLeftClick, ColorUnderLastLeftClick)
				UPDATE_VECTOR_AND_COLOR(VectorLastLeftPosition, ColorUnderLastLeftPosition)

				if(getMouseFunctionMode()==ModeMotion || getMouseFunctionMode()==ModeRotation)
					setMouseCursor(Qt::ClosedHandCursor);

				// No action to perform
			}			

			if(moved)
			{
				vectors[VectorLastLeftShift]->setVector(event->scenePos() - vectors[VectorLastLeftPosition]->getVector(), collection);
				vectors[VectorLastLeftPosition]->setVector(event->scenePos(), collection);				
	
				UPDATE_VECTOR_AND_COLOR(VectorLastLeftPosition, ColorUnderLastLeftPosition)
				UPDATE_VECTOR(VectorLastLeftShift)

				applyMouseAction(*vectors[VectorLastLeftPositionImage], vectors[VectorLastLeftShiftImage]);
				applyMouseAction(*vectors[VectorLastLeftShiftGl]);
			}

			if(released)
			{
				vectors[VectorLastLeftRelease]->setVector(event->scenePos(), (getMouseFunctionMode()==ModeCollection));
				vectors[VectorLastLeftCompletedVector]->setVector(event->scenePos() - vectors[VectorLastLeftClick]->getVector(), collection);

				UPDATE_VECTOR_AND_COLOR(VectorLastLeftRelease, ColorUnderLastLeftRelease)
				UPDATE_VECTOR(VectorLastLeftCompletedVector)

				if(getMouseFunctionMode()==ModeMotion || getMouseFunctionMode()==ModeRotation)
					setMouseCursor(Qt::OpenHandCursor);

				// No action to perform
			}

			if(clicked || moved || released)
				event->accept();
			else
				event->ignore();
		}
		else if(event->button()==Qt::RightButton || (event->buttons() & Qt::RightButton)!=0)
		{
			if(clicked)
			{
				vectors[VectorLastRightClick]->setVector(event->scenePos(), collection);
				vectors[VectorLastRightPosition]->setVector(event->scenePos(), collection);

				UPDATE_VECTOR_AND_COLOR(VectorLastRightClick, ColorUnderLastRightClick)
				UPDATE_VECTOR_AND_COLOR(VectorLastRightPosition, ColorUnderLastRightPosition)

				// No action to perform
			}

			if(moved)
			{
				vectors[VectorLastRightShift]->setVector(event->scenePos() - vectors[VectorLastRightPosition]->getVector(), collection);
				vectors[VectorLastRightPosition]->setVector(event->scenePos(), collection);	

				UPDATE_VECTOR_AND_COLOR(VectorLastRightPosition, ColorUnderLastRightPosition)
				UPDATE_VECTOR(VectorLastRightShift)

				// No action to perform
			}

			if(released)
			{
				vectors[VectorLastRightRelease]->setVector(event->scenePos(), collection);
				vectors[VectorLastRightCompletedVector]->setVector(event->scenePos() - vectors[VectorLastRightClick]->getVector(), collection);

				UPDATE_VECTOR_AND_COLOR(VectorLastRightRelease, ColorUnderLastRightRelease)
				UPDATE_VECTOR(VectorLastRightCompletedVector)

				// No action to perform
			}

			if(clicked || moved || released)
				event->accept();
			else
				event->ignore();
		}

		// Qt::MidButton ?
	}

	#undef UPDATE_VECTOR
	#undef UPDATE_VECTOR_AND_COLOR

	const MouseState::FunctionMode& MouseState::getMouseFunctionMode(void) const
	{
		return functionMode;
	}

	void MouseState::setMouseFunctionMode(const FunctionMode& m)
	{
		functionMode = m;
		
		// See http://qt-project.org/doc/qt-4.8/qt.html#CursorShape-enum for more cursors shapes : 
		switch(m)
		{
			case ModeMotion :
			case ModeRotation :
				setMouseCursor(Qt::OpenHandCursor);
				break;
			case ModeCollection :
				setMouseCursor(Qt::CrossCursor);
				break;
			default : 
				throw Exception("MouseState::setFunctionMode - Unknown function mode.", __FILE__, __LINE__);
		}
	}

	bool MouseState::isVectorIDValid(const VectorID& vID)
	{
		switch(vID)
		{
		#define NAME_MAP( id ) case id : return true;

			NAME_MAP( VectorLastLeftClick )
			NAME_MAP( VectorLastLeftClickGl )
			NAME_MAP( VectorLastLeftClickQuad )
			NAME_MAP( VectorLastLeftClickImage )
			NAME_MAP( VectorLastLeftClickFragment )

			NAME_MAP( VectorLastLeftPosition )
			NAME_MAP( VectorLastLeftPositionGl )
			NAME_MAP( VectorLastLeftPositionQuad )
			NAME_MAP( VectorLastLeftPositionImage )
			NAME_MAP( VectorLastLeftPositionFragment )

			NAME_MAP( VectorLastLeftShift )
			NAME_MAP( VectorLastLeftShiftGl )
			NAME_MAP( VectorLastLeftShiftQuad )
			NAME_MAP( VectorLastLeftShiftImage )
			NAME_MAP( VectorLastLeftShiftFragment )

			NAME_MAP( VectorLastLeftRelease )
			NAME_MAP( VectorLastLeftReleaseGl )
			NAME_MAP( VectorLastLeftReleaseQuad )
			NAME_MAP( VectorLastLeftReleaseImage )
			NAME_MAP( VectorLastLeftReleaseFragment )

			NAME_MAP( VectorLastLeftCompletedVector )
			NAME_MAP( VectorLastLeftCompletedVectorGl )
			NAME_MAP( VectorLastLeftCompletedVectorQuad )
			NAME_MAP( VectorLastLeftCompletedVectorImage )
			NAME_MAP( VectorLastLeftCompletedVectorFragment )

			NAME_MAP( VectorLastRightClick )
			NAME_MAP( VectorLastRightClickGl )
			NAME_MAP( VectorLastRightClickQuad )
			NAME_MAP( VectorLastRightClickImage )
			NAME_MAP( VectorLastRightClickFragment )

			NAME_MAP( VectorLastRightPosition )
			NAME_MAP( VectorLastRightPositionGl )
			NAME_MAP( VectorLastRightPositionQuad )
			NAME_MAP( VectorLastRightPositionImage )
			NAME_MAP( VectorLastRightPositionFragment )

			NAME_MAP( VectorLastRightShift )
			NAME_MAP( VectorLastRightShiftGl )
			NAME_MAP( VectorLastRightShiftQuad )
			NAME_MAP( VectorLastRightShiftImage )
			NAME_MAP( VectorLastRightShiftFragment )

			NAME_MAP( VectorLastRightRelease )
			NAME_MAP( VectorLastRightReleaseGl )
			NAME_MAP( VectorLastRightReleaseQuad )
			NAME_MAP( VectorLastRightReleaseImage )
			NAME_MAP( VectorLastRightReleaseFragment )

			NAME_MAP( VectorLastRightCompletedVector )
			NAME_MAP( VectorLastRightCompletedVectorGl )
			NAME_MAP( VectorLastRightCompletedVectorQuad )
			NAME_MAP( VectorLastRightCompletedVectorImage )
			NAME_MAP( VectorLastRightCompletedVectorFragment )

			NAME_MAP( VectorLastWheelUp )
			NAME_MAP( VectorLastWheelUpGl )
			NAME_MAP( VectorLastWheelUpQuad )
			NAME_MAP( VectorLastWheelUpImage )
			NAME_MAP( VectorLastWheelUpFragment )

			NAME_MAP( VectorLastWheelDown )
			NAME_MAP( VectorLastWheelDownGl )
			NAME_MAP( VectorLastWheelDownQuad )
			NAME_MAP( VectorLastWheelDownImage )
			NAME_MAP( VectorLastWheelDownFragment )
			
			default : return false;
		#undef NAME_MAP
		}
	}	

	bool MouseState::isColorIDValid(const ColorID& cID)
	{
		switch(cID)
		{
		#define NAME_MAP( id ) case id : return true;

			NAME_MAP( ColorUnderLastLeftClick )
			NAME_MAP( ColorUnderLastLeftPosition )
			NAME_MAP( ColorUnderLastLeftRelease )
			NAME_MAP( ColorUnderLastRightClick )
			NAME_MAP( ColorUnderLastRightPosition )
			NAME_MAP( ColorUnderLastRightRelease )

			default : return false;
		#undef NAME_MAP
		}
	}
	
	QString MouseState::getVectorIDName(const VectorID& vID)
	{
		switch(vID)
		{
		#define NAME_MAP( id ) case id : return GLIP_STR( id );

			NAME_MAP( VectorLastLeftClick )
			NAME_MAP( VectorLastLeftClickGl )
			NAME_MAP( VectorLastLeftClickQuad )
			NAME_MAP( VectorLastLeftClickImage )
			NAME_MAP( VectorLastLeftClickFragment )

			NAME_MAP( VectorLastLeftPosition )
			NAME_MAP( VectorLastLeftPositionGl )
			NAME_MAP( VectorLastLeftPositionQuad )
			NAME_MAP( VectorLastLeftPositionImage )
			NAME_MAP( VectorLastLeftPositionFragment )

			NAME_MAP( VectorLastLeftShift )
			NAME_MAP( VectorLastLeftShiftGl )
			NAME_MAP( VectorLastLeftShiftQuad )
			NAME_MAP( VectorLastLeftShiftImage )
			NAME_MAP( VectorLastLeftShiftFragment )

			NAME_MAP( VectorLastLeftRelease )
			NAME_MAP( VectorLastLeftReleaseGl )
			NAME_MAP( VectorLastLeftReleaseQuad )
			NAME_MAP( VectorLastLeftReleaseImage )
			NAME_MAP( VectorLastLeftReleaseFragment )

			NAME_MAP( VectorLastLeftCompletedVector )
			NAME_MAP( VectorLastLeftCompletedVectorGl )
			NAME_MAP( VectorLastLeftCompletedVectorQuad )
			NAME_MAP( VectorLastLeftCompletedVectorImage )
			NAME_MAP( VectorLastLeftCompletedVectorFragment )

			NAME_MAP( VectorLastRightClick )
			NAME_MAP( VectorLastRightClickGl )
			NAME_MAP( VectorLastRightClickQuad )
			NAME_MAP( VectorLastRightClickImage )
			NAME_MAP( VectorLastRightClickFragment )

			NAME_MAP( VectorLastRightPosition )
			NAME_MAP( VectorLastRightPositionGl )
			NAME_MAP( VectorLastRightPositionQuad )
			NAME_MAP( VectorLastRightPositionImage )
			NAME_MAP( VectorLastRightPositionFragment )

			NAME_MAP( VectorLastRightShift )
			NAME_MAP( VectorLastRightShiftGl )
			NAME_MAP( VectorLastRightShiftQuad )
			NAME_MAP( VectorLastRightShiftImage )
			NAME_MAP( VectorLastRightShiftFragment )

			NAME_MAP( VectorLastRightRelease )
			NAME_MAP( VectorLastRightReleaseGl )
			NAME_MAP( VectorLastRightReleaseQuad )
			NAME_MAP( VectorLastRightReleaseImage )
			NAME_MAP( VectorLastRightReleaseFragment )

			NAME_MAP( VectorLastRightCompletedVector )
			NAME_MAP( VectorLastRightCompletedVectorGl )
			NAME_MAP( VectorLastRightCompletedVectorQuad )
			NAME_MAP( VectorLastRightCompletedVectorImage )
			NAME_MAP( VectorLastRightCompletedVectorFragment )

			NAME_MAP( VectorLastWheelUp )
			NAME_MAP( VectorLastWheelUpGl )
			NAME_MAP( VectorLastWheelUpQuad )
			NAME_MAP( VectorLastWheelUpImage )
			NAME_MAP( VectorLastWheelUpFragment )

			NAME_MAP( VectorLastWheelDown )
			NAME_MAP( VectorLastWheelDownGl )
			NAME_MAP( VectorLastWheelDownQuad )
			NAME_MAP( VectorLastWheelDownImage )
			NAME_MAP( VectorLastWheelDownFragment )
			
			default : return QObject::tr("<InvalidVectorID:%1").arg(vID);
		#undef NAME_MAP
		}
	}

	QString MouseState::getColorIDName(const ColorID& cID)
	{
		switch(cID)
		{
		#define NAME_MAP( id ) case id : return GLIP_STR( id );

			NAME_MAP( ColorUnderLastLeftClick )
			NAME_MAP( ColorUnderLastLeftPosition )
			NAME_MAP( ColorUnderLastLeftRelease )
			NAME_MAP( ColorUnderLastRightClick )
			NAME_MAP( ColorUnderLastRightPosition )
			NAME_MAP( ColorUnderLastRightRelease )

			default : return QObject::tr("<InvalidColorID:%1").arg(cID);
		#undef NAME_MAP
		}
	}

	MouseState::VectorID MouseState::getPixelVectorID(const VectorID& vID)
	{
		return static_cast<VectorID>(vID - (vID % NumBasis));
	}

	MouseState::BasisID MouseState::getVectorBasis(const VectorID& vID)
	{
		return static_cast<BasisID>(vID % NumBasis);
	}

	MouseState::ColorID MouseState::getCorrespondingColorID(const VectorID& vID)
	{
		return static_cast<ColorID>(vID - (vID % NumBasis));
	}

	bool MouseState::isBasisRelative(const BasisID& bID)
	{
		return (bID==PixelRelativeBasis) || (bID==GlRelativeBasis) || (bID==QuadRelativeBasis) || (bID==ImageRelativeBasis);
	}

// ContextWidget :
	ContextWidget::ContextWidget(QGLContext* ctx, QWidget* parent)
	 : 	QGLWidget(ctx, parent),
		glipOwnership(false)
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
		std::cout << "ContextWidget::resizeGL - Called" << std::endl;
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

// GLScene :
	GLScene::GLScene(GlipViewWidget* _qvglParent)
	 : 	qvglParent(_qvglParent),
		quad(NULL),
		shaderProgram(NULL),
		clearColorRed(0.1f),
		clearColorGreen(0.1f),
		clearColorBlue(0.1f)
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
			shaderProgram	= new HdlProgram;
			shaderProgram->updateShader(vertexShader,false);
			shaderProgram->updateShader(pixelShader,false);
			shaderProgram->link();
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
	}

	GLScene::~GLScene(void)
	{
		delete quad;
		delete shaderProgram;
		quad = NULL;
		shaderProgram = NULL;
		qvglParent = NULL;
	}

	void GLScene::drawView(View* view)
	{
		float 	imageScale[2],
			sceneScale[2],
			adaptationScale;

		// Get the various scales : 
		view->getAspectRatioScaling(imageScale[0], imageScale[1]);
		qvglParent->getSceneRatioScaling(sceneScale[0], sceneScale[1]);
		adaptationScale = qvglParent->getAdaptationScaling(view->getImageRatio());

		// Load the data : 
		shaderProgram->setVar("imageScale", 		GL_FLOAT_VEC2,	imageScale);
		shaderProgram->setVar("sceneScale", 		GL_FLOAT_VEC2,	sceneScale);
		shaderProgram->setVar("adaptationScale", 	GL_FLOAT,	adaptationScale);
		shaderProgram->setVar("viewCenter",		GL_FLOAT_VEC2,	view->viewCenter);
		shaderProgram->setVar("homothecyCenter",	GL_FLOAT_VEC2,	view->homothecyCenter);
		shaderProgram->setVar("angle",			GL_FLOAT,	view->angle);
		shaderProgram->setVar("homothecyScale",		GL_FLOAT,	view->homothecyScale);

		// Draw : 
		view->prepareToDraw();

		quad->draw();
	}

	void GLScene::drawView(View* view, const int& x, const int& y, const int& w, const int& h)
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
		shaderProgram->setVar("imageScale", 		GL_FLOAT_VEC2,	imageScale);
		shaderProgram->setVar("sceneScale", 		GL_FLOAT_VEC2,	sceneScale);
		shaderProgram->setVar("adaptationScale", 	GL_FLOAT,	adaptationScale);
		shaderProgram->setVar("viewCenter",		GL_FLOAT_VEC2,	view->viewCenter);
		shaderProgram->setVar("homothecyCenter",	GL_FLOAT_VEC2,	view->homothecyCenter);
		shaderProgram->setVar("angle",			GL_FLOAT,	view->angle);
		shaderProgram->setVar("homothecyScale",		GL_FLOAT,	view->homothecyScale);

		// Draw : 
		view->prepareToDraw();

		quad->draw();
	}

	void GLScene::drawViewsTable(ViewsTable* viewsTable)
	{
		for(QMap<View*, Vignette*>::iterator it=viewsTable->begin(); it!=viewsTable->end(); it++)
		{
			int x, y;
			viewsTable->getGLPositionOfVignette(*it, x, y);
			drawView((*it)->getView(), x, y, (*it)->getWidth(), (*it)->getHeight());
		}

		// Restore view port : 
		glViewport(0, 0, width(), height());
	}

	void GLScene::drawBackground(QPainter* painter, const QRectF& rect)
	{
		UNUSED_PARAMETER(painter)
		UNUSED_PARAMETER(rect)

		HdlTexture::unbind();

		glClearColor(clearColorRed, clearColorGreen, clearColorBlue, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// Enable transparency on the background :
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	void GLScene::keyPressEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyPressEvent(event);

		if(!event->isAccepted())
			qvglParent->processKeyEvent(event, true);
	}

	void GLScene::keyReleaseEvent(QKeyEvent* event)
	{
		QGraphicsScene::keyReleaseEvent(event);

		if(!event->isAccepted())
			qvglParent->processKeyEvent(event, false);
	}

	void GLScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
	{
		// Reset event status : 
		event->setAccepted(false);

		// Pass down : 
		QGraphicsScene::mouseMoveEvent(event);

		// If not taken : 
		if(!event->isAccepted())
			qvglParent->processMouseEvent(event, false, true, false);
	}

	void GLScene::wheelEvent(QGraphicsSceneWheelEvent* event)
	{
		// Reset event status : 
		event->setAccepted(false);

		// Pass down : 
		QGraphicsScene::wheelEvent(event);
		
		// If not taken : 
		if(!event->isAccepted())
			qvglParent->processMouseEvent(event);
	}

	void GLScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
	{
		// Reset event status : 
		event->setAccepted(false);

		// Pass down : 
		QGraphicsScene::mousePressEvent(event);

		// If not taken : 
		if(!event->isAccepted())
			qvglParent->processMouseEvent(event, true, false, false);
	}

	void GLScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
	{
		// Reset event status : 
		event->setAccepted(false);

		// Pass down : 
		QGraphicsScene::mouseReleaseEvent(event);

		// If not taken : 
		if(!event->isAccepted())
			qvglParent->processMouseEvent(event, false, false, true);
	}

	void GLScene::setSceneSettings(const GlipViewSettings& settings)
	{
		clearColorRed	= static_cast<float>(settings.backgroundColor.red())/255.0f;
		clearColorGreen	= static_cast<float>(settings.backgroundColor.green())/255.0f;
		clearColorBlue	= static_cast<float>(settings.backgroundColor.blue())/255.0f;
		update();
	}

// GlipViewWidget :
	GlipViewWidget::GlipViewWidget(QWidget* parent, const QSize& originalSize)
	 :	QGraphicsView(parent),
		KeyboardState(GlipViewSettings()),
		MouseState(this),
		contextWidget(NULL),
		glScene(NULL),
		infosDialog(NULL),
		settingsInterface(GlipViewSettings()),
		currentViewIndex(-1),
		currentViewsTableIndex(-1),
		mainViewsTable(NULL),
		opacityActiveSubWidget(0.8),
		opacityIdleSubWidget(0.2),
		opacityActiveBar(0.8),
		opacityIdleBar(0.4)
	{
		// Create the GL widget : 
		QGLFormat glFormat(QGL::DepthBuffer | QGL::DoubleBuffer);
		contextWidget = new ContextWidget(new QGLContext(glFormat), this);
		setViewport(contextWidget);

		// Create the scene : 
		glScene = new GLScene(this);
		setScene(glScene);

		// Other parameters : 	
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);	// For container catching.
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);	// Because GL has to redraw the complete area.

		// Set the size before adding SubWidgets/Bars :
		resize(originalSize);
		scene()->setSceneRect(QRect(QPoint(0, 0), originalSize));

		// Add the bars :
		topBar.setGraphicsProxy(glScene->addWidget(&topBar));
		bottomBar.setGraphicsProxy(glScene->addWidget(&bottomBar));
		//forceItemOrdering();

		// Reset bars views : 
		barSelected(&topBar);
		barSelected(&bottomBar);

		QObject::connect(this,			SIGNAL(viewAdded(View*)),			&topBar,	SLOT(addView(View*)));
		QObject::connect(this,			SIGNAL(viewsTableAdded(ViewsTable*)),		&topBar,	SLOT(addViewsTable(ViewsTable*)));
		QObject::connect(this,			SIGNAL(subWidgetAdded(SubWidget*)),		&topBar,	SLOT(addSubWidget(SubWidget*)));
		QObject::connect(&topBar,		SIGNAL(selected(TopBar*)),			this, 		SLOT(barSelected(TopBar*)));
		QObject::connect(&topBar,		SIGNAL(changeViewRequest(View*)),		this, 		SLOT(viewRequireDisplay(View*)));
		QObject::connect(&topBar,		SIGNAL(changeViewsTableRequest(ViewsTable*)),	this, 		SLOT(viewsTableRequireDisplay(ViewsTable*)));
		QObject::connect(&topBar,		SIGNAL(requestAction(ActionID)),		this, 		SLOT(processAction(ActionID)));
		QObject::connect(&topBar,		SIGNAL(requestOpenInfos()),			this,		SLOT(processOpenInfosRequest()));
		QObject::connect(&topBar,		SIGNAL(requestOpenSettingsInterface()),		this,		SLOT(processOpenSettingsInterfaceRequest()));
		QObject::connect(&topBar,		SIGNAL(showSubWidgetRequest(SubWidget*)),	this, 		SLOT(showSubWidget(SubWidget*)));
		QObject::connect(&bottomBar,		SIGNAL(selected(BottomBar*)),			this, 		SLOT(barSelected(BottomBar*)));
		QObject::connect(&settingsInterface,	SIGNAL(applySettings()),			this,		SLOT(applySettings()));

		// Create main table view : 
		mainViewsTable = new ViewsTable("Main Table");
		addViewsTable(mainViewsTable);
		changeCurrentViewsTable(0);
	
		// Dialogs : 
		infosDialog = new InfosDialog;

		// Add new views to the main table : 
		QObject::connect(this,			SIGNAL(viewAdded(View*)),			mainViewsTable,	SLOT(addView(View*)));

		// Push the settings : 
		glScene->setSceneSettings(settingsInterface.getSettings());
		setKeyboardSettings(settingsInterface.getSettings());
	}

	GlipViewWidget::~GlipViewWidget(void)
	{
		disconnect();

		delete mainViewsTable;

		for(QMap<int,View*>::Iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			(*it)->qvglParent = NULL;
		for(QMap<int,SubWidget*>::Iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			(*it)->setQVGLParent(NULL);

		viewsList.clear();
		settingsInterface.close();
		delete infosDialog;

		//delete glScene; // Will be deleted by the view?
		delete contextWidget;
	}

	void GlipViewWidget::resizeEvent(QResizeEvent *event)
	{
		if(scene()!=NULL)
		{
			// Force the size of the scene to be exactly equal to the size of the widget :
			scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
		}
		QGraphicsView::resizeEvent(event);
   	}

	/*void GlipViewWidget::forceItemOrdering(void)
	{
		// Force all the items to be on bottom, and the widgets to be on top.
		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		// Sort in the list : 
		for(QList<QGraphicsItem*>::iterator it1=itemsList.end(); it1!=itemsList.begin(); it1--)
		{
			QList<QGraphicsItem*>::iterator itLag = itemsList.end();
			for(QList<QGraphicsItem*>::iterator it2=itemsList.begin(); it2!=it1; it2++)
			{
				// If it2 is a widget (SubWidget or a bar) and itLag is not, then swap the two values :
				if(itLag!=itemsList.end() && (((*itLag)->data(QGraphicsItemVisualPartKey).toInt() & WidgetVisualPartMask)==0 && ((*it2)->data(QGraphicsItemVisualPartKey).toInt() & WidgetVisualPartMask)!=0))
					std::swap(*itLag, *it2);

				itLag = it2;
			}
		}

		// Reset to the correct Z-Values : 
		int z = 1;
		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			(*it)->setZValue(itemsList.size()-z);
			z++;
		}

		// To be removed?
	}*/

	void GlipViewWidget::putWidgetOnTop(QGraphicsProxyWidget* graphicsProxy)
	{
		// Assumption : all widgets are already on top of all the others graphicsItems.

		if(graphicsProxy==NULL || (graphicsProxy->data(QGraphicsItemVisualPartKey).toInt() & WidgetVisualPartMask)==0)
			return ;

		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			if((*it)==graphicsProxy->graphicsItem())
			{
				(*it)->setZValue(itemsList.size()-1);
				break;
			}
			else 
				(*it)->setZValue((*it)->zValue()-1);
		}
	}

	void GlipViewWidget::putWidgetOnBottom(QGraphicsProxyWidget* graphicsProxy)
	{
		// Assumption : all widgets are already on top of all the others graphicsItems.

		if(graphicsProxy==NULL || (graphicsProxy->data(QGraphicsItemVisualPartKey).toInt() & WidgetVisualPartMask)==0)
			return ;

		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		int lastZ = -1;
		QList<QGraphicsItem*>::iterator itTarget = itemsList.end();
		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			if(((*it)->data(QGraphicsItemVisualPartKey).toInt() & WidgetVisualPartMask)!=0)
				lastZ = (*it)->zValue();
			else
				break;

			if(itTarget!=itemsList.end())
				(*it)->setZValue((*it)->zValue()+1);
			else if((*it)==graphicsProxy->graphicsItem())
				itTarget = it;
		}

		if(itTarget!=itemsList.end())
			(*itTarget)->setZValue(lastZ);
	}

	SubWidget* GlipViewWidget::getTopSubWidget(bool onlyVisible)
	{
		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			if(((*it)->data(QGraphicsItemVisualPartKey).toInt() & SubWidgetVisualPart)!=0 && ((*it)->isVisible() || !onlyVisible))
				return SubWidget::getPtrFromProxyItem(*it);
		}

		return NULL;
	}

	SubWidget* GlipViewWidget::getBottomSubWidget(bool onlyVisible)
	{
		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		SubWidget* ptr = NULL;
		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			if(((*it)->data(QGraphicsItemVisualPartKey).toInt() & SubWidgetVisualPart)!=0 && ((*it)->isVisible() || !onlyVisible))
				ptr = SubWidget::getPtrFromProxyItem(*it);
		}

		return ptr;
	}

	SubWidget* GlipViewWidget::getSubWidget(int index, bool onlyVisible)
	{
		QList<QGraphicsItem*> itemsList = glScene->items(); // Returns a list in random z-stacking order.
		sortItems(itemsList);

		SubWidget* ptr = NULL;
		int k = 0;
		for(QList<QGraphicsItem*>::iterator it=itemsList.begin(); it!=itemsList.end(); it++)
		{
			if(((*it)->data(QGraphicsItemVisualPartKey).toInt() & SubWidgetVisualPart)!=0 && ((*it)->isVisible() || !onlyVisible))
			{
				ptr = SubWidget::getPtrFromProxyItem(*it);

				if(k==index)
					break;
				else
					k++;
			}
		}

		return ptr;
	}

	void GlipViewWidget::sortItems(QList<QGraphicsItem*>& list, const Qt::SortOrder& order)
	{
		// Simple bubble sort : 
		for(QList<QGraphicsItem*>::iterator it1=list.begin(); it1!=list.end(); it1++)
			for(QList<QGraphicsItem*>::iterator it2=(it1+1); it2!=list.end(); it2++)
				if( ((*it1)->zValue()>(*it2)->zValue()) != (order==Qt::DescendingOrder) )
					std::swap(*it1, *it2);
	}

	void GlipViewWidget::updateMouseVectorAndColor(const VectorData& vPixelBasis, VectorData& vGlBasis, VectorData& vQuadBasis, VectorData& vImageBasis, VectorData& vFragmentBasis, const bool isRelative, ColorData* colorData)
	{
		const bool collection = (getMouseFunctionMode()==ModeCollection);
		const View* currentView = getCurrentView();
		const ViewsTable* currentViewsTable = getCurrentViewsTable();

		float 	xGl	= 0.0f, 
			yGl	= 0.0f, 
			xQuad	= 0.0f, 
			yQuad	= 0.0f,
			xImg	= 0.0f,
			yImg	= 0.0f,
			xFrag	= 0.0f,
			yFrag	= 0.0f;

		const QPointF& vPixel = vPixelBasis.getVector();

		if(currentView!=NULL && currentViewsTable==NULL)
		{
			// Full scene : 
			toGlCoordinates(vPixel.x(), vPixel.y(), xGl, yGl, isRelative);
			toQuadCoordinates(xGl, yGl, xQuad, yQuad, isRelative);
			toImageCoordinates(xQuad, yQuad, xImg, yImg, isRelative);
			toFragmentCoordinates(xQuad, yQuad, xFrag, yFrag, isRelative); 
		}
		else if(currentView!=NULL)
		{
			// Restricted to the vignette frame : 
			QRectF rect = currentViewsTable->getVignetteFrame(currentView);

			toGlCoordinates(vPixel.x(), vPixel.y(), xGl, yGl, isRelative, rect);
			toQuadCoordinates(xGl, yGl, xQuad, yQuad, isRelative, rect, currentView);
			toImageCoordinates(xQuad, yQuad, xImg, yImg, isRelative, currentView);
			toFragmentCoordinates(xQuad, yQuad, xFrag, yFrag, isRelative, currentView); 
		}

		vGlBasis.setVector(QPointF(xGl, yGl), collection);
		vQuadBasis.setVector(QPointF(xQuad, yQuad), collection);
		vImageBasis.setVector(QPointF(xImg, yImg), collection);
		vFragmentBasis.setVector(QPointF(xFrag, yFrag), collection);
		
		if(collection && (vPixelBasis.id==MouseState::VectorLastLeftClick || vPixelBasis.id==MouseState::VectorLastLeftPosition || vPixelBasis.id==MouseState::VectorLastLeftRelease ||
				  vPixelBasis.id==MouseState::VectorLastRightClick || vPixelBasis.id==MouseState::VectorLastRightPosition || vPixelBasis.id==MouseState::VectorLastRightRelease))
		{
			QColor color(0,0,0,0);

			if(colorData!=NULL)
			{
				color = getColorAt(vPixel.x(), vPixel.y());
				colorData->setColor(color, collection);
			}
			topBar.updatePositionAndColor(vImageBasis.getVector(), color);
		}
	}

	void GlipViewWidget::applyMouseAction(const VectorData& vectorData, const VectorData* shiftVectorData, const float wheelDelta)
	{
		View* currentView = getCurrentView();
		const ViewsTable* currentViewsTable = getCurrentViewsTable();

		if(currentView!=NULL && (getMouseFunctionMode()==MouseState::ModeMotion || getMouseFunctionMode()==MouseState::ModeRotation))
		{
			// Get adaptation scale (full screen or frame) :
			float adaptationScale;
			if(currentViewsTable==NULL)
				adaptationScale = getAdaptationScaling(currentView->getImageRatio());
			else
			{
				QRectF sceneRect = currentViewsTable->getVignetteFrame(currentView);
				adaptationScale = getAdaptationScaling(sceneRect.width() / sceneRect.height(), currentView->getImageRatio());
			}

			// Apply : 
			if(getMouseFunctionMode()==MouseState::ModeMotion && vectorData.id==MouseState::VectorLastLeftShiftGl) // translate
			{
				// Translate :
				QPointF v = vectorData.getVector(); // MouseState::VectorLastLeftShiftGl

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
			else if(getMouseFunctionMode()==MouseState::ModeRotation && vectorData.id==VectorLastLeftPositionImage && shiftVectorData!=NULL && shiftVectorData->id==MouseState::VectorLastLeftShiftImage)
			{
				// Rotate : 
				float 	xImageCenter = 0.0f, 
					yImageCenter = 0.0f;

				currentView->getImageCenter(xImageCenter, yImageCenter);

				QPointF a 	= vectorData.getVector(), // MouseState::VectorLastLeftPositionImage
					s	= shiftVectorData->getVector(); // MouseState::VectorLastLeftShiftImage
				a.setX(a.x() - xImageCenter);
				a.setY(a.y() - yImageCenter);
				float	p	= a.x()*s.x() + a.y()*s.y(),
					laSq	= a.x()*a.x() + a.y()*a.y();
				QPointF u	= QPointF(s.x() - p*a.x()/laSq, s.y() - p*a.y()/laSq);
				float	luSq	= u.x()*u.x()+u.y()*u.y(),
					sign	= (u.x()*a.y()-a.x()*u.y())<0.0 ? -1.0f : 1.0f,
					angle	= atan2(sign * std::sqrt(luSq), std::sqrt(laSq));

				currentView->rotate(angle);
			}
			else if(vectorData.id==MouseState::VectorLastWheelUpGl || vectorData.id==MouseState::VectorLastWheelDownGl)
			{
				// Zoom in/out :
				QPointF v = vectorData.getVector();

				v.rx() /= adaptationScale;
				v.ry() /= adaptationScale;

				currentView->zoom(v.x(), v.y(), std::pow(settingsInterface.getSettings().zoomFactor, wheelDelta));
			}
		}
	}

	void GlipViewWidget::setMouseCursor(Qt::CursorShape cursorShape)
	{
		setCursor(cursorShape);
		setCursor(cursorShape);
		viewport()->setCursor(cursorShape);
	}

	void GlipViewWidget::viewRequireDisplay(View* view)
	{
		int idx = viewsList.key(view, -1);

		if(idx>=0)
			changeCurrentView(idx);
	}

	void GlipViewWidget::viewRequireDisplay(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());

		viewRequireDisplay(view);
	}

	void GlipViewWidget::viewChangeSelection(View* view)
	{
		int idx = viewsList.key(view, -1);

		if(idx>=0)
			changeCurrentView(idx, false);
	}

	void GlipViewWidget::viewUpdated(View* view)
	{
		if(view==getCurrentView() && view!=NULL && glScene!=NULL)
			glScene->update();
	}

	void GlipViewWidget::viewUpdated(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());

		viewUpdated(view);
	}

	void GlipViewWidget::viewClosed(View* view)
	{
		const int idx = viewsList.key(view, -1);

		// If this view is in the list : 
		if(idx>=0)
		{
			// Remove this view from the list :
			viewsList.remove(idx);

			// Disconnect : 
			view->disconnect(this);
			view->qvglParent = NULL;
			view->close();
				
			// Change current view if needed :
			if(currentViewIndex==idx && getCurrentViewsTable()==NULL)
				changeCurrentView(currentViewIndex);
		}
	}

	void GlipViewWidget::viewClosed(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());
		
		viewClosed(view);
	}

	void GlipViewWidget::viewDestroyed(void)
	{
		// Get the emitter : 
		View* view = reinterpret_cast<View*>(QObject::sender());
		const int idx = viewsList.key(view, -1);

		// If this view is in the list : 
		if(idx>=0)
		{
			// Remove this view from the list :
			viewsList.remove(idx);

			// Change current view if needed :
			if(currentViewIndex==idx && getCurrentViewsTable()==NULL)
				changeCurrentView(currentViewIndex);
		}
	}

	void GlipViewWidget::closeAllViews(void)
	{
		for(QMap<int,View*>::iterator it=viewsList.begin(); it!=viewsList.end(); it++)
		{
			View* view = it.value();
			it.value() = NULL;
			view->disconnect(this);
			view->qvglParent = NULL;
			view->close();
		}
		viewsList.clear();

		// Update : 
		hideCurrentView();
	}

	void GlipViewWidget::viewsTableRequireDisplay(ViewsTable* viewsTable)
	{
		const int idx = viewsTablesList.key(viewsTable, -1);

		if(idx>=0)
			changeCurrentViewsTable(idx);
	}

	void GlipViewWidget::viewsTableRequireDisplay(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		viewsTableRequireDisplay(viewsTable);
	}

	void GlipViewWidget::viewsTableClosed(ViewsTable* viewsTable)
	{
		const int idx = viewsTablesList.key(viewsTable, -1);

		if(idx>=0) // Do not remove main table
		{
			viewsTablesList.remove(idx);

			if(glScene!=NULL)
				glScene->removeItem(viewsTable);
			viewsTable->disconnect(this);

			if(currentViewsTableIndex==idx)
				changeCurrentViewsTable(currentViewsTableIndex);		
		}
	}

	void GlipViewWidget::viewsTableClosed(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		viewsTableClosed(viewsTable);
	}

	void GlipViewWidget::viewsTableDestroyed(void)
	{
		// Get the emitter : 
		ViewsTable* viewsTable = reinterpret_cast<ViewsTable*>(QObject::sender());

		const int idx = viewsTablesList.key(viewsTable, -1);

		if(idx>=0)
		{
			viewsTablesList.remove(idx);

			if(currentViewsTableIndex==idx)
				changeCurrentViewsTable(currentViewsTableIndex);
		}
	}

	void GlipViewWidget::closeAllViewsTables(void)
	{
		QVector<int> toBeRemoved;
		toBeRemoved.reserve(viewsTablesList.size());

		for(QMap<int,ViewsTable*>::iterator it=viewsTablesList.begin(); it!=viewsTablesList.end(); it++)
		{
			if(it.value()!=mainViewsTable)
			{
				if(glScene!=NULL)
					glScene->removeItem(it.value());
				it.value()->disconnect(this);
				it.value()->close();
				toBeRemoved.push_back(it.key());
			}
		}

		for(QVector<int>::iterator it=toBeRemoved.begin(); it!=toBeRemoved.end(); it++)
			viewsTablesList.remove(*it);

		if(!viewsTablesList.contains(currentViewsTableIndex))
			changeCurrentViewsTable(currentViewsTableIndex);

		/*while(viewsTablesList.size()>1) // Keep the main table
		{
			ViewsTable* viewsTable = viewsTablesList.back();

			if(viewsTable!=mainViewsTable)
				viewsTable->close();
		}*/
	}

	void GlipViewWidget::subWidgetSelected(SubWidget* subWidget)
	{
		if(subWidgetsList.key(subWidget, -1)>=0 && subWidget->getQVGLParent()==this && subWidget->getGraphicsProxy()!=NULL)
		{
			// Forget any temporary hide : 
			if(!temporaryHiddenSubWidgetsList.empty())
				temporaryHideAllSubWidgets(false);

			// Change opacity of all other subWidgets : 
			for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				it.value()->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityIdleBar);
			bottomBar.setWindowOpacity(opacityIdleBar);

			// Change the opacity of the current subWidget : 
			subWidget->setWindowOpacity(opacityActiveSubWidget);

			// Raise the current subWidget : 
			putWidgetOnTop(subWidget->getGraphicsProxy());

			// Release all buttons from the main interface : 
			forceKeyRelease();
		}
	}

	void GlipViewWidget::subWidgetSelected(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		subWidgetSelected(subWidget);
	}

	void GlipViewWidget::showSubWidget(SubWidget* subWidget)
	{
		if(!subWidget->isVisible())
			subWidget->show();		// implement the Show (1st part)
		else
		{
			subWidget->resetPosition(false);
			subWidgetSelected(subWidget);	// implement the raise (2nd part)
		}
	}

	void GlipViewWidget::showSubWidget(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		showSubWidget(subWidget);
	}
	
	void GlipViewWidget::hideSubWidget(SubWidget* subWidget)
	{
		if(subWidgetsList.key(subWidget, -1)>=0 && subWidget->getQVGLParent()==this && subWidget->getGraphicsProxy()!=NULL)
		{
			// Lower the current subWidget : 
			putWidgetOnBottom(subWidget->getGraphicsProxy());

			// Raise the top bar : 
			putWidgetOnTop(topBar.getGraphicsProxy());
		}
	}

	void GlipViewWidget::hideSubWidget(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		hideSubWidget(subWidget);
	}

	void GlipViewWidget::subWidgetClosed(SubWidget* subWidget)
	{
		int idx = subWidgetsList.key(subWidget,-1);

		if(idx>=0)
		{
			// Remove this view from the list :
			subWidgetsList.remove(idx);

			// Try removing : 
			if(glScene!=NULL)
				glScene->removeItem(subWidget->getGraphicsProxy());
			subWidget->getGraphicsProxy()->setWidget(NULL);
			subWidget->getGraphicsProxy()->deleteLater();
			subWidget->setGraphicsProxy(NULL);

			// Disconnect : 
			subWidget->disconnect(this);
			subWidget->setQVGLParent(NULL);
		}
	}

	void GlipViewWidget::subWidgetClosed(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());
		
		subWidgetClosed(subWidget);
	}

	void GlipViewWidget::subWidgetDestroyed(void)
	{
		// Get the emitter : 
		SubWidget* subWidget = reinterpret_cast<SubWidget*>(QObject::sender());

		int idx = subWidgetsList.key(subWidget,-1);

		if(idx>=0)
			subWidgetsList.remove(idx);
	}

	void GlipViewWidget::nextSubWidget(void)
	{
		SubWidget* secondSubWidget = getSubWidget(1, true);
		
		if(secondSubWidget!=NULL)
			subWidgetSelected(secondSubWidget);
	}

	void GlipViewWidget::previousSubWidget(void)
	{
		SubWidget* bottom = getBottomSubWidget(true);

		if(bottom!=NULL)
			subWidgetSelected(bottom);
	}

	void GlipViewWidget::temporaryHideAllSubWidgets(bool enabled)
	{
		// Hide :
		if(enabled && temporaryHiddenSubWidgetsList.empty())
		{
			temporaryHiddenSubWidgetsList.clear();

			for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			{
				if(it.value()->getGraphicsProxy()!=NULL && it.value()->getGraphicsProxy()->isVisible())
				{
					temporaryHiddenSubWidgetsList.push_back(it.value());
					it.value()->getGraphicsProxy()->setVisible(false);
				}
			}
		}
		else if(!enabled) // Release :
		{
			for(QList<SubWidget*>::iterator it=temporaryHiddenSubWidgetsList.begin(); it!=temporaryHiddenSubWidgetsList.end(); it++)
			{
				if((*it)->getGraphicsProxy()!=NULL)
					(*it)->getGraphicsProxy()->setVisible(true);
			}
		
			temporaryHiddenSubWidgetsList.clear();
		}
	}

	void GlipViewWidget::toggleTemporaryHideAllSubWidgets(void)
	{
		temporaryHideAllSubWidgets(temporaryHiddenSubWidgetsList.empty());
	}

	void GlipViewWidget::hideAllSubWidgets(void)
	{
		for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
			it.value()->hide();
	}

	void GlipViewWidget::barSelected(TopBar* bar)
	{
		if(bar==&topBar)
		{
			// Change opacity of all other subWidgets :
			for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				it.value()->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityActiveBar);
			bottomBar.setWindowOpacity(opacityActiveBar);

			// Raise the bar :
			putWidgetOnTop(bottomBar.getGraphicsProxy());
			putWidgetOnTop(topBar.getGraphicsProxy());

			// Release all buttons from the main interface : 
			forceKeyRelease();
		}	
	}

	void GlipViewWidget::barSelected(BottomBar* bar)
	{
		if(bar==&bottomBar)
		{
			// Change opacity of all other subWidgets : 
			for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
				it.value()->setWindowOpacity(opacityIdleSubWidget);

			// Change the opacity of the bars : 
			topBar.setWindowOpacity(opacityActiveBar);
			bottomBar.setWindowOpacity(opacityActiveBar);

			// Raise the bar : 
			putWidgetOnTop(bottomBar.getGraphicsProxy());
			putWidgetOnTop(topBar.getGraphicsProxy());

			// Release all buttons from the main interface : 
			forceKeyRelease();
		}
	}

	void GlipViewWidget::processOpenInfosRequest(void)
	{
		addSubWidget(infosDialog);
	}

	void GlipViewWidget::processOpenSettingsInterfaceRequest(void)
	{
		addSubWidget(&settingsInterface);
	}

	void GlipViewWidget::applySettings(void)
	{
		if(glScene!=NULL)
		{
			setKeyboardSettings(settingsInterface.getSettings());
			glScene->setSceneSettings(settingsInterface.getSettings());
		}
	}

	View* GlipViewWidget::getCurrentView(void) const
	{
		QMap<int,View*>::const_iterator it=viewsList.find(currentViewIndex);
		return (it==viewsList.end()) ? NULL : it.value();
	}	

	ViewsTable* GlipViewWidget::getCurrentViewsTable(void)
	{
		QMap<int,ViewsTable*>::const_iterator it=viewsTablesList.find(currentViewsTableIndex);
		return (it==viewsTablesList.end()) ? NULL : it.value();
	}

	void GlipViewWidget::changeCurrentView(int targetID, const bool showNow, const bool goingUp)
	{
		if(viewsList.contains(targetID))
			currentViewIndex = targetID;
		else
		{
			currentViewIndex = -1;
			int 	closestIndex = -1,
				bestDistance = std::numeric_limits<int>::max(),
				bestSatisfyingDistance = std::numeric_limits<int>::max();
			for(QMap<int,View*>::iterator it=viewsList.begin(); it!=viewsList.end(); it++)
			{
				const float d = std::abs(it.key()-targetID);
				if(d<bestSatisfyingDistance && ((it.key()>=targetID && goingUp) || (it.key()<=targetID && !goingUp)))
				{
					currentViewsTableIndex = it.key();
					bestSatisfyingDistance = d;
				}

				if(d<bestDistance)
				{
					closestIndex = it.key();
					bestDistance = d;
				}				
			}
			if(currentViewIndex<0)
				currentViewIndex = closestIndex;
		}

		View* currentView = getCurrentView();

		if(currentView!=NULL && showNow)
		{
			// Hide current Table : 
			hideCurrentViewsTable();

			// Change title : 
			topBar.setTitle(*currentView);

			// Show : 
			if(glScene!=NULL)
				glScene->update();
		}
		else if(currentView==NULL)
			topBar.setTitle("(No View)");
	}

	void GlipViewWidget::hideCurrentView(void)
	{
		topBar.setTitle("(No View)");
		currentViewIndex = -1;
		if(glScene!=NULL)
			glScene->update();
	}

	void GlipViewWidget::changeCurrentViewsTable(int targetID, const bool goingUp)
	{
		// Hide previous table :
		hideCurrentViewsTable();

		if(viewsTablesList.contains(targetID))
			currentViewsTableIndex = targetID;
		else
		{
			currentViewsTableIndex = -1;
			int 	closestIndex = -1,
				bestDistance = std::numeric_limits<int>::max(),
				bestSatisfyingDistance = std::numeric_limits<int>::max();
			for(QMap<int,ViewsTable*>::iterator it=viewsTablesList.begin(); it!=viewsTablesList.end(); it++)
			{
				const float d = std::abs(it.key()-targetID);
				if(d<bestSatisfyingDistance && ((it.key()>=targetID && goingUp) || (it.key()<=targetID && !goingUp)))
				{
					currentViewsTableIndex = it.key();
					bestDistance = d;
				}
		
				if(d<bestDistance)
				{
					closestIndex = it.key();
					bestDistance = d;
				}
			}
			if(currentViewsTableIndex<0)
				currentViewsTableIndex = closestIndex;
		}

		ViewsTable* currentTable = getCurrentViewsTable();

		if(currentTable!=NULL)
		{
			// Change title : 
			topBar.setTitle(*currentTable);

			// Show : 
			currentTable->setVisible(true);
			if(glScene!=NULL)
				glScene->update();
		}
	}

	void GlipViewWidget::hideCurrentViewsTable(void)
	{
		ViewsTable* currentTable = getCurrentViewsTable();

		if(currentTable!=NULL)
		{	
			topBar.setTitle("(No View)");
			currentViewsTableIndex = -1;
			currentTable->setVisible(false);
		}
	}

	void GlipViewWidget::getSceneRatioScaling(const float& sceneRatio, float& xSceneScale, float& ySceneScale) const
	{
		xSceneScale = std::min(1.0f, 1.0f/sceneRatio);
		ySceneScale = std::min(1.0f, sceneRatio); 	// The minus sign set the orientation of the GL axis to be the same as Qt.
	}

	void GlipViewWidget::getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const
	{
		getSceneRatioScaling(getSceneRatio(), xSceneScale, ySceneScale);
	}

	float GlipViewWidget::getAdaptationScaling(const float& sceneRatio, const float& imageRatio) const
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

	float GlipViewWidget::getAdaptationScaling(const float& imageRatio) const
	{
		return getAdaptationScaling(getSceneRatio(), imageRatio);
	}

	void GlipViewWidget::toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative, const QRectF& rect) const
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

	void GlipViewWidget::toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const
	{
		toGlCoordinates(x, y, xGl, yGl, isRelative, sceneRect());
	}

	void GlipViewWidget::toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const QRectF& rect, const View* view) const
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
			 		x2 =  c * x + s * y,
					y2 = -s * x + c * y;

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
			 		x2 =  c * x + s * y,
					y2 = -s * x + c * y;

			// Image scaling : 
			float xImgScale, yImgScale;
			view->getAspectRatioScaling(xImgScale, yImgScale);

			xQuad = x2 / xImgScale;
			yQuad = y2 / yImgScale;
		}
	}

	void GlipViewWidget::toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const View* view) const
	{
		toQuadCoordinates(xGl, yGl, xQuad, yQuad, isRelative, sceneRect(), view);
	}

	void GlipViewWidget::toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, const View* view) const
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

	void GlipViewWidget::toFragmentCoordinates(const float& xQuad, const float& yQuad, float& xFrag, float& yFrag, bool isRelative, const View* view) const
	{
		if(view==NULL)
			view = getCurrentView();

		if(view==NULL || !view->isValid())
		{
			xFrag = 0.0f;
			yFrag = 0.0f;
		}
		else if(!isRelative)
		{
			xFrag = (xQuad + 1.0f)*view->getFormat().getWidth()/2.0f;
			yFrag = (yQuad + 1.0f)*view->getFormat().getHeight()/2.0f;
		}
		else
		{
			xFrag = xQuad*view->getFormat().getWidth()/2.0f;
			yFrag = yQuad*view->getFormat().getHeight()/2.0f;
		}
	}

	void GlipViewWidget::closeEvent(QCloseEvent *event)
	{
		for(QMap<int,SubWidget*>::iterator it=subWidgetsList.begin(); it!=subWidgetsList.end(); it++)
		{
			QCoreApplication::sendEvent(it.value(), event);
		
			// One subwidget refuses : 
			if(!event->isAccepted())
				return ;
		}
		
		QWidget::closeEvent(event);
	}

	float GlipViewWidget::getSceneRatio(void) const
	{
		QRectF rect = sceneRect();
		return rect.width() / rect.height();
	}

	QColor GlipViewWidget::getColorAt(int x, int y)
	{
		QColor c(0, 0, 0, 0);

		if(glScene!=NULL && x>=0 && x<glScene->width() && y>=0 && y<glScene->height())
		{
			unsigned char bgra[4];

			glReadBuffer(GL_BACK);

			// Subtle point here : the frame buffer is verticaly flipped!
			glReadPixels(x, contextWidget->height()-(y+1), 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, bgra);

			// Split : 
			c.setRed(bgra[2]);
			c.setGreen(bgra[1]);
			c.setBlue(bgra[0]);
			c.setAlpha(bgra[3]);
		}

		return c;
	}

	QString GlipViewWidget::getActionName(const ActionID& a)
	{
		switch(a)
		{
			case ActionUp : 				return "Up";
			case ActionDown : 				return "Down";
			case ActionLeft : 				return "Left";
			case ActionRight : 				return "Right";
			case ActionZoomIn : 				return "Zoom In";
			case ActionZoomOut : 				return "Zoom Out";
			case ActionRotationClockWise : 			return "Rotation (clockwise)";
			case ActionRotationCounterClockWise : 		return "Rotation (counter clockwise)";
			case ActionToggleFullscreen : 			return "Toggle Fullscreen";
			case ActionExitFullscreen : 			return "Exit Fullscreen";
			case ActionResetView : 				return "Reset View";
			case ActionPreviousView : 			return "Previous View";
			case ActionNextView : 				return "Next View";
			case ActionCloseView : 				return "Close View";
			case ActionCloseAllViews : 			return "Close All Views";
			case ActionCloseViewsTable : 			return "Close Views Table";
			case ActionCloseAllViewsTables : 		return "Close All Views Tables";
			case ActionMotionModifier : 			return "View Motion";
			case ActionRotationModifier : 			return "View Rotation";
			case ActionNextSubWidget : 			return "Next SubWidget";
			case ActionPreviousSubWidget : 			return "Previous SubWidget";
			case ActionToggleTemporaryHideAllSubWidgets : 	return "Toggle Hide All SubWigets";
			case ActionHideAllSubWidgets : 			return "Hide All SubWidgets";
			case ActionQuit : 				return "Quit";
			case NoAction : 				return "No Action";
			default : 
				return QString(tr("<Unknown ActionID : %1>").arg(a));
		}
	}

	void GlipViewWidget::addView(View* view)
	{
		static int c = 0;

		if(viewsList.key(view,-1)<0 && view->qvglParent==NULL)
		{
			viewsList[c] = view;
			c++;

			view->qvglParent = this;

			// Connect actions : 
			QObject::connect(view, SIGNAL(requireDisplay()),	this, SLOT(viewRequireDisplay()));
			QObject::connect(view, SIGNAL(updated()),		this, SLOT(viewUpdated()));
			QObject::connect(view, SIGNAL(closed()), 		this, SLOT(viewClosed()));
			QObject::connect(view, SIGNAL(destroyed()), 		this, SLOT(viewDestroyed()));

			// Update :
			emit viewAdded(view);
		}
	}

	void GlipViewWidget::addViewsTable(ViewsTable* viewsTable)
	{
		static int c = 0;

		if(viewsTablesList.key(viewsTable,-1)<0)
		{
			for(QMap<View*, Vignette*>::iterator it=viewsTable->begin(); it!=viewsTable->end(); it++)
				addView(it.key());

			if(glScene!=NULL)
			{
				glScene->addItem(viewsTable);
				//forceItemOrdering();
			}
			viewsTablesList[c] = viewsTable;
			c++;

			QObject::connect(viewsTable, SIGNAL(requireDisplay()),		this, SLOT(viewsTableRequireDisplay()));
			QObject::connect(viewsTable, SIGNAL(showView(View*)), 		this, SLOT(viewRequireDisplay(View*)));
			QObject::connect(viewsTable, SIGNAL(viewSelection(View*)),	this, SLOT(viewChangeSelection(View*)));
			QObject::connect(viewsTable, SIGNAL(closed()),			this, SLOT(viewsTableClosed()));
			QObject::connect(viewsTable, SIGNAL(destroyed()),		this, SLOT(viewsTableDestroyed()));

			// Update :
			emit viewsTableAdded(viewsTable);
		}
	}

	void GlipViewWidget::addSubWidget(SubWidget* subWidget)
	{
		static int c = 0;

		if(glScene!=NULL && subWidget!=NULL && subWidgetsList.key(subWidget,-1)<0 && subWidget->getQVGLParent()==NULL)
		{
			QGraphicsProxyWidget* proxy = glScene->addWidget(subWidget);
			subWidget->setGraphicsProxy(proxy);
			//forceItemOrdering();
	
			// Connect : 
			QObject::connect(subWidget, SIGNAL(selected(SubWidget*)), 	this, SLOT(subWidgetSelected(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(showRequest(SubWidget*)),	this, SLOT(showSubWidget(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(hideRequest(SubWidget*)),	this, SLOT(hideSubWidget(SubWidget*)));
			QObject::connect(subWidget, SIGNAL(closed(void)),		this, SLOT(subWidgetClosed(void)));
			QObject::connect(subWidget, SIGNAL(destroyed()),		this, SLOT(subWidgetDestroyed()));

			// Save link : 
			subWidget->setQVGLParent(this);
			subWidgetsList[c] = subWidget;
			c++;

			// Move but show only if it is currently visible : 
			subWidget->move(0, topBar.height());
			subWidget->show();

			// Update : 
			emit subWidgetAdded(subWidget);
		}
		else if(subWidget!=NULL && subWidgetsList.key(subWidget,-1)>=0 && subWidget->getQVGLParent()==this)
			subWidgetSelected(subWidget);
	}

	void GlipViewWidget::processAction(ActionID action, bool takenBack)
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

					currentView->zoom(xCenter, yCenter, settingsInterface.getSettings().zoomFactor);
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

					currentView->zoom(xCenter, yCenter, 1.0f/settingsInterface.getSettings().zoomFactor);
				}
				break;
			case ActionRotationClockWise :
				if(currentView!=NULL) currentView->rotate(-settingsInterface.getSettings().rotationStep);//-0.17453f
				break;
			case ActionRotationCounterClockWise :
				if(currentView!=NULL) currentView->rotate(+settingsInterface.getSettings().rotationStep);//0.17453f
				break;
			case ActionToggleFullscreen :
				if(!isFullScreen())
				{
					showFullScreen();
					break;
				}
				// else : 
			case ActionExitFullscreen :
				showNormal();
				break;
			case ActionResetView :
				if(currentView!=NULL) currentView->reset();
				break;
			case ActionPreviousView :
				changeCurrentView(currentViewIndex - 1, true, false); // Going down
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
				if(currentViewsTable!=NULL && currentViewsTable!=mainViewsTable) currentViewsTable->close();
				break;
			case ActionCloseAllViewsTables : 
				closeAllViewsTables();
				break;
			case ActionMotionModifier :
				if(!takenBack)
					setMouseFunctionMode(MouseState::ModeMotion);
				else
					setMouseFunctionMode(MouseState::ModeCollection);
				break;
			case ActionRotationModifier :
				if(!takenBack)
					setMouseFunctionMode(MouseState::ModeRotation);
				else
					setMouseFunctionMode(MouseState::ModeCollection);
				break;
			case ActionNextSubWidget :
				nextSubWidget();
				break;
			case ActionPreviousSubWidget :
				previousSubWidget();
				break;
			case ActionToggleTemporaryHideAllSubWidgets :
				toggleTemporaryHideAllSubWidgets();
				break;
			case ActionHideAllSubWidgets :
				hideAllSubWidgets();
				break;
			case ActionQuit :
				close();
				break;
			case NoAction :
				break;
			default : 
				throw Exception("GlipViewWidget::processAction - Unknown action (code : " + toString(action) + ").", __FILE__, __LINE__);
		}
	}

