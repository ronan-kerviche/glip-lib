/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : PipelineManager.hpp                                                                       */
/*     Original Date : May 26th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for pipeline interaction.                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_PIPELINE_MANAGER__
#define __GLIPLIB_PIPELINE_MANAGER__

	// Use QVGL::SubWidget definition, and allow user interaction :  
	//#define __USE_QVGL__

// Includes :
	#include "GLIPLib.hpp"
	#include "ImageItem.hpp"
	#include "UniformsLoaderInterface.hpp"
	#include <QHeaderView>
	#include <QTimer>
	#include <QTextEdit>

	#ifdef __USE_QVGL__
		#include "GLSceneWidget.hpp"
	#endif 

namespace QGPM
{
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Special enums : 
	enum
	{
		PipelineHeaderItemType,
		InputsHeaderItemType,
		InputItemType,
		OutputsHeaderItemType,
		OutputItemType,
		UniformsHeaderItemType
	};

	// Prototypes : 
	class Connection;
	class ConnectionToImageItem;
	class ConnectionToPipelineOutput;
	class PipelineItem;
	class PipelineManager;

	// Classes :
	struct ModulesList
	{
		virtual ~ModulesList(void);
		virtual void addModules(LayoutLoader& loader) const;
		virtual std::vector<LayoutLoaderModule*> getModulesList(void) const;
	};
 
	class Connection : public QObject
	{
		Q_OBJECT

		private :
			Connection(const Connection&);	// No copy constructor.

		public :
			Connection(void);
			virtual ~Connection(void);

			virtual bool isValid(void) const = 0;
			virtual bool isReady(void) const = 0;
			virtual QString getName(void) const = 0;
			virtual QString getToolTipInformation(void) = 0;
			virtual bool selfTest(PipelineItem* _pipelineItem) const = 0;
			virtual const HdlAbstractTextureFormat& getFormat(void) const = 0;
			virtual HdlTexture& getTexture(void) = 0;
			virtual void lock(bool enabled);
			virtual const void* getIdentification(void) const = 0;

		signals :
			void modified(QVector<const void*> resourceChain);
			void formatModified(void);
			void statusChanged(bool valid);
			void connectionClosed(void);
			// destroyed() inherited from QObject		
	};

	class ConnectionToImageItem : public Connection
	{
		Q_OBJECT
	
		private : 
			QGIC::ImageItem* imageItem;

		private slots : 
			void imageItemDestroyed(void);

		public : 
			ConnectionToImageItem(QGIC::ImageItem* _imageItem);
			virtual ~ConnectionToImageItem(void);

			bool isValid(void) const;
			bool isReady(void) const;
			QString getName(void) const;
			QString getToolTipInformation(void);
			bool selfTest(PipelineItem* _pipelineItem) const;
			const HdlAbstractTextureFormat& getFormat(void) const;
			HdlTexture& getTexture(void);
			void lock(bool enabled);
			const void* getIdentification(void) const;
	};

	class ConnectionToPipelineOutput : public Connection
	{
		Q_OBJECT

		private : 
			PipelineItem*	pipelineItem;
			int		outputIdx;

		private slots : 
			void pipelineComputationFinished(int count, QVector<const void*> resourceChain);
			void pipelineItemStatusChanged(void);
			void pipelineItemDestroyed(void);

		public :
			ConnectionToPipelineOutput(PipelineItem* _pipelineItem, int _outputIdx);
			virtual ~ConnectionToPipelineOutput(void);

			bool isValid(void) const;
			bool isReady(void) const;
			QString getName(void) const;
			QString getToolTipInformation(void);
			bool selfTest(PipelineItem* _pipelineItem) const;
			const HdlAbstractTextureFormat& getFormat(void) const;
			HdlTexture& getTexture(void);
			const void* getIdentification(void) const;

		public slots :
			void safetyFuse(void);
	};

	class InputPortItem : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			PipelineItem*	parentPipelineItem;
			int		portIdx;
			Connection*	connection;
			QVGL::View*	view;

			void setText(int column, const QString& text);

		private slots : 
			void connectionModified(QVector<const void*> resourceChain);
			void connectionFormatModified(void);
			void connectionStatusChanged(bool validity);
			void connectionDestroyed(void);
			void viewClosed(void);

		public :
			InputPortItem(PipelineItem* _parentPipeline, int _portIdx);
			virtual ~InputPortItem(void);

			PipelineItem* getParentPipelineItem(void) const;
			QString getName(void) const;
			bool isConnected(void) const;
			void connect(Connection* _connection); // Takes ownership of the connection.
			Connection* getConnection(void); // Returns the current connection (possibly NULL).
			void setName(const std::string& name);

			static InputPortItem* getPtrFromGenericItem(QTreeWidgetItem* item);

		public slots : 
			void doubleClicked(int column);

		signals : 
			void connectionAdded(int portIdx);
			void connectionContentModified(int portIdx, QVector<const void*> resourceChain);
			void connectionContentFormatModified(int portIdx);
			void connectionStatusChanged(int portIdx, bool validity);			
			void connectionClosed(int portIdx);
			void addViewRequest(QVGL::View* view);
			void updateColumnSize(void);
	};

	class OutputPortItem : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			PipelineItem*	parentPipelineItem;
			int		portIdx;
			QString		filename;
			QVGL::View*	view;
	
			void setText(int column, const QString& text);
			void updateToolTip(void);

		protected : 
			void setName(const std::string& name);

			friend class PipelineItem;

		private slots : 
			void viewClosed(void);

		public : 
			OutputPortItem(PipelineItem* _parentPipeline, int _portIdx);
			virtual ~OutputPortItem(void);

			PipelineItem* getParentPipelineItem(void) const;
			QString getName(void) const;
			
			const QString& getFilename(void) const;
			void setFilename(const QString& newFilename);
			bool isValid(void) const;
			ConnectionToPipelineOutput* getConnection(void); // Returns a new connection to this output.
			HdlTexture& out(void);
			void save(void);

			static OutputPortItem* getPtrFromGenericItem(QTreeWidgetItem* item);

		public slots : 
			void pipelineDestroyed(void);
			void computationFinished(int computeCount, QVector<const void*> resourceChain);
			void doubleClicked(int column);

		signals : 
			void discardConnection(void);	
			void addViewRequest(QVGL::View* view);	
			void updateColumnSize(void);	
	};

	class PipelineItem : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			const QObject*				referrer;
			LayoutLoader				loader;
			std::string				source,
								sourceName;
			QString					inputFormatString;
			void*					identifier;
			LayoutLoader::PipelineScriptElements	elements;
			Pipeline*				pipeline;
			QTreeWidgetItem				inputsNode,
								outputsNode;
			QGUI::UniformsLoaderInterface*		uniformsNode;
			QVector<InputPortItem*>			inputPortItems;
			QVector<OutputPortItem*>		outputPortItems;
			int					cellA,
								cellB,
								computationCount;
			QString					uniformsFilename;
			bool					locked;
			int					coolDown,
								remainingCoolDownTicks;
			bool 					catchupComputation;
			QTimer					coolDownTimer;

			void setText(int column, const QString & text);
			std::string getInputFormatName(int idx);

			void updateNameString(void);
			void updateStatusString(void);
			void preInterpret(void);
			void addInputPortItem(int id, const std::string& name);
			void addOutputPortItem(int id, const std::string& name);
			void refurnishPortItems(void);
			void refurnishPortItems(const PipelineLayout& layout);
			bool checkConnections(void);
			void compile(void);
			void compute(QVector<const void*> resourceChain = QVector<const void*>());
			void checkUniforms(void);
			void checkCells(void);
			void deletePipeline(void);

		private slots : 
			void connectionAdded(int portIdx);
			void connectionContentModified(int portIdx, QVector<const void*> resourceChain);
			void connectionContentFormatModified(int portIdx);
			void connectionStatusChanged(int portIdx, bool validity);			
			void connectionClosed(int portIdx);
			void uniformsModified(void);
			void updateCoolDown(void);

		public :
			PipelineItem(void* _identifier, const QObject* _referrer, const ModulesList* modulesList);
			virtual ~PipelineItem(void);

			QString getName(void) const;
			void updateSource(const std::string& _source, const std::string& path, const std::string& _sourceName);
			bool isValid(void) const;
			const QVector<OutputPortItem*>&	getOutputPortItems(void) const;
			int getNumInputPorts(void) const;
			QString getInputPortName(int idx) const;
			int getNumOutputPorts(void) const;
			QString getOutputPortName(int idx) const;
			const HdlAbstractTextureFormat& getOutputFormat(int idx);
			HdlTexture& out(int idx);
			int getComputationCount(void) const;
			bool isReady(void) const;
			bool isLocked(void) const;
			void lock(bool enabled);
			int getCoolDown(void) const;
			void setCoolDown(int seconds);
			void renewBuffers(void);

			QString getUniformsFilename(void) const;
			void loadUniforms(QString filename="");
			void saveUniforms(QString filename="");

			static PipelineItem* getPtrFromGenericItem(QTreeWidgetItem* item);

		signals : 	
			void pipelineCreated(void);
			void pipelineDestroyed(void);
			void showIdentifierWidget(void* identifier);
			void compilationSuccessNotification(void* identifier);
			void compilationFailureNotification(void* identifier, Exception compilationError);
			void pipelineInputPortAdded(InputPortItem* inputPortItem);
			void pipelineOutputPortAdded(OutputPortItem* outputPortItem);
			void addViewRequest(QVGL::View* view);
			void computationFinished(int computeCount, QVector<const void*> resourceChain);
			void updateColumnSize(void);
	};

	class ConnectionsMenu : public QMenu
	{
		Q_OBJECT

		private : 
			class PotentialConnections
			{
				private : 
					QMap<InputPortItem*, QGIC::ImageItem*> 			imageConnections;
					QMap<InputPortItem*, QPair<PipelineItem*, int> > 	pipelineOutputConnections;
				
				public : 
					PotentialConnections(void);
					~PotentialConnections(void);

					void add(InputPortItem* inputPortItem, QGIC::ImageItem* imageItem);
					void add(InputPortItem* inputPortItem, PipelineItem* pipelineItem, int outputIdx);
					void apply(void);
			};

			QList<InputPortItem*>			selectedInputPortItems;
			QGIC::ImageItem*			voidImage;
			QAction					noImageConnectionAction,
								noPipelineConnectionAction,
								voidImageConnection;
			QMenu					imageItemsMenu,
								pipelineItemsMenu;
			QList<QGIC::ImageItem*>			imageItems;
			QList<PipelineItem*>			pipelineItems;
			QMap<QAction*, PotentialConnections*>	potentialConnectionsMap;
			
			void clearPotentialConnectionsMap(void);
			void buildMenu(void);

		private slots : 
			void imageItemDestroyed(void);
			void pipelineItemDestroyed(void);
			void actionTriggered(void);
			void voidImageActionTriggered(void);

		public : 
			ConnectionsMenu(QWidget* parent=NULL);
			virtual ~ConnectionsMenu(void);

			void addToMenu(QMenu& menu);
		
		public slots :
			void addImageItem(QGIC::ImageItem* imageItem);
			void addPipelineItem(QGPM::PipelineItem* pipelineItem);
			void updateToSelection(QList<QTreeWidgetItem*>& selection);
	};
	
	class OutputsMenu : public QMenu
	{
		Q_OBJECT 

		private : 
			QList<OutputPortItem*>		selectedOutputPortItems;
			QAction				*saveAction,
							*saveAsAction,
							*copyAsNewImageItemAction,
							*copyAction;

		private slots : 
			void outputPortItemDestroyed(void);
			void save(OutputPortItem* outputPortItem);
			void save(void);
			void saveAs(OutputPortItem* outputPortItem);
			void saveAs(void);
			void copyAsNewImageItem(OutputPortItem* outputPortItem);
			void copyAsNewImageItem(void);
			void copy(OutputPortItem* outputPortItem);
			void copy(void);	

		public : 
			OutputsMenu(QWidget* parent=NULL);
			virtual ~OutputsMenu(void);

			void addToMenu(QMenu& menu);

		public slots : 
			void updateToSelection(QList<QTreeWidgetItem*>& selection);

		signals : 
			void addImageItemRequest(HdlTexture* texture, const QString name);
	};

	class PipelineMenu : public QMenu
	{
		Q_OBJECT

		private :
			QAction		*reloadUniformsAction,
					*loadUniformsAction,
					*saveUniformsAction,
					*saveUniformsAsAction,
					*editUniformsAction,
					*toggleLockPipelineAction,
					*renewBuffersAction,
					*removePipelineAction;
			QMenu		*coolDownMenu;
			QList<QAction*>	coolDownActions;
			QSignalMapper	coolDownMapper;
			PipelineItem	*currentPipelineItem;

			void updateToggles(void);

		private slots : 
			void reloadUniforms(void);	
			void loadUniforms(void);
			void saveUniforms(void);
			void saveUniformsAs(void);
			void editUniforms(void);
			void toggleLockPipeline(void);
			void renewBuffers(void);
			void changeCoolDown(int seconds);
			void removePipeline(void);

		public : 
			PipelineMenu(QWidget* parent=NULL);
			virtual ~PipelineMenu(void);

			void addToMenu(QMenu& menu);

		public slots :
			void updateToSelection(QList<QTreeWidgetItem*>& selection);

		signals :
			void removePipeline(PipelineItem* pipelineItem);
			void editFile(const QString filename);
	};

	class ModulesDocumentation : public QWidget
	{
		Q_OBJECT

		private :
			QVBoxLayout					layout;
			QLineEdit					searchBox;
			QTextEdit					documentation;
	
		private slots :
			void searchTextEdited(const QString& text);
			void searchTextEdited(void);
		
		public :
			ModulesDocumentation(ModulesList* modulesList=NULL);
			virtual ~ModulesDocumentation(void);

			static QString getRichTextDocumentation(const LayoutLoaderModule& module);
	};

	class PipelineManager : public QWidget
	{
		Q_OBJECT

		private : 	
			QMap<void*, PipelineItem*>		pipelineItems;
			QList<QGIC::ImageItem*>			imageItems;
			QVBoxLayout				layout;
			QMenuBar				menuBar;
			PipelineMenu				pipelineMenu;
			ConnectionsMenu				connectionsMenu;
			QGUI::UniformsLinkMenu			uniformsLinkMenu;
			OutputsMenu				outputsMenu;
			QGUI::QTreeWidgetSpecial		treeWidget;
			ModulesList				*modulesList;

		private slots :
			void itemSelectionChanged(void);
			void itemDoubleClicked(QTreeWidgetItem* item, int column);
			void itemExpanded(QTreeWidgetItem* item);
			void itemCollapsed(QTreeWidgetItem* item);
			void execCustomContextMenu(const QPoint& pos); 
			void removePipeline(PipelineItem* pipelineItem);

		public : 
			PipelineManager(ModulesList* _modulesList=NULL);
			virtual ~PipelineManager(void);

			static QTreeWidgetItem* getRoot(QTreeWidgetItem* item);

		public slots : 
			void addImageItem(QGIC::ImageItem* imageItem); 
			void compileSource(std::string source, std::string path, std::string sourceName, void* identifier, const QObject* referrer);
			void removeSource(void* _identifier);
			void updateColumnSize(void);

		signals : 
			void pipelineItemAdded(QGPM::PipelineItem* pipelineItem);
			void addViewRequest(QVGL::View* view);
			void addViewsTableRequest(QVGL::ViewsTable* newTable);
			void addImageItemRequest(HdlTexture* texture, const QString name);
			void editFile(const QString filename);
	};

	#ifdef __USE_QVGL__
	class ModulesDocumentationSubWidget : public QVGL::SubWidget
	{
		Q_OBJECT 
	
		private :
			ModulesDocumentation modulesDocumentation;

		public :
			ModulesDocumentationSubWidget(ModulesList* modulesList=NULL);
			virtual ~ModulesDocumentationSubWidget(void);
	};

	class PipelineManagerSubWidget : public QVGL::SubWidget
	{
		Q_OBJECT

		private : 
			PipelineManager	manager;

		public : 
			PipelineManagerSubWidget(void);
			virtual ~PipelineManagerSubWidget(void);

			PipelineManager* getManagerPtr(void);
	};
	#endif
}

#endif

