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

		// GLIP LayoutLoader Keywords : 
			glipLayoutLoaderKeywordFormat.setForeground(QColor(255, 51, 255));
			glipLayoutLoaderKeywordFormat.setFontWeight(QFont::Bold);
			QStringList glipllkeywordPatterns;

			for(int i=0; i<Glip::Modules::LL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + Glip::Modules::keywordsLayoutLoader[i] + "\\b";
				glipllkeywordPatterns << str.c_str();
			}

			foreach (const QString& pattern, glipllkeywordPatterns)
			{
				rule.pattern = QRegExp(pattern);
				rule.format = glipLayoutLoaderKeywordFormat;
				highlightingRules.append(rule);
			}

		// GLIP Uniform Loader Keywords : 
			glipUniformLoaderKeywordFormat.setForeground(QColor(51, 255, 255));
			glipUniformLoaderKeywordFormat.setFontWeight(QFont::Bold);
			QStringList glipulkeywordPatterns;

			for(int i=0; i<Glip::Modules::UL_NumKeywords; i++)
			{
				std::string str = std::string("\\b") + Glip::Modules::keywordsUniformsVarsLoader[i] + "\\b";
				glipulkeywordPatterns << str.c_str();
			}

			foreach (const QString& pattern, glipulkeywordPatterns)
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
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		font.setPointSize(currentFontSize);
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

// PathWidget
	PathWidget::PathWidget(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), addPathAct(tr("Add path"), this), removePathAct(tr("Remove path"), this), clearAllPathAct(tr("Clear all paths"), this)
	{
		connect(&addPathAct, 		SIGNAL(triggered()), this, SLOT(addPath()));
		connect(&removePathAct, 	SIGNAL(triggered()), this, SLOT(removePath()));
		connect(&clearAllPathAct, 	SIGNAL(triggered()), this, SLOT(clearAll()));

		menuBar.addAction(&addPathAct);
		menuBar.addAction(&removePathAct);
		menuBar.addAction(&clearAllPathAct);

		data.setSelectionMode(QAbstractItemView::ExtendedSelection);
		data.setAlternatingRowColors(true);

		QFont font;
		font.setFamily("Monospace");
		font.setFixedPitch(true);
		data.setFont(font);

		layout.addWidget(&menuBar);
		layout.addWidget(&data);

		// Load path : 
		std::fstream file;
		file.open("./paths.txt", std::fstream::in);
		
		if(file.is_open() && file.good() && !file.fail())
		{
			addPath("./");

			std::string 	line;

			file.seekg(0, std::ios::beg);

			while(std::getline(file,line))
			{
				// Insert the path : 
				addPath(line);
			}

			file.close();
		}
		else
			clearAll();
	}

	PathWidget::~PathWidget(void)
	{
		if(paths.size()>1)
		{
			// Write to file : 
			std::fstream file;
			file.open("./paths.txt", std::fstream::out | std::fstream::trunc);

			for(int k=1; k<paths.size(); k++)
				file << paths[k] << "\n";

			file.close();
		}

		clearAll();
	}

	void PathWidget::addPath(void)
	{
		QString directoryName = QFileDialog::getExistingDirectory(this, tr("Add directory to paths"), ".");

		if(!directoryName.isEmpty())
		{
			QString slashed = QDir::fromNativeSeparators( directoryName ) + "/";
		
			addPath( slashed.toStdString() );
		}
	}

	void PathWidget::removePath(void)
	{
		QList<QListWidgetItem*> selectedItems = data.selectedItems();
		
		if( !selectedItems.isEmpty() )
		{
			for(int k=0; k<selectedItems.count(); k++)
			{
				int p = data.row(selectedItems[k]);

				if(p>0)
				{
					paths.erase( paths.begin() + p );
					QListWidgetItem* item = data.takeItem(p);
					delete item;
				}
			}
		}
	}

	void PathWidget::clearAll(void)
	{
		paths.clear();

		// Clean the list : 
		while(data.count()>0)
		{
			QListWidgetItem* item = data.takeItem(0);
			delete item;
		}

		// Add : 
		addPath("./");
	}

	void PathWidget::addPath(std::string newPath)
	{
		if(newPath.empty())
			return ;

		// Cut : 
		size_t p = newPath.find_last_not_of(" \t\r\n");
		
		if(p!=std::string::npos)
			newPath = newPath.substr(0, p+1);

		if(newPath[newPath.size()-1]!='/')
			newPath += '/';

		// Find if a double exists : 
		std::vector<std::string>::iterator it = std::find(paths.begin(), paths.end(), newPath);

		if(it==paths.end())
		{
			paths.push_back( newPath );
			data.addItem( newPath.c_str() );
		}
	}
	
	const std::vector<std::string>& PathWidget::getPaths(void) const
	{
		return paths;
	}

// TemplateMenu : 
	const char* TemplateMenu::templatesName[numTemplates]		= { 	"Add path",
										"Include file",
										"Required format", 
										"Required pipeline", 
										"Texture format",
										"Shared source", 
										"Shader source",
										"Shader source + GLSL fragment",
										"Include shared source",
										"Include shader",
										"Filter layout", 
										"Pipeline layout", 
										"Main pipeline layout",
										"Input ports", 
										"Output ports", 
										"Filter instance",
										"Pipeline instance", 
										"Connection",
										"Full Pipeline structure",
										"Full MainPipeline structure"
									};

	const char* TemplateMenu::templatesCode[numTemplates]		= {	"ADD_PATH( )\n",
										"INCLUDE_FILE( )\n",
										"REQUIRED_FORMAT: ( )\n",
										"REQUIRED_PIPELINE: ( )\n",
										"TEXTURE_FORMAT: ( , , GL_RGB, GL_UNSIGNED_BYTE)\n",
										"SHARED_SOURCE: \n{\n\t\n}\n",
										"SHADER_SOURCE: \n{\n\t\n}\n",
										"SHADER_SOURCE: \n{\n\t#version 130\n\t\n\tuniform sampler2D ;\n\tout vec4 ;\n\t\n\tvoid main()\n\t{\n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\tvec4 col = textureLod( , pos, 0);\n\t\n\t\t = col;\n\t}\n}\n",
										"INCLUDE_SHARED_SOURCE:\n",
										"SHADER_SOURCE: ()\n",
										"FILTER_LAYOUT: ( , )\n",
										"PIPELINE_LAYOUT: \n{\n\t\n}\n",
										"PIPELINE_MAIN: \n{\n\t\n}\n",
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
										"REQUIRED_FORMAT: /* name to use */( /* required format name */ )\n",
										"REQUIRED_PIPELINE: /* name to use */ ( /* required pipeline name */ )\n",
										"TEXTURE_FORMAT: /* name */ ( /* width */, /* height */, GL_RGB, GL_UNSIGNED_BYTE)\n",
										"SHARED_SOURCE: /* name */\n{\n\t/* Code */\n}\n",
										"SHADER_SOURCE: /* name */\n{\n\t/* Code */\n}\n",
										"SHADER_SOURCE: /* name */\n{\n\t#version 130\n\t\n\tuniform sampler2D /* input texture name */;\n\tout vec4 /* output texture name */;\n\t\n\t// uniform vec3 someVariableYouWantToModify = vec3(0.0, 0.0, 0.0);\n\t\n\tvoid main()\n\t{\n\t\t// The current fragment position : \n\t\tvec2 pos = gl_TexCoord[0].st;\n\t\t// Read the base level of the texture at the current position : \n\t\tvec4 col = textureLod(/* input texture name */, pos, 0);\n\t\n\t\t/* output texture name */ = col;\n\t}\n}\n",
										"INCLUDE_SHARED_SOURCE:/* shared source name */\n",
										"SHADER_SOURCE: /* name */( /* filename */ )\n",
										"FILTER_LAYOUT: ( /* output texture format name */, /* shader name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\t/* structure code */\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\t/* structure code */\n}\n",
										"INPUT_PORTS( /* ports names list */ )\n",
										"OUTPUT_PORTS( /* ports names list */ )\n",
										"FILTER_INSTANCE: /* instance name */ ( /* filter layout name */ )\n",
										"PIPELINE_INSTANCE: /* instance name */ ( /* pipeline layout name */ )\n",
										"CONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */)\n",
										"PIPELINE_LAYOUT: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNETION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n",
										"PIPELINE_MAIN: /* name */\n{\n\tINPUT_PORTS( /* list of ports names */ )\n\tOUTPUT_PORTS( /* list of ports names */ )\n\n\tFILTER_INSTANCE: /* instance name */ ( /* layout name */ )\n\n\t// Note that in the case that your connections names are not ambiguous, you are allowed to not declare any CONNETION. Connections will be made automatically.\n\tCONNECTION( /* source instance name or THIS (for input ports) */, /* port name */, /* source instance name or THIS (for output ports) */, /* port name */ )\n\tCONNECTION( /* source instance name or THIS (for input port) */, /* port name */, /* source instance name or THIS (for output port) */, /* port name */ )\n}\n"
									};

	TemplateMenu::TemplateMenu(QWidget* parent)
	 : QMenu("Insert Template", parent), signalMapper(this),  addComments("Option : insert comments", this), lastInsertionID(tplUnknown)
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
		lastInsertionID = static_cast<CodeTemplates>(k);
		emit insertTemplate();
	}

	QString TemplateMenu::getTemplateCode(void)
	{
		if(lastInsertionID>=0 && lastInsertionID<numTemplates)
		{
			QString res;

			if(addComments.isChecked())
				res = templatesCodeWithHelp[lastInsertionID];
			else
				res = templatesCode[lastInsertionID];

			lastInsertionID = tplUnknown;
			return res;
		}
		else
		{
			lastInsertionID = tplUnknown;
			return "";
		}
	}

// NotificationsBar
	const char* NotificationsBar::notificationClassesColors[NumNotificationClasses]				= {	"#EE3333",
															"#BB7722",
															"#333333",
															"#33BB33"};
	const bool NotificationsBar::canbeHidden[NumNotificationClasses]					= {	false,
															true,
															true,
															true};

	const char* NotificationsBar::notificationMessages[NumNotifications]					= {	"Some files are modified but not saved.",
															"Some files are not saved to the disk.",
															"Current file is modified but not saved.",
															"Current file is not saved to the disk.",	
															"Current file is empty.",
															"Everything is all right."};
	const NotificationsBar::NotificationClass NotificationsBar::notificationClasses[NumNotifications]	= {	NotificationWarning,
															NotificationWarning,
															NotificationWarning,
															NotificationError,
															NotificationError,
															NotificationAllRight};
	
	NotificationsBar::NotificationsBar(QWidget* parent)
	 : QWidget(parent), layout(this), notification(this), hideNotification(this), hideCheckBox(this), lastNotification(NoNotification)
	{
		notification.setText("No notification");
		hideNotification.setText("<i>Hide future similar notifications :</i>");
		hideCheckBox.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		hideNotification.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout.addWidget(&notification);
		layout.addWidget(&hideNotification);
		layout.addWidget(&hideCheckBox);		

		for(int k=0; k<NumNotifications; k++)
			isHidden[k] = false;

		connect(&hideCheckBox, SIGNAL(stateChanged(int)), this, SLOT(turnDownCurrentNotification(void)));
	}

	void NotificationsBar::turnDownCurrentNotification(void)
	{	
		// Lock : 
		bool previousState = this->blockSignals(true);
		hideCheckBox.setChecked(false);
		this->blockSignals(previousState);
	
		if(lastNotification>=0 && lastNotification<NoNotification)
		{
			if( canbeHidden[ notificationClasses[ lastNotification ] ] )
			{
				isHidden[lastNotification] = true;
				emit showMe(false);
			}
		}
	}

	void NotificationsBar::declare(const Notification& n)
	{
		if(n>=0 && n<NoNotification)
		{
			if(!isHidden[n])
			{
				// Put the message :
				notification.setText( tr("  %1").arg( notificationMessages[n]) );
				notification.setStyleSheet(tr("QLabel { font: bold; background-color : qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 %1, stop: 1 #333333); color : white; }").arg( notificationClassesColors[notificationClasses[n]] ) );

				notification.show();
				hideNotification.show();
				hideCheckBox.show();

				if( canbeHidden[ notificationClasses[n] ] )
					hideCheckBox.setDisabled(false);
				else
					hideCheckBox.setDisabled(true);

				lastNotification = n;

				emit showMe(true);
			}
			else
				emit showMe(false);
		}
		else if(n==NoNotification)
			emit showMe(false);
		else
			throw Exception("Internal error : notification ID out of range.", __FILE__, __LINE__);
	}

// CodeEditorsPannel
	CodeEditorsPannel::CodeEditorsPannel(QWidget* parent)
	 : QWidget(parent), layout(this), menuBar(this), widgets(this), fileMenu("File", this), templateMenu(this), 
	   newTabAct(tr("&New tab"), this), saveAct(tr("&Save"), this), saveAsAct(tr("Save as"), this), saveAllAct(tr("Save all"), this), openAct(tr("&Open"), this), refreshAct("&Refresh | Compile", this), closeTabAct(tr("&Close"), this), showPathWidget(tr("Paths"),this), pathWidget(this), closeAllAct(tr("Close all"), this), notificationBar(this), aboutAct(tr("About"), this)
	{
		// Add the actions : 
		newTabAct.setShortcuts(QKeySequence::New);
		connect(&newTabAct, SIGNAL(triggered()), this, SLOT(newTab()));

		openAct.setShortcuts(QKeySequence::Open);
		connect(&openAct, SIGNAL(triggered()), this, SLOT(open()));

		saveAct.setShortcuts(QKeySequence::Save);
		connect(&saveAct, SIGNAL(triggered()), this, SLOT(save()));

		saveAsAct.setShortcuts(QKeySequence::SaveAs);
		connect(&saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

		connect(&saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));

		connect(&closeAllAct, SIGNAL(triggered()), this, SLOT(closeAll()));

		connect(&templateMenu, SIGNAL(insertTemplate()), this, SLOT(insertTemplate()));

		QList<QKeySequence> refreshShortcuts;
		refreshShortcuts.push_back(QKeySequence(tr("Ctrl+r")));
		refreshShortcuts.push_back(QKeySequence(tr("F5")));
		refreshAct.setShortcuts(refreshShortcuts);
		connect(&refreshAct, SIGNAL(triggered()), this, SLOT(refresh()));

		closeTabAct.setStatusTip(tr("Close"));
		closeTabAct.setShortcuts(QKeySequence::Close);
		connect(&closeTabAct, SIGNAL(triggered()), this, SLOT(closeTab()));

		connect(&notificationBar, SIGNAL(showMe(bool)), this, SLOT(switchNotification(bool)));

		showPathWidget.setStatusTip(tr("Show paths"));
		connect(&showPathWidget, SIGNAL(triggered()), this, SLOT(switchPathWidget()));

		connect(&aboutAct, SIGNAL(triggered()), this, SLOT(aboutMessage()));

		// Movable : 	
		widgets.setMovable(true);

		// Menus :
		fileMenu.addAction(&newTabAct);
		fileMenu.addAction(&openAct);
		fileMenu.addAction(&saveAct);
		fileMenu.addAction(&saveAsAct);
		fileMenu.addAction(&saveAllAct);
		fileMenu.addAction(&closeTabAct);
		fileMenu.addAction(&closeAllAct);
	
		menuBar.addMenu(&fileMenu);
		menuBar.addMenu(&templateMenu);
		menuBar.addAction(&refreshAct);
		menuBar.addAction(&showPathWidget);
		menuBar.addAction(&aboutAct);

		// Add the first tab : 
		newTab();

		// Hide paths : 
		pathWidget.hide();

		layout.addWidget(&menuBar);
		layout.addWidget(&notificationBar);	
		layout.addWidget(&widgets);
		layout.addWidget(&pathWidget);

		notificationBar.hide();
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

		switchNotification(false);
	}

	void CodeEditorsPannel::open(void)
	{
		QString currentPath;

		if(widgets.count()>0)
		{
			int c = widgets.currentIndex();
			currentPath = tabs[c]->path();
		}
		else
			currentPath = "./";

		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Pipeline Script Files"), currentPath, tr("Pipeline Script Files (*.ppl *.glsl *.ext *.uvd)"));

		for(int k=0; k<filenames.count(); k++)
		{
			if(!filenames[k].isEmpty())
			{
				if(widgets.count() > 0)
				{
					int c = widgets.currentIndex();
					if(!tabs[c]->empty())
						newTab();
				}
				else
					newTab();

				int c = widgets.currentIndex();
		
				tabs[c]->setFilename( filenames[k] );
				if(!tabs[c]->load())
					closeTab();
				else
				{
					// Append the path : 
					pathWidget.addPath(tabs[c]->path().toStdString());
				}
			}
		}
		
		switchNotification(false);
	}

	void CodeEditorsPannel::save(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			if(tabs[c]->filename().isEmpty())
			{
				QString currentPath = tabs[c]->path();

				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pipeline Script File"), currentPath, tr("Pipeline Script Files (*.ppl *.glsl *.ext *.uvd)"));

				if(fileName.isEmpty())
					return ;
				else
					tabs[c]->setFilename(fileName);
			}

			tabs[c]->save();

			switchNotification(false);
		}
	}

	void CodeEditorsPannel::saveAs(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();
		
			QString currentPath = tabs[c]->path();
	
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pipeline Script File"), currentPath, tr("Pipeline Script Files (*.ppl *.glsl *.ext *.uvd)"));
		
			if(!fileName.isEmpty())
			{
				tabs[c]->setFilename(fileName);
				tabs[c]->save();
	
				switchNotification(false);
			}
		}
	}

	void CodeEditorsPannel::saveAll(void)
	{
		int original = widgets.currentIndex();

		for(int k=0; k<tabs.size(); k++)
		{
			if(tabs[k]->filename().isEmpty())
			{
				widgets.setCurrentIndex(k);

				QString currentPath = tabs[k]->path();

				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pipeline Script File"), currentPath, tr("Pipeline Script Files (*.ppl *.glsl *.ext *.uvd)"));

				if(!fileName.isEmpty())
				{
					tabs[k]->setFilename(fileName);
					tabs[k]->save();

					switchNotification(false);
				}
			}
			else
			{
				tabs[k]->save();
				widgets.setCurrentIndex(k);
			}
		}

		if(!tabs.empty())
			widgets.setCurrentIndex(original);
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
				switchNotification(false);

				widgets.removeTab(c);
				delete tabs[c];

				tabs.erase(tabs.begin() + c);
			}
		}
	}

	void CodeEditorsPannel::closeAll(void)
	{
		switchNotification(false);

		for(int k=0; k<tabs.size(); k++)
		{
			widgets.setCurrentIndex(k);

			if(tabs[k]->canBeClosed())
			{
				widgets.removeTab(k);
				delete tabs[k];

				tabs.erase(tabs.begin() + k);
			}
		}
	}

	void CodeEditorsPannel::switchPathWidget(void)
	{
		if(pathWidget.isVisible())
		{
			pathWidget.hide();
			showPathWidget.setText("Paths");			
		}
		else
		{
			pathWidget.show();
			showPathWidget.setText("Hide paths");
		}
	}

	void CodeEditorsPannel::switchNotification(bool t)
	{
		if(t)
			notificationBar.show();
		else
			notificationBar.hide();
	}

	void CodeEditorsPannel::updateTitles(void)
	{
		for(int k=0; k<tabs.size(); k++)
			widgets.setTabText(k, tabs[k]->getTitle());
	}

	void CodeEditorsPannel::insertTemplate(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			tabs[c]->insert( templateMenu.getTemplateCode() );
		}
	}

	void CodeEditorsPannel::aboutMessage(void)
	{
		int pointSize = font().pointSize();

		QMessageBox::about(this, "GlipStudio", tr("<center><p style=\"font-family: times, serif; font-size:%2pt; font-style:bold\">GlipStudio</p></center><p style=\"font-family: times, serif; font-size:%1pt; font-style:bold\">GlipStudio is the IDE for GlipLib (OpenGL Image Processing Library). Find more information, documentation and examples at : <a href='http://glip-lib.sourceforge.net/'>http://glip-lib.sourceforge.net/</a>.</p><center><p style=\"font-size:%3pt; font-style:italic\">Copyright &copy; 2013, Ronan Kerviche, MIT License</p></center><p style=\"font-size:%4pt;\"> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: <BR> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. <BR> THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p><table><tr><td><b><i>Binary build date</i></b> : </td><td>%5; %6</td></tr><tr><td><b><i>Hardware vendor : </i></b></td><td>%7</td></tr><tr><td><b><i>Renderer : </i></b></td><td>%8</td></tr><tr><td><b><i>OpenGL version : </i></b></td><td>%9</td></tr><tr><td><b><i>GLSL version : </i></b></td><td>%10</td></tr></table>").arg(pointSize).arg(pointSize+4).arg(pointSize+1).arg(pointSize-2).arg(__DATE__).arg(__TIME__).arg(HandleOpenGL::getVendorName().c_str()).arg(HandleOpenGL::getRendererName().c_str()).arg(HandleOpenGL::getVersion().c_str()).arg(HandleOpenGL::getGLSLVersion().c_str()));
	}

	std::string CodeEditorsPannel::getCurrentFilename(void)
	{
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			if(tabs[c]->empty())
			{
				notificationBar.declare(NotificationsBar::Error_CurrentIsEmpty);
				return "";
			}

			bool 	atLeastOneModified 	= false,
				atLeastOneNotOnDisk	= false,
				mainNotSaved		= false,
				mainNotOnDisk		= false;

			for(int k=0; k<tabs.size(); k++)
			{
				if(k!=c && tabs[k]->filename().isEmpty() && !tabs[k]->empty())
					atLeastOneNotOnDisk = true;
				else if(k!=c && tabs[k]->isModified() && !tabs[k]->empty())
					atLeastOneNotOnDisk = true;
				else if(k==c && tabs[k]->filename().isEmpty())
					mainNotOnDisk = true;
				else if(k==c && tabs[k]->isModified())
					mainNotSaved = true;

			}

			if(mainNotOnDisk)
			{
				notificationBar.declare(NotificationsBar::Error_CurrentFileIsNotOnDisk);
				return "";
			}
			else if(mainNotSaved)
			{
				notificationBar.declare(NotificationsBar::Error_CurrentFileIsNotSaved);
				return "";
			}
			else if(atLeastOneModified)
				notificationBar.declare(NotificationsBar::Warning_SomeFilesAreNotSaved);
			else if(atLeastOneNotOnDisk)
				notificationBar.declare(NotificationsBar::Warning_SomeFilesAreNotOnDisk);
			else
				notificationBar.declare();
					

			return tabs[c]->filename().toStdString();
		}
		else
			return "";	
	}

	const std::vector<std::string>& CodeEditorsPannel::getPaths(void)
	{
		// Make sure the list of path contains the one of the current pipeline :
		if(widgets.count() > 0)
		{
			int c = widgets.currentIndex();

			pathWidget.addPath( tabs[c]->path().toStdString() );
		}

		return pathWidget.getPaths();
	}

	bool CodeEditorsPannel::canBeClosed(void)
	{
		bool test = true;

		for(int i=0; i<tabs.size() && test; i++)
		{
			widgets.setCurrentIndex(i);
			test = test && tabs[i]->canBeClosed();	
		}

		return test;
	}

