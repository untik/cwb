#include "JavascriptInterface.h"
#include "include/libplatform/libplatform.h"
#include "include/v8.h"
#include <QDebug>

using namespace v8;

class JavascriptInterface::ArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
	virtual void* Allocate(size_t length)
	{
		void* data = AllocateUninitialized(length);
		return data == NULL ? data : memset(data, 0, length);
	}
	virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
	virtual void Free(void* data, size_t) { free(data); }
};

QString stringValue(Local<Value> obj)
{
	String::Utf8Value objStr(obj);
	return QString::fromUtf8(*objStr, objStr.length());
}


JavascriptInterface::JavascriptInterface(QObject* parent)
	: QObject(parent)
{
	initializeV8();
}

JavascriptInterface::~JavascriptInterface()
{
	shutdownV8();
}

void JavascriptInterface::initializeV8()
{
	// Initialize V8.
	V8::InitializeICU();
	platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

#ifdef QT_NO_DEBUG
	V8::InitializeExternalStartupData("..\\bin\\natives_blob.bin", "..\\bin\\snapshot_blob.bin");
#else
	V8::InitializeExternalStartupData("..\\bin_debug\\natives_blob.bin", "..\\bin_debug\\snapshot_blob.bin");
#endif

	alocator = new ArrayBufferAllocator();

	// Create a new Isolate and make it the current one.
	Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = alocator;
	isolate = Isolate::New(create_params);
}

void JavascriptInterface::shutdownV8()
{
	// Dispose the isolate and tear down V8.
	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	delete alocator;
}

void JavascriptInterface::testV8()
{
	Isolate::Scope isolate_scope(isolate);

	// Create a stack-allocated handle scope.
	HandleScope handle_scope(isolate);

	// Create a new context.
	Local<Context> context = Context::New(isolate);

	// Enter the context for compiling and running the hello world script.
	Context::Scope context_scope(context);

	// Create a string containing the JavaScript source code.
	Local<String> source =
		String::NewFromUtf8(isolate, "'Hello' + ', World!'",
		NewStringType::kNormal).ToLocalChecked();

	// Compile the source code.
	Local<Script> script = Script::Compile(context, source).ToLocalChecked();

	// Run the script to get the result.
	Local<Value> result = script->Run(context).ToLocalChecked();

	// Convert the result to an UTF8 string and print it.
	String::Utf8Value utf8(result);
	//printf("%s\n", *utf8);
	qDebug() << *utf8;
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	

	if (args.Length() < 1)
		return;
	HandleScope scope(args.GetIsolate());
	Local<Value> arg = args[0];
	String::Utf8Value value(arg);
	qDebug() << *value;
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	Local<Object> self = args.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();

	QString* str = reinterpret_cast<QString*>(ptr);
	qDebug() << *str;


	args.GetReturnValue().Set(
		v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion(),
		v8::NewStringType::kNormal).ToLocalChecked());
}

void GetText(Local<String> property, const PropertyCallbackInfo<Value>& info)
{
	Local<Object> self = info.Holder();
	Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	QString value = *static_cast<QString*>(ptr);
	info.GetReturnValue().Set(value.length());
}


QString JavascriptInterface::evaluate(const QString& scriptText, const QString& inputText)
{
	Isolate::Scope isolate_scope(isolate);

	// Create a stack-allocated handle scope.
	HandleScope handle_scope(isolate);

	// Create a template for the global object and set the
	// built-in global functions.
	Local<ObjectTemplate> global = ObjectTemplate::New(isolate);

	Local<ObjectTemplate> textTemplate = ObjectTemplate::New(isolate);
	textTemplate->SetInternalFieldCount(1);
	textTemplate->SetAccessor(String::NewFromUtf8(isolate, "text"), GetText);

	QString* textPtr = new QString(inputText);

	Local<Object> textObj = textTemplate->NewInstance();
	textObj->SetInternalField(0, External::New(isolate, textPtr));

	global->Set(String::NewFromUtf8(isolate, "Text"), textTemplate);
	

	//Local<ObjectTemplate> testObject = ObjectTemplate::New(isolate);
	//testObject->SetInternalFieldCount(1);

	//testObject->Set(String::NewFromUtf8(isolate, "version"), FunctionTemplate::New(isolate, Version));
	//testObject->Set(String::NewFromUtf8(isolate, "log"), FunctionTemplate::New(isolate, LogCallback));

	//Local<Object> obj = testObject->NewInstance();
	//obj->SetInternalField(0, External::New(isolate, testPtr));

	

	



	//global->Set(String::NewFromUtf8(isolate, "version"), FunctionTemplate::New(isolate, Version));
	//global->Set(String::NewFromUtf8(isolate, "log"), FunctionTemplate::New(isolate, LogCallback));

	// Create a new context.
	Local<Context> context = Context::New(isolate, NULL, global);

	// Enter the context for compiling and running the script.
	Context::Scope context_scope(context);

	// Create a string containing the JavaScript source code.
	const uint16_t* textData = reinterpret_cast<const uint16_t*>(scriptText.constData());
	Local<String> source = String::NewFromTwoByte(isolate, textData, NewStringType::kNormal).ToLocalChecked();
	if (source.IsEmpty())
		return QString();

	TryCatch trycatch(isolate);

	// Compile the source code.
	MaybeLocal<Script> script = Script::Compile(context, source);
	if (script.IsEmpty()) {
		reportException(&trycatch);
		return QString();
	}

	// Run the script to get the result.
	MaybeLocal<Value> result = script.ToLocalChecked()->Run(context);
	if (result.IsEmpty()) {
		reportException(&trycatch);
		return QString();
	}

	// Convert the result to an UTF8 string and print it.
	QString resultString = stringValue(result.ToLocalChecked());
	return resultString;
}

void JavascriptInterface::reportException(v8::TryCatch* trycatch)
{
	QString report = buildExceptionReport(trycatch);
	emit error(report);
}

QString JavascriptInterface::buildExceptionReport(v8::TryCatch* trycatch)
{
	HandleScope handle_scope(isolate);
	QString exceptionString = stringValue(trycatch->Exception());

	Local<Message> message = trycatch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error;
		return exceptionString;
	}

	// Print (filename):(line number): (message).
	QString fileName = stringValue(message->GetScriptOrigin().ResourceName());
	Local<Context> context(isolate->GetCurrentContext());
	int lineNumber = message->GetLineNumber(context).FromJust();

	QString report = QString("%1:%2: %3\n").arg(fileName).arg(lineNumber).arg(exceptionString);

	// Print line of source code.
	QString sourceLine = stringValue(message->GetSourceLine(context).ToLocalChecked());
	report.append(sourceLine);
	report.append("\n");

	// Print wavy underline (GetUnderline is deprecated).
	int start = message->GetStartColumn(context).FromJust();
	for (int i = 0; i < start; i++) {
		report.append(" ");
	}
	int end = message->GetEndColumn(context).FromJust();
	for (int i = start; i < end; i++) {
		report.append("^");
	}
	report.append("\n");

	// Print stack trace
	QString stackTrace = stringValue(trycatch->StackTrace(context).ToLocalChecked());
	if (!stackTrace.isEmpty()) {
		report.append(stackTrace);
		report.append("\n");
	}

	return report;
}
