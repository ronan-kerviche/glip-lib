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
	#include <QElapsedTimer>

	#include <QLineEdit>
	#include <QComboBox>
	#include <QListWidget>
	#include <QDialogButtonBox>

// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

namespace QVGL
{
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
		ActionCloseViewsTable,
		ActionCloseAllViewsTables,
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
			View(const QString& _name);
			View(HdlTexture* _texture, const QString& _name);
			~View(void);

			bool isValid(void) const;
			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			const QString& getName(void) const;
			void setName(const QString& newName);
			void setTexture(HdlTexture* texture);
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
		
		class Vignette : public QObject, public QGraphicsItemGroup
		{
			Q_OBJECT

			private : 
				static QColor		frameColor,
							selectedFrameColor,
							titleBarColor,
							titleColor,
							infosColor;
				static float		frameThickness,
							titleBarOpacity;

				bool			selected;
				View*			view;
				QGraphicsRectItem	frame,
							titleBar;
				QGraphicsSimpleTextItem	title,
							infos;
			
				Vignette(void);
				Vignette(const Vignette& v);
				void setTitleBarHeight(void);

				void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
				void mousePressEvent(QGraphicsSceneMouseEvent* event);

			public : 
				Vignette(View* _view);
				virtual ~Vignette(void);

				View* getView(void);
				int getWidth(void) const;
				int getHeight(void) const;
				void resize(const QSize& size);
				bool isSelected(void) const;
				void setSelection(bool enabled);
				QRectF boundingRect(void) const;
			
			public slots :
				void updateTitle(void);
				void updateInfos(void);

			signals : 
				void selection(void);
				void showView(View* view);
		};

	class ViewsTable : public QObject, public QGraphicsItemGroup
	{
		Q_OBJECT

		private :
			static float		rho;
			QString			name;
			QMap<View*, Vignette*>	vignettesList;
			int 			a, b, w, h, H, topBarHeight;
			float 			u, v;
			QGraphicsSimpleTextItem	emptyNotification;
			//bool 			visible;

			void computeTableParameters(const QRectF& sceneViewWidget, int N=-1);
			void getIndices(const Vignette* vignette, int& i, int& j) const;
			QPoint getScenePosition(const int& i, const int& j) const;
			QPoint getScenePosition(const Vignette* vignette) const;
			QVariant itemChange(GraphicsItemChange change, const QVariant& value);
			//void setVisible(bool enabled);

			//friend class MainWidget;

		private slots : 
			void resize(const QRectF& size);
			void resize(void);
			void updateSelection(void);
			void viewClosed(void);

		public :
			ViewsTable(const QString& tableName);
			~ViewsTable(void);

			QMap<View*, Vignette*>::iterator begin(void);
			QMap<View*, Vignette*>::const_iterator begin(void) const;
			QMap<View*, Vignette*>::iterator end(void);
			QMap<View*, Vignette*>::const_iterator end(void) const;

			const QString& getName(void) const;
			void setName(const QString& newName);
			View* getCurrentSelectedView(void) const;
			void getGLPositionOfVignette(const Vignette* vignette, int& x, int& y) const;
			QRectF getVignetteFrame(View* view) const;
			//bool isVisible(void) const;
			bool isClosed(void) const;

			//void setVisible(bool enabled); // TEST, TO REMOVE

		public slots :
			void addView(View* view, bool resizeNow=true);
			void addViews(const QList<View*>& viewsList);
			void removeView(View* view, bool resizeNow=true);
			void removeViews(const QList<View*>& viewsList);
			void clear(void);
			void show(void);
			void close(void);

		signals :
			void requireDisplay(void);
			void viewSelection(View* view);
			void showView(View* view);			
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
			virtual ~SubWidget(void);

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
			void closed(void);
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
								subWidgetsMenu;
				QAction				*viewsSeparator,
								closeCurrentViewAction,	
								closeAllViewsAction,
								*viewsTablesSeparator,
								closeCurrentViewsTableAction,
								closeAllViewsTableAction,
								temporaryHideAllSubWidgetsAction,
								hideAllSubWidgetsAction,
								*subWidgetsSeparator;
				QLabel				titleLabel;
				PositionColorInfoMini		positionColorInfo;
				QSignalMapper			signalMapper,
								viewsSignalMapper,
								viewsTablesSignalMapper,
								widgetsSignalMapper;
				QMap<View*, QAction*>		viewsActions;
				QMap<ViewsTable*, QAction*>	viewsTablesActions;
				QMap<SubWidget*, QAction*>	subWidgetsActions;

				void mousePressEvent(QMouseEvent* event);
				void mouseDoubleClickEvent(QMouseEvent* event);
				void mouseMoveEvent(QMouseEvent* event);
				void mouseReleaseEvent(QMouseEvent* event);

				friend class SceneWidget;
				friend class MainWidget;
				
			private slots : 
				void stretch(const QRectF& rect);
				void castViewPointer(QObject* ptr);
				void castViewsTablePointer(QObject* ptr);
				void castSubWidgetPointer(QObject* ptr);
				void sendSelectedSignal(void);
				void transferActionSignal(int actionID);
				void viewClosed(void);
				void viewsTableClosed(void);
				void subWidgetClosed(void);

			public :
				TopBar(void);
				~TopBar(void);

				void setTitle(void);
				void setTitle(QString title);
				void setTitle(const View& view);
				void setTitle(const ViewsTable& table);
				void updatePositionAndColor(const QPointF& pos, const QColor& color);
				void setWindowOpacity(qreal level);

				static int getHeight(void); 

			public slots : 
				void addView(View*);
				void addViewsTable(ViewsTable*);
				void addSubWidget(SubWidget*);

			signals : 
				void changeViewRequest(View* targetView);
				void changeViewsTableRequest(ViewsTable* targetViewsTable);
				void requestAction(ActionID);
				void showSubWidgetRequest(SubWidget* targetWidget);
				void selected(TopBar* ptr);
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
			bool				takeBackEnabled[NumActions];
			bool				actionPressed[NumActions];
	
		protected :
			friend class SceneWidget;
			friend class MainWidget;

			void keyPressed(QKeyEvent* event);	
			void keyReleased(QKeyEvent* event);
			void forceRelease(void);

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

			enum FunctionMode
			{
				ModeMotion,
				ModeCollection
			};

		private : 
			FunctionMode				functionMode;
			QMap<VectorID, QPair<int, QPointF> >	vectors;	// The integer represent a count to the last modification.
			QMap<ColorID,  QPair<int, QColor> >	colors;		// (0 : requires update, 1 : modified, 2+ : not modified)
			QList<VectorID>				vectorIDs;
			QList<ColorID>				colorIDs;
			float					wheelDelta;
			QElapsedTimer				elapsedTimer;
			int					minActionDelta_ms;		
	
		protected :
			friend class SceneWidget;
			friend class MainWidget;

			//const QPointF& invisibleGetVector(const VectorID& id) const;
			//const QColor& invisibleGetColor(const ColorID& id) const;
			void incrementEventCounters(void);
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
			const QPointF& getVector(const VectorID& id) const;
			const QColor& getColor(const ColorID& id) const;
			bool isWheelDeltaModified(void) const;
			float getWheelDelta(void);
			const FunctionMode& getFunctionMode(void) const;
			void setFunctionMode(const FunctionMode& m);

			static VectorID validate(const VectorID& vID);
			static ColorID validate(const ColorID& cID);
			static VectorID getVectorIDFromName(const std::string& name);
			static ColorID getColorIDFromName(const std::string& name);
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

			// Tools : 
			void drawView(View* view);
			void drawView(View* view, const int& x, const int& y, const int& w, const int& h);
			void drawViewsTable(ViewsTable* viewsTable);
			//void getTableParameters(const int& W, const int& H, const int& N, const float& rho, int& a, int& b, int& w, int& h, float& u, float& v) const;

			// Qt events : 
			void drawBackground(QPainter* painter, const QRectF& rect);
			void keyPressEvent(QKeyEvent* event);
			void keyReleaseEvent(QKeyEvent* event);
			void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
			void wheelEvent(QGraphicsSceneWheelEvent* event);
			void mousePressEvent(QGraphicsSceneMouseEvent* event);
			void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
			//void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);	

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
			void addItem(QGraphicsItem* item);
			void removeItem(QGraphicsItem* item);
			SubWidget* getUppermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			SubWidget* getLowermostSubWidget(const QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			void orderSubWidgetsList(QList<SubWidget*>& list, bool onlyIfVisible=true) const;
			void putItemOnTop(QGraphicsProxyWidget* graphicsProxy);				// Both subWidgets and bars.
			void putItemOnBottom(QGraphicsProxyWidget* graphicsProxy);			// Only subWidgets (but accept bars also).
			void makeGLContextAvailable(void);
			void getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);
			void getColorAt(int x, int y, QColor& c);
			void update(void);

		//signals : 
			//void resized(QSize newSize);
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
			QList<ViewsTable*>		viewsTablesList;
			int				currentViewIndex,
							currentViewsTableIndex;
			ViewsTable*			mainViewsTable;

			float				opacityActiveSubWidget,
							opacityIdleSubWidget,
							opacityActiveBar,
							opacityIdleBar;

		private slots :
			void updateMouseStateData(void);
			void performMouseAction(void);
			void setMouseCursor(Qt::CursorShape cursorShape);

			// Views : 
				void viewRequireDisplay(View* view);
				void viewRequireDisplay(void);
				void viewChangeSelection(View* view);
				void viewUpdated(View* view);
				void viewUpdated(void);
				void viewClosed(View* view);
				void viewClosed(void);
				void viewDestroyed(void);
				void closeAllViews(void);

			// ViewsTable :
				void viewsTableRequireDisplay(ViewsTable* viewsTable);
				void viewsTableRequireDisplay(void);
				void viewsTableClosed(ViewsTable* viewsTable);
				void viewsTableClosed(void);
				void viewsTableDestroyed(void);
				void closeAllViewsTables(void);

			// Widgets : 	
				void subWidgetSelected(SubWidget* subWidget);
				void subWidgetSelected(void);
				void showSubWidget(SubWidget* subWidget);
				void showSubWidget(void);
				void hideSubWidget(SubWidget* subWidget);
				void hideSubWidget(void);
				void subWidgetClosed(SubWidget* subWidget);
				void subWidgetClosed(void);
				void subWidgetDestroyed(void);
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
			QList<View*>& getCurrentViewList(void); // To be removed in the future?
			ViewsTable* getCurrentViewsTable(void);
			void changeCurrentView(int targetID, bool showNow=true);
			void hideCurrentView(void);
			void changeCurrentViewsTable(int targetID);
			void hideCurrentViewsTable(void);
			void getSceneRatioScaling(const float& sceneRatio, float& xSceneScale, float& ySceneScale) const;
			void getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const;							// correcting for the aspect ratio of the scene.
			float getAdaptationScaling(const float& sceneRatio, const float& imageRatio) const;
			float getAdaptationScaling(const float& imageRatio) const;									// correcting for the image filling the scene.
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative, const QRectF& rect) const;
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const QRectF& rect, View* view=NULL) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, View* view=NULL) const;
			void toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, View* view=NULL) const;

			friend class SceneWidget;
			friend class SceneViewWidget;
			
		public :
			MainWidget(QWidget* parent=NULL);
			~MainWidget(void);

			const KeyboardState& getKeyboardState(void) const;
			const MouseState& getMouseState(void) const;
			float getSceneRatio(void) const;
			QRectF sceneRect(void) const;
			void getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);
			void getColorAt(int x, int y, QColor& c);

		public slots : 
			void addView(QVGL::View* view);
			void addViewsTable(QVGL::ViewsTable* newTable);
			void addSubWidget(QVGL::SubWidget* subWidget);
			void processAction(ActionID action, bool takenBack=false);

		signals :
			void viewAdded(View*);
			void viewsTableAdded(ViewsTable*);
			void subWidgetAdded(SubWidget*);
	};
}

#endif

