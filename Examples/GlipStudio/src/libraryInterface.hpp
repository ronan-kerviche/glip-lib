#ifndef __GLIPSTUDIO_LIBRARY_INTERFACE__
#define __GLIPSTUDIO_LIBRARY_INTERFACE__

	#include "GLIPLib.hpp"
	#include "ressourcesTab.hpp"
	#include "compilationTab.hpp"
	#include "uniformsTab.hpp"
	#include <QtGui>

	class LibraryInterface : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout			layout;
			QTabWidget			tabs;
			RessourcesTab			ressourceTab;			
			CompilationTab			compilationTab;
			UniformsTab			uniformsTab;
			
			LayoutLoader			pipelineLoader;
			Pipeline*			mainPipeline;
			bool 				lastComputeSucceeded;

		private slots :
			void compute(void);
			void updateUniforms(void);
					
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
