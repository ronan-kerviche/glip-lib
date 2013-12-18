#ifndef __GLIPSTUDIO_TITLE_BAR__
#define __GLIPSTUDIO_TITLE_BAR__

	#include "GLIPLib.hpp"
	#include <QtGlobal>
	#include <QX11Info>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	// Adapted from http://qt-project.org/faq/answer/how_can_i_handle_events_in_the_titlebar_and_change_its_color_etc
	// And http://qt.developpez.com/tutoriels/braindeadbzh/customwindow/

	class TitleBar : public QWidget
	{
		Q_OBJECT
	
		private :
			QToolButton 	minimize,
					maximize,
					close;
			bool 		maximized;
			QPoint 		startPos,
					clickPos;
			QLabel		titleLabel;
			QHBoxLayout	layout;

		private slots :
			void showSmall(void);
			void showMaxRestore(void);

		protected :
			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void mouseDoubleClickEvent(QMouseEvent * event);

		public :
			TitleBar(QWidget *parent);
			~TitleBar(void);

			bool isMaximized(void) const;
			void setWindowTitle(QString title);
	};
     
	class WindowFrame : public QFrame
	{
		Q_OBJECT

		private :
			TitleBar 	titleBarWidget;
			QWidget*	contentWidget;
			QPoint 		oldPos;
			bool 		mouseDown,
					moveLeft,
					moveRight,
					moveUp,
					moveDown;
			QVBoxLayout	layout;
			QPixmap		*windowCache;

		protected :
			void paintEvent(QPaintEvent *event);
			void resizeEvent(QResizeEvent *event);

			void mousePressEvent(QMouseEvent *event);
			void mouseMoveEvent(QMouseEvent *event);
			void mouseReleaseEvent(QMouseEvent *event);

		public :
			WindowFrame(QWidget* _contentWidget);
			~WindowFrame(void);

			const QWidget* content(void) const;
			void removeContent(void);
			TitleBar& titleBar(void);

		public slots :
			void close(void);

		signals :
			void closeSignal(void);
	};

#endif

