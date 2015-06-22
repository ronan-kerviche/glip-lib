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

#include "CodeEditor.hpp"
#include "GLSLKeywords.hpp"
#include "QMenuTools.hpp"
#include <QFontDatabase>
#include <QHeaderView>
#include <QApplication>

using namespace QGED;

// Highlighter
	Highlighter::Highlighter(QTextDocument *parent)
	 : 	QSyntaxHighlighter(parent), 
		highlightEnabled(true)
	{
		HighlightingRule rule;

		// GLSL Keywords :
			for(int i=0; i<GLSLLanguage::KW_END; i++)
			{
				rule.pattern = QRegExp(tr("\\b%1\\b").arg(GLSLLanguage::GLSLKeywords[i]));
				rule.format = &glslKeywordFormat;
				highlightingRules.append(rule);
			}

		// GLSL Functions : 	
			for(int i=0; i<GLSLLanguage::FN_END; i++)
			{
				rule.pattern = QRegExp(tr("\\b%1\\b").arg(GLSLLanguage::GLSLFunctions[i]));
				rule.format = &glslFunctionFormat;
				highlightingRules.append(rule);
			}

		// GLSL Defines :
			for(int i=0; i<GLSLLanguage::PP_END; i++)
			{
				QString pattern = tr("%1\\b").arg(GLSLLanguage::GLSLPreprocessor[i]);
				pattern.replace("#", "#\\b"); // Need to make the sharp sign outside of the word boundary '\b'
				rule.pattern = QRegExp(pattern);
				rule.format = &glslPreprocessorFormat;
				highlightingRules.append(rule);
			}

		// GLIP LayoutLoader Keywords : 	
			for(int i=0; i<Glip::Modules::LL_NumKeywords; i++)
			{
				rule.pattern = QRegExp(tr("\\b%1\\b").arg(LayoutLoader::getKeyword(static_cast<Glip::Modules::LayoutLoaderKeyword>(i))));
				rule.format = &glipLayoutLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// GLIP Uniform Loader Keywords : 
			for(int i=0; i<Glip::Modules::UL_NumKeywords; i++)
			{
				rule.pattern = QRegExp(tr("\\b%1\\b").arg(UniformsLoader::getKeyword(static_cast<UniformsLoaderKeyword>(i))));
				rule.format = &glipUniformLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// Class : 
			/*classFormat.setFontWeight(QFont::Bold);
			classFormat.setForeground(Qt::darkMagenta);
			rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
			rule.format = classFormat;
			highlightingRules.append(rule);*/

		// Single line comment : 
			//singleLineCommentFormat.setForeground(QColor(51,153,255));
			rule.pattern = QRegExp("//[^\n]*");
			rule.format = &singleLineCommentFormat;
			highlightingRules.append(rule);

		// Mult line comment : 
			//multiLineCommentFormat.setForeground(QColor(51,153,255));
			commentStartExpression = QRegExp("/\\*");
			commentEndExpression = QRegExp("\\*/");
	
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
	}

	Highlighter::~Highlighter(void)
	{
		setDocument(NULL);
	}

	void Highlighter::highlightBlock(const QString &text)
	{
		if( highlightEnabled )
		{
			foreach(const HighlightingRule &rule, highlightingRules)
			{
				QRegExp expression(rule.pattern);
				int index = expression.indexIn(text);
				while(index>=0)
				{
					int length = expression.matchedLength();
					setFormat(index, length, *rule.format);
					index = expression.indexIn(text, index + length);
				}
			}
			setCurrentBlockState(0);

			// Multi-line comment : 
			int startIndex = 0;
			if (previousBlockState() != 1)
				startIndex = commentStartExpression.indexIn(text);

			while(startIndex>=0)
			{
				int endIndex = commentEndExpression.indexIn(text, startIndex);
				int commentLength;
				if(endIndex==-1)
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
	}

	QTextCharFormat Highlighter::format(int position) const
	{
		return QSyntaxHighlighter::format(position);
	}

	void Highlighter::updateSettings(const CodeEditorSettings& settings)
	{
		// Color : 
		glslKeywordFormat.setForeground(		settings.glslKeywordColor );
		glslFunctionFormat.setForeground(		settings.glslFunctionColor );
		glslPreprocessorFormat.setForeground(		settings.glslPreprocessorColor );
		glipLayoutLoaderKeywordFormat.setForeground(	settings.glipLayoutLoaderKeywordColor );
		glipUniformLoaderKeywordFormat.setForeground(	settings.glipUniformLoaderKeywordColor );
		singleLineCommentFormat.setForeground(		settings.commentsColor );
		multiLineCommentFormat.setForeground(		settings.commentsColor );

		// Font : 
		glslKeywordFormat.setFont(			settings.keywordFont );
		glslFunctionFormat.setFont(			settings.keywordFont );
		glslPreprocessorFormat.setFont(			settings.keywordFont );	
		glipLayoutLoaderKeywordFormat.setFont(		settings.keywordFont );
		glipUniformLoaderKeywordFormat.setFont(		settings.keywordFont );
		singleLineCommentFormat.setFont(		settings.keywordFont );
		multiLineCommentFormat.setFont(			settings.keywordFont );

		// General highlighting (only if highlight was allowed for this editor) :
		highlightEnabled = settings.enableHighlight;

		// Update : 
		rehighlight();
	}	

// LineNumberArea 
	LineNumberArea::LineNumberArea(CodeEditor *editor)
	 : 	QWidget(editor), 
		codeEditor(editor)
	{ }

	LineNumberArea::~LineNumberArea(void)
	{ }

	void LineNumberArea::paintEvent(QPaintEvent *event)
	{
		codeEditor->lineNumberAreaPaintEvent(event);
	}

	QSize LineNumberArea::sizeHint() const
	{
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

// CodeEditor
	CodeEditor::CodeEditor(QWidget *parent)
	 : 	QPlainTextEdit(parent),
		highlightLine(false),
		braceMatching(false),
		currentFilename(""),
		highLighter(NULL),
		lineNumberArea(NULL)
	{
		lineNumberArea = new LineNumberArea(this);
		updateLineNumberAreaWidth(0);

		highLighter = new Highlighter(document());

		QObject::connect(this, 		SIGNAL(blockCountChanged(int)), 	this, SLOT(updateLineNumberAreaWidth(int)));
		QObject::connect(this,		SIGNAL(updateRequest(QRect,int)), 	this, SLOT(updateLineNumberArea(QRect,int)));
		QObject::connect(this,		SIGNAL(cursorPositionChanged()),	this, SLOT(updateToCursorPosition()));
		QObject::connect(document(),	SIGNAL(contentsChanged()),		this, SLOT(updateSearchExtraSelection(void)));
		QObject::connect(document(), 	SIGNAL(modificationChanged(bool)), 	this, SIGNAL(modified(bool)));
	}

	CodeEditor::~CodeEditor(void)
	{
		highLighter->deleteLater();
		lineNumberArea->deleteLater();
	}	

	int CodeEditor::lineNumberAreaWidth(void) const
	{
		int digits = std::floor( std::log10( qMax(1, blockCount()) ) ) + 1;

		//int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
		int space = 3 + fontMetrics().width('0') * digits;

		return space;
	}

	void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
	{
		QPainter 	painter(lineNumberArea);
		QColor 		bg = palette().background().color().lighter(130),
				ed = palette().text().color().lighter(70);

		painter.fillRect(event->rect(), bg);

		const int	pixelOffset	= -1;
		QTextBlock 	block 		= firstVisibleBlock();
		int 		blockNumber 	= block.blockNumber();
		int 		top 		= (int) blockBoundingGeometry(block).translated(contentOffset()).top();
		int 		bottom 		= top + (int) blockBoundingRect(block).height();

		painter.setPen(ed);
		QFont 	painterFont	= painter.font();
		QPen 	painterPen	= painter.pen();

		QFontMetrics metrics(painterFont);

		while(block.isValid() && top<=event->rect().bottom())
		{
			if(block.isVisible() && bottom>=event->rect().top())
			{
				const bool currentLine = (block==textCursor().block());
				bool reset = false;				

				// This the current line, change the line number representation :
				if(currentLine)
				{	
					QFont tmpFont = painterFont;
					tmpFont.setBold(true);
					QPen tmpPen = painterPen;
					tmpPen.setColor(Qt::white);

					painter.setFont(tmpFont);
					painter.setPen(tmpPen);

					reset = true;
				}

				// Test if this is a line with some error :
				if(!errorLineNumbers.isEmpty() && errorLineNumbers.contains(blockNumber+1))
				{
					QFont tmpFont = painterFont;
					tmpFont.setBold(true);	
					QPen tmpPen = painterPen;
					tmpPen.setColor(Qt::white);

					painter.setFont(tmpFont);
					painter.setPen(tmpPen);

					// Red rectangle :
					painter.fillRect(0, top-pixelOffset, lineNumberArea->width(), metrics.height(), Qt::red);

					reset = true;
				}

				QString number = QString::number(blockNumber + 1);
				//painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
				painter.drawText(0, top-pixelOffset, lineNumberArea->width(), metrics.height(), Qt::AlignRight | Qt::AlignVCenter, number);

				if(reset)
				{
					// Restore :
					painter.setFont(painterFont);
					painter.setPen(painterPen);
				}
			}

			block = block.next();
			top = bottom;
			bottom = top + (int) blockBoundingRect(block).height();
			++blockNumber;
		}
	}

	void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
	{
		UNUSED_PARAMETER(newBlockCount)
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

			// If the selection contains only a single line :
			if(!selectedText.contains('\n'))
			{
				QPlainTextEdit::keyPressEvent(e);
				return ;
			}

			// Get the selection :
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

			cursor.beginEditBlock();
			for(; block.isValid() && !(endBlock < block); block = block.next())
			{
				if(!block.isValid())
					continue;

				cursor.movePosition(QTextCursor::StartOfLine);
				cursor.clearSelection();

				if(e->key()==Qt::Key_Tab)
					cursor.insertText("\t");
				else
				{
					// Select the equivalent of one tab.
					QFontMetrics metrics(document()->defaultFont());
					const int tabSpacesEquivalent = tabStopWidth()/metrics.width(' ');
					int numSpaces = 0;
					
					// Scan the beginning of the line, to remove UP TO one tabulation :
					QChar c = document()->characterAt(cursor.position());
					while(c.isSpace() && numSpaces<tabSpacesEquivalent)
					{
						if(c=='\t' || c=='\n')
						{
							cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
							break;
						}
						else
							numSpaces++;

						cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
						c = document()->characterAt(cursor.position());
					}

					cursor.removeSelectedText();
				}
				
				cursor.movePosition(QTextCursor::NextBlock);
			}
			cursor.endEditBlock();
		}
		else // Otherwise, propagate : 
			QPlainTextEdit::keyPressEvent(e);
	}	

	void CodeEditor::contextMenuEvent(QContextMenuEvent* event)
	{
		QMenu* menu = createStandardContextMenu();
		
		menu->addSeparator();

		for(int k=0; k<subMenus.count(); k++)
			duplicateMenu(menu, *subMenus[k]);

		menu->exec(event->globalPos());
		menu->deleteLater();
	}

	void CodeEditor::matchBraces(QTextCursor& current, bool& acceptableMatch, QTextCursor& result)
	{
		const QString	openChar 	= "([{",
				closeChar	= ")]}";
		int 		counter[3]	= {0, 0, 0};

		acceptableMatch	= false;
		result		= QTextCursor();
		
		if(current.isNull() || current.document()==NULL || !(current.selectionStart()==current.position() && current.selectionEnd()==current.position())) // And do not match when selecting text
			return ;

		QChar character = current.document()->characterAt(current.position());
		
		int 	indexOpen 	= openChar.indexOf(character),
			indexClose 	= closeChar.indexOf(character);

		// Test the previous character in case of failure : 
		if(indexOpen<0 && indexClose<0)
		{
			current.movePosition(QTextCursor::PreviousCharacter);

			character = current.document()->characterAt(current.position());

			indexOpen 	= openChar.indexOf(character),
			indexClose 	= closeChar.indexOf(character);
		}
		
		if(indexOpen>=0)
		{
			acceptableMatch	= true;

			result = current;
			result.movePosition(QTextCursor::NextCharacter);
			while(!result.isNull())
			{
				const QChar currentChar = current.document()->characterAt(result.position());				
				int 	currentIndexOpen  = openChar.indexOf(currentChar),
					currentIndexClose = closeChar.indexOf(currentChar);

				if(currentIndexOpen>=0)
					counter[currentIndexOpen]++;
				else if(currentIndexClose>=0)
					counter[currentIndexClose]--;

				if(counter[indexOpen]<0)
					break ;
				else
				{
					if(!result.movePosition(QTextCursor::NextCharacter))
					{
						result = QTextCursor();
						break ;
					}
				}
			}
		}
		else if(indexClose>=0)
		{
			acceptableMatch	= true;

			result = current;
			result.movePosition(QTextCursor::PreviousCharacter);
			while(!result.isNull())
			{
				const QChar currentChar = current.document()->characterAt(result.position());				
				int 	currentIndexOpen  = openChar.indexOf(currentChar),
					currentIndexClose = closeChar.indexOf(currentChar);

				if(currentIndexOpen>=0)
					counter[currentIndexOpen]--;
				else if(currentIndexClose>=0)
					counter[currentIndexClose]++;

				if(counter[indexClose]<0)
					break ;
				else
				{
					if(!result.movePosition(QTextCursor::PreviousCharacter))
					{
						result = QTextCursor();
						break ;
					}
				}
			}
		}
		// else, default error

		if(!current.isNull())
			current.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

		if(!result.isNull())
			result.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
	}

	void CodeEditor::updateBracesMatchExtraSelection(void)
	{
		bracesMatchExtraSelections.clear();

		if(!braceMatching)
			return ;

		bool acceptableMatch = false;
		QTextCursor currentTextCursor = textCursor();
		QTextCursor result = QTextCursor();

		matchBraces(currentTextCursor, acceptableMatch, result);

		if(acceptableMatch)
		{
			if(!result.isNull())
			{
				QTextEdit::ExtraSelection selectionBegin;

				selectionBegin.format.setBackground(braceMatchingColor);
				selectionBegin.cursor = currentTextCursor;
				bracesMatchExtraSelections.append(selectionBegin);
		
				QTextEdit::ExtraSelection selectionEnd;

				selectionEnd.format.setBackground(braceMatchingColor);
				selectionEnd.cursor = result;
				bracesMatchExtraSelections.append(selectionEnd);
			}
			else
			{
				QTextEdit::ExtraSelection selection;

				selection.format.setBackground(Qt::red);
				selection.cursor = currentTextCursor;
				bracesMatchExtraSelections.append(selection);
			}
		}
	}

	void CodeEditor::updateSearchExtraSelection(void)
	{
		searchExtraSelections.clear();
	
		if(!searchExpression.isEmpty())
		{
			for(QTextBlock block=document()->begin(); block!=document()->end(); block=block.next()) 
			{
				const QString text = block.text();
				int index = searchExpression.indexIn(text);

				QTextCursor cursor(block);

				while(index>=0)
				{
					int length = searchExpression.matchedLength();
					cursor.setPosition(block.position()+index, QTextCursor::MoveAnchor);
					cursor.setPosition(block.position()+index+length, QTextCursor::KeepAnchor);

					// Append :
					QTextEdit::ExtraSelection selection;
					selection.format.setBackground(searchHighlightColor);
					selection.cursor = cursor;
					searchExtraSelections.append(selection);

					// Next :
					index = searchExpression.indexIn(text, index + length);	
				}
			}

			// Update the highlighting :
			if(!searchExtraSelections.isEmpty())
				updateExtraHighlight();		
		}
	}	

	void CodeEditor::updateExtraHighlight(void)
	{
		if(!isReadOnly())
		{
			const QColor regularBackground = palette().background().color();
			const QColor lineColor = regularBackground.lighter(150);

			QList<QTextEdit::ExtraSelection> extraSelections;
	
			// Add current line highlight :
			if(highlightLine)
			{
				QTextEdit::ExtraSelection selection;
				selection.format.setBackground(lineColor);
				selection.format.setProperty(QTextFormat::FullWidthSelection, true);
				selection.cursor = textCursor();
				selection.cursor.clearSelection();

				extraSelections.append(selection);
			}
	
			// Add the braces match and search extra selections :
			extraSelections.append(bracesMatchExtraSelections);
			extraSelections.append(searchExtraSelections);

			// Submit :
			setExtraSelections(extraSelections);
		}
	}

	void CodeEditor::updateToCursorPosition(void)
	{
		updateBracesMatchExtraSelection();
		updateExtraHighlight();
	}

	void CodeEditor::clearHighlightOfCurrentLine(void)
	{
		// Prevent clear, if it was not already in place.
		if(highlightLine && !isReadOnly())	
		{
			QList<QTextEdit::ExtraSelection> extraSelections;

			QTextEdit::ExtraSelection selection;

			const QColor regularBackground = palette().background().color();	
		
			selection.format.setBackground(regularBackground);
			selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			selection.cursor = textCursor();
			selection.cursor.clearSelection();
			extraSelections.append(selection);
		
			// Add the search extra selection :
			extraSelections.append(bracesMatchExtraSelections);
			extraSelections.append(searchExtraSelections);

			// Submit :
			setExtraSelections(extraSelections);
		}
	}

	bool CodeEditor::empty(void) const
	{
		return document()->isEmpty();
	}

	const QString& CodeEditor::getFilename(void) const
	{
		return currentFilename;
	}

	void CodeEditor::setFilename(const QString& newFilename)
	{
		currentFilename = newFilename;
		emit titleChanged();
	}

	QString CodeEditor::getPath(void) const
	{
		if(currentFilename.isEmpty())
			return "";
		else
		{
			QFileInfo path(currentFilename);
			return path.path();
		}
	}

	QString CodeEditor::getRawTitle(void) const
	{
		QFileInfo path(currentFilename);

		if(path.exists())
			return path.fileName();
		else
			return "<Unnamed>";
	}

	QString CodeEditor::getTitle(void) const
	{
		QFileInfo path(currentFilename);
		QString fileName;
		
		if(path.exists())
			fileName = path.fileName();
		else
			fileName = "Unnamed.ppl";

		if( isModified() )
			fileName += " *";

		return fileName;
	}
	
	std::string CodeEditor::getCurrentContent(void) const
	{
		return toPlainText().toStdString();
	}

	bool CodeEditor::isModified(void) const
	{
		return document()->isModified();
	}

	bool CodeEditor::canBeSaved(void) const
	{
		QFileInfo path(currentFilename);
		return path.isWritable();
	}

	void CodeEditor::open(QString newFilename)
	{
		if(newFilename.isEmpty())
			newFilename = currentFilename;

		QFile file(newFilename);

		if(!file.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox::warning(this, tr("Error : "), tr("Cannot read file %1 :\n%2.").arg(newFilename).arg(file.errorString()));
			return ;
		}

		QTextStream in(&file);

		// Prevent to understand a real modification :
		bool prevState = this->blockSignals(true);

		setPlainText(in.readAll());

		// Force the size of the margin :
		setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);

		this->blockSignals(prevState);

		// Set the filename : 
		setFilename(newFilename);
	}

	void CodeEditor::save(QString newFilename)
	{
		if(newFilename.isEmpty())
			newFilename = currentFilename;

		QFile file(newFilename);

		if(!file.open(QFile::WriteOnly | QFile::Text))
		{
			QMessageBox::warning(this, tr("Error : "),tr("Cannot write file %1 :\n%2.").arg(newFilename).arg(file.errorString()));
			return ;
		}

		// Write : 
		QTextStream out(&file);

		out << toPlainText();

		file.close();

		document()->setModified(false);

		// Set the filename : 
		setFilename(newFilename);
	}

	void CodeEditor::insert(const QString& text)
	{
		textCursor().insertText(text);
	}

	void CodeEditor::addSubMenu(QMenu* menu)
	{
		subMenus.append(menu);
	}

	void CodeEditor::updateSettings(const CodeEditorSettings& settings)
	{
		// Prevent the code from sending modification signal :
		blockSignals(true);
		document()->blockSignals(true);

		// Set the font : 
		setFont(settings.editorFont);
		document()->setDefaultFont(settings.editorFont);

		// Set the tabulation length :
		const int tabStop = settings.tabNumberOfSpaces;
		QFontMetrics metrics(settings.editorFont);
		setTabStopWidth(tabStop * metrics.width(' '));

		// Set word wrap : 
		setWordWrapMode(settings.wrapMode );

		// Set line highlight :
		highlightLine = settings.enableHighlight;

		if(!highlightLine)
			clearHighlightOfCurrentLine();

		// Set brace matching : 
		braceMatching = settings.braceMatching;

		if(!braceMatching)
			updateToCursorPosition();
		else
			braceMatchingColor = settings.braceMatchingColor;

		// Propagate : 
		if(highLighter!=NULL)
			highLighter->updateSettings(settings);

		// Set search highlight color :
		searchHighlightColor = settings.searchColor;

		document()->blockSignals(false);
		blockSignals(false);
	}

	void CodeEditor::gotoLine(int lineNumber)
	{
		QTextCursor cursor(document()->findBlockByLineNumber(lineNumber-1));
		setTextCursor(cursor);
	}

	void CodeEditor::search(QRegExp expression, QTextDocument::FindFlags flags)
	{
		searchExpression = expression;
		updateSearchExtraSelection();

		QTextCursor cursor = document()->find(expression, textCursor(), flags);
		
		// Not found, cycle around : 
		if(cursor.isNull() && (flags & QTextDocument::FindBackward)>0)
			cursor = document()->find(expression, document()->characterCount()-1, flags);
		else if(cursor.isNull())
			cursor = document()->find(expression, 0, flags);

		// Move : 
		if(!cursor.isNull())
			setTextCursor(cursor);
	}

	void CodeEditor::replace(QRegExp expression, QTextDocument::FindFlags flags, QString text)
	{
		searchExpression = expression;
		updateSearchExtraSelection();

		QTextCursor currentCursor = textCursor();
		currentCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		QTextCursor cursor = document()->find(expression, currentCursor, flags);

		// Not found, cycle around : 
		if(cursor.isNull() && (flags & QTextDocument::FindBackward)>0)
			cursor = document()->find(expression, document()->characterCount()-1, flags);
		else
			cursor = document()->find(expression, 0, flags);

		if(!cursor.isNull())
		{
			// Move : 
			setTextCursor(cursor);

			// Replace :
			cursor.removeSelectedText();
			cursor.insertText(text);
		}
	}

	void CodeEditor::replaceAll(QRegExp expression, QTextDocument::FindFlags flags, QString text)
	{
		// Always forward :
		flags = flags & ~QTextDocument::FindBackward;

		QTextCursor currentCursor = textCursor();
		currentCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		QTextCursor cursor = document()->find(expression, currentCursor, flags);
		
		cursor.beginEditBlock();
		while(!cursor.isNull())
		{
			// Replace :
			cursor.removeSelectedText();
			cursor.insertText(text);
			
			// Next :
			cursor = document()->find(expression, cursor.position(), flags);
		}
		cursor.endEditBlock();

		// Show remaining :
		searchExpression = expression;
		updateSearchExtraSelection();
	}

	void CodeEditor::clearSearch(void)
	{
		searchExpression = QRegExp(); // clear
		updateSearchExtraSelection();
		updateExtraHighlight();
	}

	void CodeEditor::highlightErrorLines(const QList<int>& lineNumbers)
	{
		errorLineNumbers = lineNumbers;
		
		// Force update :
		repaint();
	}

	void CodeEditor::clearHighlightErrorLines(void)
	{
		errorLineNumbers.clear();

		// Force update :
		repaint();
	}

// CodeEditorContainer :
	CodeEditorContainer::CodeEditorContainer(QWidget* parent)
	 : 	QWidget(parent),
		layout(this),
		splitterLayout(Qt::Vertical, this),
		editor(this),
		errorsList(this)
	{		
		splitterLayout.addWidget(&editor);
		splitterLayout.addWidget(&errorsList);
		layout.addWidget(&splitterLayout);

		QObject::connect(&errorsList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(errorItemDoubleClicked(QTreeWidgetItem*, int))); 

		layout.setMargin(0);
		layout.setSpacing(0);

		QList<QString> errorsLabels;
		errorsLabels << "Error";
		errorsLabels << "Line";
		errorsLabels << "File/Source";
		errorsList.setHeaderLabels(errorsLabels);
		errorsList.setIndentation(1);

		clearErrors();
	}

	CodeEditorContainer::~CodeEditorContainer(void)
	{ }

	void CodeEditorContainer::errorItemDoubleClicked(QTreeWidgetItem* item, int column)
	{
		UNUSED_PARAMETER(column)

		if(item!=NULL && item->data(0, Qt::UserRole).toBool())
		{
			const QString source = item->data(2, Qt::UserRole).toString();
			const int lineNumber = item->data(1, Qt::UserRole).toInt();

			if(source=="THIS")
				editor.gotoLine(lineNumber);
			else if(!source.isEmpty())
				emit openFile(source, lineNumber);
		}
	}

	const CodeEditor& CodeEditorContainer::getEditor(void) const
	{
		return editor;
	}

	CodeEditor& CodeEditorContainer::getEditor(void)
	{
		return editor;
	}

	void CodeEditorContainer::clearErrors(void)
	{
		errorsList.clear();
		errorsList.hide();
		editor.clearHighlightErrorLines();
	}

	void CodeEditorContainer::showErrors(Exception compilationError)
	{
		QList<int> errorLines;
	
		errorsList.clear();

		const QColor	infoColor(128,128,128),
				errorColor(255,255,255);

		// Generate the compilation error report :
		for(int k=compilationError.getNumSubExceptions(); k>=0; k--)
		{
			const Exception& e= (k==compilationError.getNumSubExceptions()) ? compilationError : compilationError.getSubException(k);

			QTreeWidgetItem* item = new QTreeWidgetItem(&errorsList, static_cast<int>(e.getType()));
			item->setText(0, QString::fromStdString(e.getMessage()));
			QFont font = item->font(0); // Setting the font is not effective?
			font.setFixedPitch(true);
			item->setFont(0, font);

			if(e.getType()==Exception::ClientShaderException || e.getType()==Exception::ClientScriptException)
			{
				item->setText(1, tr("%1").arg(e.getLineNumber()));
				item->setText(2, QString::fromStdString(e.getFilename()));

				if(e.getFilename()=="THIS" || e.getFilename()==editor.getFilename().toStdString())
				{
					errorLines.push_back(e.getLineNumber());
					item->setData(0, Qt::UserRole, true);
					item->setData(1, Qt::UserRole, e.getLineNumber());
					item->setData(2, Qt::UserRole, "THIS");
				}
				else
				{
					item->setData(0, Qt::UserRole, true);
					item->setData(1, Qt::UserRole, e.getLineNumber());
					item->setData(2, Qt::UserRole, QString::fromStdString(e.getFilename()));
				}
			}
			else
			{
				item->setData(0, Qt::UserRole, false);
				item->setText(1, "-");
				item->setText(2, tr("GlipLib::%1").arg(QString::fromStdString(e.getShortFilename())));
				item->setData(1, Qt::UserRole, -1);
			}			

			errorsList.addTopLevelItem(item);

			if(e.getType()==Exception::ClientShaderException || e.getType()==Exception::ClientScriptException || k==0)
			{
				for(int k=0; k<item->columnCount(); k++)
					item->setForeground(k, QBrush(errorColor));
			}
			else
			{
				for(int k=0; k<item->columnCount(); k++)
					item->setForeground(k, QBrush(infoColor));
			}
		}

		editor.highlightErrorLines(errorLines);
	
		// Resize the columns :
		for(int k=0; k<errorsList.columnCount(); k++)
			errorsList.resizeColumnToContents(k);

		// Resize the box :
		if(errorsList.topLevelItemCount()>0)
		{
			int newHeight = 0;
	
			//if(errorsList.headerItem()!=NULL)
			//	newHeight += errorsList.headerItem()->sizeHint(0).height()*2;
			if(errorsList.header()!=NULL)
				newHeight += errorsList.header()->height();

			for(int k=0; k<errorsList.topLevelItemCount(); k++)
				newHeight += (errorsList.sizeHintForRow(k) + 1);

			QList<int> sizes = splitterLayout.sizes();
			int totalHeight = 0; 
			for(QList<int>::const_iterator it=sizes.begin(); it!=sizes.end(); it++)
				totalHeight += *it;
			
			// Prevent the error widget to take more than 1/3 of the total size :
			newHeight = std::min(newHeight, totalHeight/3);

			sizes[0] = totalHeight - newHeight;
			sizes[1] = newHeight;

			splitterLayout.setSizes(sizes);
		}

		errorsList.show();
	}

// SearchAndReplaceMenu :
	SearchAndReplaceMenu::SearchAndReplaceMenu(QWidget* parent)
	 : 	QMenu(parent),
		widgetAction(this),
		widget(this),
		layout(&widget),
		searchLabel("Search : ", &widget),
		replaceLabel("Replace by : ", &widget),
		searchPattern(&widget),
		replaceString(&widget),
		worldOnlyCheck("World Only", &widget),
		matchCaseCheck("Match case", &widget),
		backwardSearchCheck("Backward search", &widget),
		openMenuAction("Find", this),
		findNextAction("Find Next", this),
		replaceNextAction("Replace Next", this),
		replaceAllAction("Replace All", this),
		clearHighlightAction("Clear Highlight", this),
		target(NULL)
	{
		layout.addWidget(&searchLabel, 		0, 0);
		layout.addWidget(&searchPattern,	0, 1);
		layout.addWidget(&replaceLabel,		1, 0);
		layout.addWidget(&replaceString,	1, 1);
		layout.addWidget(&worldOnlyCheck,	2, 0, 1, 2);
		layout.addWidget(&matchCaseCheck,	3, 0, 1, 2);
		layout.addWidget(&backwardSearchCheck,	4, 0, 1, 2);
		layout.addWidget(&errorString,		5, 0, 1, 2);

		errorString.hide();

		widgetAction.setDefaultWidget(&widget);

		addAction(&widgetAction);
		addAction(&findNextAction);
		addAction(&replaceNextAction);
		addAction(&replaceAllAction);
		addAction(&clearHighlightAction);

		QObject::connect(&searchPattern,	SIGNAL(returnPressed()), &findNextAction, 	SLOT(trigger()));
		QObject::connect(&findNextAction, 	SIGNAL(triggered()), 	 this, 			SLOT(prepareExpression()));
		QObject::connect(&replaceNextAction, 	SIGNAL(triggered()), 	 this, 			SLOT(prepareExpression()));
		QObject::connect(&replaceAllAction, 	SIGNAL(triggered()), 	 this, 			SLOT(prepareExpression()));
		QObject::connect(&clearHighlightAction, SIGNAL(triggered()),	 this, 			SLOT(clearSearchHighlight()));

		QObject::connect(&openMenuAction, SIGNAL(triggered()), this, SLOT(openMenu()));

		// Shortcuts : 
		openMenuAction.setShortcuts(QKeySequence::Find);
		findNextAction.setShortcuts(QKeySequence::FindNext);
		replaceNextAction.setShortcuts(QKeySequence::Replace);

		QList<QKeySequence> keyList;
		keyList.append(QKeySequence(Qt::CTRL + Qt::Key_K));
		clearHighlightAction.setShortcuts(keyList);

		openMenuAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		findNextAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		replaceNextAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		clearHighlightAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

		// Add action to parent :
		if(parent!=NULL)
		{
			parent->addAction(&openMenuAction);
			parent->addAction(&findNextAction);
			parent->addAction(&replaceNextAction);
			parent->addAction(&replaceAllAction);
			parent->addAction(&clearHighlightAction);
		}
	}
	
	SearchAndReplaceMenu::~SearchAndReplaceMenu(void)
	{
		widgetAction.releaseWidget(&widget);
	}

	void SearchAndReplaceMenu::prepareExpression(void)
	{
		QAction* 	action 		= reinterpret_cast<QAction*>(QObject::sender());
		QString 	expression 	= searchPattern.text(),
				replacement	= replaceString.text();

		if(!expression.isEmpty() && target!=NULL)
		{
			QRegExp regExpression(expression);
			QTextDocument::FindFlags flags = 0;

			if(matchCaseCheck.isChecked())
			{
				regExpression.setCaseSensitivity(Qt::CaseSensitive);
				flags = flags | QTextDocument::FindCaseSensitively;	// actually, might be useless, see QTextDocument::find with QRegExp argument.
			}			
			else
				regExpression.setCaseSensitivity(Qt::CaseInsensitive);
			
			if(worldOnlyCheck.isChecked())
				flags = flags | QTextDocument::FindWholeWords;

			if(backwardSearchCheck.isChecked())
				flags = flags | QTextDocument::FindBackward;

			if(regExpression.isValid())
			{
				// No error to report : 
				errorString.hide();

				if(action==&findNextAction)
				{
					// Send the search request : 
					target->search(regExpression, flags);
				}
				else if(action==&replaceNextAction)	
				{
					// Send the replace request : 
					target->replace(regExpression, flags, replacement);
				}
				else if(action==&replaceAllAction)
				{
					// Send the replace all request : 
					target->replaceAll(regExpression, flags, replacement);
				}
			}			
			else
			{
				errorString.setText(tr("Error : %1").arg(regExpression.errorString()));
				errorString.show();
			}
		}
	}

	void SearchAndReplaceMenu::clearSearchHighlight(void)
	{
		if(target!=NULL)
			target->clearSearch();
	}

	void SearchAndReplaceMenu::openMenu(void)
	{
		QList<QWidget*> associatedWidgets = openMenuAction.associatedWidgets();

		if(!associatedWidgets.isEmpty())
		{
			if(target!=NULL)
			{
				// Try to use the current selection :
				QString selectedText = target->textCursor().selectedText();
		
				if(!selectedText.isEmpty())
				{
					selectedText.replace(QString::fromWCharArray(L"\u2029"), "\\n");
					searchPattern.setText(selectedText);
				}
			}

			searchPattern.selectAll();
			searchPattern.setFocus(Qt::PopupFocusReason);
			replaceString.clear();

			// Compute the position at which the menu should be displayed : 
			QPoint execPoint = associatedWidgets.front()->mapToGlobal(QPoint(0,0));

			// Shift to the right position, if possible : 
			if(associatedWidgets.size()>=2)
				execPoint += QPoint(associatedWidgets.front()->width() - width(), associatedWidgets[1]->height());
			else
				execPoint += QPoint(associatedWidgets.front()->width() - width(), 0);

			// Open the menu : 
			exec(execPoint);
		}
	}

	QAction* SearchAndReplaceMenu::getAction(void)
	{
		return &openMenuAction;
	}

	void SearchAndReplaceMenu::setCurrentCodeEditor(CodeEditor* currentCodeEditor)
	{
		target = currentCodeEditor;

		searchPattern.setEnabled(true);
		replaceString.setEnabled(true);
		worldOnlyCheck.setEnabled(true);
		matchCaseCheck.setEnabled(true);
		backwardSearchCheck.setEnabled(true);
		findNextAction.setEnabled(true);
		replaceNextAction.setEnabled(true);
		replaceAllAction.setEnabled(true);
		clearHighlightAction.setEnabled(true);
	}

	void SearchAndReplaceMenu::clearCurrentCodeEditor(void)
	{
		target = NULL;
	
		searchPattern.setEnabled(false);
		replaceString.setEnabled(false);
		worldOnlyCheck.setEnabled(false);
		matchCaseCheck.setEnabled(false);
		backwardSearchCheck.setEnabled(false);
		findNextAction.setEnabled(false);
		replaceNextAction.setEnabled(false);
		replaceAllAction.setEnabled(false);
		clearHighlightAction.setEnabled(false);
	}

// CodeEditorSettings :
	CodeEditorSettings::CodeEditorSettings(const int _defaultFontSize)
	 : defaultFontSize(_defaultFontSize)
	{
		resetSettings();
	}

	CodeEditorSettings::CodeEditorSettings(const CodeEditorSettings& c)
	 : 	defaultFontSize(c.defaultFontSize),
		glslKeywordColor(c.glslKeywordColor),
		glslFunctionColor(c.glslFunctionColor),
		glslPreprocessorColor(c.glslPreprocessorColor),
		glipLayoutLoaderKeywordColor(c.glipLayoutLoaderKeywordColor),
		glipUniformLoaderKeywordColor(c.glipUniformLoaderKeywordColor),
		commentsColor(c.commentsColor),
		braceMatchingColor(c.braceMatchingColor),
		searchColor(c.searchColor),
		editorFont(c.editorFont),
		keywordFont(c.keywordFont),
		wrapMode(c.wrapMode),
		tabNumberOfSpaces(c.tabNumberOfSpaces),
		enableHighlight(c.enableHighlight),
		highlightCurrentLine(c.highlightCurrentLine),
		braceMatching(c.braceMatching)
	{ }

	CodeEditorSettings::~CodeEditorSettings(void)
	{ }

	void CodeEditorSettings::resetSettings(void)
	{
		// Colors : 
		glslKeywordColor		= QColor(255,	128,	0);
		glslFunctionColor		= QColor(85,	255,	0);
		glslPreprocessorColor		= QColor(208,	32,	32);
		glipLayoutLoaderKeywordColor	= QColor(255, 	51, 	255);
		glipUniformLoaderKeywordColor	= QColor(51, 	255, 	255);
		commentsColor			= QColor(51,	153,	255);
		braceMatchingColor		= QColor(128,	128,	200);
		searchColor			= QColor(0,	192,	192);

		// Fonts : 
		QFontDatabase db;
		editorFont = db.font("Source Code Pro", "Regular", defaultFontSize);
		editorFont.setFixedPitch(true);
		keywordFont = db.font("Source Code Pro", "Bold", defaultFontSize);
		keywordFont.setFixedPitch(true);

		// Set the tabulation length :
		tabNumberOfSpaces = 8;

		// Wrap mode :
		wrapMode = QTextOption::WordWrap;

		// Highlight : 
		enableHighlight 	= true;
		highlightCurrentLine	= true;
		braceMatching		= true;
	}

	std::string CodeEditorSettings::getSettingsString(void) const
	{
		std::string str;

		Glip::Modules::VanillaParserSpace::Element element;

		element.noName 		= true;
		element.noArgument	= false;
		element.noBody		= true;

		#define SAVE_COLOR( varName ) \
			element.strKeyword = GLIP_STR( varName ); \
			element.arguments.push_back( varName .name().toStdString() ); \
			str += element.getCode() + "\n"; \
			element.arguments.clear();

		#define SAVE_NUMBER( varName ) \
			element.strKeyword = GLIP_STR( varName ); \
			element.arguments.push_back( toString(static_cast<int>( varName )) ); \
			str += element.getCode() + "\n"; \
			element.arguments.clear(); \

		#define SAVE_TEST( varName, test ) \
			element.strKeyword = GLIP_STR( varName ); \
			element.arguments.push_back( toString(static_cast<int>( varName test )) ); \
			str += element.getCode() + "\n"; \
			element.arguments.clear();	

		#define SAVE_FONT( varName ) \
			element.strKeyword = GLIP_STR( varName ); \
			element.arguments.push_back( varName .family().toStdString() ); \
			element.arguments.push_back( varName .styleName().toStdString() ); \
			element.arguments.push_back( toString( varName .pointSize()) ); \
			str += element.getCode() + "\n"; \
			element.arguments.clear();
	
			SAVE_COLOR( glslKeywordColor )
			SAVE_COLOR( glslFunctionColor )
			SAVE_COLOR( glslPreprocessorColor )
			SAVE_COLOR( glipLayoutLoaderKeywordColor )
			SAVE_COLOR( glipUniformLoaderKeywordColor )
			SAVE_COLOR( commentsColor )
			SAVE_COLOR( braceMatchingColor )
			SAVE_COLOR( searchColor )

			SAVE_NUMBER( tabNumberOfSpaces )
			SAVE_NUMBER( enableHighlight )
			SAVE_NUMBER( highlightCurrentLine )
			SAVE_NUMBER( braceMatching )

			SAVE_TEST( wrapMode, ==QTextOption::WordWrap )
			
			SAVE_FONT( editorFont )
			SAVE_FONT( keywordFont )

		#undef SAVE_COLOR
		#undef SAVE_NUMBER
		#undef SAVE_TEST
		#undef SAVE_FONT

		return str;
	}

	void CodeEditorSettings::setSettingsFromString(const std::string& str)
	{
		Glip::Modules::VanillaParserSpace::VanillaParser parser(str, "SOURCETOBEDEFINED");

		#define READ_COLOR( varName ) \
			if(it->strKeyword==GLIP_STR( varName )) \
			{ \
				if( it->arguments.size()==1 ) \
					varName .setNamedColor( it->arguments.front().c_str() ); \
				else \
					throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read color " GLIP_STR( varName ) ".", __FILE__, __LINE__); \
			}

		#define READ_NUMBER( varName ) \
			if(it->strKeyword==GLIP_STR( varName )) \
			{ \
				if( it->arguments.size()==1 ) \
				{ \
					if(!fromString(it->arguments.front(), varName)) \
						throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read " GLIP_STR( varName ) " : " + it->arguments.front() + ".", __FILE__, __LINE__); \
				} \
				else \
					throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read " GLIP_STR( varName ) ".", __FILE__, __LINE__); \
			}

		#define READ_TEST( varName, valTrue, valFalse ) \
			if(it->strKeyword==GLIP_STR( varName )) \
			{ \
				if( it->arguments.size()==1 ) \
				{ \
					bool t = false; \
					if(!fromString(it->arguments.front(), t)) \
						throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read " GLIP_STR( varName ) " : " + it->arguments.front() + ".", __FILE__, __LINE__); \
					else if(t) \
						varName = valTrue; \
					else \
						varName = valFalse; \
				} \
				else \
					throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read " GLIP_STR( varName ) ".", __FILE__, __LINE__); \
			}	

		#define READ_FONT( varName ) \
			if(it->strKeyword==GLIP_STR( varName )) \
			{ \
				if( it->arguments.size()==3 ) \
				{ \
					QString familyName = it->arguments[0].c_str(), \
						styleName  = it->arguments[1].c_str(); \
					int	pointSize; \
					\
					if(!fromString(it->arguments[2], pointSize)) \
						throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read pointSize for font " GLIP_STR( varName ) " : " + it->arguments[2] + ".", __FILE__, __LINE__); \
					\
					QFontDatabase db; \
					varName = db.font( familyName, styleName, pointSize); \
				} \
				else \
					throw Glip::Exception("CodeEditorSettings::setSettingsFromString - Cannot read " GLIP_STR( varName ) ".", __FILE__, __LINE__); \
			} 

		for(std::vector<Glip::Modules::VanillaParserSpace::Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
		{
			READ_COLOR( glslKeywordColor )
			READ_COLOR( glslFunctionColor )
			READ_COLOR( glslPreprocessorColor )
			READ_COLOR( glipLayoutLoaderKeywordColor )
			READ_COLOR( glipUniformLoaderKeywordColor )
			READ_COLOR( commentsColor )
			READ_COLOR( braceMatchingColor )
			READ_COLOR( searchColor )

			READ_NUMBER( tabNumberOfSpaces )
			READ_NUMBER( enableHighlight )
			READ_NUMBER( highlightCurrentLine )
			READ_NUMBER( braceMatching )

			READ_TEST( wrapMode, QTextOption::WordWrap, QTextOption::NoWrap)
			
			READ_FONT( editorFont )
			READ_FONT( keywordFont )
		}
	}

// CodeEditorSettingsInterface :
	CodeEditorSettingsInterface::CodeEditorSettingsInterface(void)
	 : 	
	#ifndef __USE_QVGL__
		widgetPtr(this),
	#else
		QVGL::SubWidget(static_cast<QVGL::SubWidget::Flag>(QVGL::SubWidget::NotResizeable | QVGL::SubWidget::NotAnchorable | QVGL::SubWidget::NotMaximizable | QVGL::SubWidget::CloseOnHideRequest)),
		widgetPtr(&innerWidget),
	#endif
		settings( fontInfo().pointSize() ),
		layout(widgetPtr),
		groupColors(widgetPtr),
		groupFonts(widgetPtr),
		groupMisc(widgetPtr),
		layoutColors(NULL),
		layoutFonts(NULL),
		layoutMisc(NULL),
		glslKeywordColorLabel("GLSL Keywords", widgetPtr),
		glslFunctionColorLabel("GLSL Functions", widgetPtr),
		glslPreprocessorColorLabel("GLSL Preprocessor", widgetPtr),
		glipLayoutLoaderKeywordColorLabel("GLIP Layout Loader Keywords", widgetPtr),
		glipUniformLoaderKeywordColorLabel("GLIP Uniforms Loader Keywords", widgetPtr),
		commentsColorLabel("Comments", widgetPtr),
		braceMatchingColorLabel("Matching braces", widgetPtr),
		searchColorLabel("Search", widgetPtr),
		glslKeywordColorButton(widgetPtr),
		glslFunctionColorButton(widgetPtr),
		glslPreprocessorColorButton(widgetPtr),
		glipLayoutLoaderKeywordColorButton(widgetPtr),
		glipUniformLoaderKeywordColorButton(widgetPtr),
		commentsColorButton(widgetPtr),
		searchColorButton(widgetPtr),
		braceMatchingColorButton(widgetPtr),
		editorFontButton(widgetPtr),
		keywordFontButton(widgetPtr),
		dialogButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Reset | QDialogButtonBox::Cancel, Qt::Horizontal, &innerWidget),
		/*okButton("OK", widgetPtr),
		applyButton("Apply", widgetPtr),
		cancelButton("Cancel", widgetPtr),
		resetButton("Reset", widgetPtr),*/
		highlightKeywordsCheck("Highlight keywords", widgetPtr),
		highlightCurrentLineCheck("Highlight current line", widgetPtr),
		braceMatchingCheck("Enable brace matching", widgetPtr),
		wrapModesBox(widgetPtr),
		tabSpacesSpin(widgetPtr)
	{
		// Create the layout for the GUI : 
			// Colors : 
				layoutColors.addWidget(&glslKeywordColorLabel, 			0, 0);
				layoutColors.addWidget(&glslFunctionColorLabel, 		1, 0);
				layoutColors.addWidget(&glslPreprocessorColorLabel,		2, 0);
				layoutColors.addWidget(&glipLayoutLoaderKeywordColorLabel, 	3, 0);
				layoutColors.addWidget(&glipUniformLoaderKeywordColorLabel, 	4, 0);
				layoutColors.addWidget(&commentsColorLabel, 			5, 0);
				layoutColors.addWidget(&braceMatchingColorLabel,		6, 0);
				layoutColors.addWidget(&searchColorLabel, 			7, 0);

				layoutColors.addWidget(&glslKeywordColorButton, 		0, 1);
				layoutColors.addWidget(&glslFunctionColorButton,		1, 1);
				layoutColors.addWidget(&glslPreprocessorColorButton,		2, 1);
				layoutColors.addWidget(&glipLayoutLoaderKeywordColorButton, 	3, 1);
				layoutColors.addWidget(&glipUniformLoaderKeywordColorButton, 	4, 1);
				layoutColors.addWidget(&commentsColorButton, 			5, 1);
				layoutColors.addWidget(&braceMatchingColorButton,		6, 1);
				layoutColors.addWidget(&searchColorButton,			7, 1);

				groupColors.setTitle("Highlight colors");
				groupColors.setLayout(&layoutColors);

				// Connect : 
				connect(&glslKeywordColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glslFunctionColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glslPreprocessorColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glipLayoutLoaderKeywordColorButton,	SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glipUniformLoaderKeywordColorButton,	SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&commentsColorButton,			SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&braceMatchingColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&searchColorButton,			SIGNAL(released()),	this, SLOT(changeColor()));

			// Font : 
				layoutFonts.addWidget(&editorFontButton);
				layoutFonts.addWidget(&keywordFontButton);

				groupFonts.setTitle("Fonts");
				groupFonts.setLayout(&layoutFonts);

				// Connect :
				connect(&editorFontButton,			SIGNAL(released()),	this, SLOT(changeFont()));
				connect(&keywordFontButton,			SIGNAL(released()),	this, SLOT(changeFont()));

			// Misc : 
				wrapModesBox.addItem( "Word wrap", 	QVariant(QTextOption::WordWrap) );
				wrapModesBox.addItem( "No wrap", 	QVariant(QTextOption::NoWrap) );

				tabSpacesSpin.setRange(0, 128);
				tabSpacesSpin.setPrefix("Tabulations length : ");

				layoutMisc.addWidget(&highlightKeywordsCheck);
				layoutMisc.addWidget(&highlightCurrentLineCheck);
				layoutMisc.addWidget(&braceMatchingCheck);
				layoutMisc.addWidget(&wrapModesBox);
				layoutMisc.addWidget(&tabSpacesSpin);
				
				groupMisc.setTitle("Miscellaneous");
				groupMisc.setLayout(&layoutMisc);

				dialogButtons.setCenterButtons(true);

			// General : 
				layout.addWidget(&groupFonts, 	0, 0, 2, 2);
				layout.addWidget(&groupMisc, 	2, 0, 4, 2);
				layout.addWidget(&groupColors, 	0, 2, 5, 2);
				layout.addWidget(&dialogButtons, 7, 0, 1, 4);
				
				connect(&dialogButtons, 	SIGNAL(clicked(QAbstractButton*)),	this, SLOT(processDialogButtonPressed(QAbstractButton*)));

		// Final update :
			updateInterface();

		#ifdef __USE_QVGL__
			setInnerWidget(&innerWidget);
			setTitle("Code Editor Settings");
		#endif
	}

	CodeEditorSettingsInterface::~CodeEditorSettingsInterface(void)
	{ }

	void CodeEditorSettingsInterface::updateInterface(void)
	{
		// From Values to GUI...

		// Colors : 
		glslKeywordColorButton.setStyleSheet(			tr("background:%1;").arg(settings.glslKeywordColor.name()) );
		glslFunctionColorButton.setStyleSheet(			tr("background:%1;").arg(settings.glslFunctionColor.name()) );
		glslPreprocessorColorButton.setStyleSheet(		tr("background:%1;").arg(settings.glslPreprocessorColor.name()) );
		glipLayoutLoaderKeywordColorButton.setStyleSheet(	tr("background:%1;").arg(settings.glipLayoutLoaderKeywordColor.name()) );
		glipUniformLoaderKeywordColorButton.setStyleSheet(	tr("background:%1;").arg(settings.glipUniformLoaderKeywordColor.name()) );
		commentsColorButton.setStyleSheet(			tr("background:%1;").arg(settings.commentsColor.name()) );
		braceMatchingColorButton.setStyleSheet(			tr("background:%1;").arg(settings.braceMatchingColor.name()) );
		searchColorButton.setStyleSheet(			tr("background:%1;").arg(settings.searchColor.name()) );

		// Fonts : 
		editorFontButton.setText(tr("Editor : %1 (%2)").arg(settings.editorFont.family()).arg(settings.editorFont.pointSize()));
		editorFontButton.setFont(settings.editorFont);

		keywordFontButton.setText(tr("Keywords : %1 (%2)").arg(settings.keywordFont.family()).arg(settings.keywordFont.pointSize()));
		keywordFontButton.setFont(settings.keywordFont);

		// Misc : 
		highlightKeywordsCheck.setChecked( settings.enableHighlight );
		highlightCurrentLineCheck.setChecked( settings.highlightCurrentLine );
		braceMatchingCheck.setChecked( settings.braceMatching );
		wrapModesBox.setCurrentIndex( wrapModesBox.findData(QVariant(settings.wrapMode)) );
		tabSpacesSpin.setValue( settings.tabNumberOfSpaces );
	}

	void CodeEditorSettingsInterface::updateValues(void)
	{
		// From GUI to Values...

		// Colors :
		settings.glslKeywordColor		= glslKeywordColorButton.palette().color(QPalette::Window);
		settings.glslFunctionColor		= glslFunctionColorButton.palette().color(QPalette::Window);
		settings.glslPreprocessorColor		= glslPreprocessorColorButton.palette().color(QPalette::Window);
		settings.glipLayoutLoaderKeywordColor	= glipLayoutLoaderKeywordColorButton.palette().color(QPalette::Window);
		settings.glipUniformLoaderKeywordColor	= glipUniformLoaderKeywordColorButton.palette().color(QPalette::Window);
		settings.commentsColor			= commentsColorButton.palette().color(QPalette::Window);
		settings.braceMatchingColor		= braceMatchingColorButton.palette().color(QPalette::Window);
		settings.searchColor			= searchColorButton.palette().color(QPalette::Window);

		// Fonts :
		settings.editorFont			= editorFontButton.font();
		settings.keywordFont			= keywordFontButton.font();

		// Misc : 
		settings.enableHighlight		= highlightKeywordsCheck.isChecked();
		settings.highlightCurrentLine		= highlightCurrentLineCheck.isChecked();
		settings.braceMatching			= braceMatchingCheck.isChecked();
		settings.wrapMode			= static_cast<QTextOption::WrapMode>( wrapModesBox.itemData( wrapModesBox.currentIndex() ).toUInt() );
		settings.tabNumberOfSpaces 		= tabSpacesSpin.value();

		// Propagate : 
		emit settingsModified();
	}

	void CodeEditorSettingsInterface::changeColor(void)
	{
		// Get the pushbutton : 
		QPushButton* target = reinterpret_cast<QPushButton*>(QObject::sender());

		QString title;

		if(target==&glslKeywordColorButton)
			title = "GLSL Keywords Color";
		else if(target==&glslFunctionColorButton)
			title = "GLSL Functions Color";
		else if(target==&glslPreprocessorColorButton)
			title = "GLSL Preprocessor Color";
		else if(target==&glipLayoutLoaderKeywordColorButton)
			title = "GLIP Layout Loader Keywords Color";
		else if(target==&glipUniformLoaderKeywordColorButton)
			title = "GLIP Uniforms Loader Keyword";
		else if(target==&commentsColorButton)
			title = "Comments Color";
		else if(target==&braceMatchingColorButton)
			title = "Matching Braces Color";
		else if(target==&searchColorButton)
			title = "Search Color";
		else
			throw Exception("CodeEditorSettingsInterface::changeColor - Unknown color picker (internal error).", __FILE__, __LINE__);

		QColor result = QColorDialog::getColor(target->palette().color(QPalette::Window), NULL, title);

		// If the user pressed 'Ok' : 
		if(result.isValid())
			target->setStyleSheet( tr("background:%1;").arg(result.name()) );
	}

	void CodeEditorSettingsInterface::changeFont(void)
	{
		QPushButton* target = reinterpret_cast<QPushButton*>(QObject::sender());

		QString title;

		if(target==&editorFontButton)
			title = "Editor Font";
		else if(target==&keywordFontButton)
			title = "Keyword Font";
		else
			throw Exception("CodeEditorSettingsInterface::changeFont - Unknown color picker (internal error).", __FILE__, __LINE__);

		bool ok = false;
		QFont result = QFontDialog::getFont(&ok, target->font(), this, title);

		if(ok)
		{
			result.setFixedPitch(true);

			if(target==&editorFontButton)
				editorFontButton.setText(tr("Editor : %1 (%2)").arg(result.family()).arg(result.pointSize()));
			else if(target==&keywordFontButton)
				keywordFontButton.setText(tr("Keywords : %1 (%2)").arg(result.family()).arg(result.pointSize()));

			target->setFont(result);
		}
	}

	void CodeEditorSettingsInterface::processDialogButtonPressed(QAbstractButton* button)
	{
		const QDialogButtonBox::StandardButton standardButton = dialogButtons.standardButton(button);

		switch(standardButton)
		{
			case QDialogButtonBox::Ok :
				updateValues();
				close();
				break;
			case QDialogButtonBox::Apply : 
				updateValues();
				break;
			case QDialogButtonBox::Reset :
				resetSettings();
				break;
			case QDialogButtonBox::Cancel :
				updateInterface();
				close();
				break;
			default : 
				// Nothing to do.
				break;
		}
	}

	const CodeEditorSettings& CodeEditorSettingsInterface::getSettings(void) const
	{
		return settings;
	}

	void CodeEditorSettingsInterface::resetSettings(void)
	{
		settings.resetSettings();
		updateInterface();
	}

// TemplateMenu : 
	const char* TemplateMenu::templatesName[numTemplates]		= { 	"Add path",
										"Include file",
										"Call module",
										"Required format", 
										"Required pipeline", 
										"Texture format",
										"Geometry + StandardQuad",
										"Geometry + Grid2D",
										"Geometry + Grid3D",
										"Geometry + Custom Model",
										"Source + GLSL fragment",
										"Source + GLSL vertex",
										"Insert source",
										"Include source from file",
										"Filter layout", 
										"Pipeline layout", 
										"Main pipeline layout",
										"Main pipeline indirection",
										"Input ports", 
										"Output ports", 
										"Filter instance",
										"Pipeline instance", 
										"Connection",
										"Full Pipeline structure",
										"Full MainPipeline structure"
									};

	const char* TemplateMenu::templatesCode[numTemplates]		= {	"ADD_PATH( )\n",
										"INCLUDE( )\n",
										"CALL: ( )\n",
										"REQUIRED_FORMAT: ( )\n",
										"REQUIRED_PIPELINE: ( )\n",
										"TEXTURE_FORMAT: ( , , GL_RGB, GL_UNSIGNED_BYTE)\n",
										"GEOMETRY: (STANDARD_QUAD)\n",
										"GEOMETRY: (GRID_2D, , )\n",
										"GEOMETRY: (GRID_3D, , , )\n",
										"GEOMETRY: (CUSTOM_MODEL, , )\n{\n\tVERTEX( x, y, z, u, v)\n\t\n\tELEMENT(a, b, c, d)\n}\n",
										"SOURCE: \n{\n\t#version 130\n\t\n\tuniform sampler2D ;\n\tout vec4 ;\n\t\n\tvoid main()\n\t{\n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\tvec4 col = textureLod( , pos, 0);\n\t\n\t\t = col;\n\t}\n}\n",
										"SOURCE: \n{\n\t#version 130\n\t\n\tvoid main()\n\t{\n\t\tgl_FrontColor = gl_Color;\n\n\t\tgl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n\t\tgl_Position = ftransform();\n\t}\n}\n",
										"INSERT(\n)",
										"SOURCE: ()\n",
										"FILTER_LAYOUT: ( , )\n",
										"PIPELINE_LAYOUT: \n{\n\t\n}\n",
										"PIPELINE_MAIN: \n{\n\t\n}\n",
										"PIPELINE_MAIN: ()\n",
										"INPUT_PORTS( )\n",
										"OUTPUT_PORTS( )\n",
										"FILTER_INSTANCE: ( )\n",
										"PIPELINE_INSTANCE: ( )\n",
										"CONNECTION( , , , )\n",
										"PIPELINE_LAYOUT: \n{\n\tINPUT_PORTS( )\n\tOUTPUT_PORTS( )\n\n\tFILTER_INSTANCE: ( )\n\n\tCONNECTION( , , , )\n\tCONNECTION( , , , )\n}\n",
										"PIPELINE_MAIN: \n{\n\tINPUT_PORTS( )\n\tOUTPUT_PORTS( )\n\n\tFILTER_INSTANCE: ( )\n\n\tCONNECTION( , , , )\n\tCONNECTION( , , , )\n}\n"
									};

	const char* TemplateMenu::templatesCodeWithHelp[numTemplates]	= {	"ADD_PATH( /* path */ )\n",
										"INCLUDE_FILE( /* filename */ )\n",
										"CALL: /* module name */ ( /* module arguments */ )\n",
										"REQUIRED_FORMAT: /* name to use */( /* required format name */ )\n",
										"REQUIRED_PIPELINE: /* name to use */ ( /* required pipeline name */ )\n",
										"TEXTURE_FORMAT: /* name */ ( /* width */, /* height */, GL_RGB, GL_UNSIGNED_BYTE)\n",
										"GEOMETRY: /* name */ (STANDARD_QUAD)\n",
										"GEOMETRY: /* name */ (GRID_2D, /* interger : width */, /* interger : height */)\n",
										"GEOMETRY: /* name */ (GRID_3D, /* interger : width */, /* interger : height */, /* interger : depth */)\n",
										"GEOMETRY: /* name */ (CUSTOM_MODEL, /* GL primitive (GL_POINTS, GL_LINES, GL_TRIANGLES, etc.) */, /* It has texcoord embedded? true or false */)\n{\n\tVERTEX( x, y, z, u, v) /* depending on the model format */\n\t\n\tELEMENT(a, b, c, d) /* depending on the model format */\n}\n",
										"SOURCE: /* name */\n{\n\t#version 130\n\t\n\tuniform sampler2D /* input texture name */;\n\tout vec4 /* output texture name */;\n\t\n\t// uniform vec3 someVariableYouWantToModify = vec3(0.0, 0.0, 0.0);\n\t\n\tvoid main()\n\t{\n\t\t// The current fragment position : \n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\t// Read the base level of the texture at the current position : \n\t\tvec4 col = textureLod(/* input texture name */, pos, 0);\n\t\n\t\t/* output texture name */ = col;\n\t}\n}\n",
										"SOURCE: /* name */\n{\n\t#version 130\n\t\n\tvoid main()\n\t{\n\t\tgl_FrontColor = gl_Color;\n\n\t\tgl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n\t\tgl_Position = ftransform();\n\t}\n}\n",
										"INSERT(/* shared code name */)\n",
										"SOURCE: /* name */ ( /* filename */ )\n",
										"FILTER_LAYOUT: /* name */ ( /* output texture format name */, /* shader name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\t/* structure code */\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\t/* structure code */\n}\n",
										"PIPELINE_MAIN: /* name */ ( /* other pipeline name */ )\n",
										"INPUT_PORTS( /* ports names list */ )\n",
										"OUTPUT_PORTS( /* ports names list */ )\n",
										"FILTER_INSTANCE: /* instance name */ ( /* filter layout name */ )\n",
										"PIPELINE_INSTANCE: /* instance name */ ( /* pipeline layout name */ )\n",
										"CONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNETION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNECTION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n"
									};

	TemplateMenu::TemplateMenu(QWidget* parent)
	 :	QMenu("Insert Template", parent), 
		signalMapper(this),  
		addComments("Option : insert comments", this)
	{
		addComments.setCheckable(true);
		addAction(&addComments);

		// Init : 
		for(int k=0; k<numTemplates; k++)
		{
			templatesActions[k] = new QAction( QString(templatesName[k]), this );
			addAction(templatesActions[k]);
			signalMapper.setMapping(templatesActions[k], k);

			connect(templatesActions[k], SIGNAL(triggered()), &signalMapper, SLOT(map()));
		}

		connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(insertTemplateCalled(int)));

		addComments.setChecked(true);
	}

	TemplateMenu::~TemplateMenu(void) 
	{
		for(int k=0; k<numTemplates; k++)
			delete templatesActions[k];
	}

	void TemplateMenu::insertTemplateCalled(int k)
	{
		CodeTemplates ct = static_cast<CodeTemplates>(k);

		if(addComments.isChecked())
			emit insertTemplate(QString(templatesCodeWithHelp[ct]));
		else
			emit insertTemplate(QString(templatesCode[ct]));
	}

// ElementsMenu :
	EditorDataMenu::EditorDataMenu(ElementsMenu* _parent, CodeEditor* _editor)
	 : 	QMenu(tr("Untitled_%1").arg(QString().sprintf("%8p", reinterpret_cast<void*>(_editor))), _parent->parentWidget()),
		deltaRescan(20000),
		parent(_parent),
		editor(_editor),
		modified(false)
	{
		update();

		_parent->addMenu(this);

		QObject::connect(editor->document(), SIGNAL(contentsChanged()), this, SLOT(conditionalUpdate()));

		timer.start();
	}

	EditorDataMenu::~EditorDataMenu(void)
	{ }

	void EditorDataMenu::update(void)
	{
		// Read : 
		LayoutLoader layoutLoader;
		LayoutLoader::PipelineScriptElements  elements;

		try
		{
			elements = layoutLoader.listElements(editor->getCurrentContent());
		}
		catch(Glip::Exception& e)
		{ } // Forget the possible Exceptions

		// Rebuild the menu :
		clear();

		addAction(tr("Include %1").arg(editor->getRawTitle()), parent, SLOT(insertCalled()))->setToolTip(tr("INCLUDE(%1)\n").arg(editor->getRawTitle()));

		if(!editor->getPath().isEmpty())
			addAction(tr("Add path..."), parent, SLOT(insertCalled()))->setToolTip(tr("ADD_PATH(%1)\n").arg(editor->getPath()));

		// Load : 
		#define MAKE_LIST( listName, name ) \
			if(!elements. listName .empty()) \
			{ \
				QMenu* tmp = addMenu(tr( name " (%1)").arg(elements. listName .size())); \
				\
				for(unsigned int k=0; k<elements. listName .size(); k++) \
					tmp->addAction(elements. listName [k].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements. listName [k].c_str()); \
			}

		MAKE_LIST( requiredFormats, 	"Required Formats" );
		MAKE_LIST( requiredGeometries,	"Required Geometries" );
		MAKE_LIST( requiredPipelines,	"Required Pipelines" );
		MAKE_LIST( formats,		"Formats" );
		MAKE_LIST( sources,		"Shader Sources" );
		MAKE_LIST( geometries,		"Geometries" );
		MAKE_LIST( filtersLayout,	"Filter Layouts");

		#undef MAKE_LIST

		// Pipelines : 
		if(!elements.pipelines.empty())
		{
			QMenu* tmp = addMenu(tr("Pipelines (%1)").arg(elements.pipelines.size()));

			for(unsigned int k=0; k<elements.pipelines.size(); k++)
			{
				tmp->addAction(elements.pipelines[k].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.pipelines[k].c_str());

				QMenu* i = tmp->addMenu(tr("Inputs of %1 (%2)").arg(elements.pipelines[k].c_str()).arg(elements.pipelineInputs[k].size()));

				for(unsigned int ki=0; ki<elements.pipelineInputs[k].size(); ki++)
					i->addAction(elements.pipelineInputs[k][ki].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.pipelineInputs[k][ki].c_str());

				QMenu* o = tmp->addMenu(tr("Outputs of %1 (%2)").arg(elements.pipelines[k].c_str()).arg(elements.pipelineOutputs[k].size()));

				for(unsigned int ko=0; ko<elements.pipelineOutputs[k].size(); ko++)
					o->addAction(elements.pipelineOutputs[k][ko].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.pipelineOutputs[k][ko].c_str());
			}
		}

		if(!elements.mainPipeline.empty())
		{
			QMenu* tmp = addMenu("Main Pipeline");

			tmp->addAction(elements.mainPipeline.c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.mainPipeline.c_str());

			QMenu* i = tmp->addMenu(tr("Inputs of %1 (%2)").arg(elements.mainPipeline.c_str()).arg(elements.mainPipelineInputs.size()));

			for(unsigned int ki=0; ki<elements.mainPipelineInputs.size(); ki++)
				i->addAction(elements.mainPipelineInputs[ki].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.mainPipelineInputs[ki].c_str());

			QMenu* o = tmp->addMenu(tr("Outputs of %1 (%2)").arg(elements.mainPipeline.c_str()).arg(elements.mainPipelineOutputs.size()));

			for(unsigned int ko=0; ko<elements.mainPipelineOutputs.size(); ko++)
				o->addAction(elements.mainPipelineOutputs[ko].c_str(), parent, SLOT(insertCalled()))->setToolTip(elements.mainPipelineOutputs[ko].c_str());
		}

		// Title : 
		setTitle(editor->getTitle());
	}

	void EditorDataMenu::conditionalUpdate(void)
	{
		if(timer.elapsed()>deltaRescan)
		{
			update();
			timer.restart();
		}
	}

// ElementsMenu :
	ElementsMenu::ElementsMenu(QWidget* parent)
	 : 	QMenu("Elements", parent)
	{
		setEnabled(false);
	}

	ElementsMenu::~ElementsMenu(void)
	{
		clear();
	}

	void ElementsMenu::insertCalled(void)
	{
		QAction* sender = reinterpret_cast<QAction*>(QObject::sender());

		emit insertElement(sender->toolTip());
	}

	void ElementsMenu::editorDestroyed(void)
	{
		CodeEditor* editor = reinterpret_cast<CodeEditor*>(QObject::sender());

		QMap<CodeEditor*, EditorDataMenu*>::iterator it = menus.find(editor);

		if(it!=menus.end())
		{
			it.value()->deleteLater();
			menus.erase(it);
		}

		if(menus.isEmpty())
			setEnabled(false);
	}

	void ElementsMenu::track(CodeEditor* editor)
	{
		EditorDataMenu* editorDataMenu = new EditorDataMenu(this, editor);	
		menus[editor] = editorDataMenu;
		QObject::connect(editor, 		SIGNAL(destroyed(void)), this, SLOT(editorDestroyed(void)));
		QObject::connect(editor->document(), 	SIGNAL(destroyed(void)), this, SLOT(editorDestroyed(void))); // Which ever comes first.
		setEnabled(true);
	}

// RecentFileMenu ;
	RecentFilesMenu::RecentFilesMenu(QWidget* parent)
	 : 	QMenu("Recent files", parent),
		clearAction("Clear", this)
	{
		QObject::connect(&signalMapper, SIGNAL(mapped(const QString&)), this, SIGNAL(openRequest(const QString&)));
		QObject::connect(&clearAction,	SIGNAL(triggered()),		this, SLOT(clear()));

		buildMenu();
	}

	RecentFilesMenu::~RecentFilesMenu(void)
	{ }

	void RecentFilesMenu::buildMenu(void)
	{
		// Remove clear action : 
		removeAction(&clearAction);

		// Delete all other actions : 
		QMenu::clear();

		if(recentFiles.isEmpty())
			addAction("(None)")->setEnabled(false);
		else
		{
			for(QList<QString>::const_iterator it=recentFiles.begin(); it!=recentFiles.end(); it++)
			{
				QFileInfo info(*it);
				QAction* action = addAction(info.fileName(), &signalMapper, SLOT(map()));
				signalMapper.setMapping(action, *it);
			}

			addSeparator();
			addAction(&clearAction);
		}
	}

	const QList<QString>& RecentFilesMenu::getList(void) const
	{
		return recentFiles;
	}

	void RecentFilesMenu::append(const QString& filename, bool updateMenuNow)
	{
		int c = recentFiles.indexOf(filename);

		if(c>=0)
			recentFiles.removeAt(c);
	
		QFileInfo info(filename);

		if(info.exists())
			recentFiles.prepend(filename);

		if(updateMenuNow)
			buildMenu();
	}

	void RecentFilesMenu::append(const QList<QString>& filenames)
	{
		for(QList<QString>::const_iterator it=filenames.begin(); it!=filenames.end(); it++)
			append(*it, false);

		buildMenu();
	}

	void RecentFilesMenu::clear(void)
	{
		recentFiles.clear();

		buildMenu();
	}

// CodeEditorTabs :
	CodeEditorTabs::CodeEditorTabs(void)
	 : 	currentPath("."),
		layout(this),
		topBar(NULL),
		menuBarLeft(this),
		menuBarRight(this),
		mainMenu("Menu", this),
		tabBar(this),
		newAction("+", this),
		openAction("Open", this),	
		saveAction("Save", this),
		saveAsAction("Save as", this),
		saveAllAction("Save all", this),
		closeAction("Close", this),
		closeAllAction("Close all", this),
		settingsAction("Preferences", this),
		compileAction("Compile", this),
		templateMenu(this),
		elementsMenu(this),
		//settingsInterface(NULL),
		searchAndReplaceMenu(this),
		recentFilesMenu(this)
	{
		// Build Menu : 
		mainMenu.addAction(&openAction);
		mainMenu.addMenu(&recentFilesMenu);
		mainMenu.addAction(&saveAction);
		mainMenu.addAction(&saveAsAction);
		mainMenu.addAction(&saveAllAction);
		mainMenu.addAction(&closeAllAction);
		mainMenu.addSeparator();
		mainMenu.addMenu(&templateMenu);
		mainMenu.addMenu(&elementsMenu);
		mainMenu.addSeparator();
		mainMenu.addAction(&settingsAction);

		// Tab Settings : 
		tabBar.setExpanding(false);
		tabBar.setMovable(true);
		tabBar.setTabsClosable(true);
		tabBar.setDrawBase(false);

		// Actions settings :
		menuBarLeft.addMenu(&mainMenu);
		menuBarLeft.addAction(&compileAction);
		menuBarLeft.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		menuBarRight.addAction(&newAction);
		menuBarRight.addAction(searchAndReplaceMenu.getAction());
		menuBarRight.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		topBar.addWidget(&menuBarLeft);
		topBar.addWidget(&tabBar);
		topBar.addWidget(&menuBarRight);

		topBar.setMargin(0);
		topBar.setSpacing(1);

		layout.addLayout(&topBar);
		layout.addLayout(&stack);
		layout.setMargin(0);
		layout.setSpacing(0);

		addAction(&closeAction);

		//settingsInterface = new CodeEditorSettingsInterface;

		// Signals : 
		QObject::connect(&tabBar, 			SIGNAL(currentChanged(int)), 					this, 			SLOT(changedToTab(int)));
		QObject::connect(&tabBar,			SIGNAL(tabCloseRequested(int)),					this, 			SLOT(closeTab(int)));
		QObject::connect(&newAction,			SIGNAL(triggered(void)), 					this, 			SLOT(addTab(void)));
		QObject::connect(&openAction,			SIGNAL(triggered(void)), 					this, 			SLOT(open()));
		QObject::connect(&recentFilesMenu,		SIGNAL(openRequest(const QString&)),				this,			SLOT(open(const QString&)));
		QObject::connect(&saveAction,			SIGNAL(triggered(void)), 					this, 			SLOT(save()));
		QObject::connect(&saveAsAction,			SIGNAL(triggered(void)), 					this, 			SLOT(saveAs()));
		QObject::connect(&saveAllAction,		SIGNAL(triggered(void)), 					this, 			SLOT(saveAll()));
		QObject::connect(&closeAction,			SIGNAL(triggered(void)), 					this, 			SLOT(closeTab()));
		QObject::connect(&closeAllAction,		SIGNAL(triggered(void)), 					this, 			SLOT(closeAll()));
		#ifndef __USE_QVGL__
		QObject::connect(&settingsAction,		SIGNAL(triggered(void)), 					&settingsInterface, 	SLOT(show()));
		#else
		QObject::connect(&settingsAction,		SIGNAL(triggered(void)), 					this, 			SIGNAL(showEditorSettingsInterface()));
		#endif
		QObject::connect(&templateMenu,			SIGNAL(insertTemplate(QString)),				this,			SLOT(insert(QString)));
		QObject::connect(&elementsMenu,			SIGNAL(insertElement(QString)),					this,			SLOT(insert(QString)));
		QObject::connect(&settingsInterface,		SIGNAL(settingsModified(void)),					this,			SLOT(updateSettings(void)));
		QObject::connect(&compileAction,		SIGNAL(triggered(void)), 					this, 			SLOT(transferSourceCompilation(void)));

		// Shortcuts : 
		newAction.setShortcuts(		QKeySequence::New );
		openAction.setShortcuts(	QKeySequence::Open );
		saveAction.setShortcuts(	QKeySequence::Save );
		saveAsAction.setShortcuts(	QKeySequence::SaveAs );
		//saveAllAction.setShortcuts	TODO
		closeAction.setShortcuts(	QKeySequence::Close );
		//closeAllAction.setShortcuts	TODO

		QList<QKeySequence> compileActionShortCuts;
		compileActionShortCuts.push_back(Qt::CTRL + Qt::Key_R);
		//compileActionShortCuts.push_back(QKeySequence::Refresh);
		compileAction.setShortcuts(compileActionShortCuts);

		newAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		openAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		saveAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		saveAsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		saveAllAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		closeAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		closeAllAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
		compileAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

		// Add the actions to this inner widget : 
		addAction(&newAction);
		addAction(&openAction);
		addAction(&saveAction);
		addAction(&saveAsAction);
		addAction(&saveAllAction);
		addAction(&closeAction);
		addAction(&closeAllAction);
		addAction(&compileAction);

		// Create a new tab by default : 
		addTab();
	}

	CodeEditorTabs::~CodeEditorTabs(void)
	{ 
		while(tabBar.count()>0)
			removeTab(0);
			//closeTab(0, true);
		//delete settingsInterface;
	}
	
	CodeEditorSettingsInterface& CodeEditorTabs::getEditorSettingsInterface(void)
	{
		return settingsInterface;
	}
	
	const CodeEditorSettings& CodeEditorTabs::getEditorSettings(void) const
	{
		return settingsInterface.getSettings();
	}

	CodeEditorContainer* CodeEditorTabs::getCurrentEditor(void)
	{
		int tabID = tabBar.currentIndex();

		if(tabID<0)
			return NULL;

		QMap<int, CodeEditorContainer*>::iterator it = editors.find(tabBar.tabData(tabID).toUInt());

		if(it!=editors.end())
			return (*it);
		else
			return NULL;
	}
	
	CodeEditorContainer* CodeEditorTabs::getEditor(int tabID)
	{
		if(tabID<0 || tabID>=tabBar.count())
			return NULL;
		else
		{
			const int idx = tabBar.tabData(tabID).toUInt();
			QMap<int, CodeEditorContainer*>::iterator it = editors.find(idx);
			if(it!=editors.end())
				return (*it);
			else
				return NULL;
		}
	}

	int CodeEditorTabs::getTabIndex(CodeEditorContainer* editor)
	{
		if(editor==NULL || tabBar.count()==0)
			return -1;
		else
		{
			for(int c=0; c<tabBar.count(); c++)
			{
				CodeEditorContainer* _editor = getEditor(c);

				if(_editor!=NULL && _editor==editor)
					return c;
			}
		
			return -1;
		}
	}

	int CodeEditorTabs::getTabIndex(CodeEditor* editor)
	{
		if(editor==NULL || tabBar.count()==0)
			return -1;
		else
		{
			for(int c=0; c<tabBar.count(); c++)
			{
				CodeEditorContainer* container = getEditor(c);
				
				if(container!=NULL && &container->getEditor()==editor)
					return c;
			}
		
			return -1;
		}
	}

	int CodeEditorTabs::getTabIndex(const QString& filename)
	{
		if(filename.isEmpty())
			return -1;
		else
		{
			const QFileInfo targetInfo(filename);

			for(int c=0; c<tabBar.count(); c++)
			{
				CodeEditorContainer* container = getEditor(c);	
		
				if(container!=NULL && !container->getEditor().getFilename().isEmpty())
				{
					const QFileInfo info(container->getEditor().getFilename());
					
					if(info==targetInfo)
						return c;
				}
			}
		
			return -1;
		}
	}

	void CodeEditorTabs::setCurrentPath(QString path)
	{
		QFileInfo info(path);
		currentPath = info.path();
	}

	void CodeEditorTabs::save(CodeEditorContainer* editor)
	{
		if(editor==NULL)
			editor = getCurrentEditor();

		if(editor==NULL)
			return ;

		if(editor->getEditor().canBeSaved())
		{
			editor->getEditor().save();
			setCurrentPath(editor->getEditor().getFilename());
			recentFilesMenu.append(editor->getEditor().getFilename());
		}
		else 
			saveAs(editor);
	}

	void CodeEditorTabs::saveAs(CodeEditorContainer* editor, QString filename)
	{
		if(editor==NULL)
			editor = getCurrentEditor();

		if(editor==NULL)
			return ;

		if(filename.isEmpty())
			filename = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Save Source File as ..."), currentPath, "Source (*.ppl *.shr *.uvd *.txt)");

		if(filename.isEmpty())
			return ;
		else
		{
			editor->getEditor().save(filename);
			setCurrentPath(filename);
			recentFilesMenu.append(editor->getEditor().getFilename());
		}
	}

	void CodeEditorTabs::removeTab(int tabID)
	{
		CodeEditorContainer* container = getEditor(tabID);

		if(container!=NULL)
		{
			tabBar.removeTab(tabID);
			stack.removeWidget(container);
			container->deleteLater();
			editors.remove(editors.key(container));
		}
	}

	void CodeEditorTabs::closeEvent(QCloseEvent* event)
	{
		while(tabBar.count()>0)
		{
			if(!closeTab(0))
			{
				event->ignore();
				return ;
			}
		}

		QWidget::closeEvent(event);
	}

	void CodeEditorTabs::addTab(const QString& filename, int lineNumber)
	{
		static unsigned int counter = 0; 
	
		// Close any existing, empty and non saved tab : 
		CodeEditorContainer* currentEditor = getCurrentEditor();
		if(currentEditor!=NULL && currentEditor->getEditor().empty() && currentEditor->getEditor().getFilename().isEmpty())
		{
			int idx = getTabIndex(currentEditor);
			closeTab(idx);
		}

		// Create the widget : 
		CodeEditorContainer* ptr = new CodeEditorContainer(this);
		editors[counter] = ptr;
		ptr->getEditor().updateSettings(getEditorSettings());
		ptr->getEditor().addSubMenu(&templateMenu);
		ptr->getEditor().addSubMenu(&elementsMenu);

		QObject::connect(&ptr->getEditor(), 	SIGNAL(titleChanged(void)),	this, SLOT(tabTitleChanged(void)));
		QObject::connect(&ptr->getEditor(), 	SIGNAL(modified(bool)), 	this, SLOT(documentModified(bool)));
		QObject::connect(ptr,			SIGNAL(openFile(QString, int)),	this, SLOT(open(const QString&, int)));

		// Create the tab :
		stack.addWidget(ptr);
		int c = tabBar.addTab("Unnamed.ppl");
		tabBar.setTabData(c, counter);
		tabBar.setCurrentIndex(c);
		searchAndReplaceMenu.setCurrentCodeEditor(&ptr->getEditor());
		
		counter++;

		if(!filename.isEmpty())
		{
			ptr->getEditor().open(filename);
	
			if(lineNumber>0)
				ptr->getEditor().gotoLine(lineNumber);
		}

		elementsMenu.track(&ptr->getEditor());
	}

	void CodeEditorTabs::tabTitleChanged(void)
	{
		CodeEditor* ptr = reinterpret_cast<CodeEditor*>(QObject::sender());

		int tabID = getTabIndex(ptr);

		if(ptr!=NULL && tabID>=0)
		{
			tabBar.setTabText(tabID, ptr->getTitle());

			QString toolTip = "<table>";
			toolTip += tr("<tr><td><i>Filename</i></td><td>:</td><td>%1</td></tr>").arg(ptr->getRawTitle());
			toolTip += tr("<tr><td><i>Path</i></td><td>:</td><td>%1</td></tr>").arg(ptr->getPath());
			toolTip += "</table>";

			tabBar.setTabToolTip(tabID, toolTip);
		}
	}

	void CodeEditorTabs::documentModified(bool changed)
	{
		UNUSED_PARAMETER(changed)
		CodeEditor* ptr = reinterpret_cast<CodeEditor*>(QObject::sender());

		int tabID = getTabIndex(ptr);

		if(ptr!=NULL && tabID>=0)
			tabBar.setTabText(tabID, ptr->getTitle());
	}

	void CodeEditorTabs::insert(QString str)
	{
		CodeEditorContainer* editor = getCurrentEditor();

		if(editor!=NULL)
			editor->getEditor().insert(str);
	}

	void CodeEditorTabs::changedToTab(int tabID)
	{
		if(tabID<0)
			searchAndReplaceMenu.clearCurrentCodeEditor();
		else
		{
			CodeEditorContainer* container = getEditor(tabID);

			if(container!=NULL)
			{
				stack.setCurrentWidget(container);
				searchAndReplaceMenu.setCurrentCodeEditor(&container->getEditor());
			}
			else
				searchAndReplaceMenu.clearCurrentCodeEditor();
		}
	}

	void CodeEditorTabs::addTab(void)
	{
		addTab("");
	}

	void CodeEditorTabs::open(QStringList filenameList, QVector<int> lineNumberList)
	{
		if(filenameList.empty())
		{
			filenameList = QFileDialog::getOpenFileNames(QApplication::activeWindow(), "Open Source File(s)", currentPath, "Source Files (*.ppl *.shr *.uvd *.txt)");
			lineNumberList.fill(0, filenameList.size());
		}

		if(!filenameList.empty())
		{
			if(lineNumberList.empty() || filenameList.size()!=lineNumberList.size()) // silent failure.
				lineNumberList.fill(0, filenameList.size());

			QVector<int>::iterator ln=lineNumberList.begin();
			for(QStringList::iterator it=filenameList.begin(); it!=filenameList.end(); it++)
			{
				const int c = getTabIndex(*it);

				if(c<0) // The file is not currently opened, open it in a new tab :
				{
					addTab(*it, *ln);
					recentFilesMenu.append(*it);
					ln++;
				}
				else // The file is currently opened, swith to the appropriate tab :
				{
					CodeEditorContainer* container = getEditor(c);

					if(container!=NULL)
					{
						tabBar.setCurrentIndex(c);

						// If the caller is required to goto to a definite line number :
						if(*ln>0)
							container->getEditor().gotoLine(*ln);
					}
				}		
			}

			setCurrentPath(filenameList.front());
		}
	}

	void CodeEditorTabs::open(const QString& filename, int lineNumber)
	{
		QStringList 	filenameList(filename);
		QVector<int>	lineNumberList(lineNumber);
		open(filenameList, lineNumberList);
	}

	void CodeEditorTabs::save(void)
	{
		save(getCurrentEditor());
	}

	void CodeEditorTabs::saveAs(const QString& filename)
	{
		saveAs(getCurrentEditor());
	}

	void CodeEditorTabs::saveAll(void)
	{
		for(QMap<int, CodeEditorContainer*>::iterator it=editors.begin(); it!=editors.end(); it++)
			save(*it);
	}

	void CodeEditorTabs::updateSettings(void)
	{
		for(QMap<int, CodeEditorContainer*>::iterator it=editors.begin(); it!=editors.end(); it++)
			(*it)->getEditor().updateSettings(getEditorSettings());
	}

	void CodeEditorTabs::transferSourceCompilation(void)
	{
		CodeEditorContainer* editor = getCurrentEditor();

		if(editor!=NULL)
		{
			std::string 	source 		= editor->getEditor().getCurrentContent(),
					path		= editor->getEditor().getPath().toStdString(),
					sourceName	= editor->getEditor().getFilename().toStdString();

			if(sourceName.empty())
				sourceName = "THIS";

			emit compileSource(source, path, sourceName, reinterpret_cast<void*>(editor), reinterpret_cast<QObject*>(this));
		}
	}	

	bool CodeEditorTabs::closeTab(int tabID, bool imperative)
	{
		CodeEditorContainer* container = getEditor(tabID);

		if(container!=NULL)
		{
			if(container->getEditor().isModified())
			{
				changedToTab(tabID);

				QMessageBox::StandardButton 	returnedButton;
				QMessageBox::StandardButtons	buttons;

				if(imperative)
					buttons = QMessageBox::Save | QMessageBox::Discard;
				else
					buttons = QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel;

				if(container->getEditor().getFilename().isEmpty())
					returnedButton = QMessageBox::warning(NULL, tr("Warning!"), tr("New file has been modified.\n Do you want to save your changes?"), buttons);
				else
					returnedButton = QMessageBox::warning(NULL, tr("Warning!"), tr("The file %1 has been modified.\n Do you want to save your changes?").arg(container->getEditor().getFilename()), buttons);

				if(returnedButton==QMessageBox::Save)
					save(container);
				else if(returnedButton == QMessageBox::Cancel)
					return false;
			}

			removeTab(tabID);
		}

		return true;
	}

	void CodeEditorTabs::compilationSuccessNotification(void* identifier)
	{
		CodeEditorContainer* editor = reinterpret_cast<CodeEditorContainer*>(identifier);

		if(getTabIndex(editor)>=0)
			editor->clearErrors();
	}

	void CodeEditorTabs::compilationFailureNotification(void* identifier, Exception compilationError)
	{
		CodeEditorContainer* editor = reinterpret_cast<CodeEditorContainer*>(identifier);

		if(getTabIndex(editor)>=0)
			editor->showErrors(compilationError);
		else
		{
			// Warning :
			QMessageBox messageBox(QMessageBox::Warning, "Error", tr("An exception was caught. However, you might be able to continue execution."), QMessageBox::Ok);
			messageBox.setDetailedText(compilationError.what());
			messageBox.exec();
		}
	}

	void CodeEditorTabs::closeTab(void)
	{
		closeTab(tabBar.currentIndex());
	}
	
	void CodeEditorTabs::closeAll(void)
	{
		while(tabBar.count()>0)
			closeTab(0);
	}

// CodeEditorSubWidget
#ifdef __USE_QVGL__
	CodeEditorTabsSubWidget::CodeEditorTabsSubWidget(void)
	{
		setInnerWidget(&codeEditorTabs);
		setTitle("Code Editor");

		// Resize : 
		const QFontInfo fontInfo(font());
		const int em = fontInfo.pixelSize();
		resize(64*em, 32*em);

		QObject::connect(&codeEditorTabs, SIGNAL(showEditorSettingsInterface(void)), this, SLOT(showEditorSettingsInterface(void)));
	}
	
	CodeEditorTabsSubWidget::~CodeEditorTabsSubWidget(void)
	{ }

	void CodeEditorTabsSubWidget::showEditorSettingsInterface(void)
	{
		QVGL::GlipViewWidget* parent = getQVGLParent();
		if(parent!=NULL)
			parent->addSubWidget(&codeEditorTabs.getEditorSettingsInterface());
	}

	CodeEditorTabs* CodeEditorTabsSubWidget::getCodeEditorPtr(void)
	{
		return &codeEditorTabs;
	}
#endif

