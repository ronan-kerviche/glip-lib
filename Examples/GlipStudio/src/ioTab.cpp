#include "ioTab.hpp"

	IOTab::IOTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this),
		pipelineStatusLabel(this),
		inputMenuBar(this),
		outputMenuBar(this),
		inputsList(this),
		outputsList(this),
		inputsViewManager(NULL),
		outputsViewManager(NULL)
	{
		inputsViewManager = getViewManager();

		if(inputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (input).", __FILE__, __LINE__);

		outputsViewManager = getViewManager();

		if(outputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (output).", __FILE__, __LINE__);

		inputMenuBar.addMenu(inputsViewManager);
		outputMenuBar.addMenu(outputsViewManager);

		layout.addWidget(&pipelineStatusLabel);
		layout.addWidget(&inputMenuBar);
		layout.addWidget(&inputsList);
		layout.addWidget(&outputMenuBar);
		layout.addWidget(&outputsList);

		// Init : 
		pipelineWasDestroyed();

		// Connect : 
		connect(&inputsList, 		SIGNAL(itemSelectionChanged()), this, SLOT(inputSelectionChanged()));
		connect(&outputsList, 		SIGNAL(itemSelectionChanged()), this, SLOT(outputSelectionChanged()));
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

				inputRecordIDs[k] = inputsList.addRecord( pipeline().getInputPortName(k), s );
				pipelineInputWasModified(k);
			}

			for(int k=0; k<pipeline().getNumOutputPort(); k++)
			{
				TextureStatus s( TextureStatus::OutputPort );
				s.location 		= TextureStatus::OnVRAM;
				s.connectionStatus 	= lastComputationWasSuccessful() ? TextureStatus::Connected : TextureStatus::NotConnected ;
				s.portID		= k;

				outputRecordIDs[k] = outputsList.addRecord( pipeline().getOutputPortName(k), pipeline().out(k).format(), s );
			}
		}

		void IOTab::pipelineWasComputed(void)
		{
			/*if(isThisLinkedToDisplay() && pipelineOutputOnDisplay)
			{
				// Update colors : 
				for(int k=0; k<pipeline().getNumOutputPort(); k++)
				{
					TextureStatus s 	= outputsList.recordStatus( outputRecordIDs[k] );
					s.connectionStatus 	= TextureStatus::Connected;
					outputsList.updateRecordStatus( outputRecordIDs[k], s );
				}

				outputSelectionChanged();
			}*/

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
				TextureStatus 	s 	= outputsList.recordStatus( outputRecordIDs[k] );
				s.connectionStatus	= TextureStatus::NotConnected;
				outputsList.updateRecordStatus( outputRecordIDs[k], s );
			}
		}

		void IOTab::pipelineInputWasModified(int portID)
		{
			if(pipelineExists() && isInputValid(portID))
			{
				std::string name;
				getInputTextureInformation(portID, name);

				inputsList.updateRecordName( inputRecordIDs[portID], pipeline().getInputPortName(portID) + " <- " + name );
				inputsList.updateRecordFormat( inputRecordIDs[portID], inputTexture(portID) );

				TextureStatus s 	= inputsList.recordStatus( inputRecordIDs[portID] );
				s.connectionStatus 	= TextureStatus::Connected;

				inputsList.updateRecordStatus( inputRecordIDs[portID], s );
			}
		}

		void IOTab::pipelineInputWasReleased(int portID)
		{
			if(pipelineExists())
			{
				inputsList.updateRecordName( inputRecordIDs[portID], pipeline().getInputPortName(portID) );
				inputsList.updateRecordFormat( inputRecordIDs[portID] );

				TextureStatus s 	= inputsList.recordStatus( inputRecordIDs[portID] );
				s.connectionStatus	= TextureStatus::NotConnected;

				inputsList.updateRecordStatus( inputRecordIDs[portID], s );
			}
		}

		void IOTab::pipelineWasDestroyed(void)
		{
			pipelineStatusLabel.setText(tr("No Pipeline"));

			// Clean the lists :
			inputRecordIDs.clear();
			outputRecordIDs.clear();
			inputsList.removeAllRecords();
			outputsList.removeAllRecords();
		}

		void IOTab::inputSelectionChanged(void)
		{
			std::vector<int> recordIDs = inputsList.getSelectedRecordIDs();
			
			if(!recordIDs.empty())
			{
				inputsViewManager->enableCreationAction(true);

				int portID = getInputPortIDFromRecordID( recordIDs.back() );

				if(isInputValid(portID))
				{
					// Grab the display : 
					/*WindowRenderer* display = NULL;
					if(requireDisplay(display))
					{
						HdlTexture& input = inputTexture(portID);

						display->setImageAspectRatio( input.format() );
						(*display) << input << OutputDevice::Process;
					}*/

					inputsViewManager->show(portID, inputTexture(portID));
				}
			}
			else
				inputsViewManager->enableCreationAction(true);
		}

		void IOTab::outputSelectionChanged(void)
		{
			std::vector<int> recordIDs = outputsList.getSelectedRecordIDs();

			if(!recordIDs.empty() && pipelineExists())
			{
				outputsViewManager->enableCreationAction(true);

				int portID = getOutputPortIDFromRecordID( recordIDs.back() );

				if(portID>=0 && portID<pipeline().getNumOutputPort() )
				{
					// Grab the display : 
					/*WindowRenderer* display = NULL;
					if(requireDisplay(display))
					{
						display->setImageAspectRatio( pipeline().out(portID) );
						(*display) << pipeline().out(portID) << OutputDevice::Process;
						pipelineOutputOnDisplay = true;
					}*/

					outputsViewManager->show(portID, pipeline().out(portID));
				}
			}
			else
				outputsViewManager->enableCreationAction(false);
		}

		void IOTab::newInputView(void)
		{
			std::vector<int> recordIDs = inputsList.getSelectedRecordIDs();
			
			if(!recordIDs.empty())
			{
				int portID = getInputPortIDFromRecordID( recordIDs.back() );

				if(isInputValid(portID))
					inputsViewManager->show(portID, inputTexture(portID), true);
			}
		}

		void IOTab::newOutputView(void)
		{
			std::vector<int> recordIDs = outputsList.getSelectedRecordIDs();

			if(!recordIDs.empty() && pipelineExists())
			{
				int portID = getOutputPortIDFromRecordID( recordIDs.back() );

				if(portID>=0 && portID<pipeline().getNumOutputPort() )
					outputsViewManager->show(portID, pipeline().out(portID), true);
			}
		}

