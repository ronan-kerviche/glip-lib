#ifndef __GLIPSTUDIO_LIBRARY_INTERFACE__
#define __GLIPSTUDIO_LIBRARY_INTERFACE__

	#include "GLIPLib.hpp"
	#include "ressourcesTab.hpp"
	#include <QtGui>

	class LibraryInterface : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout			layout;
			QTabWidget			tabs;
			RessourcesTab			ressourceTab;
			QTreeWidget			uniformsTab;
			QListWidget			compilationTab;
			
			LayoutLoader			pipelineLoader;
			Pipeline*			mainPipeline;
			bool 				lastComputeSucceeded;
			
			void cleanCompilationTab(bool writeNoPipeline);
			void compilationSucceeded(void);
			void compilationFailed(Exception& e);

		private slots :
			void compute(void);
					
		public : 
			LibraryInterface(QWidget *parent=NULL);
			~LibraryInterface(void);

			bool hasOutput(void) const;
			HdlTexture& currentOutput(void);
			void compile(const std::string& code, const std::string& path);
			
		signals : 
			void requireRedraw(void);
	};

#endif
