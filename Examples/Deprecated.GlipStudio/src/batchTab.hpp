#ifndef __GLIPSTUDIO_BATCHTAB__
#define __GLIPSTUDIO_BATCHTAB__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	#include "imagesCollection.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class BatchTab : public Module 
	{
		Q_OBJECT

		private :
			std::vector<int>	imageRecordIDs;

			QHBoxLayout		inputsOutpusOptions,
						fileBar,
						processProgressBar;
			QLabel			inputsLabel,
						outputsLabel,	
						pathLabel;		
			QComboBox		inputsBox,
						outputsBox,
						outputsNameFormat,
						formatsBox;
			QPushButton		changePathButton,
						runButton;
			QLineEdit		pathLine;
			QProgressBar		progressBar;
			
			QVBoxLayout		layout;
			QMenuBar		menuBar;
			ImagesCollection	collection;
			ViewManager* 		viewManager;

			bool isValidTexture(int recordID);
			HdlTexture& getTexture(int recordID);
			void giveTextureInformation(int recordID, std::string& name);
			void cleanRecordDependances(int recordID);
			bool canBeClosed(void);

		private slots :
			void pipelineWasCreated(void);
			void pipelineInputWasModified(int portID);
			void pipelineInputWasReleased(int portID);
			void pipelineWasDestroyed(void);
			void focusChanged(int recordID);
			void selectionChanged(void);
			void imageLoaded(int recordID);
			void imageSettingsChanged(int recordID);
			void imageUnloadedFromDevice(int recordID);
			void imageFreed(int recordID);
			void createNewView(void);

		public :
			BatchTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~BatchTab(void);
	};

#endif

