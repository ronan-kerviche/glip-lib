/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : PipelineManager.hpp                                                                       */
/*     Original Date : May 26th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for pipeline interaction.                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_PIPELINE_MANAGER__
#define __GLIPLIB_PIPELINE_MANAGER__

	// Use QVGL::SubWidget definition :  
	#define __USE_QVGL__

// Includes :
	#include "GLIPLib.hpp"
	#include "ImageItem.hpp"

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
	class Connection : public QObject
	{
		Q_OBJECT

		private :
			Connection(const Connection&);	// No copy constructor.

		public :
			Connection(void);
			virtual ~Connection(void);

			virtual bool isValid(void) const = 0;
			virtual bool selfTest(PipelineItem* _pipelineItem) const = 0;
			virtual const __ReadOnly_HdlTextureFormat& getFormat(void) const = 0;
			virtual HdlTexture& getTexture(void) = 0;

		signals :
			void modified(void);
			void statusChanged(bool valid);
			void connectionClosed(void);			
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
			~ConnectionToImageItem(void);

			bool isValid(void) const;
			bool selfTest(PipelineItem* _pipelineItem) const;
			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			HdlTexture& getTexture(void);
	};

	class ConnectionToPipelineOutput : public Connection
	{
		Q_OBJECT

		private : 
			PipelineItem*	pipelineItem;
			int		outputIdx;

		private slots : 
			void pipelineItemStatusChanged(void);
			void pipelineItemDestroyed(void);

		public :
			ConnectionToPipelineOutput(PipelineItem* _pipelineItem, int _outputIdx);
			~ConnectionToPipelineOutput(void);

			bool isValid(void) const;
			bool selfTest(PipelineItem* _pipelineItem) const;
			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			HdlTexture& getTexture(void);

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

		private slots : 
			void connectionModified(void);
			void connectionStatusChanged(bool validity);
			void connectionDestroyed(void);

		public :
			InputPortItem(PipelineItem* _parentPipeline, int _portIdx);
			~InputPortItem(void);

			PipelineItem* getParentPipelineItem(void) const;
			QString getName(void) const;
			bool isConnected(void) const;
			void connect(Connection* _connection); // Takes ownership of the connection.
			Connection* getConnection(void); // Returns the current connection (possibly NULL).
			void setName(std::string& name);

			static InputPortItem* getPtrFromGenericItem(QTreeWidgetItem* item);

		signals : 
			void connectionAdded(int portIdx);
			void connectionContentModified(int portIdx);
			void connectionStatusChanged(int portIdx, bool validity);			
			void connectionClosed(int portIdx);
	};

	class OutputPortItem : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			PipelineItem*	parentPipelineItem;
			int		portIdx;

		public : 
			OutputPortItem(PipelineItem* _parentPipeline, int _portIdx);
			~OutputPortItem(void);

			PipelineItem* getParentPipelineItem(void) const;
			QString getName(void) const;
			ConnectionToPipelineOutput* getConnection(void); // Returns a new connection to this output.
			void setName(std::string& name);

			static OutputPortItem* getPtrFromGenericItem(QTreeWidgetItem* item);

		signals : 
			void discardConnection(void);			
	};

	class PipelineItem : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			const QObject*				referrer;
			LayoutLoader				loader;
			std::string				source;
			QString					inputFormatString;
			void*					identifier;
			LayoutLoader::PipelineScriptElements	elements;
			PipelineLayout*				pipelineLayout;
			Pipeline*				pipeline;
			QTreeWidgetItem				inputsNode,
								outputsNode;
			QVector<InputPortItem*>			inputPortItems;
			QVector<OutputPortItem*>		outputPortItems;
			int					cellA,
								cellB,
								computeCount;
			

			std::string getInputFormatName(int idx);

			void preInterpret(void);
			void refurnishPortItems(void);
			bool checkConnections(void);
			void compile(void);
			void compute(void);

		private slots : 
			void connectionAdded(int portIdx);
			void connectionContentModified(int portIdx);
			void connectionStatusChanged(int portIdx, bool validity);			
			void connectionClosed(int portIdx);

		public : 
			PipelineItem(const std::string& _source, void* _identifier, const QObject* _referrer, const char* notificationMember);
			~PipelineItem(void);

			QString getName(void) const;
			void updateSource(const std::string& _source);
			bool isValid(void) const;
			const QVector<OutputPortItem*>&	getOutputPortItems(void) const;
			QString getInputPortName(int idx) const;
			QString getOutputPortName(int idx) const;
			const __ReadOnly_HdlTextureFormat& getOutputFormat(int idx);
			HdlTexture& out(int idx);
	
			void remove(void);

		signals : 	
			void statusChanged(void);
			void removed(void);
			void referrerShowUp(void);
			void compilationFailureNotification(void* identifier, Exception compilationError);
			void pipelineInputPortAdded(InputPortItem* inputPortItem);
			void pipelineOutputPortAdded(OutputPortItem* outputPortItem);
	};

	class ConnectionsMenu : public QMenu
	{
		Q_OBJECT

		private : 
			struct PotentialConnectionMap
			{
				QMap<InputPortItem*, Connection*> connectionsMap;
				
				PotentialConnectionMap(void);
				~PotentialConnectionMap(void);

				void add(InputPortItem* inputPortItem, QGIC::ImageItem* imageItem);
				void add(InputPortItem* inputPortItem, PipelineItem* pipelineItem, int outputIdx);
				void apply(void);
			};

			QAction					noImageConnectionAction,
								noPipelineConnectionAction;
			QMenu					imageItemsMenu,
								pipelineItemsMenu;
			QList<QGIC::ImageItem*>			imageItems;
			QList<PipelineItem*>			pipelineItems;
			QMap<QAction*, PotentialConnectionMap*>	potentialConnectionsMapMap;
	
		private slots : 
			void imageItemDestroyed(void);
			void pipelineItemDestroyed(void);
			void actionTriggered(void);
			void actionDestroyed(void);

		public : 
			ConnectionsMenu(QWidget* parent=NULL);
			~ConnectionsMenu(void);
		
		public slots :
			void addImageItem(QGIC::ImageItem* imageItem);
			void addPipelineItem(QGPM::PipelineItem* pipelineItem);
			void updateToSelection(QList<QTreeWidgetItem*>& selection);
	};

	class PipelineManager : public QWidget
	{
		Q_OBJECT

		private : 	
			QMap<void*, PipelineItem*>		pipelineItems;
			QList<QGIC::ImageItem*>			imageItems;
			QVBoxLayout				layout;
			QMenuBar				menuBar;
			ConnectionsMenu				connectionsMenu;
			QTreeWidget				treeWidget;

		private slots : 
			void itemSelectionChanged(void);

		public : 
			PipelineManager(void);
			~PipelineManager(void);

		public slots : 
			void addImageItem(QGIC::ImageItem* imageItem); 
			void compileSource(std::string _source, void* _identifier, const QObject* referrer, const char* notificationMember);
			void removeSource(void* _identifier);

		signals : 
			void pipelineItemAdded(QGPM::PipelineItem* pipelineItem);
	};

	#ifdef __USE_QVGL__
	class PipelineManagerSubWidget : public QVGL::SubWidget
	{
		Q_OBJECT

		private : 
			PipelineManager	manager;

		public : 
			PipelineManagerSubWidget(void);
			~PipelineManagerSubWidget(void);

			PipelineManager* getManagerPtr(void);
	};
	#endif
}

#endif

