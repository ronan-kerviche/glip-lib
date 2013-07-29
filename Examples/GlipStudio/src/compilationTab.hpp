#ifndef __GLIPSTUDIO_COMPILATIONTAB__
#define __GLIPSTUDIO_COMPILATIONTAB__

	#include "GLIPLib.hpp"
	
	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	#include <QListWidget>
	#include <QVBoxLayout>

	using namespace Glip;

	class CompilationTab : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout	layout;
			QListWidget	data;

			void cleanCompilationTab(bool writeNoPipeline=false);

		public : 
			CompilationTab(QWidget* parent=NULL);
			~CompilationTab(void);
	
		public slots : 
			void compilationSucceeded(void);
			void compilationFailed(Exception& e);		
	};

#endif

