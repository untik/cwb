#ifndef JAVASCRIPTINTERFACE_H
#define JAVASCRIPTINTERFACE_H

#include <QString>
#include <QObject>
#include "ScriptResult.h"

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
	JavascriptInterface(const QString& coreLibraryPath, QObject* parent = NULL);
	~JavascriptInterface();

	// Run javascript
	ScriptResult evaluate(const QString& scriptText, const QString& workspaceText = QString());

private:
	WorkbenchEngine* engine;
};

#endif // JAVASCRIPTINTERFACE_H
