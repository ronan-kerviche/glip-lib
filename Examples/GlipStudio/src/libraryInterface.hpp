#ifndef __GLIPSTUDIO_LIBRARY_INTERFACE__
#define __GLIPSTUDIO_LIBRARY_INTERFACE__

	#include "GLIPLib.hpp"
	#include "RessourceLoader.hpp"
	#include <QtGui>

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

	class FormatObject : public HdlTextureFormat
	{
		private : 
			QString name;
		
		public : 
			FormatObject(const QString& _name, const __ReadOnly_HdlTextureFormat& fmt);
			FormatObject(const FormatObject& cpy);

			const QString& getName(void) const;
	};

	class LibraryInterface : public QWidget
	{
		Q_OBJECT

		private : 
			enum
			{
				RessourceImages 	= QTreeWidgetItem::UserType + 1,
				RessourceImages_Max 	= QTreeWidgetItem::UserType + 1025,
				RessourceFormats	= QTreeWidgetItem::UserType + 1026,
				RessourceFormats_Max	= QTreeWidgetItem::UserType + 2030,
				RessourceOutputs	= QTreeWidgetItem::UserType + 2031,
				RessourceOutputs_Max	= QTreeWidgetItem::UserType + 3055
			};

			QVBoxLayout			layout;
			QMenuBar 			menuBar;
			QAction 			openAct,
							saveAct,
							saveAsAct;
			QTabWidget			tabs;
			QTreeWidget			ressourceTab,
							uniformsTab;
			QListView			compilationTab;

			QList<QTreeWidgetItem*> 	ressourcesHeaders;
			QList<QTreeWidgetItem*> 	imagesList;

			std::vector<TextureObject*>	textures;
			std::vector<FormatObject>	formats;
			HdlTexture*			currentOutTexture;

		private slots :
			void imageSelected(QTreeWidgetItem* item, int column);
			void loadImage(void);
			void updateFormatList(void);
			void updateRessourceAlternateColors(void);			

		public : 
			LibraryInterface(QWidget *parent=NULL);
			~LibraryInterface(void);

			int getNumImages(void) const;
			bool hasOutput(void) const;
			HdlTexture& currentOutput(void);
			HdlTexture& texture(const std::string& name);			
			HdlTextureFormat& format(const std::string& name);
			
		signals : 
			void requireRedraw(void);
	};

#endif
