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
		pipelineItem	= NULL;
		outputIdx	= -1;

		emit Connection::connectionClosed();
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

// PipelineItem :
	PipelineItem::PipelineItem(const std::string& _source, void* _identifier, const QObject* _referrer, const char* notificationMember)
	 : 	QTreeWidgetItem(PipelineHeaderItemType),
		referrer(_referrer),
		source(_source),
		inputFormatString("inputFormat%d"),
		identifier(NULL),
		pipelineLayout(NULL),
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
		for(QList<QTreeWidgetItem*>::iterator it=inputItems.begin(); it!=inputItems.end(); it++)
			delete (*it);
		inputItems.clear();

		for(QList<QTreeWidgetItem*>::iterator it=outputItems.begin(); it!=outputItems.end(); it++)
			delete (*it);
		outputItems.clear();

		cellA = -1;
		cellB = -1;

		delete pipelineLayout;
		delete pipeline;

		identifier 	= NULL;	
		pipelineLayout	= NULL;
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
			result.replace("%d", QString(idx));

			return result.toStdString();
		}
	}

	void PipelineItem::preInterpret(void)
	{
		delete pipelineLayout;
		pipelineLayout = NULL;
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

		// Clear previous lists : 
		for(QList<QTreeWidgetItem*>::iterator it=inputItems.begin(); it!=inputItems.end(); it++)
			delete (*it);
		inputItems.clear();

		for(QList<QTreeWidgetItem*>::iterator it=outputItems.begin(); it!=outputItems.end(); it++)
			delete (*it);
		outputItems.clear();

		// Add inputs :
		int kInput = 0; 
		for(std::vector<std::string>::iterator it=elements.mainPipelineInputs.begin(); it!=elements.mainPipelineInputs.end(); it++)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(InputItem);
			item->setText(0, it->c_str());
			item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
			item->setData(1, Qt::UserRole, kInput);

			inputsNode.addChild(item);
			inputItems.push_back(item);

			kInput++;
		}
		inputsNode.setExpanded(true);

		// Add outputs : 
		int kOutput = 0; 
		for(std::vector<std::string>::iterator it=elements.mainPipelineOutputs.begin(); it!=elements.mainPipelineOutputs.end(); it++)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(OutputItem);
			item->setText(0, it->c_str());
			item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(this)));
			item->setData(1, Qt::UserRole, kOutput);

			outputsNode.addChild(item);
			outputItems.push_back(item);

			kOutput++;
		}
		outputsNode.setExpanded(true);

		// Set name : 
		setText(0, elements.mainPipeline.c_str());

		// Number of connections : 	
		while(elements.mainPipelineInputs.size()>inputConnectionsList.size())
			inputConnectionsList.push_back(NULL);

		while(elements.mainPipelineInputs.size()<inputConnectionsList.size())
			inputConnectionsList.pop_back();
	}

	bool PipelineItem::checkConnections(void)
	{
		// Check that all connections are available : 
		for(QVector<Connection*>::iterator it=inputConnectionsList.begin(); it!=inputConnectionsList.end(); it++)
		{
			if((*it)==NULL || !(*it)->isValid())
				return false;
		}

		return true;
	}

	void PipelineItem::interpret(void)
	{
		delete pipelineLayout;
		pipelineLayout = NULL;
		delete pipeline;
		pipeline = NULL;

		try
		{
			// Set the inputs format : 
			loader.clearRequiredElements();

			for(int k=0; k<elements.mainPipelineInputs.size(); k++)
			{
				std::string name = getInputFormatName(k);

				if(k<inputConnectionsList.size() && inputConnectionsList[k]!=NULL && inputConnectionsList[k]->isValid())
					loader.addRequiredElement(name, inputConnectionsList[k]->getFormat());
			}

			pipelineLayout = new PipelineLayout(loader(source)); 
		}
		catch(Exception& e)
		{
			emit compilationFailureNotification(identifier, e);
		}
	}

	void PipelineItem::compile(void)
	{
		if(pipelineLayout==NULL)
			return ;

		delete pipeline;
		pipeline = NULL;

		try
		{
			pipeline = new Pipeline(*pipelineLayout, "Pipeline");
		}
		catch(Exception& e)
		{
			// TODO : error message.
			std::cerr << "Exception caught : " << std::endl;
			std::cerr << e.what() << std::endl;
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
				if(k<inputConnectionsList.size() && inputConnectionsList[k]!=NULL && inputConnectionsList[k]->isValid())
					(*pipeline) << inputConnectionsList[k]->getTexture();
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

	void PipelineItem::connectionStatusChanged(bool validity)
	{
		
	}

	void PipelineItem::connectionDestroyed(void)
	{
		
	}

	void PipelineItem::updateSource(const std::string& _source)
	{
		
	}

	bool PipelineItem::isValid(void) const
	{
		return (pipeline!=NULL);
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

	void PipelineItem::makeConnection(QMap<QTreeWidgetItem*, Connection*> connectionsMap)
	{
		
	}

// PipelineManager :
	PipelineManager::PipelineManager(void)
	 : 	layout(this),
		menuBar(this)
	{
		layout.addWidget(&menuBar);
		layout.addWidget(&treeWidget);
		layout.setMargin(0);
		layout.setSpacing(0);		
	}

	PipelineManager::~PipelineManager(void)
	{
		for(QMap<void*, PipelineItem*>::iterator it=pipelineItems.begin(); it!=pipelineItems.end(); it++)
			delete it.value();
		pipelineItems.clear();
	}

	void PipelineManager::addImageItem(QGIC::ImageItem* imageItem)
	{
		
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

