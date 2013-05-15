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

		private:
			QString 	currentFilename;
			QFont 		font;
			Highlighter 	*highLighter;
			QWidget 	*lineNumberArea;
			bool 		modified;

			bool maybeSave(void);
			bool loadFile(const QString& fileName);
			bool saveFile(const QString& fileName);
			void setCurrentFile(const QString &fileName);
			void ensureOneLine(void);

		protected:
			void resizeEvent(QResizeEvent *event);

			private slots:
			void updateLineNumberAreaWidth(int newBlockCount);
			void highlightCurrentLine();
			void updateLineNumberArea(const QRect &, int);

		private slots :
			void documentWasModified(void);

		public slots:
			bool open(void);
			bool save(void);
			bool saveAs(void);

		public:
			CodeEditor(QWidget *parent = 0);
			~CodeEditor(void);

			void lineNumberAreaPaintEvent(QPaintEvent *event);
			int lineNumberAreaWidth();

			bool canBeClosed(void);
			QString getTitle(void) const;
			std::string getCode(void) const;
			bool openFile(const QString& filename);

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

	class CodeEditorsPannel : public QWidget
	{
		Q_OBJECT
 		
		private : 
			QVBoxLayout		layout;
			QTabWidget		widgets;
			QMenuBar 		menuBar;
			QAction 		newTabAct,
						saveAct,
						saveAsAct,
						openAct,
						refreshAct,
						closeTabAct;
			QVector<CodeEditor*>	tabs;

		private slots : 	
			void newTab(void);
			void open(void);
			void save(void);
			void saveAs(void);
			void refresh(void);
			void closeTab(void);
			void updateTitles(void);

		public :
			CodeEditorsPannel(QWidget* parent);
			~CodeEditorsPannel(void);

			std::string getCurrentCode(void) const;
			bool canBeClosed(void);

		signals : 
			void requireRefresh(void);
	};

#endif
