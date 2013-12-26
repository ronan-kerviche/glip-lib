#include <cmath>
#include "codePannel.hpp"
#include "settingsManager.hpp"
//#include "keywords.hpp"

// PathWidget
	PathWidget::PathWidget(QWidget* parent)
	 : Window(parent), layout(this), menuBar(this), addPathAct(tr("Add path"), this), removePathAct(tr("Remove path"), this), clearAllPathAct(tr("Clear all paths"), this)
	{
		frame.titleBar().setWindowTitle("Include Paths");

		frame.setMinimumWidth(768);

		connect(&addPathAct, 		SIGNAL(triggered()), 		this, SLOT(addPath()));
		connect(&removePathAct, 	SIGNAL(triggered()), 		this, SLOT(removePath()));
		connect(&clearAllPathAct, 	SIGNAL(triggered()), 		this, SLOT(clearAll()));
		connect(&data,			SIGNAL(itemSelectionChanged()),	this, SLOT(update()));

		removePathAct.setEnabled(false);
		menuBar.addAction(&addPathAct);
		menuBar.addAction(&removePathAct);
		menuBar.addAction(&clearAllPathAct);

		data.setSelectionMode(QAbstractItemView::ExtendedSelection);
		data.setAlternatingRowColors(true);

		/*QFont font;
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		data.setFont(font);*/
		QFontDatabase db;
		data.setFont( db.font("Source Code Pro", "Regular", data.font().pointSize()) );

		layout.addWidget(&menuBar);
		layout.addWidget(&data);

		// Load path : 
		clearAll();	

		SettingsManager settingsManager;

		Element m = settingsManager.getModuleData("PathWidget", "NumPaths");
		int numPaths = 0;

		if(m.arguments.size()==1)
		{
			if(!from_string(m.arguments.front(), numPaths))
				numPaths = 0;
		}

		for(int k=0; k<numPaths; k++)
		{
			Element e = settingsManager.getModuleData("PathWidget", "Paths_" + to_string(k));

			if(!e.body.empty())
				addPath( e.getCleanBody() );
		}
	}

	PathWidget::~PathWidget(void)
	{
		SettingsManager settingsManager;

		Element m = settingsManager.getModuleData("PathWidget", "NumPaths");
		m.arguments.assign(1, to_string(paths.size()));
		settingsManager.setModuleData("PathWidget", "NumPaths", m);
		
		for(int k=0; k<paths.size(); k++)
		{
			const std::string name = "Paths_" + to_string(k);
			Element e = settingsManager.getModuleData("PathWidget", name);
			e.body = paths[k];
			settingsManager.setModuleData("PathWidget", name, e);
		}

		clearAll();
	}

	void PathWidget::update(void)
	{
		QList<QListWidgetItem*> selectedItems = data.selectedItems();

		removePathAct.setDisabled( selectedItems.isEmpty() );
	}

	void PathWidget::addPath(void)
	{
		QString directoryName = QFileDialog::getExistingDirectory(this, tr("Add directory to paths"), ".");

		if(!directoryName.isEmpty())
		{
			QString slashed = QDir::fromNativeSeparators( directoryName ) + "/";
		
			addPath( slashed.toStdString() );
		}
	}

	void PathWidget::removePath(void)
	{
		QList<QListWidgetItem*> selectedItems = data.selectedItems();
		
		if( !selectedItems.isEmpty() )
		{
			for(int k=0; k<selectedItems.count(); k++)
			{
				int p = data.row(selectedItems[k]);

				if(p>0)
				{
					paths.erase( paths.begin() + p );
					QListWidgetItem* item = data.takeItem(p);
					delete item;
				}
			}
		}
	}

	void PathWidget::clearAll(void)
	{
		paths.clear();

		// Clean the list : 
		while(data.count()>0)
		{
			QListWidgetItem* item = data.takeItem(0);
			delete item;
		}

		// Add : 
		addPath("./");
	}

	void PathWidget::addPath(std::string newPath)
	{
		if(newPath.empty())
			return ;

		// Cut : 
		size_t p = newPath.find_last_not_of(" \t\r\n");
		
		if(p!=std::string::npos)
			newPath = newPath.substr(0, p+1);

		if(newPath[newPath.size()-1]!='/')
			newPath += '/';

		// Find if a double exists : 
		std::vector<std::string>::iterator it = std::find(paths.begin(), paths.end(), newPath);

		if(it==paths.end())
		{
			paths.push_back( newPath );
			data.addItem( newPath.c_str() );
		}
	}
	
	const std::vector<std::string>& PathWidget::getPaths(void) const
	{
		return paths;
	}

// TemplateMenu : 
	const char* TemplateMenu::templatesName[numTemplates]		= { 	"Add path",
										"Include file",
										"Required format", 
										"Required pipeline", 
										"Texture format",
										"Geometry + StandardQuad",
										"Geometry + Grid2D",
										"Geometry + Grid3D",
										"Geometry + Custom Model",
										"Shared source", 
										"Shader source + GLSL fragment",
										"Shader source + GLSL vertex",
										"Include shared source",
										"Include shader",
										"Filter layout", 
										"Pipeline layout", 
										"Main pipeline layout",
										"Main pipeline indirection",
										"Input ports", 
										"Output ports", 
										"Filter instance",
										"Pipeline instance", 
										"Connection",
										"Full Pipeline structure",
										"Full MainPipeline structure"
									};

	const char* TemplateMenu::templatesCode[numTemplates]		= {	"ADD_PATH( )\n",
										"INCLUDE_FILE( )\n",
										"REQUIRED_FORMAT: ( )\n",
										"REQUIRED_PIPELINE: ( )\n",
										"TEXTURE_FORMAT: ( , , GL_RGB, GL_UNSIGNED_BYTE)\n",
										"GEOMETRY: (STANDARD_QUAD)\n",
										"GEOMETRY: (GRID_2D, , )\n",
										"GEOMETRY: (GRID_3D, , , )\n",
										"GEOMETRY: (CUSTOM_MODEL, , )\n{\n\tVERTEX( x, y, z, u, v)\n\t\n\tELEMENT(a, b, c, d)\n}\n",
										"SHARED_SOURCE: \n{\n\t\n}\n",
										"SHADER_SOURCE: \n{\n\t#version 130\n\t\n\tuniform sampler2D ;\n\tout vec4 ;\n\t\n\tvoid main()\n\t{\n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\tvec4 col = textureLod( , pos, 0);\n\t\n\t\t = col;\n\t}\n}\n",
										"SHADER_SOURCE: \n{\n\t#version 130\n\t\n\tvoid main()\n\t{\n\t\tgl_FrontColor = gl_Color;\n\n\t\tgl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n\t\tgl_Position = ftransform();\n\t}\n}\n",
										"INCLUDE_SHARED_SOURCE:\n",
										"SHADER_SOURCE: ()\n",
										"FILTER_LAYOUT: ( , )\n",
										"PIPELINE_LAYOUT: \n{\n\t\n}\n",
										"PIPELINE_MAIN: \n{\n\t\n}\n",
										"PIPELINE_MAIN: ()\n",
										"INPUT_PORTS( )\n",
										"OUTPUT_PORTS( )\n",
										"FILTER_INSTANCE: ( )\n",
										"PIPELINE_INSTANCE: ( )\n",
										"CONNECTION( , , , )\n",
										"PIPELINE_LAYOUT: \n{\n\tINPUT_PORTS( )\n\tOUTPUT_PORTS( )\n\n\tFILTER_INSTANCE: ( )\n\n\tCONNECTION( , , , )\n\tCONNECTION( , , , )\n}\n",
										"PIPELINE_MAIN: \n{\n\tINPUT_PORTS( )\n\tOUTPUT_PORTS( )\n\n\tFILTER_INSTANCE: ( )\n\n\tCONNECTION( , , , )\n\tCONNECTION( , , , )\n}\n"
									};

	const char* TemplateMenu::templatesCodeWithHelp[numTemplates]	= {	"ADD_PATH( /* path */ )\n",
										"INCLUDE_FILE( /* filename */ )\n",
										"REQUIRED_FORMAT: /* name to use */( /* required format name */ )\n",
										"REQUIRED_PIPELINE: /* name to use */ ( /* required pipeline name */ )\n",
										"TEXTURE_FORMAT: /* name */ ( /* width */, /* height */, GL_RGB, GL_UNSIGNED_BYTE)\n",
										"GEOMETRY: /* name */ (STANDARD_QUAD)\n",
										"GEOMETRY: /* name */ (GRID_2D, /* interger : width */, /* interger : height */)\n",
										"GEOMETRY: /* name */ (GRID_3D, /* interger : width */, /* interger : height */, /* interger : depth */)\n",
										"GEOMETRY: /* name */ (CUSTOM_MODEL, /* GL primitive (GL_POINTS, GL_LINES, GL_TRIANGLES, etc.) */, /* It has texcoord embedded? true or false */)\n{\n\tVERTEX( x, y, z, u, v) /* depending on the model format */\n\t\n\tELEMENT(a, b, c, d) /* depending on the model format */\n}\n",
										"SHADER_SOURCE: /* name */\n{\n\t/* Code */\n}\n",
										"SHADER_SOURCE: /* name */\n{\n\t#version 130\n\t\n\tuniform sampler2D /* input texture name */;\n\tout vec4 /* output texture name */;\n\t\n\t// uniform vec3 someVariableYouWantToModify = vec3(0.0, 0.0, 0.0);\n\t\n\tvoid main()\n\t{\n\t\t// The current fragment position : \n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\t// Read the base level of the texture at the current position : \n\t\tvec4 col = textureLod(/* input texture name */, pos, 0);\n\t\n\t\t/* output texture name */ = col;\n\t}\n}\n",
										"SHADER_SOURCE: /* name */\n{\n\t#version 130\n\t\n\tvoid main()\n\t{\n\t\tgl_FrontColor = gl_Color;\n\n\t\tgl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n\t\tgl_Position = ftransform();\n\t}\n}\n",
										"INCLUDE_SHARED_SOURCE:/* shared source name */\n",
										"SHADER_SOURCE: /* name */ ( /* filename */ )\n",
										"FILTER_LAYOUT: /* name */ ( /* output texture format name */, /* shader name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\t/* structure code */\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\t/* structure code */\n}\n",
										"PIPELINE_MAIN: /* name */ ( /* other pipeline name */ )\n",
										"INPUT_PORTS( /* ports names list */ )\n",
										"OUTPUT_PORTS( /* ports names list */ )\n",
										"FILTER_INSTANCE: /* instance name */ ( /* filter layout name */ )\n",
										"PIPELINE_INSTANCE: /* instance name */ ( /* pipeline layout name */ )\n",
										"CONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNETION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNETION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n"
									};

	TemplateMenu::TemplateMenu(QWidget* parent)
	 : QMenu("Insert Template", parent), signalMapper(this),  addComments("Option : insert comments", this), lastInsertionID(tplUnknown)
	{
		addComments.setCheckable(true);
		addAction(&addComments);

		// Init : 
		for(int k=0; k<numTemplates; k++)
		{
			templatesActions[k] = new QAction( QString(templatesName[k]), this );
			addAction(templatesActions[k]);
			signalMapper.setMapping(templatesActions[k], k);

			connect(templatesActions[k], SIGNAL(triggered()), &signalMapper, SLOT(map()));
		}

		connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(insertTemplateCalled(int)));

		addComments.setChecked(true);
	}

	TemplateMenu::~TemplateMenu(void) 
	{
		for(int k=0; k<numTemplates; k++)
			delete templatesActions[k];
	}

	void TemplateMenu::insertTemplateCalled(int k)
	{
		lastInsertionID = static_cast<CodeTemplates>(k);
		emit insertTemplate();
	}

	QString TemplateMenu::getTemplateCode(void)
	{
		if(lastInsertionID>=0 && lastInsertionID<numTemplates)
		{
			QString res;

			if(addComments.isChecked())
				res = templatesCodeWithHelp[lastInsertionID];
			else
				res = templatesCode[lastInsertionID];

			lastInsertionID = tplUnknown;
			return res;
		}
		else
		{
			lastInsertionID = tplUnknown;
			return "";
		}
	}

// ElementsMenu
	ElementsMenu::ElementsMenu(QWidget* parent)
	 : QMenu("Elements", parent), timer(this)
	{
		updateMenu();

		connect(&timer, SIGNAL(timeout()), this, SIGNAL(updateElements()));

		timer.setInterval(15 * 1000);
		timer.start();
	}

	ElementsMenu::~ElementsMenu(void)
	{
		clear();
	}

	void ElementsMenu::insertCalled(void)
	{
		QAction* sender = reinterpret_cast<QAction*>( QObject::sender() );

		emit insertElement(sender->toolTip());
	}

	void ElementsMenu::updateMenu(void)
	{
		if(menus.empty())
		{
			clear();
			addAction("No elements")->setEnabled(false);
		}		
		else
		{
			QList<QAction*> actionsList = actions();
			for(int k=0; k<actionsList.count(); k++)
				removeAction( actionsList[k] );

			for(std::map<CodeEditor*, QMenu*>::iterator it=menus.begin(); it!=menus.end(); it++)
				addMenu(it->second);
		}
	}
	
	void ElementsMenu::scan(CodeEditor* editor, LayoutLoader::PipelineScriptElements& elements)
	{
		std::map<CodeEditor*, QMenu*>::iterator it = menus.find(editor);

		if(!editor->empty())
		{
			QMenu* menu = new QMenu(editor->getTitle());

			menu->addAction(tr("Include %1").arg(editor->getRawTitle()), this, SLOT(insertCalled()))->setToolTip(tr("INCLUDE_FILE(%1)\n").arg(editor->getRawTitle()));

			if(!editor->path().isEmpty())
				menu->addAction(tr("Add path..."), this, SLOT(insertCalled()))->setToolTip(tr("ADD_PATH(%1)\n").arg(editor->path()));

			// Load : 
				#define MAKE_LIST( listName, name ) \
					if(!elements. listName .empty()) \
					{ \
						QMenu* tmp = menu->addMenu(tr( name " (%1)").arg(elements. listName .size())); \
						\
						for(int k=0; k<elements. listName .size(); k++) \
							tmp->addAction(elements. listName [k].c_str(), this, SLOT(insertCalled()))->setToolTip(elements. listName [k].c_str()); \
	 				}

				MAKE_LIST( requiredFormats, 	"Required Formats" );
				MAKE_LIST( requiredGeometries,	"Required Geometries" );
				MAKE_LIST( requiredPipelines,	"Required Pipelines" );
				MAKE_LIST( formats,		"Formats" );
				MAKE_LIST( shaderSources,	"Shader Sources" );
				MAKE_LIST( geometries,		"Geometries" );
				MAKE_LIST( filtersLayout,	"Filter Layouts");

				#undef MAKE_LIST

				// Pipelines : 
				if(!elements.pipelines.empty())
				{
					QMenu* tmp = menu->addMenu(tr("Pipelines (%1)").arg(elements.pipelines.size()));

					for(int k=0; k<elements.pipelines.size(); k++)
					{
						tmp->addAction(elements.pipelines[k].c_str(), this, SLOT(insertCalled()))->setToolTip(elements.pipelines[k].c_str());

						QMenu* i = tmp->addMenu(tr("Inputs of %1 (%2)").arg(elements.pipelines[k].c_str()).arg(elements.pipelineInputs[k].size()));

						for(int ki=0; ki<elements.pipelineInputs[k].size(); ki++)
							i->addAction(elements.pipelineInputs[k][ki].c_str(), this, SLOT(insertCalled()))->setToolTip(elements.pipelineInputs[k][ki].c_str());

						QMenu* o = tmp->addMenu(tr("Outputs of %1 (%2)").arg(elements.pipelines[k].c_str()).arg(elements.pipelineOutputs[k].size()));

						for(int ko=0; ko<elements.pipelineOutputs[k].size(); ko++)
							o->addAction(elements.pipelineOutputs[k][ko].c_str(), this, SLOT(insertCalled()))->setToolTip(elements.pipelineOutputs[k][ko].c_str());
					}
				}

				if(!elements.mainPipeline.empty())
				{
					QMenu* tmp = menu->addMenu("Main Pipeline");

					tmp->addAction(elements.mainPipeline.c_str(), this, SLOT(insertCalled()))->setToolTip(elements.mainPipeline.c_str());

					QMenu* i = tmp->addMenu(tr("Inputs of %1 (%2)").arg(elements.mainPipeline.c_str()).arg(elements.mainPipelineInputs.size()));

					for(int ki=0; ki<elements.mainPipelineInputs.size(); ki++)
						i->addAction(elements.mainPipelineInputs[ki].c_str(), this, SLOT(insertCalled()))->setToolTip(elements.mainPipelineInputs[ki].c_str());

					QMenu* o = tmp->addMenu(tr("Outputs of %1 (%2)").arg(elements.mainPipeline.c_str()).arg(elements.mainPipelineOutputs.size()));

					for(int ko=0; ko<elements.mainPipelineOutputs.size(); ko++)
						o->addAction(elements.mainPipelineOutputs[ko].c_str(), this, SLOT(insertCalled()))->setToolTip(elements.mainPipelineOutputs[ko].c_str());
				}

			// Test and update : 
			if(menu->actions().isEmpty())
			{
				// clear : 
				remove(editor);

				// clean : 
				delete menu;
			}
			else if(it!=menus.end())
			{
				removeAction(it->second->menuAction());
				delete it->second;

				it->second = menu;
			}
			else
				menus[editor] = menu;

			updateMenu();
		}
	}

	void ElementsMenu::remove(CodeEditor* editor)
	{
		std::map<CodeEditor*, QMenu*>::iterator it = menus.find(editor);

		if(it!=menus.end())
		{
			removeAction(it->second->menuAction());
			delete it->second;
			menus.erase(it);
		}
	}

// CodeEditorsPannel
	CodeEditorsPannel::CodeEditorsPannel(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent), 
		layout(this), menuBar(this), 
		widgets(this), 
		fileMenu("File", this), 
		openSaveInterface("CodePannel", "File", "*.ppl *.glsl *.ext *.uvd *.txt"),
		templateMenu(this), 
		elementsMenu(this),
	  	newTabAction(tr("&New tab"), this),
		saveAllAction("Save all", this),
		refreshAction("Compile", this),
		closeTabAction(tr("&Close"), this),
		showPathWidgetAction("Paths",this),
		pathWidget(this),
		showEditorSettingsAction("Editor Settings", this),
		closeAllAction("Close all", this),
		aboutAction("About", this)
	{
		// Add the actions : 
		newTabAction.setShortcuts(QKeySequence::New);
		openSaveInterface.enableShortcuts(true);

		connect(&newTabAction, 			SIGNAL(triggered()), 			this, SLOT(newTab()));
		connect(&openSaveInterface, 		SIGNAL(openFile(const QStringList&)), 	this, SLOT(open(const QStringList&)));
		connect(&openSaveInterface, 		SIGNAL(saveFile(void)), 		this, SLOT(save(void)));
		connect(&openSaveInterface, 		SIGNAL(saveFileAs(const QString&)), 	this, SLOT(saveAs(const QString&)));
		connect(&saveAllAction, 		SIGNAL(triggered()), 			this, SLOT(saveAll()));
		connect(&closeAllAction, 		SIGNAL(triggered()), 			this, SLOT(closeAll()));
		connect(&templateMenu, 			SIGNAL(insertTemplate()), 		this, SLOT(insertTemplate()));
		connect(&elementsMenu,			SIGNAL(updateElements()),		this, SLOT(updateElements()));
		connect(&elementsMenu,			SIGNAL(insertElement(const QString&)),	this, SLOT(insertElement(const QString&)));

		QKeySequence qs(Qt::CTRL + Qt::Key_R);
		refreshAction.setShortcut(qs);
		refreshAction.setText(refreshAction.text() + " (" + qs.toString() + ")");
		connect(&refreshAction, 		SIGNAL(triggered()), 			this, SLOT(refresh()));

		closeTabAction.setStatusTip(tr("Close"));
		closeTabAction.setShortcuts(QKeySequence::Close);
		connect(&closeTabAction, 		SIGNAL(triggered()), 			this, SLOT(closeTab()));

		showPathWidgetAction.setStatusTip(tr("Show paths"));
		connect(&showPathWidgetAction, 		SIGNAL(triggered()), 			this, SLOT(switchPathWidget()));

		connect(&showEditorSettingsAction,	SIGNAL(triggered()), 			this, SLOT(showEditorSettings()));
		connect(&aboutAction, 			SIGNAL(triggered()), 			this, SLOT(aboutMessage()));

		connect(&widgets, 			SIGNAL(currentChanged(int)), 		this, SLOT(tabChanged(int)));

		// Movable : 	
		widgets.setMovable(true);

		// Menus :
		fileMenu.addAction(&newTabAction);
		openSaveInterface.addToMenu(fileMenu);
		fileMenu.addAction(&saveAllAction);
		fileMenu.addAction(&closeTabAction);
		fileMenu.addAction(&closeAllAction);
	
		menuBar.addMenu(&fileMenu);
		menuBar.addMenu(&templateMenu);
		menuBar.addMenu(&elementsMenu);
		menuBar.addAction(&refreshAction);
		menuBar.addAction(&showPathWidgetAction);
		menuBar.addAction(&showEditorSettingsAction);
		menuBar.addAction(&aboutAction);

		// Add the first tab : 
		newTab();

		// Hide paths : 
		pathWidget.hide();

		layout.addWidget(&menuBar);	
		layout.addWidget(&widgets);

		// TODO : improve this in a mini tutorial : 
		SettingsManager settings;
		if( settings.isFirstTimeRun() )
			aboutAction.trigger();
	}

	CodeEditorsPannel::~CodeEditorsPannel(void)
	{
		editorSettings.close();
	}

	void CodeEditorsPannel::newTab(void)
	{
		CodeEditor* newEditor = new CodeEditor(this);
		newEditor->addSubMenu(&templateMenu);
		newEditor->addSubMenu(&elementsMenu);
		widgets.addTab(newEditor, newEditor->getTitle());
		widgets.setCurrentWidget( newEditor );
		widgets.setCurrentTabTextColor( QColor("#BBBBBB") );

		connect(newEditor, SIGNAL(titleChanged()), this, SLOT(updateTitle()));
	}

	void CodeEditorsPannel::open(const QStringList& filenames)
	{
		for(int k=0; k<filenames.count(); k++)
			openFile( filenames[k] );
	}

	void CodeEditorsPannel::save(void)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			if(e->filename().isEmpty())
			{
				QString filename = openSaveInterface.saveAsDialog();
	
				if(!filename.isEmpty())			
					e->setFilename(filename);
				else
					return ;
			}

			e->save();

			openSaveInterface.reportSuccessfulSave(e->filename());

			updateCurrentToolTip();
		}
	}

	void CodeEditorsPannel::save(const QString& filename)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			e->setFilename(filename);
			e->save();

			openSaveInterface.reportSuccessfulSave(filename);

			updateCurrentToolTip();
		}
	}

	void CodeEditorsPannel::saveAs(const QString& filename)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			e->setFilename(filename);
			e->save();

			openSaveInterface.reportSuccessfulSave(filename);
			openSaveInterface.enableSave(true);

			updateCurrentToolTip();
		}
	}

	void CodeEditorsPannel::saveAll(void)
	{
		CodeEditor* original = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

		for(int k=0; k<widgets.count(); k++)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(k));

			widgets.setCurrentWidget(e);
			save();
		}

		if(widgets.count() > 0)
			widgets.setCurrentWidget(original);
	}

	void CodeEditorsPannel::refresh(void)
	{
		if(widgets.count()>0)
		{
			// Update colors : 
			widgets.setTabsTextColor(QColor("#BBBBBB") );
				
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());
			if(!e->empty())
				widgets.setCurrentTabTextColor(Qt::white);

			// Send code : 
			requirePipelineCreation(getCurrentCode() + "\n");
		}
	}

	void CodeEditorsPannel::closeTab(void)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			if(e->canBeClosed())
			{
				elementsMenu.remove(e);
				widgets.removeTab(widgets.indexOf(e));
				delete e;
			}
		}
	}

	void CodeEditorsPannel::closeAll(void)
	{
		for(int k=0; k<widgets.count(); k++)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(k));

			if(e->canBeClosed())
			{
				widgets.removeTab(widgets.indexOf(e));
				delete e;
			}
		}
	}

	void CodeEditorsPannel::switchPathWidget(void)
	{
		pathWidget.show();
	}

	void CodeEditorsPannel::updateTitle(void)
	{
		CodeEditor* e = reinterpret_cast<CodeEditor*>( QObject::sender() );

		widgets.setTabText( widgets.indexOf(e), tr("   %1   ").arg(e->getTitle()));
	}

	void CodeEditorsPannel::insertTemplate(void)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			e->insert( templateMenu.getTemplateCode() );
		}
	}

	void CodeEditorsPannel::showEditorSettings(void)
	{
		editorSettings.show();
	}

	void CodeEditorsPannel::aboutMessage(void)
	{
		int pointSize = font().pointSize();

		QMessageBox::about(this, "GlipStudio", tr("<center><p style=\"font-family: times, serif; font-size:%2pt; font-style:bold\">GlipStudio</p></center><p style=\"font-family: times, serif; font-size:%1pt; font-style:bold\">GlipStudio is the IDE for GlipLib (OpenGL Image Processing Library). Find more information, documentation and examples at : <a href='http://glip-lib.sourceforge.net/'>http://glip-lib.sourceforge.net/</a>.</p><center><p style=\"font-size:%3pt; font-style:italic\">Copyright &copy; 2013, Ronan Kerviche, MIT License</p></center><p style=\"font-size:%4pt;\"> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: <BR> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. <BR> THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p><table><tr><td><b><i>Binary build date</i></b> : </td><td>%5; %6</td></tr><tr><td><b><i>Hardware vendor : </i></b></td><td>%7</td></tr><tr><td><b><i>Renderer : </i></b></td><td>%8</td></tr><tr><td><b><i>OpenGL version : </i></b></td><td>%9</td></tr><tr><td><b><i>GLSL version : </i></b></td><td>%10</td></tr></table>").arg(pointSize).arg(pointSize+4).arg(pointSize+1).arg(pointSize-2).arg(__DATE__).arg(__TIME__).arg(HandleOpenGL::getVendorName().c_str()).arg(HandleOpenGL::getRendererName().c_str()).arg(HandleOpenGL::getVersion().c_str()).arg(HandleOpenGL::getGLSLVersion().c_str()));
	}

	void CodeEditorsPannel::tabChanged(int c)
	{
		if(widgets.count() > 0)
		{
			openSaveInterface.enableSave(true);
			saveAllAction.setEnabled(true);
		}
		else
		{
			openSaveInterface.enableSave(false);
			saveAllAction.setEnabled(false);
		}	
	}

	void CodeEditorsPannel::updateElements(void)
	{
		for(int k=0; k<widgets.count(); k++)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(k));

			if(e->isModifiedTrigger())
				updateElementsOfEditor(e);
		}
	}

	void CodeEditorsPannel::insertElement(const QString& element)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			e->insert( element );
		}
	}

	std::string CodeEditorsPannel::getCurrentFilename(void)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			return e->filename().toStdString();
		}
		else
			return "";
	}

	std::string CodeEditorsPannel::getCurrentCode(void)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			return e->currentContent();
		}
		else
			return "";
	}

	void CodeEditorsPannel::updateCurrentToolTip(void)
	{
		CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

		QString toolTip = "<table>";
			toolTip += tr("<tr><td><i>Filename</i></td><td>:</td><td>%1</td></tr>").arg(e->getTitle());
			toolTip += tr("<tr><td><i>Path</i></td><td>:</td><td>%1</td></tr>").arg(e->path());
		toolTip += "</table>";

		widgets.setTabToolTip(widgets.currentIndex(), toolTip);
	}

	const std::vector<std::string>& CodeEditorsPannel::getPaths(void)
	{
		// Make sure the list of path contains the one of the current pipeline :
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			pathWidget.addPath( e->path().toStdString() );
		}

		return pathWidget.getPaths();
	}

	void CodeEditorsPannel::preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos)
	{
		// Add path : 
		loader.clearPaths();
		loader.addToPaths( getPaths() );
	}

	bool CodeEditorsPannel::canBeClosed(void)
	{
		bool test = true;

		for(int k=0; k<widgets.count() && test; k++)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(k));

			widgets.setCurrentWidget(e);
			test = test && e->canBeClosed();	
		}

		return test;
	}

	void CodeEditorsPannel::openFile(const QString& filename)
	{
		QFileInfo info( filename );

		if(info.exists())
		{
			if(widgets.count() > 0)
			{
				CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());
				if(!e->empty())
					newTab();
			}
			else
				newTab();

			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());
	
			e->setFilename( filename );
			if(!e->load())
				closeTab();
			else
			{
				// Append the path : 
				pathWidget.addPath( e->path().toStdString() );

				// Report : 
				openSaveInterface.reportSuccessfulLoad( filename );
				openSaveInterface.enableSave(true);
				updateCurrentToolTip();

				// Scan : 
				updateElementsOfEditor(e);
			}
		}
	}

	void CodeEditorsPannel::closeEvent(QEvent* event)
	{
		pathWidget.close();
		editorSettings.close();

		event->accept();
	}

	void CodeEditorsPannel::close(void)
	{
		pathWidget.close();
		editorSettings.close();

		QWidget::close();
	}

	void CodeEditorsPannel::updateElementsOfEditor(CodeEditor* e)
	{
		try
		{
			LayoutLoader::PipelineScriptElements elements = scan(e->currentContent() + "\n");

			elementsMenu.scan(e, elements);
		}
		catch(Exception& err)
		{
			elementsMenu.remove(e);
		}
	}

