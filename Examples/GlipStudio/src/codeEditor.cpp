#include <cmath>
#include "codeEditor.hpp"
#include "keywords.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// Highlighter
	Highlighter::Highlighter(QTextDocument *parent)
	 : QSyntaxHighlighter(parent)
	{
		HighlightingRule rule;

		QFontDatabase db;
		QFont keyFont = db.font("Source Code Pro", "Bold", glslkeywordFormat.font().pointSize());

		// GLSL Keywords :
			glslkeywordFormat.setForeground(QColor(255,128,0));
			//glslkeywordFormat.setFontWeight(QFont::Bold);
			glslkeywordFormat.setFont(keyFont);
			QStringList glslkeywordPatterns;

			for(int i=0; i<GLSL_KW_END; i++)
			{
				std::string str = std::string("\\b") + GLSLKeyword[i] + "\\b";
				glslkeywordPatterns << str.c_str();
			}


			foreach(const QString& pattern, glslkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glslkeywordFormat;
				highlightingRules.append(rule);
			}

		// GLSL Functions : 
			glslfunctionFormat.setForeground(QColor(255,128,0));
			glslfunctionFormat.setFont(keyFont);
			QStringList glslfunctionPatterns;

			for(int i=0; i<GLSL_FN_END; i++)
			{
				std::string str = std::string("\\b") + GLSLFunction[i] + "\\b";
				glslfunctionPatterns << str.c_str();
			}


			foreach(const QString& pattern, glslfunctionPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glslfunctionFormat;
				highlightingRules.append(rule);
			}

		// GLIP LayoutLoader Keywords : 
			glipLayoutLoaderKeywordFormat.setForeground(QColor(255, 51, 255));
			//glipLayoutLoaderKeywordFormat.setFontWeight(QFont::Bold);
			glipLayoutLoaderKeywordFormat.setFont(keyFont);
			QStringList glipllkeywordPatterns;

			for(int i=0; i<Glip::Modules::LL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + Glip::Modules::keywordsLayoutLoader[i] + "\\b";
				glipllkeywordPatterns << str.c_str();
			}

			foreach(const QString& pattern, glipllkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glipLayoutLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// GLIP Uniform Loader Keywords : 
			glipUniformLoaderKeywordFormat.setForeground(QColor(51, 255, 255));
			//glipUniformLoaderKeywordFormat.setFontWeight(QFont::Bold);
			glipLayoutLoaderKeywordFormat.setFont(keyFont);
			QStringList glipulkeywordPatterns;

			for(int i=0; i<Glip::Modules::UL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + Glip::Modules::keywordsUniformsVarsLoader[i] + "\\b";
				glipulkeywordPatterns << str.c_str();
			}

			foreach(const QString& pattern, glipulkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glipUniformLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// Class : 
			/*classFormat.setFontWeight(QFont::Bold);
			classFormat.setForeground(Qt::darkMagenta);
			rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
			rule.format = classFormat;
			highlightingRules.append(rule);*/

		// Single line comment : 
			singleLineCommentFormat.setForeground(QColor(51,153,255));
			rule.pattern = QRegExp("//[^\n]*");
			rule.format = singleLineCommentFormat;
			highlightingRules.append(rule);

		// Mult line comment : 
			multiLineCommentFormat.setForeground(QColor(51,153,255));

		// Quotation ; 
			/*quotationFormat.setForeground(QColor(51,255,51));
			rule.pattern = QRegExp("\".*\"");
			rule.format = quotationFormat;
			highlightingRules.append(rule);*/

		// function : 
			/*functionFormat.setFontItalic(true);
			functionFormat.setForeground(Qt::blue);
			rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
			rule.format = functionFormat;
			highlightingRules.append(rule);*/

		
		commentStartExpression = QRegExp("/\\*");
		commentEndExpression = QRegExp("\\*/");
	}

	void Highlighter::highlightBlock(const QString &text)
	{
		foreach(const HighlightingRule &rule, highlightingRules)
		{
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while(index >= 0)
			{
				int length = expression.matchedLength();
				setFormat(index, length, rule.format);
				index = expression.indexIn(text, index + length);
			}
		}
		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
			startIndex = commentStartExpression.indexIn(text);

		while(startIndex >= 0)
		{
			int endIndex = commentEndExpression.indexIn(text, startIndex);
			int commentLength;
			if(endIndex == -1)
			{
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			}
			else
				commentLength = endIndex - startIndex + commentEndExpression.matchedLength();

			setFormat(startIndex, commentLength, multiLineCommentFormat);
			startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
		}
	}

// CodeEditor
	CodeEditor::CodeEditor(QWidget *parent)
	 : QPlainTextEdit(parent), currentFilename(""), firstModification(true), documentModified(false)
	{
		lineNumberArea = new LineNumberArea(this);

		connect(this, SIGNAL(blockCountChanged(int)), 	this, SLOT(updateLineNumberAreaWidth(int)));
		connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
		connect(this, SIGNAL(cursorPositionChanged()),	this, SLOT(highlightCurrentLine()));
		connect(document(), SIGNAL(contentsChanged()), 	this, SLOT(documentWasModified()));

		updateLineNumberAreaWidth(0);
	
		// Set the font : 
		int currentFontSize = document()->defaultFont().pointSize(); 
		//font.setFamily("Monospace");
		//font.setFixedPitch(true);
		//font.setPointSize(currentFontSize);
		QFontDatabase db;
		font = db.font("Source Code Pro", "Regular", currentFontSize);
		font.setFixedPitch(true);
		setFont(font);	

		// Set the tabulation length :
		const int tabStop = 8;
		QFontMetrics metrics(font);
		setTabStopWidth(tabStop * metrics.width(' '));

		highLighter = new Highlighter(document());
	}

	CodeEditor::~CodeEditor(void)
	{
		blockSignals(true);

		Highlighter 	*tmp1 = highLighter;
		QWidget 	*tmp2 = lineNumberArea;

		highLighter	= NULL;
		lineNumberArea	= NULL;

		delete tmp1;
		delete tmp2;
	}

	int CodeEditor::lineNumberAreaWidth(void) const
	{
		int digits = std::floor( std::log10( qMax(1, blockCount()) ) ) + 1;

		//int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
		int space = 3 + fontMetrics().width( '0' ) * digits;

		return space;
	}

	void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
	{
		this->setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
	}

	void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
	{
		if(lineNumberArea==NULL)
			return ;

		if(dy)
			lineNumberArea->scroll(0, dy);
		else
			lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

		if(rect.contains(viewport()->rect()))
			updateLineNumberAreaWidth(0);
	}

	void CodeEditor::documentWasModified(void)
	{
		if(firstModification)
			firstModification = false;
		else if(!documentModified)
		{
			documentModified = true;
			emit titleChanged();
		}
	}
	
	void CodeEditor::resizeEvent(QResizeEvent *e)
	{
		QPlainTextEdit::resizeEvent(e);

		QRect cr = contentsRect();
		lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
	}

	void CodeEditor::keyPressEvent(QKeyEvent* e)
	{
		// Ensure multi-line tabulations : 
		if(e->key()==Qt::Key_Tab || e->key()==Qt::Key_Backtab)
		{
			// Grab all the current selection : 
			QString selectedText = textCursor().selectedText();

			// Replace unicode : 
			selectedText.replace(QString::fromWCharArray(L"\u2029"), "\n");

			QTextCursor cursor = textCursor();

			int	startSel = cursor.selectionStart(), 
				endSel = cursor.selectionEnd();

			// If no selection, process the tab / backtab as usual : 
			if(endSel-startSel<=0 && e->key()==Qt::Key_Tab)
			{
				QPlainTextEdit::keyPressEvent(e);
				return ;
			}
			else if(endSel-startSel<=0 && e->key()==Qt::Key_Backtab)
			{
				cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
				cursor.insertText( cursor.selectedText().replace("\t","") );
				return ;
			}

			cursor.setPosition(endSel, QTextCursor::KeepAnchor);
			QTextBlock endBlock = cursor.block();

			cursor.setPosition(startSel, QTextCursor::KeepAnchor);
			QTextBlock block = cursor.block();

			for(; block.isValid() && !(endBlock < block); block = block.next())
			{
				if (!block.isValid())
					continue;

				cursor.movePosition(QTextCursor::StartOfLine);
				cursor.clearSelection();

				if(e->key()==Qt::Key_Tab)
					cursor.insertText("\t");
				else
				{
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
					cursor.insertText( cursor.selectedText().replace("\t","") );
				}
				
				cursor.movePosition(QTextCursor::NextBlock);
			}
		}
		else // Otherwise, propagate : 
			QPlainTextEdit::keyPressEvent(e);
	}

	void CodeEditor::highlightCurrentLine(void)
	{
		QList<QTextEdit::ExtraSelection> extraSelections;

		if (!isReadOnly())
		{
			QTextEdit::ExtraSelection selection;

			QColor lineColor = palette().background().color().lighter(130);			

			selection.format.setBackground(lineColor);
			selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			selection.cursor = textCursor();
			selection.cursor.clearSelection();
			extraSelections.append(selection);
		}

		setExtraSelections(extraSelections);
	}

	void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
	{
		QPainter 	painter(lineNumberArea);
		QColor 		bg = palette().background().color().lighter(130),
				ed = palette().text().color().lighter(70);

		painter.fillRect(event->rect(), bg);

		QTextBlock 	block 		= firstVisibleBlock();
		int 		blockNumber 	= block.blockNumber();
		int 		top 		= (int) blockBoundingGeometry(block).translated(contentOffset()).top();
		int 		bottom 		= top + (int) blockBoundingRect(block).height();

		QFontMetrics metrics(font);

		while (block.isValid() && top <= event->rect().bottom())
		{
			if (block.isVisible() && bottom >= event->rect().top())
			{
				QString number = QString::number(blockNumber + 1);
				painter.setPen(ed);
				//painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
				painter.drawText(0, top, lineNumberArea->width(), metrics.height(), Qt::AlignRight | Qt::AlignVCenter, number);
			}

			block = block.next();
			top = bottom;
			bottom = top + (int) blockBoundingRect(block).height();
			++blockNumber;
		}
	}

	bool CodeEditor::empty(void) const
	{
		return document()->isEmpty();
	}

	const QString& CodeEditor::filename(void) const
	{
		return currentFilename;
	}

	QString CodeEditor::path(void) const
	{
		if(filename().isEmpty())
			return "";
		else
		{
			QFileInfo path(filename());
			return path.path() + "/";
		}
	}

	QString CodeEditor::getTitle(void) const
	{
		QFileInfo path(filename());
		QString fileName;
		
		if(path.exists())
			fileName = path.fileName();
		else
			fileName = "Unnamed.ppl";

		if( documentModified )
			return fileName + " *";
		else
			return fileName;
	}
	
	std::string CodeEditor::currentContent(void) const
	{
		return toPlainText().toStdString();
	}

	bool CodeEditor::isModified(void) const
	{
		return documentModified;
	}

	bool CodeEditor::canBeClosed(void)
	{
		if(!documentModified)
			return true;
		else
		{
			QMessageBox::StandardButton ret;

			if(filename().isEmpty())
				ret = QMessageBox::warning(this, tr("Warning!"), tr("New file has been modified.\n Do you want to save your changes?"), QMessageBox::Discard | QMessageBox::Cancel);
			else
				ret = QMessageBox::warning(this, tr("Warning!"), tr("The file %1 has been modified.\n Do you want to save your changes?").arg(filename()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

			if (ret == QMessageBox::Save)
				return save();
			else if (ret == QMessageBox::Cancel)
				return false;
			else
				return true;
		}
	}

	void CodeEditor::setFilename(const QString& newFilename)
	{
		currentFilename = newFilename;
	}

	bool CodeEditor::load(void)
	{
		if(filename().isEmpty())
			return false;
		else
		{
			QFile file(filename());

			if (!file.open(QFile::ReadOnly | QFile::Text))
			{
				QMessageBox::warning(this, tr("Error : "), tr("Cannot read file %1 :\n%2.").arg(filename()).arg(file.errorString()));
				return false;
			}

			QTextStream in(&file);
			#ifndef QT_NO_CURSOR
				QApplication::setOverrideCursor(Qt::WaitCursor);
			#endif

			// Prevent to understand a real modification :
			bool prevState = this->blockSignals(true);

			setPlainText(in.readAll());

			// Force the size of the margin :
			setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);

			this->blockSignals(prevState);

			#ifndef QT_NO_CURSOR
				QApplication::restoreOverrideCursor();
			#endif

			documentModified = false;
			emit titleChanged();

			return true;
		}
	}

	bool CodeEditor::save(void)
	{
		QFile file(filename());

		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			QMessageBox::warning(this, tr("Error : "),tr("Cannot write file %1 :\n%2.").arg(filename()).arg(file.errorString()));
			return false;
		}

		QTextStream out(&file);

		#ifndef QT_NO_CURSOR
			QApplication::setOverrideCursor(Qt::WaitCursor);
		#endif

		out << toPlainText();

		#ifndef QT_NO_CURSOR
			QApplication::restoreOverrideCursor();
		#endif

		documentModified = false;

		emit titleChanged();

		return true;
	}

	void CodeEditor::insert(const QString& text)
	{
		textCursor().insertText(text);
	}

// LineNumberArea 
	LineNumberArea::LineNumberArea(CodeEditor *editor)
	 : QWidget(editor), codeEditor(editor)
	{ }

	void LineNumberArea::paintEvent(QPaintEvent *event)
	{
		codeEditor->lineNumberAreaPaintEvent(event);
	}

	QSize LineNumberArea::sizeHint() const
	{
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

