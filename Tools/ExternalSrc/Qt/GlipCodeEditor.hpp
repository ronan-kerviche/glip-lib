/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
	#include <QGroupBox>
	#include <QCheckBox>
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
	#include <QFileDialog>
	#include <QSettings>
	#include <QDialogButtonBox>
	#include <QGridLayout>
	#include <QTimer>

namespace QGlip
{
// Protypes : 
	class CodeEditor;
	class LineNumberArea;
	class CodeEditorSettings;	
	class ElementsMenu;

// Highlighter :
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
			QTextCharFormat glslKeywordFormat,
					glslFunctionFormat,
					glslPreprocessorFormat,
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
			virtual ~Highlighter(void);

			QTextCharFormat format(int position) const;
			void updateSettings(const CodeEditorSettings& settings);
	};	

// LineNumberArea :
	class LineNumberArea : public QWidget
	{
		Q_OBJECT

		private:
			CodeEditor *codeEditor;

		protected:
			void paintEvent(QPaintEvent *event);

		public :
			LineNumberArea(CodeEditor *editor);
			virtual ~LineNumberArea(void);
			QSize sizeHint() const;
	};

// ElementsMenu :
	class ElementsMenu : public QMenu
	{
		Q_OBJECT

		private :
			QSignalMapper signalMapper;

		public :
			ElementsMenu(QWidget* parent=NULL);
			virtual ~ElementsMenu(void);
			void update(const Glip::Modules::LayoutLoader::PipelineScriptElements& elements, const QFileInfo& fileInfo);

		signals :
			void elementSelected(QString text);
	};

// CodeEditor :
	class CodeEditor : public QPlainTextEdit
	{
		Q_OBJECT
		
		private : 
			bool					highlightLine,
								braceMatching;
			QFileInfo				fileInfo;
			QFont					font;
			Highlighter 				*highLighter;
			QWidget 				*lineNumberArea;
			QColor					braceMatchingColor;
			QList<QTextEdit::ExtraSelection>	bracesMatchExtraSelections;
			QRegExp 				searchExpression;
			QColor					searchHighlightColor;
			QList<QTextEdit::ExtraSelection>	searchExtraSelections;
			QVector<int>				errorLineNumbers;
			QVector<QMenu*>				subContextMenus;
			QVector<ElementsMenu*>			managedMenus;
			QTimer					timer;
	
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
			void updateElements(void);
			void subContextMenuDestroyed(void);
			void elementsMenuDestroyed(void);
	
		public :
			CodeEditor(QWidget *parent=NULL);
			virtual ~CodeEditor(void);

			bool isEmpty(void) const;
			const QFileInfo& getFileInfo(void) const;
			std::string getContent(void) const;
			bool isModified(void) const;
			void open(QString newFilename="");
			void save(QString newFilename="");
			void insert(const QString& text);
			void addSubContextMenu(QMenu* menu);
			ElementsMenu* createManagedMenu(void);
			void updateSettings(const CodeEditorSettings& settings);
			void gotoLine(int lineNumber);
			void search(QRegExp expression, QTextDocument::FindFlags flags);
			void replace(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void replaceAll(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void clearSearch(void);
			void highlightErrorLines(const QVector<int>& lineNumbers);
			void clearHighlightErrorLines(void);

		signals :
			void fileInfoChanged(void);
			void modified(bool changed);	// Triggered once when the state changes to 'modified'.
	};

// CodeEditorSettings :
	class CodeEditorSettings
	{
		private :
			QSettings 		settingsManager;

		public :
			const int		defaultFontSize;
			QColor 			glslKeywordColor,
						glslFunctionColor,
						glslPreprocessorColor,
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
			CodeEditorSettings(const int _defaultFontSize=8);
			CodeEditorSettings(const CodeEditorSettings& c);
			virtual ~CodeEditorSettings(void);
			void resetSettings(void);
			void loadSettings(void);
			void saveSettings(void);
	};

// CodeEditorSettingsWidget :
	class CodeEditorSettingsWidget : public QWidget
	{
		Q_OBJECT

		private :
			// Data :
			CodeEditorSettings	settings;

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
						glslPreprocessorColorLabel,
						glipLayoutLoaderKeywordColorLabel,
						glipUniformLoaderKeywordColorLabel,
						commentsColorLabel,
						braceMatchingColorLabel,
						searchColorLabel;
			QPushButton		glslKeywordColorButton,
						glslFunctionColorButton,
						glslPreprocessorColorButton,
						glipLayoutLoaderKeywordColorButton,
						glipUniformLoaderKeywordColorButton,
						commentsColorButton,
						searchColorButton,
						braceMatchingColorButton,
						editorFontButton,
						keywordFontButton;
			QDialogButtonBox	dialogButtons;
			QCheckBox		highlightKeywordsCheck,
						highlightCurrentLineCheck,
						braceMatchingCheck;	
			QComboBox		wrapModesBox;
			QSpinBox		tabSpacesSpin;

			void updateInterface(void);
			void updateValues(void);

		private slots :
			void changeColor(void);
			void changeFont(void);
			void processDialogButtonPressed(QAbstractButton* button);

		public : 
			CodeEditorSettingsWidget(void);
			virtual ~CodeEditorSettingsWidget(void);
			const CodeEditorSettings& getSettings(void) const;

		public slots :
			void resetSettings(void);

		signals :
			void settingsModified(void);
	};

// CodeEditorContainer :
/*	class CodeEditorContainer : public QWidget
	{
		Q_OBJECT

		private : 
			QVBoxLayout	layout;
			QSplitter 	splitterLayout;
			CodeEditor	editor;
			QTreeWidget	errorsList;

		private slots :
			void errorItemDoubleClicked(QTreeWidgetItem* item, int column);

		public : 
			CodeEditorContainer(QWidget* parent);
			virtual ~CodeEditorContainer(void);

			const CodeEditor& getEditor(void) const;
			CodeEditor& getEditor(void);

		public slots : 
			void clearErrors(void);
			void showErrors(Glip::Exception compilationError);

		signals:
			void openFile(QString filename, int lineNumber);
	};

// SearchAndReplaceMenu :
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
			virtual ~SearchAndReplaceMenu(void);

			QAction* getAction(void);
			void setCurrentCodeEditor(CodeEditor* currentCodeEditor);
			void clearCurrentCodeEditor(void);
	};
*/



// TemplateMenu
/*
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

// RecentFilesMenu :
	class RecentFilesMenu : public QMenu
	{
		Q_OBJECT

		private :
			QSettings		settingsManager;
			const QString		name;
			QList<QString>		recentFiles;
			QSignalMapper		signalMapper;
			QAction			clearAction;

			void buildMenu(void);

		public :
			RecentFilesMenu(const QString& _name, QWidget* parent=NULL);
			virtual ~RecentFilesMenu(void);

			const QList<QString>& getList(void) const;

		public slots :
			void append(const QString& filename, bool updateMenuNow=true);
			void append(const QList<QString>& filenames);
			void clear(void);

		signals : 
			void openRequest(const QString& filename);
	};

// CodeEditorTabs :
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
			CodeEditorSettingsWidget	settingsInterface;
			SearchAndReplaceMenu		searchAndReplaceMenu;
			RecentFilesMenu			recentFilesMenu;

			CodeEditorContainer* getCurrentEditor(void);
			CodeEditorContainer* getEditor(int tabID);
			int getTabIndex(CodeEditorContainer* editor);
			int getTabIndex(CodeEditor* editor);
			int getTabIndex(const QString& filename);
			void setCurrentPath(QString path);
			void save(CodeEditorContainer* editor);
			void saveAs(CodeEditorContainer* editor, QString filename="");
			void removeTab(int tabID);
			void closeEvent(QCloseEvent* event);

		public :
			CodeEditorTabs(void);
			virtual ~CodeEditorTabs(void);
			CodeEditorSettingsWidget& getEditorSettingsInterface(void);
			const CodeEditorSettings& getEditorSettings(void) const;

		private slots :
			void addTab(const QString& filename, int lineNumber=0);
			void tabTitleChanged(void);
			void documentModified(bool changed);
			void insert(QString str);
			void changedToTab(int tabID);
			bool closeTab(int tabID, bool imperative=false);
			void updateSettings(void);
			void transferSourceCompilation(void);	

		public slots :
			void addTab(void);
			void open(QStringList filenameList=QStringList(), QVector<int> lineNumberList=QVector<int>());
			void open(const QString& filename, int lineNumber=0);
			void save(void);
			void saveAs(const QString& filename="");
			void saveAll(void);
			void compilationSuccessNotification(void* identifier);
			void compilationFailureNotification(void* identifier, Glip::Exception compilationError);
			void closeTab(void);
			void closeAll(void);

		signals :
			void showEditorSettingsInterface(void);
			void compileSource(std::string source, std::string path, std::string sourceName, void* identifier, const QObject* referrer);
	};
*/
}

#endif

