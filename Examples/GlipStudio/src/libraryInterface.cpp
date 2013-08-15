#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// LibraryInterface :
	LibraryInterface::LibraryInterface(QWidget *parent)
	 : QWidget(parent), layout(this), tabs(this), resourceTab(this), compilationTab(this), uniformsTab(this), mainPipeline(NULL), lastComputeSucceeded(false)
	{
		// Layout : 
		tabs.addTab(&resourceTab, 	"   Resources   ");
		tabs.addTab(&compilationTab,	"   Compilation   ");
		tabs.addTab(&uniformsTab, 	"   Uniforms   ");
		layout.addWidget(&tabs);

		// Connections : 
			QObject::connect(&resourceTab,		SIGNAL(outputChanged(void)),		this,	SIGNAL(requireRedraw(void)));
			QObject::connect(&resourceTab,		SIGNAL(updatePipelineRequest(void)),	this,	SLOT(compute(void)));
			QObject::connect(&resourceTab,		SIGNAL(saveOutput(int)),		this,	SLOT(saveOutput(int)));
			QObject::connect(&resourceTab,		SIGNAL(copyOutputAsNewResource(int)),	this,	SLOT(copyOutputToResources(int)));
			QObject::connect(&uniformsTab,		SIGNAL(requestDataUpdate(void)),	this,	SLOT(updateUniforms(void)));
			QObject::connect(&uniformsTab,		SIGNAL(requestDataLoad()),		this,	SLOT(loadUniforms(void)));
			QObject::connect(&uniformsTab,		SIGNAL(requestDataSave()),		this,	SLOT(saveUniforms(void)));
			QObject::connect(&uniformsTab,		SIGNAL(requestPipeline()),		this,	SLOT(needPipeline(void)));
	}

	LibraryInterface::~LibraryInterface(void)
	{
		delete mainPipeline;
		mainPipeline = NULL;
	}

	void LibraryInterface::updateComputeStatus(bool status)
	{
		lastComputeSucceeded = status;
		resourceTab.updateLastComputingStatus(status);
	}

	void LibraryInterface::compute(void)
	{
		if(mainPipeline!=NULL)
		{
			updateComputeStatus(false);

			// Check that all of the input have a connection : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(!resourceTab.isInputConnected(k))
				{
					emit requireRedraw();
					return ;
				}
			}

			// Apply : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
				(*mainPipeline) << resourceTab.input(k);

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

	void LibraryInterface::loadUniforms(void)
	{
		if(mainPipeline!=NULL)
			if( uniformsTab.loadData(*mainPipeline) )
				compute();
	}

	void LibraryInterface::saveUniforms(void)
	{
		if(mainPipeline!=NULL)
			uniformsTab.saveData(*mainPipeline);
	}

	void LibraryInterface::needPipeline(void)
	{
		if(mainPipeline!=NULL)
			if( uniformsTab.takePipeline(*mainPipeline) )
				compute();
	}

	void LibraryInterface::saveOutput(int id)
	{
		if(mainPipeline!=NULL)
		{
			if(id>=0 && id<mainPipeline->getNumOutputPort() && lastComputeSucceeded)
			{
				// Save output with resources : 
				//std::cout << "Save output " << id << " : " << mainPipeline->out(id).getWidth() << 'x' << mainPipeline->out(id).getHeight() << std::endl;
				resourceTab.saveOutputToFile( mainPipeline->out(id) );
			}
		}
	}

	void LibraryInterface::copyOutputToResources(int id)
	{
		if(mainPipeline!=NULL)
		{
			if(id>=0 && id<mainPipeline->getNumOutputPort() && lastComputeSucceeded)
			{
				// Save output with resources : 
				//std::cout << "Copy output " << id << " : " << mainPipeline->out(id).getWidth() << 'x' << mainPipeline->out(id).getHeight() << std::endl;
				resourceTab.copyOutputAsNewResource( mainPipeline->out(id) );
			}
		}
	}

	bool LibraryInterface::hasOutput(void) const
	{
		// Resourcetab must have an output and either it is not using the pipeline output or it is and the last computation succeeded 
		return resourceTab.hasOutput() && (!resourceTab.outputIsPartOfPipelineOutputs() || lastComputeSucceeded); 
	}

	HdlTexture& LibraryInterface::currentOutput(void)
	{
		HdlTexture* ptr = resourceTab.getOutput(mainPipeline);

		if(ptr!=NULL)
			return *ptr;
		else
			throw Exception("LibraryInterface::currentOutput - Internal error : no currently associated texture.", __FILE__, __LINE__);
	}

	void LibraryInterface::compile(const std::string& pathToCode, const std::vector<std::string>& paths)
	{
		if( !uniformsTab.prepareUpdate(mainPipeline) )
			return ; // Refresh rejected ...

		if(pathToCode.empty())
			return ; //Nothing to do...

		bool success = true;
		
		pipelineLoader.clearPaths();
		pipelineLoader.addToPaths(paths);

		try
		{
			delete mainPipeline;
			mainPipeline = NULL;

			// Put the formats : 
			pipelineLoader.clearRequiredElements();
			resourceTab.appendFormats(pipelineLoader);
			
			// Load : 
			mainPipeline = pipelineLoader(pathToCode, "");
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
			resourceTab.updatePipelineInfos(mainPipeline);
			uniformsTab.updatePipeline(*mainPipeline);
			compute();

			// Test writer : 
			//LayoutWriter writer;
			//writer.writeToFile(*mainPipeline, "dump.ppl");
		}
		else
		{
			resourceTab.updatePipelineInfos();
			uniformsTab.updatePipeline();
			updateComputeStatus(false);

			// require a failed redraw (clear) : 
			emit requireRedraw();
		}
	}

