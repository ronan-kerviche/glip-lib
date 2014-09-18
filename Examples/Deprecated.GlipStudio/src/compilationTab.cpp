#include "compilationTab.hpp"

// ModuleDocumentation :
	ModuleDocumentation::ModuleDocumentation(QWidget* parent)
	 :	Window(parent),
		layout(this),
		title("Module : "),
		comboBox(this),			
		description(this, false)
	{
		frame.titleBar().setWindowTitle("Module Documentation");

		title.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		title.setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );

		comboBox.setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum) );
		comboBox.setEditable(false);

		moduleChoiceLine.addWidget(&title);
		moduleChoiceLine.addWidget(&comboBox);

		layout.addLayout(&moduleChoiceLine);
		layout.addWidget(&description);
	
		description.setReadOnly(true);

		connect(&comboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateDocumentationDisplay(const QString&)));

		setMinimumWidth(512);
	}

	ModuleDocumentation::~ModuleDocumentation(void)
	{ }

	void ModuleDocumentation::updateDocumentationDisplay(const QString& moduleName)
	{
		int id = moduleNames.indexOf(moduleName);

		if(id>=0)
			description.setPlainText( tr("%1\nMANUAL :\n%2").arg(moduleInfo[id], moduleManuals[id]) );
	}

	bool ModuleDocumentation::isDocumented(const QString& moduleName) const
	{
		return moduleNames.contains(moduleName);
	}

	bool ModuleDocumentation::isEmpty(void) const
	{
		return moduleNames.empty();
	}

	void ModuleDocumentation::update(const LayoutLoader& loader)
	{
		std::vector<std::string> modulesNamesList = loader.listModules();

		for(std::vector<std::string>::iterator it=modulesNamesList.begin(); it!=modulesNamesList.end(); it++)
		{
			if(!isDocumented( (*it).c_str() ))
			{
				const LayoutLoaderModule& module = loader.module(*it);

				// Create the data : 
				QString info;
	
					if(module.getMinNumArguments()==0)
						info +=	tr("Minimum number of arguments : No arguments allowed.\n");
					else if(module.getMinNumArguments()<0)
						info +=	tr("Minimum number of arguments : Unlimited.\n");
					else
						info +=	tr("Minimum number of arguments : %1\n").arg(module.getMinNumArguments());

					if(module.getMaxNumArguments()==0)
						info +=	tr("Maximum number of arguments : No arguments allowed.\n");
					else if(module.getMaxNumArguments()<0)
						info +=	tr("Maximum number of arguments : Unlimited.\n");
					else
						info +=	tr("Maximum number of arguments : %1\n").arg(module.getMaxNumArguments());

					if(module.bodyPresenceTest()<0)
						info +=	tr("Body                        : Cannot have a body.\n");
					else if(module.bodyPresenceTest()==0)
						info +=	tr("Body                        : Body is optional.\n");
					else
						info +=	tr("Body                        : Must have a body.\n");

				// Append : 
				moduleNames.append( module.getName().c_str() );
				moduleInfo.append( info );
				moduleManuals.append( module.getManual().c_str() );

				// Combo box : 
				comboBox.addItem( module.getName().c_str() );
			}
		}
	}

// Compilation Tab : 
	CompilationTab::CompilationTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this),
		data(this),
		openSaveInterface("CompilationPannel", "File", "*.ppl, *.txt")
	{
		openSaveInterface.enableOpen(false);
		openSaveInterface.enableSave(false);

		showDocumentationAction = menuBar.addAction("Modules Documentation", this, SLOT(showDocumentation()));
		showDocumentationAction->setEnabled(false);

		dumpPipelineCodeAction	= menuBar.addAction("Save Pipeline Code As...", this, SLOT(dumpPipelineCode()));
		dumpPipelineCodeAction->setEnabled(false);

		layout.addWidget(&menuBar);
		layout.addWidget(&data);

		data.setAlternatingRowColors(true);

		int fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Regular.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		QFontDatabase db;
		data.setFont( db.font("Source Code Pro", "Regular", data.font().pointSize()) );

		cleanCompilationTab(true);

		updateDocumentation(_masterModule.loader());
	}

	CompilationTab::~CompilationTab(void)
	{
		cleanCompilationTab(false);
	}

	void CompilationTab::updateDocumentation(const LayoutLoader& loader)
	{
		documentation.update(loader);
	
		if(documentation.isEmpty())
		{
			documentation.hide();
			showDocumentationAction->setEnabled(false);
		}
		else
			showDocumentationAction->setEnabled(true);
	}

	void CompilationTab::cleanCompilationTab(bool writeNoPipeline)
	{
		while(data.count()>0)
		{
			QListWidgetItem* item = data.takeItem(0);
			delete item;
		}

		if(writeNoPipeline)
		{
			data.addItem("No Pipeline...");
			data.item(0)->setFont(QFont("", -1, -1, true));
		}
	}

	void CompilationTab::preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos)
	{
		updateDocumentation(loader);
	}

	void CompilationTab::pipelineWasCreated(void)
	{
		cleanCompilationTab(false);

		// Add OK message : 
		data.addItem("Compilation succeeded...");
		data.item(0)->setFont(QFont("", -1, -1, true));

		dumpPipelineCodeAction->setEnabled(true);
	}

	void CompilationTab::pipelineCompilationFailed(const Exception& e)
	{
		cleanCompilationTab(false);

		// Add errors : 
		Exception err = e;
		err.hideHeader(true);
		std::string line;
		std::istringstream stream(err.what());

		while( std::getline(stream, line) )
			data.addItem( line.c_str() );

		dumpPipelineCodeAction->setEnabled(false);
	}

	void CompilationTab::showDocumentation(void)
	{
		documentation.show();
	}

	void CompilationTab::dumpPipelineCode(void)
	{
		if(pipelineExists())
		{
			QString filename = openSaveInterface.saveAsDialog(tr("Save Raw Pipeline Code for %1").arg(pipeline().getFullName().c_str()));

			if(!filename.isEmpty())
			{
				// Get the code : 
				LayoutWriter writer;

				writer.writeToFile(pipeline(), filename.toStdString());
			}
		}
	}

	void CompilationTab::closeEvent(QCloseEvent *event)
	{
		documentation.close();
	}

