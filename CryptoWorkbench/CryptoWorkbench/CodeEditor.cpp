#include "CodeEditor.h"
#include <QPainter>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
{
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));

	lineNumberArea = new LineNumberArea(this);

	updateLineNumberAreaWidth(0);
}

CodeEditor::~CodeEditor()
{}

void CodeEditor::setTabSize(int spaces)
{
	setTabStopWidth(fontMetrics().width(QLatin1Char('9')) * spaces);
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
	setViewportMargins(lineNumberAreaWidth, 0, 0, 0);
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
	painter.fillRect(QRect(0, 0, lineNumberAreaWidth, height()), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, lineNumberAreaWidth - 5, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
	editor->lineNumberAreaPaintEvent(event);
}
