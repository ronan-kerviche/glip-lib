#ifndef __GLIPSTUDIO_RESOURCETAB__
#define __GLIPSTUDIO_RESOURCETAB__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	#include "imagesCollection.hpp"

	#include <QString>
	#include <QMenu>
	#include <QAction>
	#include <QHBoxLayout>
	#include <QPushButton>
	#include <QProgressBar>
	#include <QStringList>
	#include <QVBoxLayout>
	#include <QMenuBar>
	#include <QTreeWidget>
	#include <QSignalMapper>
	#include <QFileInfo>
	#include <QMessageBox>
	#include <QFileDialog>
	#include <QInputDialog>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class ConnectionMenu : public QMenu
	{
		Q_OBJECT

		private : 
			QList<QString>		portsNames;
			QList<QAction *>	currentActions;
			QSignalMapper		mapper;

		public : 
			ConnectionMenu(QWidget* parent=NULL);
			~ConnectionMenu(void);

			void activate(bool state, int numConnections=1);
			void update(void);
			void update(const __ReadOnly_PipelineLayout& layout);			

		signals :
			void connectToInput(int i);
	};

	class ResourcesTab : public Module 
	{
		Q_OBJECT

		private : 
			std::vector<int>	imageRecordIDs;

			QVBoxLayout		layout;
			QMenuBar		menuBar;
			ConnectionMenu		connectionMenu;
			ImagesCollection	collection;

			ViewManager 		viewManager;

			//void updateDisplay(WindowRenderer& display);
			bool isValidTexture(int recordID);
			HdlTexture& getTexture(int recordID);
			void giveTextureInformation(int recordID, std::string& name);
			void cleanRecordDependances(int recordID);
			void updateTexturesLinkInformation(void);

			static ViewLink* createViewLink(void* obj);

		private slots :
			// These will be called upon external modifications, in order for the module to update itself :
			void pipelineWasCreated(void);
			void pipelineWasDestroyed(void);
			void selectionChanged(void);
			void connectToInput(int i);
			void imageLoaded(int recordID);
			void imageUnloadedFromDevice(int recordID);
			void imageFreed(int recordID);
			void createNewView(void);

		public :
			ResourcesTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~ResourcesTab(void);
	};

/*// Texture object :
	class TextureObject
	{
		private : 
			bool		virtualFile;
			QString 	filename;
			HdlTexture*	textureData;

		public : 
			TextureObject(const QString& _filename, int maxLevel=0);
			TextureObject(HdlTexture& texture, const QString& _filename);
			~TextureObject(void);
		
			bool isValid(void) const;
			bool isVirtual(void) const;
			void reload(int maxLevel=0);
			const QString& getFileName(void) const;
			QString getName(void) const;
			HdlTexture& texture(void);
	};

// Format object : 
	class FormatObject : public HdlTextureFormat
	{
		private : 
			QString name;
		
		public : 
			FormatObject(const QString& _name, const __ReadOnly_HdlTextureFormat& fmt);
			FormatObject(const FormatObject& cpy);

			const QString& getName(void) const;
			void setFormat(const __ReadOnly_HdlTextureFormat& cpy);
	};

// Menus : 
	class ConnectionMenu : public QMenu
	{
		Q_OBJECT

		private : 
			QList<QString>		portsNames;
			QList<QAction *>	currentActions;
			QSignalMapper		mapper;

		public : 
			ConnectionMenu(QWidget* parent=NULL);
			~ConnectionMenu(void);

			void activate(bool state, int numConnections=1);
			void update(void);
			void update(const __ReadOnly_PipelineLayout& layout);			

		signals :
			void connectToInput(int i);
	};

// Loading widget : 
	class LoadingWidget : public QWidget
	{
		Q_OBJECT

		private : 
			QString				currentPath;
			bool				canceled;
			QHBoxLayout 			layout;
			QPushButton 			cancelButton;
			QProgressBar			progressBar;
			QStringList			filenames;
			std::vector<TextureObject*> 	loadedObjects;
		
			// Loader : 
			void clear(void);
			void loadImageProcess(const QStringList& filenames);

		private slots :
			void load(void);

		public : 
			LoadingWidget(QWidget* parent);
			~LoadingWidget(void);

			void insertNewImagesInto(std::vector<TextureObject*>& mainCollection);

		public slots :
			void cancel(void);
			void startLoad(void);		

		signals :
			void loadNext(void);
			void finished(void);
	};

// Resources GUI :
	class ResourcesTab : public Module
	{
		Q_OBJECT

		private : 
			enum ResourceCategory
			{
				ResourceImages,
				ResourceFormats,
				ResourceInputs,
				ResourceOutputs,
				ResourceNumber
			};

			// Connection hub : 
			std::vector<TextureObject*>	textures;
			std::vector<FormatObject>	formats;
			std::vector<TextureObject*>	preferredConnections;

			// GUI : 	
			QVBoxLayout		layout;
			QMenuBar 		menuBar;
			QTreeWidget		tree;
			ConnectionMenu		connectionMenu;
			FilterMenu		filterMenu;
			WrappingMenu		wrappingMenu;
			LoadingWidget		loadingWidget;
			QMenu			imageMenu;
			QAction			loadImage,
						freeImage,
						saveOutputAs,
						copyAsNewResource;

			QString			pipelineName;
			ResourceCategory	currentOutputCategory;
			int			currentOutputID;
			bool			infoLastComputeSucceeded;
			QString			currentOutputPath;

			// Tools : 
			QTreeWidgetItem* addItem(ResourceCategory category, QString title, int resourceID);
			void removeAllChildren(QTreeWidgetItem* root);
			void appendTextureInformation(QTreeWidgetItem* item);
			void appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& texture, size_t provideSize=0);
			void appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture);
			void updateResourceAlternateColors(QTreeWidgetItem* root);
			TextureObject* getCorrespondingTexture(QTreeWidgetItem* item);
			TextureObject* getCorrespondingTexture(const QString& name);
			FormatObject* getCorrespondingFormat(QTreeWidgetItem* item);
			void appendNewImage(HdlTexture& texture, const QString& filename);

			// Update sections : 
			void rebuildImageList(void);
			void updateImageListDisplay(void);
			void updateFormatListDisplay(void);
			void updateInputConnectionDisplay(void);
			void updateMenuOnCurrentSelection(ConnectionMenu* connections=NULL, FilterMenu* filters=NULL, WrappingMenu* wrapping=NULL, QAction* removeImage=NULL, QAction* saveOutAs=NULL, QAction* copyOutAs=NULL);

			void updateDisplay(void);
			void updateDisplay(WindowRenderer*& display);

			void clearPipelineInfo(void);
			void preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos);

		private slots :
			void fetchLoadedImages(void);
			void selectionChanged(void);
			void updateImageFiltering(GLenum minFilter, GLenum magFilter);
			void updateImageWrapping(GLenum sWrapping, GLenum tWrapping);
			void showContextMenu(const QPoint& point);
			void freeSelectedImages(void);
			void applyConnection(int idInput);
			void startRequestSaveImage(void);
			void requestCopyAsNewResource(void);

		public : 
			ResourcesTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~ResourcesTab(void);

			void appendFormats(LayoutLoader& loader);

			bool isInputConnected(int id) const;
			HdlTexture& input(int id);

			bool hasOutput(void) const;
			bool outputIsPartOfPipelineOutputs(void) const;
			HdlTexture* getOutput(void);

		private slots :
			void pipelineWasCreated(void);
			void pipelineCompilationFailed(Exception& e);
			void pipelineWasDestroyed(void);
			void saveOutputToFile(void);
			void copyOutputAsNewResource(void);
			void updateLastComputingStatus(bool succeeded);
	
		signals : 
			void updatePipelineRequest(void);
			void saveOutput(int i);
			void copyOutputAsNewResource(int i);
	};*/

#endif
