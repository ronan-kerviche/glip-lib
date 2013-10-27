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
			static int 	viewCounter;
			HdlTexture* 	target;

			OutputDevice::OutputDeviceLayout getLayout(void) const;
			void process(void);

		protected : 
			GLSceneWidget* 	scene;
			float		haloColorRGB[3];

			ViewLink(GLSceneWidget* _scene);
			
			bool preparedToDraw(void);
			const __ReadOnly_HdlTextureFormat& format(void);

			friend class GLSceneWidget; 

		public : 
			~ViewLink(void);

			void setHaloColor(float r, float g, float b);
			void clear(void);
			void bringUp(void);
			bool isVisible(void) const;
			bool isSelected(void) const;
			bool isClosed(void) const;	

		signals : 
			void broughtUp(void);
			void selected(void);
			void closed(void);
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
				KeyControl,
				KeyShiftRotate,
				// Add new keys before this line
				NumActionKey,
				NoAction
			};

		private : 
			static GLSceneWidget*			masterContext;
			static std::vector<GLSceneWidget*>	subContext;		
			std::vector<ViewLink*>			links;
			std::vector<float>			xCoord,
								yCoord,
								angleRadians,
								scale;
			std::list<int>				displayList;
			std::vector<int>			selectionList;

			// Mouse : 
			bool 					mouseMovementsEnabled,
								doubleLeftClick,
								doubleRightClick,
								leftClick,
								rightClick,
								mouseJustLeftClicked,
								mouseJustRightClicked,
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
			Qt::Key					keyAssociation[NumActionKey];
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

			// Menu : 
			QMenu					contextMenu;

			// Tools : 
			int getViewID(const ViewLink* view) const;
			KeyAction correspondingAction(const Qt::Key& k) const;

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
			bool justRightClicked(void);
			bool justDoubleLeftClicked(void);
			bool justDoubleRightClicked(void);
			bool justMouseWheelTurned(void);

			// Event processing function : 
			int  updateSelection(void);
			void processAction(void);

			// QGL Tools : 
			void initializeGL(void);
			void resizeGL(int width, int height);
			void drawScene(bool forSelection);
			void paintGL(void);
			void getGLCoordinates(float x, float y, float& glX, float& glY);
			int  getObjectIDUnder(int x, int y);
			void setFullscreenMode(bool enabled);
			void toggleFullscreenMode(void);

		private slots : 
			// Actions (for the contextual menu) : 
			void hideAll(void);
			void showAll(void);
			void hideCurrentSelection(void);
			void closeSelection(void);
			void resetSelectionAngle(void);
			void resetSelectionPosition(void);

		public : 
			GLSceneWidget(int width, int height, QWidget* _parent=NULL);
			~GLSceneWidget(void);

			// Main mechanics : 
			void updateScene(void);
			ViewLink* createView(void);
			void bringUpView(ViewLink* view);
			void bringUpView(int viewID);
			void pushBackView(ViewLink* view);
			void pushBackView(int viewID);
			void hideView(ViewLink* view);
			void hideView(int viewID);
			void unselectView(ViewLink* view);
			void unselectView(int viewID);
			bool viewIsVisible(const ViewLink* view) const;
			bool viewIsVisible(int viewID) const;
			bool viewIsSelected(const ViewLink* view) const;
			bool viewIsSelected(int viewID) const;
			void removeView(ViewLink* view);
			void removeView(int viewID);

			// Enable/Disable/Set keys :
			bool isKeyboardActionsEnabled(void) const;
			void setKeyboardActions(bool enabled);
			bool isMouseActionsEnabled(void) const;
			void setMouseActions(bool enabled);
			void setKeyForAction(const KeyAction& action, const Qt::Key& key);
			void removeKeyForAction(const KeyAction& action);

			// Other settings : 
			void setClearColor(float red, float green, float blue);

			// Temporary : 
			void reloadPlacementShader(void);

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

		private slots :
			void viewClosed(void);
			void closeAllViews(void);

		public :
			ViewManager(QWidget* parent=NULL);
			~ViewManager(void);

			void enableCreationAction(bool s);
			void show(int recordID, HdlTexture& texture, void* obj, ViewLink* (*createViewLink)(void*), bool newView=false);
			void update(int recordID, HdlTexture& texture);
			bool isLinkedToAView(int recordID) const;
			bool isOnDisplay(int recordID) const;
			bool hasViews(void) const;
			void removeRecord(int recordID);

		signals :
			void createNewView(void);
	};

#endif

