#ifndef __GLIPSTUDIO_MAIN_INTERFACE__
#define __GLIPSTUDIO_MAIN_INTERFACE__

	#include "GLIPLib.hpp"
	#include "WindowRendering.hpp"
	#include "codeEditor.hpp"
	#include "libraryInterface.hpp"
	#include <QtGui>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class MainWindow : public QWidget	
	{
		Q_OBJECT

		private : 
			QVBoxLayout		mainLayout;
			QSplitter		mainSplitter;
			QWidget			container;
			QVBoxLayout		containerLayout;
			QSplitter		secondarySplitter;
			WindowRendererContainer display;
			LibraryInterface	libraryInterface;
			CodeEditorsPannel 	codeEditors;

			LayoutLoader		pipelineLoader;
			Pipeline*		mainPipeline;

			void closeEvent(QCloseEvent *event);

		private slots :
			void refreshPipeline(void);
			void updateOutput(void);

		public : 
			MainWindow(void);
			~MainWindow(void);
	};

	class GlipStudio : public QApplication
	{
		Q_OBJECT

		private : 
			MainWindow *mainWindow;

		public : 
			GlipStudio(int& argc, char** argv);
			~GlipStudio(void); 
	};

#endif
