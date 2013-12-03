#include "batchTab.hpp"

// BatchTab :
	BatchTab::BatchTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent), 
		layout(this),
		menuBar(this),
		collection("BatchTab", this),
		viewManager(NULL),
		inputsLabel("Inputs : "),
		outputsLabel("Outputs : "),
		pathLabel("Output directory : "),
		changePathButton("..."),
		runButton("Process")
	{
		viewManager = getViewManager();

		if(viewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager.", __FILE__, __LINE__);

		inputsOutpusOptions.addWidget(&inputsLabel);
		inputsOutpusOptions.addWidget(&inputsBox);
		inputsOutpusOptions.addWidget(&outputsLabel);
		inputsOutpusOptions.addWidget(&outputsBox);
		inputsOutpusOptions.addWidget(&outputsNameFormat);

		outputsNameFormat.addItem("outputName{iteration}", QVariant(0));
		outputsNameFormat.addItem("outputName_{iteration}", QVariant(0));
		outputsNameFormat.addItem("outputName{iteration, 2 digits fill}", QVariant(0));
		outputsNameFormat.addItem("outputName_{iteration, 2 digits fill}", QVariant(0));
		outputsNameFormat.addItem("outputName{iteration, 4 digits fill}", QVariant(0));
		outputsNameFormat.addItem("outputName_{iteration, 4 digits fill}", QVariant(0));

		fileBar.addWidget(&pathLabel);
		fileBar.addWidget(&pathLine);
		fileBar.addWidget(&changePathButton);
		fileBar.addWidget(&formatsBox);

		formatsBox.addItem("BMP", QVariant(0));
		formatsBox.addItem("PNG", QVariant(0));
		formatsBox.addItem("JPG", QVariant(0));
		formatsBox.addItem("PGM", QVariant(0));
		formatsBox.addItem("PPM", QVariant(0));

		pathLine.setReadOnly(true);

		processProgressBar.addWidget(&runButton);
		processProgressBar.addWidget(&progressBar);
		
		collection.addActionsToMenuBar(menuBar);
		menuBar.addMenu(viewManager);
		collection.addToContextMenu(*viewManager);

		layout.addWidget(&menuBar);
		layout.addWidget(&collection);
		layout.addLayout(&inputsOutpusOptions);
		layout.addLayout(&fileBar);
		layout.addLayout(&processProgressBar);

		connect(&collection, 		SIGNAL(itemSelectionChanged()), 	this, SLOT(selectionChanged()));
		connect(&collection, 		SIGNAL(focusChanged(int)),		this, SLOT(focusChanged(int)));
		connect(&collection, 		SIGNAL(imageLoaded(int)),		this, SLOT(imageLoaded(int)));
		connect(&collection, 		SIGNAL(imageSettingsChanged(int)),	this, SLOT(imageSettingsChanged(int)));
		connect(&collection, 		SIGNAL(imageUnloadedFromDevice(int)),	this, SLOT(imageUnloadedFromDevice(int)));
		connect(&collection, 		SIGNAL(imageFreed(int)),		this, SLOT(imageFreed(int)));
		connect(viewManager,		SIGNAL(createNewView()),		this, SLOT(createNewView()));

		// Init :
		pipelineWasDestroyed();
	}

	BatchTab::~BatchTab(void)
	{
		// Needed to prevent a segfault from Qt : 
		layout.removeItem(&inputsOutpusOptions);
		layout.removeItem(&fileBar);
		layout.removeItem(&processProgressBar);
		inputsOutpusOptions.setParent(NULL);
		fileBar.setParent(NULL);
		processProgressBar.setParent(NULL);
	}

	// Private : 
		bool BatchTab::isValidTexture(int recordID)
		{
			return collection.imageExists(recordID);
		}

		HdlTexture& BatchTab::getTexture(int recordID)
		{
			return collection.texture(recordID);
		}

		void BatchTab::giveTextureInformation(int recordID, std::string& name)
		{
			name = collection.recordName(recordID);
		}

		void BatchTab::cleanRecordDependances(int recordID)
		{
			viewManager->removeRecord(recordID);
			unregisterInputTexture(recordID);
		}

		bool BatchTab::canBeClosed(void)
		{
			return collection.canBeClosed();
		}

	// Slots : 
		void BatchTab::pipelineWasCreated(void)
		{
			// Fill the boxes : 
			inputsBox.clear();

			inputsBox.addItem("Continuous");
			inputsBox.addItem("Interleaved");
			inputsBox.insertSeparator(2);

			for(int k=0; k<pipeline().getNumInputPort(); k++)
				inputsBox.addItem(pipeline().getInputPortName(k).c_str());

			inputsBox.setEnabled(true);

			outputsBox.clear();
			
			for(int k=0; k<pipeline().getNumOutputPort(); k++)
				outputsBox.addItem(pipeline().getOutputPortName(k).c_str());
		}

		void BatchTab::pipelineInputWasModified(int portID)
		{

		}

		void BatchTab::pipelineInputWasReleased(int portID)
		{

		}

		void BatchTab::pipelineWasDestroyed(void)
		{
			inputsBox.clear();
			inputsBox.addItem("No input");
			inputsBox.setEnabled(false);

			outputsBox.clear();
			outputsBox.addItem("No output");
			outputsBox.setEnabled(false);
		}

		void BatchTab::focusChanged(int recordID)
		{
			throw Exception("Not working.", __FILE__, __LINE__);
			// Display : 
			//viewManager->show( recordID, collection.texture(recordID));
		}

		void BatchTab::selectionChanged(void)
		{
			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();
		
			if(selectedRecordsID.empty())
			{
				viewManager->enableCreationAction(false);
			}
			else
			{
				viewManager->enableCreationAction(true);
			}
		}

		void BatchTab::imageLoaded(int recordID)
		{
			imageRecordIDs.push_back(recordID);
		}

		void BatchTab::imageSettingsChanged(int recordID)
		{
			// Display : 
			if(viewManager->isLinkedToAView(recordID))
				viewManager->update(recordID, collection.texture(recordID));

			// Pipeline : 
			int portID = 0;
			if(isUsedAsPipelineInput(recordID, &portID))
				registerInputTexture( recordID, portID );
		}

		void BatchTab::imageUnloadedFromDevice(int recordID)
		{
			cleanRecordDependances(recordID);
		}

		void BatchTab::imageFreed(int recordID)
		{
			cleanRecordDependances(recordID);

			std::vector<int>::iterator it = std::find(imageRecordIDs.begin(), imageRecordIDs.end(), recordID);
	
			imageRecordIDs.erase( it );
		}

		void BatchTab::createNewView(void)
		{
			throw Exception("Not working.", __FILE__, __LINE__);
			/*std::vector<int> selectedRecordIDs = collection.getSelectedRecordIDs();

			if(!selectedRecordIDs.empty())
				viewManager->show( selectedRecordIDs.back(), collection.texture(selectedRecordIDs.front()), true);*/
		}

