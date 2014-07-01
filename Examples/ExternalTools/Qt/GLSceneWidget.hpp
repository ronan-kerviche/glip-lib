/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ResourceLoader.hpp                                                                        */
/*     Original Date : December 28th 2012                                                                        */
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
	#include <QGraphicsScene>
	#include <QGraphicsView>
	#include <QGraphicsSceneMouseEvent>
	#include <QGraphicsProxyWidget>
	#include <QMenu>
	#include <QPushButton>
	#include <QSignalMapper>

	#include <QComboBox>
	#include <QMenuBar>

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Prototypes : 
	class QVGLView;
	class QVGLSubWidget;
	class QVGLContextWidget;
	class QVGLKeyboardState;
	class QVGLSceneWidget;
	class QVGLSceneViewWidget;
	class QVGLWidget;

	enum QVGLActionID
	{
		QVGLActionUp,
		QVGLActionDown,
		QVGLActionLeft,
		QVGLActionRight,
		QVGLActionZoomIn,
		QVGLActionZoomOut,
		QVGLActionRotationClockWise,
		QVGLActionRotationCounterClockWise,
		QVGLActionToggleFullscreen,
		QVGLActionExitFullscreen,
		QVGLActionResetView,
		QVGLActionPreviousView,
		QVGLActionNextView,
		QVGLActionCloseView,
		QVGLActionCloseAllViews,
		QVGLActionMotionModifier,
		QVGLActionRotationModifier,
		// Add new actions before this line
		QVGLNumActions,
		QVGLNoAction
	};

	class QVGLView : public QObject	// One image, plus data
	{
		Q_OBJECT 

		private :
			HdlTexture	*texture;
			QString		name;
			QVGLWidget	*qvglParent;
			float		angle,			// in Radians.
					viewCenter[2],
					homothecyCenter[2],
					homothecyScale;
			bool 		flipUpDown,		// upside down
					flipLeftRight;		// mirrored

			void prepareToDraw(void);

			friend class QVGLSceneWidget;
			friend class QVGLWidget;

		public : 
			QVGLView(HdlTexture* _texture, const QString& _name);
			~QVGLView(void);

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

	class QVGLSubWidget : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout		layout;
			QHBoxLayout		titleBar;
			QLabel			titleLabel;
			QToolButton		hideButton;
			QVGLWidget		*qvglParent;
			QGraphicsProxyWidget 	*graphicsProxy;

			bool invalidMotion;
			QPoint offset;
 
			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void mouseReleaseEvent(QMouseEvent* event);

			friend class QVGLSceneViewWidget;
			friend class QVGLWidget;

		public : 
			QVGLSubWidget(QWidget* parent=NULL);
			~QVGLSubWidget(void);

			void setLayout(QLayout* subLayout);
			QString getTitle(void);
			void setTitle(QString title);
			QVGLWidget* getQVGLParent(void);

		public slots :
			// Re-implement some of the QWidget functions : 
			void show(void);
			void hide(void);

		signals  :
			void titleChanged(void);
			void selected(QVGLSubWidget*);
			void showRequest(QVGLSubWidget*);
			void hideRequest(QVGLSubWidget*);
	};

		class QVGLTopBar : public QWidget
		{
			Q_OBJECT

			private : 
				QGraphicsProxyWidget 	*graphicsProxy;
				QHBoxLayout		bar;
				QPushButton		mainMenuButton,
							viewsMenuButton,
							widgetsMenuButton;
				QMenu			mainMenu,
							viewsMenu,
							widgetsMenu;
				QAction			hideAllWidgets,
							closeAllWidgets;
				QLabel			titleLabel;
				QSignalMapper		viewsSignalMapper,
							widgetsSignalMapper;

				void mousePressEvent(QMouseEvent* event);

				friend QVGLSceneWidget;
				friend class QVGLWidget;
				
			private slots : 
				void stretch(const QRectF& rect);
				void castViewPointer(QObject* ptr);
				void castSubWidgetPointer(QObject* ptr);

			public :
				QVGLTopBar(void);
				~QVGLTopBar(void);

				void setTitle(void);
				void setTitle(QString title);
				void setTitle(const QVGLView& view);
				void updateViewsList(const QList<QVGLView*>& viewsList);
				void updateSubWidgetsList(const QList<QVGLSubWidget*>& subWidgetsList);
				void setWindowOpacity(qreal level);

			signals : 
				void changeViewRequest(QVGLView* targetView);
				void showSubWidgetRequest(QVGLSubWidget* targetWidget);
				void selected(QVGLTopBar*);
		};

	class QVGLContextWidget : public QGLWidget
	{
		private :
			bool	glipOwnership;
			float	clearColorRed,
				clearColorGreen,
				clearColorBlue;

			void initializeGL(void);
			void resizeGL(int width, int height);
			
		public : 
			QVGLContextWidget(QGLContext* ctx, QWidget* parent=NULL);
			~QVGLContextWidget(void);
	};

	class QVGLKeyboardState : public QObject
	{
		Q_OBJECT

		private : 
			QMap<QKeySequence, QVGLActionID> 	keysActionsAssociations;
			bool					actionPressed[QVGLNumActions];
	
		protected :
			friend class QVGLSceneWidget;

			void keyPressed(QKeyEvent* event);	
			void keyReleased(QKeyEvent* event);

		public : 
			QVGLKeyboardState(void);
			~QVGLKeyboardState(void);

			QVGLActionID getActionAssociatedToKey(const QKeySequence& keySequence) const;
			QVGLActionID getActionAssociatedToKey(const QKeyEvent* event) const;
			QKeySequence getKeysAssociatedToAction(const QVGLActionID& a);
			void setActionKeySequence(const QVGLActionID& a, const QKeySequence& keySequence);
			void resetActionsKeySequences(void);

		signals : 
			void actionReceived(QVGLActionID);
	};

	class QVGLMouseState : public QObject
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
				VectorLastWheelDownImage		= VectorLastWheelDown + ImageBasis
			};

			enum ColorID
			{
				ColorUnderLastLeftClick,
				ColorUnderLastLeftPosition,
				ColorUnderLastLeftRelease,
				ColorUnderLastRightClick,
				ColorUnderLastRightPosition,
				ColorUnderLastRightRelease
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
			friend class QVGLSceneWidget;
			friend class QVGLWidget;

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
			QVGLMouseState(void);
			~QVGLMouseState(void);

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

			static BasisID getVectorBasis(const VectorID& vID);
			static bool isBasisRelative(const BasisID& bID);

		signals : 
			void requestExternalUpdate(void);
			void updated(void);
	};

	class QVGLSceneWidget : public QGraphicsScene
	{
		Q_OBJECT

		private :
			// Data : 
			QVGLWidget		*qvglParent;
			GeometryInstance	*quad;
			HdlProgram		*shaderProgram;
			QVGLTopBar		*topBar;

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
			QVGLSceneWidget(QVGLWidget* _qvglParent, QVGLTopBar* _topBar);
			~QVGLSceneWidget(void);

			void addSubWidget(QVGLSubWidget* subWidget);
			void putItemOnTop(QGraphicsProxyWidget*);
	};

	class QVGLSceneViewWidget : public QGraphicsView
	{
		Q_OBJECT

		private : 
			// Data :
			QVGLContextWidget			*contextWidget;
			QVGLSceneWidget 			*sceneWidget;
			QVGLWidget				*qvglParent;

			// Functions :
			void resizeEvent(QResizeEvent *event);

		public : 
			QVGLSceneViewWidget(QVGLWidget* _qvglParent, QVGLTopBar* _topBar);
			~QVGLSceneViewWidget(void);

			void addSubWidget(QVGLSubWidget* subWidget);
			void putItemOnTop(QGraphicsProxyWidget* graphicsProxy);				// Both subWidgets and bars.
			void putItemOnBottom(QGraphicsProxyWidget* graphicsProxy);			// Only subWidgets (but accept bars also).
			void makeGLContextAvailable(void);
			void update(void);

		// Deprecated : 
		//signals :
			//void requireContainerCatch(void);
	};
	
	class QVGLWidget : public QWidget
	{
		Q_OBJECT

		private :
			QBoxLayout 			container;
			QVGLKeyboardState		keyboardState;
			QVGLMouseState			mouseState;
			QVGLTopBar			topBar;
			QVGLSceneViewWidget  		sceneViewWidget;
			QList<QVGLView*>		viewsList;
			QList<QVGLSubWidget*>		subWidgetsList;
			int				currentViewIndex;

			float				opacityActiveSubWidget,
							opacityIdleSubWidget,
							opacityActiveBar,
							opacityIdleBar;

		private slots :
			void updateMouseStateData(void);
			void performMouseAction(void);
			//void handleCatch(void); // Deprecated.

			// Views : 
				void viewRequireDisplay(QVGLView* view);
				void viewRequireDisplay(void);
				void viewUpdated(QVGLView* view);
				void viewUpdated(void);
				void viewClosed(QVGLView* view);
				void viewClosed(void);

			// Widgets : 	
				void subWidgetSelected(QVGLSubWidget* subWidget);
				void subWidgetSelected(void);
				void showSubWidget(QVGLSubWidget* subWidget);
				void showSubWidget(void);
				void hideSubWidget(QVGLSubWidget* subWidget);
				void hideSubWidget(void);
				void subWidgetClosed(QVGLSubWidget* subWidget);
				void subWidgetClosed(void);

			// Bars : 
				void barSelected(QVGLTopBar* bar);

		protected :
			QVGLKeyboardState& getKeyboardState(void);
			QVGLMouseState&	getMouseState(void);
			QVGLView* getCurrentView(void) const;
			void changeCurrentView(int targetID);
			void getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const;							// correcting for the aspect ratio of the scene.
			float getAdaptationScaling(const float& imageRatio) const;									// correcting for the image filling the scene.
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, QVGLView* view=NULL) const;
			void toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, QVGLView* view=NULL) const;

			friend class QVGLSceneWidget;
			friend class QVGLSceneViewWidget;
			
		public :
			QVGLWidget(QWidget* parent=NULL);
			~QVGLWidget(void);

			const QVGLKeyboardState& getKeyboardState(void) const;
			const QVGLMouseState& getMouseState(void) const;
			void addView(QVGLView* view);
			void addSubWidget(QVGLSubWidget* subWidget);
			float getSceneRatio(void) const;
			QRectF sceneRect(void) const;

		public slots : 
			void processAction(QVGLActionID action);
	};

#endif

