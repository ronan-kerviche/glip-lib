#ifndef __GLIPSTUDIO_GL_SCENE_WIDGET__
#define __GLIPSTUDIO_GL_SCENE_WIDGET__

	// Display messages on std::cout :
	//#define __VERBOSE__

	#include "GLIPLib.hpp"
	#include <cmath>
	#include <QGLWidget>
	#include <QKeyEvent>
	#include <QVBoxLayout>
	#include <QMessageBox>
	#include <QMenu>

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	// Prototype
	class GLSceneWidget;

	// Class
	class ViewLink : public QObject, public OutputDevice
	{
		Q_OBJECT

		private :
			static int 		viewCounter;
			static GLSceneWidget	*quietSceneTarget;

			HdlTexture		*target;

			OutputDevice::OutputDeviceLayout getLayout(void) const;
			void process(void);

		protected : 
			GLSceneWidget* 	scene;
			float		haloColorRGB[3];
			float		centerCoords[2],
					angleRadians,
					scale,
					originalScreenRatio;
			bool		fliplr,
					flipud;

			ViewLink(GLSceneWidget* _scene, float screenWidth, float screenHeight);
			
			bool preparedToDraw(void);
			void resetOriginalScreenRatio(float screenWidth, float screenHeight);
			const __ReadOnly_HdlTextureFormat& format(void);
			void getScalingRatios(float* imageScaling, float* haloScaling=NULL, float haloSize=0.0f, float currentPixelX=0.0f, float currentPixelY=0.0f);
			void getLocalCoordinates(float x, float y, float& lx, float& ly);

			friend class GLSceneWidget; 

		public : 
			~ViewLink(void);

			void setHaloColor(float r, float g, float b);
			void clear(void);
			void bringUp(void);
			bool isVisible(void) const;
			bool isSelected(void) const;
			bool isClosed(void) const;
			void selectView(bool dropCurrentSelection=true);

			void beginQuietUpdate(void);
			static void endQuietUpdate(void);

		signals : 
			void broughtUp(void);
			void selected(void);
			void closed(void);
	};

	class ViewManager : public QMenu
	{
		Q_OBJECT

		private : 
			static int 		managerCount;
			int			currentManagerID;
			float			r,
						g, 
						b;
			std::vector<ViewLink*> 	viewLinks;
			std::vector<int>	recordIDs;
			QAction			createNewViewAction,
						closeAllViewAction;

			void genColor(float hue, float& red, float& green, float& blue);
			std::vector<ViewLink*>	getSelectedViewsList(void) const;

		private slots :
			void viewClosed(void);

		protected :
			GLSceneWidget* 	scene;
			ViewManager(GLSceneWidget* _scene, QWidget* parent=NULL);

			friend class GLSceneWidget; 

		public :
			~ViewManager(void);

			void enableCreationAction(bool s);
			void show(int recordID, HdlTexture& texture, bool newView=false);
			void update(int recordID, HdlTexture& texture);
			bool isLinkedToAView(int recordID) const;
			bool isOnDisplay(int recordID) const;
			bool hasViews(void) const;
			void removeRecord(int recordID);

			void beginQuietUpdate(void);
			void endQuietUpdate(void);

		public slots : 
			void closeAllViews(void);

		signals :
			void closed(void);
			void createNewView(void);
	};

	class GLSceneWidget : public QGLWidget
	{
		Q_OBJECT 
		
		public :
			static const int maxViews;

			// Known keyboard actions :
			enum KeyAction
			{
				KeyUp,
				KeyDown,
				KeyLeft,
				KeyRight,
				KeyZoomIn,
				KeyZoomOut,
				KeyRotationClockWise,
				KeyRotationCounterClockWise,
				KeyToggleFullscreen,
				KeyExitOnlyFullscreen,
				KeyResetView,
				KeyCloseView,
				KeyControl,
				KeyShiftRotate,
				KeyToggleHandMode,
				KeySelectAll,
				// Add new keys before this line
				NumActionKey,
				NoAction
			};

		private : 
			static GLSceneWidget*			masterContext;
			static std::vector<GLSceneWidget*>	subContext;		
			std::vector<ViewLink*>			links;
			std::vector<ViewManager*>		managers;
			std::list<ViewLink*>			displayList;
			std::vector<ViewLink*>			selectionList;

			// Mouse : 
			bool 					mouseMovementsEnabled,
								doubleLeftClick,
								doubleRightClick,
								leftClick,
								rightClick,
								mouseJustLeftClicked,
								mouseJustRightClicked,
								mouseJustLeftClickReleased,
								mouseJustRightClickReleased,
								mouseWheelJustTurned;
			int 					wheelSteps,
								deltaWheelSteps,
								deltaX,
								deltaY,
							 	lastPosX,
							 	lastPosY,
								wheelRotationAtX,
								wheelRotationAtY;

			// Keyboard : 
			QKeySequence				keyAssociation[NumActionKey];
			bool 					keyboardMovementsEnabled,
								keyPressed[NumActionKey],
								keyJustPressed[NumActionKey],
								keyJustReleased[NumActionKey];

			// Geometry : 
			GeometryInstance			*quad;

			// Shader : 
			HdlProgram				*placementProgram;

			// GL and Window settings : 
			QWidget*				parent;
			bool 					fullscreenModeEnabled;
			float					clearColorRed,
								clearColorGreen,
								clearColorBlue;
			float					screenCenter[2],
								homothetieCentre[2];
			float					homothetieRapport;
			bool					handMode;

			// Menu : 
			QMenu					contextMenu;
			QAction					*selectAllAction,
								*selectAllVisibleAction,
								*resetSelectedAngleAction,
								*resetSelectedScaleAction,
								*resetSelectedPositionAction,
								*resetSelectionAction,
								*hideSelectedAction,
								*closeSelectedAction,
								*hideAllAction,
								*showAllAction,
								*resetGlobalPositionAction,
								*resetGlobalZoomAction,
								*resetGlobalAction,
								*handModeAction,
								*toggleFullscreenAction;
			QMenu					*transformationOfSelectionMenu;				
			QAction					*turn0Action,
								*turn90Action,
								*turn180Action,
								*turn270Action,
								*fliplrAction,
								*flipudAction;

			// Tools : 
			int getViewID(const ViewLink* view) const;
			KeyAction correspondingAction(const QKeySequence& k) const;
			KeyAction correspondingAction(const QKeyEvent& e) const;

			// Qt Events interception : 
			void keyPressEvent(QKeyEvent* event);
			void keyReleaseEvent(QKeyEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void wheelEvent(QWheelEvent *event);
			void mousePressEvent(QMouseEvent *event);
			void mouseReleaseEvent(QMouseEvent *event);
			void mouseDoubleClickEvent(QMouseEvent* event);

			bool pressed(const KeyAction& a) const;
			bool justPressed(const KeyAction& a);
			bool justReleased(const KeyAction& a);
			bool justLeftClicked(void);
			bool justLeftClickReleased(void);
			bool justRightClicked(void);
			bool justRightClickReleased(void);
			bool justDoubleLeftClicked(void);
			bool justDoubleRightClicked(void);
			bool justMouseWheelTurned(void);

			// Event processing function : 
			ViewLink* updateSelection(bool dropSelection=true);
			void processAction(void);

			// QGL Tools : 
			void initializeGL(void);
			void resizeGL(int width, int height);
			void drawScene(bool forSelection);
			void paintGL(void);
			float getGlobalScale(void) const;
			void getGLCoordinatesAbsoluteRaw(float x, float y, float& glX, float& glY);
			void getGLCoordinatesRelativeRaw(float x, float y, float& glX, float& glY);
			void getGLCoordinatesAbsolute(float x, float y, float& glX, float& glY);
			void getGLCoordinatesRelative(float x, float y, float& glX, float& glY);
			void homothetieComposition(float xc, float yc, float zoomDirection);
			ViewLink* getObjectIDUnder(int x, int y, unsigned char* rgb=NULL);

		private slots : 
			// Actions (for the contextual menu) : 
			void updateContextMenu(void);
			void selectAll(void);
			void selectAllVisible(void);
			void hideAll(void);
			void showAll(void);
			void hideCurrentSelection(void);
			void closeSelection(void);
			void resetSelectionAngle(void);
			void resetSelectionScale(void);
			void resetSelectionPosition(void);
			void resetSelection(void);
			void resetGlobalPosition(void);
			void resetGlobalZoom(void);
			void resetGlobal(void);
			void switchSelectionMode(void);
			void setFullscreenMode(bool enabled);
			void toggleFullscreenMode(void);
			void turn0(void);
			void turn90(void);
			void turn180(void);
			void turn270(void);
			void fliplr(void);
			void flipud(void);

		public : 
			GLSceneWidget(int width, int height, QWidget* _parent=NULL);
			~GLSceneWidget(void);

			// Main mechanics : 
			void updateScene(void);
			ViewLink* createView(void);
			bool viewExists(ViewLink* view, bool throwException = false);
			void bringUpView(ViewLink* view);
			void pushBackView(ViewLink* view);
			void hideView(ViewLink* view);
			void selectView(ViewLink* view, bool dropCurrentSelection=true);
			void unselectView(ViewLink* view);
			bool viewIsVisible(const ViewLink* view) const;
			bool viewIsSelected(const ViewLink* view) const;
			void removeView(ViewLink* view, bool sendSignal=false);

			ViewManager* createManager(void);
			void removeManager(ViewManager* manager, bool sendSignal=false);
			
			// Enable/Disable/Set keys :
			bool isKeyboardActionsEnabled(void) const;
			void setKeyboardActions(bool enabled);
			bool isMouseActionsEnabled(void) const;
			void setMouseActions(bool enabled);
			void setKeyForAction(const KeyAction& action, const QKeySequence& key);
			void removeKeyForAction(const KeyAction& action);

			// Other settings : 
			void setClearColor(float red, float green, float blue);

			// Temporary : 
			//void reloadPlacementShader(void);

		signals :
			void requireContainerCatch(void);
	};

	class GLSceneWidgetContainer : public QWidget
	{
		Q_OBJECT

		private :
			QVBoxLayout 	container;
			GLSceneWidget 	scene;

		private slots :
			void handleCatch(void);

		public :
			GLSceneWidgetContainer(int width, int height, QWidget* parent=NULL);
			~GLSceneWidgetContainer(void);

			GLSceneWidget& sceneWidget(void);
	};

#endif

