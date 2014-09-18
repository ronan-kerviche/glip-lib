#ifndef __GLIPSTUDIO_GL_SCENE_WIDGET__
#define __GLIPSTUDIO_GL_SCENE_WIDGET__

	// Display messages on std::cout :
	//#define __VERBOSE__

	#include "GLIPLib.hpp"
	#include "glView.hpp"
	#include <cmath>
	#include <cstring>
	#include <QGLWidget>
	#include <QKeyEvent>
	#include <QVBoxLayout>
	#include <QMessageBox>
	#include <QMenu>

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

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
				KeyCloseAllViews,
				KeyControl,
				KeyShiftRotate,
				KeySetHandMode,
				KeySetManipulationMode,
				KeySetSelectionMode,
				KeySelectAll,
				// Add new keys before this line
				NumActionKey,
				NoAction
			};

			// Current mouse mode : 
			enum MouseMode
			{
				HandMode,
				ManipulationMode,
				SelectionMode,
				NumMouseMode,
				NoMode
			};

			// Current mouse data : 
			struct MouseData
			{
				int 		lastSelectionWidth,
						lastSelectionHeight;
				float 		xLastClick,
						yLastClick,
						xCurrent,
						yCurrent,
						xLastRelease,
						yLastRelease,
						xVectorCurrent,
						yVectorCurrent,
						xLastVector,
						yLastVector;
				unsigned char	colorLastClick[3],
						colorCurrent[3],
						colorLastRelease[3];

				MouseData(void);
				MouseData(const MouseData& c);
				QColor getQColorLastClick(void) const;
				QColor getQColorCurrent(void) const;
				QColor getQColorLastRelease(void) const;
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
			MouseMode				currentMouseMode;

			// Menu : 
			QMenu					contextMenu;
			QAction					*selectAllAction,
								*resetSelectedAngleAction,
								*resetSelectedScaleAction,
								*resetSelectedPositionAction,
								*resetSelectionAction,
								*closeSelectedAction,
								*closeAllAction,
								*resetGlobalPositionAction,
								*resetGlobalZoomAction,
								*resetGlobalAction,
								*handModeAction,
								*manipulationModeAction,
								*selectionModeAction,
								*toggleFullscreenAction;
			QMenu					*transformationOfSelectionMenu;			
			QAction					*turn0Action,
								*turn90Action,
								*turn180Action,
								*turn270Action,
								*fliplrAction,
								*flipudAction;
			QMenu					*changeSelectionStackMenu;
			QAction					*raiseSelectedAction,
								*setSelectedOnForegroundAction,
								*lowerSelectedAction,
								*setSelectedOnBackgroundAction;

			// Mouse and coordinates memory : 
			MouseData				mouseData;

			// Tools : 
			int getViewID(const ViewLink* view) const;
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
			ViewLink* updateSelection(bool addToSelection, bool dropSelection, unsigned char* colorUnderClick=NULL);
			void processMouseAction(void);
			void processKeyboardAction(void);

			// QGL Tools : 
			void initializeGL(void);
			void resizeGL(int width, int height);
			void drawScene(bool forSelection);
			void drawInformationBar(QPainter& painter);
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
			void closeSelection(void);
			void closeAll(void);
			void resetSelectionAngle(void);
			void resetSelectionScale(void);
			void resetSelectionPosition(void);
			void resetSelection(void);
			void resetGlobalPosition(void);
			void resetGlobalZoom(void);
			void resetGlobal(void);
			void switchSelectionMode(void);
			void switchSelectionMode(MouseMode newMouseMode);
			void setFullscreenMode(bool enabled);
			void toggleFullscreenMode(void);
			void turn0(void);
			void turn90(void);
			void turn180(void);
			void turn270(void);
			void fliplr(void);
			void flipud(void);
			void raiseSelected(void);
			void setSelectedOnForeground(void);
			void lowerSelected(void);
			void setSelectedOnBackground(void);

		public : 
			GLSceneWidget(int width, int height, QWidget* _parent=NULL);
			~GLSceneWidget(void);

			// Main mechanics : 
			void updateScene(void);
			ViewLink* createView(void);
			bool viewExists(ViewLink* view, bool throwException = false);
			void bringUpView(ViewLink* view);
			void raiseView(ViewLink* view);
			void pushBackView(ViewLink* view);
			void lowerView(ViewLink* view);
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
			void mouseDataUpdated(const GLSceneWidget::MouseData& data); 
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

