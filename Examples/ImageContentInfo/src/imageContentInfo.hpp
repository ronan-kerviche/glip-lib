#ifndef __IMAGE_CONTENT_INFO__
#define __IMAGE_CONTENT_INFO__

	#include <iostream>
	#include <fstream>
	//#include "WindowRendering.hpp"
	#include "GLSceneWidget.hpp"
	#include "ResourceLoader.hpp"
	#include "CodeEditor.hpp"
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QWidget>
	#include <QPushButton>
	#include <QVBoxLayout>
	#include <QFileDialog>
	#include <QMessageBox>
	#include <QComboBox>

	// Tmp : 
	#include <QDebug>

	// Prototypes :
		class WindowRenderer;

	// Namespace :
		using namespace Glip;
		using namespace Glip::CoreGL;
		using namespace Glip::CorePipeline;
		using namespace Glip::Modules;

	// Object
		class IHM : public QWidget
		{
			Q_OBJECT

			private :
				std::fstream		log;
				QVBoxLayout		layout;
				QPushButton		saveButton;
				ImageLoaderInterface	imageLoaderInterface;
				PipelineLoaderInterface pipelineLoaderInterface;

				QVGL::MainWidget	window;

			public :
				IHM(void);
				~IHM(void);

			public slots :
				void save(void);
				void requestComputingUpdate(void);
				void updateOutput(void);
		};

		class ImageContentInformation : public QApplication
		{
			Q_OBJECT

			private :
				IHM	*ihm;

			public :
				ImageContentInformation(int& argc, char** argv);
				~ImageContentInformation(void);
		};

	// TMP : 
		class TmpWidget : public QVGL::SubWidget
		{
			Q_OBJECT

			private : 	
				QWidget		widget;	
				QVBoxLayout 	layout;
				QPushButton 	a, b, c;

				QImage 		img;
				HdlTexture* 	texture;
				QVGL::View*	view;

			private slots :
				void buttonAPressed(void);
				void buttonBPressed(void);
				void buttonCPressed(void);
			public :
				TmpWidget(void);
				~TmpWidget(void);
		};

		class CodeEditorSubWidget : public QVGL::SubWidget
		{	
			private : 
				QGED::MainWidget mainWidget;

			public :
				CodeEditorSubWidget(void);
				~CodeEditorSubWidget(void);
		};

#endif
