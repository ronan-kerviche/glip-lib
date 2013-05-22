#include "libraryInterface.hpp"
#include <algorithm>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

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
	 : QWidget(parent), /*menuBar(this),*/ layout(this), tabs(this), ressourceTab(this), uniformsTab(this), connectionMenu(this),
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

		/*menuBar.addAction(&openAct);		
		menuBar.addMenu(&connectionMenu);
		menuBar.addAction(&saveAct);
		menuBar.addAction(&saveAsAct);
		menuBar.addAction(&freeImageAct);*/

		tabs.addTab(&ressourceTab, 	"Ressources");
		tabs.addTab(&compilationTab,	"Compilation");
		tabs.addTab(&uniformsTab, 	"Uniforms");

		//layout.addWidget(&menuBar);
		layout.addWidget(&tabs);

		// Ressources : 
			/*ressourceTab.setIndentation(1);

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
			//QObject::connect(&ressourceTab, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(imageSelected(QTreeWidgetItem*, int)));*/

		// Compilation : 
			cleanCompilationTab(true);
			compilationTab.setAlternatingRowColors(true);

			QFont font;
			font.setFamily("Monospace");
			font.setFixedPitch(true);
			compilationTab.setFont(font);

		// Context menu :
			//ressourceTab.setContextMenuPolicy(Qt::CustomContextMenu);
			//QObject::connect(&ressourceTab, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

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
		e.hideHeader(true);
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
		/*QTreeWidgetItem* 	item = ressourceTab.topLevelItem(RessourceImages)->takeChild(id);
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

		emit requireRedraw();*/
	}

	/*void LibraryInterface::updateRessourceAlternateColors(QTreeWidgetItem* root)
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
	}*/

	void LibraryInterface::imageSelected(QTreeWidgetItem* item, int column)
	{
		/*int 	type = item->type(),
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
		}*/
	}

	void LibraryInterface::loadImage(void)
	{
		/*bool oneSucceeded = false;
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
		}*/
	}

	void LibraryInterface::updateConnection(int idInput)
	{
		/*// Get the current selected items :
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
		}*/
	}

	void LibraryInterface::compute(bool forcePipelineOutput)
	{
		/*if(mainPipeline!=NULL)
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
		}*/
	}

	void LibraryInterface::freeImage(void)
	{
		/*// Get the current selected items :
		QList<QTreeWidgetItem *> selectedItems = ressourceTab.selectedItems();

		if(selectedItems.count()==1)
		{
			QTreeWidgetItem* item = selectedItems.front();
			
			int id = item->data(0, Qt::UserRole).toInt();

			if(item->type()==RessourceImages && id>=0)
				removeImage(ressourceTab.topLevelItem(RessourceImages)->indexOfChild(item));
		}*/
	}

	int LibraryInterface::getNumImages(void) const
	{
		/*int res = 0;

		for(int k=0; k<textures.size(); k++)
		{
			if(textures[k]!=NULL)
				res++;
		}

		return res;*/

		return 0;
	}

	bool LibraryInterface::hasOutput(void) const
	{
		//return currentOutputLnk!=NULL;
		return false;
	}

	HdlTexture& LibraryInterface::currentOutput(void)
	{
		if(currentOutputLnk!=NULL)
			return *currentOutputLnk;
		else
			throw Exception("LibraryInterface::currentOutput - Internal error : no currently associated texture.", __FILE__, __LINE__);
	}

	void LibraryInterface::compile(const std::string& code, const std::string& path)
	{
		/*if(code.empty())
			return ; //Nothing to do...

		bool success = true;
		
		disconnectLinkFromPipelineOutput();

		pipelineLoader.setPath(path);

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
		}*/
	}

	void LibraryInterface::showContextMenu(const QPoint& pos)
	{
		/*
		// Get the global position : 
		QPoint globalPos = ressourceTab.viewport()->mapToGlobal(pos); //ressourceTab.mapToGlobal(pos);

		// Get the item under the right click : 
		QTreeWidgetItem* item =  ressourceTab.itemAt(pos);

		if(item!=NULL)
		{
			if(item->type()!=RessourceImages || item->data(0, Qt::UserRole).toInt()<0)
				return ; // Not an image

			// Get the corresponding object : 
			TextureObject* obj = textures[item->data(0, Qt::UserRole).toInt()];

			// Change the menu according to the column : 
			int c = ressourceTab.currentColumn();

			if(c==0) // Name 
			{
				QMenu menu;

				// Create connection menu : 
				if(mainPipeline!=NULL)
				{
					for(int k=0; k<mainPipeline->getNumInputPort(); k++)
						menu.addAction( tr("Connect to %1").arg(mainPipeline->getInputPortName(k).c_str()) )->setData( QVariant(k) );

					// Separator : 
					menu.addSeparator();
				} 

				// Free image : 
				menu.addAction("Free image")->setData( QVariant(-1) );

				menu.exec(globalPos);
			}
			else if(c==3) // Min/Mag 
			{
				QMenu menu(&ressourceTab);

				menu.addAction("NEAREST")->setData( QVariant( QPoint(GL_NEAREST, GL_NEAREST) ) );
				menu.addAction("LINEAR")->setData( QVariant( QPoint(GL_LINEAR, GL_LINEAR) ) );

				QAction* selectedItem = menu.exec(globalPos);

				if(selectedItem!=NULL)
				{
					obj->texture().setMinFilter( selectedItem->data().toPoint().x() );
					obj->texture().setMagFilter( selectedItem->data().toPoint().y() );
					updateImageListDisplay();
					emit requireRedraw();
				}
			}
			else if(c==4) // Warp
			{
				QMenu menu;

				menu.addAction("CLAMP")->setData( QVariant( QPoint(GL_CLAMP, GL_CLAMP) ) );
				menu.addAction("CLAMP_TO_EDGE")->setData( QVariant( QPoint(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) ) );
				menu.addAction("REPEAT")->setData( QVariant( QPoint(GL_REPEAT, GL_REPEAT) ) );

				QAction* selectedItem = menu.exec(globalPos);

				if(selectedItem!=NULL)
				{
					obj->texture().setSWrapping( selectedItem->data().toPoint().x() );
					obj->texture().setTWrapping( selectedItem->data().toPoint().y() );
					updateImageListDisplay();
					emit requireRedraw();
				}
			}
			else if(c==5)
			{

			}

			//std::cout << item->data(0, Qt::UserRole).toInt() << std::endl;
			//if(item->data(0, Qt::UserRole).toInt()>=0)
			//	std::cout << textures[item->data(0, Qt::UserRole).toInt()]->getName().toStdString() << std::endl;
		}*/

		/*QMenu myMenu;
		myMenu.addAction("Menu Item 1");

		QAction* selectedItem = myMenu.exec(globalPos);
		if(selectedItem!=NULL)
		{
			std::cout << "Something" << std::endl;
			// something was chosen, do stuff
		}
		else
		{
			std::cout << "Nothing" << std::endl;
			// nothing was chosen
		}*/
	}


