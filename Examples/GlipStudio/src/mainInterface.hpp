#ifndef __GLIPSTUDIO_MAIN_INTERFACE__
#define __GLIPSTUDIO_MAIN_INTERFACE__

	#include "GLIPLib.hpp"
	#include "settingsManager.hpp"
	#include "dataModules.hpp"
	#include "codePannel.hpp"
	#include "libraryInterface.hpp"

	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class MainWindow : public ControlModule
	{
		Q_OBJECT

		private : 
			QSplitter		mainSplitter;
			QWidget			container;
			QVBoxLayout		containerLayout;
			QSplitter		secondarySplitter;

			LibraryInterface	libraryInterface;
			CodeEditorsPannel 	codeEditors;

			void closeEvent(QCloseEvent *event);

		public : 
			MainWindow(void);
			~MainWindow(void);
	};

	class GlipStudio : public QApplication
	{
		Q_OBJECT

		private : 
			SettingsManager* settingsManager;
			MainWindow *mainWindow;

		public : 
			GlipStudio(int& argc, char** argv);
			~GlipStudio(void); 

			virtual bool notify(QObject* receiver, QEvent* event);
	};

#endif
