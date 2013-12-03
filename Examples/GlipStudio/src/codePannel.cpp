#include <cmath>
#include "codePannel.hpp"
#include "settingsManager.hpp"
//#include "keywords.hpp"

// PathWidget
	PathWidget::PathWidget(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), addPathAct(tr("Add path"), this), removePathAct(tr("Remove path"), this), clearAllPathAct(tr("Clear all paths"), this)
	{
		connect(&addPathAct, 		SIGNAL(triggered()), this, SLOT(addPath()));
		connect(&removePathAct, 	SIGNAL(triggered()), this, SLOT(removePath()));
		connect(&clearAllPathAct, 	SIGNAL(triggered()), this, SLOT(clearAll()));

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

// CodeEditorsPannel
	CodeEditorsPannel::CodeEditorsPannel(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent), 
		layout(this), menuBar(this), 
		widgets(this), 
		fileMenu("File", this), 
		openSaveInterface("CodePannel", "File", "*.ppl *.glsl *.ext *.uvd *.txt"),
		templateMenu(this), 
	  	newTabAct(tr("&New tab"), this),
		saveAllAct(tr("Save all"), this),
		refreshAct("Compile", this),
		closeTabAct(tr("&Close"), this),
		showPathWidget(tr("Paths"),this),
		pathWidget(this),
		closeAllAct(tr("Close all"), this),
		aboutAct(tr("About"), this)
	{
		// Add the actions : 
		newTabAct.setShortcuts(QKeySequence::New);
		openSaveInterface.enableShortcuts(true);

		connect(&newTabAct, SIGNAL(triggered()), this, SLOT(newTab()));
		connect(&openSaveInterface, SIGNAL(openFile(const QStringList&)), this, SLOT(open(const QStringList&)));
		connect(&openSaveInterface, SIGNAL(saveFile(const QString&)), this, SLOT(save(const QString&)));
		connect(&openSaveInterface, SIGNAL(saveFileAs(const QString&)), this, SLOT(saveAs(const QString&)));
		connect(&saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));
		connect(&closeAllAct, SIGNAL(triggered()), this, SLOT(closeAll()));
		connect(&templateMenu, SIGNAL(insertTemplate()), this, SLOT(insertTemplate()));

		QList<QKeySequence> refreshShortcuts;
		refreshShortcuts.push_back(QKeySequence(tr("Ctrl+r")));
		refreshShortcuts.push_back(QKeySequence(tr("F5")));
		refreshAct.setShortcuts(refreshShortcuts);
		connect(&refreshAct, SIGNAL(triggered()), this, SLOT(refresh()));

		closeTabAct.setStatusTip(tr("Close"));
		closeTabAct.setShortcuts(QKeySequence::Close);
		connect(&closeTabAct, SIGNAL(triggered()), this, SLOT(closeTab()));

		showPathWidget.setStatusTip(tr("Show paths"));
		connect(&showPathWidget, SIGNAL(triggered()), this, SLOT(switchPathWidget()));

		connect(&aboutAct, SIGNAL(triggered()), this, SLOT(aboutMessage()));

		connect(&widgets, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

		// Movable : 	
		widgets.setMovable(true);

		// Menus :
		fileMenu.addAction(&newTabAct);
		openSaveInterface.addToMenu(fileMenu);
		fileMenu.addAction(&saveAllAct);
		fileMenu.addAction(&closeTabAct);
		fileMenu.addAction(&closeAllAct);
	
		menuBar.addMenu(&fileMenu);
		menuBar.addMenu(&templateMenu);
		menuBar.addAction(&refreshAct);
		menuBar.addAction(&showPathWidget);
		menuBar.addAction(&aboutAct);

		// Add the first tab : 
		newTab();

		// Hide paths : 
		pathWidget.hide();

		layout.addWidget(&menuBar);	
		layout.addWidget(&widgets);
		layout.addWidget(&pathWidget);

		// TODO : improve this in a mini tutorial : 
		SettingsManager settings;
		if( settings.isFirstTimeRun() )
			aboutAct.trigger();
	}

	CodeEditorsPannel::~CodeEditorsPannel(void)
	{
		/*while(widgets.count()>0)
			widgets.removeTab(0);

		for(int i=0; i<tabs.size(); i++)
			delete tabs[i];

		tabs.clear();*/
	}

	void CodeEditorsPannel::newTab(void)
	{
		CodeEditor* newEditor = new CodeEditor(this);
		widgets.addTab(newEditor, newEditor->getTitle());
		widgets.setCurrentWidget( newEditor );

		connect(newEditor, SIGNAL(titleChanged()), this, SLOT(updateTitle()));
	}

	void CodeEditorsPannel::open(const QStringList& filenames)
	{
		for(int k=0; k<filenames.count(); k++)
			openFile( filenames[k] );
	}

	void CodeEditorsPannel::save(const QString& filename)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

			e->setFilename(filename);
			e->save();
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
		}
	}

	void CodeEditorsPannel::saveAll(void)
	{
		CodeEditor* original = reinterpret_cast<CodeEditor*>(widgets.currentWidget());

		for(int k=0; k<widgets.count(); k++)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(k));

			if(e->filename().isEmpty())
			{
				widgets.setCurrentWidget(e);

				QString currentPath = e->path();

				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pipeline Script File"), currentPath, tr("Pipeline Script Files (*.ppl *.glsl *.ext *.uvd)"));

				if(!fileName.isEmpty())
				{
					e->setFilename(fileName);
					e->save();
				}
			}
			else
			{
				e->save();
				widgets.setCurrentWidget(e);
			}
		}

		if(widgets.count() > 0)
			widgets.setCurrentWidget(original);
	}

	void CodeEditorsPannel::refresh(void)
	{
		if(widgets.count() > 0)
		{
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
		if(pathWidget.isVisible())
		{
			pathWidget.hide();
			showPathWidget.setText("Paths");			
		}
		else
		{
			pathWidget.show();
			showPathWidget.setText("Hide paths");
		}
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

	void CodeEditorsPannel::aboutMessage(void)
	{
		int pointSize = font().pointSize();

		QMessageBox::about(this, "GlipStudio", tr("<center><p style=\"font-family: times, serif; font-size:%2pt; font-style:bold\">GlipStudio</p></center><p style=\"font-family: times, serif; font-size:%1pt; font-style:bold\">GlipStudio is the IDE for GlipLib (OpenGL Image Processing Library). Find more information, documentation and examples at : <a href='http://glip-lib.sourceforge.net/'>http://glip-lib.sourceforge.net/</a>.</p><center><p style=\"font-size:%3pt; font-style:italic\">Copyright &copy; 2013, Ronan Kerviche, MIT License</p></center><p style=\"font-size:%4pt;\"> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: <BR> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. <BR> THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p><table><tr><td><b><i>Binary build date</i></b> : </td><td>%5; %6</td></tr><tr><td><b><i>Hardware vendor : </i></b></td><td>%7</td></tr><tr><td><b><i>Renderer : </i></b></td><td>%8</td></tr><tr><td><b><i>OpenGL version : </i></b></td><td>%9</td></tr><tr><td><b><i>GLSL version : </i></b></td><td>%10</td></tr></table>").arg(pointSize).arg(pointSize+4).arg(pointSize+1).arg(pointSize-2).arg(__DATE__).arg(__TIME__).arg(HandleOpenGL::getVendorName().c_str()).arg(HandleOpenGL::getRendererName().c_str()).arg(HandleOpenGL::getVersion().c_str()).arg(HandleOpenGL::getGLSLVersion().c_str()));
	}

	void CodeEditorsPannel::tabChanged(int c)
	{
		if(widgets.count() > 0)
		{
			CodeEditor* e = reinterpret_cast<CodeEditor*>(widgets.widget(c));

			if(!e->filename().isEmpty())
			{
				openSaveInterface.reportSuccessfulSave( e->filename() );
				openSaveInterface.enableSave(true);
			}
			else
				openSaveInterface.enableSave(false);

			openSaveInterface.enableSaveAs(true);
		}
		else
		{
			openSaveInterface.enableSave(false);
			openSaveInterface.enableSaveAs(false);
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

				// Save as new file : 
				openSaveInterface.reportSuccessfulLoad( filename );
				openSaveInterface.reportSuccessfulSave( filename );
				openSaveInterface.enableSave(true);
			}
		}
	}
