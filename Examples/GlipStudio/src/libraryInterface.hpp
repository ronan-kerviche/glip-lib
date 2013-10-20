#ifndef __GLIPSTUDIO_LIBRARY_INTERFACE__
#define __GLIPSTUDIO_LIBRARY_INTERFACE__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	#include "resourcesTab.hpp"
	#include "compilationTab.hpp"
	#include "ioTab.hpp"
	#include "uniformsTab.hpp"
	
	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	class LibraryInterface : public Module
	{
		Q_OBJECT

		private : 
			QVBoxLayout			layout;
			QTabWidget			tabs;
			ResourcesTab			resourceTab;			
			CompilationTab			compilationTab;
			IOTab				ioTab;
			UniformsTab			uniformsTab;

		private slots :
			void pipelineCompilationFailed(Exception& e);
					
		public : 
			LibraryInterface(ControlModule& _masterModule, QWidget *parent);
			~LibraryInterface(void);
	};

#endif
