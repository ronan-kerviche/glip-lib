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

#ifndef __GLIPLIB_UNIFORM_VARS_LOADER_INTERFACE__
#define __GLIPLIB_UNIFORM_VARS_LOADER_INTERFACE__
	
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
			UniformsVarsLoader::Resource&	resource;
			QGridLayout			layout;
			std::vector<QDoubleSpinBox*>	boxes;
			QSignalMapper			signalMapper;
			
		private slots :
			void pushModificationToResource(int index);

		public : 
			ValuesInterface(UniformsVarsLoader::Resource& _resource);
			~ValuesInterface(void);
			
		signals : 
			void modified(void);
	};

	class UniformsVarsLoaderInterface : public QObject, public QTreeWidgetItem
	{
		Q_OBJECT

		private : 
			// Data : 
			UniformsVarsLoader 				loader;
			std::map<const std::string, QTreeWidgetItem*>	itemRoots;

			// Tools : 
			QTreeWidgetItem* addResource(UniformsVarsLoader::Resource& resource, QTreeWidgetItem* root);
			QTreeWidgetItem* addNode(UniformsVarsLoader::Node& node, QTreeWidgetItem* root);
			QTreeWidgetItem* addNodeAsRoot(UniformsVarsLoader::Node& node);
			bool isNodeListed(const std::string& name) const;
			void scanLoader(void);

		public : 
			UniformsVarsLoaderInterface(int type);
			virtual ~UniformsVarsLoaderInterface(void);

			void load(std::string source);
			void load(Pipeline& pipeline);
			bool hasPipeline(const std::string& name) const;
			int applyTo(Pipeline& pipeline, bool forceWrite=true) const;

		signals : 
			void modified(void);
	};

#endif

