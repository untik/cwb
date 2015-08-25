#ifndef JAVASCRIPTINTERFACE_H
#define JAVASCRIPTINTERFACE_H

#include <QString>
#include <QObject>

namespace v8 {
class TryCatch;
class Platform;
class Isolate;
}


////////////////////////////////////////////////////////////////////////////////////
///
/// Class wrapping interface for v8 javascript engine
///
////////////////////////////////////////////////////////////////////////////////////
class JavascriptInterface : public QObject
{
	Q_OBJECT
	class ArrayBufferAllocator;

public:
	JavascriptInterface(QObject* parent = NULL);
	~JavascriptInterface();

	// Execute v8 HelloWorld example
	void testV8();

	// Evaluate provided script and return the result as string
	QString evaluate(const QString& scriptText, const QString& inputText = QString(), const QString& outputText = QString());

signals:
	void error(const QString& errorString);

private:
	void initializeV8();
	void shutdownV8();
	void reportException(v8::TryCatch* trycatch);
	QString buildExceptionReport(v8::TryCatch* trycatch);

private:
	v8::Platform* platform;
	v8::Isolate* isolate;
	ArrayBufferAllocator* alocator;
};

#endif // JAVASCRIPTINTERFACE_H
