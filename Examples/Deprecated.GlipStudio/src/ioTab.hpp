#ifndef __GLIPSTUDIO_IO_TAB__
#define __GLIPSTUDIO_IO_TAB__

	#include <QLineEdit>
	#include "dataModules.hpp"
	#include "imagesCollection.hpp"

	class IOTab : public Module 
	{
		Q_OBJECT

		private : 
			QVBoxLayout		layout;
			QLineEdit		pipelineStatusLabel;
			QMenuBar		menuBar;
			QMenu			imagesMenu,
						contextMenu;
			QAction			releaseInputAction,
						copyAsNewResourceAction,
						replaceResourceAction,
						copyAsNewResourceWithNewNameAction;
			OpenSaveInterface	openSaveInterface;
			TexturesList		portsList;
			std::vector<int>	inputRecordIDs,
						outputRecordIDs;

			ViewManager		*inputsViewManager,
						*outputsViewManager;
			ImagesCollection	*resourcesManagerLink;

			int getInputPortIDFromRecordID( int recordID );
			int getOutputPortIDFromRecordID( int recordID );

		private slots :
			// Pipeline events : 
			void pipelineWasCreated(void);
			void pipelineWasComputed(void);
			void pipelineComputationFailed(const Exception& e);
			void pipelineInputWasModified(int portID);
			void pipelineInputWasReleased(int portID);
			void pipelineWasDestroyed(void);

			// Show inputs and outputs : 
			void focusChanged(int recordID);
			void selectionChanged(void);

			void newInputView(void);
			void newOutputView(void);

			void releaseInput(void);
			void copyAsNewResource(bool replace=false);
			void replaceResource(void);
			void copyAsNewResourceWithNewName(void);
			void saveOutput(void);
			void saveOutput(const QString& filename);

			void showContextMenu(const QPoint& point);	

		public :
			IOTab(ControlModule& _masterModule, ImagesCollection *_resourcesManager, QWidget* parent=NULL);
			~IOTab(void);
	};

#endif
