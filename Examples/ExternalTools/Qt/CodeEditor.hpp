/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : CodeEditor.hpp                                                                            */
/*     Original Date : July 6th 2014                                                                             */
/*                                                                                                               */
/*     Description   : Qt interface for GLIPLIB pipeline source code editor.                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_CODE_EDITOR__
#define __GLIPLIB_CODE_EDITOR__

// Includes :
	#include "GLIPLib.hpp"
	#include <cmath>
	#include <QPlainTextEdit>
	#include <QVector>
	#include <QSyntaxHighlighter>
	#include <QRegExp>
	#include <QTextCharFormat>
	#include <QString>
	#include <QFont>
	#include <QMenu>
	#include <QAction>
	#include <QMenuBar>
	#include <QVBoxLayout>
	#include <QTabWidget>
	#include <QGroupBox>
	#include <QCheckBox>
	#include <QTimer>
	#include <QLabel>
	#include <QPushButton>
	#include <QComboBox>
	#include <QSpinBox>
	#include <QSignalMapper>
	#include <QPainter>
	#include <QFileInfo>
	#include <QMessageBox>
	#include <QTextStream>
	#include <QColorDialog>
	#include <QFontDialog>

namespace QGED
{
	// Namespaces : 
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Protypes : 
	class CodeEditor;
	class LineNumberArea;
	class CodeEditorSettings;

	class Highlighter : public QSyntaxHighlighter
	{
		Q_OBJECT

		private:
			struct HighlightingRule
			{
				QRegExp pattern;
				QTextCharFormat* format;
			};

			QVector<HighlightingRule> highlightingRules;

			QRegExp commentStartExpression;
			QRegExp commentEndExpression;

			bool		highlightEnabled;
			QTextCharFormat glslkeywordFormat,
					glslfunctionFormat,
					glipLayoutLoaderKeywordFormat,
					glipUniformLoaderKeywordFormat,
					singleLineCommentFormat,
					multiLineCommentFormat;
					//quotationFormat,
					//classFormat,
					//functionFormat;

		protected:
			void highlightBlock(const QString &text);

		public:
			Highlighter(QTextDocument *parent = 0);

			void updateSettings(const CodeEditorSettings& settings);
	};

	class CodeEditor : public QPlainTextEdit 
	{
		Q_OBJECT
		
		private : 
			bool		firstModification,
					documentModified,
					documentModifiedTrigger,
					highlightLine;
			QString 	currentFilename;
			QFont		font;
			Highlighter 	*highLighter;
			QWidget 	*lineNumberArea;
			QList<QMenu*>	subMenus;

			void 		contextMenuEvent(QContextMenuEvent* event);
	
		protected :
			void resizeEvent(QResizeEvent *event);
			void keyPressEvent(QKeyEvent* e);

		private slots:
			void updateLineNumberAreaWidth(int newBlockCount);
			void highlightCurrentLine(void);
			void clearHighlightOfCurrentLine(void);
			void updateLineNumberArea(const QRect &, int);
			void documentWasModified(void);
			void updateSettings(void);
			
		public :
			CodeEditor(QWidget *parent = 0, bool syntaxColoration=true);
			virtual ~CodeEditor(void);

			int lineNumberAreaWidth(void) const;
			bool empty(void) const;
			const QString& filename(void) const;
			QString path(void) const;
			QString getRawTitle(void) const;
			QString getTitle(void) const;
			std::string currentContent(void) const;
			bool isModified(void) const;
			bool isModifiedTrigger(void);
			bool canBeClosed(void);
			void setFilename(const QString& newFilename);
			bool load(void);
			bool save(void);
			void insert(const QString& text);
			void addSubMenu(QMenu* menu);

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

	class CodeEditorSettings : public QWidget
	{
		Q_OBJECT

		private :
			static CodeEditorSettings* 	singleton;
			static const std::string	moduleName;

			// Data : 
			const int		defaultFontSize;
			QColor 			glslKeywordColor,
						glslFunctionColor,
						glipLayoutLoaderKeywordColor,
						glipUniformLoaderKeywordColor,
						commentsColor;
			QFont			editorFont,
						keywordFont;
			QTextOption::WrapMode	wrapMode;
			int			tabNumberOfSpaces;
			bool			enableHighlight,
						highlightCurrentLine;

			// Gui : 
			QGridLayout		layout;
			QGroupBox		groupColors,
						groupFonts,
						groupMisc;
			QGridLayout		layoutColors;
			QVBoxLayout		layoutFonts,
						layoutMisc;
			QLabel			glslKeywordColorLabel,
						glslFunctionColorLabel,
						glipLayoutLoaderKeywordColorLabel,
						glipUniformLoaderKeywordColorLabel,
						commentsColorLabel;
			QPushButton		glslKeywordColorButton,
						glslFunctionColorButton,
						glipLayoutLoaderKeywordColorButton,
						glipUniformLoaderKeywordColorButton,
						commentsColorButton,
						editorFontButton,
						keywordFontButton,
						okButton,
						applyButton,
						cancelButton,
						resetButton;
			QCheckBox		highlightKeywordsCheck,
						highlightCurrentLineCheck;	
			QComboBox		wrapModesBox;
			QSpinBox		tabSpacesSpin;

			void updateGUI(void);
			void updateValues(void);

		private slots :
			void changeColor(void);
			void changeFont(void);
			void softApply(void);
			void quitDialog(void);

		public : 
			CodeEditorSettings(QWidget* parent=NULL);
			~CodeEditorSettings(void);

			const QColor& getGLSLKeywordColor(void) const;
			const QColor& getGLSLFunctionColor(void) const;
			const QColor& getGLIPLayoutLoaderKeywordColor(void) const;
			const QColor& getGLIPUniformLoaderKeywordColor(void) const;
			const QColor& getCommentsColor(void) const;
			const QFont& getEditorFont(void) const;
			const QFont& getKeywordFont(void) const;
			const QTextOption::WrapMode& getWrapMode(void) const;
			const int& getNumberOfSpacesPerTabulation(void) const;
			const bool& isHighlightEnabled(void) const;
			const bool& isLineHighlightEnabled(void) const;

			static CodeEditorSettings& instance(void);

		public slots :
			void resetSettings(void);

		signals :
			void settingsModified(void);
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

	class MainWidget : public QWidget
	{
		Q_OBJECT
 		
		private : 
			QVBoxLayout		layout;
			QTabWidget		widgets;
			QMenuBar 		menuBar;
			QMenu			fileMenu;
			//OpenSaveInterface	openSaveInterface;
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
			//PathWidget		pathWidget;
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
			MainWidget(void);
			~MainWidget(void);

		public slots : 
			void openFile(const QString& filename);
			void close(void);
	};
}

#endif

