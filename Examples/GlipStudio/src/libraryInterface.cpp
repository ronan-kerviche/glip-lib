#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// LibraryInterface :
	LibraryInterface::LibraryInterface(QWidget *parent)
	 : QWidget(parent), layout(this), tabs(this), ressourceTab(this), uniformsTab(this), mainPipeline(NULL), lastComputeSucceeded(false)
	{
		// Layout : 
		tabs.addTab(&ressourceTab, 	"Ressources");
		tabs.addTab(&compilationTab,	"Compilation");
		tabs.addTab(&uniformsTab, 	"Uniforms");
		layout.addWidget(&tabs);

		// Compilation : 
			cleanCompilationTab(true);
			compilationTab.setAlternatingRowColors(true);

			QFont font;
			font.setFamily("Monospace");
			font.setFixedPitch(true);
			compilationTab.setFont(font);

		// Connections : 
			QObject::connect(&ressourceTab,		SIGNAL(outputChanged(void)),		this,	SIGNAL(requireRedraw(void)));
			QObject::connect(&ressourceTab,		SIGNAL(updatePipelineRequest(void)),	this,	SLOT(compute(void)));
	}

	LibraryInterface::~LibraryInterface(void)
	{
		delete mainPipeline;
		mainPipeline = NULL;
	}

	void LibraryInterface::cleanCompilationTab(bool writeNoPipeline)
	{
		while(compilationTab.count()>0)
		{
			QListWidgetItem* item = compilationTab.takeItem(0);
			delete item;
		}

		if(writeNoPipeline)
		{
			compilationTab.addItem("No Pipeline...");
			compilationTab.item(0)->setFont(QFont("", -1, -1, true));
		}
	}
		
	void LibraryInterface::compilationSucceeded(void)
	{
		cleanCompilationTab(false);

		// Add OK message : 
		compilationTab.addItem("Compilation succeeded...");
		compilationTab.item(0)->setFont(QFont("", -1, -1, true));
	}

	void LibraryInterface::compilationFailed(Exception& e)
	{
		cleanCompilationTab(false);

		// Add errors : 
		e.hideHeader(true);
		std::string line;
		std::istringstream stream(e.what());
		while( std::getline(stream, line) )
			compilationTab.addItem( line.c_str() );

		// Switch to error tab
		tabs.setCurrentWidget(&compilationTab);
	}

	void LibraryInterface::compute(void)
	{
		if(mainPipeline!=NULL)
		{
			lastComputeSucceeded = false;

			// Check that all of the input have a connection : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(!ressourceTab.isInputConnected(k))
					return ;
			}

			// Apply : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
				(*mainPipeline) << ressourceTab.input(k);

			(*mainPipeline) << Pipeline::Process;

			lastComputeSucceeded = true;

			// Need redraw : 
			emit requireRedraw();
		}
	}

	bool LibraryInterface::hasOutput(void) const
	{
		// Ressourcetab must have an output and either it is not using the pipeline output or it is and the last computation succeeded 
		return ressourceTab.hasOutput() && (!ressourceTab.outputIsPartOfPipelineOutputs() || lastComputeSucceeded); 
	}

	HdlTexture& LibraryInterface::currentOutput(void)
	{
		HdlTexture* ptr = ressourceTab.getOutput(mainPipeline);

		if(ptr!=NULL)
			return *ptr;
		else
			throw Exception("LibraryInterface::currentOutput - Internal error : no currently associated texture.", __FILE__, __LINE__);
	}

	void LibraryInterface::compile(const std::string& code, const std::string& path)
	{
		if(code.empty())
			return ; //Nothing to do...

		bool success = true;

		pipelineLoader.setPath(path);

		try
		{
			delete mainPipeline;
			mainPipeline = NULL;

			// Put the formats : 
			pipelineLoader.clearRequiredElements();
			
			/*for(int k=0; k<formats.size(); k++)
				pipelineLoader.addRequiredElement(formats[k].getName().toStdString(), formats[k]);*/

			// Load : 
			mainPipeline = pipelineLoader(code, "MainPipeline");
		}
		catch(Exception& e)
		{
			success = false;

			// Info : 
			compilationFailed(e);

			delete mainPipeline;
			mainPipeline = NULL;
		}

		if(success)
		{
			compilationSucceeded();
			ressourceTab.updatePipelineInfos(mainPipeline);
			compute();
		}
		else
			ressourceTab.updatePipelineInfos();

		/*if(success)
		{
			// Make sure we can track the inputs : 
			while(preferredConnections.size()<mainPipeline->getNumInputPort())
				preferredConnections.push_back(NULL);
			
			compilationSucceeded();

			updateInputConnectionDisplay();

			// Expand : 
			ressourceTab.topLevelItem(RessourceInputs)->setExpanded(true);

			// Update output list : 
			updateOutputConnectionDisplay();

			// Expand : 
			ressourceTab.topLevelItem(RessourceOutputs)->setExpanded(true);

			// Update the connection menu : 
			connectionMenu.update(*mainPipeline);

			// Check the compute : 
			compute(true);
		}
		else
		{
			// Clean outputs and inputs :
			updateInputConnectionDisplay();
			updateOutputConnectionDisplay();

			connectionMenu.clearHub();

			emit requireRedraw();
		}*/
	}

