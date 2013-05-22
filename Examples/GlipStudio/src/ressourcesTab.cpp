#include "ressourceTab.hpp"

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

// Ressources GUI :
	RessourcesTab::RessourcesTab(QWidget* parent=NULL)
	{
		
		// Connections : 
		QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	}

	RessourcesTab::~RessourcesTab(void)
	{

	}

// Tools : 
	QTreeWidgetItem* LibraryInterface::addItem(RessourceCategory category, QString title, int ressourceID)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(category);

		item->setText(0, title);

		// Set the link to the data : 
		item->setData(0, Qt::UserRole, QVariant(ressourceID));

		ressourceTab.topLevelItem(category)->addChild(item);
	
		return item;
	}

	void LibraryInterface::removeAllChildren(QTreeWidgetItem* root)
	{
		if(root==NULL)
			return ;
	
		while(root->childCount()>0)
		{
			QTreeWidgetItem* ptr = root->takeChild(0);
			delete ptr;
		}
	}

	void LibraryInterface::appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& fmt, size_t provideSize)
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

	void LibraryInterface::appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture)
	{
		// First on the format : 
		appendTextureInformation(item, texture.format(), texture.getSizeOnGPU());
	}

	TextureObject* LibraryInterface::getCorrespondingTexture(QTreeWidgetItem* item)
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

	FormatObject* LibraryInterface::getCorrespondingFormat(QTreeWidgetItem* item)
	{
		if(item==NULL)
			return NULL;
		else if(item->type()!=RessourceFormats)
			return NULL;
		else if(item->data(0, Qt::UserRole).toInt()<0)
			return NULL;
		else
			return formats[item->data(0, Qt::UserRole).toInt()];
	}
	
// Update sections : 
	void LibraryInterface::updateImageListDisplay(void)
	{
		int count = 0;
		QTreeWidgetItem* root = ressourceTab.topLevelItem(RessourceImages);
		removeAllChildren(root);

		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
			{
				// Write infos : 
				QString title = tr("    %1").arg(textures[k]->getName());

				QTreeWidgetItem* item = addItem(RessourceImages, title, k);

				appendTextureInformation(item, textures[k]->texture());
		
				count++;
			}
		}

		if(count>0)
		{
			// Update design : 
			updateRessourceAlternateColors(ressourceTab.topLevelItem(RessourceImages));
		}
		
		// Update the title : 
		ressourceTab.topLevelItem(RessourceImages)->setText(0, tr("Images (%1)").arg(count));
	}

	void LibraryInterface::updateFormatListDisplay(void)
	{
		QTreeWidgetItem* root = ressourceTab.topLevelItem(RessourceFormats);
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

	void LibraryInterface::updateInputConnectionDisplay(void)
	{
		QTreeWidgetItem* root = ressourceTab.topLevelItem(RessourceInputs);
		removeAllChildren(root);

		if(mainPipeline!=NULL)
		{
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				QString title;

				if(preferredConnections[k]==NULL)
					title = tr("    %1 (Not Connected)").arg(mainPipeline->getInputPortName(k).c_str());
				else
					title = tr("    %1 <- %2").arg(mainPipeline->getInputPortName(k).c_str()).arg(preferredConnections[k]->getName()); 

				QTreeWidgetItem* item = addItem(RessourceInputs, title, k);

				if(preferredConnections[k]!=NULL)
					appendTextureInformation(item, preferredConnections[k]->texture());
			}

			if( mainPipeline->getNumInputPort()>0 )
			{
				// Update design : 
				updateRessourceAlternateColors(root);	
			}
		
			// Update the title : 
			root->setText(0, tr("Inputs (%1)").arg(mainPipeline->getNumInputPort()));
		}
		else
			// Update the title : 
			root->setText(0, tr("Inputs (0)"));
	}

	void LibraryInterface::updateOutputConnectionDisplay(void)
	{
		QTreeWidgetItem* root = ressourceTab.topLevelItem(RessourceOutputs);
		removeAllChildren(root);

		if(mainPipeline!=NULL)
		{
			for(int k=0; k<mainPipeline->getNumOutputPort(); k++)
			{
				QString title= tr("    %1").arg(mainPipeline->getOutputPortName(k).c_str()); 

				QTreeWidgetItem* item = addItem(RessourceOutputs, title, k);

				appendTextureInformation(item, mainPipeline->out(k));
			}

			if( mainPipeline->getNumOutputPort()>0 )
			{
				// Update design : 
				updateRessourceAlternateColors(root);	
			}

			// Update the title : 
			root->setText(0, tr("Outputs (%1)").arg(mainPipeline->getNumOutputPort()));
		}
		else
			// Update the title : 
			root->setText(0, tr("Outputs (0)"));
	}

// Private Slots : 
	

