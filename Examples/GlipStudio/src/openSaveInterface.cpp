#include "openSaveInterface.hpp"

// OpenSaveInterface
	const int OpenSaveInterface::maxLinks = 15;

	OpenSaveInterface::OpenSaveInterface(const std::string& _moduleName, const std::string& _objectName, const std::string& _extensionsList)
	 : 	moduleName(_moduleName),
		objectName(_objectName),
		extensionsList(_extensionsList),
		openAction(tr("Open %1s").arg(_objectName.c_str()), this),
		saveAction(tr("Save %1").arg(_objectName.c_str()), this),
		saveAsAction(tr("Save %1 as...").arg(_objectName.c_str()), this),
		recentFilesMenu(tr("Recent %1s").arg(_objectName.c_str()))
	{
		SettingsManager settings;

		for(int k=0; k<maxLinks; k++)
		{
			Element e = settings.getModuleData(moduleName, "RecentFile_"+to_string(k));

			if(!e.body.empty())
				append( e.getCleanBody().c_str() );
		}

		// Paths : 
		Element e1 = settings.getModuleData(moduleName, "CurrentOpenPath");
		if(!e1.body.empty())
			currentOpenPath = e1.getCleanBody().c_str();

		Element e2 = settings.getModuleData(moduleName, "CurrentSavePath");
		if(!e2.body.empty())
			currentSavePath = e2.getCleanBody().c_str();

		updateMenu();

		connect(&openAction,	SIGNAL(triggered()),	this,	SLOT(openDialog()));
		connect(&saveAction,	SIGNAL(triggered()),	this,	SLOT(saveSignal()));
		connect(&saveAsAction,	SIGNAL(triggered()),	this,	SLOT(saveAsDialog()));

		enableSave(false);
	}

	OpenSaveInterface::~OpenSaveInterface(void)
	{
		SettingsManager settings;

		for(int k=filenames.count()-1; k>=0; k--)
		{	
			const std::string name = "RecentFile_"+to_string(filenames.count()-1-k);

			Element e = settings.getModuleData(moduleName, name);

			e.body = filenames[k].toStdString();

			settings.setModuleData(moduleName, name, e);
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
	}

	void OpenSaveInterface::append(const QString& filename)
	{
		filenames.removeAll(filename);

		if(!filename.isEmpty())
		{
			QFileInfo info(filename);
			
			if(info.exists() && info.isFile())
				filenames.push_front( filename );
			else
				std::cerr << "File does not exists : \"" << filename.toStdString() << "\"." << std::endl;
		}

		while(filenames.count()>maxLinks)
			filenames.removeLast();

		updateMenu();
	}
			
	void OpenSaveInterface::updateMenu(void)
	{
		recentFilesMenu.clear();

		if(!filenames.isEmpty())
		{
			for(int k=0; k<filenames.count(); k++)
			{
				QFileInfo info( filenames[k] );
				QAction* tmp = recentFilesMenu.addAction( tr("%1. %2").arg(k+1).arg(info.fileName()), this, SLOT(requestOpenAction()));
				tmp->setToolTip( filenames[k] );
				tmp->setStatusTip( filenames[k] );
			}
		}
		else
		{
			QAction* tmp = recentFilesMenu.addAction( "(none)" );
			tmp->setDisabled(true);
		}
	}

	void OpenSaveInterface::requestOpenAction(void)
	{
		QAction* sender = reinterpret_cast<QAction*>(QObject::sender());

		QStringList filenames;

		filenames.append(sender->statusTip());

		append(sender->statusTip());

		emit openFile(filenames);
	}

	void OpenSaveInterface::openDialog(void)
	{
		QStringList filenames = QFileDialog::getOpenFileNames(NULL, tr("Open %1").arg(objectName.c_str()), currentOpenPath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));
			
		if(!filenames.isEmpty())
		{
			// Save the current path : 
			QFileInfo info(filenames.front());
			currentOpenPath = info.path();

			// Push in recent files list : 
			for(int k=0; k<filenames.count(); k++)
				append(filenames.at(k));

			emit openFile(filenames);
		}
	}

	void OpenSaveInterface::saveSignal(void)
	{
		if(!lastSaveFilename.isEmpty())
			emit saveFile( lastSaveFilename );
	}

	void OpenSaveInterface::saveAsDialog(void)
	{
		QString filename = QFileDialog::getSaveFileName(NULL, tr("Save %1").arg(objectName.c_str()), currentSavePath, tr("%1 (%2)").arg(objectName.c_str()).arg(extensionsList.c_str()));

		if(!filename.isEmpty())
			emit saveFileAs(filename);
	}

	void OpenSaveInterface::enableOpen(bool state)
	{
		openAction.setEnabled(state);
	}

	void OpenSaveInterface::enableSave(bool state)
	{
		if(!lastSaveFilename.isEmpty() && state)
			saveAction.setEnabled(state);
		else
			saveAction.setEnabled(false);
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
		menu.addMenu(&recentFilesMenu);
	}

	void OpenSaveInterface::addSaveToMenu(QMenu& menu)
	{
		menu.addAction(&saveAction);
		menu.addAction(&saveAsAction);
	}

	void OpenSaveInterface::reportSuccessfulLoad(const QString& filename)
	{
		append( filename );
	}

	void OpenSaveInterface::reportFailedLoad(const QString& filename)
	{
		filenames.removeAll(filename);

		updateMenu();
	}

	void OpenSaveInterface::reportSuccessfulSave(const QString& filename)
	{
		lastSaveFilename = filename;

		append( filename );

		QFileInfo info(filename);
		currentSavePath = info.path();
	}

	void OpenSaveInterface::clearLastSaveMemory(void)
	{
		lastSaveFilename.clear();
		saveAction.setEnabled(false);
	}

