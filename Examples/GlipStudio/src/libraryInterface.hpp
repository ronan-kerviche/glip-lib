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

	class ConnectToInputHub : public QMenu
	{
		Q_OBJECT

		private : 
			QList<QAction *>	currentActions;
			QSignalMapper		mapper;

		public : 
			ConnectToInputHub(QWidget* parent=NULL);
			~ConnectToInputHub(void);

			void clearHub(void);
			void activate(bool a);
			void update(const __ReadOnly_PipelineLayout& layout);
		
		signals :
			void connectToInput(int i);			
	};

	class LibraryInterface : public QWidget
	{
		Q_OBJECT

		private : 
			enum
			{
				RessourceImages,
				RessourceFormats,
				RessourceInputs,
				RessourceOutputs,
				RessourceNumber
			};

			QVBoxLayout			layout;
			QMenuBar 			menuBar;
			QAction 			openAct,
							freeImageAct,
							saveAct,
							saveAsAct;
			ConnectToInputHub		connectionMenu;
			QTabWidget			tabs;
			QTreeWidget			ressourceTab,
							uniformsTab;
			QListWidget			compilationTab;

			QTreeWidgetItem* 		ressourcesHeaders[RessourceNumber];

			std::vector<TextureObject*>	textures;
			std::vector<FormatObject>	formats;
			std::vector<TextureObject*>	preferredConnections;
			
			LayoutLoader			pipelineLoader;
			Pipeline*			mainPipeline;
			HdlTexture*			currentOutputLnk;
			int 				lastUsedPipelineOutput;
			bool 				lastComputeSucceeded;

			void removeAllChildren(QTreeWidgetItem* root);
			void appendTextureInformation(QTreeWidgetItem* item, HdlTexture& texture);
			void updateRessourceAlternateColors(QTreeWidgetItem* root);
			void updateFormatList(void);
			void cleanCompilationTab(bool writeNoPipeline);
			void compilationSucceeded(void);
			void compilationFailed(Exception& e);
			void updateInputConnectionDisplay(void);
			bool disconnectLinkFromPipelineOutput(void);
			void removeImage(int id);

		private slots :
			void imageSelected(QTreeWidgetItem* item, int column);
			void loadImage(void);
			void updateConnection(int idInput);
			void compute(bool forcePipelineOutput=false);
			void freeImage(void);
					
		public : 
			LibraryInterface(QWidget *parent=NULL);
			~LibraryInterface(void);

			int getNumImages(void) const;
			bool hasOutput(void) const;
			HdlTexture& currentOutput(void);
			void compile(const std::string& code); 
			
		signals : 
			void requireRedraw(void);
	};

#endif
