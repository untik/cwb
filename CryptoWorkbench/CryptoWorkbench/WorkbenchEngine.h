#ifndef WORKBENCHENGINE_H
#define WORKBENCHENGINE_H

#include <QString>
#include <QStringList>
#include "include/v8.h"
#include "ScriptResult.h"
#include "Environment.h"

////////////////////////////////////////////////////////////////////////////////////
///
/// Class wrapping v8 javascript engine
///
////////////////////////////////////////////////////////////////////////////////////
class WorkbenchEngine
{
	class ArrayBufferAllocator;

public:
	WorkbenchEngine(const Environment& engineEnvironment);
	~WorkbenchEngine();

	// Run javascript
	ScriptResult evaluate(const QString& scriptText, const QString& workspaceText = QString());

	// Resolve absolute path to provided file
	// Returns empty string on error
	QString resolveScriptFilePath(const QString& fileName);

	// Append exception details to list of encountered exceptions
	void appendExceptionReport(v8::TryCatch* trycatch);

private:
	v8::Local<v8::Context> createGlobalContext(const QString& workspaceText);
	QString buildExceptionReport(v8::TryCatch* trycatch);

private:
	v8::Platform* platform;
	v8::Isolate* isolate;
	ArrayBufferAllocator* alocator;
	QStringList exceptions;
	QString coreLibraryCode;
	Environment environment;
};

#endif // WORKBENCHENGINE_H
