#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

////////////////////////////////////////////////////////////////////////////////////
///
/// Extension to QPlainTextEdit showing line numbers
///
////////////////////////////////////////////////////////////////////////////////////
class CodeEditor : public QPlainTextEdit
{
	friend class LineNumberArea;
	Q_OBJECT

public:
	CodeEditor(QWidget* parent = 0);
	~CodeEditor();

	void setTabSize(int spaces = 4);

	void setLineNumbersBackgroundColor(const QColor& color) { lineNumbersBackgroundColor = color; }
	void setLineNumbersForegroundColor(const QColor& color) { lineNumbersForegroundColor = color; }

	void commentSelection();
	void uncommentSelection();

protected:
	void resizeEvent(QResizeEvent* event);
	void lineNumberAreaPaintEvent(QPaintEvent* event);
	void keyPressEvent(QKeyEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect& rect, int dy);

private:
	int lineNumberAreaWidth;
	LineNumberArea* lineNumberArea;
	QColor lineNumbersBackgroundColor;
	QColor lineNumbersForegroundColor;
};


////////////////////////////////////////////////////////////////////////////////////
///
/// Widget for catching paint event for code editor line number area
/// Used internally by CodeEditor
///
////////////////////////////////////////////////////////////////////////////////////
class LineNumberArea : public QWidget
{
	friend class CodeEditor;

private:
	LineNumberArea(CodeEditor *scriptEditor)
		: editor(scriptEditor), QWidget(scriptEditor)
	{}

protected:
	void paintEvent(QPaintEvent* event);

private:
	CodeEditor* editor;
};

#endif // CODE_EDITOR_H
