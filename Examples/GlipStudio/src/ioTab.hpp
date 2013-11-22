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
			TexturesList		portsList;
			std::vector<int>	inputRecordIDs,
						outputRecordIDs;

			ViewManager		*inputsViewManager,
						*outputsViewManager;

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
			/*void inputSelectionChanged(void);
			void outputSelectionChanged(void);*/
			void focusChanged(int recordID);
			void selectionChanged(void);

			void newInputView(void);
			void newOutputView(void);

		public :
			IOTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~IOTab(void);
	};

#endif
