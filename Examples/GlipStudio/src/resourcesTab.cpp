#include "resourcesTab.hpp"
#include <algorithm>

// ConnectionMenu
	ConnectionMenu::ConnectionMenu(QWidget* parent)
	 : QMenu("Connect", parent)	
	{
		update();
	
		QObject::connect(&mapper, SIGNAL(mapped(int)), this, SIGNAL(connectToInput(int))); 
	}

	ConnectionMenu::~ConnectionMenu(void)
	{
		clear();
		currentActions.clear();
	}

	void ConnectionMenu::activate(bool state, int numConnections)
	{
		clear();
		currentActions.clear();

		if(state && numConnections==1 && !portsNames.isEmpty())
		{
			for(int k=0; k<portsNames.count(); k++)
			{
				QAction* action = addAction(tr("To %1").arg(portsNames[k]));
				currentActions.push_back(action);
				mapper.setMapping(action, k);
				QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
			}
		}
		else if(state && numConnections>1 && numConnections<=portsNames.count())
		{
			for(int k=0; k<portsNames.count()-numConnections+1; k++)
			{
				QAction* action = addAction(tr("From %1 to %2").arg(portsNames[k]).arg(portsNames[k + numConnections - 1]));
				currentActions.push_back(action);
				mapper.setMapping(action, k);
				QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
			}
		}
		else if(state && numConnections>portsNames.count() && !portsNames.isEmpty())
			addAction("Too many resources selected")->setEnabled(false);
		else if(portsNames.isEmpty())
			addAction("No input ports defined")->setEnabled(false);
		else if(!state)
			addAction("No suitable input port")->setEnabled(false);	
		else
			addAction("No available input ports")->setEnabled(false);
	}

	void ConnectionMenu::update(void)
	{
		clear();
		currentActions.clear();
		addAction("No available input port")->setEnabled(false);
	}

	void ConnectionMenu::update(const __ReadOnly_PipelineLayout& layout)
	{
		clear();
		currentActions.clear();
		portsNames.clear();

		for(int k=0; k<layout.getNumInputPort(); k++)
			portsNames.push_back(layout.getInputPortName(k).c_str());
	}

// ResourceTab
	ResourcesTab::ResourcesTab(ControlModule& _masterModule, QWidget* parent)
	 : Module(_masterModule, parent), layout(this), collection("ResourcesTab", this), viewManager(this)
	{
		collection.addActionsToMenuBar(menuBar);

		menuBar.addMenu(&connectionMenu);
		menuBar.addMenu(&viewManager);
		collection.addToContextMenu(connectionMenu);
		collection.addToContextMenu(viewManager);

		layout.addWidget(&menuBar);
		layout.addWidget(&collection);

		connect(&collection, 		SIGNAL(itemSelectionChanged()), 	this, SLOT(selectionChanged()));
		connect(&collection, 		SIGNAL(imageLoaded(int)),		this, SLOT(imageLoaded(int)));
		connect(&collection, 		SIGNAL(imageUnloadedFromDevice(int)),	this, SLOT(imageUnloadedFromDevice(int)));
		connect(&collection, 		SIGNAL(imageFreed(int)),		this, SLOT(imageFreed(int)));
		connect(&connectionMenu,	SIGNAL(connectToInput(int)),		this, SLOT(connectToInput(int)));
		connect(&viewManager,		SIGNAL(createNewView()),		this, SLOT(createNewView()));
	}

	ResourcesTab::~ResourcesTab(void)
	{ }

	// Private functions : 
		bool ResourcesTab::isValidTexture(int recordID)
		{
			return collection.imageExists(recordID);
		}

		HdlTexture& ResourcesTab::getTexture(int recordID)
		{
			return collection.texture(recordID);
		}

		void ResourcesTab::giveTextureInformation(int recordID, std::string& name)
		{
			name = collection.recordName(recordID);
		}

		void ResourcesTab::cleanRecordDependances(int recordID)
		{
			viewManager.removeRecord(recordID);
			unregisterInputTexture(recordID);
		}

		void ResourcesTab::updateTexturesLinkInformation(void)
		{
			if(pipelineExists())
			{
				for(std::vector<int>::iterator it = imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
				{
					int portID = 0;
					TextureStatus s 	= collection.recordStatus( *it );

					if( isListedAsPipelineInput(*it, &portID) )
					{
						if( isUsedAsPipelineInput(*it) )
						{
							s.connectionStatus 	= TextureStatus::Connected;
							s.portID		= portID;
							collection.updateRecordStatus( *it, s);
						}
						else 
						{
							s.connectionStatus 	= TextureStatus::WaitingLink;
							s.portID		= portID;
							collection.updateRecordStatus( *it, s);
						}
					}
					else
					{
						s.connectionStatus 	= TextureStatus::NotConnected;
						s.portID		= 0;
						collection.updateRecordStatus( *it, s);
					}
				}
			}
			else
			{
				for(std::vector<int>::iterator it = imageRecordIDs.begin(); it!=imageRecordIDs.end(); it++)
				{
					int portID = 0;

					if( isListedAsPipelineInput(*it, &portID) )
					{
						TextureStatus s 	= collection.recordStatus( *it );
						s.connectionStatus 	= TextureStatus::WaitingLink;
						s.portID		= portID;
						collection.updateRecordStatus( *it, s);
					}
					else
					{
						TextureStatus s 	= collection.recordStatus( *it );
						s.connectionStatus 	= TextureStatus::NotConnected;
						s.portID		= 0;
						collection.updateRecordStatus( *it, s);
					}
				}
			}
		}

		ViewLink* ResourcesTab::createViewLink(void* obj)
		{
			ResourcesTab* obj2 = reinterpret_cast<ResourcesTab*>(obj);
			return obj2->getViewLink();
		}

	// Private slots : 
		void ResourcesTab::pipelineWasCreated(void)
		{
			updateTexturesLinkInformation();

			// Update connection menu : 
			connectionMenu.update( pipeline() );

			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			connectionMenu.activate(!selectedRecordsID.empty(), selectedRecordsID.size());
		}

		void ResourcesTab::pipelineWasDestroyed(void)
		{
			updateTexturesLinkInformation();

			// Update connection menu : 
			connectionMenu.update();

			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			connectionMenu.activate(!selectedRecordsID.empty(), selectedRecordsID.size());
		}

		void ResourcesTab::selectionChanged(void)
		{
			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();
		
			if(selectedRecordsID.empty())
			{
				viewManager.enableCreationAction(false);
				connectionMenu.activate(false);
			}
			else
			{
				viewManager.enableCreationAction(true);

				connectionMenu.activate(true, selectedRecordsID.size());

				// Display : 
				viewManager.show( selectedRecordsID.front(), collection.texture(selectedRecordsID.front()), reinterpret_cast<void*>(this), &ResourcesTab::createViewLink);
			}
		}

		void ResourcesTab::connectToInput(int i)
		{
			std::vector<int> selectedRecordsID = collection.getSelectedRecordIDs();

			for(int k=0; k<selectedRecordsID.size(); k++)
			{
				registerInputTexture( selectedRecordsID[k], i + k);
				collection.lockTextureToDevice( selectedRecordsID[k] );

				TextureStatus s 	= collection.recordStatus( selectedRecordsID[k] );
				s.connectionStatus 	= TextureStatus::Connected;
				s.portID		= i + k;
				collection.updateRecordStatus( selectedRecordsID[k], s);
			}
		}

		void ResourcesTab::imageLoaded(int recordID)
		{
			imageRecordIDs.push_back(recordID);
		}

		void ResourcesTab::imageUnloadedFromDevice(int recordID)
		{
			cleanRecordDependances(recordID);
		}

		void ResourcesTab::imageFreed(int recordID)
		{
			cleanRecordDependances(recordID);

			std::vector<int>::iterator it = std::find(imageRecordIDs.begin(), imageRecordIDs.end(), recordID);
	
			imageRecordIDs.erase( it );
		}

		void ResourcesTab::createNewView(void)
		{
			std::vector<int> selectedRecordIDs = collection.getSelectedRecordIDs();

			if(!selectedRecordIDs.empty())
				viewManager.show( selectedRecordIDs.back(), collection.texture(selectedRecordIDs.front()), reinterpret_cast<void*>(this), &ResourcesTab::createViewLink, true);
		}

/*// TextureObject
	TextureObject::TextureObject(const QString& _filename, int maxLevel)
	 : virtualFile(false), textureData(NULL), filename(_filename)
	{
		reload(maxLevel);
	}

	TextureObject::TextureObject(HdlTexture& texture, const QString& _filename)
	 : virtualFile(true), textureData(NULL), filename(_filename)
	{
		textureData = new HdlTexture(texture.format());

		TextureCopier copier("TextureObjectCopier", texture.format(), texture.format(), true);

		copier.provideTexture(textureData);

		copier << texture << OutputDevice::Process;
	}

	TextureObject::~TextureObject(void)
	{
		delete textureData;
		textureData = NULL;
	}

	bool TextureObject::isValid(void) const
	{
		return textureData!=NULL;
	}

	bool TextureObject::isVirtual(void) const
	{
		return virtualFile;
	}

	void TextureObject::reload(int maxLevel)
	{
		try
		{	
			delete textureData;
			textureData = NULL;

			QFileInfo file(filename);
			
			if(file.completeSuffix()=="ppm" || file.completeSuffix()=="pgm")
			{
				NetPBM::Image img(filename.toStdString());

				textureData = img.createTexture();
			}
			else
			{
				QImage image(filename);

				if(image.isNull())
					QMessageBox::information(NULL, QObject::tr("TextureObject"), QObject::tr("Cannot load image %1.").arg(filename));
				else
					textureData = ImageLoader::createTexture(image, GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP, maxLevel);
			}
		}
		catch(Exception& e)
		{
			std::cout << "TextureObject::reload - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("TextureObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
		catch(std::exception& e)
		{
			std::cout << "TextureObject::reload - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("TextureObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
	}

	const QString& TextureObject::getFileName(void) const
	{
		return filename;
	}

	QString TextureObject::getName(void) const
	{
		QFileInfo info(filename);
		return info.fileName();
	}

	HdlTexture& TextureObject::texture(void)
	{
		if(!isValid())
			throw Exception("TextureObject::texture - object is invalid (load failed).", __FILE__, __LINE__);
		else
			return *textureData;
	}

// FormatObject
	FormatObject::FormatObject(const QString& _name, const __ReadOnly_HdlTextureFormat& fmt)
	 : HdlTextureFormat(fmt), name(_name)
	{ }

	FormatObject::FormatObject(const FormatObject& cpy)
	 : HdlTextureFormat(cpy), name(cpy.name)
	{ }

	const QString& FormatObject::getName(void) const
	{
		return name;
	}

	void FormatObject::setFormat(const __ReadOnly_HdlTextureFormat& cpy)
	{
		HdlTextureFormat::operator=(cpy);
	}

// FilterMenu
	FilterMenu::FilterMenu(QWidget* parent)
	 : 	QMenu("Filtering", parent),
		minFilter("Min filter", this),
		magFilter("Mag filter", this),
		bothNearest("Change both to GL_NEAREST", this),
		bothLinear("Change both to GL_LINEAR", this),
		minNearest("GL_NEAREST", this),
		minLinear("GL_LINEAR", this),
		minNearestMipmapNearest("GL_NEAREST_MIPMAP_NEAREST", this),
		minNearestMipmapLinear("GL_NEAREST_MIPMAP_LINEAR", this),
		minLinerarMipmapNearest("GL_LINEAR_MIPMAP_NEAREST", this),
		minLinearMipmapLinear("GL_LINEAR_MIPMAP_LINEAR", this),
		magNearest("GL_NEAREST", this),
		magLinear("GL_LINEAR", this)
	{
		minFilter.addAction(&minNearest);
		minFilter.addAction(&minLinear);
		minFilter.addAction(&minNearestMipmapNearest);
		minFilter.addAction(&minNearestMipmapLinear);
		minFilter.addAction(&minLinerarMipmapNearest);
		minFilter.addAction(&minLinearMipmapLinear);

		magFilter.addAction(&magNearest);
		magFilter.addAction(&magLinear);

		addAction(&bothNearest);
		addAction(&bothLinear);				
		addMenu(&minFilter);
		addMenu(&magFilter);

		update();

		QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(processAction(QAction*)));
	}

	void FilterMenu::update(void)
	{
		const bool state = false;
		
		bothNearest.setEnabled(state);
		bothLinear.setEnabled(state);
		minNearest.setEnabled(state);
		minLinear.setEnabled(state);
		magNearest.setEnabled(state);
		magLinear.setEnabled(state);
		minNearestMipmapNearest.setEnabled(state);
		minNearestMipmapLinear.setEnabled(state);
		minLinerarMipmapNearest.setEnabled(state);
		minLinearMipmapLinear.setEnabled(state);
	}

	void FilterMenu::update(const __ReadOnly_HdlTextureFormat& fmt)
	{
		const bool state = true;

		// Set default data : 
		bothNearest.setData( QVariant( QPoint( GL_NEAREST, GL_NEAREST ) ) );
		bothLinear.setData( QVariant( QPoint( GL_LINEAR, GL_LINEAR ) ) );

		minNearest.setData( QVariant( QPoint( GL_NEAREST, fmt.getMagFilter() ) ) );
		minLinear.setData( QVariant( QPoint( GL_LINEAR, fmt.getMagFilter() ) ) );

		minNearestMipmapNearest.setData( QVariant( QPoint( GL_NEAREST_MIPMAP_NEAREST, fmt.getMagFilter() ) ) );
		minNearestMipmapLinear.setData( QVariant( QPoint( GL_NEAREST_MIPMAP_LINEAR, fmt.getMagFilter() ) ) );
		minLinerarMipmapNearest.setData( QVariant( QPoint( GL_LINEAR_MIPMAP_NEAREST, fmt.getMagFilter() ) ) );
		minLinearMipmapLinear.setData( QVariant( QPoint( GL_LINEAR_MIPMAP_LINEAR, fmt.getMagFilter() ) ) );

		magNearest.setData( QVariant( QPoint( fmt.getMinFilter(), GL_NEAREST ) ) );
		magLinear.setData( QVariant( QPoint( fmt.getMinFilter(), GL_LINEAR ) ) );

		bothNearest.setEnabled(state);
		bothLinear.setEnabled(state);
		minNearest.setEnabled(state);
		minLinear.setEnabled(state);
		magNearest.setEnabled(state);
		magLinear.setEnabled(state);

		// Disable unaccessible data : 
		if(fmt.getMaxLevel()==0)
		{
			minNearestMipmapNearest.setEnabled(!state);
			minNearestMipmapLinear.setEnabled(!state);
			minLinerarMipmapNearest.setEnabled(!state);
			minLinearMipmapLinear.setEnabled(!state);
		}
		else
		{
			minNearestMipmapNearest.setEnabled(state);
			minNearestMipmapLinear.setEnabled(state);
			minLinerarMipmapNearest.setEnabled(state);
			minLinearMipmapLinear.setEnabled(state);
		}
	}

	void FilterMenu::get(QAction* action, GLenum& dminFilter, GLenum& dmagFilter)
	{
		if(action!=NULL)
		{
			dminFilter = action->data().toPoint().x();
			dmagFilter = action->data().toPoint().y();
		}
	}

	bool FilterMenu::ask(const QPoint& pos, GLenum& minFilter, GLenum& magFilter)
	{
		QAction* selectedItem = exec(pos);

		if(selectedItem!=NULL)
		{
			minFilter = selectedItem->data().toPoint().x();
			magFilter = selectedItem->data().toPoint().y();
			return true;
		}
		else
			return false;
	}

	void FilterMenu::processAction(QAction* action)
	{
		if(action!=NULL)
			emit changeFilter( action->data().toPoint().x(), action->data().toPoint().y() );
	}

// WrappingMenu
	WrappingMenu::WrappingMenu(QWidget* parent)
	 : 	QMenu("Wrapping", parent),
		sMenu("S Wrapping", this),
		tMenu("T Wrapping", this), 
		bothClamp("Change both to GL_CLAMP", this),
		bothClampToBorder("Change both to GL_CLAMP_TO_BORDER", this),
		bothClampToEdge("Change both to GL_CLAMP_TO_EDGE", this),
		bothRepeat("Change both to GL_REPEAT", this),
		bothMirroredRepeat("Change both to GL_MIRRORED_REPEAT", this),
		sClamp("GL_CLAMP", this),
		sClampToBorder("GL_CLAMP_TO_BORDER", this),
		sClampToEdge("GL_CLAMP_TO_EDGE", this),
		sRepeat("GL_REPEAT", this),
		sMirroredRepeat("GL_MIRRORED_REPEAT", this),
		tClamp("GL_CLAMP", this),
		tClampToBorder("GL_CLAMP_TO_BORDER", this),
		tClampToEdge("GL_CLAMP_TO_EDGE", this),
		tRepeat("GL_REPEAT", this),
		tMirroredRepeat("GL_MIRRORED_REPEAT", this)
	{
		sMenu.addAction(&sClamp);
		sMenu.addAction(&sClampToBorder);
		sMenu.addAction(&sClampToEdge);
		sMenu.addAction(&sRepeat);
		sMenu.addAction(&sMirroredRepeat);

		tMenu.addAction(&tClamp);
		tMenu.addAction(&tClampToBorder);
		tMenu.addAction(&tClampToEdge);
		tMenu.addAction(&tRepeat);
		tMenu.addAction(&tMirroredRepeat);

		addAction(&bothClamp);
		addAction(&bothClampToBorder);
		addAction(&bothClampToEdge);
		addAction(&bothRepeat);
		addAction(&bothMirroredRepeat);

		addMenu(&sMenu);
		addMenu(&tMenu);

		update();

		QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(processAction(QAction*)));
	}

	void WrappingMenu::update(void)
	{
		const bool state = false;

		bothClamp.setEnabled(state);
		bothClampToBorder.setEnabled(state);
		bothClampToEdge.setEnabled(state);
		bothRepeat.setEnabled(state);
		bothMirroredRepeat.setEnabled(state);
		sClamp.setEnabled(state);
		sClampToBorder.setEnabled(state);
		sClampToEdge.setEnabled(state);
		sRepeat.setEnabled(state);
		sMirroredRepeat.setEnabled(state);
		tClamp.setEnabled(state);
		tClampToBorder.setEnabled(state);
		tClampToEdge.setEnabled(state);
		tRepeat.setEnabled(state);
		tMirroredRepeat.setEnabled(state);
	}

	void WrappingMenu::update(const __ReadOnly_HdlTextureFormat& fmt)
	{
		const bool state = true;

		bothClamp.setEnabled(state);
		bothClampToBorder.setEnabled(state);
		bothClampToEdge.setEnabled(state);
		bothRepeat.setEnabled(state);
		bothMirroredRepeat.setEnabled(state);
		sClamp.setEnabled(state);
		sClampToBorder.setEnabled(state);
		sClampToEdge.setEnabled(state);
		sRepeat.setEnabled(state);
		sMirroredRepeat.setEnabled(state);
		tClamp.setEnabled(state);
		tClampToBorder.setEnabled(state);
		tClampToEdge.setEnabled(state);
		tRepeat.setEnabled(state);
		tMirroredRepeat.setEnabled(state);

		bothClamp.setData( QVariant( QPoint(		GL_CLAMP,		GL_CLAMP) ) );
		bothClampToBorder.setData( QVariant( QPoint(	GL_CLAMP_TO_BORDER, 	GL_CLAMP_TO_BORDER) ) );
		bothClampToEdge.setData( QVariant( QPoint(	GL_CLAMP_TO_EDGE,	GL_CLAMP_TO_EDGE) ) );
		bothRepeat.setData( QVariant( QPoint(		GL_REPEAT,		GL_REPEAT) ) );
		bothMirroredRepeat.setData( QVariant( QPoint(	GL_MIRRORED_REPEAT,	GL_MIRRORED_REPEAT) ) );

		sClamp.setData( QVariant( QPoint(		GL_CLAMP,		fmt.getTWrapping() ) ) );
		sClampToBorder.setData( QVariant( QPoint(	GL_CLAMP_TO_BORDER, 	fmt.getTWrapping() ) ) );
		sClampToEdge.setData( QVariant( QPoint(		GL_CLAMP_TO_EDGE,	fmt.getTWrapping() ) ) );
		sRepeat.setData( QVariant( QPoint(		GL_REPEAT,		fmt.getTWrapping() ) ) );
		sMirroredRepeat.setData( QVariant( QPoint(	GL_MIRRORED_REPEAT,	fmt.getTWrapping() ) ) );

		tClamp.setData( QVariant( QPoint(		fmt.getSWrapping(),	GL_CLAMP) ) );
		tClampToBorder.setData( QVariant( QPoint(	fmt.getSWrapping(),	GL_CLAMP_TO_BORDER) ) );
		tClampToEdge.setData( QVariant( QPoint(		fmt.getSWrapping(),	GL_CLAMP_TO_EDGE) ) );
		tRepeat.setData( QVariant( QPoint(		fmt.getSWrapping(),	GL_REPEAT) ) );
		tMirroredRepeat.setData( QVariant( QPoint(	fmt.getSWrapping(),	GL_MIRRORED_REPEAT) ) );
	}

	void WrappingMenu::get(QAction* action, GLenum& dsWrapping, GLenum& dtWrapping)
	{
		if(action!=NULL)
		{
			dsWrapping = action->data().toPoint().x();
			dtWrapping = action->data().toPoint().y();
		}
	}

	bool WrappingMenu::ask(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping)
	{
		QAction* selectedItem = exec(pos);

		if(selectedItem!=NULL)
		{
			sWrapping = selectedItem->data().toPoint().x();
			tWrapping = selectedItem->data().toPoint().y();
			return true;
		}
		else
			return false;
	}

	void  WrappingMenu::processAction(QAction* action)
	{
		if(action!=NULL)
			emit changeWrapping( action->data().toPoint().x(), action->data().toPoint().y() );
	}

// LoadingWidget : 
	LoadingWidget::LoadingWidget(QWidget* parent)
	 : QWidget(parent), layout(this), cancelButton("Cancel", this), currentPath(".")
	{
		QPalette pal = progressBar.palette();
		pal.setColor(QPalette::Text, Qt::white);
		progressBar.setPalette(pal);

		progressBar.setMinimum(0);
		progressBar.setMaximum(100);

		layout.addWidget(&progressBar);
		layout.addWidget(&cancelButton);

		QObject::connect(this, 		SIGNAL(loadNext(void)), this, SLOT(load(void)));
		QObject::connect(&cancelButton, SIGNAL(released(void)), this, SLOT(cancel(void)));

		hide();
	}

	LoadingWidget::~LoadingWidget(void)
	{
		clear();
		filenames.clear();
	}

	void LoadingWidget::clear(void)
	{
		for(std::vector<TextureObject*>::iterator it=loadedObjects.begin(); it!=loadedObjects.end(); it++)
			delete *it;
		loadedObjects.clear();
	}

	void LoadingWidget::cancel(void)
	{
		canceled = true;
	}

	void LoadingWidget::startLoad(void)
	{
		filenames.clear();

		// Get files : 
		filenames = QFileDialog::getOpenFileNames(this, tr("Load images : "), currentPath, tr("Image (*.bmp *.png *.jpg *.JPEG *.pgm *.ppm)"));

		// If some file were selected, then start to load them : 
		if(!filenames.isEmpty())
		{
			// Set the current path : 
			QFileInfo info(filenames.front());
			currentPath = info.path();

			canceled = false;
			clear();
			show();
			emit loadNext();
		}
	}

	void LoadingWidget::load(void)
	{
		if(canceled || (loadedObjects.size() >= filenames.size()) )
		{
			canceled = false;
			filenames.clear();
			clear();
			hide();

			return ;
		}

		try
		{
			QString currentFilename = filenames.at( loadedObjects.size() );

			// Try to load : 
			TextureObject* ptr = new TextureObject(currentFilename);

			// Save : 
			loadedObjects.push_back(ptr);
		}
		catch(Exception& e)
		{
			// Todo
			std::cerr << "Caught exception while loading : " << std::endl;
			std::cout << e.what() << std::endl;
		}

		progressBar.setValue(100 * loadedObjects.size() / filenames.size());

		if(loadedObjects.size() < filenames.size() )
			emit loadNext();
		else
		{
			filenames.clear();
			hide();

			// Ok : 
			emit finished();
		}
	}

	void LoadingWidget::insertNewImagesInto(std::vector<TextureObject*>& mainCollection)
	{
		mainCollection.insert( mainCollection.end(), loadedObjects.begin(), loadedObjects.end() );
		loadedObjects.clear();
	}

// Resources GUI :
	ResourcesTab::ResourcesTab(ControlModule& _masterModule, QWidget* parent)
	 : Module(_masterModule, parent), layout(this), menuBar(this), connectionMenu(this), filterMenu(this), wrappingMenu(this), loadingWidget(this),
	   imageMenu("Image", this), loadImage("Load image...", this), freeImage("Free image", this), saveOutputAs("Save output as...", this), copyAsNewResource("Copy as new resource...", this), 
	   currentOutputCategory(ResourceImages), currentOutputID(-1), infoLastComputeSucceeded(false), currentOutputPath("./")
	{
		// Create image menu : 
		imageMenu.addAction(&loadImage);
		imageMenu.addAction(&freeImage);
		imageMenu.addAction(&saveOutputAs);
		imageMenu.addAction(&copyAsNewResource);

		saveOutputAs.setEnabled(false);
	
		// Create Menu bar : 
		menuBar.addMenu(&imageMenu);
		menuBar.addMenu(&connectionMenu);
		menuBar.addMenu(&filterMenu);
		menuBar.addMenu(&wrappingMenu);

		// Create tree : 
			tree.setIndentation(2);
			tree.setSelectionMode(QAbstractItemView::ExtendedSelection);
			tree.setContextMenuPolicy(Qt::CustomContextMenu);

			QStringList listLabels;
			listLabels.push_back("Name");
			listLabels.push_back("Size");
			listLabels.push_back("Mode/Depth");
			listLabels.push_back("Min/Mag");
			listLabels.push_back("SWarp/TWarp");
			listLabels.push_back("Mipmap");
			tree.setHeaderLabels( listLabels );

			int currentFontSize = static_cast<int>( static_cast<double>(font().pointSize() * 1.2) );
            		QFont headerFont("", currentFontSize);

			tree.addTopLevelItem(new QTreeWidgetItem(ResourceImages));
			tree.topLevelItem(ResourceImages)->setText(0, "Images (0)");
			tree.topLevelItem(ResourceImages)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(ResourceImages)->setFont(0, headerFont);			
			
			tree.addTopLevelItem(new QTreeWidgetItem(ResourceFormats));
			tree.topLevelItem(ResourceFormats)->setText(0, "Formats (0)");
			tree.topLevelItem(ResourceFormats)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(ResourceFormats)->setFont(0, headerFont);

			tree.addTopLevelItem(new QTreeWidgetItem(ResourceInputs));
			tree.topLevelItem(ResourceInputs)->setText(0, "Inputs (0)");
			tree.topLevelItem(ResourceInputs)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(ResourceInputs)->setFont(0, headerFont);

			tree.addTopLevelItem(new QTreeWidgetItem(ResourceOutputs));
			tree.topLevelItem(ResourceOutputs)->setText(0, "Outputs (0)");
			tree.topLevelItem(ResourceOutputs)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(ResourceOutputs)->setFont(0, headerFont);

		// Build layout : 
		layout.addWidget(&menuBar);
		layout.addWidget(&tree);
		layout.addWidget(&loadingWidget);

		// Connections : 
		QObject::connect(&loadImage, 		SIGNAL(triggered()), 					&loadingWidget, SLOT(startLoad()));
		QObject::connect(&saveOutputAs,		SIGNAL(triggered()),					this,		SLOT(saveOutputToFile()));
		QObject::connect(&copyAsNewResource,	SIGNAL(triggered()),					this,		SLOT(copyOutputAsNewResource()));
		QObject::connect(&loadingWidget, 	SIGNAL(finished()), 					this, 		SLOT(fetchLoadedImages()));
		QObject::connect(&tree,			SIGNAL(itemSelectionChanged()),				this,		SLOT(selectionChanged()));
		QObject::connect(&filterMenu,		SIGNAL(changeFilter(GLenum, GLenum)),			this,		SLOT(updateImageFiltering(GLenum, GLenum)));
		QObject::connect(&wrappingMenu,		SIGNAL(changeWrapping(GLenum, GLenum)),			this,		SLOT(updateImageWrapping(GLenum, GLenum)));
		QObject::connect(&tree, 		SIGNAL(customContextMenuRequested(const QPoint&)), 	this, 		SLOT(showContextMenu(const QPoint&)));
		QObject::connect(&freeImage,		SIGNAL(triggered()),					this,		SLOT(freeSelectedImages()));
		QObject::connect(&connectionMenu,	SIGNAL(connectToInput(int)),				this,		SLOT(applyConnection(int)));

		// Update buttons to current selection : 
		selectionChanged();
	}

	ResourcesTab::~ResourcesTab(void)
	{
		loadingWidget.cancel();

		for(std::vector<TextureObject*>::iterator it=textures.begin(); it!=textures.end(); it++)
			delete *it;
		textures.clear();

		formats.clear();

		preferredConnections.clear();
	}

// Tools : 
	QTreeWidgetItem* ResourcesTab::addItem(ResourceCategory category, QString title, int resourceID)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(category);

		item->setText(0, title);

		// Set the link to the data : 
		item->setData(0, Qt::UserRole, QVariant(resourceID));

		tree.topLevelItem(category)->addChild(item);
	
		return item;
	}

	void ResourcesTab::removeAllChildren(QTreeWidgetItem* root)
	{
		// Disconnect : 
		tree.blockSignals(true);

		if(root==NULL)
			return ;

		while(root->childCount()>0)
		{
			QTreeWidgetItem* ptr = root->takeChild(0);
			delete ptr;
		}

		// reconnect : 
		tree.blockSignals(false);
	}

	void ResourcesTab::appendTextureInformation(QTreeWidgetItem* item)
	{
		item->setText(1, "");
		item->setText(2, "");
		item->setText(3, "");
		item->setText(4, "");
		item->setText(5, "");
	}

	void ResourcesTab::appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& fmt, size_t provideSize)
	{
		size_t s = fmt.getSize();

		if(provideSize!=0)
			s = provideSize;		
		
		QString sizeStr;

		if(s>1024*1024)
			sizeStr = tr("%1 MB").arg(s/(1024*1024));
		else
			sizeStr = tr("%1 KB").arg(s/(1024));

		item->setText(1, tr("%1 x %2 (%3)").arg(fmt.getWidth()).arg(fmt.getHeight()).arg(sizeStr));
		item->setText(2, tr("%1 / %2").arg(glParamName(fmt.getGLMode()).c_str()).arg(glParamName(fmt.getGLDepth()).c_str()));
		item->setText(3, tr("%1 / %2").arg(glParamName(fmt.getMinFilter()).c_str()).arg(glParamName(fmt.getMagFilter()).c_str()));
		item->setText(4, tr("%1 / %2").arg(glParamName(fmt.getSWrapping()).c_str()).arg(glParamName(fmt.getTWrapping()).c_str()));
		item->setText(5, tr("%1").arg(fmt.getMaxLevel()));
	}

	void ResourcesTab::appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture)
	{
		// First on the format : 
		appendTextureInformation(item, texture.format(), texture.getSizeOnGPU());
	}
	
	void ResourcesTab::updateResourceAlternateColors(QTreeWidgetItem* root)
	{
		QBrush 	foreground	= palette().foreground().color(),
			original 	= palette().background().color(),
			darker		= QBrush(original.color().lighter(80)),
			lighter		= QBrush(original.color().lighter(110));
		
		QBrush* ptr = NULL;
		for(int k=0; k<root->childCount(); k++)
		{
			if(k%2==0)
				ptr = &lighter;
			else
				ptr = &darker;

			for(int l=0; l<tree.columnCount(); l++)
			{
				root->child(k)->setForeground(l, QBrush(foreground));
				root->child(k)->setBackground(l, QBrush(*ptr));
			}
		}
	}

	TextureObject* ResourcesTab::getCorrespondingTexture(QTreeWidgetItem* item)
	{
		if(item==NULL)
			return NULL;
		else if(item->type()!=ResourceImages)
			return NULL;
		else if(item->data(0, Qt::UserRole).toInt()<0)
			return NULL;
		else
			return textures[item->data(0, Qt::UserRole).toInt()];
	}

	TextureObject* ResourcesTab::getCorrespondingTexture(const QString& name)
	{
		if(name.isEmpty())
			return NULL;
	
		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
			{
				if(textures[k]->getName()==name)
					return textures[k];
			}
		}

		return NULL;
	}

	FormatObject* ResourcesTab::getCorrespondingFormat(QTreeWidgetItem* item)
	{
		if(item==NULL)
			return NULL;
		else if(item->type()!=ResourceFormats)
			return NULL;
		else if(item->data(0, Qt::UserRole).toInt()<0)
			return NULL;
		else
			return &formats[item->data(0, Qt::UserRole).toInt()];
	}

	void ResourcesTab::appendNewImage(HdlTexture& texture, const QString& filename)
	{
		textures.push_back( new TextureObject(texture, filename) );
		rebuildImageList();

		// Open section : 
		tree.topLevelItem(ResourceImages)->setExpanded(true);
	}
	
// Update sections : 
	void ResourcesTab::rebuildImageList(void)
	{
		QTreeWidgetItem* root = tree.topLevelItem(ResourceImages);
		removeAllChildren(root);

		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
				QTreeWidgetItem* item = addItem(ResourceImages, "    -", k);
		}

		updateImageListDisplay();
	}

	void ResourcesTab::updateImageListDisplay(void)
	{
		int count = 0;
		QTreeWidgetItem* root = tree.topLevelItem(ResourceImages);

		for(int k=0; k<root->childCount(); k++)
		{
			TextureObject* obj = getCorrespondingTexture( root->child(k) );

			// Write infos : 
			QString title = tr("    %1").arg(obj->getName());

			root->child(k)->setText(0, title);

			appendTextureInformation(root->child(k), obj->texture());
		}

		// Update design : 
		updateResourceAlternateColors(tree.topLevelItem(ResourceImages));
		
		// Update the title : 
		tree.topLevelItem(ResourceImages)->setText(0, tr("Images (%1)").arg(root->childCount()));
	}

	void ResourcesTab::updateFormatListDisplay(void)
	{
		QTreeWidgetItem* root = tree.topLevelItem(ResourceFormats);
		removeAllChildren(root);

		// Rebuild Format list : 
			formats.clear();

			// Add inputs : 
			for(int k=0; k<preferredConnections.size(); k++)
			{
				if(preferredConnections[k]!=NULL)
					formats.push_back( FormatObject(tr("FormatIn_%1").arg(k), preferredConnections[k]->texture().format() ) );
			}

			// Add largest and smallest : 
			if(!formats.empty())
			{
				formats.push_back( FormatObject("FormatLargest", formats[0]) );
				formats.push_back( FormatObject("FormatSmallest", formats[0]) );

				int 	idLargest 	= formats.size() - 2,
					idSmallest	= formats.size() - 1;

				for(int k=1; k<(formats.size()-2); k++)
				{
					if(formats[idLargest].getNumPixels() < formats[k].getNumPixels())
						formats[idLargest].setFormat( formats[k] );
					if(formats[idSmallest].getNumPixels() > formats[k].getNumPixels())
						formats[idSmallest].setFormat( formats[k] );
				}
			}

		// Build list : 
		for(int k=0; k<formats.size(); k++)
		{
			QString title = tr("    %1").arg(formats[k].getName());
			QTreeWidgetItem* item = addItem(ResourceFormats, title, k);

			appendTextureInformation(item, formats[k]);
		}	

		updateResourceAlternateColors(root);
		root->setText(0, tr("Formats (%1)").arg(formats.size()));
	}

	void ResourcesTab::updateInputConnectionDisplay(void)
	{
		int count = 0;
		QTreeWidgetItem* root = tree.topLevelItem(ResourceInputs);

		for(int k=0; k<root->childCount(); k++)
		{
			// Write infos : 
			QString title,
				currentName = root->child(k)->data(0, Qt::StatusTipRole).toString();

			if(preferredConnections[k]!=NULL)
			{
				title = tr("    %1 <- %2").arg(currentName).arg(preferredConnections[k]->getName());
				appendTextureInformation(root->child(k), preferredConnections[k]->texture());
			}
			else
			{
				title = tr("    %1").arg(currentName);
				appendTextureInformation(root->child(k));
			}
			
			root->child(k)->setText(0, title);
		}

		// Update design : 
		updateResourceAlternateColors(tree.topLevelItem(ResourceInputs));
		
		// Update the title : 
		if(!pipelineName.isEmpty())
			tree.topLevelItem(ResourceInputs)->setText(0, tr("Inputs of %1 (%2)").arg(pipelineName).arg(root->childCount()));
		else
			tree.topLevelItem(ResourceInputs)->setText(0, tr("Inputs (%1)").arg(root->childCount()));
	}

	void ResourcesTab::updateMenuOnCurrentSelection(ConnectionMenu* connections, FilterMenu* filters, WrappingMenu* wrapping, QAction* removeImage, QAction* saveOutAs, QAction* copyOutAs)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(!selectedItems.isEmpty())
		{
			bool 	allImages 		= true,
				atLeastOneHeader	= false;

			for(int k=0; k<selectedItems.size() && allImages; k++)
			{
				allImages 		= allImages && (getCorrespondingTexture(selectedItems.at(k))!=NULL);
				atLeastOneHeader	= atLeastOneHeader || (selectedItems.at(k)->data(0, Qt::UserRole).toInt()>=0);	
			}

			// If they are all resources :  
			if(allImages) 
			{
				if(filters!=NULL)	filters->update( getCorrespondingTexture(selectedItems.at(0))->texture() );
				if(wrapping!=NULL)	wrapping->update( getCorrespondingTexture(selectedItems.at(0))->texture() );
				if(removeImage!=NULL)	removeImage->setEnabled(true);
			}
			else
			{
				if(filters!=NULL)	filters->update();
				if(wrapping!=NULL)	wrapping->update();
				if(removeImage!=NULL)	removeImage->setEnabled(false);
			}

			if(allImages && connections!=NULL)
				connections->activate(true, selectedItems.size());
			else if(connections!=NULL)
				connections->activate(false, selectedItems.size());

			if(selectedItems.size()==1 && selectedItems.front()->type()==ResourceOutputs && infoLastComputeSucceeded)
			{
				if(saveOutAs!=NULL)	saveOutAs->setEnabled(true);
				if(saveOutAs!=NULL)	copyOutAs->setEnabled(true);
			}			
			else
			{
				if(saveOutAs!=NULL)	saveOutAs->setEnabled(false);
				if(copyOutAs!=NULL)	copyOutAs->setEnabled(false);
			}
		}
		else
		{
			if(connections!=NULL)	connections->activate(false);
			if(filters!=NULL)	filters->update();
			if(wrapping!=NULL)	wrapping->update();
			if(removeImage!=NULL)	removeImage->setEnabled(false);
			if(saveOutAs!=NULL)	saveOutAs->setEnabled(false);
			if(copyOutAs!=NULL)	copyOutAs->setEnabled(false);
		}
	}

	void ResourcesTab::updateDisplay(void)
	{
		WindowRenderer* display = NULL;
		
		if( requireDisplay(display) )
			updateDisplay(display);
	}

	void ResourcesTab::updateDisplay(WindowRenderer*& display)
	{
		HdlTexture* output = getOutput();

		if(output!=NULL)
		{
			display->setImageAspectRatio(*output); 
			(*display) << (*output) << OutputDevice::Process;
		}
	}

	void ResourcesTab::clearPipelineInfo(void)
	{
		pipelineName.clear();

		// update the connection menu : 
		connectionMenu.update();
		
		QTreeWidgetItem* root = tree.topLevelItem(ResourceInputs);
		removeAllChildren(root);

		root->setText(0, tr("Inputs (0)"));

		root = tree.topLevelItem(ResourceOutputs);
		removeAllChildren(root);

		root->setText(0, tr("Outputs (0)"));

		// Update the selection : 
		selectionChanged();
	}

	void ResourcesTab::preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos)
	{
		//if(!formats.empty())
		//{
		//	for(int k=0; k<formats.size(); k++)
		//		loader.addRequiredElement(formats[k].getName().toStdString(), formats[k]);
		//}
		//else
		//{
		//	// Default, for first time load : 
		//	for(int k=0; k<infos.requiredFormats; k++)
		//		loader.addRequiredElement("FormatIn_" + to_string(k), HdlTextureFormat(1, 1, GL_RGB, GL_UNSIGNED_BYTE));
		//}

		for(int k=0; k<infos.requiredFormats.size(); k++)
		{
			if(k<formats.size())
				loader.addRequiredElement(infos.requiredFormats[k], formats[k]);
			else
				loader.addRequiredElement(infos.requiredFormats[k], HdlTextureFormat(1, 1, GL_RGB, GL_UNSIGNED_BYTE));
		}
	}

// Private Slots :
	void ResourcesTab::fetchLoadedImages(void)
	{
		loadingWidget.insertNewImagesInto(textures);
		rebuildImageList();

		// Open section : 
		tree.topLevelItem(ResourceImages)->setExpanded(true);
	}

	void ResourcesTab::selectionChanged(void)
	{
		// Update menus : 
		updateMenuOnCurrentSelection(&connectionMenu, &filterMenu, &wrappingMenu, &freeImage, &saveOutputAs, &copyAsNewResource);

		// Update output, maybe : 
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(selectedItems.size()==1)
		{
			ResourceCategory category = static_cast<ResourceCategory>(selectedItems.front()->type());
			int id = selectedItems.front()->data(0, Qt::UserRole).toInt();

			if(((category==ResourceImages || category==ResourceOutputs) && id>=0) || (category==ResourceInputs && id>=0 && preferredConnections[id]!=NULL) )
			{
				currentOutputCategory 	= category;
				currentOutputID		= id;

				// Output changed : 
				updateDisplay();
			}
		}
	}

	void ResourcesTab::updateImageFiltering(GLenum minFilter, GLenum magFilter)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(!selectedItems.isEmpty())
		{
			bool needToUpdatePipeline = false;

			for(int k=0; k<selectedItems.size(); k++)
			{
				TextureObject* ptr = getCorrespondingTexture(selectedItems.at(k));
				ptr->texture().setMinFilter(minFilter);
				ptr->texture().setMagFilter(magFilter);

				needToUpdatePipeline = needToUpdatePipeline || (std::find(preferredConnections.begin(), preferredConnections.end(), ptr)!=preferredConnections.end());

				// If this is the output : 
				if(currentOutputID==selectedItems.at(k)->data(0, Qt::UserRole).toInt() && currentOutputCategory==ResourceImages)
					updateDisplay();
			}

			updateImageListDisplay();

			if(needToUpdatePipeline)
			{
				updateFormatListDisplay();
				updateInputConnectionDisplay();
				emit updatePipelineRequest();
			}
		}
	}

	void ResourcesTab::updateImageWrapping(GLenum sWrapping, GLenum tWrapping)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(!selectedItems.isEmpty())
		{
			bool needToUpdatePipeline = false;

			for(int k=0; k<selectedItems.size(); k++)
			{
				TextureObject* ptr = getCorrespondingTexture(selectedItems.at(k));
				ptr->texture().setSWrapping(sWrapping);
				ptr->texture().setTWrapping(tWrapping);

				needToUpdatePipeline = needToUpdatePipeline || (std::find(preferredConnections.begin(), preferredConnections.end(), ptr)!=preferredConnections.end());

				// If this is the output : 
				if(currentOutputID==selectedItems.at(k)->data(0, Qt::UserRole).toInt() && currentOutputCategory==ResourceImages)
					updateDisplay();
			}

			updateImageListDisplay();

			if(needToUpdatePipeline)
			{
				updateFormatListDisplay();
				updateInputConnectionDisplay();
				emit updatePipelineRequest();
			}
		}
	}

	void ResourcesTab::showContextMenu(const QPoint& point)
	{
		// Get the global position : 
		QPoint globalPos = tree.viewport()->mapToGlobal(point); 

		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(!selectedItems.isEmpty())
		{
			int c = tree.currentColumn();
			
			if(c>=0 && c<=2)
			{
				QMenu menu;

				menu.addMenu(&connectionMenu);				
				menu.addAction(&saveOutputAs);
				menu.addAction(&copyAsNewResource);
				menu.addAction(&freeImage);

				QAction* action = menu.exec(globalPos);
			}
			else if(c==3)
			{
				FilterMenu menu;

				updateMenuOnCurrentSelection(NULL, &menu, NULL);

				QAction* action = menu.exec(globalPos);

				if(action!=NULL)
				{
					GLenum a, b;
					menu.get(action, a, b);
					updateImageFiltering(a, b);
				}
			}
			else if(c==4)
			{
				WrappingMenu menu;

				updateMenuOnCurrentSelection(NULL, NULL, &menu);

				QAction* action = menu.exec(globalPos);

				if(action!=NULL)
				{
					GLenum a, b;
					menu.get(action, a, b);
					updateImageWrapping(a, b);
				}
			}			
		}
	}

	void ResourcesTab::freeSelectedImages(void)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		bool    requestRebuild 		= false,
			requestPipelineUpdate 	= false;

		for(int k=0; k<selectedItems.size(); k++)
		{
			int id = selectedItems.at(k)->data(0, Qt::UserRole).toInt();

			if(textures[id]!=NULL)
			{
				std::vector<TextureObject*>::iterator it = std::find(preferredConnections.begin(), preferredConnections.end(), textures[id]);

				delete textures[id];
				textures[id] = NULL;

				if(it!=preferredConnections.end())
				{
					*it = NULL;
					requestPipelineUpdate = true;
				}

				requestRebuild = true;

				if(currentOutputID==id && currentOutputCategory==ResourceImages)
				{
					currentOutputID = -1;
					updateDisplay();
				}
			}
		}

		if(requestRebuild)
		{
			rebuildImageList();
			updateFormatListDisplay();
			updateInputConnectionDisplay();
		}

		if(requestPipelineUpdate)
		{
			updateInputConnectionDisplay();
			emit updatePipelineRequest();
		}
	}

	void ResourcesTab::applyConnection(int idInput)
	{
		if(requirePrepareToPipelineInputsModification())
		{
			QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

			for(int k=0; k<selectedItems.size(); k++)
			{
				if(preferredConnections.size() <= idInput + k)
					preferredConnections.resize(idInput + k + 1, NULL);

				preferredConnections[idInput + k] = getCorrespondingTexture(selectedItems[k]);
			}

			// update : 
			updateInputConnectionDisplay();
			updateFormatListDisplay();

			emit pipelineInputsModification();
		}
	}

	void ResourcesTab::startRequestSaveImage(void)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(selectedItems.size()==1)
		{
			int id = selectedItems.front()->data(0, Qt::UserRole).toInt();

			if(selectedItems.front()->type()==ResourceOutputs && id>=0 && infoLastComputeSucceeded)
				emit saveOutput(id);
		}
	}

	void ResourcesTab::requestCopyAsNewResource(void)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(selectedItems.size()==1)
		{
			int id = selectedItems.front()->data(0, Qt::UserRole).toInt();

			if(selectedItems.front()->type()==ResourceOutputs && id>=0 && infoLastComputeSucceeded)
				emit copyOutputAsNewResource(id);
		}
	}

// Public : 
	void ResourcesTab::appendFormats(LayoutLoader& loader)
	{
		
	}

	bool ResourcesTab::isInputConnected(int id) const
	{
		if(id<0 || id>=preferredConnections.size())
			return false;
		else
			return (preferredConnections[id]!=NULL);
	}

	HdlTexture& ResourcesTab::input(int id)
	{
		if(!isInputConnected(id))
			throw Exception("ResourcesTab::input - Internal error : input " + to_string(id) + " is not connected.", __FILE__, __LINE__);
		else
			return preferredConnections[id]->texture();
	}

	bool ResourcesTab::hasOutput(void) const
	{
		return ( (currentOutputCategory==ResourceImages || currentOutputCategory==ResourceInputs || currentOutputCategory==ResourceOutputs) && currentOutputID>=0 );
	}

	bool ResourcesTab::outputIsPartOfPipelineOutputs(void) const
	{
		return (currentOutputCategory==ResourceOutputs) && currentOutputID>=0;
	}

	HdlTexture* ResourcesTab::getOutput(void)
	{
		if(currentOutputID<0)
			return NULL;

		switch(currentOutputCategory)
		{
			case ResourceImages :
				return &textures[currentOutputID]->texture();
			case ResourceInputs :
				return &preferredConnections[currentOutputID]->texture();
			case ResourceOutputs :
				if( pipelineExists() )
					return &pipeline().out(currentOutputID);
				else
					return NULL;
			default :
				return NULL;
		}
	}

// Private slots : 
	void ResourcesTab::pipelineWasCreated(void)
	{
		pipelineName = pipeline().getName().c_str();

		// update connection size : 
		if(pipeline().getNumInputPort()>preferredConnections.size())
			preferredConnections.resize(pipeline().getNumInputPort(), NULL);

		// update the connection menu : 
		connectionMenu.update(pipeline());

		// Rebuild input list : 
		QTreeWidgetItem* root = tree.topLevelItem(ResourceInputs);
		removeAllChildren(root);

		for(int k=0; k<pipeline().getNumInputPort(); k++)
		{
			QString title = tr("    %1").arg(pipeline().getInputPortName(k).c_str());

			QTreeWidgetItem* item = addItem(ResourceInputs, title, k);

			item->setData(0, Qt::StatusTipRole, QString(pipeline().getInputPortName(k).c_str()));
		}
		
		// Update the list : 
		updateInputConnectionDisplay();

		tree.topLevelItem(ResourceInputs)->setExpanded(true);

		// Rebuild output list : 
		root = tree.topLevelItem(ResourceOutputs);
		removeAllChildren(root);

		for(int k=0; k<pipeline().getNumOutputPort(); k++)
		{
			QString title = tr("    %1").arg(pipeline().getOutputPortName(k).c_str());

			QTreeWidgetItem* item = addItem(ResourceOutputs, title, k);

			appendTextureInformation(item, pipeline().out(k));
		}

		// Update design : 
		updateResourceAlternateColors(root);

		// Update the title : 
		if(!pipelineName.isEmpty())
			root->setText(0, tr("Outputs of %1 (%2)").arg(pipelineName).arg(pipeline().getNumOutputPort()));
		else
			root->setText(0, tr("Outputs (%1)").arg(pipeline().getNumOutputPort()));
	
		tree.topLevelItem(ResourceOutputs)->setExpanded(true);

		if( (currentOutputCategory==ResourceOutputs && currentOutputID>=pipeline().getNumOutputPort()) || (currentOutputCategory==ResourceInputs && currentOutputID>=pipeline().getNumInputPort()) )
		{
			currentOutputID = -1;
			updateDisplay();
		}

		// Update the selection : 
		selectionChanged();
	}

	void ResourcesTab::pipelineCompilationFailed(Exception& e)
	{
		clearPipelineInfo();
	}

	void ResourcesTab::pipelineWasDestroyed(void)
	{
		clearPipelineInfo();
	}

	void ResourcesTab::saveOutputToFile(void)
	{
		try
		{
			HdlTexture* output = getOutput();

			if(output!=NULL)
			{
				QImage image = ImageLoader::createImage(*output);

				QString filename = QFileDialog::getSaveFileName(this, "Save Image", currentOutputPath, tr("Image (*.jpg *.jpeg *.png *.bmp)"));

				if(!filename.isEmpty())
				{
					if(!image.save(filename))
						QMessageBox::warning(this, tr("Warning"), tr("Save operation to file \"%1\" failed.").arg(filename));
					else
					{
						// Set the current path for output : 
						QFileInfo info(filename);
						currentOutputPath = info.path();
					}
				}
			}
		}
		catch(Exception& e)
		{
			std::cerr << "ResourcesTab::saveOutputToFile - Exception : " << std::endl;
			std::cerr << e.what();
		
			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("Could not save the image. This might be due to the image having an incompatible format."), QMessageBox::Ok, this);
			messageBox.setDetailedText(e.what());
			messageBox.exec();
		}
	}

	void ResourcesTab::copyOutputAsNewResource(void)
	{
		bool 	over = false,
			ok = false;
	    	QString name;

		HdlTexture* output = getOutput();

		if(output!=NULL)
		{
			do
			{
				// Get the new name : 
				if(getCorrespondingTexture(name)==NULL)
					name = QInputDialog::getText(this, tr("Copy Output as new Resource..."), tr("New resource name : "), QLineEdit::Normal, tr("Resource_%1").arg(textures.size()+1), &ok);
				else
					name = QInputDialog::getText(this, tr("Copy Output as new Resource..."), tr("A resource with the name \"%1\" already exists, choose a different resource name : ").arg(name), QLineEdit::Normal, tr("Resource_%1").arg(textures.size()+1), &ok);

				// Check if the name exits : 
				over = !ok || (!name.isEmpty() && (getCorrespondingTexture(name)==NULL));

			} while(!over);

				// Copy : 
			if(ok)
				appendNewImage(*output, name);
		}
	}

	void ResourcesTab::updateLastComputingStatus(bool succeeded)
	{
		infoLastComputeSucceeded = succeeded;
	}*/

