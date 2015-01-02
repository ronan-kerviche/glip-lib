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

	#include <QLineEdit>
	#include <QComboBox>
	#include <QListWidget>
	#include <QDialogButtonBox>

	// Enable some interactivity with Uniforms Variables modules (might be declared somewhere else) :
	//#define __MAKE_VARIABLES__
	
	#ifdef __MAKE_VARIABLES__
		#include "UniformsLoaderInterface.hpp"	
	#endif

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
	class GLScene;
	class GLSceneViewWidget;
	class MainWidget;

	const int 	QGraphicsItemVisualPartKey 		= 0,
			QGraphicsItemSubWidgetPointerKey 	= 1;

	enum VisualPart
	{
		ViewVisualPart		= 0x00000001,
		VignetteVisualPart	= 0x00000002,
		ViewsTableVisualPart	= 0x00000004,
		SubWidgetVisualPart	= 0x00000008,
		TopBarVisualPart	= 0x00000010,
		BottomBarVisualPart	= 0x00000030,
		WidgetVisualPartMask	= 0x00000038,
		NotAVisualPart 		= 0
	};

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
		ActionToggleTemporaryHideAllSubWidgets,
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
			bool 		flipUpDown,		// upside down.
					flipLeftRight;		// mirrored.

			void prepareToDraw(void);

			friend class GLScene;
			friend class MainWidget;

		public : 
			QMap<QString, QString> infos;

			View(const QString& _name);
			View(HdlTexture* _texture, const QString& _name);
			~View(void);

			bool isValid(void) const;
			const HdlAbstractTextureFormat& getFormat(void) const;
			const QString& getName(void) const;
			void setName(const QString& newName);
			void setTexture(HdlTexture* texture);
			float getAngle(void) const;
			void setAngle(const float& a);
			void rotate(const float& a);
			void getViewCenter(float& x, float& y) const;
			void getImageCenter(float& x, float& y) const;
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
			static QString getSizeString(size_t sizeInBytes);
			QString getSizeString(void) const;
			QString getDescriptionToolTip(void);

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

		public : 
			enum AnchorMode
			{
				AnchorFree	= 0,
				AnchorLeft	= 1,
				AnchorRight	= 2,
				AnchorMaximized	= 3
			};

		private : 
			QVBoxLayout		layout;
			QWidget			titleWidget;	// For fixed size;
			QHBoxLayout		titleBar;
			QLabel			titleLabel;
			QToolButton		hideButton;
			QWidget			*widget;
			MainWidget		*qvglParent;
			QGraphicsProxyWidget 	*graphicsProxy;
			bool 			motionActive,
						resizeActive,
						resizeHorizontalLock,
						resizeVerticalLock;
			QSize			originalSize,
						sizeAtMousePress;
			QPoint 			originalPosition,
						mousePress,
						offset;
			AnchorMode		anchorMode;

			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void mouseReleaseEvent(QMouseEvent* event);
			void mouseDoubleClickEvent(QMouseEvent* event);
			void wheelEvent(QWheelEvent* event);

			void addChild(QObject* pObject);
			void removeChild(QObject* pObject);
			void childEvent(QChildEvent* e);
			bool eventFilter(QObject* target, QEvent* e);

		private slots :
			void sceneRectChanged(const QRectF& sceneRect);
			void sceneRectChanged(void);
			void graphicsProxyDestroyed(void);

		public : 
			SubWidget(void);
			virtual ~SubWidget(void);

			void setInnerWidget(QWidget* _widget);
			QWidget* getInnerWidget(void);
			void setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy);
			QGraphicsProxyWidget* getGraphicsProxy(void);
			QString getTitle(void);
			void setTitle(QString title);
			void setQVGLParent(MainWidget* _qvglParent);
			MainWidget* getQVGLParent(void);
			void setAnchor(AnchorMode mode);
			const AnchorMode& getAnchor(void) const;

			static SubWidget* getPtrFromProxyItem(QGraphicsItem *item);
			static SubWidget* getPtrFromProxyItem(QGraphicsProxyWidget *proxy);

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
				static TopBar			*singleton;
				QGraphicsProxyWidget 		*graphicsProxy;
				QHBoxLayout			bar;
				QMenuBar			menuBar;
				QMenu				mainMenu,
								viewsMenu,
								viewsTablesMenu,
								subWidgetsMenu;
				QAction				toggleFullscreenAction,
								openSettingsAction,
								quitAction,
								*viewsSeparator,
								closeCurrentViewAction,	
								closeAllViewsAction,
								*viewsTablesSeparator,
								closeCurrentViewsTableAction,
								closeAllViewsTableAction,
								*subWidgetsSeparator,
								toggleTemporaryHideAllSubWidgetsAction,
								hideAllSubWidgetsAction;
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
				void graphicsProxyDestroyed(void);

			public :
				TopBar(void);
				~TopBar(void);

				void setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy);
				QGraphicsProxyWidget* getGraphicsProxy(void);
				void setTitle(void);
				void setTitle(QString title);
				void setTitle(View& view);
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

			private slots : 
				void stretch(const QRectF& rect);
				void graphicsProxyDestroyed(void);

			public :
				BottomBar(void);
				~BottomBar(void);

				void setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy);
				QGraphicsProxyWidget* getGraphicsProxy(void);

			signals : 
				void selected(BottomBar*);
		};

		class SettingsDialog : public QWidget
		{
			Q_OBJECT

			private : 

			public : 
				SettingsDialog(QWidget* parent=NULL);
				~SettingsDialog(void);
		};

	#ifdef __MAKE_VARIABLES__
		class VariablesTrackerSubWidget : public SubWidget
		{
			Q_OBJECT

			private :
				QTreeWidget						innerTreeWidget;
				QMap<GLenum, QTreeWidgetItem*>				typeRoots;
				QMap<const QGUI::VariableRecord*, QTreeWidgetItem*>	items;

				void updateAlternateColors(void);

			private slots :
				void variableAdded(const QGUI::VariableRecord* ptr);
				void variableUpdated(const QGUI::VariableRecord* ptr);
				void variableUpdated(void);
				void variableDeleted(void);
		
			public :
				VariablesTrackerSubWidget(void);
				~VariablesTrackerSubWidget(void);
		};
	#endif

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
			friend class GLScene;
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
				ModeRotation,
				ModeCollection
			};

		private :
			struct VectorData
			{
				private :
					VectorData(const VectorData&);

				public :
					int			modification;
					QPointF			vector;
					#ifdef __MAKE_VARIABLES__
					QGUI::VariableRecord*	record;
					#endif

					VectorData(const QString& name, QObject* parent=NULL);
					~VectorData(void);
			};

			struct ColorData
			{
				private :
					ColorData(const ColorData&);
				public :
					int 			modification; 	// The integer represent a count to the last modification.
					QColor			color;		// (0 : requires update, 1 : modified, 2+ : not modified)
					#ifdef __MAKE_VARIABLES__
					QGUI::VariableRecord*	record;
					#endif

					ColorData(const QString& name, QObject* parent=NULL);
					~ColorData(void);
			};

			static const QMap<VectorID, QString>	vectorsNameMap;
			static const QMap<ColorID, QString>	colorsNameMap; 
			FunctionMode				functionMode;
			QMap<VectorID, VectorData*>		vectors;	
			QMap<ColorID, ColorData*>		colors; 
			QList<VectorID>				vectorIDs;
			QList<ColorID>				colorIDs;
			float					wheelDelta;
	
			static QMap<VectorID, QString> initVectorsNameMap(void);
			static QMap<ColorID, QString> initColorsNameMap(void);

		protected :
			friend class GLScene;
			friend class MainWidget;

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
			static QString getVectorIDName(const VectorID& vID);
			static QString getColorIDName(const ColorID& cID);
			static VectorID getVectorIDFromName(const QString& name);
			static ColorID getColorIDFromName(const QString& name);
			static VectorID getPixelVectorID(const VectorID& vID);
			static BasisID getVectorBasis(const VectorID& vID);
			static ColorID getCorrespondingColorID(const VectorID& cID);
			static bool isBasisRelative(const BasisID& bID);

		signals : 
			void requestExternalUpdate(void);
			void mustSetMouseCursor(Qt::CursorShape cursorShape);
			void updated(void);
	};

	class GLScene : public QGraphicsScene
	{
		Q_OBJECT

		private :
			// Data : 
			MainWidget		*qvglParent;
			GeometryInstance	*quad;
			HdlProgram		*shaderProgram;

			// Tools : 
			void drawView(View* view);
			void drawView(View* view, const int& x, const int& y, const int& w, const int& h);
			void drawViewsTable(ViewsTable* viewsTable);

			// Qt events : 
			void drawBackground(QPainter* painter, const QRectF& rect);
			void keyPressEvent(QKeyEvent* event);
			void keyReleaseEvent(QKeyEvent* event);
			void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
			void wheelEvent(QGraphicsSceneWheelEvent* event);
			void mousePressEvent(QGraphicsSceneMouseEvent* event);
			void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

		public : 
			GLScene(MainWidget* _Parent);
			~GLScene(void);
	};

	class GLSceneViewWidget : public QGraphicsView
	{
		Q_OBJECT

		private : 
			// Data :
			ContextWidget			*contextWidget;
			GLScene 			*glScene;
			MainWidget			*qvglParent;

			// Functions :
			void resizeEvent(QResizeEvent *event);

		public : 
			GLSceneViewWidget(MainWidget* _Parent, TopBar* topBar=NULL, BottomBar* bottomBar=NULL);
			~GLSceneViewWidget(void);

			void addSubWidget(SubWidget* subWidget);
			void addItem(QGraphicsItem* item);
			void removeItem(QGraphicsItem* item);
			void forceItemOrdering(void);
			void putWidgetOnTop(QGraphicsProxyWidget* graphicsProxy);
			void putWidgetOnBottom(QGraphicsProxyWidget* graphicsProxy);
			SubWidget* getTopSubWidget(bool onlyVisible=false);
			SubWidget* getBottomSubWidget(bool onlyVisible=false);
			SubWidget* getSubWidget(int index, bool onlyVisible=false);
			void makeGLContextAvailable(void);
			void getColorAt(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);
			void getColorAt(int x, int y, QColor& c);
			void update(void);

			static void sortItems(QList<QGraphicsItem*>& list, const Qt::SortOrder& order=Qt::DescendingOrder);

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
			GLSceneViewWidget  		glSceneViewWidget;
			QList<View*>			viewsList;
			QList<SubWidget*>		subWidgetsList;
			QList<SubWidget*>		temporaryHiddenSubWidgetsList;
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
				void toggleTemporaryHideAllSubWidgets(void);
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

			friend class GLScene;
			friend class GLSceneViewWidget;
			
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

// Qt Declarations : 
	Q_DECLARE_METATYPE(QVGL::SubWidget*)

#endif

