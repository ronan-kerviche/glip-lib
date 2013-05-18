#include "libraryInterface.hpp"
#include <algorithm>

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

	void FormatObject::setFormat(const __ReadOnly_HdlTextureFormat& cpy)
	{
		HdlTextureFormat::operator=(cpy);
	}

// ConnectToInputHub
	ConnectToInputHub::ConnectToInputHub(QWidget* parent)
	 : QMenu("Connect to input", parent)
	{
		clearHub();

		QObject::connect(&mapper, SIGNAL(mapped(int)), this, SIGNAL(connectToInput(int))); 
	}

	ConnectToInputHub::~ConnectToInputHub(void)
	{
		clear();
	}

	void ConnectToInputHub::clearHub(void)
	{
		clear();
		currentActions.clear();
		addAction("No available input port");
	}

	void ConnectToInputHub::update(const __ReadOnly_PipelineLayout& layout)
	{
		clear();

		for(int k=0; k<layout.getNumInputPort(); k++)
		{
			QAction* action = addAction(layout.getInputPortName(k).c_str());
			currentActions.push_back(action);
			mapper.setMapping(action, k);
			QObject::connect(action, SIGNAL(triggered(bool)), &mapper, SLOT(map(void))); 
		}
	}


// LibraryInterface :
	LibraryInterface::LibraryInterface(QWidget *parent)
	 : QWidget(parent), menuBar(this), layout(this), tabs(this), ressourceTab(this), uniformsTab(this), connectionMenu(this),
	   openAct(tr("&Load Image"), this), freeImageAct(tr("&Free Image"), this), saveAct(tr("Save Output"), this), saveAsAct(tr("Save Output as"), this),
	   currentOutputLnk(NULL), mainPipeline(NULL), lastUsedPipelineOutput(-1), lastComputeSucceeded(false)
	{
		// Layout : 
		openAct.setShortcuts(QKeySequence::Open);
		openAct.setStatusTip(tr("Load image ..."));
		QObject::connect(&openAct, SIGNAL(triggered()), this, SLOT(loadImage(void)));

		freeImageAct.setStatusTip(tr("Free image from memory..."));
		QObject::connect(&freeImageAct, SIGNAL(triggered()), this, SLOT(freeImage(void)));

		//saveAct.setShortcuts(QKeySequence::Save);
		saveAct.setStatusTip(tr("Save output ..."));
		//QObject::connect(&saveAct, SIGNAL(triggered()), this, SLOT(save()));

		//saveAsAct.setShortcuts(QKeySequence::SaveAs);
		saveAsAct.setStatusTip(tr("Save output to ..."));
		//QObject::connect(&saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

		menuBar.addAction(&openAct);		
		menuBar.addMenu(&connectionMenu);
		menuBar.addAction(&saveAct);
		menuBar.addAction(&saveAsAct);
		menuBar.addAction(&freeImageAct);

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
			listLabels.push_back("Mode/Depth");
			listLabels.push_back("Min/Mag");
			listLabels.push_back("SWarp/TWarp");
			listLabels.push_back("Mipmap");
			ressourceTab.setHeaderLabels( listLabels );

			ressourceTab.addTopLevelItem(new QTreeWidgetItem(RessourceImages));
			ressourceTab.topLevelItem(RessourceImages)->setText(0, "Images (0)");
			ressourceTab.topLevelItem(RessourceImages)->setData(0, Qt::UserRole, QVariant(-1));
			ressourceTab.topLevelItem(RessourceImages)->setFont(0, QFont("", 12));			
			
			ressourceTab.addTopLevelItem(new QTreeWidgetItem(RessourceFormats));
			ressourceTab.topLevelItem(RessourceFormats)->setText(0, "Formats (0)");
			ressourceTab.topLevelItem(RessourceFormats)->setData(0, Qt::UserRole, QVariant(-1));
			ressourceTab.topLevelItem(RessourceFormats)->setFont(0, QFont("", 12));

			ressourceTab.addTopLevelItem(new QTreeWidgetItem(RessourceInputs));
			ressourceTab.topLevelItem(RessourceInputs)->setText(0, "Inputs (0)");
			ressourceTab.topLevelItem(RessourceInputs)->setData(0, Qt::UserRole, QVariant(-1));
			ressourceTab.topLevelItem(RessourceInputs)->setFont(0, QFont("", 12));

			ressourceTab.addTopLevelItem(new QTreeWidgetItem(RessourceOutputs));
			ressourceTab.topLevelItem(RessourceOutputs)->setText(0, "Outputs (0)");
			ressourceTab.topLevelItem(RessourceOutputs)->setData(0, Qt::UserRole, QVariant(-1));
			ressourceTab.topLevelItem(RessourceOutputs)->setFont(0, QFont("", 12));
			

			QObject::connect(&ressourceTab, 	SIGNAL(itemClicked(QTreeWidgetItem*, int)), 	this, SLOT(imageSelected(QTreeWidgetItem*, int)));
			QObject::connect(&ressourceTab,		SIGNAL(itemActivated(QTreeWidgetItem*, int)),	this, SLOT(imageSelected(QTreeWidgetItem*, int)));
			//QObject::connect(&ressourceTab, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(imageSelected(QTreeWidgetItem*, int)));

		// Compilation : 
			cleanCompilationTab(true);
			compilationTab.setAlternatingRowColors(true);

			QFont font;
			font.setFamily("Monospace");
			font.setFixedPitch(true);
			compilationTab.setFont(font);

		// Connections : 
			QObject::connect(&connectionMenu, SIGNAL(connectToInput(int)),	this, SLOT(updateConnection(int)));
	}

	LibraryInterface::~LibraryInterface(void)
	{
		delete mainPipeline;

		for(int k=0; k<textures.size(); k++)
			delete textures[k];
		textures.clear();
	}

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

	void LibraryInterface::cleanCompilationTab(bool writeNoPipeline)
	{
		while(compilationTab.count()>0)
		{
			QListWidgetItem* item = compilationTab.takeItem(0);
			delete item;
		}

		if(writeNoPipeline)
		{
			compilationTab.addItem("No Pipeline...");
			compilationTab.item(0)->setFont(QFont("", -1, -1, true));
		}
	}
		
	void LibraryInterface::compilationSucceeded(void)
	{
		cleanCompilationTab(false);

		// Add OK message : 
		compilationTab.addItem("Compilation succeeded...");
		compilationTab.item(0)->setFont(QFont("", -1, -1, true));
	}

	void LibraryInterface::compilationFailed(Exception& e)
	{
		cleanCompilationTab(false);

		// Add errors : 
		std::string line;
		std::istringstream stream(e.what());
		while( std::getline(stream, line) )
			compilationTab.addItem( line.c_str() );

		// Switch to error tab
		tabs.setCurrentWidget(&compilationTab);
	}

	

	bool LibraryInterface::disconnectLinkFromPipelineOutput(void)
	{
		if(mainPipeline!=NULL && currentOutputLnk!=NULL)
		{
			for(int k=0; k<mainPipeline->getNumOutputPort(); k++)
			{
				if(currentOutputLnk==&mainPipeline->out(k))
				{
					currentOutputLnk = NULL;
					return true;
				}	
			}
		}

		return false;
	}

	void LibraryInterface::removeImage(int id)
	{
		QTreeWidgetItem* 	item = ressourceTab.topLevelItem(RessourceImages)->takeChild(id);
		int 			idi = item->data(0, Qt::UserRole).toInt();

		if(idi<0)
			return ;

		// Get the ressource : 
		TextureObject* 		obj = textures[idi];

		// Remove it from the first list : 
		textures[idi] = NULL;

		// Find if it is used as input : 
		bool inputConnectionChanged = false;
		for(int k=0; k<preferredConnections.size(); k++)
		{
			if(preferredConnections[k]==obj)
			{
				inputConnectionChanged = true;
				preferredConnections[k] = NULL;
			}
		}

		// Test if the link is for this image : 
		if(currentOutputLnk==&obj->texture())
			currentOutputLnk = NULL;

		// Update display : 
		if(inputConnectionChanged)
		{
			updateRessourceAlternateColors(ressourceTab.topLevelItem(RessourceImages));
			updateInputConnectionDisplay();
			compute();
		}

		delete item;
		delete obj;

		emit requireRedraw();
	}

	void LibraryInterface::updateRessourceAlternateColors(QTreeWidgetItem* root)
	{
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
	

	void LibraryInterface::imageSelected(QTreeWidgetItem* item, int column)
	{
		int 	type = item->type(),
			id = item->data(0, Qt::UserRole).toInt();

		switch(type)
		{
			case RessourceImages :
				if(id>=0)
				{
					if(textures[id]!=NULL)
					{
						currentOutputLnk = &textures[id]->texture();
						emit requireRedraw();
					}
					else
						throw Exception("LibraryInterface::imageSelected - Internal error : Item does not correspond to any ressource.", __FILE__, __LINE__);
				}
				break;
			case RessourceFormats :
				break;
			case RessourceInputs :
				if(id>=0)
				{
					if(preferredConnections[id]!=NULL)
					{
						currentOutputLnk = &preferredConnections[id]->texture();
						emit requireRedraw();
					}
				}
				break;
			case RessourceOutputs :
				if(id>=0 && mainPipeline!=NULL && lastComputeSucceeded)
				{
					if(id<mainPipeline->getNumOutputPort())
					{
						currentOutputLnk = &mainPipeline->out(id);
						lastUsedPipelineOutput = id;
						emit requireRedraw();
					}
				}
				break;
			default :
				throw Exception("LibraryInterface::imageSelected - Internal error : unknown scalar type for QTreeWidgetItem.", __FILE__, __LINE__);
		}

		
	}

	void LibraryInterface::loadImage(void)
	{
		bool oneSucceeded = false;
		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load an Image"), ".", tr("Image (*.bmp *.png *.jpg"));
		
		for(int k=0; k<filenames.count(); k++)
		{
			if(!filenames[k].isEmpty())
			{
				TextureObject* obj = new TextureObject(filenames[k]);

				if(obj->isValid())
				{
					// Add the texture object to the list : 
					textures.push_back(obj);

					oneSucceeded = true;
				}
				else
				{
					delete obj;
					obj = NULL;
				}
			}
		}

		// If at least one item was loaded, show it : 
		if(oneSucceeded)	
		{
			// Update designs : 
			updateImageListDisplay();
			updateFormatListDisplay();
	
			// Expand : 
			ressourceTab.topLevelItem(RessourceImages)->setExpanded(true);
		}
	}

	void LibraryInterface::updateConnection(int idInput)
	{
		// Get the current selected items :
		QList<QTreeWidgetItem *> selectedItems = ressourceTab.selectedItems();

		if(selectedItems.count()==1)
		{
			QTreeWidgetItem* item = selectedItems.front();

			int id = item->data(0, Qt::UserRole).toInt();

			if(item->type()==RessourceImages && id>=0)
			{
				if(textures[id]!=NULL)
				{
					preferredConnections[idInput] = textures[id];
					updateInputConnectionDisplay();
					compute();
				}
				else
					throw Exception("LibraryInterface::updateConnection - Internal error : Item does not correspond to any ressource.", __FILE__, __LINE__);
			}
		}
	}

	void LibraryInterface::compute(bool forcePipelineOutput)
	{
		if(mainPipeline!=NULL)
		{
			lastComputeSucceeded = false;

			// Check that all of the input have a connection : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(preferredConnections[k]==NULL)
				{
					disconnectLinkFromPipelineOutput();
					return;
				}
			}	

			// Apply : 
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
				(*mainPipeline) << preferredConnections[k]->texture();

			(*mainPipeline) << Pipeline::Process;

			lastComputeSucceeded = true;

			if(forcePipelineOutput && lastUsedPipelineOutput>=0 && lastUsedPipelineOutput<mainPipeline->getNumOutputPort())
				currentOutputLnk = &mainPipeline->out(lastUsedPipelineOutput);

			// Need redraw : 
			emit requireRedraw();
		}
	}

	void LibraryInterface::freeImage(void)
	{
		// Get the current selected items :
		QList<QTreeWidgetItem *> selectedItems = ressourceTab.selectedItems();

		if(selectedItems.count()==1)
		{
			QTreeWidgetItem* item = selectedItems.front();
			
			int id = item->data(0, Qt::UserRole).toInt();

			if(item->type()==RessourceImages && id>=0)
				removeImage(ressourceTab.topLevelItem(RessourceImages)->indexOfChild(item));
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
		return currentOutputLnk!=NULL;
	}

	HdlTexture& LibraryInterface::currentOutput(void)
	{
		if(currentOutputLnk!=NULL)
			return *currentOutputLnk;
		else
			throw Exception("LibraryInterface::currentOutput - Internal error : no currently associated texture.", __FILE__, __LINE__);
	}

	void LibraryInterface::compile(const std::string& code)
	{
		bool success = true;
		
		disconnectLinkFromPipelineOutput();

		try
		{
			delete mainPipeline;
			mainPipeline = NULL;

			disconnectLinkFromPipelineOutput();

			// Put the formats : 
			pipelineLoader.clearRequiredElements();
			
			for(int k=0; k<formats.size(); k++)
				pipelineLoader.addRequiredElement(formats[k].getName().toStdString(), formats[k]);

			// Load : 
			mainPipeline = pipelineLoader(code, "MainPipeline");
		}
		catch(Exception& e)
		{
			success = false;

			// Info : 
			compilationFailed(e);

			delete mainPipeline;
			mainPipeline = NULL;
		}

		if(success)
		{
			// Make sure we can track the inputs : 
			while(preferredConnections.size()<mainPipeline->getNumInputPort())
				preferredConnections.push_back(NULL);
			
			compilationSucceeded();

			updateInputConnectionDisplay();

			// Expand : 
			ressourceTab.topLevelItem(RessourceInputs)->setExpanded(true);

			// Update output list : 
			updateOutputConnectionDisplay();

			// Expand : 
			ressourceTab.topLevelItem(RessourceOutputs)->setExpanded(true);

			// Update the connection menu : 
			connectionMenu.update(*mainPipeline);

			// Check the compute : 
			compute(true);
		}
		else
		{
			// Clean outputs and inputs :
			updateInputConnectionDisplay();
			updateOutputConnectionDisplay();

			connectionMenu.clearHub();

			emit requireRedraw();
		}
	}

