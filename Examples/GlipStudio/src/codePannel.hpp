#ifndef __GLIPSTUDIO_CODE_PANNEL__
#define __GLIPSTUDIO_CODE_PANNEL__

	#include "customTabWidget.hpp"
	#include "codeEditor.hpp"
	#include "openSaveInterface.hpp"
	#include "titleBar.hpp"

	class PathWidget : public Window
	{
		Q_OBJECT

		private : 
			std::vector<std::string> 	paths;
			QListWidget			data;
			QVBoxLayout			layout;
			QMenuBar 			menuBar;
			QAction 			addPathAct,
							removePathAct,
							clearAllPathAct;

		private slots :
			void update(void);
			void addPath(void);
			void removePath(void);
			void clearAll(void);

		public : 
			PathWidget(QWidget* parent=NULL);
			virtual ~PathWidget(void);

			void addPath(std::string);
			const std::vector<std::string>& getPaths(void) const;
	};

	class TemplateMenu : public QMenu
	{
		Q_OBJECT

		private : 
			enum CodeTemplates
			{
				tplAddPath,
				tplIncludeFile,
				tplRequiredFormat,
				tplRequiredPipeline,
				tplTextureFormat,
				tplGeometryStandardQuad,
				tplGeometryGrid2D,
				tplGeometryGrid3D,
				tplGeometryCustomModel,
				tplSharedSource,
				tplShaderSourceFragGLSL,
				tplShaderSourceVertGLSL,
				tplIncludeSharedSource,
				tplLinkShader,
				tplFilterLayout,
				tplPipelineLayout,
				tplMainPipelineLayout,
				tplMainPipelineLayoutIndirection,
				tplInputPorts, 
				tplOutputPorts,
				tplFilterInstance,
				tplPipelineInstance, 
				tplConnection,
				tplFullPipeline,
				tplFullMainPipeline,	
				numTemplates,
				tplUnknown
			};
	
			static const char* templatesName[numTemplates];
			static const char* templatesCode[numTemplates];
			static const char* templatesCodeWithHelp[numTemplates];

			CodeTemplates lastInsertionID;
			QSignalMapper signalMapper;
			QAction addComments;
			QAction* templatesActions[numTemplates];

		private slots :
			void insertTemplateCalled(int k);

		public : 
			TemplateMenu(QWidget* parent=NULL);
			virtual ~TemplateMenu(void);

			QString getTemplateCode(void);

		signals : 
			void insertTemplate(void);
	};

	class ElementsMenu : public QMenu
	{
		Q_OBJECT
		
		private : 
			QTimer 				timer;
			std::map<CodeEditor*, QMenu*>	menus;
			
			void updateMenu(void);

		private slots :
			void insertCalled(void);

		public : 
			ElementsMenu(QWidget* parent=NULL);
			~ElementsMenu(void);

			void scan(CodeEditor* editor, LayoutLoader::PipelineScriptElements& elements);
			void remove(CodeEditor* editor);

		signals :
			void updateElements(void);
			void insertElement(const QString& element);
	};

	class CodeEditorsPannel : public Module
	{
		Q_OBJECT
 		
		private : 
			QVBoxLayout		layout;
			CustomTabWidget		widgets;
			QMenuBar 		menuBar;
			QMenu			fileMenu;
			OpenSaveInterface	openSaveInterface;
			TemplateMenu		templateMenu;
			ElementsMenu		elementsMenu;
			QAction 		newTabAction,
						saveAllAction,
						refreshAction,
						closeTabAction,
						closeAllAction,
						showPathWidgetAction,
						showEditorSettingsAction,
						aboutAction;
			PathWidget		pathWidget;
			CodeEditorSettings	editorSettings;

			bool canBeClosed(void);
			std::string getCurrentFilename(void);
			std::string getCurrentCode(void);
			void updateCurrentToolTip(void);
			const std::vector<std::string>& getPaths(void);
			void preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos);
			void closeEvent(QEvent* event);
			void updateElementsOfEditor(CodeEditor* e);
			
		private slots : 	
			void newTab(void);
			void open(const QStringList& filenames);
			void save(void);
			void save(const QString& filename);
			void saveAs(const QString& filename);
			void saveAll(void);
			void refresh(void);
			void closeTab(void);
			void closeAll(void);
			void switchPathWidget(void);
			void updateTitle(void);
			void insertTemplate(void);
			void showEditorSettings(void);
			void aboutMessage(void);
			void tabChanged(int c);
			void updateElements(void);
			void insertElement(const QString& element);

		public :
			CodeEditorsPannel(ControlModule& _masterModule, QWidget* parent);
			~CodeEditorsPannel(void);

		public slots : 
			void openFile(const QString& filename);
			void close(void);
	};

#endif
