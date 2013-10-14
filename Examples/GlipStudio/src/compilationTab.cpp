#include "compilationTab.hpp"

	CompilationTab::CompilationTab(ControlModule& _masterModule, QWidget* parent)
	 : Module(_masterModule, parent), layout(this), data(this)
	{
		layout.addWidget(&data);

		data.setAlternatingRowColors(true);

		QFont font;
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		data.setFont(font);

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

	void CompilationTab::pipelineWasCreated(void)
	{
		cleanCompilationTab(false);

		// Add OK message : 
		data.addItem("Compilation succeeded...");
		data.item(0)->setFont(QFont("", -1, -1, true));
	}

	void CompilationTab::pipelineCompilationFailed(Exception& e)
	{
		cleanCompilationTab(false);

		// Add errors : 
		e.hideHeader(true);
		std::string line;
		std::istringstream stream(e.what());

		while( std::getline(stream, line) )
			data.addItem( line.c_str() );
	}

