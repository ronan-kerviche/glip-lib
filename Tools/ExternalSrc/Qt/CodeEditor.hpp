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

// Settings : 
	// Use QVGL::SubWidget definition :  
	//#define __USE_QVGL__

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
	#include <QSplitter>
	#include <QListWidget>
	#include <QElapsedTimer>
	#include <QHBoxLayout>
	#include <QTabBar>
	#include <QStackedLayout>
	#include <QToolButton>
	#include <QFileDialog>
	#include <QLineEdit>
	#include <QWidgetAction>
	#include <QTime>

	#ifdef __USE_QVGL__
		#include "GLSceneWidget.hpp"
	#endif 

	// Namespaces : 
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

namespace QGED
{
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

			QRegExp commentStartExpression,
				commentEndExpression,
				searchExpression;

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

			QTextCharFormat format(int position) const;
			void updateSettings(const CodeEditorSettings& settings);
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

	class CodeEditor : public QPlainTextEdit
	{
		Q_OBJECT
		
		private : 
			bool					highlightLine,
								braceMatching;
			QString 				currentFilename;
			QFont					font;
			Highlighter 				*highLighter;
			QWidget 				*lineNumberArea;

			QColor					braceMatchingColor;
			QList<QTextEdit::ExtraSelection>	bracesMatchExtraSelections;

			QRegExp 				searchExpression;
			QColor					searchHighlightColor;
			QList<QTextEdit::ExtraSelection>	searchExtraSelections;
	
			QList<int>				errorLineNumbers;

			QList<QMenu*>				subMenus;
	
		protected :
			int lineNumberAreaWidth(void) const;
			void lineNumberAreaPaintEvent(QPaintEvent *event);
			void resizeEvent(QResizeEvent *event);
			void keyPressEvent(QKeyEvent* e);
			void contextMenuEvent(QContextMenuEvent* event);
			static void matchBraces(QTextCursor& current, bool& acceptableMatch, QTextCursor& result);

			friend class LineNumberArea;

		private slots:
			void updateLineNumberAreaWidth(int newBlockCount);
			void updateLineNumberArea(const QRect &, int);
			void updateBracesMatchExtraSelection(void);
			void updateSearchExtraSelection(void);
			void updateExtraHighlight(void);
			void clearHighlightOfCurrentLine(void);
			void updateToCursorPosition(void);		
	
		public :
			CodeEditor(QWidget *parent = 0);
			virtual ~CodeEditor(void);

			bool empty(void) const;
			const QString& getFilename(void) const;
			void setFilename(const QString& newFilename);
			QString getPath(void) const;
			QString getRawTitle(void) const;
			QString getTitle(void) const;
			std::string getCurrentContent(void) const;
			bool isModified(void) const;
			bool canBeSaved(void) const;
			
			void open(QString newFilename="");
			void save(QString newFilename="");
			void insert(const QString& text);
			void addSubMenu(QMenu* menu);
			void updateSettings(const CodeEditorSettings& settings);
			void gotoLine(int lineNumber);

			void search(QRegExp expression, QTextDocument::FindFlags flags);
			void replace(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void replaceAll(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void clearSearch(void);

			void highlightErrorLines(const QList<int>& lineNumbers);
			void clearHighlightErrorLines(void);

		signals :
			void titleChanged(void);
			void modified(bool changed);	// Triggered once when the state changes to 'modified'.
	};

	class CodeEditorContainer : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout	layout;
			QSplitter 	splitterLayout;
			CodeEditor	editor;
			QListWidget	errorsList;

		private slots :
			void errorItemDoubleClicked(QListWidgetItem* item);

		public : 
			CodeEditorContainer(QWidget* parent);
			~CodeEditorContainer(void);

			const CodeEditor& getEditor(void) const;
			CodeEditor& getEditor(void);

		public slots : 
			void clearErrors(void);
			void showErrors(Exception compilationError);
	};

	class SearchAndReplaceMenu : public QMenu
	{
		Q_OBJECT 

		private :
			QWidgetAction	widgetAction;
			QWidget		widget;
			QGridLayout	layout;
			QLabel		searchLabel,
					replaceLabel;
			QLineEdit	searchPattern,
					replaceString;
			QCheckBox	worldOnlyCheck,
					matchCaseCheck,
					backwardSearchCheck;
			QAction		openMenuAction,
					findNextAction,
					replaceNextAction,
					replaceAllAction,
					clearHighlightAction;
			QLabel		errorString;
			CodeEditor*	target;

		private slots : 
			void prepareExpression(void);
			void clearSearchHighlight(void);
			void openMenu(void);

		public :
			SearchAndReplaceMenu(QWidget* parent=NULL);
			~SearchAndReplaceMenu(void);

			QAction* getAction(void);
			void setCurrentCodeEditor(CodeEditor* currentCodeEditor);
			void clearCurrentCodeEditor(void);
	};

	class CodeEditorSettings : public QWidget
	{
		Q_OBJECT

		private :
			// Data : 
			const int		defaultFontSize;
			QColor 			glslKeywordColor,
						glslFunctionColor,
						glipLayoutLoaderKeywordColor,
						glipUniformLoaderKeywordColor,
						commentsColor,
						braceMatchingColor,
						searchColor;
			QFont			editorFont,
						keywordFont;
			QTextOption::WrapMode	wrapMode;
			int			tabNumberOfSpaces;
			bool			enableHighlight,
						highlightCurrentLine,
						braceMatching;

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
						commentsColorLabel,
						braceMatchingColorLabel,
						searchColorLabel;
			QPushButton		glslKeywordColorButton,
						glslFunctionColorButton,
						glipLayoutLoaderKeywordColorButton,
						glipUniformLoaderKeywordColorButton,
						commentsColorButton,
						searchColorButton,
						braceMatchingColorButton,
						editorFontButton,
						keywordFontButton,
						okButton,
						applyButton,
						cancelButton,
						resetButton;
			QCheckBox		highlightKeywordsCheck,
						highlightCurrentLineCheck,
						braceMatchingCheck;	
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
			const QColor& getBraceMatchingColor(void) const;
			const QColor& getSearchColor(void) const;
			const QFont& getEditorFont(void) const;
			const QFont& getKeywordFont(void) const;
			const QTextOption::WrapMode& getWrapMode(void) const;
			const int& getNumberOfSpacesPerTabulation(void) const;
			const bool& isHighlightEnabled(void) const;
			const bool& isLineHighlightEnabled(void) const;
			const bool& isBraceMatchingEnabled(void) const;
		
			std::string getSettingsString(void) const;
			void setSettingsFromString(const std::string& str);

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
				tplInclude,
				tplCall,
				tplRequiredFormat,
				tplRequiredPipeline,
				tplTextureFormat,
				tplGeometryStandardQuad,
				tplGeometryGrid2D,
				tplGeometryGrid3D,
				tplGeometryCustomModel,
				tplSharedCode,
				tplShaderSourceFragGLSL,
				tplShaderSourceVertGLSL,
				tplInsert,
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

			QSignalMapper signalMapper;
			QAction addComments;
			QAction* templatesActions[numTemplates];

		private slots :
			void insertTemplateCalled(int k);

		public : 
			TemplateMenu(QWidget* parent=NULL);
			virtual ~TemplateMenu(void);

		signals : 
			void insertTemplate(QString str);
	};

		class ElementsMenu;

		class EditorDataMenu : public QMenu
		{
			Q_OBJECT

			private :
				const int		deltaRescan;
				const ElementsMenu*	parent;
				const CodeEditor*	editor;
				bool			modified;
				QTime			timer;

			public : 
				EditorDataMenu(ElementsMenu* _parent, CodeEditor* _editor);
				~EditorDataMenu(void);

			public slots :
				void update(void);
				void conditionalUpdate(void);
		};

	class ElementsMenu : public QMenu
	{
		Q_OBJECT
		
		private :

			QMap<CodeEditor*, EditorDataMenu*>	menus;
			
		private slots :
			void insertCalled(void);
			void editorDestroyed(void);

		public : 
			ElementsMenu(QWidget* parent=NULL);
			~ElementsMenu(void);

			void track(CodeEditor* editor);

		signals :
			void insertElement(QString element);
	};

	class RecentFilesMenu : public QMenu
	{
		Q_OBJECT

		private :
			QList<QString>	recentFiles;
			QSignalMapper	signalMapper;
			QAction		clearAction;

			void buildMenu(void);

		public :
			RecentFilesMenu(QWidget* parent=NULL);
			~RecentFilesMenu(void);

			const QList<QString>& getList(void) const;

		public slots :
			void append(const QString& filename, bool updateMenuNow=true);
			void append(const QList<QString>& filenames);
			void clear(void);

		signals : 
			void openRequest(const QString& filename);
	};

	class CodeEditorTabs : public QWidget
	{
		Q_OBJECT

		private : 
			QString				currentPath;
			QVBoxLayout			layout;
			QHBoxLayout			topBar;
			QMenuBar			menuBarLeft,
							menuBarRight;
			QMenu				mainMenu;
			QTabBar				tabBar;
			QStackedLayout			stack;
			QMap<int, CodeEditorContainer*>	editors;
			QAction				newAction,
							openAction,	
							saveAction,
							saveAsAction,
							saveAllAction,
							closeAction,
							closeAllAction,
							settingsAction,
							compileAction;
			TemplateMenu			templateMenu;
			ElementsMenu			elementsMenu;
			CodeEditorSettings		settings;
			SearchAndReplaceMenu		searchAndReplaceMenu;
			RecentFilesMenu			recentFilesMenu;

			CodeEditorContainer* getCurrentEditor(void);
			int getTabIndex(CodeEditorContainer* editor);
			int getTabIndex(CodeEditor* editor);
			void setCurrentPath(QString path);
			void save(CodeEditorContainer* editor);
			void saveAs(CodeEditorContainer* editor, QString filename="");

		//protected :
		//	virtual LayoutLoader::PipelineScriptElements scanSource(const std::string& code) const;

		public :
			CodeEditorTabs(void);
			~CodeEditorTabs(void);

		private slots :
			void tabTitleChanged(void);
			void documentModified(bool changed);
			void insert(QString str);
			void changeToTab(int tabID);
			void closeTab(int tabID, bool imperative=false);
			void updateSettings(void);
			void transferSourceCompilation(void);	

		public slots :
			void addTab(const QString& filename="");
			void open(QStringList filenameList = QStringList());
			void open(const QString& filename);
			void save(void);
			void saveAs(const QString& filename="");
			void saveAll(void);
			void compilationSuccessNotification(void* identifier);
			void compilationFailureNotification(void* identifier, Exception compilationError);
			void closeTab(void);
			void closeAll(void);

		signals :
			void compileSource(std::string source, std::string path, void* identifier, const QObject* referrer);
	};

	#ifdef __USE_QVGL__
	class CodeEditorTabsSubWidget : public QVGL::SubWidget
	{	
		private : 
			CodeEditorTabs codeEditorTabs;

		public :
			CodeEditorTabsSubWidget(void);
			~CodeEditorTabsSubWidget(void);

			CodeEditorTabs* getCodeEditorPtr(void);
	};
	#endif
}

#endif

