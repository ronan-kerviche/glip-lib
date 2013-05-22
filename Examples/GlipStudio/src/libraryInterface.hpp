#ifndef __GLIPSTUDIO_LIBRARY_INTERFACE__
#define __GLIPSTUDIO_LIBRARY_INTERFACE__

	#include "GLIPLib.hpp"
	#include "ressourcesTab.hpp"
	#include <QtGui>

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
			QVBoxLayout			layout;
			QAction 			openAct,
							freeImageAct,
							saveAct,
							saveAsAct;
			ConnectToInputHub		connectionMenu;
			QTabWidget			tabs;
			RessourcesTab			ressourceTab;
			QTreeWidget			uniformsTab;
			QListWidget			compilationTab;

			std::vector<TextureObject*>	textures;
			std::vector<FormatObject>	formats;
			std::vector<TextureObject*>	preferredConnections;
			
			LayoutLoader			pipelineLoader;
			Pipeline*			mainPipeline;
			HdlTexture*			currentOutputLnk;
			int 				lastUsedPipelineOutput;
			bool 				lastComputeSucceeded;
			
			void cleanCompilationTab(bool writeNoPipeline);
			void compilationSucceeded(void);
			void compilationFailed(Exception& e);

			bool disconnectLinkFromPipelineOutput(void);
			void removeImage(int id);

		private slots :
			void imageSelected(QTreeWidgetItem* item, int column);
			void loadImage(void);
			void updateConnection(int idInput);
			void compute(bool forcePipelineOutput=false);
			void freeImage(void);
			void showContextMenu(const QPoint& pos);
					
		public : 
			LibraryInterface(QWidget *parent=NULL);
			~LibraryInterface(void);

			int getNumImages(void) const;
			bool hasOutput(void) const;
			HdlTexture& currentOutput(void);
			void compile(const std::string& code, const std::string& path); 
			
		signals : 
			void requireRedraw(void);
	};

#endif
