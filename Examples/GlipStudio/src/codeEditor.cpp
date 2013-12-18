#include <cmath>
#include "codeEditor.hpp"
#include "keywords.hpp"
#include "settingsManager.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// Highlighter
	Highlighter::Highlighter(QTextDocument *parent)
	 : QSyntaxHighlighter(parent), highlightEnabled(true)
	{
		HighlightingRule rule;

		//QFontDatabase db;
		//QFont keyFont = db.font("Source Code Pro", "Bold", glslkeywordFormat.font().pointSize());

		// GLSL Keywords :
			//glslkeywordFormat.setForeground(QColor(255,128,0));
			//glslkeywordFormat.setFontWeight(QFont::Bold);
			//glslkeywordFormat.setFont(keyFont);
			QStringList glslkeywordPatterns;

			for(int i=0; i<GLSL_KW_END; i++)
			{
				std::string str = std::string("\\b") + GLSLKeyword[i] + "\\b";
				glslkeywordPatterns << str.c_str();
			}

			foreach(const QString& pattern, glslkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = &glslkeywordFormat;
				highlightingRules.append(rule);
			}

		// GLSL Functions : 
			//glslfunctionFormat.setForeground(QColor(255,128,0));
			//glslfunctionFormat.setFont(keyFont);
			QStringList glslfunctionPatterns;

			for(int i=0; i<GLSL_FN_END; i++)
			{
				std::string str = std::string("\\b") + GLSLFunction[i] + "\\b";
				glslfunctionPatterns << str.c_str();
			}

			foreach(const QString& pattern, glslfunctionPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = &glslfunctionFormat;
				highlightingRules.append(rule);
			}

		// GLIP LayoutLoader Keywords : 
			//glipLayoutLoaderKeywordFormat.setForeground(QColor(255, 51, 255));
			//glipLayoutLoaderKeywordFormat.setFontWeight(QFont::Bold);
			//glipLayoutLoaderKeywordFormat.setFont(keyFont);
			QStringList glipllkeywordPatterns;

			for(int i=0; i<Glip::Modules::LL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + LayoutLoader::getKeyword(static_cast<Glip::Modules::LayoutLoaderKeyword>(i)) + "\\b";
				glipllkeywordPatterns << str.c_str();
			}

			foreach(const QString& pattern, glipllkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = &glipLayoutLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// GLIP Uniform Loader Keywords : 
			//glipUniformLoaderKeywordFormat.setForeground(QColor(51, 255, 255));
			//glipUniformLoaderKeywordFormat.setFontWeight(QFont::Bold);
			//glipLayoutLoaderKeywordFormat.setFont(keyFont);
			QStringList glipulkeywordPatterns;

			for(int i=0; i<Glip::Modules::UL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + UniformsVarsLoader::getKeyword(static_cast<UniformVarsLoaderKeyword>(i)) + "\\b";
				glipulkeywordPatterns << str.c_str();
			}

			foreach(const QString& pattern, glipulkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
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

	void Highlighter::highlightBlock(const QString &text)
	{
		if( highlightEnabled )
		{
			foreach(const HighlightingRule &rule, highlightingRules)
			{
				QRegExp expression(rule.pattern);
				int index = expression.indexIn(text);
				while(index >= 0)
				{
					int length = expression.matchedLength();
					setFormat(index, length, *rule.format);
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
	}

	void Highlighter::updateSettings(const CodeEditorSettings& settings)
	{
		// Color : 
		glslkeywordFormat.setForeground(		settings.getGLSLKeywordColor() );
		glslfunctionFormat.setForeground(		settings.getGLSLFunctionColor() );
		glipLayoutLoaderKeywordFormat.setForeground(	settings.getGLIPLayoutLoaderKeywordColor() );
		glipUniformLoaderKeywordFormat.setForeground(	settings.getGLIPUniformLoaderKeywordColor() );
		singleLineCommentFormat.setForeground(		settings.getCommentsColor() );
		multiLineCommentFormat.setForeground(		settings.getCommentsColor() );

		// Font : 
		glslkeywordFormat.setFont(			settings.getKeywordFont() );
		glslfunctionFormat.setFont(			settings.getKeywordFont() );	
		glipLayoutLoaderKeywordFormat.setFont(		settings.getKeywordFont() );
		glipUniformLoaderKeywordFormat.setFont(		settings.getKeywordFont() );
		singleLineCommentFormat.setFont(		settings.getKeywordFont() );
		multiLineCommentFormat.setFont(			settings.getKeywordFont() );

		// General highlighting (only if highlight was allowed for this editor) :
		highlightEnabled = settings.isHighlightEnabled();

		// Update : 
		rehighlight();
	}

// CodeEditor
	CodeEditor::CodeEditor(QWidget *parent, bool syntaxColoration)
	 : 	QPlainTextEdit(parent), 
		currentFilename(""),
		firstModification(true),
		documentModified(false),
		highlightLine(true),
		highLighter(NULL)
	{
		// Get settings : 
		CodeEditorSettings& editorSettings = CodeEditorSettings::instance();

		lineNumberArea = new LineNumberArea(this);

		connect(this, 			SIGNAL(blockCountChanged(int)), 	this, SLOT(updateLineNumberAreaWidth(int)));
		connect(this,			SIGNAL(updateRequest(QRect,int)), 	this, SLOT(updateLineNumberArea(QRect,int)));
		connect(this, 			SIGNAL(cursorPositionChanged()),	this, SLOT(highlightCurrentLine()));
		connect(document(), 		SIGNAL(contentsChanged()), 		this, SLOT(documentWasModified()));
		connect(&editorSettings,	SIGNAL(settingsModified()),		this, SLOT(updateSettings()));

		updateLineNumberAreaWidth(0);
	
		// Set the font : 
		/*int currentFontSize = document()->defaultFont().pointSize();
		QFontDatabase db;
		font = db.font("Source Code Pro", "Regular", currentFontSize);
		font.setFixedPitch(true);
		setFont(font);*/

		// Set the tabulation length :
		/*const int tabStop = 8;
		QFontMetrics metrics(font);
		setTabStopWidth(tabStop * metrics.width(' '));*/

		if(syntaxColoration)
			highLighter = new Highlighter(document());

		// Update settings : 
		updateSettings();
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

	void CodeEditor::updateSettings(void)
	{
		// Prevent the code from sending modification signal :
		blockSignals(true);
		document()->blockSignals(true);

		CodeEditorSettings& editorSettings = CodeEditorSettings::instance();

		// Set the font : 
		setFont(editorSettings.getEditorFont());
		document()->setDefaultFont(editorSettings.getEditorFont());

		// Set the tabulation length :
		const int tabStop = editorSettings.getNumberOfSpacesPerTabulation();
		QFontMetrics metrics(editorSettings.getEditorFont());
		setTabStopWidth(tabStop * metrics.width(' '));

		// Set word wrap : 
		setWordWrapMode( editorSettings.getWrapMode() );

		// Set line highlight :
		highlightLine = editorSettings.isLineHighlightEnabled();

		if(!highlightLine)
			clearHighlightOfCurrentLine();

		// Propagate : 
		if(highLighter!=NULL)
			highLighter->updateSettings(editorSettings);

		document()->blockSignals(false);
		blockSignals(false);
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
		if(highlightLine)
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
	}

	void CodeEditor::clearHighlightOfCurrentLine(void)
	{
		QList<QTextEdit::ExtraSelection> extraSelections;

		if (!isReadOnly())
		{
			QTextEdit::ExtraSelection selection;

			QColor lineColor = palette().background().color();			

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

// CodeEditorSettings
	CodeEditorSettings* CodeEditorSettings::singleton 	= NULL;
	const std::string CodeEditorSettings::moduleName 	= "CodeEditorSettings";

	CodeEditorSettings::CodeEditorSettings(QWidget* parent)
	 : 	QWidget(parent),
		layout(this),
		glslKeywordColorLabel("GLSL Keywords"),
		glslFunctionColorLabel("GLSL Functions"),
		glipLayoutLoaderKeywordColorLabel("GLIP Layout Loader Keywords"),
		glipUniformLoaderKeywordColorLabel("GLIP Uniforms Loader Keywords"),
		commentsColorLabel("Comments"),
		highlightKeywordsCheck("Highlight keywords"),
		highlightCurrentLineCheck("Highlight current line"),
		okButton("OK"),
		applyButton("Apply"),
		cancelButton("Cancel"),
		resetButton("Reset")
	{
		if(singleton!=NULL)
			throw Exception("CodeEditorSettings::CodeEditorSettings - A settings widget already exists (internal error).", __FILE__, __LINE__);
		
		// Set the singleton : 
		singleton = this;

		// Load the data : 
			// First, restore all to default : 
			resetSettings();

			// Then load settings : 
			SettingsManager settings;
			Element e;

			e = settings.getModuleData(moduleName, "ColorGLSLKeywords");
			if(!e.arguments.empty())
				glslKeywordColor.setNamedColor( e.arguments.front().c_str() );

			e = settings.getModuleData(moduleName, "ColorGLSLFunctions");
			if(!e.arguments.empty())
				glslFunctionColor.setNamedColor( e.arguments.front().c_str() );

			e = settings.getModuleData(moduleName, "ColorGLIPLayoutLoaderKeywords");
			if(!e.arguments.empty())
				glipLayoutLoaderKeywordColor.setNamedColor( e.arguments.front().c_str() );
				
			e = settings.getModuleData(moduleName, "ColorGLIPUniformsLoaderKeywords");
			if(!e.arguments.empty())
				glipUniformLoaderKeywordColor.setNamedColor( e.arguments.front().c_str() );
			
			e = settings.getModuleData(moduleName, "ColorComments");
			if(!e.arguments.empty())
				commentsColor.setNamedColor( e.arguments.front().c_str() );

			e = settings.getModuleData(moduleName, "TabulationLength");
			if(!e.arguments.empty())
				from_string(e.arguments.front(), tabNumberOfSpaces);

			e = settings.getModuleData(moduleName, "HighlightEnabled");
			if(!e.arguments.empty())
			{
				int tmp = 0;
				from_string(e.arguments.front(), tmp);
				enableHighlight = tmp>0;
			}

			e = settings.getModuleData(moduleName, "HighlightCurrentLineEnabled");
			if(!e.arguments.empty())
			{
				int tmp = 0;
				from_string(e.arguments.front(), tmp);
				highlightCurrentLine = tmp>0;
			}

			e = settings.getModuleData(moduleName, "WordWrapEnabled");
			if(!e.arguments.empty())
			{
				int tmp = 0;
				from_string(e.arguments.front(), tmp);

				if(tmp>0)
					wrapMode = QTextOption::WordWrap;
				else
					wrapMode = QTextOption::NoWrap;
			}

			// Fonts :
			QFontDatabase db;

			e = settings.getModuleData(moduleName, "EditorFont");
			if(e.arguments.size()==3)
			{
				int pointSize = 11;

				if(!from_string(e.arguments.back()))
					pointSize = 11;

				editorFont = db.font( e.arguments[0].c_str(), e.arguments[2].c_str(), pointSize);
			}

			e = settings.getModuleData(moduleName, "KeywordsFont");
			if(e.arguments.size()==3)
			{
				int pointSize = 11;

				if(!from_string(e.arguments.back()))
					pointSize = 11;

				keywordFont = db.font( e.arguments[0].c_str(), e.arguments[2].c_str(), pointSize);
			}
				
		// Create the layout for the GUI : 
			// Colors : 
				layoutColors.addWidget(&glslKeywordColorLabel, 			0, 0);
				layoutColors.addWidget(&glslFunctionColorLabel, 		1, 0);
				layoutColors.addWidget(&glipLayoutLoaderKeywordColorLabel, 	2, 0);
				layoutColors.addWidget(&glipUniformLoaderKeywordColorLabel, 	3, 0);
				layoutColors.addWidget(&commentsColorLabel, 			4, 0);

				layoutColors.addWidget(&glslKeywordColorButton, 		0, 1);
				layoutColors.addWidget(&glslFunctionColorButton,		1, 1);
				layoutColors.addWidget(&glipLayoutLoaderKeywordColorButton, 	2, 1);
				layoutColors.addWidget(&glipUniformLoaderKeywordColorButton, 	3, 1);
				layoutColors.addWidget(&commentsColorButton, 			4, 1);

				groupColors.setTitle("Highlight colors");
				groupColors.setLayout(&layoutColors);

				// Connect : 
				connect(&glslKeywordColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glslFunctionColorButton,		SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glipLayoutLoaderKeywordColorButton,	SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&glipUniformLoaderKeywordColorButton,	SIGNAL(released()),	this, SLOT(changeColor()));
				connect(&commentsColorButton,			SIGNAL(released()),	this, SLOT(changeColor()));

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
				layoutMisc.addWidget(&wrapModesBox);
				layoutMisc.addWidget(&tabSpacesSpin);
				
				groupMisc.setTitle("Miscellaneous");
				groupMisc.setLayout(&layoutMisc);

			// General : 
				layout.addWidget(&groupFonts, 	0, 0, 2, 2);
				layout.addWidget(&groupMisc, 	2, 0, 4, 2);
				layout.addWidget(&groupColors, 	0, 2, 5, 2);
				layout.addWidget(&resetButton, 	7, 0);
				layout.addWidget(&cancelButton, 7, 1);
				layout.addWidget(&applyButton, 	7, 2);
				layout.addWidget(&okButton, 	7, 3);

				connect(&applyButton,				SIGNAL(released()),	this, SLOT(softApply()));
				connect(&okButton,				SIGNAL(released()),	this, SLOT(quitDialog()));
				connect(&cancelButton,				SIGNAL(released()),	this, SLOT(quitDialog()));
				connect(&resetButton,				SIGNAL(released()),	this, SLOT(resetSettings())); 

		// Final update :
			updateGUI();
	}

	CodeEditorSettings::~CodeEditorSettings(void)
	{
		if(singleton==this)
		{
			// Save the data : 
			SettingsManager settings;
			Element e;

			e = settings.getModuleData(moduleName, "ColorGLSLKeywords");
			e.arguments.clear();
			e.arguments.push_back( glslKeywordColor.name().toStdString() );
			settings.setModuleData(moduleName, "ColorGLSLKeywords", e);

			e = settings.getModuleData(moduleName, "ColorGLSLFunctions");
			e.arguments.clear();
			e.arguments.push_back( glslFunctionColor.name().toStdString() );
			settings.setModuleData(moduleName, "ColorGLSLFunctions", e);
			
			e = settings.getModuleData(moduleName, "ColorGLIPLayoutLoaderKeywords");
			e.arguments.clear();
			e.arguments.push_back( glipLayoutLoaderKeywordColor.name().toStdString() );
			settings.setModuleData(moduleName, "ColorGLIPLayoutLoaderKeywords", e);
				
			e = settings.getModuleData(moduleName, "ColorGLIPUniformsLoaderKeywords");
			e.arguments.clear();
			e.arguments.push_back( glipUniformLoaderKeywordColor.name().toStdString() );
			settings.setModuleData(moduleName, "ColorGLIPUniformsLoaderKeywords", e);
			
			e = settings.getModuleData(moduleName, "ColorComments");
			e.arguments.clear();
			e.arguments.push_back( commentsColor.name().toStdString() );
			settings.setModuleData(moduleName, "ColorComments", e);

			e = settings.getModuleData(moduleName, "TabulationLength");
			e.arguments.clear();
			e.arguments.push_back( to_string(tabNumberOfSpaces) );
			settings.setModuleData(moduleName, "TabulationLength", e);

			e = settings.getModuleData(moduleName, "HighlightEnabled");
			e.arguments.clear();
			e.arguments.push_back( to_string(static_cast<int>(enableHighlight)) );
			settings.setModuleData(moduleName, "HighlightEnabled", e);
			
			e = settings.getModuleData(moduleName, "HighlightCurrentLineEnabled");
			e.arguments.clear();
			e.arguments.push_back( to_string(static_cast<int>(highlightCurrentLine)) );
			settings.setModuleData(moduleName, "HighlightCurrentLineEnabled", e);

			e = settings.getModuleData(moduleName, "WordWrapEnabled");
			e.arguments.clear();
			if(wrapMode==QTextOption::WordWrap)
				e.arguments.push_back( to_string(static_cast<int>(true)) );
			else
				e.arguments.push_back( to_string(static_cast<int>(false)) );
			settings.setModuleData(moduleName, "WordWrapEnabled", e);

			e = settings.getModuleData(moduleName, "EditorFont");
			e.arguments.clear();
			e.arguments.push_back( editorFont.family() );
			e.arguments.push_back( editorFont.styleName() );
			e.arguments.push_back( to_string(editorFont.pointSize()) );
			settings.setModuleData(moduleName, "EditorFont", e);

			e = settings.getModuleData(moduleName, "KeywordsFont");
			e.arguments.clear();
			e.arguments.push_back( keywordFont.family() );
			e.arguments.push_back( keywordFont.styleName() );
			e.arguments.push_back( to_string(keywordFont.pointSize()) );
			settings.setModuleData(moduleName,"KeywordsFont", e);

			// Clear : 
			singleton = NULL;
		}
		// else : do nothing.
	}

	void CodeEditorSettings::updateGUI(void)
	{
		// From Values to GUI...

		// Colors : 
		glslKeywordColorButton.setStyleSheet(			tr("background:%1;").arg(glslKeywordColor.name()) );
		glslFunctionColorButton.setStyleSheet(			tr("background:%1;").arg(glslFunctionColor.name()) );
		glipLayoutLoaderKeywordColorButton.setStyleSheet(	tr("background:%1;").arg(glipLayoutLoaderKeywordColor.name()) );
		glipUniformLoaderKeywordColorButton.setStyleSheet(	tr("background:%1;").arg(glipUniformLoaderKeywordColor.name()) );
		commentsColorButton.setStyleSheet(			tr("background:%1;").arg(commentsColor.name()) );

		// Fonts : 
		editorFontButton.setText(tr("Editor : %1 (%2)").arg(editorFont.family()).arg(editorFont.pointSize()));
		editorFontButton.setFont(editorFont);

		keywordFontButton.setText(tr("Keywords : %1 (%2)").arg(keywordFont.family()).arg(keywordFont.pointSize()));
		keywordFontButton.setFont(keywordFont);

		// Misc : 
		highlightKeywordsCheck.setChecked( enableHighlight );
		highlightCurrentLineCheck.setChecked( highlightCurrentLine );
		wrapModesBox.setCurrentIndex( wrapModesBox.findData(QVariant(wrapMode)) );
		tabSpacesSpin.setValue( tabNumberOfSpaces );
	}

	void CodeEditorSettings::updateValues(void)
	{
		// From GUI to Values...

		// Colors :
		glslKeywordColor		= glslKeywordColorButton.palette().color(QPalette::Window);
		glslFunctionColor		= glslFunctionColorButton.palette().color(QPalette::Window);
		glipLayoutLoaderKeywordColor	= glipLayoutLoaderKeywordColorButton.palette().color(QPalette::Window);
		glipUniformLoaderKeywordColor	= glipUniformLoaderKeywordColorButton.palette().color(QPalette::Window);
		commentsColor			= commentsColorButton.palette().color(QPalette::Window);

		// Fonts :
		editorFont			= editorFontButton.font();
		keywordFont			= keywordFontButton.font();

		// Misc : 
		enableHighlight			= highlightKeywordsCheck.isChecked();
		highlightCurrentLine		= highlightCurrentLineCheck.isChecked();
		wrapMode			= static_cast<QTextOption::WrapMode>( wrapModesBox.itemData( wrapModesBox.currentIndex() ).toUInt() );
		tabNumberOfSpaces 		= tabSpacesSpin.value();

		// Propagate : 
		emit settingsModified();
	}

	void CodeEditorSettings::changeColor(void)
	{
		// Get the pushbutton : 
		QPushButton* target = reinterpret_cast<QPushButton*>(QObject::sender());

		QString title;

		if(target==&glslKeywordColorButton)
			title = "GLSL Keywords Color";
		else if(target==&glslFunctionColorButton)
			title = "GLSL Functions Color";
		else if(target==&glipLayoutLoaderKeywordColorButton)
			title = "GLIP Layout Loader Keywords Color";
		else if(target==&glipUniformLoaderKeywordColorButton)
			title = "GLIP Uniforms Loader Keyword";
		else if(target==&commentsColorButton)
			title = "Comments Color";
		else
			throw Exception("CodeEditorSettings::changeColor - Unknown color picker (internal error).", __FILE__, __LINE__);

		QColor result = QColorDialog::getColor(target->palette().color(QPalette::Window), this, title);

		target->setStyleSheet( tr("background:%1;").arg(result.name()) );
	}

	void CodeEditorSettings::changeFont(void)
	{
		QPushButton* target = reinterpret_cast<QPushButton*>(QObject::sender());

		QString title;

		if(target==&editorFontButton)
			title = "Editor Font";
		else if(target==&keywordFontButton)
			title = "Keyword Font";
		else
			throw Exception("CodeEditorSettings::changeFont - Unknown color picker (internal error).", __FILE__, __LINE__);

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

	void CodeEditorSettings::softApply(void)
	{
		updateValues();
	}

	void CodeEditorSettings::quitDialog(void)
	{
		QPushButton* sender = reinterpret_cast<QPushButton*>(QObject::sender());

		hide();

		if(sender==&okButton)
			updateValues();
	}

	void CodeEditorSettings::resetSettings(void)
	{
		// Colors : 
		glslKeywordColor		= QColor(255,	128,	0);
		glslFunctionColor		= QColor(85,	255,	0);
		glipLayoutLoaderKeywordColor	= QColor(255, 	51, 	255);
		glipUniformLoaderKeywordColor	= QColor(51, 	255, 	255);
		commentsColor			= QColor(51,	153,	255);

		// Fonts : 
		QFontDatabase db;

		editorFont = db.font("Source Code Pro", "Regular", 11);
		editorFont.setFixedPitch(true);

		keywordFont = db.font("Source Code Pro", "Bold", 11);
		editorFont.setFixedPitch(true);

		// Set the tabulation length :
		tabNumberOfSpaces = 8;

		// Wrap mode :
		wrapMode = QTextOption::WordWrap;

		// Highlight : 
		enableHighlight 	= true;
		highlightCurrentLine	= true;

		updateGUI();
	}

	const QColor& 			CodeEditorSettings::getGLSLKeywordColor(void) const			{ return glslKeywordColor; }
	const QColor& 			CodeEditorSettings::getGLSLFunctionColor(void) const			{ return glslFunctionColor; }
	const QColor& 			CodeEditorSettings::getGLIPLayoutLoaderKeywordColor(void) const		{ return glipLayoutLoaderKeywordColor; }
	const QColor& 			CodeEditorSettings::getGLIPUniformLoaderKeywordColor(void) const	{ return glipUniformLoaderKeywordColor; }
	const QColor& 			CodeEditorSettings::getCommentsColor(void) const			{ return commentsColor; }
	const QFont&			CodeEditorSettings::getEditorFont(void) const				{ return editorFont; }
	const QFont& 			CodeEditorSettings::getKeywordFont(void) const				{ return keywordFont; }
	const QTextOption::WrapMode& 	CodeEditorSettings::getWrapMode(void) const				{ return wrapMode; }
	const int& 			CodeEditorSettings::getNumberOfSpacesPerTabulation(void) const		{ return tabNumberOfSpaces; }
	const bool& 			CodeEditorSettings::isHighlightEnabled(void) const			{ return enableHighlight; }
	const bool&			CodeEditorSettings::isLineHighlightEnabled(void) const			{ return highlightCurrentLine; }

	CodeEditorSettings& CodeEditorSettings::instance(void)
	{
		if(singleton==NULL)
			throw Exception("CodeEditorSettings::instance - No instance is available (internal error).", __FILE__, __LINE__);
		else
			return *singleton;
	}

