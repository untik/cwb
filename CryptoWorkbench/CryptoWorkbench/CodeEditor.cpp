#include "CodeEditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QDebug>

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
{
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));

	lineNumberArea = new LineNumberArea(this);

	lineNumbersBackgroundColor = Qt::lightGray;
	lineNumbersForegroundColor = Qt::black;

	updateLineNumberAreaWidth(0);
	setTabSize(4);
}

CodeEditor::~CodeEditor()
{}

void CodeEditor::setTabSize(int spaces)
{
	setTabStopWidth(fontMetrics().width(QLatin1Char('9')) * spaces);
	editorTabSize = spaces;
}

void CodeEditor::commentSelection()
{
	QTextCursor cursor = textCursor();
	SelectionInfo selection(document(), cursor);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd, QTextCursor::KeepAnchor);
	QString selected = cursor.selection().toPlainText();

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		list.replace(i, "//" + list.at(i));
	}
	selected = list.join('\n');

	cursor.insertText(selected);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd + (list.count() * 2), QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

void CodeEditor::uncommentSelection()
{
	QTextCursor cursor = textCursor();
	SelectionInfo selection(document(), cursor);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd, QTextCursor::KeepAnchor);
	QString selected = cursor.selection().toPlainText();

	int removedTotal = 0;

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		const QString& line = list.at(i);
		if (line.startsWith("//")) {
			list.replace(i, line.mid(2));
			removedTotal += 2;
		}
	}
	selected = list.join('\n');

	cursor.insertText(selected);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd - removedTotal, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
	Q_UNUSED(newBlockCount);

	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	lineNumberAreaWidth = 12 + fontMetrics().width(QLatin1Char('9')) * digits;
	setViewportMargins(lineNumberAreaWidth + 2, 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
	QPlainTextEdit::resizeEvent(event);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth, cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(lineNumberArea);
	painter.fillRect(QRect(0, 0, lineNumberAreaWidth, height()), lineNumbersBackgroundColor);
	painter.setPen(lineNumbersForegroundColor);

	// FIXME - Can't assign font directly for some reason
	QFont font2;
	font2.setFamily(font().family());
	font2.setPointSize(font().pointSize());
	font2.setFixedPitch(font().fixedPitch());
	painter.setFont(font2);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber() + 1;
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber);
			painter.drawText(0, top, lineNumberAreaWidth - 5, fontMetrics().height(), Qt::AlignRight | Qt::AlignVCenter, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			QPlainTextEdit::keyPressEvent(event);
			enterPressed();
			return;

		case Qt::Key_Tab:
			if (tabPressed())
				return;
			break;

		case Qt::Key_Backtab:
			backtabPressed();
			return;
	}

	QPlainTextEdit::keyPressEvent(event);
}

bool CodeEditor::tabPressed()
{
	QTextCursor cursor = textCursor();
	SelectionInfo selection(document(), cursor);

	// No selection
	if (selection.start == selection.end)
		return false;

	// Single line, not fully selected
	if (selection.firstBlockNumber == selection.lastBlockNumber && (selection.start != selection.firstBlockStart || selection.end != selection.lastBlockEnd))
		return false;

	// Multiple lines are selected
	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd, QTextCursor::KeepAnchor);
	QString selected = cursor.selection().toPlainText();

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		list.replace(i, "\t" + list.at(i));
	}
	selected = list.join('\n');

	cursor.insertText(selected);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd + list.count(), QTextCursor::KeepAnchor);
	setTextCursor(cursor);
	return true;
}

void CodeEditor::backtabPressed()
{
	QTextCursor cursor = textCursor();
	SelectionInfo selection(document(), cursor);

	// No selection
	if (selection.start == selection.end) {
		// At the start of a line
		if (selection.start == selection.firstBlockStart)
			return;

		// Select previous 0 - 4 characters
		int selectionColumn = selection.start - selection.firstBlockStart;
		int previousSectionLength = qMin(4, selectionColumn);

		cursor.setPosition(selection.start);
		cursor.setPosition(selection.start - previousSectionLength, QTextCursor::KeepAnchor);
		QString previousText = cursor.selectedText();

		qDebug() << selectionColumn << previousSectionLength << previousText;

		// Remove tab
		cursor.setPosition(selection.start);
		if (previousText.at(previousText.length() - 1) == "\t") {
			cursor.deletePreviousChar();
			return;
		}

		// Remove spaces
		cursor.beginEditBlock();
		for (int i = previousText.length() - 1; i >= 0; i--) {
			if (previousText.at(i) == ' ')
				cursor.deletePreviousChar();
			else
				break;
		}
		cursor.endEditBlock();
		return;
	}

	// Multiple lines are selected
	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd, QTextCursor::KeepAnchor);
	QString selected = cursor.selection().toPlainText();

	int removedTotal = 0;

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		const QString& line = list.at(i);
		int removeChars = 0;

		if (line.startsWith("    "))
			removeChars = 4;
		else if (line.startsWith("   "))
			removeChars = 3;
		else if (line.startsWith("  "))
			removeChars = 2;
		else if (line.startsWith("\t") || line.startsWith(" "))
			removeChars = 1;

		list.replace(i, line.mid(removeChars));
		removedTotal += removeChars;
	}
	selected = list.join('\n');

	cursor.insertText(selected);

	cursor.setPosition(selection.firstBlockStart);
	cursor.setPosition(selection.lastBlockEnd - removedTotal, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

void CodeEditor::enterPressed()
{
	QTextCursor cursor = textCursor();
	SelectionInfo selection(document(), cursor);

	if (selection.start != selection.end)
		return;

	if (selection.firstBlockNumber == 0)
		return;

	if (selection.start != selection.firstBlockStart)
		return;

	QTextBlock previousBlock = document()->findBlockByNumber(selection.firstBlockNumber - 1);
	QString blockText = previousBlock.text();

	// Get whitespace at the beginning of the previous block
	int whitespaceLength = 0;
	for (int i = 0; i < blockText.length(); i++) {
		QChar c = blockText.at(i);
		if (c == '\t' || c == ' ')
			whitespaceLength++;
		else
			break;
	}

	cursor.insertText(blockText.left(whitespaceLength));
}

CodeEditor::SelectionInfo::SelectionInfo(QTextDocument* document, QTextCursor cursor)
{
	start = cursor.selectionStart();
	end = cursor.selectionEnd();

	cursor.setPosition(start);
	firstBlockNumber = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	lastBlockNumber = cursor.blockNumber();

	firstBlock = document->findBlockByNumber(firstBlockNumber);
	lastBlock = document->findBlockByNumber(lastBlockNumber);

	firstBlockStart = firstBlock.position();
	lastBlockEnd = lastBlock.position() + lastBlock.length() - 1;
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
	editor->lineNumberAreaPaintEvent(event);
}
