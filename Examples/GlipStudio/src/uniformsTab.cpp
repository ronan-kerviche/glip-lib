#include "uniformsTab.hpp"
#include <limits>

// UniformObject	
	UniformObject::UniformObject(const std::string& _varName)
	 : QWidget(NULL), varName(_varName), hasBeenUpdated(false)
	{
		std::cout << "UniformObject::UniformObject : " << this << std::endl;
		setAutoFillBackground(true);
	}

	UniformObject::~UniformObject(void)
	{
		std::cout << "UniformObject::~UniformObject : " << this << std::endl;
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

// UniformUnknown
	UniformUnknown::UniformUnknown(const std::string& name, const QString& message)
	 : UniformObject(name), layout(this), label(message, this)
	{
		std::cout << "UniformUnknown::UniformUnknown : " << this << std::endl;
		item = new QTreeWidgetItem(NodeVarInteger);

		layout.addWidget(&label);

		item->setText(0, tr("%1 [x]").arg(getVarName().c_str()));
	}

	UniformUnknown::~UniformUnknown(void)
	{
		std::cout << "UniformUnknown::~UniformUnknown : " << this << std::endl;
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

// UniformInteger
	UniformInteger::UniformInteger(const std::string& name, GLenum _type, int n, int m, bool _un)
	 : UniformObject(name), un(_un), item(NULL), layout(this), type(_type)
	{
		std::cout << "UniformInteger::UniformInteger : " << this << std::endl;

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

		if(un)
			item->setText(0, tr("%1 [%2x%3xunsigned]").arg(getVarName().c_str()).arg(n).arg(m) );
		else
			item->setText(0, tr("%1 [%2x%3xint]").arg(getVarName().c_str()).arg(n).arg(m) );
	}

	UniformInteger::~UniformInteger(void)
	{
		std::cout << "UniformInteger::~UniformInteger : " << this << std::endl;

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

// UniformFloat
	UniformFloat::UniformFloat(const std::string& name, GLenum _type, int n, int m)
	 : UniformObject(name), item(NULL), layout(this), type(_type)
	{
		std::cout << "UniformFloat::UniformFloat : " << this << std::endl;

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
		item = new QTreeWidgetItem(NodeVarInteger);

		item->setText(0, tr("%1 [%2x%3xfloat]").arg(getVarName().c_str()).arg(n).arg(m) );
	}

	UniformFloat::~UniformFloat(void)
	{
		std::cout << "UniformFloat::~UniformFloat : " << this << std::endl;

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

// FilterGroup
	FilterElement::FilterElement(Filter& filter, const std::string& _path, QTreeWidget* tree)
	 : name(filter.getName()), path(_path), item(NULL)
	{
		std::cout << "FilterElement::FilterElement : " << this << std::endl;

		item = new QTreeWidgetItem(NodeFilter);

		item->setText(0, tr("%1 [%2]").arg(name.c_str()).arg(path.c_str()) );

		// List the variables : 
		for(int k=0; k<filter.prgm().getUniformVarsNames().size(); k++)
		{
			bool processNewItem = true;

			if(filter.prgm().getUniformVarsNames()[k]=="gl_ModelViewMatrix" || filter.prgm().getUniformVarsNames()[k]=="gl_TextureMatrix[0]")
				objects.push_back( reinterpret_cast<UniformObject*>(new UniformUnknown(filter.prgm().getUniformVarsNames()[k], "(Forbidden)")) );
			else
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
			}

			objects.back()->read(filter.prgm());

			item->addChild( objects.back()->treeItem() );
				
			tree->setItemWidget( objects.back()->treeItem(), 1, objects.back() );

			QObject::connect( objects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
			QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), objects.back(), SLOT(applySettings(BoxesSettings&)));
		}
	}

	FilterElement::~FilterElement(void)
	{
		std::cout << "FilterElement::~FilterElement : " << this << std::endl;
	}

	QTreeWidgetItem* FilterElement::treeItem(void) const
	{
		return item;
	}

	void FilterElement::update(Pipeline& pipeline)
	{
		Filter& filter = pipeline[path + SEPARATOR + name];

		for(int k=0; k<objects.size(); k++)
		{
			if(objects[k]->wasUpdated())
				objects[k]->update(filter.prgm());
		}
	}

// PipelineElement
	PipelineElement::PipelineElement(const __ReadOnly_PipelineLayout& pipeline, const std::string& path, Pipeline& mainPipeline, QTreeWidget* tree, bool isRoot)
	 : item(NULL)
	{
		std::cout << "PipelineElement::PipelineElement : " << this << std::endl;

		item = new QTreeWidgetItem(NodeFilter);

		if(!path.empty())
			item->setText(0, tr("%1 [%2]").arg(pipeline.getName().c_str()).arg(path.c_str()));
		else
			item->setText(0, tr("%1").arg(pipeline.getName().c_str()) );

		std::string currentPath;

		if(!path.empty())
			currentPath = path + SEPARATOR + pipeline.getName();
		else
			currentPath = pipeline.getName();

		if(isRoot)
			tree->addTopLevelItem(item);

		// Store the sub-elements : 
		for(int k=0; k< pipeline.getNumElements(); k++)
		{
			__ReadOnly_PipelineLayout::ComponentKind kind = pipeline.getElementKind(k);
			
			if( kind==__ReadOnly_PipelineLayout::FILTER )
			{
				Filter& filter = mainPipeline[pipeline.componentLayout(k).getName()];

				filterObjects.push_back( new FilterElement(filter, currentPath, tree) );

				item->addChild( filterObjects.back()->treeItem() );
				filterObjects.back()->treeItem()->setExpanded(true);

				QObject::connect( filterObjects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
				QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), filterObjects.back(), SIGNAL(propagateSettings(BoxesSettings&)));
			}
			else if( kind==__ReadOnly_PipelineLayout::PIPELINE )
			{
				pipelineObjects.push_back( new PipelineElement( pipeline.pipelineLayout(k), currentPath, mainPipeline, tree) );

				item->addChild( pipelineObjects.back()->treeItem() );

				QObject::connect( pipelineObjects.back(), SIGNAL(updated(void)), this, SIGNAL(updated(void)) );
				QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), pipelineObjects.back(), SIGNAL(propagateSettings(BoxesSettings&)));
			}
			else
				throw Exception("PipelineElement::PipelineElement - Internal error : unable to read component type.", __FILE__, __LINE__);
		}

		item->setExpanded(true);
	}

	PipelineElement::~PipelineElement(void)
	{
		std::cout << "PipelineElement::~PipelineElement : " << this << std::endl;

		item = NULL;

		for(int k=0; k<filterObjects.size(); k++)
			filterObjects[k]->deleteLater();
		filterObjects.clear();

		for(int k=0; k<pipelineObjects.size(); k++)
			pipelineObjects[k]->deleteLater();
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
		
			minIntBox.setValue(-1024);
			maxIntBox.setValue(1024);
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
		
			maxUnBox.setValue(1024);
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
		
			minDoubleBox.setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
			maxDoubleBox.setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
			stepDoubleBox.setRange(0.0, std::numeric_limits<double>::max());
			stepDoubleBox.setSingleStep(0.01);
		
			minDoubleBox.setValue(-1e3);
			maxDoubleBox.setValue(1e3);
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

// UniformsTab
	UniformsTab::UniformsTab(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), showSettings("Settings", this), tree(this), settings(this), mainPipeline(NULL)
	{
		menuBar.addAction(&showSettings);

		layout.addWidget(&menuBar);
		layout.addWidget(&tree);
		layout.addWidget(&settings);
		
		settings.hide();

		tree.setIndentation(8);
		tree.setSelectionMode(QAbstractItemView::NoSelection);

		QStringList listLabels;
		listLabels.push_back("Name");
		listLabels.push_back("Data");
		tree.setHeaderLabels( listLabels );

		QObject::connect(&showSettings,	SIGNAL(triggered()),		this, 	SLOT(switchSettings()));
		QObject::connect(&settings,	SIGNAL(settingsChanged()),	this,	SLOT(settingsChanged()));
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

		QTreeWidgetItem* root = tree.takeTopLevelItem(0);

		while(root->childCount()>0)
		{
			QTreeWidgetItem* ptr = root->takeChild(0);
			delete ptr;
		}

		delete root;

		// reconnect : 
		tree.blockSignals(false);

		mainPipeline->deleteLater();
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

	void UniformsTab::updatePipeline(void)
	{
		clear();
	}

	void UniformsTab::updatePipeline(Pipeline& pipeline)	
	{
		clear();

		mainPipeline = new PipelineElement(pipeline, "", pipeline, &tree, true);

		QObject::connect( mainPipeline, SIGNAL(updated(void)), this, SIGNAL(requestDataUpdate(void)) );
		QObject::connect( this, SIGNAL(propagateSettings(BoxesSettings&)), mainPipeline, SIGNAL(propagateSettings(BoxesSettings&)));

		// Set up settings : 
		settingsChanged();

		HdlProgram::stopProgram();
	}

	void UniformsTab::updateData(Pipeline& pipeline)
	{
		if(mainPipeline!=NULL)
		{
			mainPipeline->update(pipeline);
			HdlProgram::stopProgram();
		}
	}

