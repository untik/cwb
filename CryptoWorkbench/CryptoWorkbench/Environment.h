#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>

////////////////////////////////////////////////////////////////////////////////////
///
/// Struncture holding paths and filenames used by workbench engine.
///
////////////////////////////////////////////////////////////////////////////////////
struct Environment
{
	QString coreLibraryName;
	QString coreLibraryPath;

	QString currentScriptName;

	QString workspaceName;

	QString scriptLoadPath;
};

#endif // ENVIRONMENT_H
