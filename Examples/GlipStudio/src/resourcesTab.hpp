#ifndef __GLIPSTUDIO_RESOURCETAB__
#define __GLIPSTUDIO_RESOURCETAB__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	#include "imagesCollection.hpp"

	#include <QString>
	#include <QMenu>
	#include <QAction>
	#include <QHBoxLayout>
	#include <QPushButton>
	#include <QProgressBar>
	#include <QStringList>
	#include <QVBoxLayout>
	#include <QMenuBar>
	#include <QTreeWidget>
	#include <QSignalMapper>
	#include <QFileInfo>
	#include <QMessageBox>
	#include <QFileDialog>
	#include <QInputDialog>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class ConnectionMenu : public QMenu
	{
		Q_OBJECT

		private : 
			QList<QString>		portsNames;
			QList<QAction *>	currentActions;
			QSignalMapper		mapper;

		public : 
			ConnectionMenu(QWidget* parent=NULL);
			~ConnectionMenu(void);

			void activate(bool state, int numConnections=1);
			void update(void);
			void update(const __ReadOnly_PipelineLayout& layout);			

		signals :
			void connectToInput(int i);
	};

	class ResourcesTab : public Module 
	{
		Q_OBJECT

		private : 
			std::vector<int>	imageRecordIDs;

			QVBoxLayout		layout;
			QMenuBar		menuBar;
			ConnectionMenu		connectionMenu;
			ImagesCollection	collection;
			ViewManager* 		viewManager;

			//void updateDisplay(WindowRenderer& display);
			bool isValidTexture(int recordID);
			HdlTexture& getTexture(int recordID);
			const __ReadOnly_HdlTextureFormat& getTextureFormat(int recordID) const;
			void giveTextureInformation(int recordID, std::string& name);
			void cleanRecordDependances(int recordID);
			void updateTexturesLinkInformation(void);
			bool canBeClosed(void);

		private slots :
			// These will be called upon external modifications, in order for the module to update itself :
			void pipelineWasCreated(void);
			void pipelineInputFromThisModuleWasReleased(int portID, int recordID);
			void pipelineWasDestroyed(void);
			void focusChanged(int recordID);
			void selectionChanged(void);
			void connectToInput(int i);
			void imageLoaded(int recordID);
			void imageReplaced(int recordID);
			void imageSettingsChanged(int recordID);
			void imageUnloadedFromDevice(int recordID);
			void imageFreed(int recordID);
			void createNewView(void);

		public :
			ResourcesTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~ResourcesTab(void);

			ImagesCollection*	getResourcesManagerLink(void);
	};

#endif
