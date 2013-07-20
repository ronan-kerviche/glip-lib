#ifndef __GLIPSTUDIO_CODE_EDITOR__
#define __GLIPSTUDIO_CODE_EDITOR__

	#include "GLIPLib.hpp"
	#include <QtGui>

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class CodeEditor;
	class LineNumberArea;

	class Highlighter : public QSyntaxHighlighter
	{
		Q_OBJECT

		private:
			struct HighlightingRule
			{
				QRegExp pattern;
				QTextCharFormat format;
			};

			QVector<HighlightingRule> highlightingRules;

			QRegExp commentStartExpression;
			QRegExp commentEndExpression;

			QTextCharFormat glslkeywordFormat,
					glslfunctionFormat,
					glipkeywordFormat,
					singleLineCommentFormat,
					multiLineCommentFormat,
					quotationFormat;
			//QTextCharFormat classFormat;
			//QTextCharFormat functionFormat;

		protected:
			void highlightBlock(const QString &text);

		public:
			Highlighter(QTextDocument *parent = 0);
	};

	class CodeEditor : public QPlainTextEdit 
	{
		Q_OBJECT
		
		private : 
			bool		firstModification,
					documentModified;
			QString 	currentFilename;
			QFont		font;
			Highlighter 	*highLighter;
			QWidget 	*lineNumberArea;

		protected :
			void resizeEvent(QResizeEvent *event);
			void keyPressEvent(QKeyEvent* e);

		private slots:
			void updateLineNumberAreaWidth(int newBlockCount);
			void highlightCurrentLine();
			void updateLineNumberArea(const QRect &, int);
			void documentWasModified(void);
			
		public :
			CodeEditor(QWidget *parent = 0);
			~CodeEditor(void);

			int lineNumberAreaWidth(void) const;
			bool empty(void) const;
			const QString& filename(void) const;
			QString path(void) const;
			QString getTitle(void) const;
			bool isModified(void) const;
			bool canBeClosed(void);
			void setFilename(const QString& newFilename);
			bool load(void);
			bool save(void);
			void insert(const QString& text);

			void lineNumberAreaPaintEvent(QPaintEvent *event);

		signals :
			void titleChanged(void); 
	};

	class LineNumberArea : public QWidget
	{
		Q_OBJECT

		private:
			CodeEditor *codeEditor;

		protected:
			void paintEvent(QPaintEvent *event);

		public :
			LineNumberArea(CodeEditor *editor);
			QSize sizeHint() const;
	};

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
			void addPath(void);
			void removePath(void);
			void clearAll(void);

		public : 
			PathWidget(QWidget* parent=NULL);
			~PathWidget(void);

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
				tplSharedSource,
				tplShaderSource,
				tplShaderSourceGLSL,
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
			~TemplateMenu(void);

			QString getTemplateCode(void);

		signals : 
			void insertTemplate(void);
	};

	class CodeEditorsPannel : public QWidget
	{
		Q_OBJECT
 		
		private : 
			QVBoxLayout		layout;
			QTabWidget		widgets;
			QMenuBar 		menuBar;
			QMenu			fileMenu;
			TemplateMenu		templateMenu;
			QAction 		newTabAct,
						saveAct,
						saveAsAct,
						saveAllAct,
						openAct,
						refreshAct,
						closeTabAct,
						closeAllAct,
						showPathWidget;
			PathWidget		pathWidget;
			QVector<CodeEditor*>	tabs;

		private slots : 	
			void newTab(void);
			void open(void);
			void save(void);
			void saveAs(void);
			void saveAll(void);
			void refresh(void);
			void closeTab(void);
			void closeAll(void);
			void switchPathWidget(void);
			void updateTitles(void);
			void insertTemplate(void);

		public :
			CodeEditorsPannel(QWidget* parent);
			~CodeEditorsPannel(void);

			std::string getCurrentFilename(void) const;
			const std::vector<std::string>& getPaths(void);
			bool canBeClosed(void);

		signals : 
			void requireRefresh(void);
	};

#endif
