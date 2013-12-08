#include "resourcesTab.hpp"
#include <algorithm>

// ConnectionMenu
	ConnectionMenu::ConnectionMenu(QWidget* parent)
	 : QMenu("Connect", parent)	
	{
		update();
	
		QObject::connect(&mapper, SIGNAL(mapped(int)), this, SIGNAL(connectToInput(int))); 
	}

	ConnectionMenu::~ConnectionMenu(void)
	{
		clear();
		currentActions.clear();
	}

	void ConnectionMenu::activate(bool state, int numConnections)
	{
		clear();
		currentActions.clear();

		if(state && numConnections==1 && !portsNames.isEmpty())
		{
			for(int k=0; k<portsNames.count(); k++)
			{
				QAction* action = addAction(tr("To %1").arg(portsNames[k]));
				currentActions.push_back(action);
				mapper.setMapping(action, k);
				QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
			}
		}
		else if(state && numConnections>1 && numConnections<=portsNames.count())
		{
			for(int k=0; k<portsNames.count()-numConnections+1; k++)
			{
				QAction* action = addAction(tr("From %1 to %2").arg(portsNames[k]).arg(portsNames[k + numConnections - 1]));
				currentActions.push_back(action);
				mapper.setMapping(action, k);
				QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
			}
		}
		else if(state && numConnections>portsNames.count() && !portsNames.isEmpty())
			addAction("Too many resources selected")->setEnabled(false);
		else if(portsNames.isEmpty())
			addAction("No input ports defined")->setEnabled(false);
		else if(!state)
			addAction("No suitable input port")->setEnabled(false);	
		else
			addAction("No available input ports")->setEnabled(false);
	}

	void ConnectionMenu::update(void)
	{
		clear();
		currentActions.clear();
		addAction("No available input port")->setEnabled(false);
	}

	void ConnectionMenu::update(const __ReadOnly_PipelineLayout& layout)
	{
		clear();
		currentActions.clear();
		portsNames.clear();

		for(int k=0; k<layout.getNumInputPort(); k++)
			portsNames.push_back(layout.getInputPortName(k).c_str());
	}

// ResourceTab
	ResourcesTab::ResourcesTab(ControlModule& _masterModule, QWidget* parent)
	 : Module(_masterModule, parent), layout(this), collection("ResourcesTab", this), viewManager(NULL)
	{
		viewManager = getViewManager();

		if(viewManager==NULL)
			throw Exception("ResourcesTab::ResourcesTab - Unable to create a new ViewManager.", __FILE__, __LINE__);

		collection.addActionsToMenuBar(menuBar);

		menuBar.addMenu(&connectionMenu);
		menuBar.addMenu(viewManager);
		collection.addToContextMenu(connectionMenu);
		collection.addToContextMenu(*viewManager);

		layout.addWidget(&menuBar);
		layout.addWidget(&collection);

		connect(&collection, 		SIGNAL(itemSelectionChanged()), 	this, SLOT(selectionChanged()));
		connect(&collection, 		SIGNAL(focusChanged(int)),		this, SLOT(focusChanged(int)));
		connect(&collection, 		SIGNAL(imageLoaded(int)),		this, SLOT(imageLoaded(int)));
		connect(&collection, 		SIGNAL(imageReplaced(int)),		this, SLOT(imageReplaced(int)));
		connect(&collection, 		SIGNAL(imageSettingsChanged(int)),	this, SLOT(imageSettingsChanged(int)));
		connect(&collection, 		SIGNAL(imageUnloadedFromDevice(int)),	this, SLOT(imageUnloadedFromDevice(int)));
		connect(&collection, 		SIGNAL(imageFreed(int)),		this, SLOT(imageFreed(int)));
		connect(&connectionMenu,	SIGNAL(connectToInput(int)),		this, SLOT(connectToInput(int)));
		connect(viewManager,		SIGNAL(createNewView()),		this, SLOT(createNewView()));
	}

	ResourcesTab::~ResourcesTab(void)
	{ }

	// Private functions : 
		bool ResourcesTab::isValidTexture(int recordID)
		{
			return collection.imageExists(recordID);
		}

		HdlTexture& ResourcesTab::getTexture(int recordID)
		{
			return collection.texture(recordID);
		}

		const __ReadOnly_HdlTextureFormat& ResourcesTab::getTextureFormat(int recordID) const
		{
			return collection.textureFormat(recordID);
		}

		void ResourcesTab::giveTextureInformation(int recordID, std::string& name)
		{
			name = collection.recordName(recordID);
		}

		void ResourcesTab::cleanRecordDependances(int recordID)
		{
			viewManager->removeRecord(recordID);
			unregisterInputTexture(recordID);
		}

		void ResourcesTab::updateTexturesLinkInformation(void)
		{
			if(pipelineExists())
			{
				for(std::vector<int>::iterator it = imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
				{
					int portID = 0;
					TextureStatus s 	= collection.recordStatus( *it );

					if( isListedAsPipelineInput(*it, &portID) )
					{
						if( isUsedAsPipelineInput(*it) )
						{
							s.connectionStatus 	= TextureStatus::Connected;
							s.portID		= portID;
							collection.updateRecordStatus( *it, s);
						}
						else 
						{
							s.connectionStatus 	= TextureStatus::WaitingLink;
							s.portID		= portID;
							collection.updateRecordStatus( *it, s);
						}
					}
					else
					{
						s.connectionStatus 	= TextureStatus::NotConnected;
						s.portID		= 0;
						collection.updateRecordStatus( *it, s);
					}
				}
			}
			else
			{
				for(std::vector<int>::iterator it = imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
				{
					int portID = 0;

					if( isListedAsPipelineInput(*it, &portID) )
					{
						TextureStatus s 	= collection.recordStatus( *it );
						s.connectionStatus 	= TextureStatus::WaitingLink;
						s.portID		= portID;
						collection.updateRecordStatus( *it, s);
					}
					else
					{
						TextureStatus s 	= collection.recordStatus( *it );
						s.connectionStatus 	= TextureStatus::NotConnected;
						s.portID		= 0;
						collection.updateRecordStatus( *it, s);
					}
				}
			}
		}

		bool ResourcesTab::canBeClosed(void)
		{
			return collection.canBeClosed();
		}

	// Private slots : 
		void ResourcesTab::pipelineWasCreated(void)
		{
			updateTexturesLinkInformation();

			// Lock the textures needed : 
			for(std::vector<int>::iterator it=imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
			{
				if(isUsedAsPipelineInput(*it))
					collection.lockTextureToDevice(*it);
			}

			// Update connection menu : 
			connectionMenu.update( pipeline() );

			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			connectionMenu.activate(!selectedRecordsID.empty(), selectedRecordsID.size());
		}

		void ResourcesTab::pipelineInputFromThisModuleWasReleased(int portID, int recordID)
		{
			TextureStatus s 	= collection.recordStatus( recordID );
			s.connectionStatus 	= TextureStatus::NotConnected;
			s.portID		= -1;
			collection.updateRecordStatus( recordID, s);
			collection.unlockTextureFromDevice(recordID);
		}

		void ResourcesTab::pipelineWasDestroyed(void)
		{
			updateTexturesLinkInformation();

			// Release the lock on all input texture (the link is still here and the lock will be re-established if needed by the next pipeline) :
			for(std::vector<int>::iterator it=imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
				collection.unlockTextureFromDevice(*it);

			// Update connection menu : 
			connectionMenu.update();

			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			connectionMenu.activate(!selectedRecordsID.empty(), selectedRecordsID.size());
		}

		void ResourcesTab::focusChanged(int recordID)
		{
			// Display : 
			viewManager->show( recordID, collection.texture(recordID), collection.recordName(recordID).c_str());
		}

		void ResourcesTab::selectionChanged(void)
		{
			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();
		
			if(selectedRecordsID.empty())
			{
				viewManager->enableCreationAction(false);
				connectionMenu.activate(false);
			}
			else
			{
				viewManager->enableCreationAction(true);

				connectionMenu.activate(true, selectedRecordsID.size());

				// Display : 
				//viewManager->show( selectedRecordsID.front(), collection.texture(selectedRecordsID.front()));
			}
		}

		void ResourcesTab::connectToInput(int i)
		{
			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			for(int k=0; k<selectedRecordsID.size(); k++)
			{
				registerInputTexture( selectedRecordsID[k], i + k);
				collection.lockTextureToDevice( selectedRecordsID[k] );

				TextureStatus s 	= collection.recordStatus( selectedRecordsID[k] );
				s.connectionStatus 	= TextureStatus::Connected;
				s.portID		= i + k;
				collection.updateRecordStatus( selectedRecordsID[k], s);
			}
		}

		void ResourcesTab::imageLoaded(int recordID)
		{
			imageRecordIDs.push_back(recordID);
		}

		void ResourcesTab::imageReplaced(int recordID)
		{
			int portID = 0;

			// Update connection / link information :
			TextureStatus s = collection.recordStatus( recordID );
			if(isUsedAsPipelineInput(recordID, &portID))
			{
				s.connectionStatus 	= TextureStatus::Connected;
				s.portID		= portID;
			}
			else if(isListedAsPipelineInput(recordID, &portID))
			{
				s.connectionStatus 	= TextureStatus::WaitingLink;
				s.portID		= portID;
			}
			else
			{
				s.connectionStatus 	= TextureStatus::NotConnected;
				s.portID		= -1;
			}

			collection.updateRecordStatus(recordID, s);

			// Update view if needed : 
			if(viewManager->isLinkedToAView(recordID))
				viewManager->update(recordID, collection.texture(recordID));
		}

		void ResourcesTab::imageSettingsChanged(int recordID)
		{
			// Display : 
			if(viewManager->isLinkedToAView(recordID))
				viewManager->update(recordID, collection.texture(recordID));

			// Pipeline : 
			int portID = 0;
			if(isUsedAsPipelineInput(recordID, &portID))
				registerInputTexture( recordID, portID );
		}

		void ResourcesTab::imageUnloadedFromDevice(int recordID)
		{
			cleanRecordDependances(recordID);
		}

		void ResourcesTab::imageFreed(int recordID)
		{
			cleanRecordDependances(recordID);

			std::vector<int>::iterator it = std::find(imageRecordIDs.begin(), imageRecordIDs.end(), recordID);
	
			imageRecordIDs.erase( it );
		}

		void ResourcesTab::createNewView(void)
		{
			std::vector<int> selectedRecordIDs = collection.getSelectedRecordIDs();

			if(!selectedRecordIDs.empty())
				viewManager->show( selectedRecordIDs.back(), collection.texture(selectedRecordIDs.front()), collection.recordName(selectedRecordIDs.back()).c_str(), true);
		}

		ImagesCollection* ResourcesTab::getResourcesManagerLink(void)
		{
			return &collection;
		}

