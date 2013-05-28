#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// LibraryInterface :
	LibraryInterface::LibraryInterface(QWidget *parent)
	 : QWidget(parent), layout(this), tabs(this), ressourceTab(this), compilationTab(this), uniformsTab(this), mainPipeline(NULL), lastComputeSucceeded(false)
	{
		// Layout : 
		tabs.addTab(&ressourceTab, 	"Ressources");
		tabs.addTab(&compilationTab,	"Compilation");
		tabs.addTab(&uniformsTab, 	"Uniforms");
		layout.addWidget(&tabs);

		// Connections : 
			QObject::connect(&ressourceTab,		SIGNAL(outputChanged(void)),		this,	SIGNAL(requireRedraw(void)));
			QObject::connect(&ressourceTab,		SIGNAL(updatePipelineRequest(void)),	this,	SLOT(compute(void)));
			QObject::connect(&ressourceTab,		SIGNAL(saveOutput(int)),		this,	SLOT(saveOutput(int)));
			QObject::connect(&uniformsTab,		SIGNAL(requestDataUpdate(void)),	this,	SLOT(updateUniforms(void)));
	}

	LibraryInterface::~LibraryInterface(void)
	{
		delete mainPipeline;
		mainPipeline = NULL;
	}

	void LibraryInterface::updateComputeStatus(bool status)
	{
		lastComputeSucceeded = status;
		ressourceTab.updateLastComputingStatus(status);
	}

	void LibraryInterface::compute(void)
	{
		if(mainPipeline!=NULL)
		{
			updateComputeStatus(false);

			// Check that all of the input have a connection : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(!ressourceTab.isInputConnected(k))
				{
					emit requireRedraw();
					return ;
				}
			}

			// Apply : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
				(*mainPipeline) << ressourceTab.input(k);

			(*mainPipeline) << Pipeline::Process;

			updateComputeStatus(true);

			// Need redraw : 
			emit requireRedraw();
		}
	}

	void LibraryInterface::updateUniforms(void)
	{
		if(mainPipeline!=NULL)
		{
			uniformsTab.updateData(*mainPipeline);

			compute();
		}
	}

	void LibraryInterface::saveOutput(int id)
	{
		if(mainPipeline!=NULL)
		{
			if(id>=0 && id<mainPipeline->getNumOutputPort() && lastComputeSucceeded)
			{
				// Save output with ressources : 
				ressourceTab.saveOutputToFile( mainPipeline->out(id) );
			}
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
			ressourceTab.appendFormats(pipelineLoader);
			
			// Load : 
			mainPipeline = pipelineLoader(code, "MainPipeline");
		}
		catch(Exception& e)
		{
			success = false;

			// Info : 
			compilationTab.compilationFailed(e);

			// Switch to error tab
			tabs.setCurrentWidget(&compilationTab);

			delete mainPipeline;
			mainPipeline = NULL;
		}

		if(success)
		{
			compilationTab.compilationSucceeded();
			ressourceTab.updatePipelineInfos(mainPipeline);
			uniformsTab.updatePipeline(*mainPipeline);
			compute();
		}
		else
		{
			ressourceTab.updatePipelineInfos();
			uniformsTab.updatePipeline();
			updateComputeStatus(false);
		}
	}

