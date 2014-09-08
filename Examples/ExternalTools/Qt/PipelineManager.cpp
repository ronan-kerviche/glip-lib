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

	void Connection::lock(bool enabled)
	{ }

// ConnectionToImageItem : 
	ConnectionToImageItem::ConnectionToImageItem(QGIC::ImageItem* _imageItem)
	 : 	imageItem(_imageItem)
	{
		if(imageItem==NULL)
			throw Exception("ConnectionToImageItem::ConnectionToImageItem - Image item is invalid (NULL).", __FILE__, __LINE__);

		QObject::connect(imageItem, SIGNAL(formatModified()),		this, SIGNAL(modified()));
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
		std::cout << "ConnectionToImageItem::imageItemDestroyed - " << imageItem << " from connection " << this << std::endl;

		imageItem = NULL;

		emit Connection::connectionClosed();
	}

	bool ConnectionToImageItem::isValid(void) const
	{
		return (imageItem!=NULL);
	}

	bool ConnectionToImageItem::isReady(void) const
	{
		if(!isValid())
			return false;
		else if(!imageItem->isOnDevice())
			return imageItem->loadToDevice();
		else
			return true;
	}

	QString ConnectionToImageItem::getName(void) const
	{
		if(isValid())
			return imageItem->getName();
		else
			return "(invalid connection)";
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
		{
			// Try to load on first need : 
			if(!imageItem->loadToDevice())
				throw Exception("ConnectionToImageItem::getTexture - Connection is not ready.", __FILE__, __LINE__);
			else
				return imageItem->getTexture();
		}
	}

	void ConnectionToImageItem::lock(bool enabled)
	{
		if(isValid())
			imageItem->lockToDevice(enabled);
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

	bool ConnectionToPipelineOutput::isReady(void) const
	{
		return isValid();
	}

	QString ConnectionToPipelineOutput::getName(void) const
	{
		if(isValid())
			return pipelineItem->getOutputPortName(outputIdx);
		else
			return "(invalid connection)";
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
		connection(NULL),
		view(NULL)
	{
		if(_parentPipelineItem==NULL)
			throw Exception("InputPortItem::InputPortItem - Parent pipeline item cannot be undefined [Interal Error].", __FILE__, __LINE__); 

		setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
		setForeground(0, QBrush(Qt::red));
		std::cout << "InputPortItem::InputPortItem " << this << " - connection : " << connection << std::endl;
	}

	InputPortItem::~InputPortItem(void)
	{
		std::cout << "InputPortItem::~InputPortItem : " << this << std::endl;
		delete connection;
		delete view;
		connection 	= NULL;
		view		= NULL;		
	}

	void InputPortItem::setText(int column, const QString& text)
	{
		QTreeWidgetItem::setText(column, text);
		emit updateColumnSize();
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
		delete view;
		view = NULL;
		delete connection;
		connection = NULL;
	
		setText(1, "");
		setForeground(0, QBrush(Qt::red));

		emit connectionClosed(portIdx);
	}

	void InputPortItem::viewClosed(void)
	{
		delete view;
		view = NULL;
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

		if(connection!=NULL)
		{
			QObject::connect(connection, SIGNAL(modified(void)),		this, SLOT(connectionModified(void)));
			QObject::connect(connection, SIGNAL(statusChanged(bool)),	this, SLOT(connectionStatusChanged(bool)));
			QObject::connect(connection, SIGNAL(connectionClosed(void)), 	this, SLOT(connectionDestroyed(void)));

			// Set the connection : 
			std::cout << "InputPortItem::connect : " << connection->getName().toStdString() << std::endl;
			setText(1, connection->getName());
			setForeground(0, QBrush(Qt::green));

			emit connectionAdded(portIdx);
		}
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

	void InputPortItem::doubleClicked(int column)
	{
		if(view==NULL && isConnected() && connection->isReady())
		{
			view = new QVGL::View(&connection->getTexture(), text(0));

			QObject::connect(view, SIGNAL(closed(void)), this, SLOT(viewClosed(void)));

			emit addViewRequest(view);
			view->show();
		}
		else if(view!=NULL)
			view->show();
	}

// OutputPortItem :
	OutputPortItem::OutputPortItem(PipelineItem* _parentPipelineItem, int _portIdx)
	 : 	QTreeWidgetItem(OutputItemType),
		parentPipelineItem(_parentPipelineItem),
		portIdx(_portIdx),
		view(NULL)
	{
		if(_parentPipelineItem==NULL)
			throw Exception("OutputPortItem::OutputPortItem - Parent pipeline item cannot be undefined [Interal Error].", __FILE__, __LINE__); 

		setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
		setForeground(0, QBrush(Qt::red));
	}

	OutputPortItem::~OutputPortItem(void)
	{
		delete view;
		view = NULL;
	}

	void OutputPortItem::setText(int column, const QString& text)
	{
		QTreeWidgetItem::setText(column, text);
		emit updateColumnSize();
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

	void OutputPortItem::viewClosed(void)
	{
		delete view;
		view = NULL;
	}

	PipelineItem* OutputPortItem::getParentPipelineItem(void) const
	{
		return parentPipelineItem;
	}

	QString OutputPortItem::getName(void) const
	{
		return parentPipelineItem->getOutputPortName(portIdx);
	}

	const QString& OutputPortItem::getFilename(void) const
	{
		return filename;
	}

	void OutputPortItem::setFilename(const QString& newFilename)
	{
		filename = newFilename;
	}

	bool OutputPortItem::isValid(void) const
	{
		return (getParentPipelineItem()!=NULL && getParentPipelineItem()->isValid());
	}

	ConnectionToPipelineOutput* OutputPortItem::getConnection(void)
	{
		ConnectionToPipelineOutput* connection = new ConnectionToPipelineOutput(getParentPipelineItem(), portIdx);

		QObject::connect(this, SIGNAL(discardConnection(void)), connection, SLOT(safetyFuse(void)));

		return connection;
	}

	HdlTexture& OutputPortItem::out(void)
	{
		if(!isValid())
			throw Exception("OutputPortItem::out - Output port is invalid.", __FILE__, __LINE__);
		else
			return getParentPipelineItem()->out(portIdx);
	}

	void OutputPortItem::save(void)
	{
		std::cout << "OutputPortItem::save" << std::endl;
	}

	OutputPortItem* OutputPortItem::getPtrFromGenericItem(QTreeWidgetItem* item)
	{
		if(item==NULL || item->type()!=OutputItemType)
			return NULL;
		else
			return reinterpret_cast<OutputPortItem*>(item->data(0, Qt::UserRole).value<void*>());
	}

	void OutputPortItem::pipelineDestroyed(void)
	{
		if(view!=NULL)
			view->setTexture(NULL);
		setForeground(0, QBrush(Qt::red));
	}

	void OutputPortItem::computationFinished(int computeCount)
	{
		// Update the texture link, if necessary : 
		if(view!=NULL && parentPipelineItem!=NULL && parentPipelineItem->isValid())
			view->setTexture(&parentPipelineItem->out(portIdx));
		setForeground(0, QBrush(Qt::green));
	}

	void OutputPortItem::doubleClicked(int column)
	{
		if(view==NULL && parentPipelineItem!=NULL && parentPipelineItem->isValid() && parentPipelineItem->getComputationCount()>0)
		{
			view = new QVGL::View(&parentPipelineItem->out(portIdx), text(0));

			QObject::connect(view, SIGNAL(closed(void)), this, SLOT(viewClosed(void)));

			emit addViewRequest(view);
			view->show();
		}
		else if(view!=NULL)
			view->show();
	}

// PipelineItem :
	PipelineItem::PipelineItem(void* _identifier, const QObject* _referrer)
	 : 	QTreeWidgetItem(PipelineHeaderItemType),
		referrer(_referrer),
		inputFormatString("inputFormat%d"),
		identifier(_identifier),
		pipeline(NULL),
		cellA(-1),
		cellB(-1),
		computationCount(0),
		inputsNode(InputsHeaderItemType),
		outputsNode(OutputsHeaderItemType),
		uniformsNode(NULL)
	{
		LayoutLoaderModule::addBasicModules(loader);

		inputsNode.setText(0, "Inputs");
		outputsNode.setText(0, "Outputs");

		addChild(&inputsNode);
		addChild(&outputsNode);

		setExpanded(true);

		QObject::connect(this, 		SIGNAL(showIdentifierWidget(void*)),				referrer, 	SLOT(showIdentifierWidget(void*)));
		QObject::connect(this, 		SIGNAL(compilationSuccessNotification(void*)),			referrer, 	SLOT(compilationSuccessNotification(void*)));
		QObject::connect(this, 		SIGNAL(compilationFailureNotification(void*, Exception)),	referrer, 	SLOT(compilationFailureNotification(void*, Exception)));
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

		delete uniformsNode;
		delete pipeline;

		uniformsNode	= NULL;
		pipeline 	= NULL;
		identifier 	= NULL;
	}

	void PipelineItem::setText(int column, const QString& text)
	{
		QTreeWidgetItem::setText(column, text);
		emit updateColumnSize();
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
		deletePipeline();

		try
		{
			elements = loader.listElements(source);
		}
		catch(Exception& e)
		{
			emit compilationFailureNotification(identifier, e);
		}

		// TODO : special update in the case of a failure.

		// Update ports : 
		refurnishPortItems();

		// Set name : 
		if(!elements.mainPipeline.empty())
			setText(0, QString::fromStdString(elements.mainPipeline));
		else
			setText(0, "(Untitled Pipeline)");
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
			
			QObject::connect(inputPortItem, SIGNAL(connectionAdded(int)),			this, 		SLOT(connectionAdded(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionContentModified(int)),		this, 		SLOT(connectionContentModified(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionStatusChanged(int,bool)),	this, 		SLOT(connectionStatusChanged(int,bool)));
			QObject::connect(inputPortItem, SIGNAL(connectionClosed(int)),			this, 		SLOT(connectionClosed(int)));
			QObject::connect(inputPortItem, SIGNAL(addViewRequest(QVGL::View*)),		this, 		SIGNAL(addViewRequest(QVGL::View*)));
			QObject::connect(inputPortItem, SIGNAL(updateColumnSize(void)),			this, 		SIGNAL(updateColumnSize(void)));
			
			inputPortItems.push_back(inputPortItem);
			inputsNode.addChild(inputPortItem);

			emit pipelineInputPortAdded(inputPortItem);
		}

		for(int k=outputPortItems.size(); k<elements.mainPipelineOutputs.size(); k++)
		{
			// Output port item : 
			OutputPortItem* outputPortItem = new OutputPortItem(this, k);
			outputPortItem->setName(elements.mainPipelineOutputs[k]);

			QObject::connect(this, 		SIGNAL(computationFinished(int)),		outputPortItem,	SLOT(computationFinished(int)));
			QObject::connect(outputPortItem,SIGNAL(addViewRequest(QVGL::View*)),		this, 		SIGNAL(addViewRequest(QVGL::View*)));
			QObject::connect(outputPortItem,SIGNAL(updateColumnSize(void)),			this,		SIGNAL(updateColumnSize(void)));
			QObject::connect(this, 		SIGNAL(pipelineDestroyed(void)),		outputPortItem,	SLOT(pipelineDestroyed(void)));

			outputPortItems.push_back(outputPortItem);
			outputsNode.addChild(outputPortItem);

			emit pipelineOutputPortAdded(outputPortItem);
		}

		inputsNode.setText(0, tr("Inputs (%1)").arg(elements.mainPipelineInputs.size()));
		inputsNode.setExpanded(true);
		outputsNode.setText(0, tr("Outputs (%1)").arg(elements.mainPipelineOutputs.size()));
		outputsNode.setExpanded(true);
		setExpanded(true);

		// Force update :
		emit updateColumnSize();
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
		deletePipeline();

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

			checkUniforms();

			// Emit success : 
			emit compilationSuccessNotification(identifier);
		}
		catch(Exception& e)
		{
			emit compilationFailureNotification(identifier, e);
		}
	}

	void PipelineItem::checkUniforms(void)
	{
		// Try a partial reload : 
		if(uniformsNode!=NULL)
		{
			uniformsNode->applyTo(*pipeline, true, true); // Silent!
			delete uniformsNode;
			uniformsNode = NULL;
		}
		
		// Create the uniforms profile : 
		uniformsNode = new UniformsVarsLoaderInterface(UniformsHeaderItemType);

		addChild(uniformsNode);
		uniformsNode->load(*pipeline);

		QObject::connect(uniformsNode, SIGNAL(modified(void)), this, SLOT(uniformsModified(void)));

		emit updateColumnSize();
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
				if(k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL && inputPortItems[k]->getConnection()->isValid() && inputPortItems[k]->getConnection()->isReady())	
				{
					inputPortItems[k]->getConnection()->lock(true);
					(*pipeline) << inputPortItems[k]->getConnection()->getTexture();
				}
				else 
					throw Exception("PipelineItem::compute -  Cannot use connection for port \"" + pipeline->getOutputPortName(k) + "\" of " + pipeline->getFullName() + ".", __FILE__, __LINE__);
			}

			// Compute : 
			(*pipeline) << Pipeline::Process;

			computationCount++;

			emit computationFinished(computationCount);
		}
		catch(Exception& e)
		{
			// TODO : error message.
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}

	void PipelineItem::deletePipeline(void)
	{
		delete pipeline;
		pipeline = NULL;

		emit pipelineDestroyed();
	}

	void PipelineItem::connectionAdded(int portIdx)
	{
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
		if(checkConnections())
		{
			// Recompile :
			compile();
			// And run : 
			compute();
		}
	}

	void PipelineItem::connectionStatusChanged(int portIdx, bool validity)
	{
		std::cout << "PipelineItem::connectionStatusChanged" << std::endl;
	}

	void PipelineItem::connectionClosed(int portIdx)
	{
		deletePipeline();
	}

	void PipelineItem::uniformsModified(void)
	{
		if(pipeline!=NULL && uniformsNode!=NULL)
		{
			uniformsNode->applyTo(*pipeline);
			compute();
		}
	}

	QString PipelineItem::getName(void) const
	{
		if(!elements.mainPipeline.empty())
			return QString::fromStdString(elements.mainPipeline);
		else
			return QString("(Untitled Pipeline)");
	}

	QString PipelineItem::getInputPortName(int idx) const
	{
		QString res;

		if(idx>=0 && idx<elements.mainPipelineInputs.size())
			res = QString::fromStdString(elements.mainPipelineInputs[idx]);
		else
			res = tr("(Untitled input port #%1)").arg(idx);

		return res;
	}

	QString PipelineItem::getOutputPortName(int idx) const
	{
		QString res;

		if(idx>=0 && idx<elements.mainPipelineOutputs.size())
			res = QString::fromStdString(elements.mainPipelineOutputs[idx]);
		else
			res = tr("(Untitled output port #%1)").arg(idx);

		return res;
	}

	void PipelineItem::updateSource(const std::string& _source, const std::string& path)
	{
		loader.clearPaths();
		loader.addToPaths(path);

		source = _source;

		preInterpret();

		if(checkConnections())
		{
			// Recompile :
			compile();
			// And run : 
			compute();
		}
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

	int PipelineItem::getComputationCount(void) const
	{
		return computationCount;
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
		pipelineItemsMenu("Pipeline Outputs", this)
	{
		addMenu(&imageItemsMenu);
		addMenu(&pipelineItemsMenu);

		noImageConnectionAction.setEnabled(false);
		noPipelineConnectionAction.setEnabled(false);

		QList<QTreeWidgetItem*> temporaryList;
		updateToSelection(temporaryList);

		setEnabled(false);
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

		QObject::connect(imageItem, SIGNAL(removed(void)), 	this, SLOT(imageItemDestroyed(void)));
		QObject::connect(imageItem, SIGNAL(destroyed(void)), 	this, SLOT(imageItemDestroyed(void)));

		imageItems.push_back(imageItem);
	}

	void ConnectionsMenu::addPipelineItem(QGPM::PipelineItem* pipelineItem)
	{
		if(pipelineItem==NULL)
			return ;

		QObject::connect(pipelineItem, SIGNAL(removed(void)), 	this, SLOT(pipelineItemDestroyed(void)));
		QObject::connect(pipelineItem, SIGNAL(destroyed(void)), this, SLOT(pipelineItemDestroyed(void)));

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
			pipelineItemsMenu.clear();
			setEnabled(false);
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

			// Activate : 
			setEnabled(true);
		}
	}

// OutputsMenu :
	OutputsMenu::OutputsMenu(QWidget* parent)
	 :	QMenu("Outputs", parent),
		saveAction(NULL),
		saveAsAction(NULL),
		copyAsNewImageItemAction(NULL),
		copyAction(NULL)
	{
		saveAction			= addAction("Save", 			this, SLOT(save(void)), 		QKeySequence::Save);
		saveAsAction			= addAction("Save as...", 		this, SLOT(saveAs(void)),		QKeySequence::SaveAs);
		copyAsNewImageItemAction 	= addAction("Copy as new image", 	this, SLOT(copyAsNewImageItem(void)));
		copyAction			= addAction("Copy",			this, SLOT(copy(void)),			QKeySequence::Copy);

		QList<QTreeWidgetItem*> selection;
		updateToSelection(selection);
	}

	OutputsMenu::~OutputsMenu(void)
	{ }

	void OutputsMenu::outputPortItemDestroyed(void)
	{
		OutputPortItem* outputPortItem = reinterpret_cast<OutputPortItem*>(QObject::sender());

		int idx = selectedOutputPortItem.indexOf(outputPortItem);

		if(idx>=0)
			selectedOutputPortItem.removeAt(idx);
	}

	void OutputsMenu::save(OutputPortItem* outputPortItem)
	{
		if(outputPortItem!=NULL)
		{
			if(outputPortItem->getFilename().isEmpty())
				saveAs(outputPortItem);
			else
				outputPortItem->save();
		}
	}

	void OutputsMenu::save(void)
	{
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItem.begin(); it!=selectedOutputPortItem.end(); it++)
			save(*it);
	}

	void OutputsMenu::saveAs(OutputPortItem* outputPortItem)
	{
		
	}

	void OutputsMenu::saveAs(void)
	{
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItem.begin(); it!=selectedOutputPortItem.end(); it++)
			saveAs(*it);
	}
	
	void OutputsMenu::copyAsNewImageItem(OutputPortItem* outputPortItem)
	{
		
	}

	void OutputsMenu::copyAsNewImageItem(void)
	{
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItem.begin(); it!=selectedOutputPortItem.end(); it++)
			copyAsNewImageItem(*it);
	}

	void OutputsMenu::copy(OutputPortItem* outputPortItem)
	{
		if(outputPortItem!=NULL && outputPortItem->isValid())
		{
			QGIC::ImageItem* imageItem = new QGIC::ImageItem(outputPortItem->out(), outputPortItem->getName());

			imageItem->copyToClipboard();

			delete imageItem;
		}
	}

	void OutputsMenu::copy(void)
	{
		if(selectedOutputPortItem.size()==1)
			copy(selectedOutputPortItem.front());
	}

	void OutputsMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		// Test the selection :
		bool allOutput = !selection.empty();
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
			allOutput = allOutput && ((*it)->type()==OutputItemType) && OutputPortItem::getPtrFromGenericItem(*it)!=NULL && OutputPortItem::getPtrFromGenericItem(*it)->isValid();
		
		if(!allOutput)
		{
			selectedOutputPortItem.clear();
			setEnabled(false);
		}
		else
		{
			selectedOutputPortItem.clear();

			for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
				selectedOutputPortItem.push_back(OutputPortItem::getPtrFromGenericItem(*it));

			setEnabled(true);
		}
	}

// PipelineManager :
	PipelineManager::PipelineManager(void)
	 : 	layout(this),
		menuBar(this),
		connectionsMenu(&menuBar),
		outputsMenu(&menuBar)
	{
		layout.addWidget(&menuBar);
		layout.addWidget(&treeWidget);
		layout.setMargin(0);
		layout.setSpacing(0);	

		menuBar.addMenu(&connectionsMenu);
		menuBar.addMenu(&outputsMenu);

		treeWidget.header()->close(); 
		treeWidget.setColumnCount(2);
		treeWidget.setIndentation(16);
		treeWidget.setContextMenuPolicy(Qt::CustomContextMenu);
		treeWidget.setSelectionMode(QAbstractItemView::ExtendedSelection);

		QObject::connect(this, 		SIGNAL(pipelineItemAdded(QGPM::PipelineItem*)), &connectionsMenu, 	SLOT(addPipelineItem(QGPM::PipelineItem*)));
		QObject::connect(&treeWidget,	SIGNAL(itemSelectionChanged(void)),		this,			SLOT(itemSelectionChanged()));
		QObject::connect(&treeWidget,	SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,			SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
		QObject::connect(&outputsMenu,	SIGNAL(addImageItemRequest(QGIC::ImageItem*)),	this,			SIGNAL(addImageItemRequest(QGIC::ImageItem*)));
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

		// Update menu : 
		connectionsMenu.updateToSelection(selection);
		outputsMenu.updateToSelection(selection);
	}

	void PipelineManager::itemDoubleClicked(QTreeWidgetItem* item, int column)
	{
		// Dispatch the double click events : 
		if(item->type()==InputItemType)
		{
			InputPortItem* inputPortItem = InputPortItem::getPtrFromGenericItem(item);

			if(inputPortItem!=NULL)
				inputPortItem->doubleClicked(column);
		}
		else if(item->type()==OutputItemType)
		{
			OutputPortItem* outputPortItem = OutputPortItem::getPtrFromGenericItem(item);

			if(outputPortItem!=NULL)
				outputPortItem->doubleClicked(column);
		}
	}

	void PipelineManager::addImageItem(QGIC::ImageItem* imageItem)
	{
		connectionsMenu.addImageItem(imageItem);
	}

	void PipelineManager::compileSource(std::string source, std::string path, void* identifier, const QObject* referrer)
	{
		// Test if identifier already exists : 
		QMap<void*, PipelineItem*>::iterator it = pipelineItems.find(identifier);

		// Update : 
		if(it!=pipelineItems.end())
			it.value()->updateSource(source, path);
		else
		{
			// Create a new pipeline : 
			PipelineItem* pipelineItem = new PipelineItem(identifier, referrer);
			pipelineItems[identifier] = pipelineItem;
			treeWidget.addTopLevelItem(pipelineItem);

			QObject::connect(pipelineItem, SIGNAL(addViewRequest(QVGL::View*)), 	this, SIGNAL(addViewRequest(QVGL::View*)));
			QObject::connect(pipelineItem, SIGNAL(updateColumnSize(void)),		this, SLOT(updateColumnSize(void)));

			emit pipelineItemAdded(pipelineItem);

			pipelineItem->updateSource(source, path);
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

	void PipelineManager::updateColumnSize(void)
	{
		for(int k=0; k<treeWidget.columnCount(); k++)
			treeWidget.resizeColumnToContents(k);
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

