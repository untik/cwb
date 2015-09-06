#include "JavascriptInterface.h"
#include "WorkbenchEngine.h"

JavascriptInterface::JavascriptInterface(const Environment& environment, QObject* parent)
{
	engine = new WorkbenchEngine(environment);
}

JavascriptInterface::~JavascriptInterface()
{
	delete engine;
}

ScriptResult JavascriptInterface::evaluate(const QString& scriptText, const QString& workspaceText)
{
	return engine->evaluate(scriptText, workspaceText);
}
