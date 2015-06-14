#ifndef __GLIP_STUDIO__
#define __GLIP_STUDIO__

	#include "GLSceneWidget.hpp"
	#include "ImageItem.hpp"
	#include "CodeEditor.hpp"
	#include "PipelineManager.hpp"
	#include <QApplication>

	class GlipStudio : public QApplication
	{
		Q_OBJECT

		private :
			QVGL::GlipViewWidget			*window;
			QVGL::VariablesTrackerSubWidget		*variableTracker;
			QGED::CodeEditorTabsSubWidget		*editor;
			QGIC::ImageItemsCollectionSubWidget	*collection;
			QGPM::PipelineManagerSubWidget		*pipelineManager;

			void loadStyleSheet(void);
			void loadFonts(void);

		public :
			GlipStudio(int& argc, char** argv);
			~GlipStudio(void);

			bool notify(QObject* receiver, QEvent* event);
	};

#endif

