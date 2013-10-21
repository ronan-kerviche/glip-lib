#include "ioTab.hpp"

	IOTab::IOTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		pipelineOutputOnDisplay(false),
		layout(this),
		pipelineStatusLabel(this),
		inputMenuBar(this),
		outputMenuBar(this),
		inputsList(this),
		outputsList(this)
	{
		layout.addWidget(&pipelineStatusLabel);
		layout.addWidget(&inputMenuBar);
		layout.addWidget(&inputsList);
		layout.addWidget(&outputMenuBar);
		layout.addWidget(&outputsList);

		// Init : 
		pipelineWasDestroyed();

		// Connect : 
		connect(&inputsList, 	SIGNAL(itemSelectionChanged()), this, SLOT(inputSelectionChanged()));
		connect(&outputsList, 	SIGNAL(itemSelectionChanged()), this, SLOT(outputSelectionChanged()));
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
				TextureStatus 	s( TextureStatus::InputPort );
				s.location 	= TextureStatus::VirtualLink;
				s.portID	= k;

				inputRecordIDs[k] = inputsList.addRecord( pipeline().getInputPortName(k), s );
				pipelineInputWasModified(k);
			}

			for(int k=0; k<pipeline().getNumOutputPort(); k++)
			{
				TextureStatus s( TextureStatus::OutputPort );
				s.location 	= TextureStatus::OnVRAM;
				s.connected 	= lastComputationWasSuccessful();
				s.portID	= k;

				outputRecordIDs[k] = outputsList.addRecord( pipeline().getOutputPortName(k), pipeline().out(k).format(), s );
			}
		}

		void IOTab::pipelineWasComputed(void)
		{
			if(isThisLinkedToDisplay() && pipelineOutputOnDisplay)
			{
				// Update colors : 
				for(int k=0; k<pipeline().getNumOutputPort(); k++)
				{
					TextureStatus s = outputsList.recordStatus( outputRecordIDs[k] );
					s.connected = true;
					outputsList.updateRecordStatus( outputRecordIDs[k], s );
				}

				outputSelectionChanged();
			}
		}

		void IOTab::pipelineComputationFailed(Exception& e)
		{
			pipelineStatusLabel.setText(tr("Computation of Pipeline \"%1\" failed :\n%2").arg(pipeline().getName().c_str()).arg(e.what()));

			// Update colors : 
				for(int k=0; k<pipeline().getNumOutputPort(); k++)
				{
					TextureStatus s = outputsList.recordStatus( outputRecordIDs[k] );
					s.connected = false;
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

				TextureStatus s = inputsList.recordStatus( inputRecordIDs[portID] );
				s.connected = true;

				inputsList.updateRecordStatus( inputRecordIDs[portID], s );
			}
		}

		void IOTab::pipelineInputWasReleased(int portID)
		{
			if(pipelineExists())
			{
				inputsList.updateRecordName( inputRecordIDs[portID], pipeline().getInputPortName(portID) );
				inputsList.updateRecordFormat( inputRecordIDs[portID] );

				TextureStatus s = inputsList.recordStatus( inputRecordIDs[portID] );
				s.connected = false;

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
			pipelineOutputOnDisplay = false;

			std::vector<int> recordIDs = inputsList.getSelectedRecordIDs();
			
			if(!recordIDs.empty())
			{
				int portID = getInputPortIDFromRecordID( recordIDs.back() );

				if(isInputValid(portID))
				{
					// Grab the display : 
					WindowRenderer* display = NULL;
					if(requireDisplay(display))
					{
						HdlTexture& input = inputTexture(portID);

						display->setImageAspectRatio( input.format() );
						(*display) << input << OutputDevice::Process;
					}
				}
			}
		}

		void IOTab::outputSelectionChanged(void)
		{
			pipelineOutputOnDisplay = false;

			std::vector<int> recordIDs = outputsList.getSelectedRecordIDs();

			if(!recordIDs.empty() && pipelineExists())
			{
				int portID = getOutputPortIDFromRecordID( recordIDs.back() );

				if(portID>=0 && portID<pipeline().getNumOutputPort() )
				{
					// Grab the display : 
					WindowRenderer* display = NULL;
					if(requireDisplay(display))
					{
						display->setImageAspectRatio( pipeline().out(portID) );
						(*display) << pipeline().out(portID) << OutputDevice::Process;
						pipelineOutputOnDisplay = true;
					}
				}
			}
		}

