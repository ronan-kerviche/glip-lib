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

#include "UniformsLoaderInterface.hpp"
#include <QMessageBox>

// ValuesInterface :
	ValuesInterface::ValuesInterface(UniformsLoader::Resource& _resource)
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

	void ValuesInterface::pullModificationToResource(void)
	{
		blockSignals(true);

		for(int j=0; j<resource.object().getNumColumns(); j++)
		{
			for(int i=0; i<resource.object().getNumRows(); i++)
			{
				int index = resource.object().getIndex(i, j);
	
				if(resource.object().isFloatingPointType())
					floatBoxes[index]->setValue(resource.object().get(i, j));
				else
					integerBoxes[index]->setValue(resource.object().get(i, j));

				resource.modified = true;
			}
		}

		blockSignals(false);
	}

// UniformsLoaderInterface :
	UniformsLoaderInterface::UniformsLoaderInterface(int type)
	 : 	QTreeWidgetItem(type)
	{
		setText(0, "Uniform Variables");
	}

	UniformsLoaderInterface::~UniformsLoaderInterface(void)
	{
		itemRoots.clear();
	}

	// Private Tools : 
	QTreeWidgetItem* UniformsLoaderInterface::addResource(UniformsLoader::Resource& resource, QTreeWidgetItem* root)
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
			newNode->setText(0, tr("%1\n[%2]").arg(QString::fromStdString(resource.getName())).arg(glParamName(resource.object().getGLType()).c_str()));
		else
			newNode->setText(0, tr("%1 [%2]").arg(QString::fromStdString(resource.getName())).arg(glParamName(resource.object().getGLType()).c_str()));

		newNode->setData(0, Qt::UserRole, QString::fromStdString(resource.getName()));

		// Add the widget : 
		if(treeWidget()==NULL)
			throw Exception("UniformsLoaderInterface::addResource - Internal error : no tree widget associated.", __FILE__, __LINE__);
		else
			treeWidget()->setItemWidget(newNode, 1, valuesInterface);

		return newNode;
	}

	QTreeWidgetItem* UniformsLoaderInterface::addNode(UniformsLoader::Node& node, QTreeWidgetItem* root)
	{
		// Create the node : 
		QTreeWidgetItem* newNode = new QTreeWidgetItem(type());

		// Set the name : 
		newNode->setText(0, tr("%1").arg(node.getName().c_str()));

		// Add as a child in root : 
		root->addChild(newNode);

		// Add all the sub-nodes : 
		for(UniformsLoader::NodeIterator it=node.nodeBegin(); it!=node.nodeEnd(); it++)
			addNode(it->second, newNode);

		// Add all the resources : 
		for(UniformsLoader::ResourceIterator it=node.resourceBegin(); it!=node.resourceEnd(); it++)
			addResource(it->second, newNode);

		newNode->setExpanded(true);

		return newNode;
	}

	QTreeWidgetItem* UniformsLoaderInterface::addNodeAsRoot(UniformsLoader::Node& node)
	{
		return addNode(node, this);
	}

	int UniformsLoaderInterface::updateNode(UniformsLoader::Node& node, QTreeWidgetItem* nodeItem, bool updateOnly)
	{
		int count = 0;
		QList<QTreeWidgetItem*> children = nodeItem->takeChildren();

		for(QList<QTreeWidgetItem*>::iterator it=children.begin(); it!=children.end(); it++)
		{
			// Is a node itself : 
			if((*it)->childCount()>0)
			{
				const std::string name = (*it)->text(0).toStdString();

				std::cout << "    Subnode : " << name << std::endl;

				// Try to find the corresponding data node : 
				if(node.subNodeExists(name))
					count += updateNode(node.subNode(name), *it, updateOnly);
			}
			else // Is a leaf :
			{
				const std::string name = (*it)->data(0, Qt::UserRole).toString().toStdString();

				std::cout << "    Subresource : " << name << std::endl;

				// Try to find the corresponding data resource : 
				if(node.resourceExists(name))
				{
					if(treeWidget()==NULL)
						throw Exception("UniformsLoaderInterface::updateNode - Internal error : no tree widget associated.", __FILE__, __LINE__);
					else
					{
						std::cout << "UniformsLoaderInterface::updateNode - updating node : " << nodeItem << std::endl;
						delete (*it);
						
						(*it) = addResource(node.resource(name), nodeItem);
	
						count++;
					}
				}
			}
		}

		nodeItem->insertChildren(0, children);
		nodeItem->setExpanded(true);

		return count;
	}

	bool UniformsLoaderInterface::isNodeListed(const std::string& name) const
	{
		return (itemRoots.find(name)!=itemRoots.end());
	}

	void UniformsLoaderInterface::scanLoader(bool updateOnly)
	{
		std::cout << "UniformsLoaderInterface::scanLoader - updateOnly : " << updateOnly << std::endl;

		if(treeWidget()==NULL)
			throw Exception("UniformsLoaderInterface::scanLoader - Internal error : no tree widget associated.", __FILE__, __LINE__);

		for(UniformsLoader::NodeIterator it=loader.rootNodeBegin(); it!=loader.rootNodeEnd(); it++)
		{
			const bool test = isNodeListed(it->first);
			std::cout << "  Node : " << it->first << std::endl;

			if(!test && !updateOnly)
			{
				// Add node : 
				itemRoots[it->first] = addNodeAsRoot(it->second);
			}
			else if(test)
			{
				std::cout << "  Updating..." << std::endl;

				// Update node : 
				std::map<const std::string, QTreeWidgetItem*>::iterator itInternal = itemRoots.find(it->first);
				int c = updateNode(it->second, itInternal->second, updateOnly);

				std::cout << "  Count : " << c << std::endl;

				if(c>0)
				{
					emit modified();
					itInternal->second->setExpanded(true);
				}
			}
		}
		setExpanded(true);
	}

	// Public Tools : 
	void UniformsLoaderInterface::load(Pipeline& pipeline)
	{
		try
		{
			loader.load(pipeline, true);
			scanLoader();
		}
		catch(Exception& e)
		{
			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught while loading uniforms value from a pipeline."), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}
	}

	void UniformsLoaderInterface::load(const QString& filename, bool updateOnly)
	{
		try
		{
			loader.load(filename.toStdString(), true); // Replace values.
			scanLoader(updateOnly);
		}
		catch(Exception& e)
		{
			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught while loading uniforms value from file : \"%1\"").arg(filename), QMessageBox::Ok);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}
	}

	bool UniformsLoaderInterface::hasPipeline(const std::string& name) const
	{
		return false;
	}

	void UniformsLoaderInterface::save(const QString& filename)
	{
		loader.writeToFile(filename.toStdString());
	}

	int UniformsLoaderInterface::applyTo(Pipeline& pipeline, bool forceWrite, bool silent) const
	{
		return loader.applyTo(pipeline, forceWrite, silent);
	}

