/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : PipelineManager.cpp                                                                       */
/*     Original Date : May 26th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for pipeline interaction.                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "PipelineManager.hpp"

using namespace QGPM;

// Connection : 
	Connection::Connection(void)
	{ }

	Connection::~Connection(void)
	{ }

// ConnectionToImageItem : 
	ConnectionToImageItem::ConnectionToImageItem(QGIC::ImageItem* _imageItem)
	 : 	imageItem(_imageItem)
	{
		if(imageItem==NULL)
			throw Exception("ConnectionToImageItem::ConnectionToImageItem - Image item is invalid (NULL).", __FILE__, __LINE__);

		// Lock : 
		imageItem->loadToDevice();
		imageItem->lockToDevice(true);

		QObject::connect(imageItem, SIGNAL(formatModified()),		this, SIGNAL(Connection::modified()));
		QObject::connect(imageItem, SIGNAL(unloadedFromDevice()),	this, SLOT(imageItemDestroyed()));
		QObject::connect(imageItem, SIGNAL(removed()), 			this, SLOT(imageItemDestroyed()));
		QObject::connect(imageItem, SIGNAL(destroyed()), 		this, SLOT(imageItemDestroyed()));
	}

	ConnectionToImageItem::~ConnectionToImageItem(void)
	{
		if(imageItem!=NULL)
			imageItem->lockToDevice(false);

		imageItem = NULL;
	}

	void ConnectionToImageItem::imageItemDestroyed(void)
	{
		imageItem = NULL;

		emit Connection::connectionClosed();
	}

	bool ConnectionToImageItem::isValid(void) const
	{
		return (imageItem!=NULL);
	}

	bool ConnectionToImageItem::selfTest(PipelineItem* _pipelineItem) const
	{
		return false;
	}

	const __ReadOnly_HdlTextureFormat& ConnectionToImageItem::getFormat(void) const
	{
		if(!isValid())
			throw Exception("ConnectionToImageItem::getFormat - Connection is invalid.", __FILE__, __LINE__);
		else
			return imageItem->getFormat();
	}

	HdlTexture& ConnectionToImageItem::getTexture(void)
	{
		if(!isValid())
			throw Exception("ConnectionToImageItem::getTexture - Connection is invalid.", __FILE__, __LINE__);
		else
			return imageItem->getTexture();
	}

// ConnectionToPipelineOutput :
	ConnectionToPipelineOutput::ConnectionToPipelineOutput(PipelineItem* _pipelineItem, int _outputIdx)
	 : 	pipelineItem(_pipelineItem),
		outputIdx(_outputIdx)
	{
		if(pipelineItem==NULL)
			throw Exception("ConnectionToPipleineOutput::ConnectionToPipleineOutput - Pipeline item is invalid (NULL).", __FILE__, __LINE__);

		QObject::connect(pipelineItem, SIGNAL(statusChanged()),	this,	SLOT(pipelineItemStatusChanged()));
		QObject::connect(pipelineItem, SIGNAL(removed()),	this, 	SLOT(pipelineItemDestroyed()));
		QObject::connect(pipelineItem, SIGNAL(destroyed()),	this, 	SLOT(pipelineItemDestroyed()));
	}

	ConnectionToPipelineOutput::~ConnectionToPipelineOutput(void)
	{
		pipelineItem	= NULL;
		outputIdx	= -1;
	}

	void ConnectionToPipelineOutput::pipelineItemStatusChanged(void)
	{
		if(pipelineItem!=NULL)
			emit Connection::statusChanged(pipelineItem->isValid());
	}

	void ConnectionToPipelineOutput::pipelineItemDestroyed(void)
	{
		// Do this notification only once.
		if(pipelineItem!=NULL)
		{
			pipelineItem	= NULL;
			outputIdx	= -1;

			emit Connection::connectionClosed();
		}
	}

	bool ConnectionToPipelineOutput::isValid(void) const
	{
		if(pipelineItem==NULL)
			return false;
		else
			return pipelineItem->isValid();
	}

	bool ConnectionToPipelineOutput::selfTest(PipelineItem* _pipelineItem) const
	{
		return (_pipelineItem==pipelineItem);
	}

	const __ReadOnly_HdlTextureFormat& ConnectionToPipelineOutput::getFormat(void) const
	{
		if(!isValid())
			throw Exception("ConnectionToPipleineOutput::getFormat - Connection is invalid.", __FILE__, __LINE__);
		else
			return pipelineItem->getOutputFormat(outputIdx);
	}

	HdlTexture& ConnectionToPipelineOutput::getTexture(void)	
	{
		if(!isValid())
			throw Exception("ConnectionToPipleineOutput::getFormat - Connection is invalid.", __FILE__, __LINE__);
		else
			return pipelineItem->out(outputIdx);
	}

	void ConnectionToPipelineOutput::safetyFuse(void)
	{
		pipelineItemDestroyed();
	}

// InputPortItem : 
	InputPortItem::InputPortItem(PipelineItem* _parentPipelineItem, int _portIdx)
	 : 	QTreeWidgetItem(InputItemType),
		parentPipelineItem(_parentPipelineItem),
		portIdx(_portIdx),
		connection(NULL)
	{
		if(_parentPipelineItem==NULL)
			throw Exception("InputPortItem::InputPortItem - Parent pipeline item cannot be undefined [Interal Error].", __FILE__, __LINE__); 

		setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
		std::cout << "InputPortItem::InputPortItem " << this << " - connection : " << connection << std::endl;
	}

	InputPortItem::~InputPortItem(void)
	{
		std::cout << "InputPortItem::~InputPortItem : " << this << std::endl;
	}

	void InputPortItem::connectionModified(void)
	{
		emit connectionContentModified(portIdx);
	}

	void InputPortItem::connectionStatusChanged(bool validity)
	{
		emit connectionStatusChanged(portIdx, validity);
	}	

	void InputPortItem::connectionDestroyed(void)
	{
		std::cout << "InputPortItem::connectionDestroyed : " << connection << " removed from input " << this << std::endl;
		delete connection;
		connection = NULL;
	
		emit connectionClosed(portIdx);
	}

	PipelineItem* InputPortItem::getParentPipelineItem(void) const
	{
		return parentPipelineItem;
	}

	QString InputPortItem::getName(void) const
	{
		return parentPipelineItem->getInputPortName(portIdx);
	}

	bool InputPortItem::isConnected(void) const
	{
		return (connection!=NULL);
	}

	void InputPortItem::connect(Connection* _connection)
	{
		std::cout << "InputPortItem::connect : Connecting " << _connection << " to " << this << " (old connection : " << connection << ")" << std::endl;

		if(connection!=NULL)
			InputPortItem::connectionDestroyed();

		connection = _connection;

		QObject::connect(connection, SIGNAL(modified(void)),		this, SLOT(connectionModified(void)));
		QObject::connect(connection, SIGNAL(statusChanged(bool)),	this, SLOT(connectionStatusChanged(bool)));
		QObject::connect(connection, SIGNAL(connectionClosed(void)), 	this, SLOT(connectionDestroyed(void)));

		emit connectionAdded(portIdx);
	}

	Connection* InputPortItem::getConnection(void)
	{
		return connection;
	}

	void InputPortItem::setName(std::string& name)
	{
		QString currentName = text(0);

		// If there is a change in name, it means a change a purpose.
		// Drop the connection to avoid misconnection.
		if(currentName!=QString::fromStdString(name) && !currentName.isEmpty())
			connectionDestroyed();

		setText(0, name.c_str());
	}

	InputPortItem* InputPortItem::getPtrFromGenericItem(QTreeWidgetItem* item)
	{
		if(item==NULL || item->type()!=InputItemType)
			return NULL;
		else
			return reinterpret_cast<InputPortItem*>(item->data(0, Qt::UserRole).value<void*>());
	}

// OutputPortItem :
	OutputPortItem::OutputPortItem(PipelineItem* _parentPipelineItem, int _portIdx)
	 : 	QTreeWidgetItem(OutputItemType),
		parentPipelineItem(_parentPipelineItem),
		portIdx(_portIdx)
	{
		if(_parentPipelineItem==NULL)
			throw Exception("OutputPortItem::OutputPortItem - Parent pipeline item cannot be undefined [Interal Error].", __FILE__, __LINE__); 

		setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
	}

	OutputPortItem::~OutputPortItem(void)
	{ }

	PipelineItem* OutputPortItem::getParentPipelineItem(void) const
	{
		return parentPipelineItem;
	}

	QString OutputPortItem::getName(void) const
	{
		return parentPipelineItem->getOutputPortName(portIdx);
	}

	ConnectionToPipelineOutput* OutputPortItem::getConnection(void)
	{
		ConnectionToPipelineOutput* connection = new ConnectionToPipelineOutput(getParentPipelineItem(), portIdx);

		QObject::connect(this, SIGNAL(discardConnection(void)), connection, SLOT(safetyFuse(void)));

		return connection;
	}

	void OutputPortItem::setName(std::string& name)
	{
		QString currentName = text(0);

		// If there is a change in name, it means a change a purpose.
		// Drop the connection to avoid misconnection.
		if(currentName!=QString::fromStdString(name) && !currentName.isEmpty())
			emit discardConnection();

		setText(0, name.c_str());
	}

	OutputPortItem* OutputPortItem::getPtrFromGenericItem(QTreeWidgetItem* item)
	{
		if(item==NULL || item->type()!=OutputItemType)
			return NULL;
		else
			return reinterpret_cast<OutputPortItem*>(item->data(0, Qt::UserRole).value<void*>());
	}

// PipelineItem :
	PipelineItem::PipelineItem(const std::string& _source, void* _identifier, const QObject* _referrer, const char* notificationMember)
	 : 	QTreeWidgetItem(PipelineHeaderItemType),
		referrer(_referrer),
		source(_source),
		inputFormatString("inputFormat%d"),
		identifier(NULL),
		pipeline(NULL),
		cellA(-1),
		cellB(-1),
		computeCount(0),
		inputsNode(InputsHeaderItemType),
		outputsNode(OutputsHeaderItemType)
	{
		inputsNode.setText(0, "Inputs");
		outputsNode.setText(0, "Outputs");

		addChild(&inputsNode);
		addChild(&outputsNode);

		setExpanded(true);

		QObject::connect(this, SIGNAL(referrerShowUp()),					referrer, SLOT(showUp()));
		QObject::connect(this, SIGNAL(compilationFailureNotification(void*, Exception)),	referrer, SLOT(compilationFailureNotification(void*, Exception)));

		// Start build : 
		preInterpret();
	}

	PipelineItem::~PipelineItem(void)
	{
		while(!inputPortItems.isEmpty())
		{
			delete inputPortItems.last();
			inputPortItems.pop_back();
		}

		while(!outputPortItems.isEmpty())
		{
			delete outputPortItems.last();
			outputPortItems.pop_back();
		}

		cellA = -1;
		cellB = -1;

		delete pipeline;

		identifier 	= NULL;	
		pipeline	= NULL;
	}

	std::string PipelineItem::getInputFormatName(int idx)
	{
		if(idx<0 || idx>=elements.mainPipelineInputs.size())
			throw Exception("PipelineItem::getInputFormatName - Index out of range.", __FILE__, __LINE__);
		else
		{
			QString result = inputFormatString;

			result.replace("%s", elements.mainPipelineInputs[idx].c_str());
			result.replace("%d",  QString::number(idx));

			return result.toStdString();
		}
	}

	void PipelineItem::preInterpret(void)
	{
		delete pipeline;
		pipeline = NULL;

		try
		{
			elements = loader.listElements(source);
		}
		catch(Exception& e)
		{
			emit compilationFailureNotification(identifier, e);
		}

		// Update ports : 
		refurnishPortItems();

		// Set name : 
		setText(0, elements.mainPipeline.c_str());
	}

	void PipelineItem::refurnishPortItems(void)
	{
		// Remove the ports which are in excess : 
		while(elements.mainPipelineInputs.size()<inputPortItems.size())
		{
			delete inputPortItems.last();
			inputPortItems.pop_back();
		}

		while(elements.mainPipelineOutputs.size()<outputPortItems.size())
		{
			delete outputPortItems.last();
			outputPortItems.pop_back();
		}

		// Update the existing port : 
		for(int k=0; k<inputPortItems.size(); k++)
			inputPortItems[k]->setName(elements.mainPipelineInputs[k]);

		for(int k=0; k<outputPortItems.size(); k++)
			outputPortItems[k]->setName(elements.mainPipelineOutputs[k]);

		// Create new ports : 
		for(int k=inputPortItems.size(); k<elements.mainPipelineInputs.size(); k++)
		{
			InputPortItem* inputPortItem = new InputPortItem(this, k);
			inputPortItem->setName(elements.mainPipelineInputs[k]);
			
			QObject::connect(inputPortItem, SIGNAL(connectionAdded(int)),			this, SLOT(connectionAdded(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionContentModified(int)),		this, SLOT(connectionContentModified(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionStatusChanged(int,bool)),	this, SLOT(connectionStatusChanged(int,bool)));
			QObject::connect(inputPortItem, SIGNAL(connectionClosed(int)),			this, SLOT(connectionClosed(int)));
			
			inputPortItems.push_back(inputPortItem);
			inputsNode.addChild(inputPortItem);

			emit pipelineInputPortAdded(inputPortItem);
		}

		for(int k=outputPortItems.size(); k<elements.mainPipelineOutputs.size(); k++)
		{
			OutputPortItem* outputPortItem = new OutputPortItem(this, k);
			outputPortItem->setName(elements.mainPipelineOutputs[k]);

			outputPortItems.push_back(outputPortItem);
			outputsNode.addChild(outputPortItem);

			emit pipelineOutputPortAdded(outputPortItem);
		}

		inputsNode.setText(0, tr("Inputs (%1)").arg(elements.mainPipelineInputs.size()));
		inputsNode.setExpanded(true);
		outputsNode.setText(0, tr("Outputs (%1)").arg(elements.mainPipelineOutputs.size()));
		outputsNode.setExpanded(true);
	}

	bool PipelineItem::checkConnections(void)
	{
		// Check that all connections are available : 
		for(QVector<InputPortItem*>::iterator it=inputPortItems.begin(); it!=inputPortItems.end(); it++)
		{
			if((*it)==NULL || (*it)->getConnection()==NULL || !(*it)->getConnection()->isValid())
				return false;
		}

		return true;
	}

	void PipelineItem::compile(void)
	{
		delete pipeline;
		pipeline = NULL;

		try
		{
			// Set the inputs format : 
			loader.clearRequiredElements();

			for(int k=0; k<elements.mainPipelineInputs.size(); k++)
			{
				std::string name = getInputFormatName(k);

				if(k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL && inputPortItems[k]->getConnection()->isValid())
				{
					std::cout << "PipelineItem::compile - Adding requirement : " << name << std::endl;
					loader.addRequiredElement(name, inputPortItems[k]->getConnection()->getFormat());
				}
				else
					std::cout << "PipelineItem::compile - Unable to add requirement : " << name << std::endl;
			}

			pipeline = loader(source, "Pipeline");
		}
		catch(Exception& e)
		{
			emit compilationFailureNotification(identifier, e);
		}
	}

	void PipelineItem::compute(void)
	{
		if(pipeline==NULL)
			return ;

		try
		{
			(*pipeline) << Pipeline::Reset;

			for(int k=0; k<pipeline->getNumInputPort(); k++)
			{
				if(k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL && inputPortItems[k]->getConnection()->isValid())
					(*pipeline) << inputPortItems[k]->getConnection()->getTexture();
			}

			// Compute : 
			(*pipeline) << Pipeline::Process;
		}
		catch(Exception& e)
		{
			// TODO : error message.
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}

	void PipelineItem::connectionAdded(int portIdx)
	{
		std::cout << "PipelineItem::connectionAdded" << std::endl;

		if(checkConnections())
		{
			// Recompile with the new format : 
			compile();
			// And run : 
			compute();
		}
	}

	void PipelineItem::connectionContentModified(int portIdx)
	{
		std::cout << "PipelineItem::connectionContentModified" << std::endl;
	}

	void PipelineItem::connectionStatusChanged(int portIdx, bool validity)
	{
		std::cout << "PipelineItem::connectionStatusChanged" << std::endl;
	}

	void PipelineItem::connectionClosed(int portIdx)
	{
		std::cout << "PipelineItem::connectionClosed" << std::endl;
	}

	QString PipelineItem::getName(void) const
	{
		if(!elements.mainPipeline.empty())
			return QString::fromStdString(elements.mainPipeline);
		else
			return QString("[Untitled Pipeline]");
	}

	QString PipelineItem::getInputPortName(int idx) const
	{
		QString res;

		if(idx>=0 && idx<elements.mainPipelineInputs.size())
			res = QString::fromStdString(elements.mainPipelineInputs[idx]);
		else
			res = tr("[Untitled input port #%1]").arg(idx);

		return res;
	}

	QString PipelineItem::getOutputPortName(int idx) const
	{
		QString res;

		if(idx>=0 && idx<elements.mainPipelineOutputs.size())
			res = QString::fromStdString(elements.mainPipelineOutputs[idx]);
		else
			res = tr("[Untitled output port #%1]").arg(idx);

		return res;
	}

	void PipelineItem::updateSource(const std::string& _source)
	{
		
	}

	bool PipelineItem::isValid(void) const
	{
		return (pipeline!=NULL);
	}

	const QVector<OutputPortItem*>&	PipelineItem::getOutputPortItems(void) const
	{
		return outputPortItems;
	}

	const __ReadOnly_HdlTextureFormat& PipelineItem::getOutputFormat(int idx)
	{
		if(!isValid())
			throw Exception("PipelineItem::getOutputFormat - Item is not valid.", __FILE__, __LINE__);
		else
			return pipeline->out(idx).format();
	}

	HdlTexture& PipelineItem::out(int idx)
	{
		if(!isValid())
			throw Exception("PipelineItem::getOutputFormat - Item is not valid.", __FILE__, __LINE__);
		else
			return pipeline->out(idx);
	}

	void PipelineItem::remove(void)
	{
		emit removed();
	}

// ConnectionsMenu::PotentialConnectionMap :
	ConnectionsMenu::PotentialConnectionMap::PotentialConnectionMap(void)
	{ }

	ConnectionsMenu::PotentialConnectionMap::~PotentialConnectionMap(void)
	{
		for(QMap<InputPortItem*, Connection*>::iterator it=connectionsMap.begin(); it!=connectionsMap.end(); it++)
			delete it.value();
		connectionsMap.clear();
	}

	void ConnectionsMenu::PotentialConnectionMap::add(InputPortItem* inputPortItem, QGIC::ImageItem* imageItem)
	{
		connectionsMap[inputPortItem] = reinterpret_cast<Connection*>(new ConnectionToImageItem(imageItem)); 
	}

	void ConnectionsMenu::PotentialConnectionMap::add(InputPortItem* inputPortItem, PipelineItem* pipelineItem, int outputIdx)
	{
		connectionsMap[inputPortItem] = reinterpret_cast<Connection*>(new ConnectionToPipelineOutput(pipelineItem, outputIdx));
	}

	void ConnectionsMenu::PotentialConnectionMap::apply(void)
	{
		for(QMap<InputPortItem*, Connection*>::iterator it=connectionsMap.begin(); it!=connectionsMap.end(); it++)
			it.key()->connect(it.value());
		connectionsMap.clear();	
	}

// ConnectionsMenu :
	ConnectionsMenu::ConnectionsMenu(QWidget* parent)
	 : 	QMenu("Connect", parent),
		noImageConnectionAction("(none)", this),
		noPipelineConnectionAction("(none)", this),
		imageItemsMenu("Images", this),
		pipelineItemsMenu("Pipelines", this)
	{
		addMenu(&imageItemsMenu);
		addMenu(&pipelineItemsMenu);

		noImageConnectionAction.setEnabled(false);
		noPipelineConnectionAction.setEnabled(false);

		QList<QTreeWidgetItem*> temporaryList;
		updateToSelection(temporaryList);
	}

	ConnectionsMenu::~ConnectionsMenu(void)
	{
		for(QMap<QAction*, PotentialConnectionMap*>::iterator it=potentialConnectionsMapMap.begin(); it!=potentialConnectionsMapMap.end(); it++)
			delete it.value();
		potentialConnectionsMapMap.clear();
	}

	void ConnectionsMenu::imageItemDestroyed(void)
	{
		QGIC::ImageItem* imageItem = reinterpret_cast<QGIC::ImageItem*>(QObject::sender());

		if(imageItem==NULL)
			return ;

		int idx = imageItems.indexOf(imageItem);

		if(idx>=0)
			imageItems.erase(imageItems.begin() + idx);
	}
	
	void ConnectionsMenu::pipelineItemDestroyed(void)
	{
		PipelineItem* pipelineItem = reinterpret_cast<PipelineItem*>(QObject::sender());

		if(pipelineItem==NULL)
			return ;

		int idx = pipelineItems.indexOf(pipelineItem);

		if(idx>=0)
			pipelineItems.erase(pipelineItems.begin() + idx);
	}

	void ConnectionsMenu::actionTriggered(void)
	{
		QAction* action = reinterpret_cast<QAction*>(QObject::sender());
	
		if(action==NULL)
			return ;
		
		QMap<QAction*, PotentialConnectionMap*>::iterator it = potentialConnectionsMapMap.find(action);

		if(it!=potentialConnectionsMapMap.end())
		{
			it.value()->apply();
			potentialConnectionsMapMap.erase(it);
			delete action;
		}
	}

	void ConnectionsMenu::actionDestroyed(void)
	{
		QAction* action = reinterpret_cast<QAction*>(QObject::sender());
	
		if(action==NULL)
			return ;

		QMap<QAction*, PotentialConnectionMap*>::iterator it = potentialConnectionsMapMap.find(action);

		if(it!=potentialConnectionsMapMap.end())
		{
			delete it.value();
			potentialConnectionsMapMap.erase(it);
		}
	}

	void ConnectionsMenu::addImageItem(QGIC::ImageItem* imageItem)	
	{
		if(imageItem==NULL)
			return ;

		imageItems.push_back(imageItem);
	}

	void ConnectionsMenu::addPipelineItem(QGPM::PipelineItem* pipelineItem)
	{
		if(pipelineItem==NULL)
			return ;

		pipelineItems.push_back(pipelineItem);
	}

	void ConnectionsMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		// Test the selection :
		bool allInput = !selection.empty();
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
			allInput = allInput && ((*it)->type()==InputItemType);
		
		if(!allInput)
		{
			imageItemsMenu.clear();
			imageItemsMenu.addAction(&noImageConnectionAction);
			pipelineItemsMenu.clear();
			pipelineItemsMenu.addAction(&noPipelineConnectionAction);
		}
		else
		{
			// Build Image Menus :
			imageItemsMenu.clear(); 

			if((imageItems.size() - selection.size() + 1)<0)
				imageItemsMenu.addAction(&noImageConnectionAction); // No possible actions corresponding to the input selection.
			else
			{
				for(int p=0; p<(imageItems.size() - selection.size() + 1); p++)
				{
					QString title;
	
					if(selection.size()==1)
						title = imageItems[p]->getName();
					else
						title = tr("Starting at %1...").arg(imageItems[p]->getName());
		
					QAction* action = imageItemsMenu.addAction(title);
	
					// Construct the potential connection map : 
					PotentialConnectionMap* potentialConnectionMap = new PotentialConnectionMap;

					for(int q=0; q<selection.size(); q++)
						potentialConnectionMap->add(InputPortItem::getPtrFromGenericItem(selection[q]), imageItems[p+q]);

					// Set the data : 		
					potentialConnectionsMapMap[action] = potentialConnectionMap;

					// Connect : 
					QObject::connect(action, SIGNAL(triggered(void)), this, SLOT(actionTriggered(void)));
					QObject::connect(action, SIGNAL(destroyed(void)), this, SLOT(actionDestroyed(void)));
				}
			}

			// Build pipeline menu : 
			pipelineItemsMenu.clear();
			pipelineItemsMenu.addAction(&noPipelineConnectionAction);
			// TODO
		}
	}

// PipelineManager :
	PipelineManager::PipelineManager(void)
	 : 	layout(this),
		menuBar(this),
		connectionsMenu(&menuBar)
	{
		treeWidget.setSelectionMode(QAbstractItemView::ExtendedSelection);

		layout.addWidget(&menuBar);
		layout.addWidget(&treeWidget);
		layout.setMargin(0);
		layout.setSpacing(0);	

		menuBar.addMenu(&connectionsMenu);

		QObject::connect(this, 		SIGNAL(pipelineItemAdded(QGPM::PipelineItem*)), &connectionsMenu, 	SLOT(addPipelineItem(QGPM::PipelineItem*)));
		QObject::connect(&treeWidget,	SIGNAL(itemSelectionChanged(void)),		this,			SLOT(itemSelectionChanged()));
	}

	PipelineManager::~PipelineManager(void)
	{
		for(QMap<void*, PipelineItem*>::iterator it=pipelineItems.begin(); it!=pipelineItems.end(); it++)
			delete it.value();
		pipelineItems.clear();
	}

	void PipelineManager::itemSelectionChanged(void)
	{
		QList<QTreeWidgetItem*> selection = treeWidget.selectedItems();
		connectionsMenu.updateToSelection(selection);
	}

	void PipelineManager::addImageItem(QGIC::ImageItem* imageItem)
	{
		connectionsMenu.addImageItem(imageItem);
	}

	void PipelineManager::compileSource(std::string _source, void* _identifier, const QObject* referrer, const char* notificationMember)
	{
		// Test if identifier already exists : 
		QMap<void*, PipelineItem*>::iterator it = pipelineItems.find(_identifier);

		// Update : 
		if(it!=pipelineItems.end())
			it.value()->updateSource(_source);
		else
		{
			// Create a new pipeline : 
			PipelineItem* pipelineItem = new PipelineItem(_source, _identifier, referrer, notificationMember);
			pipelineItems[_identifier] = pipelineItem;
			treeWidget.addTopLevelItem(pipelineItem);

			emit pipelineItemAdded(pipelineItem);
		}
	}

	void PipelineManager::removeSource(void* _identifier)
	{
		QMap<void*, PipelineItem*>::iterator it = pipelineItems.find(_identifier);
		
		if(it!=pipelineItems.end())
		{
			delete it.value();
			pipelineItems.erase(it);
		}
	}

// PipelineManagerSubWidget :
#ifdef __USE_QVGL__
	PipelineManagerSubWidget::PipelineManagerSubWidget(void)
	{
		setInnerWidget(&manager);
		setTitle("Pipeline Manager");
	}

	PipelineManagerSubWidget::~PipelineManagerSubWidget(void)
	{ }

	PipelineManager* PipelineManagerSubWidget::getManagerPtr(void)
	{
		return &manager;
	}
#endif

