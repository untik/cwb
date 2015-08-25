#include "CryptoWorkbench.h"
#include "JavascriptInterface.h"
#include "ScriptHighlighter.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QTextDocumentFragment>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QDebug>

CryptoWorkbench::CryptoWorkbench(QWidget *parent)
	: isCodeChanged(false), QMainWindow(parent)
{
	ui.setupUi(this);
	createUi();

	js = new JavascriptInterface(this);
	connect(js, &JavascriptInterface::error, this, &CryptoWorkbench::scriptError);

	//js->testV8();
	//qDebug() << "Result:" << js->evaluate("var a = 50; function add(x,y) { return x + y; }; add(a,10) + 10;");
	codeEditor->setPlainText("var a = 50; \nfunction add(x,y) { return x + y; }; \nadd(a,10) + 10;");
}

CryptoWorkbench::~CryptoWorkbench()
{
}

void CryptoWorkbench::createUi()
{
	QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(ui.centralWidget);

	QSplitter* splitterV = new QSplitter(Qt::Vertical, ui.centralWidget);

	QSplitter* splitterH = new QSplitter(Qt::Horizontal, splitterV);
	splitterH->addWidget(createLeftEditor(splitterH));
	splitterH->addWidget(createRightEditor(splitterH));

	splitterV->addWidget(splitterH);
	splitterV->addWidget(createCodeEditor(splitterV));
	splitterV->setSizes(QList<int>() << 400 << 200);

	mainHorizontalLayout->addWidget(splitterV);

	QFont font;
	font.setFamily("Lucida Console");
	font.setFixedPitch(true);
	font.setPointSize(11);
	codeEditor->setFont(font);
	codeEditor->setTabSize(4);

	leftEditor->setFont(font);
	rightEditor->setFont(font);

	QShortcut* commentShortcut = new QShortcut(QKeySequence("Ctrl+K"), this);
	connect(commentShortcut, &QShortcut::activated, this, &CryptoWorkbench::shortcutActivatedComment);

	QShortcut* uncommentShortcut = new QShortcut(QKeySequence("Ctrl+U"), this);
	connect(uncommentShortcut, &QShortcut::activated, this, &CryptoWorkbench::shortcutActivatedUncomment);
}

QWidget* CryptoWorkbench::createLeftEditor(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
	QLabel* label = new QLabel("Left (Source)", widget);

	leftEditor = new CodeEditor(widget);

	widgetLayout->setMargin(0);
	widgetLayout->addWidget(label);
	widgetLayout->addWidget(leftEditor);
	return widget;
}

QWidget* CryptoWorkbench::createRightEditor(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
	QLabel* label = new QLabel("Right (Output)", widget);

	rightEditor = new CodeEditor(widget);

	widgetLayout->setMargin(0);
	widgetLayout->addWidget(label);
	widgetLayout->addWidget(rightEditor);
	return widget;
}

QWidget* CryptoWorkbench::createCodeEditor(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
	QHBoxLayout* toolbarLayout = new QHBoxLayout(widget);

	codeEditorLabel = new QLabel("Script", widget);
	toolbarLayout->addWidget(codeEditorLabel);

	QPushButton* buttonOpenScript = new QPushButton("&Open...", widget);
	buttonOpenScript->setMinimumHeight(30);
	connect(buttonOpenScript, &QPushButton::clicked, this, &CryptoWorkbench::openClicked);
	toolbarLayout->addWidget(buttonOpenScript);

	QPushButton* buttonSaveAsScript = new QPushButton("&Save As...", widget);
	buttonSaveAsScript->setMinimumHeight(30);
	connect(buttonSaveAsScript, &QPushButton::clicked, this, &CryptoWorkbench::saveAsClicked);
	toolbarLayout->addWidget(buttonSaveAsScript);

	QPushButton* buttonRunScript = new QPushButton("&Run", widget);
	buttonRunScript->setMinimumHeight(30);
	connect(buttonRunScript, &QPushButton::clicked, this, &CryptoWorkbench::runClicked);
	toolbarLayout->addWidget(buttonRunScript);

	codeEditor = new CodeEditor(widget);
	connect(codeEditor, &QPlainTextEdit::textChanged, this, &CryptoWorkbench::codeChanged);
	ScriptHighlighter* highlighter = new ScriptHighlighter(codeEditor->document());

	widgetLayout->setMargin(0);
	widgetLayout->addLayout(toolbarLayout);
	widgetLayout->addWidget(codeEditor);
	return widget;
}

void CryptoWorkbench::updateScriptLabel()
{
	if (currentFileName.isEmpty()) {
		codeEditorLabel->setText("Script");
	} else {
		if (isCodeChanged)
			codeEditorLabel->setText("Script (" + currentFileName + "*)");
		else
			codeEditorLabel->setText("Script (" + currentFileName + ")");
	}
}

void CryptoWorkbench::scriptError(const QString& errorString)
{
	rightEditor->appendPlainText(errorString + "\n");
	//qDebug() << "CryptoWorkbench::scriptError" << errorString;
}

void CryptoWorkbench::shortcutActivatedComment()
{
	QTextCursor cursor = codeEditor->textCursor();
	int start = cursor.selectionStart();
	int end = cursor.selectionEnd();

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
	//qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	QTextBlock blockStart = codeEditor->document()->findBlockByNumber(firstLine);
	int startCharIndex = blockStart.position();
	QTextBlock blockEnd = codeEditor->document()->findBlockByNumber(lastLine);
	int endCharIndex = blockEnd.position() + blockEnd.length() - 1;

	cursor.setPosition(startCharIndex);
	cursor.setPosition(endCharIndex, QTextCursor::KeepAnchor);

	codeEditor->setTextCursor(cursor);
	QTextDocumentFragment fragment = cursor.selection();
	QString selected = fragment.toPlainText();
	//qDebug() << selected;

	QStringList list = selected.split('\n');
	for (int i = 0; i < list.count(); i++) {
		list.replace(i, "//" + list.at(i));
	}
	selected = list.join('\n');

	cursor.insertText(selected);
}

void CryptoWorkbench::shortcutActivatedUncomment()
{
	QTextCursor cursor = codeEditor->textCursor();
	int start = cursor.selectionStart();
	int end = cursor.selectionEnd();

	cursor.setPosition(start);
	int firstLine = cursor.blockNumber();
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	int lastLine = cursor.blockNumber();
	//qWarning() << "start: " << firstLine << " end: " << lastLine << endl;

	QTextBlock blockStart = codeEditor->document()->findBlockByNumber(firstLine);
	int startCharIndex = blockStart.position();
	QTextBlock blockEnd = codeEditor->document()->findBlockByNumber(lastLine);
	int endCharIndex = blockEnd.position() + blockEnd.length() - 1;

	cursor.setPosition(startCharIndex);
	cursor.setPosition(endCharIndex, QTextCursor::KeepAnchor);

	codeEditor->setTextCursor(cursor);
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
}

void CryptoWorkbench::openClicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Script"), "../scripts/", tr("Script Files (*.js)"));
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QByteArray data = file.readAll();
		codeEditor->setPlainText(QString::fromUtf8(data));
	}

	currentFileName = QFileInfo(fileName).fileName();
	isCodeChanged = false;
	updateScriptLabel();
}

void CryptoWorkbench::saveAsClicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Script"), "../scripts/", tr("Script Files (*.js)"));
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		file.write(codeEditor->toPlainText().toUtf8());
	}

	currentFileName = QFileInfo(fileName).fileName();
	isCodeChanged = false;
	updateScriptLabel();
}

void CryptoWorkbench::runClicked()
{
	QString script = codeEditor->toPlainText();
	QString input = leftEditor->toPlainText();
	QString output = rightEditor->toPlainText();
	rightEditor->setPlainText("");

	QElapsedTimer timer;
	timer.start();
	QString result = js->evaluate(script, input, output);
	ui.statusBar->showMessage(QString("Script runtime: %1 ms").arg(timer.elapsed()));

	rightEditor->appendPlainText(result);
}

void CryptoWorkbench::codeChanged()
{
	isCodeChanged = true;
	updateScriptLabel();
}
