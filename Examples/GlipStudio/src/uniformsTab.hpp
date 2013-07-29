#ifndef __GLIPSTUDIO_UNIFORMSTAB__
#define __GLIPSTUDIO_UNIFORMSTAB__

	#include "GLIPLib.hpp"

	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	#include <QObject>
	#include <QTreeWidgetItem>
	#include <QVBoxLayout>
	#include <QLabel>
	#include <QGridLayout>
	#include <QList>
	#include <QSpinBox>
	#include <QDoubleSpinBox>
	#include <QMenu>
	#include <QMenuBar>
	#include <QAction>
	#include <QString>
	#include <QTreeWidget>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	enum NodesType
	{
		NodeFilter	= 1,		
		NodePipeline	= 2,
		NodeLeaf	= 10,
		NodeVarInteger	= 12
	};

	class BoxesSettings;

	class UniformObject : public QWidget
	{
		Q_OBJECT

		private :
			const std::string 	varName;
			bool			hasBeenUpdated;

		protected : 
			UniformObject(const std::string& _varName);
			
			virtual void applyUpdate(HdlProgram& prgm) = 0;
			virtual void readFrom(HdlProgram& prgm) = 0;

		protected slots : 
			void declareUpdate(void);

		public : 		
			virtual ~UniformObject(void);
	
			virtual QTreeWidgetItem* treeItem(void) const = 0;
			const std::string& getVarName(void) const;
			bool wasUpdated(void) const;
			void update(HdlProgram& prgm);
			void read(HdlProgram& prgm);

		public slots : 
			virtual void applySettings(BoxesSettings&) = 0;

		signals : 
			void updated(void);
	};

	class UniformUnknown : public UniformObject
	{
		Q_OBJECT 

		private : 
			QTreeWidgetItem		*item;
			QVBoxLayout		layout;
			QLabel			label;
		
			void applyUpdate(HdlProgram& prgm);
			void readFrom(HdlProgram& prgm);

		public : 
			UniformUnknown(const std::string& name, const QString& message);
			~UniformUnknown(void);

			QTreeWidgetItem* treeItem(void) const;

		public slots : 
			void applySettings(BoxesSettings&);	
	};

	class UniformInteger : public UniformObject
	{
		Q_OBJECT

		private : 
			const bool		un;
			const GLenum		type;
			QTreeWidgetItem		*item;
			QGridLayout		layout;
			QList<QSpinBox*>	data;

			void applyUpdate(HdlProgram& prgm);
			void readFrom(HdlProgram& prgm);

		public :
			UniformInteger(const std::string& name, GLenum _type, int n, int m, bool _un);
			~UniformInteger(void);

			QTreeWidgetItem* treeItem(void) const;

		public slots :
			void applySettings(BoxesSettings&);
	};

	class UniformFloat : public UniformObject
	{
		Q_OBJECT

		private : 
			const GLenum		type;
			QTreeWidgetItem		*item;
			QGridLayout		layout;
			QList<QDoubleSpinBox*>	data;

			void applyUpdate(HdlProgram& prgm);
			void readFrom(HdlProgram& prgm);

		public :
			UniformFloat(const std::string& name, GLenum _type, int n, int m);
			~UniformFloat(void);

			QTreeWidgetItem* treeItem(void) const;

		public slots :
			void applySettings(BoxesSettings&);
	};

	class FilterElement : public QObject
	{
		Q_OBJECT

		private : 
			const std::string		name;
			const std::vector<std::string>	path;
			QTreeWidgetItem			*item;
			std::vector<UniformObject*>	objects;

		public : 
			FilterElement(Filter& filter, const std::vector<std::string>& _path, QTreeWidget* tree);
			~FilterElement(void);

			QTreeWidgetItem* treeItem(void) const;
			void update(Pipeline& pipeline);
			int varsCount(void) const;

		signals :
			void updated(void);
			void propagateSettings(BoxesSettings&);
	};

	class PipelineElement : public QObject
	{
		Q_OBJECT

		private : 
			QTreeWidgetItem			*item;
			std::vector<FilterElement*>	filterObjects;
			std::vector<PipelineElement*>	pipelineObjects;

		public : 
			PipelineElement(const __ReadOnly_PipelineLayout& pipeline, const std::string& name, const std::string& pathStr, const std::vector<std::string>& path, Pipeline& mainPipeline, QTreeWidget* tree, bool isRoot = false);
			~PipelineElement(void);

			QTreeWidgetItem* treeItem(void) const;
			void update(Pipeline& pipeline);
			int varsCount(void) const;

		signals :
			void updated(void);
			void propagateSettings(BoxesSettings&);
	};

	class BoxesSettings : public QWidget
	{
		Q_OBJECT

		private : 
			QGridLayout		layout;
			QLabel			intLabel, unLabel, doubleLabel;
			QLabel			minIntLabel, maxIntLabel, stepIntLabel;
			QSpinBox		minIntBox, maxIntBox, stepIntBox;
			QLabel			maxUnLabel, stepUnLabel;
			QSpinBox		maxUnBox, stepUnBox;
			QLabel			minDoubleLabel, maxDoubleLabel, stepDoubleLabel;
			QDoubleSpinBox		minDoubleBox, maxDoubleBox, stepDoubleBox;
		
		public : 
			BoxesSettings(QWidget* parent=NULL);
			
			void updateInt(QSpinBox& box);
			void updateUn(QSpinBox& box);
			void updateDouble(QDoubleSpinBox& box);

		signals : 
			void settingsChanged(void);
	};

	class MainUniformLibrary : public QMenu
	{
		Q_OBJECT

		private : 
			enum LibAction
			{
				NoAct,
				Apply,
				Save,
				Remove
			};

			const std::string	mainLibraryFilename;
			UniformsVarsLoader	mainLibrary;
			
			QString			currentCode;
			LibAction		actionToProcess;
			QAction			infoAct,
						loadAct,
						storeAct,
						removeAct,
						syncToDiskAct;
			QMenu			availablePipelines;
			bool			showInfo;

			void showDialogCode(const QString& title, const QString& code);

		private slots : 
			void showCode(void);
			void showStoredCode(void);
			void loadFromLibrary(void);
			void saveFromLibrary(void);
			void removeFromLibrary(void);
			void syncLibraryToDisk(void);
			void updateMenu(void);

		public : 
			MainUniformLibrary(QWidget* parent);
			~MainUniformLibrary(void);

			void update(void);
			void update(Pipeline& pipeline);
			bool process(Pipeline& pipeline);

		signals : 
			void requireProcessing(void);
	};

	class UniformsTab : public QWidget
	{
		Q_OBJECT

		private : 
			bool			dontAskForSave,
						modified;
			QVBoxLayout		layout;
			QMenuBar		menuBar;
			MainUniformLibrary	mainLibraryMenu;
			QAction			showSettings,
						loadUniforms,
						saveUniforms;
			BoxesSettings		settings;
			QTreeWidget		tree;
			QString			currentPath;
			
			PipelineElement*	mainPipeline;

			void clear(void);

		private slots :
			void switchSettings(void);
			void settingsChanged(void);
			void dataWasModified(void);

		public : 
			UniformsTab(QWidget* parent=NULL);
			~UniformsTab(void);

			void updatePipeline(void);
			void updatePipeline(Pipeline& pipeline);			
			void updateData(Pipeline& pipeline);
			bool loadData(Pipeline& pipeline);	
			void saveData(Pipeline& pipeline);
			void takePipeline(Pipeline& pipeline);
			bool prepareUpdate(Pipeline* pipeline);

		signals :
			void requestDataUpdate(void);
			void requestDataLoad(void);
			void requestDataSave(void);
			void requestPipeline(void);
			void propagateSettings(BoxesSettings&);
	};

#endif

