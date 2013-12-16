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
	
	bool Module::pipelineCanBeCreated(void)					{ return true; }
	bool Module::pipelineCanBeCompiled(void)				{ return true; }
	bool Module::pipelineCanBeComputed(void)				{ return true; }
	bool Module::pipelineInputsCanBeModified(void)				{ return true; }
	bool Module::textureInputCanBeReleased(int portID, int recordID)	{ return true; }
	bool Module::pipelineUniformsCanBeModified(void)			{ return true; }
	bool Module::pipelineCanBeDestroyed(void)				{ return true; }
	bool Module::canBeClosed(void)						{ return true; }

	bool Module::requirePipelineCreation(const std::string& code)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePipelineCreation(code);
	}

	bool Module::requirePipelineComputation(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePipelineComputation();
	}

	bool Module::registerInputTexture(int recordID, int portID)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->registerInputTexture(this, recordID, portID);
	}

	void Module::unregisterInputTexture(int recordID)
	{
		if(masterModule!=NULL)
			masterModule->unregisterInputTexture(this, recordID);
	}

	void Module::releaseInputPort(int portID)
	{
		if(masterModule!=NULL)
			masterModule->releaseInputPort(portID);
	}

	bool Module::requirePrepareToPipelineUniformsModification(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePrepareToPipelineUniformsModification();
	}

	bool Module::requirePipelineDestruction(void)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->requirePipelineDestruction();
	}

	ViewLink* Module::getViewLink(void)
	{
		if(masterModule==NULL)
			return NULL;
		else
			return masterModule->getViewLink();
	}

	ViewManager* Module::getViewManager(void)
	{
		if(masterModule==NULL)
			return NULL;
		else
			return masterModule->getViewManager();
	}

	void Module::preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos)
	{ }
	
	bool Module::isValidTexture(int recordID)
	{
		return false;
	}

	HdlTexture& Module::getTexture(int recordID)
	{
		throw Exception("Module::getTexture - This Module does not have textures.", __FILE__, __LINE__);
	}

	const __ReadOnly_HdlTextureFormat& Module::getTextureFormat(int recordID) const
	{
		throw Exception("Module::getTextureFormat - This Module does not have textures.", __FILE__, __LINE__);
	}

	void Module::giveTextureInformation(int recordID, std::string& name)
	{
		name = "N.A.";
	}

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

	bool Module::isInputValid(int portID)
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->isInputValid(portID);
	}

	HdlTexture& Module::inputTexture(int portID)
	{
		if(masterModule==NULL)
			throw Exception("Module::inputTexture - No master module (internal error).", __FILE__, __LINE__);
		else
			return masterModule->inputTexture(portID);
	}

	const __ReadOnly_HdlTextureFormat&  Module::inputTextureFormat(int portID) const
	{
		if(masterModule==NULL)
			throw Exception("Module::inputTextureFormat - No master module (internal error).", __FILE__, __LINE__);
		else
			return masterModule->inputTextureFormat(portID);
	}

	void Module::getInputTextureInformation(int portID, std::string& name)
	{
		if(masterModule==NULL)
			name = "N.A.";
		else
			masterModule->getInputTextureInformation(portID, name);
	}

	bool Module::isListedAsPipelineInput(int recordID, int* portID) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->isListedAsPipelineInput(this, recordID, portID);
	}

	bool Module::isUsedAsPipelineInput(int recordID, int* portID) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->isUsedAsPipelineInput(this, recordID, portID);
	}

	bool Module::isInputPortOwner(int portID, int* recordID) const
	{
		if(masterModule==NULL)
			return false;
		else
			return masterModule->isInputPortOwner(this, portID, recordID);
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

	Pipeline& Module::pipeline(void)
	{
		if(pipelineExists())
			return masterModule->pipeline();
		else
			throw Exception("Module::pipeline - No pipeline defined.", __FILE__, __LINE__);
	}

	void Module::pipelineWasCreated(void)						{}
	void Module::pipelineCompilationFailed(const Exception& e)			{}
	void Module::pipelineWasComputed(void)						{}
	void Module::pipelineComputationFailed(const Exception& e)			{}
	void Module::pipelineInputWasModified(int portID)				{}
	void Module::pipelineInputWasReleased(int portID)				{}
	void Module::pipelineInputFromThisModuleWasReleased(int portID, int recordID)	{}
	void Module::pipelineUniformsWereModified(void)					{}
	void Module::pipelineWasDestroyed(void)						{}
	void Module::mouseParametersWereUpdated(const GLSceneWidget::MouseData& data)	{}

// ControlModule
	const int ControlModule::maxNumInputs = 256;

	ControlModule::ControlModule(void)
	 : display(640, 480, this), lastComputationSucceeded(false), pipelinePtr(NULL), displayClient(NULL)
	{
		LayoutLoaderModule::addBasicModules(pipelineLoader);

		inputTextureRecordIDs.assign(maxNumInputs, -1);
		inputTextureOwners.assign(maxNumInputs, NULL);
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

	// Tools : 
	void ControlModule::addClient(Module* m)
	{
		// Push to the list : 
		clients.push_back(m);

		// Connect the outputs : 
		QObject::connect( this, 			SIGNAL(pipelineWasCreated()), 					m, 	SLOT(pipelineWasCreated()) );
		QObject::connect( this, 			SIGNAL(pipelineCompilationFailed(const Exception&)),		m, 	SLOT(pipelineCompilationFailed(const Exception&)) );
		QObject::connect( this, 			SIGNAL(pipelineWasComputed()), 					m, 	SLOT(pipelineWasComputed()) );
		QObject::connect( this, 			SIGNAL(pipelineComputationFailed(const Exception&)),		m, 	SLOT(pipelineComputationFailed(const Exception&)) );
		QObject::connect( this, 			SIGNAL(pipelineInputWasModified(int)), 				m, 	SLOT(pipelineInputWasModified(int)) );
		QObject::connect( this, 			SIGNAL(pipelineInputWasReleased(int)), 				m, 	SLOT(pipelineInputWasReleased(int)) );
		QObject::connect( this, 			SIGNAL(pipelineUniformsWereModified()), 			m, 	SLOT(pipelineUniformsWereModified()) );
		QObject::connect( this, 			SIGNAL(pipelineWasDestroyed()), 				m, 	SLOT(pipelineWasDestroyed()) );
		QObject::connect( &display.sceneWidget(),	SIGNAL(mouseDataUpdated(const GLSceneWidget::MouseData&)),	m, 	SLOT(mouseParametersWereUpdated(const GLSceneWidget::MouseData&)));

		// Connect the inputs :
		QObject::connect( m, 		SIGNAL(pipelineUniformModification()),				this, 	SLOT(pipelineUniformModification()) );
	}

	void ControlModule::removeClient(Module* m)
	{
		std::vector<Module*>::iterator it = std::find(clients.begin(), clients.end(), m);

		if(it!=clients.end())
			clients.erase(it);
	}
	
	ViewLink* ControlModule::getViewLink(void)
	{
		return display.sceneWidget().createView();
	}

	ViewManager* ControlModule::getViewManager(void)
	{
		return display.sceneWidget().createManager();
	}

	bool ControlModule::pipelineExists(void) const
	{
		return pipelinePtr!=NULL;
	}

	bool ControlModule::lastComputationWasSuccessful(void) const
	{
		return pipelineExists() && lastComputationSucceeded;
	}
	
	bool ControlModule::isInputValid(int portID)
	{
		if(portID<0 || portID>=inputTextureOwners.size())
			return false;
		else if(inputTextureOwners[portID]!=NULL)
			return inputTextureOwners[portID]->isValidTexture( inputTextureRecordIDs[portID] );
		else
			return false;
	}

	HdlTexture& ControlModule::inputTexture(int portID)
	{
		if(inputTextureOwners[portID]==NULL)
			throw Exception("ControlModule::inputTexture - Input texture " + to_string(portID) + " does not have owner (internal error).", __FILE__, __LINE__);
		else
			return inputTextureOwners[portID]->getTexture(inputTextureRecordIDs[portID]);
	}

	const __ReadOnly_HdlTextureFormat& ControlModule::inputTextureFormat(int portID) const
	{
		if(inputTextureOwners[portID]==NULL)
			throw Exception("ControlModule::inputTextureFormat - Input texture " + to_string(portID) + " does not have owner (internal error).", __FILE__, __LINE__);
		else
			return inputTextureOwners[portID]->getTextureFormat(inputTextureRecordIDs[portID]);
	}

	void ControlModule::getInputTextureInformation(int portID, std::string& name)
	{
		if(!isInputValid(portID))
			name = "N.A.";
		else
			inputTextureOwners[portID]->giveTextureInformation(inputTextureRecordIDs[portID], name);
	}

	bool ControlModule::isListedAsPipelineInput(const Module* m, int recordID, int* portID) const
	{
		for(int k=0; k<inputTextureOwners.size(); k++)
		{
			if(inputTextureOwners[k]==m && inputTextureRecordIDs[k]==recordID)
			{
				if(portID!=NULL)
					(*portID) = k;
				return true;
			}
		}
		
		return false;
	}

	bool ControlModule::isUsedAsPipelineInput(const Module* m, int recordID, int* portID) const
	{
		int proxyPortID;
		bool test = isListedAsPipelineInput(m, recordID, &proxyPortID);

		if(test && pipelineExists())
		{
			if( proxyPortID < pipeline().getNumInputPort() )
			{
				if(portID!=NULL)
					*portID = proxyPortID;
		
				return true;
			}
			else 
				return false;
		}
		else 
			return false;
	}

	bool ControlModule::isInputPortOwner(const Module* m, int portID, int* recordID) const
	{
		if(portID<0 || portID>=inputTextureOwners.size())
			return false;
		else
		{
			bool test = (inputTextureOwners[portID]==m);

			if(test && recordID!=NULL)
				*recordID = inputTextureRecordIDs[portID];

			return test;
		}
	}

	const std::string& ControlModule::getPipelineCode(void) const
	{
		if(pipelineExists())
			return pipelineCode;
		else
			return std::string("");
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

	bool ControlModule::pipelineCompilation(void)
	{
		bool success = false;

		// First, is there a removal : 
		if(pipelineExists())
		{
			if(!requirePipelineDestruction(true)) // Keep the code...
				return false; // If the previous pipeline destruction was aborted...
		}

		// Then if anyone object the compilation ;
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeCompiled();

		if(!poll)
			return false;

		// Create the pipeline from the known code: 
		try
		{
			// First step : get informations.
			pipelineLoader.clearRequiredElements();
			LayoutLoader::PipelineScriptElements infos = pipelineLoader.listElements(pipelineCode);	

			// Ask all modules to prepare the loader : 
			for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end(); it++)
				(*it)->preparePipelineLoading(pipelineLoader, infos);

			// This must also prepare the loader with known formats : 
			for(int k=0; k<infos.mainPipelineInputs.size(); k++)
			{
				if(isInputValid(k))
					pipelineLoader.addRequiredElement( infos.mainPipelineInputs[k], inputTextureFormat(k) );
				else
					pipelineLoader.addRequiredElement( infos.mainPipelineInputs[k], HdlTextureFormat(1, 1, GL_RGB, GL_UNSIGNED_BYTE) ); // non blocking
			}

			// Compile :
			pipelinePtr = pipelineLoader(pipelineCode, "");

			// Update the computation : 
			requirePipelineComputation();

			// Propagate the information : 
			emit pipelineWasCreated();

			success = true;
		}
		catch(Exception& e)
		{
			// Manage exception
			emit pipelineCompilationFailed(e);

			// Clear out : 
			delete pipelinePtr;
			pipelinePtr	= NULL;
		
			success = false;
		}

		return success;
	}

	bool ControlModule::requirePipelineCreation(const std::string& code)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeCreated();
	
		if(poll)
		{
			// Copy the code : 
			pipelineCode 	= code;

			return pipelineCompilation();
		}
		else
			return false;
	}

	bool ControlModule::pipelineComputation(void)
	{
		lastComputationSucceeded = false;

		if(!pipelineExists())
			return false;

		if(pipeline().isBroken())
			return false;

		try
		{
			// Clean : 
			pipeline() << Pipeline::Reset;

			// Check that all inputs are present :
			bool greenLight = true; 
			for(int k=0; k<pipeline().getNumInputPort(); k++)
			{
				if(!isInputValid(k))
				{
					greenLight = false;
					break;
				}	
				else
					pipeline() << inputTexture(k);
			}

			if(!greenLight)
				emit pipelineComputationFailed( Exception("Missing input(s).") );
			else
			{
				// Compute : 
				pipeline() << Pipeline::Process;

				lastComputationSucceeded = true;

				// Signals : 
				emit pipelineWasComputed();
			}
		}
		catch(Exception& e)
		{
			lastComputationSucceeded = false;
		
			emit pipelineComputationFailed(e);
		}

		return lastComputationSucceeded;
	}

	bool ControlModule::requirePipelineComputation(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeComputed();

		if(poll)
			return pipelineComputation();
		else
			return false;
	}

	bool ControlModule::registerInputTexture(Module* m, int recordID, int portID)
	{
		if(portID<0 || portID>=maxNumInputs)
			throw Exception("ControlModule::registerInputTexture - Port ID " + to_string(portID) + "is out of bounds ([0; " + to_string(maxNumInputs-1) + "]).", __FILE__, __LINE__);

		// Check if all modules allow inputs change : 
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineInputsCanBeModified();

		if( poll )
		{
			bool needRecompilation = true;

			if(inputTextureOwners[portID]!=NULL)
			{
				if(!inputTextureOwners[portID]->textureInputCanBeReleased(portID, inputTextureRecordIDs[portID]))
					return false;
				else if(inputTextureOwners[portID]==m && inputTextureRecordIDs[portID]==recordID)
					needRecompilation = true;									// Force recompilation as the texture is the same (some settings must
																	// have changed but they won't be picked by the last case).
				else 
				{
					// Old method for the recompilation : 
					//needRecompilation = !inputTexture(portID).isCompatibleWith( m->getTexture(recordID) );

					// New method (which is more agressive, but required if the pipeline is using formats for internal purpose) : 
					needRecompilation = inputTextureFormat(portID) != m->getTextureFormat(recordID);
				}
			}
		
			// Else :
			
			// Say to the old owner that he lose the connection :
			if(inputTextureOwners[portID]!=NULL && inputTextureRecordIDs[portID]>=0)
				inputTextureOwners[portID]->pipelineInputFromThisModuleWasReleased(portID, inputTextureRecordIDs[portID]);

			// Save the new input coordinates : 
			inputTextureRecordIDs[portID] 	= recordID;
			inputTextureOwners[portID]	= m;

			emit pipelineInputWasModified(portID);

			if( pipelineExists() )
			{
				if(needRecompilation)
				{
					if(!pipelineCompilation())
						return false;
				}

				requirePipelineComputation();
			}

			return true;
		}
		else 		
			return false;
	}

	void ControlModule::unregisterInputTexture(Module* m, int recordID)
	{
		int k=0;

		for(k=0; k<inputTextureRecordIDs.size(); k++)
		{
			if(inputTextureRecordIDs[k]==recordID && inputTextureOwners[k]==m)
				break;
		}
		
		if(k<inputTextureRecordIDs.size())
		{
			// Check if all modules allow inputs change : 
			bool poll = true;

			for(std::vector<Module*>::iterator itr=clients.begin(); itr!=clients.end() && poll; itr++)
				poll = poll && (*itr)->pipelineInputsCanBeModified();

			if( poll )
			{
				inputTextureRecordIDs[k]	= -1;
				inputTextureOwners[k]		= NULL;

				if( pipelineExists() )
				{
					// Removed part : 
					//pipelineCompilation();		// Will update to the right sizes.
					//requirePipelineComputation();

					if(k<pipeline().getNumInputPort())
						requirePipelineComputation();
				}

				emit pipelineInputWasReleased(k);
			}
		}
	}

	void ControlModule::releaseInputPort(int portID)
	{
		if(portID<inputTextureRecordIDs.size())
		{
			// Check if all modules allow inputs change : 
			bool poll = true;

			for(std::vector<Module*>::iterator itr=clients.begin(); itr!=clients.end() && poll; itr++)
				poll = poll && (*itr)->pipelineInputsCanBeModified();

			if( poll )
			{
				if(inputTextureOwners[portID]!=NULL && inputTextureRecordIDs[portID]>=0)
					inputTextureOwners[portID]->pipelineInputFromThisModuleWasReleased(portID, inputTextureRecordIDs[portID]);

				inputTextureRecordIDs[portID]	= -1;
				inputTextureOwners[portID]	= NULL;

				if( pipelineExists() )
				{
					// Removed part : 
					//pipelineCompilation();		// Will update to the right sizes.
					//requirePipelineComputation();

					if(portID<pipeline().getNumInputPort())
						requirePipelineComputation();
				}

				emit pipelineInputWasReleased(portID);
			}
		}
	}

	bool ControlModule::requirePrepareToPipelineUniformsModification(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineUniformsCanBeModified();

		return poll;
	}

	bool ControlModule::requirePipelineDestruction(bool keepCode)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->pipelineCanBeDestroyed();

		if(poll)
		{
			delete pipelinePtr;
			pipelinePtr 			= NULL;
			lastComputationSucceeded	= false;

			if(!keepCode)
				pipelineCode.clear();

			emit pipelineWasDestroyed();

			return true;
		}
		else
			return false;
	}

	bool ControlModule::requireClose(void)
	{
		bool poll = true;

		for(std::vector<Module*>::iterator it=clients.begin(); it!=clients.end() && poll; it++)
			poll = poll && (*it)->canBeClosed();

		return poll;
	}

	void ControlModule::pipelineUniformModification(void)
	{
		emit pipelineUniformsWereModified();
	}

