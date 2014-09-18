#ifndef __GLIPSTUDIO_GL_VIEW__
#define __GLIPSTUDIO_GL_VIEW__

	// Display messages on std::cout :
	//#define __VERBOSE__

	#include "GLIPLib.hpp"
	#include <cmath>
	#include <cstring>
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
			void getSize(int& w, int& h);
			void getScalingRatios(float* imageScaling, float* haloScaling=NULL, float haloSize=0.0f, float currentPixelX=0.0f, float currentPixelY=0.0f);
			void getLocalCoordinates(float x, float y, float& lx, float& ly);
			void getCornersPositions(float* xs, float* ys);
			bool getCoordinatesInPixelBasis(float x, float y, int& px, int& py);

			friend class GLSceneWidget; 

		public : 
			QString title;

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
			void show(int recordID, HdlTexture& texture, const QString& title, bool newView=false);
			void update(int recordID, HdlTexture& texture);
			void clear(int recordID);
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

#endif

