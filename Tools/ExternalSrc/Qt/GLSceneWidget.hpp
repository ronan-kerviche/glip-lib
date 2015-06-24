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
	#include <QPushButton>
	#include <QGroupBox>
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
	class GlipViewWidget;

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
		ActionQuit,
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
			GlipViewWidget	*qvglParent;
			float		angle,			// in Radians.
					viewCenter[2],
					homothecyCenter[2],
					homothecyScale;
			bool 		flipUpDown,		// upside down.
					flipLeftRight;		// mirrored.

			View(void);
			View(const View&);
			View& operator=(const View&);
			void prepareToDraw(void);

			friend class GLScene;
			friend class GlipViewWidget;

		public : 
			QMap<QString, QString> infos;

			View(const QString& _name);
			View(HdlTexture* _texture, const QString& _name);
			virtual ~View(void);

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
			void updated(void);		// For fast updates, which requires duisplay update (such as view paramaters, translation, rotation, magnification, etc.)
			void internalDataUpdated(void);	// For slow update (filtering / format update of the HdlTexture, etc.)
			void requireDisplay(void);
			void nameChanged(void);
			void closed(void);
	};
		
		class Vignette : public QObject, public QGraphicsItemGroup
		{
			Q_OBJECT

			private : 
				static const QColor	frameColor,
							selectedFrameColor,
							titleBarColor,
							titleColor,
							infosColor;
				static const float	frameThickness,
							titleBarOpacity;

				bool			selected;
				View*			view;
				QGraphicsRectItem	frame,
							titleBar;
				QGraphicsSimpleTextItem	title,
							infos;
			
				Vignette(void);
				Vignette(const Vignette& v);
				Vignette& operator=(const Vignette&);
				void setTitleBarHeight(void);

				void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
				void mousePressEvent(QGraphicsSceneMouseEvent* event);
				void enforceOrdering(void);

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

			ViewsTable(void);
			ViewsTable(const ViewsTable&);
			ViewsTable& operator=(const ViewsTable&);
			void computeTableParameters(const QRectF& sceneViewWidget, int N=-1);
			void getIndices(const Vignette* vignette, int& i, int& j) const;
			QPoint getScenePosition(const int& i, const int& j) const;
			QPoint getScenePosition(const Vignette* vignette) const;
			QVariant itemChange(GraphicsItemChange change, const QVariant& value);

		private slots : 
			void resize(const QRectF& size);
			void resize(void);
			void updateSelection(void);
			void viewClosed(void);

		public :
			ViewsTable(const QString& tableName);
			virtual ~ViewsTable(void);

			QMap<View*, Vignette*>::iterator begin(void);
			QMap<View*, Vignette*>::const_iterator begin(void) const;
			QMap<View*, Vignette*>::iterator end(void);
			QMap<View*, Vignette*>::const_iterator end(void) const;

			const QString& getName(void) const;
			void setName(const QString& newName);
			View* getCurrentSelectedView(void) const;
			void getGLPositionOfVignette(const Vignette* vignette, int& x, int& y) const;
			QRectF getVignetteFrame(const View* view) const;
			bool isClosed(void) const;

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
			enum Flag
			{
				NotResizeable		= 1,
				NotAnchorable		= 2,
				NotMaximizable		= 4,
				CloseOnHideRequest	= 8,
				NoFlag			= 0
			};

			enum AnchorMode
			{
				AnchorFree		= 0,
				AnchorLeft		= 1,
				AnchorRight		= 2,
				AnchorMaximized		= 3
			};

		private : 
			QVBoxLayout		layout;
			QWidget			titleWidget;	// For fixed size;
			QHBoxLayout		titleBar;
			QLabel			titleLabel;
			QToolButton		hideButton;
			QWidget			*widget;
			GlipViewWidget		*qvglParent;
			QGraphicsProxyWidget 	*graphicsProxy;
			const Flag		flags;
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

			SubWidget(const SubWidget&);
			SubWidget& operator=(const SubWidget&);
			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void mouseReleaseEvent(QMouseEvent* event);
			void mouseDoubleClickEvent(QMouseEvent* event);
			void wheelEvent(QWheelEvent* event);
			void showEvent(QShowEvent* event);
			void closeEvent(QCloseEvent* event);

			void addChild(QObject* pObject);
			void removeChild(QObject* pObject);
			void childEvent(QChildEvent* e);
			bool eventFilter(QObject* target, QEvent* e);

		private slots :
			void sceneRectChanged(const QRectF& sceneRect);
			void sceneRectChanged(void);
			void graphicsProxyDestroyed(void);

		public : 
			SubWidget(const Flag _flags=SubWidget::NoFlag, QWidget* parent=NULL);
			virtual ~SubWidget(void);

			void setInnerWidget(QWidget* _widget);
			QWidget* getInnerWidget(void);
			void setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy);
			QGraphicsProxyWidget* getGraphicsProxy(void);
			QPoint mapItemCoordinatesToGlobal(const QPoint& p);
			QString getTitle(void);
			void setTitle(QString title);
			virtual void setQVGLParent(GlipViewWidget* _qvglParent);
			GlipViewWidget* getQVGLParent(void);
			void setAnchor(AnchorMode mode);
			const AnchorMode& getAnchor(void) const;
			void resetPosition(bool force=true);

			static SubWidget* getPtrFromProxyItem(QGraphicsItem *item);
			static SubWidget* getPtrFromProxyItem(QGraphicsProxyWidget *proxy);
			static QPoint mapItemCoordinatesToGlobal(const QPoint& p, QGraphicsProxyWidget* ptr);
			static QPoint mapItemCoordinatesToGlobal(const QPoint& p, QWidget* ptr);
			static QPoint mapItemCoordinatesToGlobal(const QPoint& p, SubWidget* ptr);

		public slots :
			// Re-implement some of the QWidget functions : 
			void show(void);
			void hide(void);	// The SubWidget is hidden from the workbench, but is still present in the lists.
			void close(void);	// The SubWidget is removed from the workbench and the list.

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
				virtual ~PositionColorInfoMini(void);

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
								openSettingsInterfaceAction,
								openInfosAction,
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

				TopBar(const TopBar&);
				TopBar& operator=(const TopBar&);
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
				TopBar(QWidget* parent=NULL);
				virtual ~TopBar(void);

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
				void requestOpenInfos(void);
				void requestOpenSettingsInterface(void);
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
				
				BottomBar(const BottomBar&);
				BottomBar& operator=(const BottomBar&);
				void mousePressEvent(QMouseEvent* event);

			private slots : 
				void stretch(const QRectF& rect);
				void graphicsProxyDestroyed(void);

			public :
				BottomBar(void);
				virtual ~BottomBar(void);

				void setGraphicsProxy(QGraphicsProxyWidget* _graphicsProxy);
				QGraphicsProxyWidget* getGraphicsProxy(void);

			signals : 
				void selected(BottomBar*);
		};	

		class InfosDialog : public SubWidget
		{
			Q_OBJECT

			private : 
				QLabel message;

			public : 
				InfosDialog(void);
				virtual ~InfosDialog(void);
		};

	#ifdef __MAKE_VARIABLES__
		class VariablesTrackerSubWidget : public SubWidget
		{
			Q_OBJECT

			private :
				QTreeWidget					innerTreeWidget;
				QMap<GLenum, QTreeWidgetItem*>			typeRoots;
				QMap<QGUI::VariableRecord*, QTreeWidgetItem*>	items;

				void updateAlternateColors(void);

			private slots :
				void variableAdded(const QGUI::VariableRecord* ptr);
				void variableUpdated(const QGUI::VariableRecord* ptr);
				void variableUpdated(void);
				void variableLockChanged(const QGUI::VariableRecord* ptr, bool locked);
				void variableLockChanged(bool locked);
				void variableDeleted(void);
				void lockSelection(void);
				void unlockSelection(void);
				void execCustomContextMenu(const QPoint& pos);
		
			public :
				VariablesTrackerSubWidget(void);
				virtual ~VariablesTrackerSubWidget(void);
		};
	#endif

	class GlipViewSettings
	{
		protected : 
			QVector<bool>			takeBackEnabled;
			QMap<QKeySequence, ActionID> 	keysActionsAssociations;
			
		public : 
			QColor				backgroundColor;
			float				translationStep,
							rotationStep,
							zoomFactor;

			GlipViewSettings(void);
			GlipViewSettings(const GlipViewSettings& c);
			virtual ~GlipViewSettings(void);

			QKeySequence getKeysAssociatedToAction(const ActionID& a) const;
			const QMap<QKeySequence, ActionID>& getKeysActionsAssociations(void) const;
			const QVector<bool>& getTakeBackEnabled(void) const;
			void setActionKeySequence(const ActionID& a, const QKeySequence& keySequence, bool enableTakeBack=false);
			void resetActionsKeySequences(void);
			void resetSettings(void);
			virtual GlipViewSettings& operator=(const GlipViewSettings& c);
	};

		class KeyGrabber : public QPushButton
		{
			Q_OBJECT

			private : 
				QKeySequence currentKey;

				void keyPressEvent(QKeyEvent * e);

				static QString getKeyName(const QKeySequence& key);

			public : 
				const ActionID actionID;

				KeyGrabber(const ActionID& _actionID, const QKeySequence& _currentKey, QWidget* parent = NULL);
				~KeyGrabber(void);

				const QKeySequence& getKey(void) const;
				void setKey(const QKeySequence& key);
				void removeKey(void);

			signals : 
				void modified(void);
		};

		class GlipViewSettingsInterface : public SubWidget
		{
			Q_OBJECT

			private :
				GlipViewSettings	settings;
				QWidget			innerWidget;
				QGroupBox		keysGroupBox,
							othersGroupBox;
				QVBoxLayout		layout;
				QGridLayout		keysLayout;
				QHBoxLayout		othersLayout;
				QVector<KeyGrabber*>	keyGrabbers;
				QLabel			translationStepLabel,
							rotationStepLabel,
							zoomFactorLabel,
							backgroundColorLabel;
				QDoubleSpinBox		translationStepSpin,
							rotationStepSpin,
							zoomFactorSpin;
				QPushButton		backgroundColorButton;
				QDialogButtonBox	dialogButtons;
			
				void updateBackgroundColorButton(void);
				void updateInterface(void);
				void updateData(void); 

			private slots :
				void checkKeys(const KeyGrabber* ptr);
				void checkKeys(void);
				void changeBackgroundColor(void);
				void processDialogButtonPressed(QAbstractButton* button);

			public : 
				GlipViewSettingsInterface(const GlipViewSettings& _settings);
				~GlipViewSettingsInterface(void);

				const GlipViewSettings& getSettings(void) const;

			public slots : 
				void resetSettings(void);

			signals : 
				void applySettings(void);
		};

	class KeyboardState
	{
		private : 
			QMap<QKeySequence, ActionID> 	keysActionsAssociations;
			QVector<bool>			takeBackEnabled;
			QVector<bool>			actionPressed;

			ActionID getActionAssociatedToKey(const QKeySequence& keySequence) const;
			ActionID getActionAssociatedToKey(const QKeyEvent* event) const;
			QKeySequence getKeysAssociatedToAction(const ActionID& a);

		public : 
			KeyboardState(const GlipViewSettings& settings);
			virtual ~KeyboardState(void);

			void setKeyboardSettings(const GlipViewSettings& settings);
			void processKeyEvent(QKeyEvent* event, bool pressed);
			void forceKeyRelease(void);
			virtual void processAction(ActionID action, bool takenBack=false) = 0;
	};

	class MouseState
	{
		public : 
			enum BasisID
			{
				PixelBasis		= 0,
				GlBasis			= 1,
				QuadBasis		= 2,
				ImageBasis		= 3,
				FragmentBasis		= 4,
				PixelRelativeBasis	= 5,	// "Relative" correspond to displacements (no offset calculation needed)
				GlRelativeBasis		= 6,
				QuadRelativeBasis	= 7,
				ImageRelativeBasis	= 8,
				FragmentRelativeBasis	= 9,
				NumBasis		= 10
			};

			enum VectorID
			{
				VectorLastLeftClick			= 0 * NumBasis,
				VectorLastLeftClickGl			= VectorLastLeftClick + GlBasis,
				VectorLastLeftClickQuad			= VectorLastLeftClick + QuadBasis,
				VectorLastLeftClickImage		= VectorLastLeftClick + ImageBasis,
				VectorLastLeftClickFragment		= VectorLastLeftClick + FragmentBasis,

				VectorLastLeftPosition			= 1 * NumBasis,
				VectorLastLeftPositionGl		= VectorLastLeftPosition + GlBasis,
				VectorLastLeftPositionQuad		= VectorLastLeftPosition + QuadBasis,
				VectorLastLeftPositionImage		= VectorLastLeftPosition + ImageBasis,
				VectorLastLeftPositionFragment		= VectorLastLeftPosition + FragmentBasis,

				VectorLastLeftShift			= 2 * NumBasis + PixelRelativeBasis,	// Will force the following to relative
				VectorLastLeftShiftGl			= VectorLastLeftShift + GlBasis,
				VectorLastLeftShiftQuad			= VectorLastLeftShift + QuadBasis,
				VectorLastLeftShiftImage		= VectorLastLeftShift + ImageBasis,
				VectorLastLeftShiftFragment		= VectorLastLeftShift + FragmentBasis,

				VectorLastLeftRelease			= 3 * NumBasis,
				VectorLastLeftReleaseGl			= VectorLastLeftRelease + GlBasis,
				VectorLastLeftReleaseQuad		= VectorLastLeftRelease + QuadBasis,
				VectorLastLeftReleaseImage		= VectorLastLeftRelease + ImageBasis,
				VectorLastLeftReleaseFragment		= VectorLastLeftRelease + FragmentBasis,

				VectorLastLeftCompletedVector		= 4 * NumBasis + PixelRelativeBasis,
				VectorLastLeftCompletedVectorGl		= VectorLastLeftCompletedVector + GlBasis,
				VectorLastLeftCompletedVectorQuad	= VectorLastLeftCompletedVector + QuadBasis,
				VectorLastLeftCompletedVectorImage	= VectorLastLeftCompletedVector + ImageBasis,
				VectorLastLeftCompletedVectorFragment	= VectorLastLeftCompletedVector + FragmentBasis,

				VectorLastRightClick			= 5 * NumBasis,
				VectorLastRightClickGl			= VectorLastRightClick + GlBasis,
				VectorLastRightClickQuad		= VectorLastRightClick + QuadBasis,
				VectorLastRightClickImage		= VectorLastRightClick + ImageBasis,
				VectorLastRightClickFragment		= VectorLastRightClick + FragmentBasis,

				VectorLastRightPosition			= 6 * NumBasis,
				VectorLastRightPositionGl		= VectorLastRightPosition + GlBasis,
				VectorLastRightPositionQuad		= VectorLastRightPosition + QuadBasis,
				VectorLastRightPositionImage		= VectorLastRightPosition + ImageBasis,
				VectorLastRightPositionFragment		= VectorLastRightPosition + FragmentBasis,

				VectorLastRightShift			= 7 * NumBasis + PixelRelativeBasis,	// Will force the following to relative
				VectorLastRightShiftGl			= VectorLastRightShift + GlBasis,
				VectorLastRightShiftQuad		= VectorLastRightShift + QuadBasis,
				VectorLastRightShiftImage		= VectorLastRightShift + ImageBasis,
				VectorLastRightShiftFragment		= VectorLastRightShift + FragmentBasis,

				VectorLastRightRelease			= 8 * NumBasis,
				VectorLastRightReleaseGl		= VectorLastRightRelease + GlBasis,
				VectorLastRightReleaseQuad		= VectorLastRightRelease + QuadBasis,
				VectorLastRightReleaseImage		= VectorLastRightRelease + ImageBasis,
				VectorLastRightReleaseFragment		= VectorLastRightRelease + FragmentBasis,

				VectorLastRightCompletedVector		= 9 * NumBasis + PixelRelativeBasis,
				VectorLastRightCompletedVectorGl	= VectorLastRightCompletedVector + GlBasis,
				VectorLastRightCompletedVectorQuad	= VectorLastRightCompletedVector + QuadBasis,
				VectorLastRightCompletedVectorImage	= VectorLastRightCompletedVector + ImageBasis,
				VectorLastRightCompletedVectorFragment	= VectorLastRightCompletedVector + FragmentBasis,

				VectorLastWheelUp			= 10 * NumBasis,
				VectorLastWheelUpGl			= VectorLastWheelUp + GlBasis,
				VectorLastWheelUpQuad			= VectorLastWheelUp + QuadBasis,
				VectorLastWheelUpImage			= VectorLastWheelUp + ImageBasis,
				VectorLastWheelUpFragment		= VectorLastWheelUp + FragmentBasis,

				VectorLastWheelDown			= 11 * NumBasis,
				VectorLastWheelDownGl			= VectorLastWheelDown + GlBasis,
				VectorLastWheelDownQuad			= VectorLastWheelDown + QuadBasis,
				VectorLastWheelDownImage		= VectorLastWheelDown + ImageBasis,
				VectorLastWheelDownFragment		= VectorLastWheelDown + FragmentBasis,

				MaxNumVectors				= 12 * NumBasis,
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

				MaxNumColors			= 9 * NumBasis,
				InvalidColorID			= 65535
				// ALSO UPDATE VALIDATE
			};	

			enum FunctionMode
			{
				ModeMotion,
				ModeRotation,
				ModeCollection
			};

			struct VectorData
			{
				private :
					QPointF			vector;
					#ifdef __MAKE_VARIABLES__
					QGUI::VariableRecord*	record;
					#endif
	
					VectorData(const VectorData&);

				public :
					const VectorID id;
					
					VectorData(const VectorID _id, const QString& name, QObject* parent=NULL);
					~VectorData(void);
					const QPointF& getVector(void) const; 
					void setVector(const QPointF& v, bool setRecord);
			};

			struct ColorData
			{
				private :
					QColor			color;
					#ifdef __MAKE_VARIABLES__
					QGUI::VariableRecord*	record;
					#endif

					ColorData(const ColorData&);

				public :
					const ColorID id;

					ColorData(const ColorID _id, const QString& name, QObject* parent=NULL);
					~ColorData(void);
					const QColor& getColor(void) const;
					void setColor(const QColor& c, bool setRecord);
			};

		private :
			
			//static const QMap<VectorID, QString>	vectorsNameMap;
			//static const QMap<ColorID, QString>	colorsNameMap; 
			FunctionMode				functionMode;
			QVector<VectorData*>			vectors;	// some of the elements will be NULL.
			QVector<ColorData*>			colors; 	// some of the elements will be NULL.
	
			//static QMap<VectorID, QString> initVectorsNameMap(void);
			//static QMap<ColorID, QString> initColorsNameMap(void);

		public : 
			void processMouseEvent(QGraphicsSceneWheelEvent* event);
			void processMouseEvent(QGraphicsSceneMouseEvent* event, const bool clicked, const bool moved, const bool released);

		protected :
			// Signals equivalent : 
			virtual void updateMouseVectorAndColor(const VectorData& vPixelBasis, VectorData& vGlBasis, VectorData& vQuadBasis, VectorData& vImageBasis, VectorData& vFragmentBasis, const bool isRelative, ColorData* colorData=NULL) = 0;
			virtual void applyMouseAction(const VectorData& vectorData, const VectorData* shiftVectorData=NULL, const float wheelDelta=0.0f) = 0;
			virtual void setMouseCursor(Qt::CursorShape cursorShape) = 0;

		public : 
			MouseState(QObject* parent);
			virtual ~MouseState(void);

			const FunctionMode& getMouseFunctionMode(void) const;
			void setMouseFunctionMode(const FunctionMode& m);

			static bool isVectorIDValid(const VectorID& vID);
			static bool isColorIDValid(const ColorID& cID);
			static QString getVectorIDName(const VectorID& vID);
			static QString getColorIDName(const ColorID& cID);
			static VectorID getPixelVectorID(const VectorID& vID);
			static BasisID getVectorBasis(const VectorID& vID);
			static ColorID getCorrespondingColorID(const VectorID& cID);
			static bool isBasisRelative(const BasisID& bID);
	};

	class ContextWidget : public QGLWidget
	{
		private :
			bool glipOwnership;
			

			void initializeGL(void);
			void resizeGL(int width, int height);
			
		public : 
			ContextWidget(QGLContext* ctx, QWidget* parent=NULL);
			virtual ~ContextWidget(void);
	};

	class GLScene : public QGraphicsScene
	{
		Q_OBJECT

		private :
			// Data : 
			GlipViewWidget		*qvglParent;
			GeometryInstance	*quad;
			HdlProgram		*shaderProgram;
			float			clearColorRed,
						clearColorGreen,
						clearColorBlue;

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
			GLScene(GlipViewWidget* _Parent);
			virtual ~GLScene(void);

			void setSceneSettings(const GlipViewSettings& settings);
	};
	
	class GlipViewWidget : public QGraphicsView, public KeyboardState, public MouseState
	{
		Q_OBJECT

		private :
			ContextWidget			*contextWidget;
			GLScene 			*glScene;
			TopBar				topBar;
			BottomBar			bottomBar;
			InfosDialog			*infosDialog; 		// Must be created after the ContextWidget.
			GlipViewSettingsInterface	settingsInterface;
			QMap<int,View*>			viewsList;
			QMap<int,SubWidget*>		subWidgetsList;
			QList<SubWidget*>		temporaryHiddenSubWidgetsList;
			QMap<int,ViewsTable*>		viewsTablesList;
			int				currentViewIndex,
							currentViewsTableIndex;
			ViewsTable*			mainViewsTable;

			float				opacityActiveSubWidget,
							opacityIdleSubWidget,
							opacityActiveBar,
							opacityIdleBar;

			// GraphicsView Tools : 
			void resizeEvent(QResizeEvent *event);
			//void forceItemOrdering(void);
			void putWidgetOnTop(QGraphicsProxyWidget* graphicsProxy);
			void putWidgetOnBottom(QGraphicsProxyWidget* graphicsProxy);
			SubWidget* getTopSubWidget(bool onlyVisible);
			SubWidget* getBottomSubWidget(bool onlyVisible);
			SubWidget* getSubWidget(int index, bool onlyVisible);
			static void sortItems(QList<QGraphicsItem*>& list, const Qt::SortOrder& order=Qt::DescendingOrder);

		protected : 
			void updateMouseVectorAndColor(const VectorData& vPixelBasis, VectorData& vGlBasis, VectorData& vQuadBasis, VectorData& vImageBasis, VectorData& vFragmentBasis, const bool isRelative, ColorData* colorData=NULL);
			void applyMouseAction(const VectorData& vectorData, const VectorData* shiftVectorData=NULL, const float wheelDelta=0.0f);
			void setMouseCursor(Qt::CursorShape cursorShape);

		private slots :
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

			// Special : 
			void processOpenInfosRequest(void);
			void processOpenSettingsInterfaceRequest(void);
			void applySettings(void);

		protected :
			View* getCurrentView(void) const;
			ViewsTable* getCurrentViewsTable(void);
			void changeCurrentView(int targetID, const bool showNow=true, const bool goingUp=true);
			void hideCurrentView(void);
			void changeCurrentViewsTable(int targetID, const bool goingUp=true);
			void hideCurrentViewsTable(void);
			void getSceneRatioScaling(const float& sceneRatio, float& xSceneScale, float& ySceneScale) const;
			void getSceneRatioScaling(float& xSceneScale, float& ySceneScale) const;				// correcting for the aspect ratio of the scene.
			float getAdaptationScaling(const float& sceneRatio, const float& imageRatio) const;
			float getAdaptationScaling(const float& imageRatio) const;						// correcting for the image filling the scene.
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative, const QRectF& rect) const;
			void toGlCoordinates(int x, int y, float& xGl, float& yGl, bool isRelative) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const QRectF& rect, const View* view=NULL) const;
			void toQuadCoordinates(const float& xGl, const float& yGl, float& xQuad, float& yQuad, bool isRelative, const View* view=NULL) const;
			void toImageCoordinates(const float& xQuad, const float& yQuad, float& xImg, float& yImg, bool isRelative, const View* view=NULL) const;
			void toFragmentCoordinates(const float& xQuad, const float& yQuad, float& xFrag, float& yFrag, bool isRelative, const View* view=NULL) const;
		
			// Events : 
			void closeEvent(QCloseEvent *event);

			friend class GLScene;
			
		public :
			GlipViewWidget(QWidget* parent=NULL, const QSize& originalSize = QSize(800, 600));
			virtual ~GlipViewWidget(void);

			float getSceneRatio(void) const;
			QColor getColorAt(int x, int y);
			ContextWidget& getContextWidget(void);

			static QString getActionName(const ActionID& a);

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

