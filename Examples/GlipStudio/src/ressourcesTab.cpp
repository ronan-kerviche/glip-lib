#include "ressourcesTab.hpp"
#include <algorithm>

// TextureObject
	TextureObject::TextureObject(const QString& _filename, int maxLevel)
	 : textureData(NULL), filename(_filename)
	{
		reload(maxLevel);
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

	void TextureObject::reload(int maxLevel)
	{
		try
		{	
			delete textureData;
			textureData = NULL;

			QImage image(filename);

			if(image.isNull())
				QMessageBox::information(NULL, QObject::tr("TextureObject"), QObject::tr("Cannot load image %1.").arg(filename));
			else
				textureData = ImageLoader::createTexture(image, GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP, maxLevel);
		}
		catch(Exception& e)
		{
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

	void ConnectionMenu::activate(bool state)
	{
		for(int k=0; k<currentActions.size(); k++)
			currentActions.at(k)->setEnabled(state);
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

		for(int k=0; k<layout.getNumInputPort(); k++)
		{
			QAction* action = addAction(tr("To %1").arg(layout.getInputPortName(k).c_str()));
			currentActions.push_back(action);
			mapper.setMapping(action, k);
			QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
		}
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
	 : QWidget(parent), layout(this), cancelButton("Cancel", this)
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
		filenames = QFileDialog::getOpenFileNames(this, tr("Load images : "), ".", tr("Image (*.bmp *.png *.jpg"));

		// If some file were selected, then start to load them : 
		if(!filenames.isEmpty())
		{
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

// Ressources GUI :
	RessourcesTab::RessourcesTab(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), connectionMenu(this), filterMenu(this), wrappingMenu(this), loadingWidget(this),
	   imageMenu("Image", this), loadImage("Load image...", this), freeImage("Free image", this),
	   currentOutputCategory(RessourceImages), currentOutputID(-1)
	{
		// Create image menu : 
		imageMenu.addAction(&loadImage);
		imageMenu.addAction(&freeImage);
	
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

			tree.addTopLevelItem(new QTreeWidgetItem(RessourceImages));
			tree.topLevelItem(RessourceImages)->setText(0, "Images (0)");
			tree.topLevelItem(RessourceImages)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(RessourceImages)->setFont(0, QFont("", 12));			
			
			tree.addTopLevelItem(new QTreeWidgetItem(RessourceFormats));
			tree.topLevelItem(RessourceFormats)->setText(0, "Formats (0)");
			tree.topLevelItem(RessourceFormats)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(RessourceFormats)->setFont(0, QFont("", 12));

			tree.addTopLevelItem(new QTreeWidgetItem(RessourceInputs));
			tree.topLevelItem(RessourceInputs)->setText(0, "Inputs (0)");
			tree.topLevelItem(RessourceInputs)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(RessourceInputs)->setFont(0, QFont("", 12));

			tree.addTopLevelItem(new QTreeWidgetItem(RessourceOutputs));
			tree.topLevelItem(RessourceOutputs)->setText(0, "Outputs (0)");
			tree.topLevelItem(RessourceOutputs)->setData(0, Qt::UserRole, QVariant(-1));
			tree.topLevelItem(RessourceOutputs)->setFont(0, QFont("", 12));

		// Build layout : 
		layout.addWidget(&menuBar);
		layout.addWidget(&tree);
		layout.addWidget(&loadingWidget);

		// Connections : 
		QObject::connect(&loadImage, 		SIGNAL(triggered()), 					&loadingWidget, SLOT(startLoad()));
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

	RessourcesTab::~RessourcesTab(void)
	{
		loadingWidget.cancel();

		for(std::vector<TextureObject*>::iterator it=textures.begin(); it!=textures.end(); it++)
			delete *it;
		textures.clear();

		formats.clear();

		preferredConnections.clear();
	}

// Tools : 
	QTreeWidgetItem* RessourcesTab::addItem(RessourceCategory category, QString title, int ressourceID)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(category);

		item->setText(0, title);

		// Set the link to the data : 
		item->setData(0, Qt::UserRole, QVariant(ressourceID));

		tree.topLevelItem(category)->addChild(item);
	
		return item;
	}

	void RessourcesTab::removeAllChildren(QTreeWidgetItem* root)
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

	void RessourcesTab::appendTextureInformation(QTreeWidgetItem* item)
	{
		item->setText(1, "");
		item->setText(2, "");
		item->setText(3, "");
		item->setText(4, "");
		item->setText(5, "");
	}

	void RessourcesTab::appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& fmt, size_t provideSize)
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

	void RessourcesTab::appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture)
	{
		// First on the format : 
		appendTextureInformation(item, texture.format(), texture.getSizeOnGPU());
	}
	
	void RessourcesTab::updateRessourceAlternateColors(QTreeWidgetItem* root)
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

	TextureObject* RessourcesTab::getCorrespondingTexture(QTreeWidgetItem* item)
	{
		if(item==NULL)
			return NULL;
		else if(item->type()!=RessourceImages)
			return NULL;
		else if(item->data(0, Qt::UserRole).toInt()<0)
			return NULL;
		else
			return textures[item->data(0, Qt::UserRole).toInt()];
	}

	FormatObject* RessourcesTab::getCorrespondingFormat(QTreeWidgetItem* item)
	{
		if(item==NULL)
			return NULL;
		else if(item->type()!=RessourceFormats)
			return NULL;
		else if(item->data(0, Qt::UserRole).toInt()<0)
			return NULL;
		else
			return &formats[item->data(0, Qt::UserRole).toInt()];
	}
	
// Update sections : 
	void RessourcesTab::rebuildImageList(void)
	{
		QTreeWidgetItem* root = tree.topLevelItem(RessourceImages);
		removeAllChildren(root);

		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
				QTreeWidgetItem* item = addItem(RessourceImages, "    -", k);
		}

		updateImageListDisplay();
	}

	void RessourcesTab::updateImageListDisplay(void)
	{
		int count = 0;
		QTreeWidgetItem* root = tree.topLevelItem(RessourceImages);

		for(int k=0; k<root->childCount(); k++)
		{
			TextureObject* obj = getCorrespondingTexture( root->child(k) );

			// Write infos : 
			QString title = tr("    %1").arg(obj->getName());

			root->child(k)->setText(0, title);

			appendTextureInformation(root->child(k), obj->texture());
		}

		// Update design : 
		updateRessourceAlternateColors(tree.topLevelItem(RessourceImages));
		
		// Update the title : 
		tree.topLevelItem(RessourceImages)->setText(0, tr("Images (%1)").arg(root->childCount()));
	}

	void RessourcesTab::updateFormatListDisplay(void)
	{
		QTreeWidgetItem* root = tree.topLevelItem(RessourceFormats);
		removeAllChildren(root);

		formats.clear();

		// List all current images formats : 
		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
			{
				// Get format : 
				FormatObject obj(tr("Format_%1").arg(textures[k]->getName()), textures[k]->texture());				 

				// Write infos : 
				QString title = tr("    %1").arg(obj.getName());

				QTreeWidgetItem* item = addItem(RessourceFormats, title, formats.size());

				appendTextureInformation(item, obj);

				// Add the format object to the list : 
				formats.push_back(obj);
			}
		}

		if(!formats.empty())
		{
			// Find and create the other formats
			std::vector<FormatObject> specials; 	

			specials.push_back( FormatObject("FormatLargest", formats[0]) );
			specials.push_back( FormatObject("FormatSmallest", formats[0]) );

			for(int k=1; k<formats.size(); k++)
			{
				if(specials[0].getNumPixels() < formats[k].getNumPixels())
					specials[0].setFormat( formats[k] );
				if(specials[1].getNumPixels() > formats[k].getNumPixels())
					specials[1].setFormat( formats[k] );
			}

			// Add them : 
			for(int k=0; k<specials.size(); k++)
			{
				QString title = tr("    %1").arg(specials[k].getName());

				QTreeWidgetItem* item = addItem(RessourceFormats, title, formats.size() + k);

				appendTextureInformation(item, specials[k]);
			}

			// Push them :
			formats.insert(formats.end(), specials.begin(), specials.end() );

			// Update design : 
			updateRessourceAlternateColors(root);		
		}

		// Update the title : 
		root->setText(0, tr("Formats (%1)").arg(formats.size()));
	}

	void RessourcesTab::updateInputConnectionDisplay(void)
	{
		int count = 0;
		QTreeWidgetItem* root = tree.topLevelItem(RessourceInputs);

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
		updateRessourceAlternateColors(tree.topLevelItem(RessourceInputs));
		
		// Update the title : 
		tree.topLevelItem(RessourceInputs)->setText(0, tr("Inputs (%1)").arg(root->childCount()));
	}

	void RessourcesTab::updateMenuOnCurrentSelection(ConnectionMenu* connections, FilterMenu* filters, WrappingMenu* wrapping, QAction* removeImage)
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

			// If they are all ressources :  
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

			if(allImages && selectedItems.size()==1)
			{
				if(connections!=NULL)	connections->activate(true);
			}			
			else
			{
				if(connections!=NULL)	connections->activate(false);
			}		
		}
		else
		{
			if(connections!=NULL)	connections->activate(false);
			if(filters!=NULL)	filters->update();
			if(wrapping!=NULL)	wrapping->update();
			if(removeImage!=NULL)	removeImage->setEnabled(false);
		}
	}

// Private Slots :
	void RessourcesTab::fetchLoadedImages(void)
	{
		loadingWidget.insertNewImagesInto(textures);
		rebuildImageList();

		// Open section : 
		tree.topLevelItem(RessourceImages)->setExpanded(true);
	}

	void RessourcesTab::selectionChanged(void)
	{
		// Update menus : 
		updateMenuOnCurrentSelection(&connectionMenu, &filterMenu, &wrappingMenu, &freeImage);

		// Update output, maybe : 
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(selectedItems.size()==1)
		{
			RessourceCategory category = static_cast<RessourceCategory>(selectedItems.front()->type());
			int id = selectedItems.front()->data(0, Qt::UserRole).toInt();

			if(((category==RessourceImages || category==RessourceOutputs) && id>=0) || (category==RessourceInputs && id>=0 && preferredConnections[id]!=NULL) )
			{
				currentOutputCategory 	= category;
				currentOutputID		= id;

				// Output changed : 
				emit outputChanged();
			}
		}
	}

	void RessourcesTab::updateImageFiltering(GLenum minFilter, GLenum magFilter)
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
				if(currentOutputID==selectedItems.at(k)->data(0, Qt::UserRole).toInt() && currentOutputCategory==RessourceImages)
					emit outputChanged();
			}

			updateImageListDisplay();

			if(needToUpdatePipeline)
				emit updatePipelineRequest();
		}
	}

	void RessourcesTab::updateImageWrapping(GLenum sWrapping, GLenum tWrapping)
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
				if(currentOutputID==selectedItems.at(k)->data(0, Qt::UserRole).toInt() && currentOutputCategory==RessourceImages)
					emit outputChanged();
			}

			updateImageListDisplay();

			if(needToUpdatePipeline)
				emit updatePipelineRequest();
		}
	}

	void RessourcesTab::showContextMenu(const QPoint& point)
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

				menu.addAction(&freeImage);
				menu.addMenu(&connectionMenu);

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

	void RessourcesTab::freeSelectedImages(void)
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

				if(currentOutputID==id && currentOutputCategory==RessourceImages)
				{
					currentOutputID = -1;
					emit outputChanged();
				}
			}
		}

		if(requestRebuild)
			rebuildImageList();

		if(requestPipelineUpdate)
		{
			updateInputConnectionDisplay();
			emit updatePipelineRequest();
		}
	}

	void RessourcesTab::applyConnection(int idInput)
	{
		QList<QTreeWidgetItem *> selectedItems = tree.selectedItems();

		if(selectedItems.size()==1)
		{
			if(preferredConnections.size() <= idInput)
				preferredConnections.resize(idInput+1, NULL);

			preferredConnections[idInput] = getCorrespondingTexture(selectedItems.front());
	
			// update : 
			updateInputConnectionDisplay();

			emit updatePipelineRequest();
		}
	}

// Public : 
	bool RessourcesTab::isInputConnected(int id) const
	{
		if(id<0 || id>=preferredConnections.size())
			return false;
		else
			return (preferredConnections[id]!=NULL);
	}

	HdlTexture& RessourcesTab::input(int id)
	{
		if(!isInputConnected(id))
			throw Exception("RessourcesTab::input - Internal error : input " + to_string(id) + " is not connected.", __FILE__, __LINE__);
		else
			return preferredConnections[id]->texture();
	}

	bool RessourcesTab::hasOutput(void) const
	{
		return ( (currentOutputCategory==RessourceImages || currentOutputCategory==RessourceInputs || currentOutputCategory==RessourceOutputs) && currentOutputID>=0 );
	}

	bool RessourcesTab::outputIsPartOfPipelineOutputs(void) const
	{
		return (currentOutputCategory==RessourceOutputs) && currentOutputID>=0;
	}

	HdlTexture* RessourcesTab::getOutput(Pipeline* pipeline)
	{
		if(currentOutputID<0)
			return NULL;

		switch(currentOutputCategory)
		{
			case RessourceImages :
				return &textures[currentOutputID]->texture();
			case RessourceInputs :
				return &preferredConnections[currentOutputID]->texture();
			case RessourceOutputs :
				if(pipeline==NULL)
					return NULL;
				else
					return &pipeline->out(currentOutputID);
			default :
				return NULL;
		}
	}

// Public slots : 
	void RessourcesTab::updatePipelineInfos(void)
	{
		// update the connection menu : 
		connectionMenu.update();

		QTreeWidgetItem* root = tree.topLevelItem(RessourceInputs);
		removeAllChildren(root);

		root->setText(0, tr("Inputs (0)"));

		root = tree.topLevelItem(RessourceOutputs);
		removeAllChildren(root);

		root->setText(0, tr("Outputs (0)"));
	}

	void RessourcesTab::updatePipelineInfos(Pipeline* pipeline)
	{
		if(pipeline==NULL)
			updatePipelineInfos();

		// update connection size : 
		if(pipeline->getNumInputPort()>preferredConnections.size())
			preferredConnections.resize(pipeline->getNumInputPort(), NULL);

		// update the connection menu : 
		connectionMenu.update(*pipeline);

		// Rebuild input list : 
		QTreeWidgetItem* root = tree.topLevelItem(RessourceInputs);
		removeAllChildren(root);

		for(int k=0; k<pipeline->getNumInputPort(); k++)
		{
			QString title = tr("    %1").arg(pipeline->getInputPortName(k).c_str());

			QTreeWidgetItem* item = addItem(RessourceInputs, title, k);

			item->setData(0, Qt::StatusTipRole, QString(pipeline->getInputPortName(k).c_str()));
		}
		
		// Update the list : 
		updateInputConnectionDisplay();

		tree.topLevelItem(RessourceInputs)->setExpanded(true);

		// Rebuild output list : 
		root = tree.topLevelItem(RessourceOutputs);
		removeAllChildren(root);

		for(int k=0; k<pipeline->getNumOutputPort(); k++)
		{
			QString title = tr("    %1").arg(pipeline->getOutputPortName(k).c_str());

			QTreeWidgetItem* item = addItem(RessourceOutputs, title, k);

			appendTextureInformation(item, pipeline->out(k));
		}

		// Update design : 
		updateRessourceAlternateColors(root);

		// Update the title : 
		root->setText(0, tr("Outputs (%1)").arg(pipeline->getNumOutputPort()));

		tree.topLevelItem(RessourceOutputs)->setExpanded(true);
	}


