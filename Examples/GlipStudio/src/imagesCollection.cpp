#include "imagesCollection.hpp"
#include "settingsManager.hpp"

// TextureStatus
	TextureStatus::TextureStatus(void)
	 : 	blank(true),
		portID(-1),
		connectionStatus(NotConnected),
		lockedToDevice(false),
		type(NoType),
		location(NoMemLoc)
	{ }

	TextureStatus::TextureStatus(const TextureStatus& c)
	 : 	blank(c.blank),
		portID(c.portID),
		connectionStatus(c.connectionStatus),
		lockedToDevice(c.lockedToDevice),
		type(c.type),
		location(c.location)
	{ }

	TextureStatus::TextureStatus(Type _type)
	 : 	blank(false),
		type(_type),
		portID(-1),
		connectionStatus(NotConnected),
		lockedToDevice(false),
		location(NoMemLoc)
	{ }

	bool TextureStatus::isBlank(void) const
	{
		return blank;
	}

	TextureStatus::Type TextureStatus::getType(void) const
	{
		return type;
	}

	const TextureStatus& TextureStatus::operator=(const TextureStatus& c)
	{
		blank			= c.blank;
		portID			= c.portID;
		connectionStatus	= c.connectionStatus;
		lockedToDevice		= c.lockedToDevice;
		type			= c.type;
		location		= c.location;

		return (*this);
	}

// TexturesList
	TexturesList::TexturesList(QWidget* parent)
	 : QTreeWidget(parent)
	{
		setIndentation(2);
		setSelectionMode(QAbstractItemView::ExtendedSelection);
		setContextMenuPolicy(Qt::CustomContextMenu);

		QStringList listLabels;
		listLabels.push_back("Status");
		listLabels.push_back("Connection");
		listLabels.push_back("Name");
		listLabels.push_back("Size");
		setHeaderLabels( listLabels );

		header()->setMovable( false );

		connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),	this, SLOT(itemChangedReceiver(QTreeWidgetItem*)));
		connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),			this, SLOT(itemChangedReceiver(QTreeWidgetItem*)));

		// size of the columns : 
		#if QT_VERSION >= 0x050000
			resizeColumnToContents(0);
			resizeColumnToContents(1);
			header()->setSectionResizeMode(0, QHeaderView::Fixed); 
			header()->setSectionResizeMode(1, QHeaderView::Stretch);
			header()->setSectionResizeMode(2, QHeaderView::Fixed);
		#else
			resizeColumnToContents(0);
			resizeColumnToContents(1);
			header()->setResizeMode(0, QHeaderView::Fixed); 
			header()->setResizeMode(1, QHeaderView::Fixed);
			header()->setResizeMode(2, QHeaderView::Stretch);
			header()->setResizeMode(3, QHeaderView::Fixed);
			
			//header()->setResizeMode(0, QHeaderView::Fixed);
			//header()->resizeSection(1, 400);
			//header()->setResizeMode(1, QHeaderView::Fixed);
			//header()->setStretchLastSection(true);
		#endif
	}

	TexturesList::~TexturesList(void)
	{
		blockSignals(true);
	}

	int TexturesList::getIndexFromRecordID(int recordID) const
	{
		std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

		if(it==recordIDs.end())
			return -1;
		else
			return std::distance(recordIDs.begin(), it);
	}

	void TexturesList::updateAlternateColors(void)
	{
		QBrush 	//foreground	= palette().foreground().color(),
			original 	= palette().background().color(),
			darker		= QBrush(original.color().lighter(90)),
			lighter		= QBrush(original.color().lighter(110));
		
		QBrush* ptr = NULL;
		for(int k=0; k<topLevelItemCount(); k++)
		{
			if(k%2==0)
				ptr = &lighter;
			else
				ptr = &darker;

			for(int l=0; l<columnCount(); l++)
			{
				//topLevelItem(k)->setForeground(l, QBrush(foreground)); // needed?
				topLevelItem(k)->setBackground(l, QBrush(*ptr));
			}
		}
	}

	void TexturesList::itemChangedReceiver(QTreeWidgetItem* item)
	{
		if(item!=NULL)
		{
			// Get the record ID : 
			int recordID = item->data(2, Qt::UserRole).toInt();

			// Send : 
			emit focusChanged(recordID);
		}
	}

	bool TexturesList::recordExists(int recordID, bool throwException) const
	{
		if( getIndexFromRecordID(recordID)<0 )
		{
			if(throwException)
				throw Exception("TexturesList::recordExists - Record #" + to_string(recordID) + " does not exist.", __FILE__, __LINE__);
			else
				return false;
		}
		else
			return true;
	}

	int TexturesList::addRecord(const std::string& name, const TextureStatus& s)
	{
		// Find the largest recordID : 
		std::vector<int>::const_iterator it = std::max_element(recordIDs.begin(), recordIDs.end());

		int newRecordID = 1;
		if(it!=recordIDs.end())
			newRecordID = *it + 1;

		// Add a new record : 
		recordIDs.push_back(newRecordID);
		namesList.push_back(name);
		statusList.push_back(s);
		formatsList.push_back(HdlTextureFormat(1,1,GL_RGB,GL_UNSIGNED_BYTE));
		hasFormatList.push_back(false);

		// Add into the tree : 
		itemsList.push_back( new QTreeWidgetItem(newRecordID) );
		addTopLevelItem( itemsList.back() );	

		// Update the line : 
		updateRecordName(newRecordID, name);
		updateRecordFormat(newRecordID);
		updateRecordStatus(newRecordID, s);

		updateAlternateColors();

		return newRecordID;
	}

	int TexturesList::addRecord(const std::string& name, const HdlTextureFormat& format, const TextureStatus& s)
	{
		int newRecordID = addRecord(name, s);

		hasFormatList.back() = true;
		formatsList.back() = format;
		updateRecordFormat(newRecordID, format);

		return newRecordID;
	}

	const std::string& TexturesList::recordName(int recordID) const
	{
		recordExists(recordID, true);
		return namesList[ getIndexFromRecordID(recordID) ];
	}

	void TexturesList::updateRecordName(int recordID, const std::string& newName)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		namesList[ id ] = newName;
		
		// Update : 
		itemsList[id]->setText(2, newName.c_str());

		// Set the link to the data : 
		itemsList[id]->setData(2, Qt::UserRole, recordID);

		// Color : 
		QBrush 	foreground	= palette().foreground().color(),
			lighter		= QBrush(foreground.color().lighter(80));
		itemsList[id]->setForeground(2, lighter );
	}

	const HdlTextureFormat& TexturesList::recordFormat(int recordID) const
	{
		recordExists(recordID, true);
		return formatsList[ getIndexFromRecordID(recordID) ];
	}

	void TexturesList::updateRecordFormat(int recordID)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		hasFormatList[ id ] = false;

		itemsList[id]->setText(3, "N.A.");
	
		// Color : 
		QBrush 	foreground	= palette().foreground().color(),
			lighter		= QBrush(foreground.color().lighter(70));
		itemsList[id]->setForeground(3, lighter );
	}

	void TexturesList::updateRecordFormat(int recordID, const HdlTextureFormat& newFormat)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		formatsList[ id ] = newFormat;
		hasFormatList[ id ] = true;

		// Make up the format chain : 
		QString formatString;

		const HdlTextureFormatDescriptor& descriptor = newFormat.getFormatDescriptor();

		// Mode : 
		if( descriptor.hasRedLayer )
			formatString.append("R");
		if( descriptor.hasGreenLayer )
			formatString.append("G");
		if( descriptor.hasBlueLayer )
			formatString.append("B");
		if( descriptor.hasAlphaLayer )
			formatString.append("A");
		if( descriptor.hasLuminanceLayer )
			formatString.append("L");

		// Depth : 
		formatString.append( tr(" %1 bits").arg(newFormat.getChannelDepth()*8) );

		// Get the size : 
		QString sizeStr;

		size_t s = newFormat.getSize();
		if(s>1024*1024)
			sizeStr = tr("%1 MB").arg(s/(1024*1024));
		else
			sizeStr = tr("%1 KB").arg(s/(1024));

		// Print : 
		itemsList[id]->setText(3, tr("%1 x %2 (%3; %4)").arg(newFormat.getWidth()).arg(newFormat.getHeight()).arg(sizeStr).arg(formatString));

		// Color : 
		QBrush 	foreground	= palette().foreground().color(),
			lighter		= QBrush(foreground.color().lighter(80));
		itemsList[id]->setForeground(3, lighter );
	}

	const TextureStatus& TexturesList::recordStatus(int recordID)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		return statusList[id];
	}
	
	void TexturesList::updateRecordStatus(int recordID, const TextureStatus& s)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		statusList[id] = s;

		//Set the status string : 
			QString message;
			QBrush brush;

			if( s.location==TextureStatus::NotLoaded )
			{
				message = "DISK";
				brush = QBrush(Qt::darkRed);
			}
			else if( s.location==TextureStatus::OnRAM )
			{
				message = "RAM";
				brush = QBrush(QColor(255, 128, 0));
			}
			else if( s.location==TextureStatus::OnVRAM )
			{
				message = "VRAM";
				brush = QBrush(Qt::green);
			}
			else if( s.location==TextureStatus::VirtualLink)
			{
				message = "LINK";
				brush = QBrush(Qt::lightGray);
			}
			else
				throw Exception("TexturesList::updateStatus - Unknown Status code : " + to_string(s.location) + ".", __FILE__, __LINE__);

			// Set the data in the tree : 
			itemsList[id]->setText(0, message);
			itemsList[id]->setTextAlignment(0, Qt::AlignHCenter);
			itemsList[id]->setForeground(0, brush);

		// Set the connection string : 
			message.clear();

			if( s.getType()==TextureStatus::Resource )
			{
				if( s.connectionStatus==TextureStatus::Connected )
				{
					message = tr("In_%1").arg( s.portID );
					brush = QBrush(Qt::green);
				}
				else if( s.connectionStatus==TextureStatus::WaitingLink )
				{
					message = tr("In_%1").arg( s.portID );
					brush = QBrush(Qt::darkGray);
				}
			}
			else if( s.getType()==TextureStatus::InputPort )
			{
				message = tr("In_%1").arg( s.portID );

				if( s.connectionStatus==TextureStatus::Connected )
					brush = QBrush(Qt::green);
				else
					brush = QBrush(Qt::red);
			}
			else if( s.getType()==TextureStatus::OutputPort )
			{
				message = tr("Out_%1").arg( s.portID );

				if( s.connectionStatus==TextureStatus::Connected )
					brush = QBrush(Qt::green);
				else
					brush = QBrush(Qt::red);
			}
			else
				throw Exception("TexturesList::updateStatus - Unknown Status code : " + to_string(s.getType()) + ".", __FILE__, __LINE__);

			// Set the data in the tree : 
			itemsList[id]->setText(1, message);
			itemsList[id]->setTextAlignment(1, Qt::AlignHCenter);
			itemsList[id]->setForeground(1, brush);
	}

	void TexturesList::removeRecord(int recordID)
	{
		recordExists(recordID, true);

		int id = getIndexFromRecordID(recordID);

		takeTopLevelItem( indexOfTopLevelItem( itemsList[id] ) );
		delete itemsList[id];

		recordIDs.erase( recordIDs.begin() + id );
		itemsList.erase( itemsList.begin() + id );
		namesList.erase( namesList.begin() + id );
		statusList.erase( statusList.begin() + id );
		formatsList.erase( formatsList.begin() + id );

		updateAlternateColors();
	}

	void TexturesList::removeAllRecords(void)
	{
		while(!recordIDs.empty())
			removeRecord(recordIDs.back());
	}

	std::vector<int> TexturesList::getSelectedRecordIDs(void)
	{
		QList<QTreeWidgetItem*> selectedItemsList = selectedItems();
		std::vector<int> selectedRecordIDs;

		for(int k=0; k<selectedItemsList.size(); k++)
			selectedRecordIDs.push_back( selectedItemsList.at(k)->data(2, Qt::UserRole).toInt() );

		return 	selectedRecordIDs;
	}

	int TexturesList::getRecordIDsUnder(const QPoint& pt)
	{
		QTreeWidgetItem* item =	itemAt(pt);
		return item->data(2, Qt::UserRole).toInt();
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
		bothNearest.setCheckable(true);
		bothLinear.setCheckable(true);
		minNearest.setCheckable(true);
		minLinear.setCheckable(true);
		magNearest.setCheckable(true);
		magLinear.setCheckable(true);
		minNearestMipmapNearest.setCheckable(true);
		minNearestMipmapLinear.setCheckable(true);
		minLinerarMipmapNearest.setCheckable(true);
		minLinearMipmapLinear.setCheckable(true);

		clearChecked();

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

	void FilterMenu::clearChecked(void)
	{
		bothNearest.setChecked(false);
		bothLinear.setChecked(false);
		minNearest.setChecked(false);
		minLinear.setChecked(false);
		magNearest.setChecked(false);
		magLinear.setChecked(false);
		minNearestMipmapNearest.setChecked(false);
		minNearestMipmapLinear.setChecked(false);
		minLinerarMipmapNearest.setChecked(false);
		minLinearMipmapLinear.setChecked(false);
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

		clearChecked();
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

		// Update checked : 
		clearChecked();

		if( fmt.getMagFilter()==fmt.getMinFilter() && fmt.getMagFilter()==GL_NEAREST)
			bothNearest.setChecked(true);
		else if( fmt.getMagFilter()==fmt.getMinFilter() && fmt.getMagFilter()==GL_LINEAR)
			bothLinear.setChecked(true);

		switch(fmt.getMinFilter())
		{
			case GL_NEAREST :
				minNearest.setChecked(true);
				break;
			case GL_LINEAR :
				minLinear.setChecked(true);
				break;
			case GL_NEAREST_MIPMAP_NEAREST : 
				minNearestMipmapNearest.setChecked(true);
				break;
			case GL_NEAREST_MIPMAP_LINEAR :
				minNearestMipmapLinear.setChecked(true);
				break;
			case GL_LINEAR_MIPMAP_NEAREST :
				minLinerarMipmapNearest.setChecked(true);
				break;
			case GL_LINEAR_MIPMAP_LINEAR :
				minLinearMipmapLinear.setChecked(true);
				break;
			default : 
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + glParamName(fmt.getSWrapping()) + "\".", __FILE__, __LINE__);
		}

		switch(fmt.getMagFilter())
		{
			case GL_NEAREST :
				magNearest.setChecked(true);
				break;
			case GL_LINEAR :
				magLinear.setChecked(true);
				break;
			default : 
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + glParamName(fmt.getSWrapping()) + "\".", __FILE__, __LINE__);
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
		bothClamp.setCheckable(true);
		bothClampToBorder.setCheckable(true);
		bothClampToEdge.setCheckable(true);
		bothRepeat.setCheckable(true);
		bothMirroredRepeat.setCheckable(true);
		sClamp.setCheckable(true);
		sClampToBorder.setCheckable(true);
		sClampToEdge.setCheckable(true);
		sRepeat.setCheckable(true);
		sMirroredRepeat.setCheckable(true);
		tClamp.setCheckable(true);
		tClampToBorder.setCheckable(true);
		tClampToEdge.setCheckable(true);
		tRepeat.setCheckable(true);
		tMirroredRepeat.setCheckable(true);

		clearChecked();

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

	void WrappingMenu::clearChecked(void)
	{
		bothClamp.setChecked(false);
		bothClampToBorder.setChecked(false);
		bothClampToEdge.setChecked(false);
		bothRepeat.setChecked(false);
		bothMirroredRepeat.setChecked(false);

		sClamp.setChecked(false);
		sClampToBorder.setChecked(false);
		sClampToEdge.setChecked(false);
		sRepeat.setChecked(false);
		sMirroredRepeat.setChecked(false);

		tClamp.setChecked(false);
		tClampToBorder.setChecked(false);
		tClampToEdge.setChecked(false);
		tRepeat.setChecked(false);
		tMirroredRepeat.setChecked(false);
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

		clearChecked();

		if(fmt.getSWrapping()==fmt.getTWrapping())
		{
			switch(fmt.getSWrapping())
			{
				case GL_CLAMP :
					bothClamp.setChecked(true);
					break;
				case GL_CLAMP_TO_BORDER :
					bothClampToBorder.setChecked(true);
					break;
				case GL_CLAMP_TO_EDGE : 
					bothClampToEdge.setChecked(true);
					break;
				case GL_REPEAT :
					bothRepeat.setChecked(true);
					break;
				case GL_MIRRORED_REPEAT :
					bothMirroredRepeat.setChecked(true);
					break;
				default : 
					throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(fmt.getSWrapping()) + "\".", __FILE__, __LINE__);
			}
		}
	
		// Tick boxes : 
		switch(fmt.getSWrapping())
		{
			case GL_CLAMP :
				sClamp.setChecked(true);
				break;
			case GL_CLAMP_TO_BORDER :
				sClampToBorder.setChecked(true);
				break;
			case GL_CLAMP_TO_EDGE : 
				sClampToEdge.setChecked(true);
				break;
			case GL_REPEAT :
				sRepeat.setChecked(true);
				break;
			case GL_MIRRORED_REPEAT :
				sMirroredRepeat.setChecked(true);
				break;
			default : 
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(fmt.getSWrapping()) + "\".", __FILE__, __LINE__);
		}

		switch(fmt.getTWrapping())
		{
			case GL_CLAMP :
				tClamp.setChecked(true);
				break;
			case GL_CLAMP_TO_BORDER :
				tClampToBorder.setChecked(true);
				break;
			case GL_CLAMP_TO_EDGE : 
				tClampToEdge.setChecked(true);
				break;
			case GL_REPEAT :
				tRepeat.setChecked(true);
				break;
			case GL_MIRRORED_REPEAT :
				tMirroredRepeat.setChecked(true);
				break;
			default : 
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(fmt.getTWrapping()) + "\".", __FILE__, __LINE__);
		}
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

// MemoryUsageDialog
	MemoryUsageDialog::MemoryUsageDialog(int currentUsageMB, int currentMaxUsageMB, int maxUsageAllowedMB, QWidget* parent)
	 : QDialog(parent), layout(this), usageTitle(this), usageMB(this), newMaxUsageTitle(this), newMaxUsageMB(this), buttonsBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)
	{
		usageTitle.setText("Current device occupancy : ");
		usageMB.setText( tr("%1 MB / %2 MB (%3%)").arg(currentUsageMB).arg(currentMaxUsageMB).arg(static_cast<int>(static_cast<double>(currentUsageMB)/static_cast<double>(currentMaxUsageMB)*100.0)) );
		newMaxUsageTitle.setText("Maximum device ocuppancy : ");

		newMaxUsageMB.setRange(10, maxUsageAllowedMB);
		newMaxUsageMB.setValue(currentMaxUsageMB);
		newMaxUsageMB.setSuffix(" MB");

		layout.addWidget(&usageTitle, 0, 0);
		layout.addWidget(&usageMB, 0, 1);
		layout.addWidget(&newMaxUsageTitle, 1, 0);
		layout.addWidget(&newMaxUsageMB, 1, 1);
		layout.addWidget(&buttonsBox, 2, 0, 1, 2);

		connect(&buttonsBox, SIGNAL(accepted()), this, SLOT(accept()));
   		connect(&buttonsBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	int MemoryUsageDialog::getMaxUsageMB(void)
	{
		return newMaxUsageMB.value();
	}

// ImagesCollection
	std::vector<ImagesCollection*> ImagesCollection::collectionsList;
	size_t ImagesCollection::maxDeviceOccupancy = 805306368; //768MB

	ImagesCollection::ImagesCollection(const std::string& moduleName, QWidget* parent)
	 : 	TexturesList(parent),
		filterMenu(this), wrappingMenu(this), contextMenu(this),
		freeImageAction("Free image", this),	
		settingsAction("Settings", this),
		openSaveInterface(moduleName, "Image", "*.bmp *.png *.jpg *.JPEG *.pgm *.ppm")
	{
		if(collectionsList.empty())
		{
			// First object, load settings : 
			SettingsManager settingsManager;

			Element e = settingsManager.getModuleData("ImagesCollection", "MaxDeviceOccupancy");

			size_t newMaxDeviceOccupancy;			
			if(e.arguments.size()==1)
			{
				if( from_string(e.arguments.front(), newMaxDeviceOccupancy) )
					maxDeviceOccupancy = newMaxDeviceOccupancy;
			}
		}

		// Add this to the list of collections : 
		collectionsList.push_back(this);

		// Create context menu : 
		contextMenu.addMenu(&filterMenu);
		contextMenu.addMenu(&wrappingMenu);
		openSaveInterface.addSaveToMenu(contextMenu);
		contextMenu.addAction(&freeImageAction);

		// Connect : 
		connect(&openSaveInterface, 	SIGNAL(openFile(const QStringList&)), 			this, SLOT(loadImages(const QStringList&)));
		connect(&openSaveInterface, 	SIGNAL(saveFile(const QString&)), 			this, SLOT(saveImage(const QString&)));
		connect(&freeImageAction, 	SIGNAL(triggered()), 					this, SLOT(freeImages()));
		connect(this,			SIGNAL(itemSelectionChanged()), 			this, SLOT(selectionChanged()));
		connect(this,			SIGNAL(customContextMenuRequested(const QPoint&)), 	this, SLOT(showContextMenu(const QPoint&)));
		connect(&filterMenu,		SIGNAL(changeFilter(GLenum, GLenum)),			this, SLOT(changeFilter(GLenum, GLenum)));
		connect(&wrappingMenu,		SIGNAL(changeWrapping(GLenum, GLenum)),			this, SLOT(changeWrapping(GLenum, GLenum)));
		connect(&settingsAction,	SIGNAL(triggered()), 					this, SLOT(openSettingsMenu()));

		// Force updates : 
		updateMenusOnCurrentSelection();
	}

	ImagesCollection::~ImagesCollection(void)
	{
		// Remove this from the list of collections : 
		std::vector<ImagesCollection*>::iterator it = std::find(collectionsList.begin(), collectionsList.end(), this);
		collectionsList.erase(it);

		// Delete all images : 
		for(std::vector<ImageObject*>::iterator it=imagesList.begin(); it!=imagesList.end(); it++)
			delete *it;
		imagesList.clear();

		// Save settings : 
		if(collectionsList.empty())
		{
			SettingsManager settingsManager;

			Element e = settingsManager.getModuleData("ImagesCollection", "MaxDeviceOccupancy");
			
			e.arguments.assign(1, to_string(maxDeviceOccupancy));

			settingsManager.setModuleData("ImagesCollection", "MaxDeviceOccupancy", e);
		}
	}

	// Private tools : 
		int ImagesCollection::getIndexFromRecordID(int recordID) const
		{
			std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

			if(it==recordIDs.end())
				return -1;
			else
				return std::distance(recordIDs.begin(), it);
		}

		size_t ImagesCollection::currentDeviceOccupancy(size_t* canBeFreed)
		{
			size_t occupancy = 0;

			if(canBeFreed!=NULL)
				*canBeFreed = 0;

			for(int k=0; k<imagesList.size(); k++)
			{
				size_t c = 0;

				if(imagesList[k]->isOnDevice())
					c = imagesList[k]->getFormat().getSize();
			
				if(canBeFreed!=NULL && !lockedToDeviceList[k])
					*canBeFreed += c;
		
				occupancy += c;
			}

			return occupancy;
		}

		void ImagesCollection::cleanCurrentCollection(void)
		{
			for(int k=0; k<imagesList.size(); k++)
			{
				if(!lockedToDeviceList[k] && imagesList[k]->isOnDevice())
				{
					imagesList[k]->unloadFromDevice();

					TextureStatus s = recordStatus( recordIDs[k] );
					s.location = TextureStatus::OnRAM;
					updateRecordStatus( recordIDs[k], s );

					emit imageUnloadedFromDevice( recordIDs[k] );
				}
			}
		}

		size_t ImagesCollection::totalDeviceOccupancy(size_t* canBeFreed)
		{
			if(canBeFreed!=NULL)
				*canBeFreed = 0;

			size_t 	totalOccupancy = 0,
				canBeFreedCurrent = 0;

			for(int k=0; k<collectionsList.size(); k++)
			{
				totalOccupancy += collectionsList[k]->currentDeviceOccupancy( &canBeFreedCurrent );
		
				if(canBeFreed!=NULL)
					*canBeFreed += canBeFreedCurrent;
			}

			return totalOccupancy; 
		}

		bool ImagesCollection::checkMemSpaceAvailabilty(size_t futureAdd)
		{
			size_t 	totalOccupancy 	= 0,
				canBeFreed	= 0;
		
			totalOccupancy = totalDeviceOccupancy(&canBeFreed);

			if(totalOccupancy + futureAdd > maxDeviceOccupancy)
			{
				if(totalOccupancy + futureAdd > maxDeviceOccupancy + canBeFreed)
					return false; // no space available.
				else
				{
					// Some cleaning required before space allocation.
					for(int k=0; k<collectionsList.size(); k++)
						collectionsList[k]->cleanCurrentCollection();
		
					return true;
				}
			}
			else
				return true; // Plenty of space!
		}

		void ImagesCollection::updateMenusOnCurrentSelection(void)
		{
			std::vector<int> selectedRecordIDs = getSelectedRecordIDs();

			if(selectedRecordIDs.empty())
			{
				openSaveInterface.enableSaveAs(false);
				freeImageAction.setEnabled(false);
				filterMenu.update();
				wrappingMenu.update();
			}
			else 
			{
				// Find if one of the image can be saved : 
				bool 	canBeSaved 	= false,
					allHaveMipmaps 	= true;
				int	targetRecordID	= selectedRecordIDs.front();
		
				for(int k=0; k<selectedRecordIDs.size(); k++)
				{
					int tid 	= getIndexFromRecordID( selectedRecordIDs[k] );
					canBeSaved 	= canBeSaved | imagesList[tid]->isVirtual();
					allHaveMipmaps 	= allHaveMipmaps & imagesList[tid]->getFormat().getMaxLevel()>0;
		
					if(!allHaveMipmaps)
						targetRecordID = selectedRecordIDs[k];
				}

				int id = getIndexFromRecordID(targetRecordID);
		
				// Set actions :
				openSaveInterface.enableSaveAs( canBeSaved );
				freeImageAction.setEnabled( true );

				// Set menus :
				filterMenu.update( imagesList[id]->getFormat() );
				wrappingMenu.update( imagesList[id]->getFormat() );

				// Misc : 
				openSaveInterface.clearLastSaveMemory();
			}
		}

	// Private slots : 
		void ImagesCollection::selectionChanged(void)
		{
			updateMenusOnCurrentSelection();
		}

		void ImagesCollection::loadImages(const QStringList& filenames)
		{
			// Load : 
			for(int k=0; k<filenames.count(); k++)
			{
				QString currentFilename = filenames.at( k );

				try
				{
					imagesList.push_back( new ImageObject(currentFilename) );

					TextureStatus s(TextureStatus::Resource);
					s.location = TextureStatus::OnRAM;

					int newRecordID = addRecord( imagesList.back()->getName().toStdString(), imagesList.back()->getFormat(), s);
					recordIDs.push_back(newRecordID);
					lockedToDeviceList.push_back(false);

					emit imageLoaded(newRecordID);
				}
				catch(Exception& e)
				{
					// TODO
					std::cerr << "Caught exception while loading : " << std::endl;
					std::cout << e.what() << std::endl;
				}
			}
		}

		void ImagesCollection::saveImage(const QString& filename)
		{
			std::vector<int> selectedRecordIDs = getSelectedRecordIDs();

			if(selectedRecordIDs.size()==1)
			{
				int id = getIndexFromRecordID( selectedRecordIDs.front() );

				if( imagesList[id]->isVirtual() )
				{
					imagesList[id]->save( filename.toStdString() );
					openSaveInterface.reportSuccessfulSave( filename );
				}
			}
		}

		void ImagesCollection::freeImages(void)
		{
			std::vector<int> selectedRecordIDs = getSelectedRecordIDs();
	
			for(int k=0; k<selectedRecordIDs.size(); k++)
			{
				int id = getIndexFromRecordID( selectedRecordIDs[k] );

				if( imagesList[id]->isVirtual() && !imagesList[id]->wasSaved() )
				{
					// TODO Dialog : 
					
				}

				// Remove : 
				removeRecord( selectedRecordIDs[k] );
				delete imagesList[id];

				recordIDs.erase(recordIDs.begin() + id);
				imagesList.erase(imagesList.begin() + id);
				lockedToDeviceList.erase(lockedToDeviceList.begin() + id);
		
				emit imageFreed( selectedRecordIDs[k] );
			}
		}

		void ImagesCollection::showContextMenu(const QPoint& point)
		{
			QPoint globalPos = this->viewport()->mapToGlobal(point);

			updateMenusOnCurrentSelection();

			contextMenu.exec(globalPos);
		}

		void ImagesCollection::changeFilter(GLenum minFilter, GLenum magFilter)
		{
			std::vector<int> selectedRecordIDs = getSelectedRecordIDs();
			
			for(int k=0; k<selectedRecordIDs.size(); k++)
			{
				int tid = getIndexFromRecordID( selectedRecordIDs[k] );
				
				imagesList[tid]->setMinFilter(minFilter);
				imagesList[tid]->setMagFilter(magFilter);

				emit imageSettingsChanged( selectedRecordIDs[k] );

				// Show : 
				updateRecordFormat(selectedRecordIDs[k], imagesList[tid]->getFormat());
			}

			// Feedback to menus : 
			updateMenusOnCurrentSelection();
		}

		void ImagesCollection::changeWrapping(GLenum sWrapping, GLenum tWrapping)
		{
			std::vector<int> selectedRecordIDs = getSelectedRecordIDs();
			
			for(int k=0; k<selectedRecordIDs.size(); k++)
			{
				int tid = getIndexFromRecordID( selectedRecordIDs[k] );
				
				imagesList[tid]->setSWrapping(sWrapping);
				imagesList[tid]->setTWrapping(tWrapping);

				emit imageSettingsChanged( selectedRecordIDs[k] );

				// Show : 
				updateRecordFormat(selectedRecordIDs[k], imagesList[tid]->getFormat());
			}

			// Feedback to menus : 
			updateMenusOnCurrentSelection();
		}

		void ImagesCollection::openSettingsMenu(void)
		{
			MemoryUsageDialog dialogBox( totalDeviceOccupancy()/1048516, maxDeviceOccupancy/1048516, 800, this);

			if(dialogBox.exec()==QDialog::Accepted)
				maxDeviceOccupancy = static_cast<size_t>(dialogBox.getMaxUsageMB())*1048516;
		}

	// Public : 
		void ImagesCollection::addActionsToMenuBar(QMenuBar& bar)
		{
			QMenu* imageMenu = bar.addMenu("Images");

			openSaveInterface.addToMenu(*imageMenu);
			imageMenu->addAction(&freeImageAction);
	
			// Create Menu bar : 
			bar.addMenu(imageMenu);
			bar.addMenu(&filterMenu);
			bar.addMenu(&wrappingMenu);
			bar.addAction(&settingsAction);
		}

		void ImagesCollection::addToContextMenu(QAction& action)
		{
			contextMenu.addAction(&action);
		}

		void ImagesCollection::addToContextMenu(QMenu& menu)
		{
			contextMenu.addMenu(&menu);
		}

		bool ImagesCollection::imageExists(int recordID, bool throwException) const
		{
			return recordExists(recordID, throwException);
		}

		void ImagesCollection::lockTextureToDevice(int recordID)
		{
			// Fake use which will push the texture onto the device if not already : 
			texture(recordID);

			// Lock : 
			lockedToDeviceList[ getIndexFromRecordID(recordID) ] = true;
		}

		bool ImagesCollection::isTextureLockedToDevice(int recordID) const
		{
			return lockedToDeviceList[ getIndexFromRecordID(recordID) ];
		}

		void ImagesCollection::unlockTextureFromDevice(int recordID)
		{
			imageExists(recordID, true);

			// force unlock, but perform no unload : 
			lockedToDeviceList[ getIndexFromRecordID(recordID) ] = false;
		}
	
		HdlTexture& ImagesCollection::texture(int recordID)
		{
			imageExists(recordID, true);

			// Check that the target is not already on the device : 
			int id = getIndexFromRecordID(recordID);

			if( imagesList[id]->isOnDevice() )
				return imagesList[id]->texture();
			else
			{
				// Check space : 
				if(checkMemSpaceAvailabilty( imagesList[id]->getFormat().getSize() ))
				{
					// Send : 
					imagesList[id]->loadToDevice();

					TextureStatus s = recordStatus( recordID );
					s.location = TextureStatus::OnVRAM;
					updateRecordStatus( recordID, s );

					// Tell : 
					emit imageLoadedToDevice(recordID);

					return imagesList[id]->texture();
				}
				else
				{
					size_t 	occupancyMB 		= totalDeviceOccupancy()/(1024*1024)+1,
						imageOccupancyMB	= imagesList[id]->getFormat().getSize()/(1024*1024)+1,
						maxDeviceOccupancyMB	= maxDeviceOccupancy/(1024*1024)+1;
					throw Exception("Could not load image to device (Size : " + to_string(imageOccupancyMB) + "MB; used memory : " + to_string(occupancyMB) + "MB; Maximum memory : " + to_string(maxDeviceOccupancyMB) + "MB).", __FILE__, __LINE__);
				}
			}
		}

		bool canBeClosed(void)
		{
			// TODO Check the virtual images which are not saved : 
			return true;		
		}

