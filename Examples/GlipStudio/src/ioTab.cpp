#include "ioTab.hpp"

	IOTab::IOTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this),
		pipelineStatusLabel(this),
		menuBar(this),
		portsList(this),
		inputsViewManager(NULL),
		outputsViewManager(NULL)
	{
		inputsViewManager = getViewManager();

		if(inputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (input).", __FILE__, __LINE__);

		outputsViewManager = getViewManager();

		if(outputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (output).", __FILE__, __LINE__);

		menuBar.addMenu(inputsViewManager);
		menuBar.addMenu(outputsViewManager);

		layout.addWidget(&menuBar);
		layout.addWidget(&portsList);
		layout.addWidget(&pipelineStatusLabel);

		inputsViewManager->setTitle("Inputs Views");
		outputsViewManager->setTitle("Outputs Views");
		pipelineStatusLabel.setReadOnly(true);

		// Init : 
		pipelineWasDestroyed();

		// Connect : 
		connect(&portsList, 		SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
		connect(&portsList, 		SIGNAL(focusChanged(int)),	this, SLOT(focusChanged(int)));
		connect(&portsList, 		SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
		connect(inputsViewManager, 	SIGNAL(createNewView()), 	this, SLOT(newInputView()));
		connect(outputsViewManager, 	SIGNAL(createNewView()), 	this, SLOT(newOutputView()));
	}

	IOTab::~IOTab(void)
	{ }

	// Private functions : 
		int IOTab::getInputPortIDFromRecordID( int recordID )
		{
			std::vector<int>::iterator it = std::find(inputRecordIDs.begin(), inputRecordIDs.end(), recordID);
			
			if(it==inputRecordIDs.end())
				return -1;
			else
				return std::distance(inputRecordIDs.begin(), it);
		}

		int IOTab::getOutputPortIDFromRecordID( int recordID )
		{
			std::vector<int>::iterator it = std::find(outputRecordIDs.begin(), outputRecordIDs.end(), recordID);
			
			if(it==outputRecordIDs.end())
				return -1;
			else
				return std::distance(outputRecordIDs.begin(), it);
		}

	// Private slots : 
		void IOTab::pipelineWasCreated(void)
		{
			pipelineStatusLabel.setText(tr("Pipeline \"%1\" was created.").arg(pipeline().getName().c_str()));

			// Set up the lists : 
			inputRecordIDs.assign( pipeline().getNumInputPort(), -1);
			outputRecordIDs.assign( pipeline().getNumOutputPort(), -1);

			for(int k=0; k<pipeline().getNumInputPort(); k++)
			{
				TextureStatus s( TextureStatus::InputPort );
				s.location 		= TextureStatus::VirtualLink;
				s.portID		= k;

				inputRecordIDs[k] = portsList.addRecord( pipeline().getInputPortName(k), s );
				pipelineInputWasModified(k);
			}

			for(int k=0; k<pipeline().getNumOutputPort(); k++)
			{
				TextureStatus s( TextureStatus::OutputPort );
				s.location 		= TextureStatus::OnVRAM;
				s.connectionStatus 	= lastComputationWasSuccessful() ? TextureStatus::Connected : TextureStatus::NotConnected ;
				s.portID		= k;

				outputRecordIDs[k] = portsList.addRecord( pipeline().getOutputPortName(k), pipeline().out(k).format(), s );
			}
		}

		void IOTab::pipelineWasComputed(void)
		{
			if( outputsViewManager->hasViews() )
			{
				outputsViewManager->beginQuietUpdate();

				for(int k=0; k<pipeline().getNumOutputPort(); k++)
					outputsViewManager->update( k, pipeline().out(k) );

				outputsViewManager->endQuietUpdate();
			}
		}

		void IOTab::pipelineComputationFailed(Exception& e)
		{
			pipelineStatusLabel.setText(tr("Computation of Pipeline \"%1\" failed :\n%2").arg(pipeline().getName().c_str()).arg(e.what()));

			// Update colors : 
			for(int k=0; k<pipeline().getNumOutputPort(); k++)
			{
				TextureStatus 	s 	= portsList.recordStatus( outputRecordIDs[k] );
				s.connectionStatus	= TextureStatus::NotConnected;
				portsList.updateRecordStatus( outputRecordIDs[k], s );
			}

			// Close outputs : 
			outputsViewManager->closeAllViews();
		}

		void IOTab::pipelineInputWasModified(int portID)
		{
			if(pipelineExists() && isInputValid(portID))
			{
				std::string name;
				getInputTextureInformation(portID, name);

				portsList.updateRecordName( inputRecordIDs[portID], pipeline().getInputPortName(portID) + " <- " + name );
				portsList.updateRecordFormat( inputRecordIDs[portID], inputTexture(portID) );

				TextureStatus s 	= portsList.recordStatus( inputRecordIDs[portID] );
				s.connectionStatus 	= TextureStatus::Connected;

				portsList.updateRecordStatus( inputRecordIDs[portID], s );
			}
		}

		void IOTab::pipelineInputWasReleased(int portID)
		{
			if(pipelineExists())
			{
				portsList.updateRecordName( inputRecordIDs[portID], pipeline().getInputPortName(portID) );
				portsList.updateRecordFormat( inputRecordIDs[portID] );

				TextureStatus s 	= portsList.recordStatus( inputRecordIDs[portID] );
				s.connectionStatus	= TextureStatus::NotConnected;

				portsList.updateRecordStatus( inputRecordIDs[portID], s );
			}
		}

		void IOTab::pipelineWasDestroyed(void)
		{
			pipelineStatusLabel.setText(tr("No Pipeline"));

			// Clean the lists :
			inputRecordIDs.clear();
			outputRecordIDs.clear();
			portsList.removeAllRecords();
			inputsViewManager->closeAllViews();
			outputsViewManager->closeAllViews();
			inputsViewManager->enableCreationAction(false);
			outputsViewManager->enableCreationAction(false);
		}

		void IOTab::focusChanged(int recordID)
		{
			int 	inputPortID 	= getInputPortIDFromRecordID(recordID),
				outputPortID 	= -1;
	
			if(inputPortID!=-1)
			{
				inputsViewManager->enableCreationAction(true);
		
				if(isInputValid(inputPortID))
					inputsViewManager->show(inputPortID, inputTexture(inputPortID));
			}
			else
			{
				outputPortID = getOutputPortIDFromRecordID(recordID);

				outputsViewManager->enableCreationAction(true);

				if(outputPortID>=0 && outputPortID<pipeline().getNumOutputPort() && lastComputationWasSuccessful())
					outputsViewManager->show(outputPortID, pipeline().out(outputPortID));
			}
		}

		void IOTab::selectionChanged(void)
		{
			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			if(recordIDs.empty())
			{
				inputsViewManager->enableCreationAction(false);
				outputsViewManager->enableCreationAction(false);
			}
		}

		void IOTab::newInputView(void)
		{
			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			if(!recordIDs.empty())
			{
				int inputPortID = getInputPortIDFromRecordID(recordIDs.back());
		
				if(isInputValid(inputPortID))
					inputsViewManager->show(inputPortID, inputTexture(inputPortID), true);
			}
		}

		void IOTab::newOutputView(void)
		{
			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			if(!recordIDs.empty() && pipelineExists())
			{
				int outputPortID = getOutputPortIDFromRecordID(recordIDs.back());
		
				if(outputPortID>=0 && outputPortID<pipeline().getNumOutputPort() )
					outputsViewManager->show(outputPortID, pipeline().out(outputPortID), true);
			}
		}

