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

		if(object.isFloatingPointType())
			floatBoxes.assign(object.getNumElements(), NULL);
		else
			integerBoxes.assign(object.getNumElements(), NULL);

		// Test the type of the resource and create the interface accordingly : 
		for(int i=0; i<object.getNumRows(); i++)
			for(int j=0; j<object.getNumColumns(); j++)
			{
				const int index = object.getIndex(i, j);

				// Create box : 
				if(object.isFloatingPointType())
					floatBoxes[index] = new QDoubleSpinBox;
				else
					integerBoxes[index] = new QSpinBox;

				// Settings : 
				if(object.isFloatingPointType())
				{
					floatBoxes[index]->setRange(-1e16, 1e16);
					floatBoxes[index]->setSingleStep(0.1);
				}
				else if(object.isBooleanType())
				{
					integerBoxes[index]->setRange(0, 1);
					integerBoxes[index]->setSingleStep(1);
				}
				else
				{
					integerBoxes[index]->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
					integerBoxes[index]->setSingleStep(1);

					if(object.isUnsignedType())
						integerBoxes[index]->setMinimum(0);
				}
		
				// Copy the value :
				if(object.isFloatingPointType())
					floatBoxes[index]->setValue(object.get(i,j));
				else
					integerBoxes[index]->setValue(object.get(i,j));

				// Signal mapper : 
				if(object.isFloatingPointType())
				{
					QObject::connect(floatBoxes[index], SIGNAL(valueChanged(double)), &signalMapper, SLOT(map(void)));
					signalMapper.setMapping(floatBoxes[index], object.getIndex(i, j));
				}
				else
				{
					QObject::connect(integerBoxes[index], SIGNAL(valueChanged(double)), &signalMapper, SLOT(map(void)));
					signalMapper.setMapping(integerBoxes[index], object.getIndex(i, j));
				}
	
				// Put in the grid : 
				if(object.isFloatingPointType())
					layout.addWidget(floatBoxes[index], i, j);
				else
					layout.addWidget(integerBoxes[index], i, j);
			}

		// Finished by conneting the SignalMapper : 
		QObject::connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(pushModificationToResource(int)));
	}

	ValuesInterface::~ValuesInterface(void)
	{
		floatBoxes.clear();
		integerBoxes.clear();
	}

	void ValuesInterface::pushModificationToResource(int index)
	{
		int 	i = 0,
			j = 0;
		
		resource.object().getCoordinates(index, i, j);

		// Get data : 
		double v = 0.0;

		if(resource.object().isFloatingPointType())
			v = floatBoxes[index]->value();
		else
			v = integerBoxes[index]->value();
	
		resource.object().set(v, i, j);

		// Set resource as modified : 
		resource.modified = true;

		// Emit modified signal : 
		emit modified();
	}

// UniformsVarsLoaderInterface :
	UniformsVarsLoaderInterface::UniformsVarsLoaderInterface(int type)
	 : 	QTreeWidgetItem(type)
	{
		setText(0, "Uniform Variables");
	}

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
		if(resource.object().getNumRows()>1)
			newNode->setText(0, tr("%1\n[%2]").arg(resource.getName().c_str()).arg(glParamName(resource.object().getGLType()).c_str()));
		else
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

