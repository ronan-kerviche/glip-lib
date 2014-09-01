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
	#define __USE_QVGL__

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

	#include <QHBoxLayout>
	#include <QTabBar>
	#include <QStackedLayout>
	#include <QToolButton>
	#include <QFileDialog>
	#include <QLineEdit>
	#include <QWidgetAction>

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
					multiLineCommentFormat,
					searchExpressionFormat;
					//quotationFormat,
					//classFormat,
					//functionFormat;

		protected:
			void highlightBlock(const QString &text);

		public:
			Highlighter(QTextDocument *parent = 0);

			void updateSettings(const CodeEditorSettings& settings);
			void setSearchHighlight(const QRegExp& expression);
			void clearSearchHighlight(void);
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
			bool		highlightLine;
			QString 	currentFilename;
			QFont		font;
			Highlighter 	*highLighter;
			QWidget 	*lineNumberArea;
			QList<QMenu*>	subMenus;

		protected :
			int lineNumberAreaWidth(void) const;
			void lineNumberAreaPaintEvent(QPaintEvent *event);
			void resizeEvent(QResizeEvent *event);
			void keyPressEvent(QKeyEvent* e);
			void contextMenuEvent(QContextMenuEvent* event);

			friend class LineNumberArea;

		private slots:
			void updateLineNumberAreaWidth(int newBlockCount);
			void highlightCurrentLine(void);
			void clearHighlightOfCurrentLine(void);
			void updateLineNumberArea(const QRect &, int);
			
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

			void search(QRegExp expression, QTextDocument::FindFlags flags);
			void replace(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void replaceAll(QRegExp expression, QTextDocument::FindFlags flags, QString text);
			void clearSearch(void);

		signals :
			void titleChanged(void);
			void modified(bool changed);
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

		private slots : 
			void prepareExpression(void);
			void openMenu(void);

		public :
			SearchAndReplaceMenu(QWidget* parent=NULL);
			~SearchAndReplaceMenu(void);

			QAction* getAction(void);

		signals :
			void search(QRegExp expression, QTextDocument::FindFlags flags);
			void replace(QRegExp expression, QTextDocument::FindFlags flags, QString replacement);
			void replaceAll(QRegExp expression, QTextDocument::FindFlags flags, QString replacement);
			void clearSearchHighlight(void);
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
						searchColor;
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
						commentsColorLabel,
						searchColorLabel;
			QPushButton		glslKeywordColorButton,
						glslFunctionColorButton,
						glipLayoutLoaderKeywordColorButton,
						glipUniformLoaderKeywordColorButton,
						commentsColorButton,
						searchColorButton,
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
			const QColor& getSearchColor(void) const;
			const QFont& getEditorFont(void) const;
			const QFont& getKeywordFont(void) const;
			const QTextOption::WrapMode& getWrapMode(void) const;
			const int& getNumberOfSpacesPerTabulation(void) const;
			const bool& isHighlightEnabled(void) const;
			const bool& isLineHighlightEnabled(void) const;
		
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

	class MainWidget : public QWidget
	{
		Q_OBJECT

		private : 
			QString			currentPath;
			QVBoxLayout		layout;
			QHBoxLayout		topBar;
			QMenuBar		menuBarLeft,
						menuBarRight;
			QMenu			mainMenu,
						searchContainerMenu;
			QTabBar			tabBar;
			QStackedLayout		stack;
			QMap<int, CodeEditor*>	editors;
			QAction			newAction,
						openAction,	
						saveAction,
						saveAsAction,
						saveAllAction,
						closeAction,
						closeAllAction,
						settingsAction,
						compileAction;
			TemplateMenu		templateMenu;
			ElementsMenu		elementsMenu;
			CodeEditorSettings	settings;
			SearchAndReplaceMenu	searchAndReplaceMenu;
			RecentFilesMenu		recentFilesMenu;

			CodeEditor* getCurrentEditor(void);
			int getTabIndex(CodeEditor* editor);
			void setCurrentPath(QString path);
			void save(CodeEditor* editor);
			void saveAs(CodeEditor* editor, QString filename="");
			void wheelEvent(QWheelEvent* event);

		protected :
			virtual LayoutLoader::PipelineScriptElements scanSource(const std::string& code) const;

		public :
			MainWidget(void);
			~MainWidget(void);

		private slots :
			void tabTitleChanged(void);
			void documentModified(bool changed);
			void insert(QString str);
			void updateElements(void);
			void changeToTab(int tabID);
			void closeTab(int tabID, bool imperative=false);
			void updateSettings(void);
			void transferSourceCompilation(void);
			void transferSearchRequest(QRegExp expression, QTextDocument::FindFlags flags);
			void transferReplaceRequest(QRegExp expression, QTextDocument::FindFlags flags, QString replacement);
			void transferReplaceAllRequest(QRegExp expression, QTextDocument::FindFlags flags, QString replacement);
			void transferClearSearchRequest(void);

		public slots :
			void addTab(const QString& filename="");
			void open(QStringList filenameList = QStringList());
			void open(const QString& filename);
			void save(void);
			void saveAs(const QString& filename="");
			void saveAll(void);
			void closeTab(void);
			void closeAll(void);

		signals :
			void compileSource(std::string source, void* identifier, const QObject* referrer, const char* notificationMember);
	};

	#ifdef __USE_QVGL__
	class CodeEditorSubWidget : public QVGL::SubWidget
	{	
		private : 
			MainWidget mainWidget;

		public :
			CodeEditorSubWidget(void);
			~CodeEditorSubWidget(void);
	};
	#endif
}

#endif

