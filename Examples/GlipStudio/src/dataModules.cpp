#include "dataModules.hpp"

// Module
	Module::Module(ControlModule& _masterModule, QWidget* parent)
	 : QWidget(parent), masterModule(&_masterModule)	
	{
		masterModule->addClient(this);
	}

	Module::~Module(void)
	{
		if(masterModule!=NULL)
			masterModule->removeClient(this);
	}
	
	bool Module::pipelineCanBeCreated(void)			{ return true; }
	bool Module::pipelineCanBeComputed(void)		{ return true; }
	bool Module::pipelineInputsCanBeModified(void)		{ return true; }
	bool Module::pipelineUniformsCanBeModified(void)	{ return true; }
	bool Module::pipelineCanBeDestroyed(void)		{ return true; }
	bool Module::canBeClosed(void)				{ return true; }

	bool Module::requirePrepareToPipelineCreation(const std::string& code)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineCreation(code);
	}

	bool Module::requirePrepareToPipelineComputation(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineComputation();
	}

	bool Module::requirePrepareToPipelineInputsModification(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineInputsModification();
	}

	bool Module::requirePrepareToPipelineUniformsModification(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineUniformsModification();
	}

	bool Module::requirePrepareToPipelineDestruction(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineDestruction();
	}

	bool Module::requireDisplay(WindowRenderer*& display)
	{
		if(masterModule==NULL)
		{
			display = NULL;
			return false;			
		}
		else
			return masterModule->linkToDisplay(this, display);
	}

	void Module::updateDisplay(WindowRenderer& display)
	{
		display.clearWindow();
	}

	bool Module::canReleaseDisplay(void)
	{
		return true;
	}

	void Module::preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos)
	{ }

	bool Module::pipelineExists(void) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->pipelineExists();
	}

	bool Module::lastComputationWasSuccessful(void) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->lastComputationWasSuccessful();
	}

	const std::string& Module::getPipelineCode(void) const
	{
		if(pipelineExists())
			return masterModule->getPipelineCode();
		else
			return "";
	}
	
	const Pipeline& Module::pipeline(void) const
	{
		if(pipelineExists())
		 	return masterModule->pipeline();

		else
			throw Exception("Module::pipeline - No pipeline defined.", __FILE__, __LINE__);
	}

	bool Module::isThisLinkedToDisplay(void) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->testLinkToDisplay(this);
	}

	Pipeline& Module::pipeline(void)
	{
		if(pipelineExists())
			return masterModule->pipeline();
		else
			throw Exception("Module::pipeline - No pipeline defined.", __FILE__, __LINE__);
	}

	void Module::pipelineWasCreated(void)			{}
	void Module::pipelineCompilationFailed(Exception& e)	{}
	void Module::pipelineWasComputed(void)			{}
	void Module::pipelineInputsWereModified(void)		{}
	void Module::pipelineUniformsWereModified(void)		{}
	void Module::pipelineWasDestroyed(void)			{}

// ControlModule
	ControlModule::ControlModule(QWidget* parent)
	 : QWidget(parent), display(this, 640, 480), lastComputationSucceeded(false), pipelinePtr(NULL), displayClient(NULL)
	{
		QObject::connect(&(display.renderer()),	SIGNAL(actionReceived(void)), this, SLOT(displayUpdate(void)));

		LayoutLoaderModule::addBasicModules(pipelineLoader);
	}

	ControlModule::~ControlModule(void)
	{
		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end(); it++)
			(*it)->masterModule = NULL;		

		clients.clear();

		delete pipelinePtr;
		pipelinePtr = NULL;
		pipelineCode.clear();
		displayClient = NULL;
	}

	// Private slots : 
	void ControlModule::displayUpdate(void)
	{
		if(displayClient==NULL)
			display.renderer().clearWindow();
		else
			displayClient->updateDisplay( display.renderer() );
	}

	// Tools : 
	void ControlModule::addClient(Module* m)
	{
		// Push to the list : 
		clients.push_back(m);

		// Connect the outputs : 
		QObject::connect( this, SIGNAL(pipelineWasCreated()), 			m, SLOT(pipelineWasCreated()) );
		QObject::connect( this, SIGNAL(pipelineCompilationFailed(Exception&)),	m, SLOT(pipelineCompilationFailed(Exception&)) );
		QObject::connect( this, SIGNAL(pipelineWasComputed()), 			m, SLOT(pipelineWasComputed()) );
		QObject::connect( this, SIGNAL(pipelineInputsWereModified()), 		m, SLOT(pipelineInputsWereModified()) );
		QObject::connect( this, SIGNAL(pipelineUniformsWereModified()), 	m, SLOT(pipelineUniformsWereModified()) );
		QObject::connect( this, SIGNAL(pipelineWasDestroyed()), 		m, SLOT(pipelineWasDestroyed()) );

		// Connect the inputs :
		QObject::connect( m, SIGNAL(pipelineComputation(bool)),			this, SLOT(pipelineComputation(bool)) );
		QObject::connect( m, SIGNAL(pipelineInputsModification()),		this, SLOT(pipelineInputsModification()) );
		QObject::connect( m, SIGNAL(pipelineUniformModification()),		this, SLOT(pipelineUniformModification()) );
	}

	void ControlModule::removeClient(Module* m)
	{
		std::vector<Module*>::iterator it = std::find(clients.begin(), clients.end(), m);

		if(it!=clients.end())
			clients.erase(it);

		releaseDisplayLink(m);
	}
	
	bool ControlModule::linkToDisplay(Module* m, WindowRenderer*& displayPtr)
	{
		if(displayClient!=NULL)
		{
			if(displayClient->canReleaseDisplay())
			{
				displayClient = m;
				displayPtr = &display.renderer();
				return true;
			}
			else
			{
				displayPtr = NULL;
				return false;
			}
		}
		else
		{
			displayClient = m;
			displayPtr = &display.renderer();
			return true;
		}
	}

	bool ControlModule::testLinkToDisplay(const Module* m) const
	{
		return displayClient==m;
	}

	void ControlModule::releaseDisplayLink(Module* m)
	{
		if(displayClient==m)
		{
			displayClient = NULL;
			displayUpdate();
		}
	}

	bool ControlModule::pipelineExists(void) const
	{
		return pipelinePtr!=NULL;
	}

	bool ControlModule::lastComputationWasSuccessful(void) const
	{
		return pipelineExists() && lastComputationSucceeded;
	}

	const std::string& ControlModule::getPipelineCode(void) const
	{
		if(pipelineExists())
			return pipelineCode;
		else
			return "";
	}

	const Pipeline& ControlModule::pipeline(void) const
	{
		if(pipelineExists())
			return *pipelinePtr;
		else
			throw Exception("ControlModule::pipeline - No pipeline in use.", __FILE__, __LINE__);
	}

	Pipeline& ControlModule::pipeline(void)
	{
		if(pipelineExists())
			return *pipelinePtr;
		else
			throw Exception("ControlModule::pipeline - No pipeline in use.", __FILE__, __LINE__);
	}

	bool ControlModule::requirePrepareToPipelineCreation(const std::string& code)
	{
		// First, is there a removal : 
		if(pipelineExists())
		{
			if(!requirePrepareToPipelineDestruction())
				return false; // If the previous pipeline destruction was aborted...
		}

		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeCreated();
	
		if(poll)
		{
			// Create the pipeline : 
			try
			{
				// Delete previous pipeline : 
				delete pipelinePtr;
				pipelinePtr			= NULL;
				lastComputationSucceeded	= false;
				
				// Copy the code : 
				pipelineCode 			= code;

				// First step : get informations.
				pipelineLoader.clearRequiredElements();
				LayoutLoader::PipelineScriptElements infos = pipelineLoader.listElements(pipelineCode);	

				// Ask all modules to prepare the loader : 
				for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
					(*it)->preparePipelineLoading(pipelineLoader, infos);

				// Compile :
				pipelinePtr = pipelineLoader(pipelineCode, "");

				// Update the bits : 
				lastComputationSucceeded	= true;

				// Propagate the information : 
				emit pipelineWasCreated();
			}
			catch(Exception& e)
			{
				// Manage exception
				emit pipelineCompilationFailed(e);

				// Clear out : 
				delete pipelinePtr;
				pipelinePtr			= NULL;
				pipelineCode.clear();
			}

			return true;
		}
		else
			return false;
	}

	bool ControlModule::requirePrepareToPipelineComputation(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeComputed();

		return poll;
	}

	bool ControlModule::requirePrepareToPipelineInputsModification(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineInputsCanBeModified();

		return poll;
	}

	bool ControlModule::requirePrepareToPipelineUniformsModification(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineUniformsCanBeModified();

		return poll;
	}

	bool ControlModule::requirePrepareToPipelineDestruction(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeDestroyed();

		if(poll)
		{
			delete pipelinePtr;
			pipelinePtr 			= NULL;
			lastComputationSucceeded	= false;
			pipelineCode.clear();
		}
	}

	bool ControlModule::requireClose(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->canBeClosed();

		return poll;
	}

	void ControlModule::pipelineComputation(bool success)
	{
		lastComputationSucceeded = success;

		emit pipelineWasComputed();
	}

	void ControlModule::pipelineInputsModification(void)
	{
		emit pipelineInputsWereModified();
	}

	void ControlModule::pipelineUniformModification(void)
	{
		emit pipelineUniformsWereModified();
	}

