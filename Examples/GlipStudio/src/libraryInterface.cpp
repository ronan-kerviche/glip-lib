#include "libraryInterface.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

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

// LibraryInterface :
	LibraryInterface::LibraryInterface(QWidget *parent)
	 : QWidget(parent), menuBar(this), layout(this), tabs(this), ressourceTab(this), uniformsTab(this),
	   openAct(tr("&Load Image"), this), saveAct(tr("&Save Output"), this), saveAsAct(tr("&Save Output as"), this),
	   currentOutTexture(NULL)
	{
		// Layout : 
		openAct.setShortcuts(QKeySequence::Open);
		openAct.setStatusTip(tr("Load Image ..."));
		QObject::connect(&openAct, SIGNAL(triggered()), this, SLOT(loadImage(void)));

		saveAct.setShortcuts(QKeySequence::Save);
		saveAct.setStatusTip(tr("Save output ..."));
		//QObject::connect(&saveAct, SIGNAL(triggered()), this, SLOT(save()));

		saveAsAct.setShortcuts(QKeySequence::SaveAs);
		saveAsAct.setStatusTip(tr("Save output to ..."));
		//QObject::connect(&saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

		menuBar.addAction(&openAct);
		menuBar.addAction(&saveAct);
		menuBar.addAction(&saveAsAct);

		tabs.addTab(&ressourceTab, 	"Ressources");
		tabs.addTab(&compilationTab,	"Compilation");
		tabs.addTab(&uniformsTab, 	"Uniforms");

		layout.addWidget(&menuBar);
		layout.addWidget(&tabs);

		// Ressources : 
			ressourceTab.setIndentation(1);

			QStringList listLabels;
			listLabels.push_back("Name");
			listLabels.push_back("Size");
			listLabels.push_back("Min/Mag");
			listLabels.push_back("SWarp/TWarp");
			listLabels.push_back("Mipmap");
			ressourceTab.setHeaderLabels( listLabels );

			ressourcesHeaders.push_back(new QTreeWidgetItem(RessourceImages));
			ressourcesHeaders.back()->setText(0, "Images");
			ressourcesHeaders.push_back(new QTreeWidgetItem(RessourceFormats));
			ressourcesHeaders.back()->setText(0, "Formats");
			ressourcesHeaders.push_back(new QTreeWidgetItem(RessourceOutputs));
			ressourcesHeaders.back()->setText(0, "Outputs");

			ressourceTab.addTopLevelItems(ressourcesHeaders);

			QObject::connect(&ressourceTab, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(imageSelected(QTreeWidgetItem*, int)));
	}

	LibraryInterface::~LibraryInterface(void)
	{
		for(int k=0; k<textures.size(); k++)
			delete textures[k];
		textures.clear();
	}

	void LibraryInterface::imageSelected(QTreeWidgetItem* item, int column)
	{
		int id = item->data(0, Qt::UserRole).toInt();

		if(textures[id]!=NULL)
		{
			currentOutTexture = &textures[id]->texture();
			emit requireRedraw();
		}
	}

	void LibraryInterface::loadImage(void)
	{
		QString filename = QFileDialog::getOpenFileName(this, tr("Load an Image"), ".", tr("Image (*.bmp *.png *.jpg"));
		
		if(!filename.isEmpty())
		{
			TextureObject* obj = new TextureObject(filename);

			if(obj->isValid())
			{
				// Write infos : 
				imagesList.push_back(new QTreeWidgetItem(RessourceImages));

				imagesList.back()->setText(0, tr("    %1").arg(obj->getName()));

				size_t s = obj->texture().getSizeOnGPU(0);
				QString sizeStr;

				if(s>1024*1024)
					sizeStr = tr("%1 MB").arg(s/(1024*1024));
				else
					sizeStr = tr("%1 KB").arg(s/(1024));

				imagesList.back()->setText(1, tr("%1 x %2 (%3)").arg(obj->texture().getWidth()).arg(obj->texture().getHeight()).arg(sizeStr));
				imagesList.back()->setText(2, tr("%1 / %2").arg(glParamName(obj->texture().getMinFilter()).c_str()).arg(glParamName(obj->texture().getMagFilter()).c_str()));
				imagesList.back()->setText(3, tr("%1 / %2").arg(glParamName(obj->texture().getSWrapping()).c_str()).arg(glParamName(obj->texture().getTWrapping()).c_str()));
				imagesList.back()->setText(4, tr("%1").arg(obj->texture().getMaxLevel()));

				// Set the link to the data : 
				imagesList.back()->setData(0, Qt::UserRole, QVariant(static_cast<int>(textures.size())));

				ressourcesHeaders[0]->addChild(imagesList.back());

				// Add the texture object to the list : 
				textures.push_back(obj);

				// Update the title : 
				ressourcesHeaders[0]->setText(0, tr("Images (%1)").arg(getNumImages()));

				// Update design : 
				updateFormatList();
				updateRessourceAlternateColors();
			}
			else
			{
				delete obj;
				obj = NULL;
			}
		}
	}

	void LibraryInterface::updateFormatList(void)
	{
		
	}

	void LibraryInterface::updateRessourceAlternateColors(void)
	{
		QTreeWidgetItem* root = ressourceTab.topLevelItem(0);

		QBrush 	foreground	= ressourceTab.palette().foreground().color(),
			original 	= ressourceTab.palette().background().color(),
			darker		= QBrush(original.color().lighter(80)),
			lighter		= QBrush(original.color().lighter(120));
		
		QBrush* ptr = NULL;
		for(int k=0; k<root->childCount(); k++)
		{
			if(k%2==0)
				ptr = &lighter;
			else
				ptr = &darker;

			for(int l=0; l<ressourceTab.columnCount(); l++)
			{
				root->child(k)->setForeground(l, QBrush(foreground));
				root->child(k)->setBackground(l, QBrush(*ptr));
			}
		}
	}

	int LibraryInterface::getNumImages(void) const
	{
		int res = 0;

		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
				res++;
		}

		return res;
	}

	bool LibraryInterface::hasOutput(void) const
	{
		return currentOutTexture!=NULL;
	}

	HdlTexture& LibraryInterface::currentOutput(void)
	{
		if(currentOutTexture!=NULL)
			return *currentOutTexture;
		else
			throw Exception("LibraryInterface::currentOutput - Internal error : no currently associated texture.", __FILE__, __LINE__);
	}


