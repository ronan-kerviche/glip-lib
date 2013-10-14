#ifndef __GLIPSTUDIO_COMPILATIONTAB__
#define __GLIPSTUDIO_COMPILATIONTAB__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	
	#include <QListWidget>
	#include <QVBoxLayout>

	using namespace Glip;

	class CompilationTab : public Module
	{
		Q_OBJECT

		private : 
			QVBoxLayout	layout;
			QListWidget	data;

			void cleanCompilationTab(bool writeNoPipeline=false);
			
		private slots : 
			void pipelineWasCreated(void);
			void pipelineCompilationFailed(Exception& e);

		public : 
			CompilationTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~CompilationTab(void);
	};

#endif

