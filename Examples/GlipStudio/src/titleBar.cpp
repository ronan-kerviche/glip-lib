#include "titleBar.hpp"

// Namespace : 
	using namespace Glip;

// Static styles : 
	const char* buttonStyle	= 	"QToolButton 		{ background:%2; border:4px solid %1; border-radius:8px; }"
					"QToolButton:hover 	{ background:%3; border:4px solid %1; border-radius:8px; }"
					"QToolButton:pressed 	{ background:%4; border:5px solid %1; border-radius:8px; }"
					"QToolButton:!enabled 	{ background:%5; border:5px solid %1; border-radius:8px; }";

// TitleBar :
	TitleBar::TitleBar(QWidget *parent)
	 : 	QWidget(parent),
		layout(this),
		minimize(this),
		maximize(this),
		close(this),
		maximized(false),
		resizable(true)
	{
		// Don't let this widget inherit the parent's backround color
		setAutoFillBackground(true);

		const QColor 	backgroundColor = QColor("#333333"),		//,palette().color(QWidget::backgroundRole())
				minimizeColor	= QColor(80, 	80, 	80),
				maximizeColor	= QColor(128, 	128, 	128),
				closeColor	= QColor(192, 	192, 	192);

		const int 	w		= 32,
				h		= 16;

		// Buttons : 
		minimize.setMinimumSize(w, h);
		minimize.setMaximumSize(w, h);
		minimize.setStyleSheet( tr(buttonStyle).arg(backgroundColor.name()).arg(minimizeColor.name()).arg(minimizeColor.lighter(200).name()).arg(minimizeColor.lighter(80).name()).arg(backgroundColor.lighter(50).name()) );
		minimize.setToolTip("Minimize");

		maximize.setMinimumSize(w, h);
		maximize.setMaximumSize(w, h);
		maximize.setStyleSheet( tr(buttonStyle).arg(backgroundColor.name()).arg(maximizeColor.name()).arg(maximizeColor.lighter(200).name()).arg(maximizeColor.lighter(80).name()).arg(backgroundColor.lighter(50).name()) );
		maximize.setToolTip("Maximize");

		close.setMinimumSize(w, h);
		close.setMaximumSize(w, h);
		close.setStyleSheet( tr(buttonStyle).arg(backgroundColor.name()).arg(closeColor.name()).arg(closeColor.lighter(200).name()).arg(closeColor.lighter(80).name()).arg(backgroundColor.lighter(50).name()) );
		close.setToolTip("Close");

		titleLabel.setMaximumWidth(140);

		layout.addWidget(&blankLabel);
		layout.addWidget(&titleLabel);
		layout.addWidget(&minimize);
		layout.addWidget(&maximize);
		layout.addWidget(&close);
		layout.addSpacing(4);

		titleLabel.setOpenExternalLinks(false);

		layout.setMargin(1);
		layout.setSpacing(0);

		connect(&close, 	SIGNAL(clicked()), parentWidget(), 	SIGNAL(closeSignal()) );
		connect(&minimize, 	SIGNAL(clicked()), this, 		SLOT(showSmall()) );
		connect(&maximize, 	SIGNAL(clicked()), this, 		SLOT(showMaxRestore()) );
	}

	TitleBar::~TitleBar(void)
	{ }

	void TitleBar::showSmall(void)
	{
		parentWidget()->showMinimized();
	}

	void TitleBar::showMaxRestore(void)
	{
		if(maximized)
		{
			parentWidget()->showNormal();
			maximized = false;
		}
		else
		{
			parentWidget()->showMaximized();
			maximized = true;
		}
	}

	void TitleBar::mousePressEvent(QMouseEvent* event)
	{
		if(event->buttons() & Qt::LeftButton)
		{
			startPos = event->globalPos();
			clickPos = mapToParent(event->pos());
		}
	}

	void TitleBar::mouseMoveEvent(QMouseEvent* event)
	{
		if((event->buttons() & Qt::LeftButton) && parentWidget()!=NULL)
		{
			if(maximized)
				showMaxRestore();

			parentWidget()->move(event->globalPos() - clickPos);
		}
	}

	void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
	{
		if(event->buttons()==Qt::LeftButton)
			showMaxRestore();
		else
			event->accept();
	}

	bool TitleBar::isMaximized(void) const
	{
		return maximized;
	}

	void TitleBar::setWindowTitle(QString title)
	{
		// Do not put rich text!
		title.remove(QRegExp("<[^>]*>"));

		titleLabel.setText(title);
		titleLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		parentWidget()->setWindowTitle(title);
	}

	bool TitleBar::isResizable(void) const
	{
		return resizable;
	}

	bool TitleBar::isMinimizable(void) const
	{
		return minimize.isEnabled();
	}

	bool TitleBar::isMaximizable(void) const
	{
		return maximize.isEnabled();
	}

	bool TitleBar::isClosable(void) const
	{
		return close.isEnabled();
	}

	void TitleBar::setResizeable(bool _resizeable)
	{
		resizable = _resizeable;
	}

	void TitleBar::setMinimizable(bool minimizable)
	{
		minimize.setEnabled(minimizable);
	}

	void TitleBar::setMaximizable(bool maximizable)
	{
		maximize.setEnabled(maximizable);
	}

	void TitleBar::setClosable(bool closable)
	{
		close.setEnabled(closable);
	}

// WindowFrame :
	WindowFrame::WindowFrame(QWidget* _contentWidget)
	 :	contentWidget(_contentWidget),
		layout(this),
		titleBarWidget(this),
		mouseDown(false),
		moveLeft(false),
		moveRight(false),
		moveUp(false),
		moveDown(false),
		windowCache(NULL)
	{
		setFrameShape(Panel);

		// Make this a borderless window which can't be resized or moved via the window system :
		setWindowFlags(Qt::FramelessWindowHint);
		setMouseTracking(true);

		layout.addWidget(&titleBarWidget);
		layout.addWidget(contentWidget);
		layout.setMargin(2);
		layout.setSpacing(0);
	}

	WindowFrame::~WindowFrame(void)
	{
		if(windowCache!=NULL)
			delete windowCache;
	}
	
	void WindowFrame::paintEvent(QPaintEvent *event)
	{
		if(windowCache!=NULL)
		{
			QPainter painter(this);

			painter.drawPixmap(0, 0, *windowCache);

			#if QT_VERSION >= 0x040500
				if(!testAttribute(Qt::WA_TranslucentBackground))
					setMask(windowCache->mask());
			#else
				setMask(windowCache->mask());
			#endif
		}
	}

	void WindowFrame::resizeEvent(QResizeEvent *event)
	{
		delete windowCache;

		windowCache = new QPixmap(size());

		windowCache->fill(Qt::transparent);

		QPainter painter(windowCache);

		const QColor backgroundColor = palette().color(QWidget::backgroundRole());

		// Window's background :
		const int 	border 		= 16,
				titleLength 	= 256;
		QPolygon background;

		background 	<< QPoint(	0,					border)
				<< QPoint(	width() - titleLength,			border)
				<< QPoint(	width() - titleLength + border,		0)
				<< QPoint(	width(),				0)
				<< QPoint(	width(), 				height())
				<< QPoint(	0, 					height());

		painter.setPen( QPen(backgroundColor) );
		painter.setBrush( QBrush(backgroundColor) );

		painter.drawPolygon(background);

		// Outline : 
		const int delta = 2;
		QPolygon outline;

		outline 	<< QPoint(	delta,						border + delta)
				<< QPoint(	width() - titleLength + delta/1.4142f,		border + delta)
				<< QPoint(	width() - titleLength + border + delta/1.4142f,	delta)
				<< QPoint(	width() - delta - 1,				delta)
				<< QPoint(	width() - delta - 1, 				height() - delta - 1)
				<< QPoint(	delta, 						height() - delta - 1);
	
		painter.setBackgroundMode(Qt::TransparentMode);
		painter.setPen( QPen(backgroundColor.lighter(300)) );
		
		painter.drawPolygon(outline);
	}
	
	void WindowFrame::mousePressEvent(QMouseEvent *event)
	{
		oldPos 		= event->pos();
		mouseDown 	= event->button() == Qt::LeftButton;		
	}

	void WindowFrame::mouseMoveEvent(QMouseEvent *event)
	{
		int x = event->x();
		int y = event->y();

		if(mouseDown && titleBar().isResizable())
		{
			int dx = x - oldPos.x();
			int dy = y - oldPos.y();

			QRect g = geometry();

			if(moveLeft)
				g.setLeft(g.left() + dx);
			if(moveRight)
				g.setRight(g.right() + dx);
			if(moveDown)
				g.setBottom(g.bottom() + dy);
			if(moveUp)
				g.setTop(g.top() + dy);

			if(g.width()>=minimumWidth() && g.width()<maximumWidth() && g.height()>=minimumHeight() && g.width()<maximumHeight())
				setGeometry(g);

			oldPos = QPoint(!moveLeft ? event->x() : oldPos.x(), event->y());
		}
		else if(titleBar().isResizable())
		{
			const int border = 1;

			QRect 	r		= rect();
				moveLeft	= qAbs(x - r.left()) 	<= border;
				moveRight 	= qAbs(x - r.right()) 	<= border;
				moveDown 	= qAbs(y - r.bottom()) 	<= border;
				moveUp 		= qAbs(y - r.top()) 	<= border;
			bool 	horizontal 	= moveLeft | moveRight;

			if(horizontal && moveDown)
			{
				if(moveLeft)
					setCursor(Qt::SizeBDiagCursor);
				else
					setCursor(Qt::SizeFDiagCursor);
			} 
			else if(horizontal)
				setCursor(Qt::SizeHorCursor);
			else if(moveDown)
				setCursor(Qt::SizeVerCursor);
			else
				setCursor(Qt::ArrowCursor);
		}
	}

	void WindowFrame::mouseReleaseEvent(QMouseEvent *event)
	{
		mouseDown = false;
	}

	const QWidget* WindowFrame::content(void) const
	{
		return contentWidget;
	}

	void WindowFrame::removeContent(void)
	{
		if(contentWidget!=NULL)
		{
			contentWidget->setParent(NULL);
			layout.removeWidget(contentWidget);
			contentWidget = NULL;
		}
	}

	TitleBar& WindowFrame::titleBar(void)
	{
		return titleBarWidget;
	}

	void WindowFrame::setResizeable(bool resizeable)
	{
		titleBarWidget.setResizeable(resizeable);
	}

	void WindowFrame::setMinimizable(bool minimizable)
	{
		titleBarWidget.setMinimizable(minimizable);
	}

	void WindowFrame::setMaximizable(bool setMaximizable)
	{
		titleBarWidget.setMaximizable(setMaximizable);
	}

	void WindowFrame::setClosable(bool closable)
	{
		titleBarWidget.setClosable(closable);
	}
	
	void WindowFrame::close(void)
	{
		removeContent();

		// Continue : 
		QFrame::close();
	}

// Window : 
	Window::Window(QWidget* parent, bool dialogMode)
	 : 	QWidget(parent),
		frame(this)
	{
		if(dialogMode)
		{
			frame.setResizeable(false);
			frame.setMinimizable(false);
			frame.setMaximizable(false);
			frame.setClosable(false);
		}

		connect(&frame, SIGNAL(closeSignal()), this, SLOT(hide()));
	}

	Window::~Window(void)
	{
		frame.setParent(NULL);
		frame.removeContent();
	}

	void Window::show(void)
	{
		frame.show();
		frame.raise();
	}

	void Window::hide(void)
	{
		frame.hide();
	}

	void Window::close(void)
	{
		frame.close();
		QWidget::close();
	}

