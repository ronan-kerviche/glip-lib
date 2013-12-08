#include "uniformsTab.hpp"
#include "codeEditor.hpp"
#include <limits>

// UniformObject	
	UniformObject::UniformObject(const std::string& _varName)
	 : QWidget(NULL), varName(_varName), hasBeenUpdated(false), currentExternalDataLink(NoExternalLink)
	{
		//std::cout << "UniformObject::UniformObject : " << this << std::endl;
		setAutoFillBackground(true);
	
		for(int k=0; k<NumExternalValueLink-1; k++)	// no accounting for no-link.
			allowedExternalValueLink[k] = false;
	}

	UniformObject::~UniformObject(void)
	{
		//std::cout << "UniformObject::~UniformObject : " << this << std::endl;
	}

	void UniformObject::declareUpdate(void)
	{
		hasBeenUpdated = true;
		emit updated();
	}

	const std::string& UniformObject::getVarName(void) const
	{
		return varName;
	}


	bool UniformObject::wasUpdated(void) const
	{
		return hasBeenUpdated;
	}

	void UniformObject::update(HdlProgram& prgm)
	{
		applyUpdate(prgm);
		hasBeenUpdated = false;
	}

	void UniformObject::read(HdlProgram& prgm)
	{
		this->blockSignals(true);

		readFrom(prgm);

		this->blockSignals(false);
	}

	bool UniformObject::isExternalValueLinkAllowed(ExternalValueLink evl) const
	{
		return allowedExternalValueLink[evl];
	}

	const ExternalValueLink& UniformObject::getCurrentExternalValueLink(void) const
	{
		return currentExternalDataLink;
	}

	bool UniformObject::loadExternalData(const GLSceneWidget::MouseData& mouseData)
	{
		return false;
	}

// UniformUnknown
	UniformUnknown::UniformUnknown(const std::string& name, const QString& message)
	 : UniformObject(name), layout(this), label(message, this)
	{
		//std::cout << "UniformUnknown::UniformUnknown : " << this << std::endl;
		item = new QTreeWidgetItem(NodeVarUnknown);
		item->setData(0, Qt::UserRole, QVariant::fromValue<UniformObject*>( reinterpret_cast<UniformObject*>(this) ));

		layout.addWidget(&label);

		item->setText(0, tr("%1 [x]").arg(getVarName().c_str()));
	}

	UniformUnknown::~UniformUnknown(void)
	{
		//std::cout << "UniformUnknown::~UniformUnknown : " << this << std::endl;
	}

	void UniformUnknown::applyUpdate(HdlProgram& prgm)
	{ }

	void UniformUnknown::readFrom(HdlProgram& prgm)
	{ }

	QTreeWidgetItem* UniformUnknown::treeItem(void) const
	{
		return item;
	}

	void UniformUnknown::applySettings(BoxesSettings&)
	{ }

	void UniformUnknown::setExternalValueLink(ExternalValueLink evl)
	{ }

// UniformInteger
	UniformInteger::UniformInteger(const std::string& name, GLenum _type, int n, int m, bool _un)
	 : UniformObject(name), un(_un), item(NULL), layout(this), type(_type)
	{
		//std::cout << "UniformInteger::UniformInteger : " << this << std::endl;

		for(int i=0; i<n; i++)
		{
			for(int j=0; j<m; j++)
			{
				data.push_back(new QSpinBox);

				if(un)
					data.back()->setRange(0, std::numeric_limits<int>::max());
				else
					data.back()->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

				if(n>1 && m>1 && i==j)
					data.back()->setValue(1);
				else
					data.back()->setValue(0);					

				layout.addWidget(data.back(), i, j);
				QObject::connect(data.back(), SIGNAL(valueChanged(int)), this, SLOT(declareUpdate(void)));
			}
		}
		
		// Build item : 
		item = new QTreeWidgetItem(NodeVarInteger);
		item->setData(0, Qt::UserRole, QVariant::fromValue<UniformObject*>( reinterpret_cast<UniformObject*>(this) ));

		if(un)
			item->setText(0, tr("%1 [%2x%3xunsigned]").arg(getVarName().c_str()).arg(n).arg(m) );
		else
			item->setText(0, tr("%1 [%2x%3xint]").arg(getVarName().c_str()).arg(n).arg(m) );

		// Set possible external links : 
		if(n==1 && m==3)
		{
			allowedExternalValueLink[ColorLastClick]	= true;
			allowedExternalValueLink[ColorCurrent]		= true;
			allowedExternalValueLink[ColorLastRelease]	= true;
		}
	}

	UniformInteger::~UniformInteger(void)
	{
		//std::cout << "UniformInteger::~UniformInteger : " << this << std::endl;

		item = NULL;	

		for(int k=0; k<data.size(); k++)
		{
			layout.removeWidget(data[k]);
			data[k]->deleteLater();
		}

		data.clear();
	}

	void UniformInteger::applyUpdate(HdlProgram& prgm)
	{
		if(un)
		{
			unsigned int buffer[16];
		
			for(int k=0; k<data.size(); k++)
				buffer[k] = data[k]->value();

			prgm.modifyVar( getVarName(), type, buffer );
		}
		else
		{
			int buffer[16];
		
			for(int k=0; k<data.size(); k++)
				buffer[k] = data[k]->value();

			prgm.modifyVar( getVarName(), type, buffer );
		}
	}

	void UniformInteger::readFrom(HdlProgram& prgm)
	{
		if(un)
		{
			unsigned int buffer[16];

			prgm.getVar( getVarName(), buffer);

			for(int k=0; k<data.size(); k++)
				data[k]->setValue(buffer[k]);
		}
		else
		{
			int buffer[16];

			prgm.getVar( getVarName(), buffer);

			for(int k=0; k<data.size(); k++)
				data[k]->setValue(buffer[k]);
		}
	}

	void UniformInteger::applySettings(BoxesSettings& settings)
	{
		for(int k=0; k<data.size(); k++)
		{
			if(!un)
				settings.updateInt(*data[k]);
			else
				settings.updateUn(*data[k]);
		}	
	}

	QTreeWidgetItem* UniformInteger::treeItem(void) const
	{
		return item;
	}

	void UniformInteger::setExternalValueLink(ExternalValueLink evl)
	{
		currentExternalDataLink = evl;

		const bool test = (evl==NoExternalLink);
		
		for(int k=0; k<data.count(); k++)
			data[k]->setEnabled(test);

		this->blockSignals(!test); // make sure that the modified flag is written but that the element does not send any further reports.
	}

	bool UniformInteger::loadExternalData(const GLSceneWidget::MouseData& mouseData)
	{
		switch(currentExternalDataLink)
		{
			case ColorLastClick :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue(mouseData.colorLastClick[k]);
				return true;
			case ColorCurrent :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue(mouseData.colorCurrent[k]);
				return true;
			case ColorLastRelease :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue(mouseData.colorCurrent[k]);
				return true;
			case NoExternalLink :
				return false;
			case LastClick :
			case CurrentPosition : 
			case LastRelease :
			case VectorCurrent :
			case LastVector :
				throw Exception("UniformInteger::loadExternalData - Received external data which should have been forbidden (internal error).", __FILE__, __LINE__);
			default :
				throw Exception("UniformInteger::loadExternalData - Unknown external data link (internal error).", __FILE__, __LINE__);
		}
	}

// UniformFloat
	UniformFloat::UniformFloat(const std::string& name, GLenum _type, int n, int m)
	 : UniformObject(name), item(NULL), layout(this), type(_type)
	{
		//std::cout << "UniformFloat::UniformFloat : " << this << std::endl;

		for(int i=0; i<n; i++)
		{
			for(int j=0; j<m; j++)
			{
				data.push_back(new QDoubleSpinBox);

				data.back()->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
				data.back()->setDecimals(3);

				if(n>1 && m>1 && i==j)
					data.back()->setValue(1.0);
				else
					data.back()->setValue(0.0);

				layout.addWidget(data.back(), i, j);
				QObject::connect(data.back(), SIGNAL(valueChanged(double)), this, SLOT(declareUpdate(void)));
			}
		}
		
		// Build item : 
		item = new QTreeWidgetItem(NodeVarFloatingPoint);
		item->setData(0, Qt::UserRole, QVariant::fromValue<UniformObject*>( reinterpret_cast<UniformObject*>(this) ));

		item->setText(0, tr("%1 [%2x%3xfloat]").arg(getVarName().c_str()).arg(n).arg(m) );

		// Set possible external links : 
		if(n==1 && m==2)
		{
			allowedExternalValueLink[LastClick]		= true;
			allowedExternalValueLink[CurrentPosition]	= true;
			allowedExternalValueLink[LastRelease]		= true;
			allowedExternalValueLink[VectorCurrent]		= true;
			allowedExternalValueLink[LastVector]		= true;
		}
		else if(n==1 && m==3)
		{
			allowedExternalValueLink[ColorLastClick]	= true;
			allowedExternalValueLink[ColorCurrent]		= true;
			allowedExternalValueLink[ColorLastRelease]	= true;
		}
	}

	UniformFloat::~UniformFloat(void)
	{
		//std::cout << "UniformFloat::~UniformFloat : " << this << std::endl;

		item = NULL;	

		for(int k=0; k<data.size(); k++)
		{
			layout.removeWidget(data[k]);
			data[k]->deleteLater();
		}

		data.clear();
	}

	void UniformFloat::applyUpdate(HdlProgram& prgm)
	{
		float buffer[16];
		
		for(int k=0; k<data.size(); k++)
			buffer[k] = data[k]->value();

		prgm.modifyVar( getVarName(), type, buffer );
	}

	void UniformFloat::readFrom(HdlProgram& prgm)
	{
		float buffer[16];

		prgm.getVar( getVarName(), buffer);

		for(int k=0; k<data.size(); k++)
			data[k]->setValue(buffer[k]);
	}

	void UniformFloat::applySettings(BoxesSettings& settings)
	{
		for(int k=0; k<data.size(); k++)
			settings.updateDouble(*data[k]);	
	}

	QTreeWidgetItem* UniformFloat::treeItem(void) const
	{
		return item;
	}

	void UniformFloat::setExternalValueLink(ExternalValueLink evl)
	{
		currentExternalDataLink = evl;

		const bool test = (evl==NoExternalLink);
		
		for(int k=0; k<data.count(); k++)
			data[k]->setEnabled(test);

		this->blockSignals(!test); // make sure that the modified flag is written but that the element does not send any further reports.
	}

	bool UniformFloat::loadExternalData(const GLSceneWidget::MouseData& mouseData)
	{
		switch(currentExternalDataLink)
		{
			case LastClick :
				data[0]->setValue( mouseData.xLastClick );
				data[1]->setValue( mouseData.yLastClick );
				return true;
			case CurrentPosition :
				data[0]->setValue( mouseData.xCurrent );
				data[1]->setValue( mouseData.yCurrent );
				return true;
			case LastRelease :
				data[0]->setValue( mouseData.xLastRelease );
				data[1]->setValue( mouseData.yLastRelease );
				return true;
			case VectorCurrent :
				data[0]->setValue( mouseData.xVectorCurrent );
				data[1]->setValue( mouseData.yVectorCurrent );
				return true;
			case LastVector :
				data[0]->setValue( mouseData.xLastVector );
				data[1]->setValue( mouseData.yLastVector );
				return true;
			case ColorLastClick :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue( static_cast<double>(mouseData.colorLastClick[k])/255.0 );
				return true;
			case ColorCurrent :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue( static_cast<double>(mouseData.colorCurrent[k])/255.0 );
				return true;
			case ColorLastRelease :
				for(int k=0; k<data.count(); k++)
					data[k]->setValue( static_cast<double>(mouseData.colorCurrent[k])/255.0 );
				return true;
			case NoExternalLink :
				return false;
			default :
				throw Exception("UniformInteger::loadExternalData - Unknown external data link (internal error).", __FILE__, __LINE__);
		}
	}

// FilterGroup
	FilterElement::FilterElement(Filter& filter, const std::vector<std::string>& _path, QTreeWidget* tree)
	 : name(filter.getName()), path(_path), item(NULL)
	{
		//std::cout << "FilterElement::FilterElement : " << this << std::endl;

		item = new QTreeWidgetItem(NodeFilter);
		item->setData(0, Qt::UserRole, QVariant::fromValue<UniformObject*>( reinterpret_cast<UniformObject*>(NULL) ));

		item->setText(0, tr("%1 [%2]").arg(name.c_str()).arg(filter.getTypeName().c_str()) );

		// List the variables : 
		for(int k=0; k<filter.prgm().getUniformVarsNames().size(); k++)
		{
			bool processNewItem = true;

			// Omit the variables : 
			/*if(filter.prgm().getUniformVarsNames()[k]=="gl_ModelViewMatrix" || filter.prgm().getUniformVarsNames()[k]=="gl_TextureMatrix[0]")
				objects.push_back( reinterpret_cast<UniformObject*>(new UniformUnknown(filter.prgm().getUniformVarsNames()[k], "(Forbidden)")) );
			else*/

			if( filter.prgm().isValid( filter.prgm().getUniformVarsNames()[k] ) )
			{
				switch(filter.prgm().getUniformVarsTypes()[k])
				{
					case GL_INT : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_INT, 1, 1, false)) );				
						break;
					case GL_INT_VEC2 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_INT_VEC2, 1, 2, false)) );				
						break;
					case GL_INT_VEC3 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_INT_VEC3, 1, 3, false)) );				
						break;
					case GL_INT_VEC4 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_INT_VEC4, 1, 4, false)) );				
						break;
					case GL_UNSIGNED_INT : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_UNSIGNED_INT, 1, 1, true)) );				
						break;
					case GL_UNSIGNED_INT_VEC2 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_UNSIGNED_INT_VEC2, 1, 2, true)) );				
						break;
					case GL_UNSIGNED_INT_VEC3 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_UNSIGNED_INT_VEC3, 1, 3, true)) );				
						break;
					case GL_UNSIGNED_INT_VEC4 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformInteger(filter.prgm().getUniformVarsNames()[k], GL_UNSIGNED_INT_VEC4, 1, 4, true)) );				
						break;
					case GL_FLOAT : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT, 1, 1)) );				
						break;
					case GL_FLOAT_VEC2 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_VEC2, 1, 2)) );				
						break;
					case GL_FLOAT_VEC3 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_VEC3, 1, 3)) );				
						break;
					case GL_FLOAT_VEC4 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_VEC4, 1, 4)) );				
						break;
					case GL_FLOAT_MAT2 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_MAT2, 2, 2)) );				
						break;
					case GL_FLOAT_MAT3 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_MAT3, 3, 3)) );				
						break;
					case GL_FLOAT_MAT4 : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformFloat(filter.prgm().getUniformVarsNames()[k], GL_FLOAT_MAT4, 4, 4)) );	
						break;
					default : 
						objects.push_back( reinterpret_cast<UniformObject*>(new UniformUnknown(filter.prgm().getUniformVarsNames()[k], tr("Unable to generate interface for type %1").arg(glParamName(filter.prgm().getUniformVarsTypes()[k]).c_str()))) );
				}

				objects.back()->read(filter.prgm());

				item->addChild( objects.back()->treeItem() );
				
				tree->setItemWidget( objects.back()->treeItem(), 1, objects.back() );

				QObject::connect( objects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
				QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), objects.back(), SLOT(applySettings(BoxesSettings&)));
			}
		}
	}

	FilterElement::~FilterElement(void)
	{
		//std::cout << "FilterElement::~FilterElement : " << this << std::endl;
	}

	QTreeWidgetItem* FilterElement::treeItem(void) const
	{
		return item;
	}

	void FilterElement::update(Pipeline& pipeline)
	{
		//throw Exception("Update is disabled.", __FILE__, __LINE__);

		/*Filter& filter = pipeline[path + SEPARATOR + name];

		for(int k=0; k<objects.size(); k++)
		{
			if(objects[k]->wasUpdated())
				objects[k]->update(filter.prgm());
		}*/

		__ReadOnly_PipelineLayout& parent = pipeline.pipelineLayout(path);
		int id = parent.getElementID(name);
		Filter& filter = pipeline[id];
		
		for(int k=0; k<objects.size(); k++)
		{
			if(objects[k]->wasUpdated())
				objects[k]->update(filter.prgm());
		}
	}

	int FilterElement::varsCount(void) const
	{
		return objects.size();
	}

	bool FilterElement::spreadExternalData(const GLSceneWidget::MouseData& mouseData)
	{
		bool test = false;

		for(int k=0; k<objects.size(); k++)
		{
			const bool t = objects[k]->loadExternalData(mouseData);
			test = t || test;
		} 
		
		return test;
	}

// PipelineElement
	PipelineElement::PipelineElement(const __ReadOnly_PipelineLayout& pipeline, const std::string& name, const std::string& pathStr, const std::vector<std::string>& path, Pipeline& mainPipeline, QTreeWidget* tree, bool isRoot)
	 : item(NULL)
	{
		//std::cout << "PipelineElement::PipelineElement : " << this << std::endl;

		item = new QTreeWidgetItem(NodeFilter);
		item->setData(0, Qt::UserRole, QVariant::fromValue<UniformObject*>( reinterpret_cast<UniformObject*>(NULL) ));

		if(!path.empty())
			item->setText(0, tr("%1 [%2]").arg(name.c_str()).arg(pathStr.c_str()));
		else
			item->setText(0, tr("%1 [%2]").arg(name.c_str()).arg(pipeline.getTypeName().c_str()) );

		std::string currentPathStr;

		if(!pathStr.empty())
			currentPathStr = pathStr + "::" + name;
		else
			currentPathStr = name;

		std::vector<std::string> currentPath = path;

		if(!isRoot)
			currentPath.push_back(name);
		else
			tree->addTopLevelItem(item);

		// Store the sub-elements : 
		for(int k=0; k< pipeline.getNumElements(); k++)
		{
			__ReadOnly_PipelineLayout::ComponentKind kind = pipeline.getElementKind(k);
			
			if( kind==__ReadOnly_PipelineLayout::FILTER )
			{
				//Filter& filter = mainPipeline[pipeline.componentLayout(k).getName()];
				int id = pipeline.getElementID(k);
				Filter& filter = mainPipeline[id];

				filterObjects.push_back( new FilterElement(filter, currentPath, tree) );

				item->addChild( filterObjects.back()->treeItem() );

				QObject::connect( filterObjects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
				QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), filterObjects.back(), SIGNAL(propagateSettings(BoxesSettings&)));
			}
			else if( kind==__ReadOnly_PipelineLayout::PIPELINE )
			{
				pipelineObjects.push_back( new PipelineElement( pipeline.pipelineLayout(k), pipeline.getElementName(k), currentPathStr, currentPath, mainPipeline, tree) );

				item->addChild( pipelineObjects.back()->treeItem() );

				QObject::connect( pipelineObjects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
				QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), pipelineObjects.back(), SIGNAL(propagateSettings(BoxesSettings&)));
			}
			else
				throw Exception("PipelineElement::PipelineElement - Internal error : unable to read component type for \"" + pipeline.getElementName(k) + "\" in path : \"" + currentPathStr + "\".", __FILE__, __LINE__);
		}
	}

	PipelineElement::~PipelineElement(void)
	{
		//std::cout << "PipelineElement::~PipelineElement : " << this << std::endl;

		item = NULL;

		for(int k=0; k<filterObjects.size(); k++)
			delete filterObjects[k];
		filterObjects.clear();

		for(int k=0; k<pipelineObjects.size(); k++)
			delete pipelineObjects[k];
		pipelineObjects.clear();
	}

	QTreeWidgetItem* PipelineElement::treeItem(void) const
	{
		return item;
	}

	void PipelineElement::update(Pipeline& pipeline)
	{
		for(int k=0; k<filterObjects.size(); k++)
			filterObjects[k]->update(pipeline);

		for(int k=0; k<pipelineObjects.size(); k++)
			pipelineObjects[k]->update(pipeline);
	}

	int PipelineElement::varsCount(void) const
	{
		int c = 0;

		for(int k=0; k<filterObjects.size(); k++)
			c += filterObjects[k]->varsCount();

		for(int k=0; k<pipelineObjects.size(); k++)
			c += pipelineObjects[k]->varsCount();

		return c;
	}

	bool PipelineElement::spreadExternalData(const GLSceneWidget::MouseData& mouseData)
	{
		bool test = false;

		for(int k=0; k<filterObjects.size(); k++)
		{
			const bool t = filterObjects[k]->spreadExternalData(mouseData);
			test = t || test;
		} 
		
		for(int k=0; k<pipelineObjects.size(); k++)
		{
			const bool t = pipelineObjects[k]->spreadExternalData(mouseData);
			test = t || test;
		}

		return test;
	}

// Settings
	BoxesSettings::BoxesSettings(QWidget* parent)
	 : QWidget(parent), layout(this), 
	   intLabel("Signed integers : ", this), unLabel("Unsigned integers : ", this), doubleLabel("Floating points : ", this),
	   minIntBox(this), maxIntBox(this), stepIntBox(this),
	   minIntLabel("Min : ", this), maxIntLabel("Max : ", this), stepIntLabel("Step : ", this),
	   maxUnBox(this), stepUnBox(this),
	   maxUnLabel("Max : ", this), stepUnLabel("Step : ", this),
	   minDoubleBox(this), maxDoubleBox(this), stepDoubleBox(this),
	   minDoubleLabel("Min : ", this), maxDoubleLabel("Max : ", this), stepDoubleLabel("Step : ", this)
	{
		// Int : 
			minIntLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			maxIntLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			stepIntLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		
			minIntBox.setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
			maxIntBox.setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
			stepIntBox.setRange(0, std::numeric_limits<int>::max());
		
			minIntBox.setValue(-8192);
			maxIntBox.setValue(8192);
			stepIntBox.setValue(1);
		
			layout.addWidget(&intLabel,0,0);
			layout.addWidget(&minIntLabel,0,1);
			layout.addWidget(&minIntBox,0,2);
			layout.addWidget(&maxIntLabel,0,3);
			layout.addWidget(&maxIntBox,0,4);
			layout.addWidget(&stepIntLabel,0,5);
			layout.addWidget(&stepIntBox,0,6);

			QObject::connect(&minIntBox,	SIGNAL(valueChanged(int)),	this,	SIGNAL(settingsChanged(void)));
			QObject::connect(&maxIntBox,	SIGNAL(valueChanged(int)),	this,	SIGNAL(settingsChanged(void)));
			QObject::connect(&stepIntBox,	SIGNAL(valueChanged(int)),	this,	SIGNAL(settingsChanged(void)));

		// Unsigned : 
			maxUnLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			stepUnLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		
			maxUnBox.setRange(0, std::numeric_limits<int>::max());
			stepUnBox.setRange(0, std::numeric_limits<int>::max());
		
			maxUnBox.setValue(8192);
			stepUnBox.setValue(1);
		
			layout.addWidget(&unLabel,1,0);
			layout.addWidget(&maxUnLabel,1,3);
			layout.addWidget(&maxUnBox,1,4);
			layout.addWidget(&stepUnLabel,1,5);
			layout.addWidget(&stepUnBox,1,6);

			QObject::connect(&maxUnBox,	SIGNAL(valueChanged(int)),	this,	SIGNAL(settingsChanged(void)));
			QObject::connect(&stepUnBox,	SIGNAL(valueChanged(int)),	this,	SIGNAL(settingsChanged(void)));

		// Double : 
			minDoubleLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			maxDoubleLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			stepDoubleLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		
			/*minDoubleBox.setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
			maxDoubleBox.setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
			stepDoubleBox.setRange(0.0, std::numeric_limits<double>::max());
			stepDoubleBox.setSingleStep(1e-6);*/
			minDoubleBox.setRange(-1e6, 1e6);
			maxDoubleBox.setRange(-1e6, 1e6);
			stepDoubleBox.setRange(0.0, 1e6);
			minDoubleBox.setSingleStep(1e-6);
			maxDoubleBox.setSingleStep(1e-6);
			stepDoubleBox.setSingleStep(1e-6);

			minDoubleBox.setDecimals(6);
			maxDoubleBox.setDecimals(6);
			stepDoubleBox.setDecimals(6);
		
			minDoubleBox.setValue(-8192.0);
			maxDoubleBox.setValue(8192.0);
			stepDoubleBox.setValue(0.01);
		
			layout.addWidget(&doubleLabel,2,0);
			layout.addWidget(&minDoubleLabel,2,1);
			layout.addWidget(&minDoubleBox,2,2);
			layout.addWidget(&maxDoubleLabel,2,3);
			layout.addWidget(&maxDoubleBox,2,4);
			layout.addWidget(&stepDoubleLabel,2,5);
			layout.addWidget(&stepDoubleBox,2,6);

			QObject::connect(&minDoubleBox,	SIGNAL(valueChanged(double)),	this,	SIGNAL(settingsChanged(void)));
			QObject::connect(&maxDoubleBox,	SIGNAL(valueChanged(double)),	this,	SIGNAL(settingsChanged(void)));
			QObject::connect(&stepDoubleBox,SIGNAL(valueChanged(double)),	this,	SIGNAL(settingsChanged(void)));
	}
			
	void BoxesSettings::updateInt(QSpinBox& box)
	{
		if(minIntBox.value()>maxIntBox.value())
			minIntBox.setValue(maxIntBox.value());

		box.setRange( minIntBox.value(), maxIntBox.value() );
		box.setSingleStep( stepIntBox.value() );
	}

	void BoxesSettings::updateUn(QSpinBox& box)
	{
		box.setRange( 0, maxUnBox.value() );
		box.setSingleStep( stepUnBox.value() );
	}

	void BoxesSettings::updateDouble(QDoubleSpinBox& box)
	{
		if(minDoubleBox.value()>maxDoubleBox.value())
			minDoubleBox.setValue(maxDoubleBox.value());

		box.setRange( minDoubleBox.value(), maxDoubleBox.value() );
		box.setSingleStep( stepDoubleBox.value() );
	}

// MainUniformLibrary
	MainUniformLibrary::MainUniformLibrary(QWidget* parent)
	 : QMenu("Main Library", parent), mainLibraryFilename("./mainLibrary.uvd"), infoAct("> No information available", this), loadAct("Load variables", this), storeAct("Store variables", this), removeAct("Remove variables", this), syncToDiskAct(tr("Save to disk : %1").arg(mainLibraryFilename.c_str()), this), actionToProcess(NoAct), availablePipelines("Available data", this), showInfo(true)
	{
		// Add : 
		addAction(&infoAct);
		addAction(&loadAct);
		addAction(&storeAct);
		addAction(&removeAct);
		addAction(&syncToDiskAct);
		addMenu(&availablePipelines);

		infoAct.setDisabled(true);
		loadAct.setDisabled(true);
		storeAct.setDisabled(true);
		removeAct.setDisabled(true);
		syncToDiskAct.setDisabled(true);

		// Connect :
		connect(&infoAct,	SIGNAL(triggered()), this, SLOT(showCode()));
		connect(&loadAct, 	SIGNAL(triggered()), this, SLOT(loadFromLibrary()));
		connect(&storeAct, 	SIGNAL(triggered()), this, SLOT(saveFromLibrary()));
		connect(&removeAct, 	SIGNAL(triggered()), this, SLOT(removeFromLibrary()));
		connect(&syncToDiskAct,	SIGNAL(triggered()), this, SLOT(syncLibraryToDisk()));

		// Check if the file exists : 
		if( QFile::exists( mainLibraryFilename.c_str() ) )
		{
			// Load : 
			try
			{
				mainLibrary.load(mainLibraryFilename, true);
			}
			catch(Exception& e)
			{
				std::cerr << "MainUniformLibrary::MainUniformLibrary - Exception : " << std::endl;
				std::cerr << e.what();

				// Clear current library : 
				mainLibrary.clear();

				// Find a suitable filename :
				int k=0;
				std::string newFileName;

				for(k=0; k<9; k++)
				{
					newFileName = "./mainLibrary_backup_" + to_string(k) + ".uvd";
					QFile newBackupFile( newFileName.c_str() );
					if( !newBackupFile.exists() )
						break;
				}

				// Move : 
				newFileName = "./mainLibrary_backup_" + to_string(k) + ".uvd";
				QFile mainLibraryFile( mainLibraryFilename.c_str() );
				mainLibraryFile.rename( newFileName.c_str() );

				// Warning :
				QMessageBox messageBox(QMessageBox::Warning, "Warning", tr("The Main Uniforms Variables Library stored in file \"%1\" cannot be read. The current content was backed up in the file \"%2\" and a new library is started with this session.").arg(mainLibraryFilename.c_str()).arg(newFileName.c_str()), QMessageBox::Ok, this);

				messageBox.setDetailedText(e.what());
				messageBox.exec();
			}
		}

		updateMenu();
	}

	MainUniformLibrary::~MainUniformLibrary(void)
	{
		mainLibrary.writeToFile(mainLibraryFilename);
		currentCode.clear();
	}

	void MainUniformLibrary::showDialogCode(const QString& title, const QString& code)
	{
		QDialog dialogBox(this);
	 
		// Dialog : 
		QGridLayout gridLayout(&dialogBox);

		// Label : 
		QLabel label(title, &dialogBox);

		gridLayout.addWidget(&label, 0, 0, 1, 1);

		// Code editor : 
		CodeEditor codeEditor(&dialogBox);
		
		codeEditor.setReadOnly(true);
		codeEditor.setFixedSize(512, 256);

		gridLayout.addWidget(&codeEditor, 1, 0, 1, 1);
	
		QDialogButtonBox buttonBox;

		buttonBox.setOrientation(Qt::Horizontal);
		buttonBox.setStandardButtons(QDialogButtonBox::Ok);
		gridLayout.addWidget(&buttonBox, 2, 0, 1, 1);

		connect(&buttonBox,	SIGNAL(accepted()),	&dialogBox,	SLOT(accept()));

		dialogBox.setSizeGripEnabled(false);
		dialogBox.setFixedSize( dialogBox.sizeHint() );	

		codeEditor.insert(code);

		dialogBox.exec();
	}

	void MainUniformLibrary::showCode(void)
	{
		if(currentCode.isEmpty())
		{
			QMessageBox messageBox(QMessageBox::NoIcon, tr("MainLibrary content"), infoAct.text(), QMessageBox::Ok, this);
			messageBox.exec();
		}
		else
			showDialogCode(infoAct.text(), currentCode);
	}

	void MainUniformLibrary::showStoredCode(void)
	{
		// Get name : 
		QAction* sender = reinterpret_cast<QAction*>(QObject::sender());
		std::string typeName = sender->statusTip().toStdString();

		QString str = tr("%1 (%2 variable/s)").arg(typeName.c_str()).arg(mainLibrary.getNumVariables(typeName));

		showDialogCode(str, mainLibrary.getCode(typeName).c_str());
	}

	void MainUniformLibrary::loadFromLibrary(void)
	{
		actionToProcess = Apply;
		emit requireProcessing();
	}

	void MainUniformLibrary::saveFromLibrary(void)
	{
		actionToProcess = Save;
		emit requireProcessing();
	}
	
	void MainUniformLibrary::removeFromLibrary(void)
	{
		actionToProcess = Remove;
		emit requireProcessing();
	}

	void MainUniformLibrary::syncLibraryToDisk(void)
	{
		syncToDiskAct.setDisabled(true);

		// Save : 
		mainLibrary.writeToFile(mainLibraryFilename);
	}

	void MainUniformLibrary::updateMenu(void)
	{
		// Clear : 
		availablePipelines.clear();

		// Get : 
		std::vector<std::string> els = mainLibrary.getPipelinesTypeNames();
		
		if(els.empty())
		{
			QAction* tmp = availablePipelines.addAction( "(None)" );
			tmp->setDisabled(true);
		}
		else
		{
			for(int k = 0; k<els.size(); k++)
			{
				QString str = tr("%1 (%2 variable/s)").arg(els[k].c_str()).arg(mainLibrary.getNumVariables(els[k]));
				QAction* tmp = availablePipelines.addAction( str, this, SLOT(showStoredCode()));
				tmp->setStatusTip( els[k].c_str() );
			}
		}
	}

	void MainUniformLibrary::update(void)
	{
		infoAct.setText("No information available");
		currentCode.clear();
		actionToProcess = NoAct;
		infoAct.setDisabled(true);
		loadAct.setDisabled(true);
		storeAct.setDisabled(true);
		removeAct.setDisabled(true);
	}

	void MainUniformLibrary::update(Pipeline& pipeline)
	{
		if(mainLibrary.hasPipeline(pipeline.getName()))
		{
			int c = mainLibrary.getNumVariables(pipeline.getName());
			infoAct.setText(tr("> %1 has %2 variables registered").arg(pipeline.getName().c_str()).arg(c));
			currentCode = mainLibrary.getCode(pipeline.getName()).c_str();

			infoAct.setDisabled(false);
			loadAct.setDisabled(false);
			removeAct.setDisabled(false);
		}
		else
		{
			infoAct.setText(tr("> %1 has no registered variables").arg(pipeline.getName().c_str()));
			currentCode.clear();
			
			infoAct.setDisabled(true);
			loadAct.setDisabled(true);
			removeAct.setDisabled(true);
		}

		int 	numFilters		= 0,
			numPipelines		= 0,
			numUniformVariables	= 0;

		pipeline.getInfoElements(numFilters, numPipelines, numUniformVariables );
		storeAct.setDisabled( numUniformVariables==0 );
		
		actionToProcess = NoAct;
	}

	bool MainUniformLibrary::process(Pipeline& pipeline, const LibraryAction& forceAction)
	{
		int c=0;

		LibraryAction act = NoAct;

		if(forceAction!=NoAct)
			act = forceAction;
		else
		{
			act = actionToProcess;
			actionToProcess = NoAct;
		}

		switch(act)
		{
			case Apply :
				try
				{
					c = mainLibrary.applyTo(pipeline);
				}
				catch(Exception& e)
				{
					QMessageBox warning(QMessageBox::Warning, "Warning", tr("Failed to load variables for %1.").arg(pipeline.getFullName().c_str()), QMessageBox::Ok, this);
					warning.setDetailedText( e.what() );

					std::cerr << "MainUniformLibrary::process - Exception : " << std::endl;
					std::cerr << e.what();

					warning.exec();
				}

				if(showInfo && c>0)
				{
					QMessageBox messageBox;
					messageBox.setText(tr("%1 variables loaded successfully.").arg(c));

					messageBox.addButton(QMessageBox::Ok);
					QPushButton *doNotShowAgain = messageBox.addButton("Do not show again", QMessageBox::NoRole);

					messageBox.exec();

					if(messageBox.clickedButton() == doNotShowAgain)
						showInfo = false;
				}
				return c>0;
			case Save :
				mainLibrary.load(pipeline, true);
				currentCode = mainLibrary.getCode(pipeline.getName()).c_str();
				syncToDiskAct.setDisabled(false);
				update(pipeline);
				updateMenu();
				return false;
			case Remove : 
				mainLibrary.clear(pipeline.getName());
				currentCode.clear();
				syncToDiskAct.setDisabled(false);
				update(pipeline);	
				updateMenu();
				return false;
			case NoAct :
			default :
				return false;
		}
	}

// LinkToExternalValue
	LinkToExternalValue::LinkToExternalValue(QWidget* parent)
	 : 	QMenu("External values links", parent),
		signalMapper(this),
		target(NULL)
	{
		links[LastClick]	= addAction("Coordinates on last left click");
		links[CurrentPosition]	= addAction("Current coordinates, when holding left click");
		links[LastRelease]	= addAction("Coordinates on last left click release");
		links[VectorCurrent]	= addAction("Current vector coordinates, between left click and current position");
		links[LastVector]	= addAction("Last vector coordinates, between left click and release");
		links[ColorLastClick]	= addAction("Color under last left click");
		links[ColorCurrent]	= addAction("Color under current position, when holding left click");
		links[ColorLastRelease]	= addAction("Color under last left click release");
		links[NoExternalLink]	= addAction("Clear link");

		for(int k=0; k<NumExternalValueLink; k++)
		{
			links[k]->setCheckable( k!=NoExternalLink );
			signalMapper.setMapping(links[k], k);
            		connect(links[k], SIGNAL(triggered()), &signalMapper, SLOT(map()));
		}

		connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(changeExternalValueLinkTo(int)));

		updateMenu();
	}

	LinkToExternalValue::~LinkToExternalValue(void)
	{ }

	void LinkToExternalValue::updateMenu(void)
	{
		target = NULL;

		for(int k=0; k<NumExternalValueLink; k++)
		{
			links[k]->setEnabled(false);
			links[k]->setChecked(false);
		}
	}

	void LinkToExternalValue::updateMenu(QTreeWidgetItem* item)
	{
		if(item==NULL)
		{
			updateMenu();
			return ;
		}

		// Get the uniform object pointer : 
		target = item->data(0, Qt::UserRole).value<UniformObject*>(); //QVariant::value<UniformObject*>( item->data(0, Qt::UserRole) );
		

		if(item->type()<=NodeLeaf || item->type()>=NodeVarUnknown || target ==NULL)
			updateMenu();
		else
		{
			// Lock down the links which are not allowed :
			for(int k=0; k<NumExternalValueLink-1; k++)
			{
				links[k]->setEnabled( target->isExternalValueLinkAllowed(static_cast<ExternalValueLink>(k)) );
				links[k]->setChecked(false);
			}
			links[NoExternalLink]->setEnabled( true );
			
			// Set the checkable : 
			if( target->getCurrentExternalValueLink()!=NoExternalLink )
				links[ target->getCurrentExternalValueLink() ]->setChecked(true);
		}
	}

	void LinkToExternalValue::changeExternalValueLinkTo(int link)
	{
		target->setExternalValueLink(static_cast<ExternalValueLink>(link));

		// Update menu : 
		for(int k=0; k<NumExternalValueLink-1; k++)
			links[k]->setChecked(false);
		
		if( target->getCurrentExternalValueLink()!=NoExternalLink )
			links[ target->getCurrentExternalValueLink() ]->setChecked(true);
	}

// UniformsTab
	UniformsTab::UniformsTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this), menuBar(this),
		showSettings("Settings", this),
		tree(this),
		settings(this),
		linkToExternalValue(this),
		mainPipeline(NULL),
		dontAskForSave(false),
		modified(false),
		mainLibraryMenu(this),
		fileMenu("File", this),
		openSaveInterface("UniformsPannel", "File", "*.ppl *.ext *.uvd")
	{
		tree.setContextMenuPolicy(Qt::CustomContextMenu);

		openSaveInterface.enableOpen(false);
		openSaveInterface.enableSave(false);
		openSaveInterface.enableSaveAs(false);

		openSaveInterface.addToMenu(fileMenu);
		menuBar.addMenu(&fileMenu);
		menuBar.addMenu(&linkToExternalValue);
		menuBar.addMenu(&mainLibraryMenu);
		menuBar.addAction(&showSettings);

		layout.addWidget(&menuBar);
		layout.addWidget(&tree);
		layout.addWidget(&settings);
		
		settings.hide();

		tree.setIndentation(8);
		//tree.setSelectionMode(QAbstractItemView::NoSelection);
		tree.setSelectionMode(QAbstractItemView::SingleSelection);

		QStringList listLabels;
		listLabels.push_back("Name");
		listLabels.push_back("Data");
		tree.setHeaderLabels( listLabels );

		QObject::connect(&showSettings,		SIGNAL(triggered()),						this, 			SLOT(switchSettings()));
		QObject::connect(&openSaveInterface,	SIGNAL(openFile(const QStringList&)),				this,			SLOT(loadData(const QStringList&)));
		QObject::connect(&openSaveInterface,	SIGNAL(saveFileAs(const QString&)),				this,			SLOT(saveData(const QString&)));
		QObject::connect(&openSaveInterface,	SIGNAL(saveFile()),						this,			SLOT(saveData(void)));
		QObject::connect(&settings,		SIGNAL(settingsChanged()),					this,			SLOT(settingsChanged()));
		QObject::connect(this, 			SIGNAL(requestDataUpdate()),					this,			SLOT(dataWasModified()));
		QObject::connect(&mainLibraryMenu,	SIGNAL(requireProcessing()),					this, 			SLOT(mainLibraryPipelineUpdate()));
		QObject::connect(&tree,			SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),	&linkToExternalValue,	SLOT(updateMenu(QTreeWidgetItem*)));
		QObject::connect(&tree,			SIGNAL(customContextMenuRequested(const QPoint&)), 		this, 			SLOT(showContextMenu(const QPoint&)));
	}

	UniformsTab::~UniformsTab(void)
	{
		clear();
	}

	void UniformsTab::clear(void)
	{
		if(mainPipeline==NULL)
			return ;

		// Disconnect : 
		tree.blockSignals(true);
		linkToExternalValue.updateMenu();

		/*QTreeWidgetItem* root = tree.takeTopLevelItem(0);

		while(root->childCount()>0)
		{
			QTreeWidgetItem* ptr = root->takeChild(0);
			delete ptr;
		}

		delete root;*/

		QTreeWidgetItem* root = tree.takeTopLevelItem(0);
		delete mainPipeline;

		// reconnect : 
		tree.blockSignals(false);

		//mainPipeline->deleteLater();
		mainPipeline = NULL;
	}

	void UniformsTab::switchSettings(void)
	{
		if(settings.isVisible())
		{
			settings.hide();
			showSettings.setText("Settings");			
		}
		else
		{
			settings.show();
			showSettings.setText("Hide Settings");
		}
	}

	void UniformsTab::settingsChanged(void)
	{
		emit propagateSettings(settings);
	}

	void UniformsTab::dataWasModified(void)
	{
		modified = true;

		// Carry out modification : 
		if(pipelineExists() && mainPipeline!=NULL)
		{
			if(pipelineUniformsCanBeModified())
			{
				mainPipeline->update(pipeline());
				requirePipelineComputation();
			}
		}
	}

	bool UniformsTab::prepareUpdate(void)
	{
		if(!dontAskForSave && pipelineExists() && mainPipeline!=NULL && modified)
		{
			if(mainPipeline->varsCount()==0)
				return true;

			QMessageBox 	message(this);
			message.setText("The Uniforms Variables were modified manually, do you want to save them?");

			QPushButton 	*saveButton		= message.addButton(tr("Save"), QMessageBox::AcceptRole),
					*storeToMainLibButton	= message.addButton(tr("Store to MainLibrary"), QMessageBox::NoRole),
					*discardButton		= message.addButton(tr("Discard"), QMessageBox::DestructiveRole),
					*discardHideButton	= message.addButton(tr("Discard and hide future requests"), QMessageBox::NoRole),
					*cancelButton		= message.addButton(tr("Cancel"), QMessageBox::RejectRole);

			message.setDefaultButton(discardButton);

			message.exec();

			if(message.clickedButton() == saveButton)
			{
				//saveData();
				QString filename = openSaveInterface.saveAsDialog();
				saveData(filename);
				return true;
			}
			else if(message.clickedButton() == storeToMainLibButton)
			{
				mainLibraryMenu.process(pipeline(), MainUniformLibrary::Save);
				return true;
			}
			else if(message.clickedButton() == discardHideButton)
			{			
				dontAskForSave = true;
				return true;
			}
			else if(message.clickedButton() == discardButton)
				return true;		
			else //if(message.clickedButton() == cancelButton)
				return false;
		}
		else
			return true;
	}

	bool UniformsTab::pipelineCanBeDestroyed(void)
	{
		return prepareUpdate();
	}

	bool UniformsTab::canBeClosed(void)
	{
		return prepareUpdate();
	}

	bool UniformsTab::loadData(const QStringList& filenames)
	{
		if(filenames.empty())
			return false;

		UniformsVarsLoader uLoader;

		try
		{
			for(int k=0; k<filenames.count(); k++)
			{
				if(!filenames[k].isEmpty())
				{
					uLoader.load( filenames[k].toStdString() );

					openSaveInterface.reportSuccessfulLoad(filenames[k]);
				}
			}
		}
		catch(Exception& e)
		{
			std::cerr << "Exception cauhgt : " << std::endl;
			std::cerr << e.what() << std::endl;
			QMessageBox::information(this, tr("Error while loading uniforms data : "), e.what());

			return false;
		}

		if(!uLoader.empty())
		{
			int c = 0;

			try
			{
				c = uLoader.applyTo(pipeline());

			}
			catch(Exception& e)
			{
				std::cerr << "Exception cauhgt : " << std::endl;
				std::cerr << e.what() << std::endl;
				QMessageBox::information(this, tr("Error while loading uniforms data : "), e.what());

				return false;
			}

			if(c==0)
			{
				std::string name = pipeline().getFullName();
				QMessageBox::information(this, tr("Error while loading uniforms data : "), tr("No data was loaded in the pipeline %1.").arg(name.c_str()));
				return false;
			}
			else
				std::cout << "UniformsTab::loadData - Loaded variables : " << c << std::endl;
		}
		else
			return false;

		// Update GUI : 
		pipelineWasCreated();

		HdlProgram::stopProgram();

		return true;
	}

	void UniformsTab::saveData(void)
	{
		saveData(lastSaveFilename);
	}

	void UniformsTab::saveData(const QString& filename)
	{
		if(filename.isEmpty())
			return ;

		// Sync !
		if(mainPipeline!=NULL)
		{
			mainPipeline->update(pipeline());
			HdlProgram::stopProgram();
		}

		// Load and save : 
		UniformsVarsLoader uWriter;

		uWriter.load(pipeline());

		if(uWriter.empty())
			return ;

		uWriter.writeToFile( filename.toStdString() );

		// Update current path : 
		openSaveInterface.reportSuccessfulSave(filename);
		openSaveInterface.enableSave(true);

		HdlProgram::stopProgram();

		lastSaveFilename = filename;
	}

	void UniformsTab::mainLibraryPipelineUpdate(void)
	{
		if( pipelineExists() )
		{
			if( mainLibraryMenu.process(pipeline()) )
			{
				requirePipelineComputation();
				pipelineWasCreated();		// Update the states.
			}
		}		
	}

	void UniformsTab::showContextMenu(const QPoint& point)
	{
		QPoint globalPos = tree.viewport()->mapToGlobal(point);

		linkToExternalValue.exec(globalPos);
	}
			
	void UniformsTab::pipelineWasCreated(void)
	{
		clear();

		std::vector<std::string> emptyPath;
		mainPipeline = new PipelineElement(pipeline(), pipeline().getName(), "", emptyPath, pipeline(), &tree, true);
		tree.expandAll();

		QObject::connect( mainPipeline, SIGNAL(updated(void)), this, SIGNAL(requestDataUpdate(void)) );
		QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), mainPipeline, SIGNAL(propagateSettings(BoxesSettings&)));

		// Set up settings : 
		settingsChanged();

		HdlProgram::stopProgram();

		mainLibraryMenu.update(pipeline());

		modified = false;

		openSaveInterface.enableOpen(true);
		openSaveInterface.enableSaveAs(true);
		openSaveInterface.enableSave(false);

		tree.resizeColumnToContents(0);
	}

	void UniformsTab::pipelineWasDestroyed(void)
	{
		clear();

		mainLibraryMenu.update();

		modified = false;

		openSaveInterface.enableOpen(false);
		openSaveInterface.enableSaveAs(false);
		openSaveInterface.enableSave(false);
	}

	void UniformsTab::mouseParametersWereUpdated(const GLSceneWidget::MouseData& data)
	{
		if(mainPipeline!=NULL)
		{
			bool test = mainPipeline->spreadExternalData(data);

			if(test)
				emit requestDataUpdate();
		}
	}

