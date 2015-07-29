/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-STUDIO                                                                                               */
/*     IDE for the OpenGL Image Processing LIBrary                                                               */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : GlipStudio.hpp                                                                            */
/*     Original Date : August 1th 2012                                                                           */
/*                                                                                                               */
/*     Description   : Core objects.                                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

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
			QGPM::ModulesDocumentationSubWidget	*modulesDocumentation;
			QGPM::PipelineManagerSubWidget		*pipelineManager;

			void loadStyleSheet(void);
			void loadFonts(void);

		public :
			GlipStudio(int& argc, char** argv);
			~GlipStudio(void);

			bool notify(QObject* receiver, QEvent* event);
	};

#endif

