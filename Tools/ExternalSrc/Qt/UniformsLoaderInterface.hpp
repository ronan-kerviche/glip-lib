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

	// For user inputs :
	#define __USE_QVGL__

	#ifdef __USE_QVGL__
		#include <QElapsedTimer>
		#include "GLSceneWidget.hpp"
	#endif
	
	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	class ValuesInterface : public QWidget
	{
		Q_OBJECT

		private : 
			UniformsLoader::Resource&		resource;
			QGridLayout				layout;
			QVector<QSpinBox*>			integerBoxes;
			QVector<QDoubleSpinBox*>		floatBoxes;
			QSignalMapper				signalMapper;

			#ifdef __USE_QVGL__
				QVGL::MouseState::VectorID	vectorID;
				QVGL::MouseState::ColorID	colorID;	
			#endif
			
		private slots :
			void pushModificationToResource(int index);
			void pushAllModificationsToResource(bool emitSignal=true);

		public : 
			ValuesInterface(UniformsLoader::Resource& _resource, QWidget* parent=NULL);
			~ValuesInterface(void);
			
			const UniformsLoader::Resource& getResource(void) const;
			UniformsLoader::Resource& getResource(void);

			#ifdef __USE_QVGL__
				void setVectorLink(const QVGL::MouseState::VectorID& lnk);
				void setColorLink(const QVGL::MouseState::ColorID& lnk);
				bool copyVectorFromMouseState(const QVGL::MouseState* mouseState);
				bool copyColorFromMouseState(const QVGL::MouseState* mouseState);
			#endif

			void pullModificationToResource(void);
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
			#ifdef __USE_QVGL__
			QElapsedTimer					timer;
			int						modificationCounter;
			const int					maxCounter,
									minimumDelta_ms;
			const QVGL::MouseState*				mouseState;
			#endif

			// Tools : 
			QTreeWidgetItem* addResource(UniformsLoader::Resource& resource, QTreeWidgetItem* root);
			QTreeWidgetItem* addNode(UniformsLoader::Node& node, QTreeWidgetItem* root);
			QTreeWidgetItem* addNodeAsRoot(UniformsLoader::Node& node);
			int updateNode(UniformsLoader::Node& node, QTreeWidgetItem* nodeItem, bool updateOnly);
			int updateNodeWithMouseState(QTreeWidgetItem* nodeItem);
			bool isNodeListed(const std::string& name) const;
			void scanLoader(bool updateOnly=false);

		private slots :
			void applyModificationFromMouseState(void);	

		public :
			#ifdef __USE_QVGL__ 
			UniformsLoaderInterface(int type, const QVGL::MouseState* _mouseState=NULL);
			#else
			UniformsLoaderInterface(int type);
			#endif
			virtual ~UniformsLoaderInterface(void);

			void load(Pipeline& pipeline);
			void load(const QString& filename, bool updateOnly=true);
			void save(const QString& filename);
			bool hasPipeline(const std::string& name) const;
			int applyTo(Pipeline& pipeline, bool forceWrite=true, bool silent=false) const;

		signals : 
			void modified(void);
	};

	#ifdef __USE_QVGL__
		class UniformsLinkMenu : public QMenu
		{
			Q_OBJECT

			private :
				const int 					type;
				QMap<QVGL::MouseState::VectorID, QAction*>	vectorPositionsActions;
				QMap<QVGL::MouseState::ColorID, QAction*>	colorsActions;
				QAction						*unlinkAction;
				QSignalMapper					vectorSignalMapper,
										colorSignalMapper;
				QList<ValuesInterface*>				currentSelection;		
		
				static QString removeHeader(const QString& str);

			private slots :
				void setUniformLinkToVector(int id);
				void setUniformLinkToColor(int id);
				void unlink(void);

			public :
				UniformsLinkMenu(int _type, QWidget* parent=NULL);
				~UniformsLinkMenu(void);

			public slots :
				void updateToSelection(QList<QTreeWidgetItem*>& selection);
		};
	#endif

#endif

