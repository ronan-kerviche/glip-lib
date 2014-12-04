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
#include "QMenuTools.hpp"
#include <QMessageBox>

using namespace QGUI;

// VariableRecord :
	VariableRecord*			VariableRecord::referenceRecord = NULL;
	QVector<VariableRecord*> 	VariableRecord::records;

	VariableRecord::VariableRecord(void)
	 :	QObject(NULL),
		name("REFERENCE"),
		modification(-1),
		object(NULL)
	{ }

	VariableRecord::VariableRecord(const QString& _name, const GLenum& type, QObject* parent)
	 :	QObject(parent),
		name(_name),
		modification(0),
		object(NULL)
	{ 
		object = HdlDynamicData::build(type);

		records.push_back(this);

		if(referenceRecord!=NULL)
			emit referenceRecord->recordAdded(this);
	}

	VariableRecord::~VariableRecord(void)
	{
		delete object;
		object = NULL;

		if(this==referenceRecord)
			referenceRecord = NULL;

		int idx = records.indexOf(this);

		if(idx>=0)
			records.remove(idx);
	}
	
	const QString& VariableRecord::getName(void) const
	{
		return name;
	}

	int VariableRecord::getModificationIndex(void) const
	{
		return modification;
	}
	
	const HdlDynamicData& VariableRecord::data(void) const
	{
		if(object==NULL)
			throw Exception("VariableRecord::data - Data is invalid.", __FILE__, __LINE__);
		else
			return (*object);
	}	

	HdlDynamicData& VariableRecord::data(void)
	{
		if(object==NULL)
			throw Exception("VariableRecord::data - Data is invalid.", __FILE__, __LINE__);
		else
			return (*object);
	}

	void VariableRecord::declareUpdate(void)
	{
		// Update the modification indices of all the other variables :
		for(QVector<VariableRecord*>::iterator it=records.begin(); it!=records.end(); it++)
			(*it)->modification++;
		
		// Reset mine :
		modification = 0;

		emit updated();
	}
	
	const VariableRecord* VariableRecord::getRecord(const QString& name)
	{
		for(QVector<VariableRecord*>::iterator it=records.begin(); it!=records.end(); it++)
		{
			if((*it)->name==name)
				return (*it);
		}

		return NULL;
	}

	const QVector<VariableRecord*>& VariableRecord::getRecords(void)
	{
		return records;
	}

	const VariableRecord* VariableRecord::getReferenceRecord(void)
	{
		if(referenceRecord==NULL)
			referenceRecord = new VariableRecord;
	
		return referenceRecord;
	}

// QTreeWidgetSpecial :
	QTreeWidgetSpecial::QTreeWidgetSpecial(QWidget* parent)
	 :	QTreeWidget(parent)
	{ }

	void QTreeWidgetSpecial::scrollContentsBy(int dx, int dy)
	{
		QTreeWidget::scrollContentsBy(dx, dy);

		//Update the geometries of the widgets : 
		updateGeometries();
	}

// ValuesInterface :
	ValuesInterface::ValuesInterface(UniformsLoader::Resource& _resource, QWidget* parent)
	 : 	QWidget(parent),
		resource(_resource),
		layout(this),
		signalMapper(this),
		variableLink(NULL)	
	{
		layout.setMargin(0);
		layout.setSpacing(0);

		HdlDynamicData& object = resource.object();

		if(object.isFloatingPointType())
			floatBoxes.fill(NULL, object.getNumElements());
		else
			integerBoxes.fill(NULL, object.getNumElements());

		// Test the type of the resource and create the interface accordingly : 
		for(int i=0; i<object.getNumRows(); i++)
		{
			for(int j=0; j<object.getNumColumns(); j++)
			{
				const int index = object.getIndex(i, j);

				// Create box : 
				if(object.isFloatingPointType())
				{
					floatBoxes[index] = new QDoubleSpinBox(this);
					floatBoxes[index]->setDecimals(3); 				
				}
				else
					integerBoxes[index] = new QSpinBox(this);

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
					QObject::connect(integerBoxes[index], SIGNAL(valueChanged(int)), &signalMapper, SLOT(map(void)));
					signalMapper.setMapping(integerBoxes[index], object.getIndex(i, j));
				}
	
				// Put in the grid : 
				if(object.isFloatingPointType())
					layout.addWidget(floatBoxes[index], i, j);
				else
					layout.addWidget(integerBoxes[index], i, j);
			}
		}

		// Add the connection layout :
		layout.addWidget(&linkLabel, object.getNumRows(), 0, 1, -1);
		linkLabel.setTextFormat(Qt::RichText);
		unlink();

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

		std::cout << "ValuesInterface::pushModificationToResource - Pushing " << index << ", " << i << ", " << j << " = " << v << std::endl;

		// Set resource as modified : 
		resource.modified = true;

		// Emit modified signal : 
		emit modified();
	}

	void ValuesInterface::pushAllModificationsToResource(bool emitSignal)
	{
		blockSignals(true);

		for(int j=0; j<resource.object().getNumColumns(); j++)
		{
			for(int i=0; i<resource.object().getNumRows(); i++)
			{
				// Get data : 
				double v = 0.0;
				const int index = resource.object().getIndex(i, j);

				if(resource.object().isFloatingPointType())
					v = floatBoxes[index]->value();
				else
					v = integerBoxes[index]->value();
	
				resource.object().set(v, i, j);
			}
		}

		blockSignals(false);

		// Set resource as modified : 
		resource.modified = true;

		// Emit modified signal : 
		if(emitSignal)
			emit modified();
	}

	void ValuesInterface::recordUpdated(void)
	{
		VariableRecord* record = reinterpret_cast<VariableRecord*>(QObject::sender());

		if(variableLink!=NULL && variableLink==record)
		{	
			resource.object() = variableLink->data();
			pullModificationFromResource();
		}
	}

	void ValuesInterface::recordDestroyed(void)
	{
		VariableRecord* record = reinterpret_cast<VariableRecord*>(QObject::sender());
		
		if(record==variableLink)
			unlink();
	}	

	const UniformsLoader::Resource& ValuesInterface::getResource(void) const
	{
		return resource;
	}

	UniformsLoader::Resource& ValuesInterface::getResource(void)
	{
		return resource;
	}

	void ValuesInterface::setVariableLink(const VariableRecord* lnk)
	{
		if(lnk==NULL)
			unlink();
		else
		{
			variableLink = lnk;
			QObject::connect(variableLink, SIGNAL(updated(void)),	this, SLOT(recordUpdated(void)));
			QObject::connect(variableLink, SIGNAL(destroyed(void)), this, SLOT(recordDestroyed(void)));
			linkLabel.setText(tr("<font color=\"#FFFFFF\">Linked to %1</i></font>").arg(lnk->getName()));
		}
	}

	void ValuesInterface::autoLink(void)
	{
		const HdlDynamicData& object = resource.object();

		const VariableRecord* target = VariableRecord::getRecord(QString::fromStdString(resource.getName()));

		if(target!=NULL && object.getGLType()==target->data().getGLType())
			setVariableLink(target);	
	}

	void ValuesInterface::unlink(void)
	{
		if(variableLink!=NULL)
			QObject::disconnect(variableLink, 0, this, 0); // Disconnect all signals.
		
		variableLink = NULL;
		linkLabel.setText("<font color=\"#000000\"><i>Not linked</i></font>");
	}	
	
	void ValuesInterface::pullModificationFromResource(void)
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

		emit modified();
	}

	ValuesInterface* ValuesInterface::getPtrFromGenericItem(QTreeWidgetItem* item, const int type)
	{
		if(item==NULL || item->type()!=type)
			return NULL;
		else
			return reinterpret_cast<ValuesInterface*>(item->data(1, Qt::UserRole).value<void*>());
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
		ValuesInterface* valuesInterface = new ValuesInterface(resource, treeWidget());

		// Connect the signal : 
		QObject::connect(valuesInterface, SIGNAL(modified(void)), this, SIGNAL(modified(void)));

		// Set the name : 
		if(resource.object().getNumRows()>1)
			newNode->setText(0, tr("%1\n[%2]").arg(QString::fromStdString(resource.getName())).arg(glParamName(resource.object().getGLType()).c_str()));
		else
			newNode->setText(0, tr("%1 [%2]").arg(QString::fromStdString(resource.getName())).arg(glParamName(resource.object().getGLType()).c_str()));

		newNode->setData(0, Qt::UserRole, QString::fromStdString(resource.getName()));
		newNode->setData(1, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(valuesInterface)));

		// Add the widget : 
		if(treeWidget()==NULL)
			throw Exception("UniformsLoaderInterface::addResource - Internal error : no tree widget associated.", __FILE__, __LINE__);
		else
			treeWidget()->setItemWidget(newNode, 1, valuesInterface);

		// Auto-link attempt :
		valuesInterface->autoLink();

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

				// Try to find the corresponding data node : 
				if(node.subNodeExists(name))
					count += updateNode(node.subNode(name), *it, updateOnly);
			}
			else // Is a leaf :
			{
				const std::string name = (*it)->data(0, Qt::UserRole).toString().toStdString();

				// Try to find the corresponding data resource : 
				if(node.resourceExists(name))
				{
					if(treeWidget()==NULL)
						throw Exception("UniformsLoaderInterface::updateNode - Internal error : no tree widget associated.", __FILE__, __LINE__);
					else
					{
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
		if(treeWidget()==NULL)
			throw Exception("UniformsLoaderInterface::scanLoader - Internal error : no tree widget associated.", __FILE__, __LINE__);

		for(UniformsLoader::NodeIterator it=loader.rootNodeBegin(); it!=loader.rootNodeEnd(); it++)
		{
			const bool test = isNodeListed(it->first);

			if(!test && !updateOnly)
			{
				// Add node : 
				itemRoots[it->first] = addNodeAsRoot(it->second);
			}
			else if(test)
			{
				// Update node : 
				std::map<const std::string, QTreeWidgetItem*>::iterator itInternal = itemRoots.find(it->first);
				int c = updateNode(it->second, itInternal->second, updateOnly);

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

// UniformsLinkMenu :
	UniformsLinkMenu::UniformsLinkMenu(int _type, QWidget* parent)
	 :	QMenu("Uniforms Links", parent),
		type(_type),
		unlinkAction(NULL)
	{	
		unlinkAction = addAction("Unlink", this, SLOT(unlink(void)));

		// Prepare the link to receive the records :
		const VariableRecord* reference = VariableRecord::getReferenceRecord();		
		QObject::connect(reference, SIGNAL(recordAdded(const VariableRecord*)), this, SLOT(addRecord(const VariableRecord*)));

		// Add the current actions :
		const QVector<VariableRecord*>& currentRecords = VariableRecord::getRecords();
		for(QVector<VariableRecord*>::const_iterator it=currentRecords.begin(); it!=currentRecords.end(); it++)
			addRecord(*it);

		// Reset :
		QList<QTreeWidgetItem*> emptyList;
		updateToSelection(emptyList);
	}
	
	UniformsLinkMenu::~UniformsLinkMenu(void)
	{
		recordActions.clear();
		clear();
	}	

	const VariableRecord* UniformsLinkMenu::getRecordFromAction(QAction* action)
	{
		return reinterpret_cast<const VariableRecord*>(action->data().value<void*>());
	}

	void UniformsLinkMenu::addRecord(const VariableRecord* record)
	{
		// Connect :
		QObject::connect(record, SIGNAL(destroyed(void)), this, SLOT(recordDestroyed(void)));

		// Create a corresponding action :
		QAction* action = addAction(record->getName(), this, SLOT(setLink()));
		action->setData(QVariant::fromValue(reinterpret_cast<void*>(const_cast<VariableRecord*>(record))));
		
		recordActions[record] = action;
	}

	void UniformsLinkMenu::recordDestroyed(void)
	{
		VariableRecord* record = reinterpret_cast<VariableRecord*>(QObject::sender());

		QMap<const VariableRecord*, QAction*>::iterator it = recordActions.find(record);

		if(it!=recordActions.end())
		{
			delete it.value();
			recordActions.erase(it);
		}
	}	

	void UniformsLinkMenu::setLink(void)
	{
		QAction* action = reinterpret_cast<QAction*>(QObject::sender());
		const VariableRecord* link = getRecordFromAction(action);

		if(link!=NULL)
		{
			for(QMap<QTreeWidgetItem*, ValuesInterface*>::iterator it=currentSelection.begin(); it!=currentSelection.end(); it++)
				it.value()->setVariableLink(link);
		}
	}

	void UniformsLinkMenu::unlink(void)
	{
		for(QMap<QTreeWidgetItem*, ValuesInterface*>::iterator it=currentSelection.begin(); it!=currentSelection.end(); it++)
			it.value()->unlink();	
	}

	void UniformsLinkMenu::addToMenu(QMenu& menu)
	{
		if(isEnabled())
			duplicateMenu(&menu, *this, true);
	}

	void UniformsLinkMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{	
		GLenum glType = GL_NONE;
		currentSelection.clear();
	
		int rowCount = -1; // not set yet.
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
		{
			if((*it)->type()==type)
			{
				ValuesInterface* valuesInterface = ValuesInterface::getPtrFromGenericItem(*it, type);

				if(valuesInterface!=NULL && (glType==GL_NONE || glType==valuesInterface->getResource().object().getGLType()))
				{
					glType = valuesInterface->getResource().object().getGLType();

					currentSelection[*it] = valuesInterface;	
				}
				else
				{
					currentSelection.clear();
					break;
				}
			}
			else
			{
				currentSelection.clear();
				break;
			}
		}

		// Set the right menus :
		if(!currentSelection.isEmpty())
		{
			for(QMap<const VariableRecord*, QAction*>::iterator it=recordActions.begin(); it!=recordActions.end(); it++)
				it.value()->setEnabled(it.key()->data().getGLType()==glType);
		
			setEnabled(true);
		}
		else
			setEnabled(false);
	}

