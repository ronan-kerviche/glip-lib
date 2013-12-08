#ifndef __GLIPSTUDIO_LOADSAVEINTERFACE__
#define __GLIPSTUDIO_LOADSAVEINTERFACE__

	// Includes : 
	#include "settingsManager.hpp"
	#include <QObject>
	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	class OpenSaveInterface : public QObject
	{
		Q_OBJECT

		private : 
			static const int	maxLinks;
			const std::string	moduleName,
						objectName, 
						extensionsList;
			bool			multiFileOpening;
			QString			currentOpenPath,
						currentSavePath,
						currentDir;
			QAction			openAction,
						saveAction,
						saveAsAction,
						clearRecentOpenedFilesMenu,
						clearRecentSavedFilesMenu;
			QMenu			recentOpenedFilesMenu,
						recentSavedFilesMenu;
			QList<QString> 		openedFilenames,
						savedFilenames;
			std::vector<QAction*>	currentOpenedFileActions,
						currentSavedFileActions;

			bool fileExists(const QString& filename);

		private slots :
			void appendOpenedFile(const QString& filename); 
			void appendSavedFile(const QString& filename); 	
			void updateOpenMenu(void);
			void updateSaveMenu(void);
			void requestOpenAction(void);
			void open(void);
			void saveSignal(void);
			void saveAs(void);
			void clearRecentOpenedFilesList(void);
			void clearRecentSavedFilesList(void);

		public : 
			OpenSaveInterface(const std::string& _moduleName, const std::string& _objectName, const std::string& _extensionsList, bool _multiFileOpening=true);
			~OpenSaveInterface(void);

			void enableOpen(bool state);
			void enableSave(bool state);
			void enableSaveAs(bool state);
			void enableShortcuts(bool state);

			void addToMenu(QMenu& menu);
			void addSaveToMenu(QMenu& menu);

			void reportSuccessfulLoad(const QString& filename);
			void reportFailedLoad(const QString& filename);
			void reportSuccessfulSave(const QString& filename);
			//void clearLastSaveMemory(void);

			QStringList openDialog(void);
			QString saveAsDialog(const QString& specialTitle="");
			QString getDirectoryDialog(void);

		signals : 
			void openFile(const QStringList& filenames);
			void saveFile(void);				// The client must know the filename (for multiple resources, it is easier that way).
			void saveFileAs(const QString& filename);
	};

#endif

