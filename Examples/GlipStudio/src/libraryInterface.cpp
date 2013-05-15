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
		// Init : 
		for(int k=0; k<RessourceNumber; k++)
			ressourcesHeaders[k] = NULL;
		
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
			listLabels.push_back("Min/Mag");
			listLabels.push_back("SWarp/TWarp");
			listLabels.push_back("Mipmap");
			ressourceTab.setHeaderLabels( listLabels );

			ressourcesHeaders[RessourceImages] = new QTreeWidgetItem(RessourceImages);
			ressourcesHeaders[RessourceImages]->setText(0, "Images (0)");
			ressourcesHeaders[RessourceImages]->setData(0, Qt::UserRole, QVariant(-1));
			ressourcesHeaders[RessourceImages]->setFont(0, QFont("", 12));
			ressourceTab.addTopLevelItem(ressourcesHeaders[RessourceImages]);			
			
			ressourcesHeaders[RessourceFormats] = new QTreeWidgetItem(RessourceFormats);
			ressourcesHeaders[RessourceFormats]->setText(0, "Formats (0)");
			ressourcesHeaders[RessourceFormats]->setData(0, Qt::UserRole, QVariant(-1));
			ressourcesHeaders[RessourceFormats]->setFont(0, QFont("", 12));
			ressourceTab.addTopLevelItem(ressourcesHeaders[RessourceFormats]);

			ressourcesHeaders[RessourceInputs] = new QTreeWidgetItem(RessourceInputs);
			ressourcesHeaders[RessourceInputs]->setText(0, "Inputs (0)");
			ressourcesHeaders[RessourceInputs]->setData(0, Qt::UserRole, QVariant(-1));
			ressourcesHeaders[RessourceInputs]->setFont(0, QFont("", 12));
			ressourceTab.addTopLevelItem(ressourcesHeaders[RessourceInputs]);

			ressourcesHeaders[RessourceOutputs] = new QTreeWidgetItem(RessourceOutputs);
			ressourcesHeaders[RessourceOutputs]->setText(0, "Outputs (0)");
			ressourcesHeaders[RessourceOutputs]->setData(0, Qt::UserRole, QVariant(-1));
			ressourcesHeaders[RessourceOutputs]->setFont(0, QFont("", 12));
			ressourceTab.addTopLevelItem(ressourcesHeaders[RessourceOutputs]);
			

			QObject::connect(&ressourceTab, 	SIGNAL(itemClicked(QTreeWidgetItem*, int)), 	this, SLOT(imageSelected(QTreeWidgetItem*, int)));
			QObject::connect(&ressourceTab,		SIGNAL(itemActivated(QTreeWidgetItem*, int)),	this, SLOT(imageSelected(QTreeWidgetItem*, int)));
			//QObject::connect(&ressourceTab, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(imageSelected(QTreeWidgetItem*, int)));

		// Compilation : 
			cleanCompilationTab(true);
			compilationTab.setAlternatingRowColors(true);

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

	void LibraryInterface::appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture)
	{
		size_t s = texture.getSizeOnGPU();
		QString sizeStr;

		if(s>1024*1024)
			sizeStr = tr("%1 MB").arg(s/(1024*1024));
		else
			sizeStr = tr("%1 KB").arg(s/(1024));

		item->setText(1, tr("%1 x %2 (%3)").arg(texture.getWidth()).arg(texture.getHeight()).arg(sizeStr));
		item->setText(2, tr("%1 / %2").arg(glParamName(texture.getMinFilter()).c_str()).arg(glParamName(texture.getMagFilter()).c_str()));
		item->setText(3, tr("%1 / %2").arg(glParamName(texture.getSWrapping()).c_str()).arg(glParamName(texture.getTWrapping()).c_str()));
		item->setText(4, tr("%1").arg(texture.getMaxLevel()));
	}

	void LibraryInterface::updateFormatList(void)
	{
		
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

	void LibraryInterface::updateInputConnectionDisplay(void)
	{
		if(mainPipeline!=NULL)
		{
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				if(preferredConnections[k]==NULL)
					ressourcesHeaders[RessourceInputs]->child(k)->setText(0, tr("    %1 (Not Connected)").arg(mainPipeline->getInputPortName(k).c_str()));
				else
				{
					ressourcesHeaders[RessourceInputs]->child(k)->setText(0, tr("    %1 <- %2").arg(mainPipeline->getInputPortName(k).c_str()).arg(preferredConnections[k]->getName()));
					appendTextureInformation(ressourcesHeaders[RessourceInputs]->child(k), preferredConnections[k]->texture());
				}
			}
		}
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
		QTreeWidgetItem* 	item = ressourcesHeaders[RessourceImages]->takeChild(id);
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
			updateRessourceAlternateColors(ressourcesHeaders[RessourceImages]);
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
		QTreeWidgetItem* lastItem = NULL;
		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Load an Image"), ".", tr("Image (*.bmp *.png *.jpg"));
		
		for(int k=0; k<filenames.count(); k++)
		{
			if(!filenames[k].isEmpty())
			{
				TextureObject* obj = new TextureObject(filenames[k]);

				if(obj->isValid())
				{
					// Write infos : 
					QTreeWidgetItem* item = new QTreeWidgetItem(RessourceImages);

					item->setText(0, tr("    %1").arg(obj->getName()));

					appendTextureInformation(item, obj->texture());

					// Set the link to the data : 
					item->setData(0, Qt::UserRole, QVariant(static_cast<int>(textures.size())));

					ressourcesHeaders[RessourceImages]->addChild(item);

					// Add the texture object to the list : 
					textures.push_back(obj);

					// Update the title : 
					ressourcesHeaders[RessourceImages]->setText(0, tr("Images (%1)").arg(getNumImages()));

					// Update design : 
					updateFormatList();
					updateRessourceAlternateColors(ressourcesHeaders[RessourceImages]);

					// Set as last item : 
					lastItem = item;
				}
				else
				{
					delete obj;
					obj = NULL;
				}
			}
		}

		// If at least one item was loaded, show it : 
		if(lastItem!=NULL)	
		{
			// Expand : 
			ressourcesHeaders[RessourceImages]->setExpanded(true);

			// Unselect all the previous children : 
			QList<QTreeWidgetItem*> selectedChildren = ressourceTab.selectedItems();

			for(int k=0; k<selectedChildren.count(); k++)
				selectedChildren[k]->setSelected(false);

			lastItem->setSelected(true);

			// Show image and require update : 
			currentOutputLnk = &textures[ lastItem->data(0, Qt::UserRole).toInt() ]->texture();
			emit requireRedraw();
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
				removeImage(id);
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
			mainPipeline = pipelineLoader(code, "MainPipeline");
		}
		catch(Exception& e)
		{
			success = false;

			// Info : 
			compilationFailed(e);

			//std::cerr << e.what() << std::endl;
			delete mainPipeline;
			mainPipeline = NULL;

			ressourcesHeaders[RessourceInputs]->setText(0, tr("Inputs (0)"));
			ressourcesHeaders[RessourceOutputs]->setText(0, tr("Outputs (0)"));
		}

		if(success)
		{
			// Make sure we can track the inputs : 
			while(preferredConnections.size()<mainPipeline->getNumInputPort())
				preferredConnections.push_back(NULL);
			
			compilationSucceeded();

			// clean input list : 
			QTreeWidgetItem* inputsList = ressourcesHeaders[RessourceInputs];
			removeAllChildren(inputsList);

			// Update input list :
			for(int k=0; k<mainPipeline->getNumInputPort(); k++)
			{
				// Append new child : 
				QTreeWidgetItem* item = new QTreeWidgetItem(RessourceInputs);

				// Set the link to the data : 
				item->setData(0, Qt::UserRole, QVariant(k));
			
				// Push : 
				inputsList->addChild(item);
			}

			// Update the infos : 
			updateInputConnectionDisplay();

			// Update the title : 
			ressourcesHeaders[RessourceInputs]->setText(0, tr("Inputs (%1)").arg(mainPipeline->getNumInputPort()));
		
			// Update the colors : 
			updateRessourceAlternateColors(ressourcesHeaders[RessourceInputs]);

			// Expand : 
			ressourcesHeaders[RessourceInputs]->setExpanded(true);

			// Update output list : 
			QTreeWidgetItem* outputsList = ressourcesHeaders[RessourceOutputs];
			removeAllChildren(outputsList);

			// Update output list :
			for(int k=0; k<mainPipeline->getNumOutputPort(); k++)
			{
				// Append new child : 
				QTreeWidgetItem* item = new QTreeWidgetItem(RessourceOutputs);

				// Set the link to the data : 
				item->setData(0, Qt::UserRole, QVariant(k));

				// Add information : 
				item->setText(0, tr("    %1").arg(mainPipeline->getOutputPortName(k).c_str()));
				appendTextureInformation(item, mainPipeline->out(k));
			
				// Push : 
				outputsList->addChild(item);
			}

			// Update the title : 
			ressourcesHeaders[RessourceOutputs]->setText(0, tr("Outputs (%1)").arg(mainPipeline->getNumOutputPort()));

			// Update the colors : 
			updateRessourceAlternateColors(ressourcesHeaders[RessourceOutputs]);

			// Expand : 
			ressourcesHeaders[RessourceOutputs]->setExpanded(true);

			// Update the connection menu : 
			connectionMenu.update(*mainPipeline);

			// Check the compute : 
			compute(true);
		}
		else
		{
			// Clean outputs and inputs :
			QTreeWidgetItem* inputsList = ressourcesHeaders[RessourceInputs];
			removeAllChildren(inputsList);
			inputsList->setExpanded(false);

			QTreeWidgetItem* outputsList = ressourcesHeaders[RessourceOutputs];
			removeAllChildren(outputsList);
			outputsList->setExpanded(false);

			connectionMenu.clearHub();

			emit requireRedraw();
		}
	}

