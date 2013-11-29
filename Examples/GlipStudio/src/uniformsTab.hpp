#ifndef __GLIPSTUDIO_UNIFORMSTAB__
#define __GLIPSTUDIO_UNIFORMSTAB__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"

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
	#include "openSaveInterface.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	enum NodesType
	{
		NodeFilter		= 1,		
		NodePipeline		= 2,
		NodeLeaf		= 10,
		NodeVarInteger		= 11,
		NodeVarFloatingPoint	= 12,
		NodeVarUnknown		= 99
	};

	enum ExternalValueLink
	{
		LastClick,
		CurrentPosition,
		LastRelease,
		VectorCurrent,
		LastVector,
		ColorLastClick,
		ColorCurrent,
		ColorLastRelease,
		NoExternalLink,
		NumExternalValueLink,
	};

	class BoxesSettings;

	class UniformObject : public QWidget
	{
		Q_OBJECT

		private :
			const std::string 	varName;
			bool			hasBeenUpdated;
			
		protected : 
			bool			allowedExternalValueLink[NumExternalValueLink];
			ExternalValueLink	currentExternalDataLink;

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

			bool isExternalValueLinkAllowed(ExternalValueLink evl) const;
			const ExternalValueLink& getCurrentExternalValueLink(void) const;
			virtual void setExternalValueLink(ExternalValueLink evl) = 0;
			virtual bool loadExternalData(const GLSceneWidget::MouseData& mouseData);

		public slots : 
			virtual void applySettings(BoxesSettings&) = 0;

		signals : 
			void updated(void);
	};

		// Qt need this : 
		Q_DECLARE_METATYPE(UniformObject*)

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

			void setExternalValueLink(ExternalValueLink evl);

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
			void setExternalValueLink(ExternalValueLink evl);
			bool loadExternalData(const GLSceneWidget::MouseData& mouseData);

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
			void setExternalValueLink(ExternalValueLink evl);
			bool loadExternalData(const GLSceneWidget::MouseData& mouseData);

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
			bool spreadExternalData(const GLSceneWidget::MouseData& mouseData);

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
			bool spreadExternalData(const GLSceneWidget::MouseData& mouseData);

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

		public :
			enum LibraryAction
			{
				NoAct,
				Apply,
				Save,
				Remove
			};

		private :
			const std::string	mainLibraryFilename;
			UniformsVarsLoader	mainLibrary;
			
			QString			currentCode;
			LibraryAction		actionToProcess;
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
			bool process(Pipeline& pipeline, const LibraryAction& forceAction=NoAct);

		signals : 
			void requireProcessing(void);
	};

	class LinkToExternalValue : public QMenu
	{
		Q_OBJECT

		private : 
			QSignalMapper	signalMapper;
			QAction		*links[NumExternalValueLink];
			UniformObject	*target;	

		public :
			LinkToExternalValue(QWidget* parent=NULL);
			~LinkToExternalValue(void);

		public slots :
			void updateMenu(void);
			void updateMenu(QTreeWidgetItem* target);
			void changeExternalValueLinkTo(int link);
	};

	class UniformsTab : public Module
	{
		Q_OBJECT

		private : 
			bool			dontAskForSave,
						modified;
			QVBoxLayout		layout;
			QMenuBar		menuBar;
			MainUniformLibrary	mainLibraryMenu;
			QMenu			fileMenu;
			OpenSaveInterface	openSaveInterface;
			QAction			showSettings;
			BoxesSettings		settings;
			LinkToExternalValue	linkToExternalValue;
			QTreeWidget		tree;
			
			PipelineElement*	mainPipeline;

			void clear(void);
			bool prepareUpdate(void);

			// Inherited : 
			bool pipelineCanBeDestroyed(void);
			bool canBeClosed(void);

		private slots :
			void switchSettings(void);
			void settingsChanged(void);
			void dataWasModified(void);
			bool loadData(const QStringList& filenames);	
			void saveData(const QString& filename);
			void mainLibraryPipelineUpdate(void);
			void showContextMenu(const QPoint& point);

			// Inherited :
			void pipelineWasCreated(void);
			void pipelineWasDestroyed(void);
			void mouseParametersWereUpdated(const GLSceneWidget::MouseData& data);

		public : 
			UniformsTab(ControlModule& _masterModule, QWidget* parent=NULL);
			~UniformsTab(void);

		signals : 
			void requestDataUpdate(void);			
			void propagateSettings(BoxesSettings&);
	};

#endif

