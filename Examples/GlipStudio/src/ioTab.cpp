#include "ioTab.hpp"

	IOTab::IOTab(ControlModule& _masterModule, ImagesCollection *_resourcesManagerLink, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this),
		pipelineStatusLabel(this),
		menuBar(this),
		portsList(this),
		imagesMenu("Images", NULL),
		contextMenu(this),
		inputsViewManager(NULL),
		outputsViewManager(NULL),
		resourcesManagerLink(_resourcesManagerLink),
		openSaveInterface("ImagesOutputPannel", "File", "*.png *.bmp *.jpg *.jpeg *.gif *.ppm"),
		copyAsNewResourceAction("Copy as New Resource", this),
		replaceResourceAction("Replace existing or add as new Resource", this),
		copyAsNewResourceWithNewNameAction("Copy as New Resource (with new name)...", this)
	{
		inputsViewManager = getViewManager();

		if(inputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (input).", __FILE__, __LINE__);

		outputsViewManager = getViewManager();

		if(outputsViewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager (output).", __FILE__, __LINE__);

		imagesMenu.addAction(&copyAsNewResourceAction);
		imagesMenu.addAction(&replaceResourceAction);
		imagesMenu.addAction(&copyAsNewResourceWithNewNameAction);
		openSaveInterface.addSaveToMenu(imagesMenu);
		openSaveInterface.enableOpen(false);
		openSaveInterface.enableSave(false);
		openSaveInterface.enableSaveAs(false);
		copyAsNewResourceAction.setEnabled(false);
		copyAsNewResourceWithNewNameAction.setEnabled(false);

		menuBar.addMenu(&imagesMenu);
		menuBar.addMenu(inputsViewManager);
		menuBar.addMenu(outputsViewManager);

		layout.addWidget(&menuBar);
		layout.addWidget(&portsList);
		layout.addWidget(&pipelineStatusLabel);

		inputsViewManager->setTitle("Inputs Views");
		outputsViewManager->setTitle("Outputs Views");
		pipelineStatusLabel.setReadOnly(true);

		contextMenu.addAction(&copyAsNewResourceAction);
		contextMenu.addAction(&replaceResourceAction);
		contextMenu.addAction(&copyAsNewResourceWithNewNameAction);
		openSaveInterface.addSaveToMenu(contextMenu);

		// Init : 
		pipelineWasDestroyed();

		// Connect : 
		connect(&portsList, 				SIGNAL(itemSelectionChanged()), 			this, SLOT(selectionChanged()));
		connect(&portsList, 				SIGNAL(focusChanged(int)),				this, SLOT(focusChanged(int)));
		connect(&portsList, 				SIGNAL(itemSelectionChanged()),	 			this, SLOT(selectionChanged()));
		connect(inputsViewManager, 			SIGNAL(createNewView()), 				this, SLOT(newInputView()));
		connect(outputsViewManager, 			SIGNAL(createNewView()), 				this, SLOT(newOutputView()));
		connect(&copyAsNewResourceAction,		SIGNAL(triggered()),					this, SLOT(copyAsNewResource()));
		connect(&replaceResourceAction,			SIGNAL(triggered()),					this, SLOT(replaceResource()));
		connect(&copyAsNewResourceWithNewNameAction,	SIGNAL(triggered()),					this, SLOT(copyAsNewResourceWithNewName()));
		connect(&openSaveInterface,			SIGNAL(saveFile(const QString&)),			this, SLOT(saveOutput(const QString&)));
		connect(&openSaveInterface,			SIGNAL(saveFileAs(const QString&)),			this, SLOT(saveOutput(const QString&)));
		connect(&portsList,				SIGNAL(customContextMenuRequested(const QPoint&)), 	this, SLOT(showContextMenu(const QPoint&)));
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
				s.savedToDisk		= true;

				inputRecordIDs[k] = portsList.addRecord( pipeline().getInputPortName(k), s );
				pipelineInputWasModified(k);
			}

			for(int k=0; k<pipeline().getNumOutputPort(); k++)
			{
				TextureStatus s( TextureStatus::OutputPort );
				s.location 		= TextureStatus::OnVRAM;
				s.connectionStatus 	= lastComputationWasSuccessful() ? TextureStatus::Connected : TextureStatus::NotConnected ;
				s.portID		= k;
				s.savedToDisk		= true;

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

		void IOTab::pipelineComputationFailed(const Exception& e)
		{
			pipelineStatusLabel.setText(tr("Computation of Pipeline \"%1\" failed :\n%2").arg(pipeline().getName().c_str()).arg(e.what()));

			// Update colors : 
			// (note : the vector outputRecordIDs might not be initialized at this point)
			for(int k=0; k<std::min(pipeline().getNumOutputPort(), static_cast<int>(outputRecordIDs.size())); k++)
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

				if( inputsViewManager->isOnDisplay( inputRecordIDs[portID] ) && isInputValid(portID) )
					inputsViewManager->update( inputRecordIDs[portID], inputTexture(portID) );
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

				inputsViewManager->removeRecord(portID);
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
			else if(lastComputationWasSuccessful())
			{
				outputPortID = getOutputPortIDFromRecordID(recordID);

				outputsViewManager->enableCreationAction(true);

				if(outputPortID>=0 && outputPortID<pipeline().getNumOutputPort() && lastComputationWasSuccessful())
					outputsViewManager->show(outputPortID, pipeline().out(outputPortID));
			}
			else
				outputsViewManager->enableCreationAction(false);
		}

		void IOTab::selectionChanged(void)
		{
			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			if(recordIDs.empty())
			{
				inputsViewManager->enableCreationAction(false);
				outputsViewManager->enableCreationAction(false);
				openSaveInterface.enableSave(false);
				openSaveInterface.enableSaveAs(false);
				copyAsNewResourceAction.setEnabled(false);
				copyAsNewResourceWithNewNameAction.setEnabled(false);
			}
			else if(!lastComputationWasSuccessful())
			{
				openSaveInterface.enableSave(false);
				openSaveInterface.enableSaveAs(false);
				copyAsNewResourceAction.setEnabled(false);
				copyAsNewResourceWithNewNameAction.setEnabled(false);
			}
			else
			{
				if(recordIDs.size()==1 && getOutputPortIDFromRecordID(recordIDs.front())>=0)
				{
					openSaveInterface.enableSave(true);
					openSaveInterface.enableSaveAs(true);
					copyAsNewResourceAction.setEnabled(true);
					copyAsNewResourceWithNewNameAction.setEnabled(true);
				}
				else
				{
					openSaveInterface.enableSave(false);
					openSaveInterface.enableSaveAs(false);

					bool test = true;
			
					for(int k=0; k<recordIDs.size() && test; k++)
						test = getOutputPortIDFromRecordID(recordIDs[k])>=0;

					copyAsNewResourceAction.setEnabled(test);
					copyAsNewResourceWithNewNameAction.setEnabled(test);
				}
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
		
		void IOTab::copyAsNewResource(bool replace)
		{
			if(!pipelineExists() || !lastComputationWasSuccessful())
				return ;

			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			for(int k=0; k<recordIDs.size(); k++)
			{
				int outputPortID = getOutputPortIDFromRecordID(recordIDs[k]);

				if(outputPortID>=0)
					resourcesManagerLink->addNewResource(pipeline().out(outputPortID), pipeline().getOutputPortName(outputPortID), replace);
			}
		}

		void IOTab::replaceResource(void)
		{
			copyAsNewResource(true);
		}

		void IOTab::copyAsNewResourceWithNewName(void)
		{
			if(!pipelineExists() || !lastComputationWasSuccessful())
				return ;

			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			for(int k=0; k<recordIDs.size(); k++)
			{
				int outputPortID = getOutputPortIDFromRecordID(recordIDs[k]);

				if(outputPortID>=0)
				{
					// Ask for new name : 
					bool 	over = false,
						ok = false;
			    		QString name;

					do
					{
						// Get the new name : 
						name = QInputDialog::getText(this, tr("Copy Output as new Resource..."), tr("New resource name : "), QLineEdit::Normal, pipeline().getOutputPortName(outputPortID).c_str(), &ok);

						// Check if the name exits : 
						over = !ok || !name.isEmpty();
					} while(!over);

					// Copy : 
					if(ok)
						resourcesManagerLink->addNewResource(pipeline().out(outputPortID), name.toStdString());
				}
			}
		}

		void IOTab::saveOutput(const QString& filename)
		{
			std::vector<int> recordIDs = portsList.getSelectedRecordIDs();

			if(recordIDs.empty())
				throw Exception("No selection was made (internal error).", __FILE__, __LINE__);

			if(recordIDs.size()>1)
				throw Exception("Too many selections (internal error).", __FILE__, __LINE__);

			int portID = getOutputPortIDFromRecordID(recordIDs.front());

			if(portID<0)
				throw Exception("The selection is not an output port (internal error).", __FILE__, __LINE__);

			ImageObject image(pipeline().out(portID));

			image.save(filename.toStdString());

			openSaveInterface.reportSuccessfulSave(filename);
		}

		void IOTab::showContextMenu(const QPoint& point)
		{
			QPoint globalPos = portsList.viewport()->mapToGlobal(point);

			contextMenu.exec(globalPos);
		}

