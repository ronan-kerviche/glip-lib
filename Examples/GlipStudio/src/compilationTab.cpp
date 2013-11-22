#include "compilationTab.hpp"

	CompilationTab::CompilationTab(ControlModule& _masterModule, QWidget* parent)
	 : Module(_masterModule, parent), layout(this), data(this)
	{
		layout.addWidget(&data);

		data.setAlternatingRowColors(true);

		/*QFont font;
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		data.setFont(font);*/

		int fid = QFontDatabase::addApplicationFont("Fonts/SourceCodePro-Regular.ttf");
		if( fid < 0)
			std::cerr << "Could not locate the font!" << std::endl;

		QFontDatabase db;
		data.setFont( db.font("Source Code Pro", "Regular", data.font().pointSize()) );

		cleanCompilationTab(true);
	}

	CompilationTab::~CompilationTab(void)
	{
		cleanCompilationTab(false);
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
		/// TODO : update list of modules available in loader...
		/*std::vector<std::string> modulesList = loader.listModules();

		std::cout << "List of modules : " << std::endl; 
		for(int k=0; k<modulesList.size(); k++)
		{
			std::cout << "- " << modulesList[k] << std::endl;
			std::cout <<  loader.module( modulesList[k] ).getManual() << std::endl << std::endl;
		}
		std::cout << "End of modules list." << std::endl;*/
	}

	void CompilationTab::pipelineWasCreated(void)
	{
		cleanCompilationTab(false);

		// Add OK message : 
		data.addItem("Compilation succeeded...");
		data.item(0)->setFont(QFont("", -1, -1, true));
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
	}

