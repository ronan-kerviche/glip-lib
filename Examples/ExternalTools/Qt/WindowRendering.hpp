/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : WindowRendering.hpp                                                                       */
/*     Original Date : September 1st 2011                                                                        */
/*                                                                                                               */
/*     Description   : Qt Widget for rendering textures.                                                         */
/*		       It includes a set of tools to zoom, translate and rotate the texture.                     */
/*		       The user can define the pixel aspect ratio and the image aspect ratio.			 */
/*		       Mouse commands (enable with WindowRenderer::setMouseActions)                              */
/*		           - single left click                 : give focus (for keyboard interaction).          */
/*		           - left click and drag               : translation.                                    */
/*		           - right click and drag (horizontal) : rotation.                                       */
/*		           - double left click                 : toggle fullscreen mode.                         */
/*		           - double right click                : reset all transformations.                      */
/*		           - wheel                             : zoom in and out.                                */
/*		       Keyboard commands (defaults, see : WindowRenderer::setKeyForAction,                       */
/*					  enable with WindowRenderer::setKeyboardActions)                        */
/*		           - arrows                            : translation.                                    */
/*                         - d / f keys                        : rotation.                                       */
/*                         - + / - keys                        : zoom in and out.                                */
/*                         - Return key                        : toggle fullscreen mode.                         */
/*                         - Esc key                           : Exit (only) fullscreen mode.                    */
/*                         - space bar                         : reset all transformations.                      */
/*                     Note for fullscreen mode, in order to use the Renderer within a QLayout or complex GUI,   */
/*                     the user must use the class WindowRendererContainer as parent buffer.                     */
/*                                                                                                               */
/*      Known bugs   :                                                                                           */
/*		       - The translation is badly processed when a rotation is applied.                          */
/*		       - The starting scale of the rendered texture is wrong.                                    */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_WINDOW_RENDERER__
#define __GLIPLIB_WINDOW_RENDERER__

	// Display messages on std::cout :
	//#define __VERBOSE__

	#include "GLIPLib.hpp"
	#include <cmath>
	#include <QGLWidget>
	#include <QKeyEvent>

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	// Class
	class WindowRenderer : public QGLWidget, public OutputDevice
	{
		Q_OBJECT

		public :
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
				// Add new keys before this line
				NumActionKey,
				NoAction
			};

		private :
			// Data - Qt
			QWidget* parent;

			// Data - Mouse status :
			bool 	mouseMovementsEnabled,
				doubleLeftClick,
				doubleRightClick,
				leftClick,
				rightClick,
				mouseWheelTurned;
			int 	wheelSteps,
				deltaWheelSteps,
				deltaX,
				deltaY,
			 	lastPosX,
			 	lastPosY,
				wheelRotationAtX,
				wheelRotationAtY;

			// Data - Keyboard status :
			Qt::Key	keyAssociation[NumActionKey];
			bool 	keyboardMovementsEnabled,
				keyPressed[NumActionKey],
				keyJustPressed[NumActionKey],
				keyJustReleased[NumActionKey];

			// OpenGL Positions and window status :
			bool 	fullscreenModeEnabled;
			float 	currentCenterX,
				currentCenterY,
				currentRotationDegrees,
				originalOrientationBeforeRightClick,
				currentRotationCos,
				currentRotationSin,
				currentStepRotationDegrees,
				currentScale,
				currentStepScale,
				currentPixelAspectRatio,
				currentImageAspectRatio,
				currentWindowAspectRatio,
				clearColorRed,
				clearColorGreen,
				clearColorBlue,
				keyPressIncr;

			// Find correspondance Key / Action :
			KeyAction corresponding(const Qt::Key& k) const;

			// Update actions from Mouse/Keys states :
			void updateActions(void);

		private :
			void getScalingCoefficients(float& scaleForCurrentWindowAspectRatioX, float& scaleForCurrentWindowAspectRatioY, float& scaleForCurrentSurfaceAspectRatioX, float& scaleForCurrentSurfaceAspectRatioY, float& scaleFitting);
			void initializeGL(void);

		protected :
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

		public :
			WindowRenderer(QWidget* _parent, int w, int h);
			~WindowRenderer(void);

			void resizeGL(int width, int height); 	// inherited from QGLWidget
			void setPixelAspectRatio(float ratio);	// Fix the pixel aspect ratio (W/H)
			void setImageAspectRatio(float ratio);	// Fix the image aspect ratio (W/H)
			void setClearColor(float red, float green, float blue);
			void clearWindow(bool swapNow = true);
			void process(void); 		// inherited from OutputDevice

			// Enable/Disable actions
			bool isKeyboardActionsEnabled(void) const;
			void setKeyboardActions(bool enabled);

			bool isMouseActionsEnabled(void) const;
			void setMouseActions(bool enabled);

			// Set keys :
			void setKeyForAction(const KeyAction& action, const Qt::Key& key);
			void removeKeyForAction(const KeyAction& action);

			// Special actions :
			void translation(float dx, float dy);
			void rotation(float d, bool realRotation=false);
			void zoom(int incr);
			void setFullscreenMode(bool enabled);
			void toggleFullscreenMode(void);
			bool isFullscreenModeEnabled(void) const;
			void resetTransformation(void);

		signals :
			void resized(void);
			void requireResize(void);
			void actionReceived(void);
	};

	class WindowRendererContainer : public QWidget
	{
		Q_OBJECT

		private :
			WindowRenderer 	childRenderer;

		private slots :
			void handleResizeRequirement(void);

		protected :
			void resizeEvent(QResizeEvent* event);

		public :
			WindowRendererContainer(QWidget* _parent, int w, int h);
			~WindowRendererContainer(void);

			WindowRenderer& renderer(void);
	};

#endif

