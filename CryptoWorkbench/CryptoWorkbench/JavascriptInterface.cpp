#include "JavascriptInterface.h"
#include "WorkbenchEngine.h"

JavascriptInterface::JavascriptInterface(const QString& coreLibraryPath, QObject* parent)
{
	engine = new WorkbenchEngine(coreLibraryPath);
}

JavascriptInterface::~JavascriptInterface()
{
	delete engine;
}

ScriptResult JavascriptInterface::evaluate(const QString& scriptText, const QString& workspaceText)
{
	return engine->evaluate(scriptText, workspaceText);
}
