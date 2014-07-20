/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GLSceneWidget.hpp                                                                         */
/*     Original Date : May 26th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for visualization with OpenGL.                                               */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
	USAGE :

	WARNINGS :
		- The texture must be loaded and mirrored along the Z direction to appear correctly.
**/

#ifndef __GLIPLIB_GL_SCENE_WIDGET__
#define __GLIPLIB_GL_SCENE_WIDGET__

// Includes :
	#include <cmath>
	#include "GLIPLib.hpp"
	#include <QList>
	#include <QWidget>
	#include <QGLWidget>
	#include <QBoxLayout>
	#include <QHBoxLayout>
	#include <QVBoxLayout>
	#include <QKeyEvent>
	#include <QMessageBox>
	#include <QLabel>
	#include <QToolButton>
	#include <QToolBar>
	#include <QGraphicsScene>
	#include <QGraphicsView>
	#include <QGraphicsSceneMouseEvent>
	#include <QGraphicsProxyWidget>
	#include <QMenu>
	#include <QMenuBar>
	#include <QSignalMapper>

	

namespace QVGL
{
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Prototypes : 
	class View;
	class SubWidget;
	class ContextWidget;
	class KeyboardState;
	class SceneWidget;
	class SceneViewWidget;
	class MainWidget;

	enum ActionID
	{
		ActionUp,
		ActionDown,
		ActionLeft,
		ActionRight,
		ActionZoomIn,
		ActionZoomOut,
		ActionRotationClockWise,
		ActionRotationCounterClockWise,
		ActionToggleFullscreen,
		ActionExitFullscreen,
		ActionResetView,
		ActionPreviousView,
		ActionNextView,
		ActionCloseView,
		ActionCloseAllViews,
		ActionMotionModifier,
		ActionRotationModifier,
		ActionNextSubWidget,
		ActionPreviousSubWidget,
		ActionTemporaryHideAllSubWidgets,
		ActionTemporaryUnhideAllSubWidgets,
		ActionHideAllSubWidgets,
		// Add new actions before this line
		NumActions,
		NoAction
	};

	class View : public QObject	// One image, plus data
	{
		Q_OBJECT 

		private :
			HdlTexture	*texture;
			QString		name;
			MainWidget	*qvglParent;
			float		angle,			// in Radians.
					viewCenter[2],
					homothecyCenter[2],
					homothecyScale;
			bool 		flipUpDown,		// upside down
					flipLeftRight;		// mirrored

			void prepareToDraw(void);

			friend class SceneWidget;
			friend class MainWidget;

		public : 
			View(HdlTexture* _texture, const QString& _name);
			~View(void);

			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			const QString& getName(void) const;
			void setName(const QString& newName);
			float getAngle(void) const;
			void setAngle(const float& a);
			void rotate(const float& a);
			void getViewCenter(float& x, float& y) const;
			void setViewCenter(const float& x, const float& y);
			void move(const float& x, const float& y);
			bool isMirrored(void) const;
			void setMirror(bool enabled);
			bool isUpsideDown(void) const;
			void setUpsideDown(bool enabled);
			void zoom(const float& xCenter, const float& yCenter, const float& factor);
			void reset(void);
			void show(void);
			void close(void);
			bool isClosed(void);

			float getImageRatio(void) const;					// width / height
			void getAspectRatioScaling(float& xImgScale, float& yImgScale) const;	// scaling to apply on the standard quad to have the same aspect ratio as the image.

			// Infos : 
			QString getSizeString(void) const;

		signals :
			void updated(void);
			void requireDisplay(void);
			void nameChanged(void);
			void closed(void);
	};

	class SubWidget : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout		layout;
			QWidget			titleWidget;	// For fixed size;
			QHBoxLayout		titleBar;
			QLabel			titleLabel;
			QToolButton		hideButton;
			QWidget			*widget;
			MainWidget		*qvglParent;
			QGraphicsProxyWidget 	*graphicsProxy;
			bool 			visible,
						motionActive,
						resizeActive,
						resizeHorizontalLock,
						resizeVerticalLock;
			QPoint 			offset;

			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void mouseReleaseEvent(QMouseEvent* event);

			void addChild(QObject* pObject);
			void removeChild(QObject* pObject);
			void childEvent(QChildEvent* e);
			bool eventFilter(QObject* target, QEvent* e);

			friend class SceneViewWidget;
			friend class MainWidget;

		private slots :
			void temporaryHide(bool enabled);

		public : 
			SubWidget(void);
			~SubWidget(void);

			void setInnerWidget(QWidget* _widget);
			QWidget* innerWidget(void);
			QString getTitle(void);
			void setTitle(QString title);
			MainWidget* getQVGLParent(void);
			bool shoudBeVisible(void) const;

		public slots :
			// Re-implement some of the QWidget functions : 
			void show(void);
			void hide(void);

		signals  :
			void titleChanged(void);
			void selected(SubWidget*);
			void showRequest(SubWidget*);
			void hideRequest(SubWidget*);
	};
		
		class PositionColorInfoMini : public QWidget
		{
			Q_OBJECT

			private : 
				QHBoxLayout		bar;
				QLabel			positionLabel,
							colorBox;

			public : 
				PositionColorInfoMini(void);
				~PositionColorInfoMini(void);

				void setWindowOpacity(qreal level);

			public slots : 
				void updatePosition(const QPointF& pos = QPointF());
				void updateColor(const QColor& color = Qt::black);
		};

		class TopBar : public QWidget
		{
			Q_OBJECT

			private : 
				static TopBar*			singleton;
				QGraphicsProxyWidget 		*graphicsProxy;
				QHBoxLayout			bar;
				QMenuBar			menuBar;
				QMenu				mainMenu,
								viewsMenu,
								widgetsMenu;
				QAction				temporaryHideAllSubWidgetsAction,
								hideAllSubWidgetsAction;
				QLabel				titleLabel;
				PositionColorInfoMini	positionColorInfo;
				QSignalMapper			viewsSignalMapper,
								widgetsSignalMapper;

				void mousePressEvent(QMouseEvent* event);

				friend class SceneWidget;
				friend class MainWidget;
				
			private slots : 
				void stretch(const QRectF& rect);
				void castViewPointer(QObject* ptr);
				void castSubWidgetPointer(QObject* ptr);
				void sendSelectedSignal(void);

			public :
				TopBar(void);
				~TopBar(void);

				void setTitle(void);
				void setTitle(QString title);
				void setTitle(const View& view);
				void updateViewsList(const QList<View*>& viewsList);
				void updateSubWidgetsList(const QList<SubWidget*>& subWidgetsList);
				void updatePositionAndColor(const QPointF& pos, const QColor& color);
				void setWindowOpacity(qreal level);

				static int getHeight(void); 
				

			signals : 
				void changeViewRequest(View* targetView);
				void showSubWidgetRequest(SubWidget* targetWidget);
				void selected(TopBar* ptr);
				void temporaryHideAllSubWidgets(void);
				void hideAllSubWidgets(void);
		};
	
		class BottomBar : public QWidget
		{
			Q_OBJECT
			
			private : 
				QGraphicsProxyWidget 	*graphicsProxy;
				QHBoxLayout		bar;
				QToolBar		toolBar;
				
				void mousePressEvent(QMouseEvent* event);

				friend class SceneWidget;
				friend class MainWidget;

			private slots : 
				void stretch(const QRectF& rect);

			public :
				BottomBar(void);
				~BottomBar(void);

			signals : 
				void selected(BottomBar*);
		};

	class ContextWidget : public QGLWidget
	{
		private :
			bool	glipOwnership;
			float	clearColorRed,
				clearColorGreen,
				clearColorBlue;

			void initializeGL(void);
			void resizeGL(int width, int height);
			
		public : 
			ContextWidget(QGLContext* ctx, QWidget* parent=NULL);
			~ContextWidget(void);
	};

	class KeyboardState : public QObject
	{
		Q_OBJECT

		private : 
			QMap<QKeySequence, ActionID> 	keysActionsAssociations;
			bool					takeBackEnabled[NumActions];
			bool					actionPressed[NumActions];
	
		protected :
			friend class SceneWidget;

			void keyPressed(QKeyEvent* event);	
			void keyReleased(QKeyEvent* event);

		public : 
			KeyboardState(void);
			~KeyboardState(void);

			ActionID getActionAssociatedToKey(const QKeySequence& keySequence) const;
			ActionID getActionAssociatedToKey(const QKeyEvent* event) const;
			QKeySequence getKeysAssociatedToAction(const ActionID& a);
			bool isActionTakeBackEnabled(const ActionID& a) const;
			void setTakeBack(const ActionID& a, bool enabled);
			void setActionKeySequence(const ActionID& a, const QKeySequence& keySequence, bool enableTakeBack=false);
			void resetActionsKeySequences(void);

		signals : 
			void actionReceived(ActionID, bool takenBack=false);
	};

	class MouseState : public QObject
	{
		Q_OBJECT

		public : 
			enum BasisID
			{
				PixelBasis		= 0,
				GlBasis			= 1,
				QuadBasis		= 2,
				ImageBasis		= 3,
				PixelRelativeBasis	= 4,	// "Relative" correspond to displacements (no offset calculation needed)
				GlRelativeBasis		= 5,
				QuadRelativeBasis	= 6,
				ImageRelativeBasis	= 7,
				NumBasis		= 8
			};

			enum VectorID
			{
				VectorLastLeftClick			= 0 * NumBasis,
				VectorLastLeftClickGl			= VectorLastLeftClick + GlBasis,
				VectorLastLeftClickQuad			= VectorLastLeftClick + QuadBasis,
				VectorLastLeftClickImage		= VectorLastLeftClick + ImageBasis,

				VectorLastLeftPosition			= 1 * NumBasis,
				VectorLastLeftPositionGl		= VectorLastLeftPosition + GlBasis,
				VectorLastLeftPositionQuad		= VectorLastLeftPosition + QuadBasis,
				VectorLastLeftPositionImage		= VectorLastLeftPosition + ImageBasis,

				VectorLastLeftShift			= 2 * NumBasis + PixelRelativeBasis,	// Will force the following to relative
				VectorLastLeftShiftGl			= VectorLastLeftShift + GlBasis,
				VectorLastLeftShiftQuad			= VectorLastLeftShift + QuadBasis,
				VectorLastLeftShiftImage		= VectorLastLeftShift + ImageBasis,

				VectorLastLeftRelease			= 3 * NumBasis,
				VectorLastLeftReleaseGl			= VectorLastLeftRelease + GlBasis,
				VectorLastLeftReleaseQuad		= VectorLastLeftRelease + QuadBasis,
				VectorLastLeftReleaseImage		= VectorLastLeftRelease + ImageBasis,

				VectorLastLeftCompletedVector		= 4 * NumBasis,
				VectorLastLeftCompletedVectorGl		= VectorLastLeftCompletedVector + GlBasis,
				VectorLastLeftCompletedVectorQuad	= VectorLastLeftCompletedVector + QuadBasis,
				VectorLastLeftCompletedVectorImage	= VectorLastLeftCompletedVector + ImageBasis,

				VectorLastRightClick			= 5 * NumBasis,
				VectorLastRightClickGl			= VectorLastRightClick + GlBasis,
				VectorLastRightClickQuad		= VectorLastRightClick + QuadBasis,
				VectorLastRightClickImage		= VectorLastRightClick + ImageBasis,

				VectorLastRightPosition			= 6 * NumBasis,
				VectorLastRightPositionGl		= VectorLastRightPosition + GlBasis,
				VectorLastRightPositionQuad		= VectorLastRightPosition + QuadBasis,
				VectorLastRightPositionImage		= VectorLastRightPosition + ImageBasis,

				VectorLastRightShift			= 7 * NumBasis + PixelRelativeBasis,	// Will force the following to relative
				VectorLastRightShiftGl			= VectorLastRightShift + GlBasis,
				VectorLastRightShiftQuad		= VectorLastRightShift + QuadBasis,
				VectorLastRightShiftImage		= VectorLastRightShift + ImageBasis,

				VectorLastRightRelease			= 8 * NumBasis,
				VectorLastRightReleaseGl		= VectorLastRightRelease + GlBasis,
				VectorLastRightReleaseQuad		= VectorLastRightRelease + QuadBasis,
				VectorLastRightReleaseImage		= VectorLastRightRelease + ImageBasis,

				VectorLastRightCompletedVector		= 9 * NumBasis,
				VectorLastRightCompletedVectorGl	= VectorLastRightCompletedVector + GlBasis,
				VectorLastRightCompletedVectorQuad	= VectorLastRightCompletedVector + QuadBasis,
				VectorLastRightCompletedVectorImage	= VectorLastRightCompletedVector + ImageBasis,

				VectorLastWheelUp			= 10 * NumBasis,
				VectorLastWheelUpGl			= VectorLastWheelUp + GlBasis,
				VectorLastWheelUpQuad			= VectorLastWheelUp + QuadBasis,
				VectorLastWheelUpImage			= VectorLastWheelUp + ImageBasis,

				VectorLastWheelDown			= 11 * NumBasis,
				VectorLastWheelDownGl			= VectorLastWheelDown + GlBasis,
				VectorLastWheelDownQuad			= VectorLastWheelDown + QuadBasis,
				VectorLastWheelDownImage		= VectorLastWheelDown + ImageBasis,
		
				InvalidVectorID				= 65535
				// ALSO UPDATE VALIDATE
			};

			enum ColorID
			{
				ColorUnderLastLeftClick		= 0 * NumBasis,
				ColorUnderLastLeftPosition	= 1 * NumBasis,
				ColorUnderLastLeftRelease	= 3 * NumBasis,
				ColorUnderLastRightClick	= 5 * NumBasis,
				ColorUnderLastRightPosition	= 6 * NumBasis,
				ColorUnderLastRightRelease	= 8 * NumBasis,

				InvalidColorID			= 65535
				// ALSO UPDATE VALIDATE
			};

			enum DataStatus
			{
				NotModified,
				RequireUpdate,
				Modified
			};

			enum FunctionMode
			{
				ModeMotion,
				ModeCollection
			};

		private : 
			FunctionMode					functionMode;
			QMap<VectorID, QPair<DataStatus, QPointF> >	vectors;
			QMap<ColorID,  QPair<DataStatus, QColor> >	colors;
			QList<VectorID>					vectorIDs;
			QList<ColorID>					colorIDs;
			float						wheelDelta;
				
		protected :
			friend class SceneWidget;
			friend class MainWidget;

			const QPointF& invisibleGetVector(const VectorID& id) const;
			const QColor& invisibleGetColor(const ColorID& id) const;
			bool doesVectorRequireUpdate(const VectorID& id) const;
			bool doesColorRequirepdate(const ColorID& id) const;
			void setVector(const VectorID& id, const QPointF& v, const bool requireUpdate=false);
			void setColor(const ColorID& id, const QColor& c);
			void processEvent(QGraphicsSceneWheelEvent* event);
			void processEvent(QGraphicsSceneMouseEvent* event, const bool clicked, const bool moved, const bool released);
			void updateProcessCompleted(void);

		public : 
			MouseState(void);
			~MouseState(void);

			void clear(void);
			const QList<VectorID>& getVectorIDs(void) const;
			const QList<ColorID>& getColorIDs(void) const;
			bool isVectorModified(const VectorID& id) const;
			bool isColorModified(const ColorID& id) const;
			const QPointF& getVector(const VectorID& id);
			const QColor& getColor(const ColorID& id);
			bool isWheelDeltaModified(void) const;
			float getWheelDelta(void);
			const FunctionMode& getFunctionMode(void) const;
			void setFunctionMode(const FunctionMode& m);

			static VectorID validate(const VectorID& vID);
			static ColorID validate(const ColorID& cID);
			static VectorID getPixelVectorID(const VectorID& vID);
			static BasisID getVectorBasis(const VectorID& vID);
			static ColorID getCorrespondingColorID(const VectorID& cID);
			static bool isBasisRelative(const BasisID& bID);

		signals : 
			void requestExternalUpdate(void);
			void mustSetMouseCursor(Qt::CursorShape cursorShape);
			void updated(void);
	};

	class SceneWidget : public QGraphicsScene
	{
		Q_OBJECT

		private :
			// Data : 
			MainWidget		*qvglParent;
			GeometryInstance	*quad;
			HdlProgram		*shaderProgram;
			TopBar			*topBar;
			BottomBar		*bottomBar;

			// Qt events : 
			void drawBackground(QPainter* painter, const QRectF& rect);
			void keyPressEvent(QKeyEvent* event);
			void keyReleaseEvent(QKeyEvent* event);
			void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
			void wheelEvent(QGraphicsSceneWheelEvent* event);
			void mousePressEvent(QGraphicsSceneMouseEvent* event);
			void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
			void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);	

		public : 
			SceneWidget(MainWidget* _Parent, TopBar* _topBar, BottomBar* _bottomBar);
			~SceneWidget(void);

			void addSubWidget(SubWidget* subWidget);
			void putItemOnTop(QGraphicsProxyWidget*);
	};

	class SceneViewWidget : public QGraphicsView
	{
		Q_OBJECT

		private : 
			// Data :
			ContextWidget			*contextWidget;
			SceneWidget 			*sceneWidget;
			MainWidget			*qvglParent;

			// Functions :
			void resizeEvent(QResizeEvent *event);

		public : 
			SceneViewWidget(MainWidget* _Parent, TopBar* _topBar, BottomBar* _bottomBar);
			~SceneViewWidget(void);

			void addSubWidget(SubWidget* subWidget);
			SubWidget* getUppermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			SubWidget* getLowermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			void orderSubWidgetsList(QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			void putItemOnTop(QGraphicsProxyWidget* graphicsProxy);				// Both subWidgets and bars.
			void putItemOnBottom(QGraphicsProxyWidget* graphicsProxy);			// Only subWidgets (but accept bars also).
			void makeGLContextAvailable(void);
			void getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);
			void getColorAt(int x, int y, QColor& c);
			void update(void);

		// Deprecated : 
		//signals :
			//void requireContainerCatch(void);
	};
	
	class MainWidget : public QWidget
	{
		Q_OBJECT

		private :
			QBoxLayout 			container;
			KeyboardState			keyboardState;
			MouseState			mouseState;
			TopBar				topBar;
			BottomBar			bottomBar;
			SceneViewWidget  		sceneViewWidget;
			QList<View*>			viewsList;
			QList<SubWidget*>		subWidgetsList;
			int				currentViewIndex;

			float				opacityActiveSubWidget,
							opacityIdleSubWidget,
							opacityActiveBar,
							opacityIdleBar;

		private slots :
			void updateMouseStateData(void);
			void performMouseAction(void);
			void setMouseCursor(Qt::CursorShape cursorShape);
			//void handleCatch(void); // Deprecated.

			// Views : 
				void viewRequireDisplay(View* view);
				void viewRequireDisplay(void);
				void viewUpdated(View* view);
				void viewUpdated(void);
				void viewClosed(View* view);
				void viewClosed(void);

			// Widgets : 	
				void subWidgetSelected(SubWidget* subWidget);
				void subWidgetSelected(void);
				void showSubWidget(SubWidget* subWidget);
				void showSubWidget(void);
				void hideSubWidget(SubWidget* subWidget);
				void hideSubWidget(void);
				void subWidgetClosed(SubWidget* subWidget);
				void subWidgetClosed(void);
				void nextSubWidget(void);
				void previousSubWidget(void);
				void temporaryHideAllSubWidgets(bool enabled = true);
				void hideAllSubWidgets(void);

			// Bars : 
				void barSelected(TopBar* bar);
				void barSelected(BottomBar* bar);

		protected :
			KeyboardState& getKeyboardState(void);
			MouseState& getMouseState(void);
			View* getCurrentView(void) const;
			void changeCurrentView(int targetID);
			void getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const;							// correcting for the aspect ratio of the scene.
			float getAdaptationScaling(const float& imageRatio) const;									// correcting for the image filling the scene.
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, View* view=NULL) const;
			void toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, View* view=NULL) const;

			friend class SceneWidget;
			friend class SceneViewWidget;
			
		public :
			MainWidget(QWidget* parent=NULL);
			~MainWidget(void);

			const KeyboardState& getKeyboardState(void) const;
			const MouseState& getMouseState(void) const;
			void addView(View* view);
			void addSubWidget(SubWidget* subWidget);
			float getSceneRatio(void) const;
			QRectF sceneRect(void) const;
			void getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);
			void getColorAt(int x, int y, QColor& c);

		public slots : 
			void processAction(ActionID action, bool takenBack=false);
	};
}

#endif

