#ifndef __GLIPSTUDIO_RESSOURCETAB__
#define __GLIPSTUDIO_RESSOURCETAB__

	#include "GLIPLib.hpp"
	#include "RessourceLoader.hpp"

	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

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
	//#include <unistd.h>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// Texture object :
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

	class FilterMenu : public QMenu
	{
		// GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR

		Q_OBJECT

		private : 
			// Sub menus : 
			QMenu 	minFilter,
				magFilter;

			// Actions list : 
			QAction bothNearest,
				bothLinear,
				minNearest,
				minLinear,
				minNearestMipmapNearest,
				minNearestMipmapLinear,
				minLinerarMipmapNearest,
				minLinearMipmapLinear,
				magNearest,
				magLinear;

		private slots : 
			void processAction(QAction* action);

		public : 
			FilterMenu(QWidget* parent=NULL);

			void update(void);
			void update(const __ReadOnly_HdlTextureFormat& fmt);
			void get(QAction* action, GLenum& dminFilter, GLenum& dmagFilter);
			bool ask(const QPoint& pos, GLenum& minFilter, GLenum& magFilter);

		signals : 
			void changeFilter(GLenum minFilter, GLenum magFilter);
	};

	class WrappingMenu : public QMenu 
	{
		// GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT

		Q_OBJECT

		private : 
			// Sub Menus : 
			QMenu 	sMenu,
				tMenu;
	
			QAction	bothClamp,
				bothClampToBorder,
				bothClampToEdge,
				bothRepeat,
				bothMirroredRepeat,
				sClamp,
				sClampToBorder,
				sClampToEdge,
				sRepeat,
				sMirroredRepeat,
				tClamp,
				tClampToBorder,
				tClampToEdge,
				tRepeat,
				tMirroredRepeat;		
			
		private slots : 
			void processAction(QAction* action);

		public : 
			WrappingMenu(QWidget* parent=NULL);

			void update(void);
			void update(const __ReadOnly_HdlTextureFormat& fmt);
			void get(QAction* action, GLenum& dsWrapping, GLenum& dtWrapping);
			bool ask(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping);
			
		signals : 
			void changeWrapping(GLenum sWrapping, GLenum tWrapping);
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

// Ressources GUI :
	class RessourcesTab : public QWidget
	{
		Q_OBJECT

		private : 
			enum RessourceCategory
			{
				RessourceImages,
				RessourceFormats,
				RessourceInputs,
				RessourceOutputs,
				RessourceNumber
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
						copyAsNewRessource;

			QString			pipelineName;
			RessourceCategory	currentOutputCategory;
			int			currentOutputID;
			bool			infoLastComputeSucceeded;
			QString			currentOutputPath;

			// Tools : 
			QTreeWidgetItem* addItem(RessourceCategory category, QString title, int ressourceID);
			void removeAllChildren(QTreeWidgetItem* root);
			void appendTextureInformation(QTreeWidgetItem* item);
			void appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& texture, size_t provideSize=0);
			void appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture);
			void updateRessourceAlternateColors(QTreeWidgetItem* root);
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

		private slots :
			void fetchLoadedImages(void);
			void selectionChanged(void);
			void updateImageFiltering(GLenum minFilter, GLenum magFilter);
			void updateImageWrapping(GLenum sWrapping, GLenum tWrapping);
			void showContextMenu(const QPoint& point);
			void freeSelectedImages(void);
			void applyConnection(int idInput);
			void startRequestSaveImage(void);
			void requestCopyAsNewRessource(void);

		public : 
			RessourcesTab(QWidget* parent=NULL);
			~RessourcesTab(void);

			void appendFormats(LayoutLoader& loader);

			bool isInputConnected(int id) const;
			HdlTexture& input(int id);

			bool hasOutput(void) const;
			bool outputIsPartOfPipelineOutputs(void) const;
			HdlTexture* getOutput(Pipeline* pipeline);

		public slots :
			void updatePipelineInfos(void);
			void updatePipelineInfos(Pipeline* pipeline);	
			void saveOutputToFile(HdlTexture& output);
			void copyOutputAsNewRessource(HdlTexture& output);
			void updateLastComputingStatus(bool succeeded);
	
		signals : 
			void outputChanged(void);
			void updatePipelineRequest(void);
			void saveOutput(int i);
			void copyOutputAsNewRessource(int i);
	};

#endif
