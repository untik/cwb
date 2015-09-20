#include "CryptoWorkbench.h"
#include "JavascriptInterface.h"
#include "ScriptHighlighter.h"
#include "Environment.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QDebug>

CryptoWorkbench::CryptoWorkbench(QWidget *parent)
	: isCodeChanged(false), helpWidget(NULL), QMainWindow(parent)
{
	ui.setupUi(this);
	createUi();
	loadDefaultFiles();

	Environment e;
	e.coreLibraryName = "corelib.js";
	e.coreLibraryPath = "../data/";
	e.currentScriptName = "current.js";
	e.workspaceName = "workspace";
	e.scriptLoadPath = "../scripts/";

	js = new JavascriptInterface(e, this);
}

CryptoWorkbench::~CryptoWorkbench()
{
	saveActiveScript();
}

void CryptoWorkbench::createUi()
{
	QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(ui.centralWidget);

	QSplitter* splitterV = new QSplitter(Qt::Vertical, ui.centralWidget);

	splitterV->addWidget(createWorkspaceEditor(splitterV));
	splitterV->addWidget(createCodeEditor(splitterV));
	splitterV->setSizes(QList<int>() << 400 << 300);

	mainHorizontalLayout->addWidget(splitterV);

	QFont font;
	font.setFamily("Lucida Console");
	font.setFixedPitch(true);
	font.setPointSize(12);
	codeEditor->setFont(font);
	codeEditor->setTabSize(4);
	codeEditor->setLineNumbersBackgroundColor("#2d2d30");
	codeEditor->setLineNumbersForegroundColor("#2b91af");

	workspaceEditor->setFont(font);
	workspaceEditor->setTabSize(4);
	workspaceEditor->setLineNumbersBackgroundColor("#2d2d30");
	workspaceEditor->setLineNumbersForegroundColor("#2b91af");

	QShortcut* commentShortcut = new QShortcut(QKeySequence("Ctrl+K"), this);
	connect(commentShortcut, &QShortcut::activated, this, &CryptoWorkbench::shortcutActivatedComment);

	QShortcut* uncommentShortcut = new QShortcut(QKeySequence("Ctrl+U"), this);
	connect(uncommentShortcut, &QShortcut::activated, this, &CryptoWorkbench::shortcutActivatedUncomment);

	QShortcut* runShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
	connect(runShortcut, &QShortcut::activated, this, &CryptoWorkbench::runClicked);
	QShortcut* runShortcut2 = new QShortcut(QKeySequence("F5"), this);
	connect(runShortcut2, &QShortcut::activated, this, &CryptoWorkbench::runClicked);

	QShortcut* helpShortcut = new QShortcut(QKeySequence("F1"), this);
	connect(helpShortcut, &QShortcut::activated, this, &CryptoWorkbench::helpClicked);

	ui.statusBar->showMessage("Ready");
	ui.statusBar->setStyleSheet("QStatusBar { background-color: #007acc; }");
}

QWidget* CryptoWorkbench::createWorkspaceEditor(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);

	workspaceEditor = new CodeEditor(widget);

	widgetLayout->setMargin(0);
	widgetLayout->addWidget(workspaceEditor);
	return widget;
}

QWidget* CryptoWorkbench::createCodeEditor(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
	QHBoxLayout* toolbarLayout = new QHBoxLayout();

	QPushButton* buttonOpenScript = new QPushButton("&Open...", widget);
	buttonOpenScript->setMinimumHeight(30);
	connect(buttonOpenScript, &QPushButton::clicked, this, &CryptoWorkbench::openClicked);
	toolbarLayout->addWidget(buttonOpenScript);

	QPushButton* buttonSaveAsScript = new QPushButton("&Save As...", widget);
	buttonSaveAsScript->setMinimumHeight(30);
	connect(buttonSaveAsScript, &QPushButton::clicked, this, &CryptoWorkbench::saveAsClicked);
	toolbarLayout->addWidget(buttonSaveAsScript);

	QPushButton* buttonHelp = new QPushButton("&Help", widget);
	buttonHelp->setMinimumHeight(30);
	connect(buttonHelp, &QPushButton::clicked, this, &CryptoWorkbench::helpClicked);
	toolbarLayout->addWidget(buttonHelp);

	QPushButton* buttonRunScript = new QPushButton("&Run", widget);
	buttonRunScript->setMinimumHeight(30);
	connect(buttonRunScript, &QPushButton::clicked, this, &CryptoWorkbench::runClicked);
	toolbarLayout->addWidget(buttonRunScript);

	codeEditor = new CodeEditor(widget);
	connect(codeEditor, &QPlainTextEdit::textChanged, this, &CryptoWorkbench::codeChanged);
	ScriptHighlighter* highlighter = new ScriptHighlighter(codeEditor->document(), ScriptHighlighter::StyleDark);

	widgetLayout->setMargin(0);
	widgetLayout->addWidget(codeEditor);
	widgetLayout->addLayout(toolbarLayout);
	return widget;
}

QWidget* CryptoWorkbench::createHelpViewer(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);

	//helpWidget = new QWidget(this);
	//QVBoxLayout* layout = new QVBoxLayout(helpWidget);

	//QTextEdit* helpEdit = new QTextEdit(helpWidget);
	//helpEdit->setReadOnly(true);

	//QFile helpFile("../data/help.html");
	//if (helpFile.open(QFile::ReadOnly | QFile::Text))
	//	helpEdit->setHtml(QString::fromUtf8(helpFile.readAll()));

	//layout->addWidget(helpEdit);
	return widget;
}

void CryptoWorkbench::loadDefaultFiles()
{
	QFile codeFile("../data/current.js");
	if (codeFile.open(QFile::ReadOnly | QFile::Text)) {
		codeEditor->setPlainText(QString::fromUtf8(codeFile.readAll()));
		isCodeChanged = false;
	}
}

void CryptoWorkbench::saveActiveScript()
{
	QFile codeFile("../data/current.js");
	if (codeFile.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
		codeFile.write(codeEditor->toPlainText().toUtf8());
		isCodeChanged = false;
	}
}

void CryptoWorkbench::shortcutActivatedComment()
{
	codeEditor->commentSelection();
}

void CryptoWorkbench::shortcutActivatedUncomment()
{
	codeEditor->uncommentSelection();
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
}

void CryptoWorkbench::runClicked()
{
	if (isCodeChanged)
		saveActiveScript();

	QString script = codeEditor->toPlainText();
	QString workspaceText = workspaceEditor->toPlainText();

	workspaceEditor->setPlainText("");

	QElapsedTimer timer;
	timer.start();
	ScriptResult result = js->evaluate(script, workspaceText);

	ui.statusBar->showMessage(QString("Script runtime: %1 ms").arg(timer.elapsed()));
	if (result.isValid())
		ui.statusBar->setStyleSheet("QStatusBar { background-color: #326c00; }");
	else
		ui.statusBar->setStyleSheet("QStatusBar { background-color: #6c0e00; }");

	workspaceEditor->appendPlainText(result.data());
}

void CryptoWorkbench::helpClicked()
{
	if (helpWidget == NULL) {
		helpWidget = new QWidget();
		QVBoxLayout* layout = new QVBoxLayout(helpWidget);

		QTextEdit* helpEdit = new QTextEdit(helpWidget);
		helpEdit->setReadOnly(true);

		QShortcut* closeShortcut1 = new QShortcut(QKeySequence("F1"), helpWidget);
		connect(closeShortcut1, &QShortcut::activated, helpWidget, &QWidget::hide);

		QShortcut* closeShortcut2 = new QShortcut(QKeySequence("Esc"), helpWidget);
		connect(closeShortcut2, &QShortcut::activated, helpWidget, &QWidget::hide);

		QFile helpFile("../data/help.html");
		if (helpFile.open(QFile::ReadOnly | QFile::Text))
			helpEdit->setHtml(QString::fromUtf8(helpFile.readAll()));

		layout->addWidget(helpEdit);
	}

	if (helpWidget->isVisible()) {
		helpWidget->hide();
	} else {
		QSize helpSize = helpWidget->size();
		helpWidget->show();
		helpWidget->resize(helpSize);
	}
}

void CryptoWorkbench::codeChanged()
{
	isCodeChanged = true;
}

