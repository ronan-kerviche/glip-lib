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
					glipLayoutLoaderKeywordFormat,
					glipUniformLoaderKeywordFormat,
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
			void highlightCurrentLine(void);
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

#endif
