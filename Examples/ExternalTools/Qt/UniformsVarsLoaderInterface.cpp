/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ResourceLoader.cpp                                                                        */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for manipulating uniform variables in pipelines.                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "UniformsVarsLoaderInterface.hpp"

// ValuesInterface :
	ValuesInterface::ValuesInterface(UniformsVarsLoader::Resource& _resource)
	 : 	resource(_resource),
		layout(this),
		signalMapper(this)
	{
		HdlDynamicData& object = resource.object();

		boxes.assign(object.getNumElements(), NULL);

		// Test the type of the resource and create the interface accordingly : 
		for(int i=0; i<object.getNumRows(); i++)
			for(int j=0; j<object.getNumColumns(); j++)
			{
				const int index = object.getIndex(i, j);

				// Create box : 
				boxes[index] = new QDoubleSpinBox;

				// Settings : 
				/// TODO
		
				// Copy the value :
				boxes[index]->setValue(object.get(i,j));

				// Signal mapper : 
				QObject::connect(boxes[index], SIGNAL(valueChanged(double)), &signalMapper, SLOT(map(void)));
				signalMapper.setMapping(boxes[index], object.getIndex(i, j));
	
				// Put in the grid : 
				layout.addWidget(boxes[index], i, j);
			}

		// Finished by conneting the SignalMapper : 
		QObject::connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(pushModificationToResource(int)));
	}

	ValuesInterface::~ValuesInterface(void)
	{
		boxes.clear();	
	}

	void ValuesInterface::pushModificationToResource(int index)
	{
		int 	i = 0,
			j = 0;
		
		resource.object().getCoordinates(index, i, j);

		// Get data : 
		const double v = boxes[index]->value();
	
		resource.object().set(v, i, j);

		// Set resource as modified : 
		resource.modified = true;

		// Emit modified signal : 
		emit modified();
	}

// UniformsVarsLoaderInterface :
	UniformsVarsLoaderInterface::UniformsVarsLoaderInterface(int type)
	 : 	QTreeWidgetItem(type)
	{ }

	UniformsVarsLoaderInterface::~UniformsVarsLoaderInterface(void)
	{
		itemRoots.clear();
	}

	// Private Tools : 
	QTreeWidgetItem* UniformsVarsLoaderInterface::addResource(UniformsVarsLoader::Resource& resource, QTreeWidgetItem* root)
	{
		// Create the node : 
		QTreeWidgetItem* newNode = new QTreeWidgetItem(type());

		root->addChild(newNode);

		// Create the interface widget : 
		ValuesInterface* valuesInterface = new ValuesInterface(resource);

		// Connect the signal : 
		QObject::connect(valuesInterface, SIGNAL(modified(void)), this, SIGNAL(modified(void)));

		// Set the name : 
		newNode->setText(0, tr("%1 [%2]").arg(resource.getName().c_str()).arg(glParamName(resource.object().getGLType()).c_str()));

		// Add the widget : 
		if(treeWidget()==NULL)
			throw Exception("UniformsVarsLoaderInterface::addResource - Internal error : no tree widget associated.", __FILE__, __LINE__);
		else
			treeWidget()->setItemWidget(newNode, 1, valuesInterface);

		return newNode;
	}

	QTreeWidgetItem* UniformsVarsLoaderInterface::addNode(UniformsVarsLoader::Node& node, QTreeWidgetItem* root)
	{
		// Create the node : 
		QTreeWidgetItem* newNode = new QTreeWidgetItem(type());

		// Set the name : 
		newNode->setText(0, tr("%1").arg(node.getName().c_str()));

		// Add as a child in root : 
		root->addChild(newNode);

		// Add all the sub-nodes : 
		for(UniformsVarsLoader::NodeIterator it=node.nodeBegin(); it!=node.nodeEnd(); it++)
			addNode(it->second, newNode);

		// Add all the resources : 
		for(UniformsVarsLoader::ResourceIterator it=node.resourceBegin(); it!=node.resourceEnd(); it++)
			addResource(it->second, newNode);

		return newNode;
	}

	QTreeWidgetItem* UniformsVarsLoaderInterface::addNodeAsRoot(UniformsVarsLoader::Node& node)
	{
		return addNode(node, this);
	}

	bool UniformsVarsLoaderInterface::isNodeListed(const std::string& name) const
	{
		return (itemRoots.find(name)!=itemRoots.end());
	}

	void UniformsVarsLoaderInterface::scanLoader(void)
	{
		for(UniformsVarsLoader::NodeIterator it=loader.rootNodeBegin(); it!=loader.rootNodeEnd(); it++)
		{
			if(!isNodeListed(it->first))
			{
				// Add node : 
				itemRoots[it->first] = addNodeAsRoot(it->second);
			}
		}
	}

	// Public Tools : 
	void UniformsVarsLoaderInterface::load(std::string source)
	{

	}	

	void UniformsVarsLoaderInterface::load(Pipeline& pipeline)
	{
		loader.load(pipeline, false);
		scanLoader();
	}

	bool UniformsVarsLoaderInterface::hasPipeline(const std::string& name) const
	{
		return false;
	}

	int UniformsVarsLoaderInterface::applyTo(Pipeline& pipeline, bool forceWrite) const
	{
		return loader.applyTo(pipeline, forceWrite);
	}

