#include "CodeEditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QTextDocumentFragment>

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
{
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));

	lineNumberArea = new LineNumberArea(this);

	lineNumbersBackgroundColor = Qt::lightGray;
	lineNumbersForegroundColor = Qt::black;

	updateLineNumberAreaWidth(0);
}

CodeEditor::~CodeEditor()
{}

void CodeEditor::setTabSize(int spaces)
{
	setTabStopWidth(fontMetrics().width(QLatin1Char('9')) * spaces);
}

void CodeEditor::commentSelection()
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	int start = cursor.selectionStart();
	int end = cursor.selectionEnd();

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
	//qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	QTextBlock blockStart = document()->findBlockByNumber(firstLine);
	int startCharIndex = blockStart.position();
	QTextBlock blockEnd = document()->findBlockByNumber(lastLine);
	int endCharIndex = blockEnd.position() + blockEnd.length() - 1;

	cursor.setPosition(startCharIndex);
	cursor.setPosition(endCharIndex, QTextCursor::KeepAnchor);

	setTextCursor(cursor);
	QTextDocumentFragment fragment = cursor.selection();
	QString selected = fragment.toPlainText();
	//qDebug() << selected;

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		list.replace(i, "//" + list.at(i));
	}
	selected = list.join('\n');

	cursor.insertText(selected);
	cursor.endEditBlock();
}

void CodeEditor::uncommentSelection()
{
	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	int start = cursor.selectionStart();
	int end = cursor.selectionEnd();

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
	//qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	QTextBlock blockStart = document()->findBlockByNumber(firstLine);
	int startCharIndex = blockStart.position();
	QTextBlock blockEnd = document()->findBlockByNumber(lastLine);
	int endCharIndex = blockEnd.position() + blockEnd.length() - 1;

	cursor.setPosition(startCharIndex);
	cursor.setPosition(endCharIndex, QTextCursor::KeepAnchor);

	setTextCursor(cursor);
	QTextDocumentFragment fragment = cursor.selection();
	QString selected = fragment.toPlainText();
	//qDebug() << selected;

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		QString line = list.at(i);
		if (line.startsWith("//"))
			list.replace(i, line.mid(2));
	}
	selected = list.join('\n');

	cursor.insertText(selected);
	cursor.endEditBlock();
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
	//switch (event->key()) {
	//	case Qt::Key_Enter:
	//	case Qt::Key_Return:
	//	case Qt::Key_Escape:
	//	case Qt::Key_Tab:
	//	case Qt::Key_Backtab:
	//		//event->ignore();
	//		//return; // let the completer do default behavior
	//	default:
	//		event->ignore();
	//		break;
	//}

	QPlainTextEdit::keyPressEvent(event);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
	editor->lineNumberAreaPaintEvent(event);
}
