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

protected:
	void resizeEvent(QResizeEvent* event);
	void lineNumberAreaPaintEvent(QPaintEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect& rect, int dy);

private:
	int lineNumberAreaWidth;
	LineNumberArea* lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *scriptEditor)
		: editor(scriptEditor), QWidget(scriptEditor)
	{}

protected:
	void paintEvent(QPaintEvent* event);

private:
	CodeEditor* editor;
};

#endif // CODE_EDITOR_H
