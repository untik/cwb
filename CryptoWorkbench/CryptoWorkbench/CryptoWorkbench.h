#ifndef CRYPTOWORKBENCH_H
#define CRYPTOWORKBENCH_H

#include <QtWidgets/QMainWindow>
#include "ui_CryptoWorkbench.h"
#include "CodeEditor.h"

class JavascriptInterface;
class QLabel;


class CryptoWorkbench : public QMainWindow
{
	Q_OBJECT

public:
	CryptoWorkbench(QWidget *parent = 0);
	~CryptoWorkbench();

private:
	void createUi();
	QWidget* createWorkspaceEditor(QWidget* parent);
	QWidget* createCodeEditor(QWidget* parent);
	QWidget* createHelpViewer(QWidget* parent);
	void loadDefaultFiles();
	void saveActiveScript();

private slots:
	void shortcutActivatedComment();
	void shortcutActivatedUncomment();
	void openClicked();
	void saveAsClicked();
	void runClicked();
	void helpClicked();
	void codeChanged();

private:
	Ui::CryptoWorkbenchClass ui;
	JavascriptInterface* js;
	CodeEditor* workspaceEditor;
	CodeEditor* codeEditor;
	CodeEditor* consoleEditor;
	QString currentFileName;
	bool isCodeChanged;
	QWidget* helpWidget;
};

#endif // CRYPTOWORKBENCH_H
