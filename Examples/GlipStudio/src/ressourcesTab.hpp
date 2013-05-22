#ifndef __GLIPSTUDIO_RESSOURCETAB__
#define __GLIPSTUDIO_RESSOURCETAB__

	#include "GLIPLib.hpp"
	#include "RessourceLoader.hpp"
	#include <QtGui>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// Texture object :
	class TextureObject
	{
		private : 
			QString 	filename;
			HdlTexture*	textureData;

		public : 
			TextureObject(const QString& _filename, int maxLevel=0);
			~TextureObject(void);
		
			bool isValid(void) const;
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
			QList<QAction *>	currentActions;
			QSignalMapper		mapper;

		public : 
			ConnectionMenu(QWidget* parent=NULL);
			~ConnectionMenu(void);

			void activate(bool state);
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
			bool ask(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping);

		signals : 
			void changeWrapping(GLenum sWrapping, GLenum tWrapping);
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
			QVBoxLayout	layout;
			QMenuBar 	menuBar;
			QTreeWidget	tree;
			ConnectionMenu	connectionMenu;
			FilterMenu	filterMenu;
			WrappingMenu	wrappingMenu;
			QMenu		imageMenu;
			QAction		loadImage,
					freeImage;

			//QProgressBar	loadProgress;

			// Tools : 
			QTreeWidgetItem* addItem(RessourceCategory category, QString title, int ressourceID);
			void removeAllChildren(QTreeWidgetItem* root);
			void appendTextureInformation(QTreeWidgetItem* item, const __ReadOnly_HdlTextureFormat& texture, size_t provideSize=0);
			void appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture);
			void updateRessourceAlternateColors(QTreeWidgetItem* root);
			TextureObject* getCorrespondingTexture(QTreeWidgetItem* item);
			FormatObject* getCorrespondingFormat(QTreeWidgetItem* item);

			// Update sections : 
			void updateImageListDisplay(void);
			void updateFormatListDisplay(void);
			void updateInputConnectionDisplay(void);
			void updateOutputConnectionDisplay(void);
			void updateConnectionMenu(void);
			void updateFilterMenu(void);
			void updateClampMenu(void);

		private slots :
			//void selectionChanged(void);

		public : 
			RessourcesTab(QWidget* parent=NULL);
			~RessourcesTab(void);

		//public slots :
			

		//signals : 

	};

#endif
