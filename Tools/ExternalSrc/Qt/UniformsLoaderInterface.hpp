/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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
	
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class ValuesInterface : public QWidget
	{
		Q_OBJECT

		private : 
			UniformsLoader::Resource&	resource;
			QGridLayout			layout;
			std::vector<QSpinBox*>		integerBoxes;
			std::vector<QDoubleSpinBox*>	floatBoxes;
			QSignalMapper			signalMapper;
			
		private slots :
			void pushModificationToResource(int index);

		public : 
			ValuesInterface(UniformsLoader::Resource& _resource);
			~ValuesInterface(void);
			
			void pullModificationToResource(void);

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

			// Tools : 
			QTreeWidgetItem* addResource(UniformsLoader::Resource& resource, QTreeWidgetItem* root);
			QTreeWidgetItem* addNode(UniformsLoader::Node& node, QTreeWidgetItem* root);
			QTreeWidgetItem* addNodeAsRoot(UniformsLoader::Node& node);
			int updateNode(UniformsLoader::Node& node, QTreeWidgetItem* nodeItem, bool updateOnly);
			bool isNodeListed(const std::string& name) const;
			void scanLoader(bool updateOnly=false);

		public : 
			UniformsLoaderInterface(int type);
			virtual ~UniformsLoaderInterface(void);

			void load(Pipeline& pipeline);
			void load(const QString& filename, bool updateOnly=true);
			void save(const QString& filename);
			bool hasPipeline(const std::string& name) const;
			int applyTo(Pipeline& pipeline, bool forceWrite=true, bool silent=false) const;

		signals : 
			void modified(void);
	};

#endif

