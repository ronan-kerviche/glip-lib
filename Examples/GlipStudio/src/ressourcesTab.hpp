#ifndef __GLIPSTUDIO_RESSOURCETAB__
#ifndef __GLIPSTUDIO_RESSOURCETAB__

	#include "GLIPLib.hpp"
	#include <QtGui>

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

// Ressources GUI :
	class RessourcesTab : public QTreeWidget
	{
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
			QMenuBar 	menuBar;
			QMenu		imageMenu,
					connectionMenu,
					outputMenu,
					filterMenu,
					clampingMenu;
			QAction		loadImage,
					freeImage,
					freeAllImages;

			QProgressBar	loadProgress;

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
			void selectionChanged(void);

		public : 
			RessourcesTab(QWidget* parent=NULL);
			~RessourcesTab(void);

		public slots :
			

		signals : 

	};

#endif
