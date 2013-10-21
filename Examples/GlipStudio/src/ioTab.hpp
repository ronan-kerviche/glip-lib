#ifndef __GLIPSTUDIO_IO_TAB__
#define __GLIPSTUDIO_IO_TAB__

	#include "dataModules.hpp"
	#include "imagesCollection.hpp"

	class IOTab : public Module 
	{
		Q_OBJECT

		private : 
			bool			pipelineOutputOnDisplay;
			QVBoxLayout		layout;
			QLabel			pipelineStatusLabel;
			QMenuBar		inputMenuBar,
						outputMenuBar;
			TexturesList		inputsList,
						outputsList;
			std::vector<int>	inputRecordIDs,
						outputRecordIDs;

			int getInputPortIDFromRecordID( int recordID );
			int getOutputPortIDFromRecordID( int recordID );

		private slots :
			// Pipeline events : 
			void pipelineWasCreated(void);
			void pipelineWasComputed(void);
			void pipelineComputationFailed(Exception& e);
			void pipelineInputWasModified(int portID);
			void pipelineInputWasReleased(int portID);
			void pipelineWasDestroyed(void);

			// Show inputs and outputs : 
			void inputSelectionChanged(void);
			void outputSelectionChanged(void);

		public :
			IOTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~IOTab(void);
	};

#endif
