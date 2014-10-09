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
#include <QFileInfo>
#include <QFileDialog>
#include "QMenuTools.hpp"

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

		QObject::connect(imageItem, SIGNAL(formatModified()),		this, SIGNAL(formatModified()));
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

	const HdlAbstractTextureFormat& ConnectionToImageItem::getFormat(void) const
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

	const void* ConnectionToImageItem::getIdentification(void) const
	{
		return reinterpret_cast<void*>(imageItem);
	}

// ConnectionToPipelineOutput :
	ConnectionToPipelineOutput::ConnectionToPipelineOutput(PipelineItem* _pipelineItem, int _outputIdx)
	 : 	pipelineItem(_pipelineItem),
		outputIdx(_outputIdx)
	{
		if(pipelineItem==NULL)
			throw Exception("ConnectionToPipleineOutput::ConnectionToPipleineOutput - Pipeline item is invalid (NULL).", __FILE__, __LINE__);

		QObject::connect(pipelineItem, SIGNAL(computationFinished(int, QVector<const void*>)),	this,	SLOT(pipelineComputationFinished(int, QVector<const void*>)));
		QObject::connect(pipelineItem, SIGNAL(statusChanged()),					this,	SLOT(pipelineItemStatusChanged()));
		QObject::connect(pipelineItem, SIGNAL(removed()),					this, 	SLOT(pipelineItemDestroyed()));
		QObject::connect(pipelineItem, SIGNAL(destroyed()),					this, 	SLOT(pipelineItemDestroyed()));
	}

	ConnectionToPipelineOutput::~ConnectionToPipelineOutput(void)
	{
		pipelineItem	= NULL;
		outputIdx	= -1;
	}

	void ConnectionToPipelineOutput::pipelineComputationFinished(int count, QVector<const void*> resourceChain)
	{
		if(pipelineItem!=NULL)
			emit Connection::modified(resourceChain);
	}

	void ConnectionToPipelineOutput::pipelineItemStatusChanged(void)
	{
		if(pipelineItem!=NULL)
		{
			std::cout << "ConnectionToPipelineOutput::pipelineItemStatusChanged" << std::endl;
			emit Connection::statusChanged(pipelineItem->isValid());
		}
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
			return tr("%1 (Pipeline : %2)").arg(pipelineItem->getOutputPortName(outputIdx)).arg(pipelineItem->getName());
		else
			return "(invalid connection)";
	}

	bool ConnectionToPipelineOutput::selfTest(PipelineItem* _pipelineItem) const
	{
		return (_pipelineItem==pipelineItem);
	}

	const HdlAbstractTextureFormat& ConnectionToPipelineOutput::getFormat(void) const
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

	const void* ConnectionToPipelineOutput::getIdentification(void) const
	{
		return reinterpret_cast<void*>(pipelineItem);
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

	void InputPortItem::connectionModified(QVector<const void*> resourceChain)
	{
		emit connectionContentModified(portIdx, resourceChain);
	}

	void InputPortItem::connectionFormatModified(void)
	{
		if(connection!=NULL && connection->isValid())
			setToolTip(1, QGIC::ImageItem::getFormatToolTip(connection->getFormat(), connection->getName()));
		else
			setToolTip(1, "");

		emit connectionContentFormatModified(portIdx);
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
		setToolTip(1, "");

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
			QObject::connect(connection, SIGNAL(modified(QVector<const void*>)),	this, SLOT(connectionModified(QVector<const void*>)));
			QObject::connect(connection, SIGNAL(formatModified(void)),		this, SLOT(connectionFormatModified(void)));
			QObject::connect(connection, SIGNAL(statusChanged(bool)),		this, SLOT(connectionStatusChanged(bool)));
			QObject::connect(connection, SIGNAL(connectionClosed(void)), 		this, SLOT(connectionDestroyed(void)));

			// Set the connection : 
			std::cout << "InputPortItem::connect : " << connection->getName().toStdString() << std::endl;
			setText(1, connection->getName());
			setForeground(0, QBrush(Qt::green));

			if(connection->isValid())
				setToolTip(1, QGIC::ImageItem::getFormatToolTip(connection->getFormat(), connection->getName()));
			else
				setToolTip(1, "(invalid connection)");

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
		if(view==NULL && isConnected() && connection->isReady() && parentPipelineItem!=NULL)
		{
			view = new QVGL::View(&connection->getTexture(), tr("%1 (Input %2 of %3)").arg(text(0)).arg(portIdx).arg(parentPipelineItem->getName()));

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
		emit discardConnection();

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
		setText(1, "(invalid)");
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
		std::cout << "OutputPortItem::out - Index : " << portIdx << std::endl;

		if(!isValid())
			throw Exception("OutputPortItem::out - Output port is invalid.", __FILE__, __LINE__);
		else
			return getParentPipelineItem()->out(portIdx);
	}

	void OutputPortItem::save(void)
	{
		if(isValid())
		{
			QGIC::ImageItem imageItem(out(), getName());
			imageItem.save(getFilename());
		}
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
		
		// Update the infos : 
		setForeground(0, QBrush(Qt::red));
		setText(1, tr("(invalid)"));
		setToolTip(1, "");
	}

	void OutputPortItem::computationFinished(int computeCount, QVector<const void*> resourceChain)
	{
		std::cout << "OutputPortItem::computationFinished - Index : " << portIdx << std::endl;

		// Update the texture link, if necessary : 
		if(view!=NULL && parentPipelineItem!=NULL && parentPipelineItem->isValid())
			view->setTexture(&parentPipelineItem->out(portIdx));
	
		// Update the infos : 
		setForeground(0, QBrush(Qt::green));

		if(parentPipelineItem!=NULL && parentPipelineItem->isValid())
		{
			const HdlTexture& outputTexture = parentPipelineItem->out(portIdx);

			setText(1, tr("%1x%2 (%3)").arg(outputTexture.getWidth()).arg(outputTexture.getHeight()).arg(QGIC::ImageItem::getSizeString(outputTexture.getSize())));
			setToolTip(1, QGIC::ImageItem::getFormatToolTip(outputTexture, parentPipelineItem->getOutputPortName(portIdx)));
		}
	}

	void OutputPortItem::doubleClicked(int column)
	{
		if(view==NULL && parentPipelineItem!=NULL && parentPipelineItem->isValid() && parentPipelineItem->getComputationCount()>0)
		{
			view = new QVGL::View(&parentPipelineItem->out(portIdx), tr("%1 (Output %2 of %3)").arg(text(0)).arg(portIdx).arg(parentPipelineItem->getName()));

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
		uniformsNode(NULL),
		locked(false)
	{
		setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));

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

	void PipelineItem::updateText(void)
	{
		if(!elements.mainPipeline.empty())
			setText(0, QString::fromStdString(elements.mainPipeline));
		else
			setText(0, "(Untitled Pipeline)");

		QString lockStatus;
		QBrush brush;

		if(isLocked())
		{
			lockStatus = "[Locked]";
			brush = QBrush(Qt::red);
		}
		else
		{
			lockStatus = "Ready";
			brush = QBrush(Qt::green);
		}
		
		setText(1, lockStatus);
		setForeground(1, brush);
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

		// Update ports : 
		refurnishPortItems();

		updateText();
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
			
			QObject::connect(inputPortItem, SIGNAL(connectionAdded(int)),					this, 		SLOT(connectionAdded(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionContentModified(int, QVector<const void*>)),	this, 		SLOT(connectionContentModified(int, QVector<const void*>)));
			QObject::connect(inputPortItem, SIGNAL(connectionContentFormatModified(int)),			this, 		SLOT(connectionContentFormatModified(int)));
			QObject::connect(inputPortItem, SIGNAL(connectionStatusChanged(int,bool)),			this, 		SLOT(connectionStatusChanged(int,bool)));
			QObject::connect(inputPortItem, SIGNAL(connectionClosed(int)),					this, 		SLOT(connectionClosed(int)));
			QObject::connect(inputPortItem, SIGNAL(addViewRequest(QVGL::View*)),				this, 		SIGNAL(addViewRequest(QVGL::View*)));
			QObject::connect(inputPortItem, SIGNAL(updateColumnSize(void)),					this, 		SIGNAL(updateColumnSize(void)));
			
			inputPortItems.push_back(inputPortItem);
			inputsNode.addChild(inputPortItem);

			emit pipelineInputPortAdded(inputPortItem);
		}

		for(int k=outputPortItems.size(); k<elements.mainPipelineOutputs.size(); k++)
		{
			// Output port item : 
			OutputPortItem* outputPortItem = new OutputPortItem(this, k);
			outputPortItem->setName(elements.mainPipelineOutputs[k]);

			QObject::connect(this, 		SIGNAL(computationFinished(int, QVector<const void*>)),		outputPortItem,	SLOT(computationFinished(int, QVector<const void*>)));
			QObject::connect(outputPortItem,SIGNAL(addViewRequest(QVGL::View*)),				this, 		SIGNAL(addViewRequest(QVGL::View*)));
			QObject::connect(outputPortItem,SIGNAL(updateColumnSize(void)),					this,		SIGNAL(updateColumnSize(void)));
			QObject::connect(this, 		SIGNAL(pipelineDestroyed(void)),				outputPortItem,	SLOT(pipelineDestroyed(void)));

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
		bool allToSelf = !inputPortItems.isEmpty();

		// Check that all connections are available : 
		for(QVector<InputPortItem*>::iterator it=inputPortItems.begin(); it!=inputPortItems.end(); it++)
		{
			if((*it)==NULL || (*it)->getConnection()==NULL || (!(*it)->getConnection()->isValid() && !(*it)->getConnection()->selfTest(this)) )
				return false;
			else
				allToSelf = allToSelf && (*it)->getConnection()->selfTest(this);
		}

		return !allToSelf;
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

				const bool baseTest = (k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL);

				// Test the connection (take it as valid if it is invalid but part of this pipeline, a self connection) : 
				if(baseTest && inputPortItems[k]->getConnection()->isValid())
				{
					std::cout << "PipelineItem::compile - Adding requirement : " << name << std::endl;
					loader.addRequiredElement(name, inputPortItems[k]->getConnection()->getFormat());
				} 
				else if(baseTest && inputPortItems[k]->getConnection()->selfTest(this))
					std::cout << "PipelineItem::compile - Not adding requirement (self connection)." << std::endl;
				else 
					std::cout << "PipelineItem::compile - Unable to add requirement : " << name << std::endl;
			}

			pipeline = loader(source, "Pipeline");

			checkUniforms();

			checkCells();

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
		uniformsNode = new QGUI::UniformsLoaderInterface(UniformsHeaderItemType);

		addChild(uniformsNode);
		uniformsNode->load(*pipeline);

		QObject::connect(uniformsNode, SIGNAL(modified(void)), this, SLOT(uniformsModified(void)));

		emit updateColumnSize();
	}

	void PipelineItem::checkCells(void)
	{
		if(pipeline!=NULL)
		{
			bool selfConnectionTest = false;

			for(int k=0; k<elements.mainPipelineInputs.size(); k++)
			{
				if(k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL && inputPortItems[k]->getConnection()->isValid())
					selfConnectionTest = selfConnectionTest || inputPortItems[k]->getConnection()->selfTest(this);
			}

			if(selfConnectionTest)
			{
				if(cellA==-1)
					cellA = pipeline->getCurrentCellID();
		
				if(cellB==-1)
					cellB = pipeline->createBuffersCell();
			}
			else
				cellA = pipeline->getCurrentCellID();
		}
	}

	void PipelineItem::compute(QVector<const void*> resourceChain)
	{
		if(pipeline==NULL || isLocked())
			return ;

		// Detect a possible loop : 
		if(resourceChain.contains(this))
			return ; // abort computation to avoid an infinite loop.

		try
		{
			// Reserve some space to push the resources identifiers : 
			resourceChain.reserve(resourceChain.size() + pipeline->getNumInputPort());

			// Set the correct buffer, if needed : 
			if(cellA!=-1 && cellB!=-1)
			{
				if((computationCount%2)==0)
					pipeline->changeTargetBuffersCell(cellA);
				else
					pipeline->changeTargetBuffersCell(cellB);
			}

			// Reset any previous remaining call : 
			(*pipeline) << Pipeline::Reset;

			// Scan and add the inputs : 
			for(int k=0; k<pipeline->getNumInputPort(); k++)
			{
				if(k<inputPortItems.size() && inputPortItems[k]!=NULL && inputPortItems[k]->getConnection()!=NULL && inputPortItems[k]->getConnection()->isValid() && inputPortItems[k]->getConnection()->isReady())	
				{
					inputPortItems[k]->getConnection()->lock(true);
					(*pipeline) << inputPortItems[k]->getConnection()->getTexture();

					resourceChain.push_back( inputPortItems[k]->getConnection()->getIdentification() );
				}
				else 
					throw Exception("PipelineItem::compute -  Cannot use connection for port \"" + pipeline->getInputPortName(k) + "\" of " + pipeline->getFullName() + ".", __FILE__, __LINE__);
			}

			// Compute : 
			(*pipeline) << Pipeline::Process;

			computationCount++;

			std::cout << "PipelineItem::compute - computation count : " << computationCount << std::endl;

			// Generate the resource chain :
			emit computationFinished(computationCount, resourceChain);
		}
		catch(Exception& e)
		{
			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught while computing the pipeline %1.").arg(getName()), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}
	}

	void PipelineItem::deletePipeline(void)
	{
		delete pipeline;
		pipeline = NULL;

		cellA = -1;
		cellB = -1;
		computationCount = 0;

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

	void PipelineItem::connectionContentModified(int portIdx, QVector<const void*> resourceChain)
	{
		if(checkConnections())
		{
			// Run : 
			compute(resourceChain);
		}
	}

	void PipelineItem::connectionContentFormatModified(int portIdx)
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
		std::cout << "PipelineItem::connectionStatusChanged - Port : " << portIdx << std::endl;
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

	int PipelineItem::getNumInputPorts(void) const
	{
		return elements.mainPipelineInputs.size();
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

	int PipelineItem::getNumOutputPorts(void) const
	{
		return elements.mainPipelineOutputs.size();
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

		source = _source + "\n";

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

	const HdlAbstractTextureFormat& PipelineItem::getOutputFormat(int idx)
	{
		std::cout << "PipelineItem::getOutputFormat - Index : " << idx << std::endl;

		if(!isValid())
			throw Exception("PipelineItem::getOutputFormat - Item is not valid.", __FILE__, __LINE__);
		else
			return pipeline->out(idx).format();
	}

	HdlTexture& PipelineItem::out(int idx)
	{
		std::cout << "PipelineItem::out - Index : " << idx << std::endl;

		if(!isValid())
			throw Exception("PipelineItem::getOutputFormat - Item is not valid.", __FILE__, __LINE__);
		else
		{
			if(cellA!=-1 && cellB!=-1)
			{
				if((computationCount%2)==0)
					return pipeline->out(idx, cellB);
				else
					return pipeline->out(idx, cellA);
			}
			else
				return pipeline->out(idx);
		}
	}

	int PipelineItem::getComputationCount(void) const
	{
		return computationCount;
	}

	bool PipelineItem::isLocked(void) const
	{
		return locked;
	}

	void PipelineItem::lock(bool enabled)
	{
		locked = enabled;
		updateText();
	}

	void PipelineItem::renewBuffers(void)
	{
		if(pipeline!=NULL)
		{
			bool changed = false;

			if(cellA!=-1)
			{
				pipeline->removeBuffersCell(cellA);
				cellA = pipeline->createBuffersCell();
				pipeline->changeTargetBuffersCell(cellA);
				changed = true;
			}				

			if(cellB!=-1)
			{
				pipeline->removeBuffersCell(cellB);
				cellB = pipeline->createBuffersCell();
				changed = true;
			}

			if(changed && checkConnections())
				compute();
		}	
	}

	const QString& PipelineItem::getUniformsFilename(void) const
	{
		return uniformsFilename;
	}

	void PipelineItem::loadUniforms(QString filename)
	{
		if(filename.isEmpty())
			filename = uniformsFilename;

		if(!filename.isEmpty() && uniformsNode!=NULL)
		{
			uniformsNode->load(filename);
				
			// Update : 
			uniformsFilename = filename;
		}
	}

	void PipelineItem::saveUniforms(QString filename)
	{
		if(filename.isEmpty())
			filename = uniformsFilename;

		if(!filename.isEmpty() && uniformsNode!=NULL)
		{
			uniformsNode->save(filename);
			
			// Update : 
			uniformsFilename = filename;
		}
	}

	PipelineItem* PipelineItem::getPtrFromGenericItem(QTreeWidgetItem* item)
	{
		if(item==NULL || item->type()!=PipelineHeaderItemType)
			return NULL;
		else
			return reinterpret_cast<PipelineItem*>(item->data(0, Qt::UserRole).value<void*>());
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

	void ConnectionsMenu::buildMenu(void)
	{
		if(selectedInputPortItems.isEmpty())
		{
			imageItemsMenu.clear();
			pipelineItemsMenu.clear();
			setEnabled(false);
		}
		else
		{
			// Build Image Menus :
			imageItemsMenu.clear(); 

			if((imageItems.size() - selectedInputPortItems.size() + 1)<=0)
				imageItemsMenu.addAction(&noImageConnectionAction); // No possible actions corresponding to the input selection.
			else
			{
				for(int p=0; p<(imageItems.size() - selectedInputPortItems.size() + 1); p++)
				{
					QString title;
	
					if(selectedInputPortItems.size()==1)
						title = imageItems[p]->getName();
					else
						title = tr("Starting at %1...").arg(imageItems[p]->getName());
		
					QAction* action = imageItemsMenu.addAction(title);
					std::cout << "ConnectionsMenu::buildMenu - Adding action, parent : " << action->parentWidget() << " (" << this << ", " << &imageItemsMenu << ")" << std::endl;

					// Construct the potential connection map : 
					PotentialConnectionMap* potentialConnectionMap = new PotentialConnectionMap;

					for(int q=0; q<selectedInputPortItems.size(); q++)
						potentialConnectionMap->add(InputPortItem::getPtrFromGenericItem(selectedInputPortItems[q]), imageItems[p+q]);

					// Set the data : 		
					potentialConnectionsMapMap[action] = potentialConnectionMap;

					// Connect : 
					QObject::connect(action, SIGNAL(triggered(void)), this, SLOT(actionTriggered(void)));
					QObject::connect(action, SIGNAL(destroyed(void)), this, SLOT(actionDestroyed(void)));
				}
			}

			// Build pipeline menu : 
			pipelineItemsMenu.clear();
			int itemsCount = 0;

			for(QList<PipelineItem*>::iterator it=pipelineItems.begin(); it!=pipelineItems.end(); it++)
			{
				QMenu* subMenu = new QMenu((*it)->getName(), &pipelineItemsMenu);

				for(int p=0; p<((*it)->getNumOutputPorts() - selectedInputPortItems.size() + 1); p++)
				{
					QString title;
	
					if(selectedInputPortItems.size()==1)
						title = (*it)->getOutputPortName(p);
					else
						title = tr("Starting at %1...").arg((*it)->getOutputPortName(p));
		
					QAction* action = subMenu->addAction(title);
	
					// Construct the potential connection map : 
					PotentialConnectionMap* potentialConnectionMap = new PotentialConnectionMap;

					for(int q=0; q<selectedInputPortItems.size(); q++)
						potentialConnectionMap->add(InputPortItem::getPtrFromGenericItem(selectedInputPortItems[q]), (*it), p+q);

					// Set the data : 		
					potentialConnectionsMapMap[action] = potentialConnectionMap;

					// Connect : 
					QObject::connect(action, SIGNAL(triggered(void)), this, SLOT(actionTriggered(void)));
					QObject::connect(action, SIGNAL(destroyed(void)), this, SLOT(actionDestroyed(void)));

					itemsCount++;
				}

				if(!subMenu->actions().isEmpty())
					pipelineItemsMenu.addMenu(subMenu);
				else
					delete subMenu;
			}

			if(itemsCount==0)
				pipelineItemsMenu.addAction(&noPipelineConnectionAction);

			setEnabled(true);
		}
	}

	void ConnectionsMenu::imageItemDestroyed(void)
	{
		QGIC::ImageItem* imageItem = reinterpret_cast<QGIC::ImageItem*>(QObject::sender());

		if(imageItem==NULL)
			return ;

		int idx = imageItems.indexOf(imageItem);

		if(idx>=0)
		{
			imageItems.erase(imageItems.begin() + idx);
			buildMenu();
		}
	}
	
	void ConnectionsMenu::pipelineItemDestroyed(void)
	{
		PipelineItem* pipelineItem = reinterpret_cast<PipelineItem*>(QObject::sender());

		if(pipelineItem==NULL)
			return ;

		int idx = pipelineItems.indexOf(pipelineItem);

		if(idx>=0)
		{
			pipelineItems.erase(pipelineItems.begin() + idx);
			buildMenu();
		}
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

		buildMenu();
	}

	void ConnectionsMenu::addPipelineItem(QGPM::PipelineItem* pipelineItem)
	{
		if(pipelineItem==NULL)
			return ;

		QObject::connect(pipelineItem, SIGNAL(removed(void)), 	this, SLOT(pipelineItemDestroyed(void)));
		QObject::connect(pipelineItem, SIGNAL(destroyed(void)), this, SLOT(pipelineItemDestroyed(void)));

		pipelineItems.push_back(pipelineItem);

		buildMenu();
	}

	void ConnectionsMenu::addToMenu(QMenu& menu)
	{
		if(isEnabled())
		{
			//menu.addMenu(&imageItemsMenu);
			//menu.addMenu(&pipelineItemsMenu);
			duplicateMenu(&menu, imageItemsMenu);
			duplicateMenu(&menu, pipelineItemsMenu);
			menu.addSeparator();
		}
	}

	void ConnectionsMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		// Test the selection :
		bool allInput = !selection.empty();
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
			allInput = allInput && ((*it)->type()==InputItemType);
		
		selectedInputPortItems.clear();

		if(allInput)
		{
			for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
				selectedInputPortItems.push_back(InputPortItem::getPtrFromGenericItem(*it));
		}
		// Else : already cleared.
	
		std::cout << "ConnectionsMenu::updateToSelection - allInput : " << allInput << std::endl;

		buildMenu();
	}

// OutputsMenu :
	OutputsMenu::OutputsMenu(QWidget* parent)
	 :	QMenu("Outputs", parent),
		saveAction(NULL),
		saveAsAction(NULL),
		copyAsNewImageItemAction(NULL),
		copyAction(NULL)
	{
		saveAction			= addAction("Save", 			this, SLOT(save(void)));
		saveAsAction			= addAction("Save as", 			this, SLOT(saveAs(void)));
		copyAsNewImageItemAction 	= addAction("Copy as new image", 	this, SLOT(copyAsNewImageItem(void)));
		copyAction			= addAction("Copy",			this, SLOT(copy(void)));

		QList<QTreeWidgetItem*> selection;
		updateToSelection(selection);
	}

	OutputsMenu::~OutputsMenu(void)
	{ }

	void OutputsMenu::outputPortItemDestroyed(void)
	{
		OutputPortItem* outputPortItem = reinterpret_cast<OutputPortItem*>(QObject::sender());

		int idx = selectedOutputPortItems.indexOf(outputPortItem);

		if(idx>=0)
			selectedOutputPortItems.removeAt(idx);
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
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItems.begin(); it!=selectedOutputPortItems.end(); it++)
			save(*it);
	}

	void OutputsMenu::saveAs(OutputPortItem* outputPortItem)
	{
		if(outputPortItem!=NULL)
		{
			QFileInfo info(outputPortItem->getFilename());

			QString filename = QFileDialog::getSaveFileName(NULL, "Save Image", info.path(), "Image File (*.*)");

			if(!filename.isEmpty())
			{
				outputPortItem->setFilename(filename);
				outputPortItem->save();
			}
		}
	}

	void OutputsMenu::saveAs(void)
	{
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItems.begin(); it!=selectedOutputPortItems.end(); it++)
			saveAs(*it);
	}
	
	void OutputsMenu::copyAsNewImageItem(OutputPortItem* outputPortItem)
	{
		
	}

	void OutputsMenu::copyAsNewImageItem(void)
	{
		for(QList<OutputPortItem*>::iterator it=selectedOutputPortItems.begin(); it!=selectedOutputPortItems.end(); it++)
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
		if(selectedOutputPortItems.size()==1)
			copy(selectedOutputPortItems.front());
	}

	void OutputsMenu::addToMenu(QMenu& menu)
	{
		if(isEnabled())
		{
			menu.addAction(saveAction);
			menu.addAction(saveAsAction);
			menu.addAction(copyAsNewImageItemAction);
			menu.addAction(copyAction);
			menu.addSeparator();
		}
	}

	void OutputsMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		// Test the selection :
		bool allOutput = !selection.empty();
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
			allOutput = allOutput && ((*it)->type()==OutputItemType) && OutputPortItem::getPtrFromGenericItem(*it)!=NULL && OutputPortItem::getPtrFromGenericItem(*it)->isValid();
		
		selectedOutputPortItems.clear();

		if(!allOutput)
			setEnabled(false);
		else
		{
			for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
				selectedOutputPortItems.push_back(OutputPortItem::getPtrFromGenericItem(*it));

			setEnabled(true);
		}
	}

// PipelineMenu : 
	PipelineMenu::PipelineMenu(QWidget* parent)
	 :	QMenu("Pipeline", parent),	
		loadUniformsAction(NULL),
		saveUniformsAction(NULL),
		saveUniformsAsAction(NULL),
		toggleLockPipelineAction(NULL),
		renewBuffersAction(NULL),
		removePipelineAction(NULL),
		currentPipelineItem(NULL)
	{
		loadUniformsAction		= addAction("Load uniforms",		this, SLOT(loadUniforms(void)));
		saveUniformsAction		= addAction("Save uniforms",		this, SLOT(saveUniforms(void)));
		saveUniformsAsAction		= addAction("Save uniforms as...",	this, SLOT(saveUniformsAs(void)));
		toggleLockPipelineAction	= addAction("To Be Defined Dynamically",this, SLOT(toggleLockPipeline(void)));
		renewBuffersAction		= addAction("Renew Buffers",		this, SLOT(renewBuffers(void)));
		removePipelineAction		= addAction("Close pipeline",		this, SLOT(removePipeline(void)));

		setEnabled(false);
	}

	PipelineMenu::~PipelineMenu(void)
	{
		currentPipelineItem = NULL;
	}	

	void PipelineMenu::updateToggles(void)
	{
		if(currentPipelineItem!=NULL)
		{
			if(currentPipelineItem->isLocked())
				toggleLockPipelineAction->setText("Unlock Pipeline");
			else
				toggleLockPipelineAction->setText("Lock Pipeline");
		}
	}

	void PipelineMenu::loadUniforms(void)
	{
		if(currentPipelineItem!=NULL)
		{
			QFileInfo info(currentPipelineItem->getUniformsFilename());

			QString filename = QFileDialog::getOpenFileName(NULL, "Open Uniforms", info.path(), "Uniforms File (*.uvd *.txt)");

			if(!filename.isEmpty())
				currentPipelineItem->loadUniforms(filename);
		}
	}

	void PipelineMenu::saveUniforms(void)
	{
		if(currentPipelineItem!=NULL)
		{
			if(currentPipelineItem->getUniformsFilename().isEmpty())
				saveUniformsAs();
			else
				currentPipelineItem->saveUniforms();
		}
	}

	void PipelineMenu::saveUniformsAs(void)
	{
		if(currentPipelineItem!=NULL)
		{
			QFileInfo info(currentPipelineItem->getUniformsFilename());

			QString filename = QFileDialog::getSaveFileName(NULL, "Save Uniforms", info.path(), "Uniforms File (*.uvd *.txt)");

			if(!filename.isEmpty())
				currentPipelineItem->saveUniforms(filename);
		}
	}

	void PipelineMenu::toggleLockPipeline(void)
	{
		if(currentPipelineItem!=NULL)
		{
			currentPipelineItem->lock( !currentPipelineItem->isLocked() );
			
			updateToggles();		
		}
	}

	void PipelineMenu::renewBuffers(void)
	{
		if(currentPipelineItem!=NULL)
			currentPipelineItem->renewBuffers();
	}

	void PipelineMenu::removePipeline(void)
	{
		if(currentPipelineItem!=NULL)
			emit removePipeline(currentPipelineItem);
	}

	void PipelineMenu::addToMenu(QMenu& menu)
	{
		if(isEnabled())
		{	
			menu.addAction(loadUniformsAction);
			menu.addAction(saveUniformsAction);
			menu.addAction(saveUniformsAsAction);
			menu.addAction(toggleLockPipelineAction);
			menu.addAction(removePipelineAction);
			menu.addSeparator();
		}
	}

	void PipelineMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		if(!selection.isEmpty())
		{
			PipelineItem* targetPipelineItem = NULL;

			// Check that the current selection is a pipeline : 
			for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
			{
				PipelineItem* target = PipelineItem::getPtrFromGenericItem(PipelineManager::getRoot(*it));

				if(targetPipelineItem==NULL)
					targetPipelineItem = target;
				else if(target!=NULL && targetPipelineItem!=target)
				{
					targetPipelineItem = NULL;
					break;
				}
			}

			currentPipelineItem = targetPipelineItem;
			setEnabled(currentPipelineItem!=NULL);

			updateToggles();	
		}
		else
		{
			currentPipelineItem = NULL;
			setEnabled(false);
		}
	}

// PipelineManager :
	PipelineManager::PipelineManager(void)
	 : 	layout(this),
		menuBar(this),
		pipelineMenu(this),
		connectionsMenu(this),
		uniformsLinkMenu(UniformsHeaderItemType, this),
		outputsMenu(this),
		treeWidget(this)
	{
		layout.addWidget(&menuBar);
		layout.addWidget(&treeWidget);
		layout.setMargin(0);
		layout.setSpacing(0);	

		menuBar.addMenu(&pipelineMenu);
		menuBar.addMenu(&connectionsMenu);
		menuBar.addMenu(&uniformsLinkMenu);
		menuBar.addMenu(&outputsMenu);

		treeWidget.header()->close(); 
		treeWidget.setColumnCount(2);
		treeWidget.setIndentation(16);
		treeWidget.setContextMenuPolicy(Qt::CustomContextMenu);
		treeWidget.setSelectionMode(QAbstractItemView::ExtendedSelection);

		QObject::connect(this, 		SIGNAL(pipelineItemAdded(QGPM::PipelineItem*)), 	&connectionsMenu, 	SLOT(addPipelineItem(QGPM::PipelineItem*)));
		QObject::connect(&treeWidget,	SIGNAL(itemSelectionChanged(void)),			this,			SLOT(itemSelectionChanged()));
		QObject::connect(&treeWidget,	SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),	this,			SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
		QObject::connect(&treeWidget,	SIGNAL(customContextMenuRequested(const QPoint&)),	this,			SLOT(execCustomContextMenu(const QPoint&)));
		QObject::connect(&pipelineMenu,	SIGNAL(removePipeline(PipelineItem*)),			this,			SLOT(removePipeline(PipelineItem*)));
		QObject::connect(&outputsMenu,	SIGNAL(addImageItemRequest(QGIC::ImageItem*)),		this,			SIGNAL(addImageItemRequest(QGIC::ImageItem*)));
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
		pipelineMenu.updateToSelection(selection);
		connectionsMenu.updateToSelection(selection);
		uniformsLinkMenu.updateToSelection(selection);
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

	void PipelineManager::execCustomContextMenu(const QPoint& pos)
	{
		QMenu contextMenu(&treeWidget);
	
		connectionsMenu.addToMenu(contextMenu);
		outputsMenu.addToMenu(contextMenu);
		uniformsLinkMenu.addToMenu(contextMenu);
		pipelineMenu.addToMenu(contextMenu);

		contextMenu.exec(treeWidget.viewport()->mapToGlobal(pos));
	}
	
	void PipelineManager::removePipeline(PipelineItem* pipelineItem)
	{
		std::cout << "PipelineManager::removePipeline : " << pipelineItem << std::endl;

		QMap<void*, PipelineItem*>::iterator it=pipelineItems.begin();

		for(; it!=pipelineItems.end(); it++)
		{
			if(it.value()==pipelineItem)
				break ;
		}
	
		std::cout << "PipelineManager::removePipeline : " << (it!=pipelineItems.end()) << std::endl;

		if(it!=pipelineItems.end())
		{
			delete it.value();
			pipelineItems.erase(it);
		}
	}

	QTreeWidgetItem* PipelineManager::getRoot(QTreeWidgetItem* item)
	{
		QTreeWidgetItem* current = item;

		while(current->parent()!=NULL)
			current = current->parent();
	
		return current;
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

