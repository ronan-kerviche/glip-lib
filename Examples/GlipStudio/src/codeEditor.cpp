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

		// GLSL Keywords :
			glslkeywordFormat.setForeground(QColor(255,128,0));
			glslkeywordFormat.setFontWeight(QFont::Bold);
			QStringList glslkeywordPatterns;

			for(int i=0; i<GLSL_KW_END; i++)
			{
				std::string str = std::string("\\b") + GLSLKeyword[i] + "\\b";
				glslkeywordPatterns << str.c_str();
			}


			foreach (const QString& pattern, glslkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glslkeywordFormat;
				highlightingRules.append(rule);
			}

		// GLSL Functions : 
			glslfunctionFormat.setForeground(QColor(255,128,0));
			QStringList glslfunctionPatterns;

			for(int i=0; i<GLSL_FN_END; i++)
			{
				std::string str = std::string("\\b") + GLSLFunction[i] + "\\b";
				glslfunctionPatterns << str.c_str();
			}


			foreach (const QString& pattern, glslfunctionPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glslfunctionFormat;
				highlightingRules.append(rule);
			}

		// GLIP Keywords : 
			glipkeywordFormat.setForeground(QColor(255, 51, 255));
			glipkeywordFormat.setFontWeight(QFont::Bold);
			QStringList glipkeywordPatterns;

			for(int i=0; i<Glip::Modules::NumKeywords; i++)
			{
				std::string str = std::string("\\b") + Glip::Modules::keywords[i] + "\\b";
				glipkeywordPatterns << str.c_str();
			}

			foreach (const QString& pattern, glipkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glipkeywordFormat;
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
			quotationFormat.setForeground(QColor(51,255,51));
			rule.pattern = QRegExp("\".*\"");
			rule.format = quotationFormat;
			highlightingRules.append(rule);

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
		foreach (const HighlightingRule &rule, highlightingRules)
		{
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0)
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

		while (startIndex >= 0)
		{
			int endIndex = commentEndExpression.indexIn(text, startIndex);
			int commentLength;
			if (endIndex == -1)
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
	 : QPlainTextEdit(parent), modified(false)
	{
		lineNumberArea = new LineNumberArea(this);

		connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
		connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
		connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));		
		connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

		updateLineNumberAreaWidth(0);

		// Set the font : 
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		font.setPointSize(11);
		setFont(font);	

		// Set the tabulation length :
		const int tabStop = 8;
		QFontMetrics metrics(font);
		setTabStopWidth(tabStop * metrics.width(' '));

		highLighter = new Highlighter(document());
	}

	CodeEditor::~CodeEditor(void)
	{
		delete lineNumberArea;
		delete highLighter;
	}

	int CodeEditor::lineNumberAreaWidth()
	{
		int digits = std::floor( std::log10( qMax(1, blockCount()) ) ) + 1;

		int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

		return space;
	}

	void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
	{
		setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
	}

	void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
	{
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

	void CodeEditor::highlightCurrentLine()
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
				painter.drawText(0, top, lineNumberArea->width(), metrics.height(), Qt::AlignRight, number);
			}

			block = block.next();
			top = bottom;
			bottom = top + (int) blockBoundingRect(block).height();
			++blockNumber;
		}
	}	

	bool CodeEditor::open(void)
	{
		maybeSave();

		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Pipeline Script File"), "./Filters/", tr("Pipeline Script Files (*.ppl *.glsl *.ext)"));
		
		if(!fileName.isEmpty())
			return loadFile(fileName);
		else
			return false;
	}

	bool CodeEditor::save(void)
	{
		if(currentFilename.isEmpty())
			return saveAs();
		else
			return saveFile(currentFilename);
	}

	bool CodeEditor::saveAs(void)
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pipeline Script File"), "./Filters/", tr("Pipeline Script Files (*.ppl *.glsl *.ext)"));
		if (fileName.isEmpty())
			return false;

		return saveFile(fileName);
	}

	void CodeEditor::documentWasModified(void)
	{
		if(!modified && document()->isModified())
		{
			modified = true;
			emit titleChanged(); // append !
		}
	}

	bool CodeEditor::maybeSave(void)
	{
		if(modified)
		{
			QMessageBox::StandardButton ret;
			ret = QMessageBox::warning(this, tr("Warning!"), tr("The document has been modified.\n Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			if (ret == QMessageBox::Save)
				return save();
			else if (ret == QMessageBox::Cancel)
				return false;
		}

		return true;
	}

	void CodeEditor::ensureOneLine(void)
	{
		// Make sure there is at least one '\n' :
		if(!toPlainText().contains('\n'))
			setPlainText(toPlainText()+"\n ");
	}

	bool CodeEditor::loadFile(const QString& fileName)
	{
		QFile file(fileName);

		if (!file.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
			return false;
		}

		QTextStream in(&file);
		#ifndef QT_NO_CURSOR
			QApplication::setOverrideCursor(Qt::WaitCursor);
		#endif

		// Prevent to understand a real modification :
		modified = true;

		setPlainText(in.readAll());

		modified = false;

		#ifndef QT_NO_CURSOR
			QApplication::restoreOverrideCursor();
		#endif

		setCurrentFile(fileName);

		return true;
	}

	bool CodeEditor::saveFile(const QString& fileName)
	{
		ensureOneLine();

		QFile file(fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			QMessageBox::warning(this, tr("CodeEditorDecorated::saveFile"),tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString()));
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

		setCurrentFile(fileName);
		modified = false;

		emit titleChanged();

		return true;
	}

	void CodeEditor::setCurrentFile(const QString &fileName)
	{
		currentFilename = fileName;
		emit titleChanged();
	}

	bool CodeEditor::canBeClosed(void)
	{
		return maybeSave();
	}

	QString CodeEditor::getTitle(void) const
	{
		QFileInfo path(currentFilename);
		QString fileName;
		
		if(path.exists())
			fileName = path.fileName();
		else
			fileName = "Unnamed.ppl";

		if( modified )
			return fileName + " !";
		else
			return fileName;
	}

	std::string CodeEditor::getCode(void) const
	{
		return toPlainText().toStdString();
	}

	std::string CodeEditor::getPath(void) const
	{
		QFileInfo path(currentFilename);
		QString str = path.path() + "/";

		return str.toStdString();
	}

	bool CodeEditor::openFile(const QString& filename)
	{
		if(!filename.isEmpty())
		{
			maybeSave();
			return loadFile(filename);
		}
		else
			return false;
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

// CodeEditorsPannel
	CodeEditorsPannel::CodeEditorsPannel(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), widgets(this),
	   newTabAct(tr("&New tab"), this), saveAct(tr("&Save"), this), saveAsAct(tr("Save as"), this), openAct(tr("&Open In New Tab"), this), refreshAct("&Refresh", this), closeTabAct(tr("&Close Tab"), this)
	{
		// Add the actions : 
		newTabAct.setShortcuts(QKeySequence::New);
		newTabAct.setStatusTip(tr("Open new tab ..."));
		connect(&newTabAct, SIGNAL(triggered()), this, SLOT(newTab()));

		openAct.setShortcuts(QKeySequence::Open);
		openAct.setStatusTip(tr("Open file ..."));
		connect(&openAct, SIGNAL(triggered()), this, SLOT(open()));

		saveAct.setShortcuts(QKeySequence::Save);
		saveAct.setStatusTip(tr("Save file ..."));
		connect(&saveAct, SIGNAL(triggered()), this, SLOT(save()));

		saveAsAct.setShortcuts(QKeySequence::SaveAs);
		saveAsAct.setStatusTip(tr("Save file to ..."));
		connect(&saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

		QList<QKeySequence> refreshShortcuts;
		refreshShortcuts.push_back(QKeySequence(tr("Ctrl+r")));
		refreshShortcuts.push_back(QKeySequence(tr("F5")));
		refreshAct.setShortcuts(refreshShortcuts);
		refreshAct.setStatusTip(tr("Refresh"));
		connect(&refreshAct, SIGNAL(triggered()), this, SLOT(refresh()));

		closeTabAct.setStatusTip(tr("Close tab"));
		connect(&closeTabAct, SIGNAL(triggered()), this, SLOT(closeTab()));

		// Movable : 	
		widgets.setMovable(true);

		// Menus :
		menuBar.addAction(&newTabAct);
		menuBar.addAction(&openAct);
		menuBar.addAction(&saveAct);
		menuBar.addAction(&saveAsAct);
		menuBar.addAction(&refreshAct);
		menuBar.addAction(&closeTabAct);

		// Add the first tab : 
		newTab();

		layout.addWidget(&menuBar);	
		layout.addWidget(&widgets);
	}

	CodeEditorsPannel::~CodeEditorsPannel(void)
	{
		while(widgets.count()>0)
			widgets.removeTab(0);

		for(int i=0; i<tabs.size(); i++)
			delete tabs[i];

		tabs.clear();
	}

	void CodeEditorsPannel::newTab(void)
	{
		tabs.push_back(new CodeEditor(this));
		widgets.addTab(tabs.back(), tabs.back()->getTitle());
		widgets.setCurrentIndex( widgets.count() - 1);

		connect(tabs.back(), SIGNAL(titleChanged()), this, SLOT(updateTitles()));
	}

	void CodeEditorsPannel::open(void)
	{
		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Pipeline Script Files"), "./Filters/", tr("Pipeline Script Files (*.ppl *.glsl *.ext)"));

		for(int k=0; k<filenames.count(); k++)
		{
			if(!filenames[k].isEmpty())
			{
				newTab();
				int c = widgets.currentIndex();
		
				if(!tabs[c]->openFile(filenames[k]))
					closeTab();
			}
		}
	}

	void CodeEditorsPannel::save(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();
			tabs[c]->save();
		}
	}

	void CodeEditorsPannel::saveAs(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();
			tabs[c]->saveAs();
		}
	}

	void CodeEditorsPannel::refresh(void)
	{
		if(widgets.count() > 0)
			emit requireRefresh();
	}

	void CodeEditorsPannel::closeTab(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			if(tabs[c]->canBeClosed())
			{
				widgets.removeTab(c);
				delete tabs[c];

				tabs.erase(tabs.begin() + c);
			}
		}
	}

	void CodeEditorsPannel::updateTitles(void)
	{
		for(int k=0; k<tabs.size(); k++)
			widgets.setTabText(k, tabs[k]->getTitle());
	}

	std::string CodeEditorsPannel::getCurrentCode(void) const
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			return tabs[c]->getCode();
		}
		else
			return "";	
	}

	std::string CodeEditorsPannel::getCodePath(void) const
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			return tabs[c]->getPath();
		}
		else
			return "";	
	}

	bool CodeEditorsPannel::canBeClosed(void)
	{
		bool test = true;

		for(int i=0; i<tabs.size() && test; i++)
			test = test && tabs[i]->canBeClosed();

		return test;
	}

