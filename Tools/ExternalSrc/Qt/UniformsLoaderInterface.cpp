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
		#ifdef __USE_QVGL__
			, vectorID(QVGL::MouseState::InvalidVectorID),
			colorID(QVGL::MouseState::InvalidColorID)
		#endif
	{
		HdlDynamicData& object = resource.object();

		if(object.isFloatingPointType())
			floatBoxes.fill(NULL, object.getNumElements());
		else
			integerBoxes.fill(NULL, object.getNumElements());

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

		#ifdef __USE_QVGL__
			// Prepare management for the user interface.
			/*if(mouseState!=NULL)
			{
				// Auto link based on variable name, if possible :				
				if(resource.object().getNumRows()==2 && resource.object().getNumColumns()==1)
				{
					vectorID = QVGL::MouseState::getVectorIDFromName(resource.getName());
		
					copyVectorFromMouseState();
				}
				else if(resource.object().getNumRows()==3 && resource.object().getNumColumns()==1)
				{
					colorID = QVGL::MouseState::getColorIDFromName(resource.getName());

					copyColorFromMouseState();	
				}

				// Connect :
				QObject::connect(mouseState, SIGNAL(modified(void)), this, SLOT(mouseStateModified(void)));
			}*/	
		#endif
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
		
				std::cout << "Index : " << index << " = " << v << std::endl;
	
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

	const UniformsLoader::Resource& ValuesInterface::getResource(void) const
	{
		return resource;
	}

	UniformsLoader::Resource& ValuesInterface::getResource(void)
	{
		return resource;
	}

#ifdef __USE_QVGL__
	void ValuesInterface::setVectorLink(const QVGL::MouseState::VectorID& lnk)
	{
		if(resource.object().getNumRows()==2 && resource.object().getNumColumns()==1)
			vectorID = lnk;
	}

	void ValuesInterface::setColorLink(const QVGL::MouseState::ColorID& lnk)
	{
		if(resource.object().getNumRows()==3 && resource.object().getNumColumns()==1)
			colorID = lnk;
	}

	bool ValuesInterface::copyVectorFromMouseState(const QVGL::MouseState* mouseState)
	{
		if(mouseState!=NULL && vectorID!=QVGL::MouseState::InvalidVectorID && mouseState->isVectorModified(vectorID))
		{
			const QPointF& v = mouseState->getVector(vectorID);
	
			// Copy the value :
			blockSignals(true);

			if(resource.object().isFloatingPointType())
			{
				const double 	x = v.x(),
						y = v.y();
				std::cout << "ValuesInterface::copyVectorFromMouseState (1) : " << x << ", " << y << std::endl;
				floatBoxes[0]->setValue(x);
				floatBoxes[1]->setValue(y);
				std::cout << "ValuesInterface::copyVectorFromMouseState (2) : " << floatBoxes[0]->value() << ", " << floatBoxes[1]->value() << std::endl;
			}
			else
			{
				const int 	x = v.x(),
						y = v.y();
				integerBoxes[0]->setValue(x);
				integerBoxes[1]->setValue(y);
			}

			blockSignals(false);

			// Push :
			pushAllModificationsToResource(false);

			return true;
		}
		else
			return false;
	}

	bool ValuesInterface::copyColorFromMouseState(const QVGL::MouseState* mouseState)
	{
		if(mouseState!=NULL && colorID!=QVGL::MouseState::InvalidColorID && mouseState->isColorModified(colorID))
		{
			const QColor& c = mouseState->getColor(colorID);
	
			// Copy the value :
			blockSignals(true);

			if(resource.object().isFloatingPointType())
			{
				floatBoxes[0]->setValue(c.red());
				floatBoxes[1]->setValue(c.green());
				floatBoxes[2]->setValue(c.blue());
			}
			else
			{
				integerBoxes[0]->setValue(c.red());
				integerBoxes[1]->setValue(c.green());
				integerBoxes[2]->setValue(c.blue());
			}
	
			blockSignals(false);
			
			// Push :
			pushAllModificationsToResource(false);

			return true;
		}
		else
			return false;
	}
#endif

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

	ValuesInterface* ValuesInterface::getPtrFromGenericItem(QTreeWidgetItem* item, const int type)
	{
		if(item==NULL || item->type()!=type)
			return NULL;
		else
			return reinterpret_cast<ValuesInterface*>(item->data(1, Qt::UserRole).value<void*>());
	}

// UniformsLoaderInterface :
	#ifdef __USE_QVGL__
	UniformsLoaderInterface::UniformsLoaderInterface(int type, const QVGL::MouseState* _mouseState)
	#else
	UniformsLoaderInterface::UniformsLoaderInterface(int type)
	#endif
	 : 	QTreeWidgetItem(type)
		#ifdef __USE_QVGL__
		, mouseState(_mouseState)
		#endif
	{
		setText(0, "Uniform Variables");
		
		#ifdef __USE_QVGL__
			if(mouseState!=NULL)
				QObject::connect(mouseState, SIGNAL(updated(void)), this, SLOT(applyModificationFromMouseState(void)));
		#endif
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
		newNode->setData(1, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(valuesInterface)));

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

	int UniformsLoaderInterface::updateNodeWithMouseState(QTreeWidgetItem* nodeItem)
	{
		static int modCount = 0;
		modCount++;
		std::cout << "UniformsLoaderInterface::updateNodeWithMouseState : " << modCount << std::endl;

		if(nodeItem->childCount()==0)
		{
			ValuesInterface* valuesInterface = ValuesInterface::getPtrFromGenericItem(nodeItem, type());
			
			if(valuesInterface->copyVectorFromMouseState(mouseState) || valuesInterface->copyColorFromMouseState(mouseState))
				return 1;
			else
				return 0;
		}
		else
		{
			int count = 0;

			for(int k=0; k<nodeItem->childCount(); k++)
				count += updateNodeWithMouseState(nodeItem->child(k));
			
			return count;
		}
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

	// Slots :
	void UniformsLoaderInterface::applyModificationFromMouseState(void)
	{
		int count = 0;

		blockSignals(true);

		// Scan variables and count the number of modification :
		for(int k=0; k<childCount(); k++)
			count += updateNodeWithMouseState(child(k));

		blockSignals(false);

		if(count>0)
			emit modified();
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
#ifdef __USE_QVGL__
	UniformsLinkMenu::UniformsLinkMenu(int _type, QWidget* parent)
	 :	QMenu("Uniforms Links", parent),
		type(_type),
		unlinkAction(NULL)
	{
		// Generate the menu :
		#define NEW_MENU(id) \
		{ \
			QMenu* menu = addMenu(removeHeader(STR(id))); \
			QAction* action=NULL; \
			action = menu->addAction(removeHeader(STR(id##Gl)), &vectorSignalMapper, SLOT(map()));\
			vectorSignalMapper.setMapping(action, static_cast<int>(id##Gl)); \
			action = menu->addAction(removeHeader(STR(id##Quad)), &vectorSignalMapper, SLOT(map()));\
			vectorSignalMapper.setMapping(action, static_cast<int>(id##Quad)); \
			action = menu->addAction(removeHeader(STR(id##Image)), &vectorSignalMapper, SLOT(map()));\
			vectorSignalMapper.setMapping(action, static_cast<int>(id##Image)); \
		}

			NEW_MENU( QVGL::MouseState::VectorLastLeftClick )
			NEW_MENU( QVGL::MouseState::VectorLastLeftPosition )
			NEW_MENU( QVGL::MouseState::VectorLastLeftShift )
			NEW_MENU( QVGL::MouseState::VectorLastLeftRelease )
			NEW_MENU( QVGL::MouseState::VectorLastLeftCompletedVector )
			NEW_MENU( QVGL::MouseState::VectorLastRightClick )
			NEW_MENU( QVGL::MouseState::VectorLastRightPosition )
			NEW_MENU( QVGL::MouseState::VectorLastRightShift )
			NEW_MENU( QVGL::MouseState::VectorLastRightRelease )
			NEW_MENU( QVGL::MouseState::VectorLastRightCompletedVector )
			NEW_MENU( QVGL::MouseState::VectorLastWheelUp )
			NEW_MENU( QVGL::MouseState::VectorLastWheelDown )

		#undef NEW_MENU

		#define NEW_MENU(id) \
		{ \
			QAction* action = addAction(removeHeader(STR(id)), &colorSignalMapper, SLOT(map())); \
			colorSignalMapper.setMapping(action, static_cast<int>(id)); \
		}

			NEW_MENU( QVGL::MouseState::ColorUnderLastLeftClick )
			NEW_MENU( QVGL::MouseState::ColorUnderLastLeftPosition )
			NEW_MENU( QVGL::MouseState::ColorUnderLastLeftRelease )
			NEW_MENU( QVGL::MouseState::ColorUnderLastRightClick )
			NEW_MENU( QVGL::MouseState::ColorUnderLastRightPosition )
			NEW_MENU( QVGL::MouseState::ColorUnderLastRightRelease )

		#undef NEW_MENU

		unlinkAction = addAction("Unlink", this, SLOT(unlink(void)));

		QObject::connect(&vectorSignalMapper,	SIGNAL(mapped(int)), 		this, SLOT(setUniformLinkToVector(int)));
		QObject::connect(&colorSignalMapper,	SIGNAL(mapped(int)), 		this, SLOT(setUniformLinkToColor(int)));

		QList<QTreeWidgetItem*> emptyList;
		updateToSelection(emptyList);
	}
	
	UniformsLinkMenu::~UniformsLinkMenu(void)
	{ }

	QString UniformsLinkMenu::removeHeader(const QString& str)
	{
		QString res = str;
		res.remove("QVGL::MouseState::");
		return res;
	}

	void UniformsLinkMenu::setUniformLinkToVector(int id)
	{
		for(QList<ValuesInterface*>::iterator it=currentSelection.begin(); it!=currentSelection.end(); it++)
			(*it)->setVectorLink(QVGL::MouseState::validate(static_cast<QVGL::MouseState::VectorID>(id)));
	}

	void UniformsLinkMenu::setUniformLinkToColor(int id)
	{
		for(QList<ValuesInterface*>::iterator it=currentSelection.begin(); it!=currentSelection.end(); it++)
			(*it)->setColorLink(QVGL::MouseState::validate(static_cast<QVGL::MouseState::ColorID>(id)));
	}

	void UniformsLinkMenu::unlink(void)
	{

	}

	void UniformsLinkMenu::updateToSelection(QList<QTreeWidgetItem*>& selection)
	{
		// Check that all the selection are QTreeWidgetItem corresponding to uniforms and that they are of the same size 
		// (either 2x1 or 3x1)

		currentSelection.clear();
	
		int rowCount = -1; // not set yet.
		for(QList<QTreeWidgetItem*>::iterator it=selection.begin(); it!=selection.end(); it++)
		{
			if((*it)->type()==type)
			{
				ValuesInterface* valuesInterface = ValuesInterface::getPtrFromGenericItem(*it, type);

				const int 	rows = valuesInterface->getResource().object().getNumRows(),
						cols = valuesInterface->getResource().object().getNumColumns();

				if((rows==2 || rows==3) && cols==1 && (rowCount==-1 || rowCount==rows))
				{
					rowCount = rows;
					currentSelection.push_back(valuesInterface);
				}
				else
				{
					currentSelection.clear();
					break;
				}
			}
		}

		// Set the right menu :
		for(QMap<QVGL::MouseState::VectorID, QAction*>::iterator it=vectorPositionsActions.begin(); it!=vectorPositionsActions.end(); it++)
			it.value()->setEnabled(rowCount==2);

		for(QMap<QVGL::MouseState::ColorID, QAction*>::iterator it=colorsActions.begin(); it!=colorsActions.end(); it++)
			it.value()->setEnabled(rowCount==3);
	
		// Final :
		setEnabled(!currentSelection.isEmpty());
	}
#endif

