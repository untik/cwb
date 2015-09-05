#ifndef WORKBENCHENGINE_H
#define WORKBENCHENGINE_H

#include <QString>
#include <QStringList>
#include "include/v8.h"
#include "ScriptResult.h"

////////////////////////////////////////////////////////////////////////////////////
///
/// Class wrapping v8 javascript engine
///
////////////////////////////////////////////////////////////////////////////////////
class WorkbenchEngine
{
	class ArrayBufferAllocator;

public:
	WorkbenchEngine(const QString& coreLibraryPath);
	~WorkbenchEngine();

	// Run javascript
	ScriptResult evaluate(const QString& scriptText, const QString& workspaceText = QString());

	// Resolve absolute path to provided file
	// Returns empty string on error
	QString resolveFilePath(const QString& fileName);

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
};

#endif // WORKBENCHENGINE_H
