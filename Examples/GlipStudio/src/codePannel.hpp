#ifndef __GLIPSTUDIO_CODE_PANNEL__
#define __GLIPSTUDIO_CODE_PANNEL__

	#include "codeEditor.hpp"
	#include "openSaveInterface.hpp"

	class PathWidget : public QWidget
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

	class CodeEditorsPannel : public Module
	{
		Q_OBJECT
 		
		private : 
			QVBoxLayout		layout;
			QTabWidget		widgets;
			QMenuBar 		menuBar;
			QMenu			fileMenu;
			OpenSaveInterface	openSaveInterface;
			TemplateMenu		templateMenu;
			QAction 		newTabAct,
						saveAllAct,
						refreshAct,
						closeTabAct,
						closeAllAct,
						showPathWidget,
						aboutAct;
			PathWidget		pathWidget;

			bool canBeClosed(void);
			std::string getCurrentFilename(void);
			std::string getCurrentCode(void);
			void updateCurrentToolTip(void);
			const std::vector<std::string>& getPaths(void);
			void preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos);

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
			void aboutMessage(void);
			void tabChanged(int c);

		public :
			CodeEditorsPannel(ControlModule& _masterModule, QWidget* parent);
			~CodeEditorsPannel(void);

		public slots : 
			void openFile(const QString& filename);
	};

#endif
