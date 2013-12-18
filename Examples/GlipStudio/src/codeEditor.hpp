#ifndef __GLIPSTUDIO_CODE_EDITOR__
#define __GLIPSTUDIO_CODE_EDITOR__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
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

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

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
					highlightLine;
			QString 	currentFilename;
			QFont		font;
			Highlighter 	*highLighter;
			QWidget 	*lineNumberArea;

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
			QString getTitle(void) const;
			std::string currentContent(void) const;
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

	class CodeEditorSettings : public QWidget
	{
		Q_OBJECT

		private :
			static CodeEditorSettings* 	singleton;
			static const std::string	moduleName;

			// Data : 
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

#endif
