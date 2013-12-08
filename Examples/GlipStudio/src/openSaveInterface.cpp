#include "openSaveInterface.hpp"

// OpenSaveInterface
	const int OpenSaveInterface::maxLinks = 15;

	OpenSaveInterface::OpenSaveInterface(const std::string& _moduleName, const std::string& _objectName, const std::string& _extensionsList, bool _multiFileOpening)
	 : 	moduleName(_moduleName),
		objectName(_objectName),
		extensionsList(_extensionsList),
		multiFileOpening(_multiFileOpening),
		openAction(tr("Open %1s").arg(_objectName.c_str()), this),
		saveAction(tr("Save %1").arg(_objectName.c_str()), this),
		saveAsAction(tr("Save %1 as...").arg(_objectName.c_str()), this),
		recentOpenedFilesMenu(tr("Recent %1s opened").arg(_objectName.c_str())),
		recentSavedFilesMenu(tr("Recent %1s saved").arg(_objectName.c_str())),
		clearRecentOpenedFilesMenu("Clear list", this),
		clearRecentSavedFilesMenu("Clear list", this)
	{
		SettingsManager settings;

		for(int k=0; k<maxLinks; k++)
		{
			Element e = settings.getModuleData(moduleName, "RecentOpenedFile_"+to_string(k));

			if(!e.body.empty())
				appendOpenedFile( e.getCleanBody().c_str() );
		}

		for(int k=0; k<maxLinks; k++)
		{
			Element e = settings.getModuleData(moduleName, "RecentSavedFile_"+to_string(k));

			if(!e.body.empty())
				appendSavedFile( e.getCleanBody().c_str() );
		}

		// Paths : 
		Element e1 = settings.getModuleData(moduleName, "CurrentOpenPath");
		if(!e1.body.empty())
			currentOpenPath = e1.getCleanBody().c_str();

		Element e2 = settings.getModuleData(moduleName, "CurrentSavePath");
		if(!e2.body.empty())
			currentSavePath = e2.getCleanBody().c_str();

		Element e3 = settings.getModuleData(moduleName, "CurrentDir");
		if(!e3.body.empty())
			currentDir = e3.getCleanBody().c_str();

		updateOpenMenu();
		updateSaveMenu();

		connect(&openAction,			SIGNAL(triggered()),	this,	SLOT(open()));
		connect(&saveAction,			SIGNAL(triggered()),	this,	SLOT(saveSignal()));
		connect(&saveAsAction,			SIGNAL(triggered()),	this,	SLOT(saveAs()));
		connect(&clearRecentOpenedFilesMenu,	SIGNAL(triggered()),	this,	SLOT(clearRecentOpenedFilesList()));
		connect(&clearRecentSavedFilesMenu,	SIGNAL(triggered()),	this,	SLOT(clearRecentSavedFilesList()));

		enableSave(false);
	}

	OpenSaveInterface::~OpenSaveInterface(void)
	{
		SettingsManager settings;

		for(int k=openedFilenames.count()-1; k>=0; k--)
		{	
			const std::string name = "RecentOpenedFile_"+to_string(openedFilenames.count()-1-k);

			Element e = settings.getModuleData(moduleName, name);

			e.body = openedFilenames[k].toStdString();

			settings.setModuleData(moduleName, name, e);
		}

		for(int k=openedFilenames.count(); k<maxLinks; k++)
		{
			// Make sure we don't reintroduce old files : 
			const std::string name = "RecentOpenedFile_"+to_string(k);
			settings.removeModuleData(moduleName, name);
		}

		for(int k=savedFilenames.count()-1; k>=0; k--)
		{	
			const std::string name = "RecentSavedFile_"+to_string(savedFilenames.count()-1-k);

			Element e = settings.getModuleData(moduleName, name);

			e.body = savedFilenames[k].toStdString();

			settings.setModuleData(moduleName, name, e);
		}

		for(int k=savedFilenames.count(); k<maxLinks; k++)
		{
			// Make sure we don't reintroduce old files : 
			const std::string name = "RecentSavedFile_"+to_string(k);
			settings.removeModuleData(moduleName, name);
		}

		// Paths : 
		if(!currentOpenPath.isEmpty())
		{
			Element e = settings.getModuleData(moduleName, "CurrentOpenPath");
			e.body = currentOpenPath.toStdString();
			settings.setModuleData(moduleName, "CurrentOpenPath", e);
		}

		if(!currentSavePath.isEmpty())
		{
			Element e = settings.getModuleData(moduleName, "CurrentSavePath");
			e.body = currentSavePath.toStdString();
			settings.setModuleData(moduleName, "CurrentSavePath", e);
		}

		if(!currentDir.isEmpty())
		{
			Element e = settings.getModuleData(moduleName, "CurrentDir");
			e.body = currentDir.toStdString();
			settings.setModuleData(moduleName, "CurrentDir", e);
		}
	}

	void OpenSaveInterface::appendOpenedFile(const QString& filename)
	{
		openedFilenames.removeAll(filename);

		if(!filename.isEmpty())
		{
			QFileInfo info(filename);
			
			if(info.exists() && info.isFile())
				openedFilenames.push_front( filename );
			else
				std::cerr << "File does not exists : \"" << filename.toStdString() << "\"." << std::endl;
		}

		while(openedFilenames.count()>maxLinks)
			openedFilenames.removeLast();

		updateOpenMenu();
	}

	void OpenSaveInterface::appendSavedFile(const QString& filename)
	{
		savedFilenames.removeAll(filename);

		if(!filename.isEmpty())
		{
			QFileInfo info(filename);
			
			if(info.exists() && info.isFile())
				savedFilenames.push_front( filename );
			else
				std::cerr << "File does not exists : \"" << filename.toStdString() << "\"." << std::endl;
		}

		while(savedFilenames.count()>maxLinks)
			savedFilenames.removeLast();

		updateSaveMenu();
	}
			
	void OpenSaveInterface::updateOpenMenu(void)
	{
		recentOpenedFilesMenu.clear();
		currentOpenedFileActions.clear();

		if(!openedFilenames.isEmpty())
		{
			for(int k=0; k<openedFilenames.count(); k++)
			{
				QFileInfo info( openedFilenames[k] );
				QAction* tmp = recentOpenedFilesMenu.addAction( tr("%1. %2").arg(k+1).arg(info.fileName()), this, SLOT(requestOpenAction()));
				tmp->setToolTip( openedFilenames[k] );
				tmp->setStatusTip( openedFilenames[k] );
				currentOpenedFileActions.push_back(tmp);
			}

			recentOpenedFilesMenu.addSeparator();
			recentOpenedFilesMenu.addAction(&clearRecentOpenedFilesMenu);
		}
		else
		{
			QAction* tmp = recentOpenedFilesMenu.addAction( "(none)" );
			tmp->setDisabled(true);
		}
	}

	void OpenSaveInterface::updateSaveMenu(void)
	{
		recentSavedFilesMenu.clear();
		currentSavedFileActions.clear();

		if(!savedFilenames.isEmpty())
		{
			for(int k=0; k<savedFilenames.count(); k++)
			{
				QFileInfo info( savedFilenames[k] );
				QAction* tmp = recentSavedFilesMenu.addAction( tr("%1. %2").arg(k+1).arg(info.fileName()), this, SLOT(requestOpenAction()));
				tmp->setToolTip( savedFilenames[k] );
				tmp->setStatusTip( savedFilenames[k] );
				currentSavedFileActions.push_back(tmp);
			}

			recentSavedFilesMenu.addSeparator();
			recentSavedFilesMenu.addAction(&clearRecentSavedFilesMenu);
		}
		else
		{
			QAction* tmp = recentSavedFilesMenu.addAction( "(none)" );
			tmp->setDisabled(true);
		}
	}

	void OpenSaveInterface::requestOpenAction(void)
	{
		QAction* sender = reinterpret_cast<QAction*>(QObject::sender());

		QStringList openedFilenames;

		openedFilenames.append(sender->statusTip());

		appendOpenedFile(sender->statusTip());

		emit openFile(openedFilenames);
	}

	void OpenSaveInterface::open(void)
	{
		QStringList openedFilenames = openDialog();

		if(!openedFilenames.isEmpty())
			emit openFile(openedFilenames);
	}

	void OpenSaveInterface::saveSignal(void)
	{
		emit saveFile();
	}

	void OpenSaveInterface::saveAs(void)
	{
		QString filename = saveAsDialog();

		if(!filename.isEmpty())
			emit saveFileAs(filename);
	}

	void OpenSaveInterface::clearRecentOpenedFilesList(void)
	{
		openedFilenames.clear();
		updateOpenMenu();
	}

	void OpenSaveInterface::clearRecentSavedFilesList(void)
	{
		savedFilenames.clear();
		updateSaveMenu();
	}

	void OpenSaveInterface::enableOpen(bool state)
	{
		openAction.setEnabled(state);
		
		for(std::vector<QAction*>::iterator it=currentOpenedFileActions.begin(); it!=currentOpenedFileActions.end(); it++)
			(*it)->setEnabled(state);

		for(std::vector<QAction*>::iterator it=currentSavedFileActions.begin(); it!=currentSavedFileActions.end(); it++)
			(*it)->setEnabled(state);
	}

	void OpenSaveInterface::enableSave(bool state)
	{
		saveAction.setEnabled(state);
	}

	void OpenSaveInterface::enableSaveAs(bool state)
	{
		saveAsAction.setEnabled(state);
	}

	void OpenSaveInterface::enableShortcuts(bool state)
	{
		if(state)
		{
			openAction.setShortcuts(QKeySequence::Open);
			saveAction.setShortcuts(QKeySequence::Save);
			saveAsAction.setShortcuts(QKeySequence::SaveAs);
		}
		else
		{
			openAction.setShortcut( QKeySequence() );
			saveAction.setShortcut( QKeySequence() );
			saveAsAction.setShortcut( QKeySequence() );
		}
	}

	void OpenSaveInterface::addToMenu(QMenu& menu)
	{
		menu.addAction(&openAction);
		addSaveToMenu(menu);
		menu.addMenu(&recentOpenedFilesMenu);
		menu.addMenu(&recentSavedFilesMenu);
	}

	void OpenSaveInterface::addSaveToMenu(QMenu& menu)
	{
		menu.addAction(&saveAction);
		menu.addAction(&saveAsAction);
	}

	void OpenSaveInterface::reportSuccessfulLoad(const QString& filename)
	{
		appendOpenedFile( filename );
	}

	void OpenSaveInterface::reportFailedLoad(const QString& filename)
	{
		openedFilenames.removeAll(filename);

		updateOpenMenu();
	}

	void OpenSaveInterface::reportSuccessfulSave(const QString& filename)
	{
		appendSavedFile( filename );

		QFileInfo info(filename);
		currentSavePath = info.path();
	}

	/*void OpenSaveInterface::clearLastSaveMemory(void)
	{
		saveAction.setEnabled(false);
	}*/

	QStringList OpenSaveInterface::openDialog(void)
	{
		QStringList filenames;

		if(multiFileOpening)
			filenames = QFileDialog::getOpenFileNames(NULL, tr("Open %1").arg(objectName.c_str()), currentOpenPath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));
		else
		{
			QString singleFilename = QFileDialog::getOpenFileName(NULL, tr("Open %1").arg(objectName.c_str()), currentOpenPath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));

			if(!singleFilename.isEmpty())
				filenames.append(singleFilename);
		}
			
		if(!filenames.isEmpty())
		{
			// Save the current path : 
			QFileInfo info(filenames.front());
			currentOpenPath = info.path();

			// Push in recent files list : 
			for(int k=0; k<filenames.count(); k++)
				appendOpenedFile(filenames.at(k));	
		}

		return filenames;
	}

	QString OpenSaveInterface::saveAsDialog(const QString& specialTitle)
	{
		if(!specialTitle.isEmpty())
			return QFileDialog::getSaveFileName(NULL, tr("Save %1 : %2").arg(objectName.c_str()).arg(specialTitle), currentSavePath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));
		else 
			return QFileDialog::getSaveFileName(NULL, tr("Save %1").arg(objectName.c_str()), currentSavePath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));
	}

	QString OpenSaveInterface::getDirectoryDialog(void)
	{
		QString dir = QFileDialog::getExistingDirectory(NULL, "Open Directory", currentDir);

		if(!dir.isEmpty())
			currentDir = dir;

		return dir;
	}

