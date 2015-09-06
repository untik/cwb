#ifndef JAVASCRIPTINTERFACE_H
#define JAVASCRIPTINTERFACE_H

#include <QString>
#include <QObject>
#include "ScriptResult.h"
#include "Environment.h"

class WorkbenchEngine;

////////////////////////////////////////////////////////////////////////////////////
///
/// Class wrapping WorkbenchEngine
///
////////////////////////////////////////////////////////////////////////////////////
class JavascriptInterface : public QObject
{
	Q_OBJECT

public:
	JavascriptInterface(const Environment& environment, QObject* parent = NULL);
	~JavascriptInterface();

	// Run javascript
	ScriptResult evaluate(const QString& scriptText, const QString& workspaceText = QString());

private:
	WorkbenchEngine* engine;
};

#endif // JAVASCRIPTINTERFACE_H
