/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : ResourceLoader.hpp                                                                        */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for manipulating uniform variables in pipelines.                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_UNIFORM_LOADER_INTERFACE__
#define __GLIPLIB_UNIFORM_LOADER_INTERFACE__
	
	#include "GLIPLib.hpp"
	#include <QWidget>
	#include <QTreeWidgetItem>
	#include <QGridLayout>
	#include <QDoubleSpinBox>
	#include <QSignalMapper>
	#include <QLabel>
	#include <QMenu>

namespace QGUI
{
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class VariableRecord : public QObject
	{
		Q_OBJECT

		private :
			static VariableRecord*		referenceRecord;
			static QVector<VariableRecord*>	records;
			QString 			name;
			int 				modification;
			bool				locked;
			HdlDynamicData* 		object;
		
			VariableRecord(void);
			VariableRecord(const VariableRecord&);	
			
		public :
			VariableRecord(const QString& _name, const GLenum& type, QObject* parent=NULL);
			~VariableRecord(void);

			const QString& getName(void) const;
			int getModificationIndex(void) const;
			const HdlDynamicData& data(void) const;
			HdlDynamicData& data(void);
			bool isLocked(void) const;
			void lock(bool enabled);
			void declareUpdate(void);

			static const VariableRecord* getRecord(const QString& name);	
			static const QVector<VariableRecord*>& getRecords(void);
			static const VariableRecord* getReferenceRecord(void); // Used for signal passing.

		signals :
			void recordAdded(const QGUI::VariableRecord*);
			void lockChanged(bool locked);
			void updated(void);
	};
	
	class QTreeWidgetSpecial : public QTreeWidget
	{
		private : 
			void scrollContentsBy(int dx, int dy);
			
		public : 
			QTreeWidgetSpecial(QWidget* parent=NULL);
	};

	class ValuesInterface : public QWidget
	{
		Q_OBJECT

		private : 
			UniformsLoader::Resource&		resource;
			QGridLayout				layout;
			QVector<QSpinBox*>			integerBoxes;
			QVector<QDoubleSpinBox*>		floatBoxes;
			QSignalMapper				signalMapper;
			QLabel					linkLabel;
			const VariableRecord*			variableLink;		
			
		private slots :
			void pushModificationToResource(int index);
			void pushAllModificationsToResource(bool emitSignal=true);
			void recordUpdated(void);
			void recordDestroyed(void);

		public : 
			ValuesInterface(UniformsLoader::Resource& _resource, QWidget* parent=NULL);
			virtual ~ValuesInterface(void);
			
			const UniformsLoader::Resource& getResource(void) const;
			UniformsLoader::Resource& getResource(void);

			void setVariableLink(const VariableRecord* lnk);
			void autoLink(void);
			void unlink(void);
			bool copyDataFromLink(void);

			void pullModificationFromResource(bool emitSignal=true);
			static ValuesInterface* getPtrFromGenericItem(QTreeWidgetItem* item, const int type);

		signals : 
			void modified(void);
	};

	class UniformsLoaderInterface : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			// Data : 
			UniformsLoader 					loader;
			std::map<const std::string, QTreeWidgetItem*>	itemRoots;
			QString						filename;

			// Tools : 
			QTreeWidgetItem* addResource(UniformsLoader::Resource& resource, QTreeWidgetItem* root);
			QTreeWidgetItem* addNode(UniformsLoader::Node& node, QTreeWidgetItem* root);
			QTreeWidgetItem* addNodeAsRoot(UniformsLoader::Node& node);
			int updateResource(UniformsLoader::Resource& resource, QTreeWidgetItem* resourceItem, bool updateOnly);
			int updateNode(UniformsLoader::Node& node, QTreeWidgetItem* nodeItem, bool updateOnly);
			bool isNodeListed(const std::string& name) const;
			void scanLoader(bool updateOnly=false);
			void updateFilenameDisplay(void);

		public :
			UniformsLoaderInterface(int type);
			virtual ~UniformsLoaderInterface(void);

			void load(Pipeline& pipeline);
			void load(QString _filename="", bool updateOnly=true);
			void save(QString _filename="");
			const QString& getFilename(void) const;
			bool hasPipeline(const std::string& name) const;
			int applyTo(Pipeline& pipeline, bool forceWrite=true, bool silent=false) const;
			void setAllExpanded(bool enabled);

		signals : 
			void modified(void);
	};

	class UniformsLinkMenu : public QMenu
	{
		Q_OBJECT

		private :
			const int 					type;
			QMap<const VariableRecord*, QAction*>		recordActions;
			QAction						*unlinkAction;
			QMap<QTreeWidgetItem*, ValuesInterface*>	currentSelection;		
	
			static const VariableRecord* getRecordFromAction(QAction* action);

		private slots :
			void addRecord(const QGUI::VariableRecord* record);
			void recordDestroyed(void);
			void setLink(void);
			void unlink(void);

		public :
			UniformsLinkMenu(int _type, QWidget* parent=NULL);
			~UniformsLinkMenu(void);

			void addToMenu(QMenu& menu);

		public slots :
			void updateToSelection(QList<QTreeWidgetItem*>& selection);
	};
}

#endif

