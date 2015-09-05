#include "WorkbenchEngine.h"
#include "include/libplatform/libplatform.h"
#include <QFile>

using namespace v8;

void throwException(Isolate* isolate, const QString& message);
void loadCallback(const FunctionCallbackInfo<Value>& args);
void readFileCallback(const FunctionCallbackInfo<Value>& args);
MaybeLocal<Value> executeString(WorkbenchEngine* workbenchEngine, Isolate* isolate, Local<String> source, Local<Value> name);


class WorkbenchEngine::ArrayBufferAllocator : public ArrayBuffer::Allocator
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


namespace Convert
{
	QString toString(const Local<Value>& obj, const QString& defaultValue = QString())
	{
		String::Utf8Value value(obj);
		return (*value == NULL) ? defaultValue : QString::fromUtf8(*value, value.length());
	}
};


WorkbenchEngine::WorkbenchEngine(const QString& coreLibraryPath)
{
	// Initialize V8.
	V8::InitializeICU();
	platform = v8::platform::CreateDefaultPlatform();
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

	// Load core library
	QFile file(coreLibraryPath);
	if (file.open(QFile::ReadOnly))
		coreLibraryCode = QString::fromUtf8(file.readAll());
}

WorkbenchEngine::~WorkbenchEngine()
{
	// Dispose the isolate and tear down V8.
	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	delete alocator;
}

ScriptResult WorkbenchEngine::evaluate(const QString& scriptText, const QString& workspaceText)
{
	exceptions.clear();

	Isolate::Scope isolate_scope(isolate);
	HandleScope handle_scope(isolate);

	Local<Context> context = createGlobalContext(workspaceText);
	if (context.IsEmpty())
		return ScriptResult::error("Error creating context");

	Context::Scope context_scope(context);

	// Create a string containing the JavaScript source code.
	Local<String> source = String::NewFromTwoByte(isolate, scriptText.utf16(), NewStringType::kNormal).ToLocalChecked();
	Local<String> origin = String::NewFromUtf8(isolate, "current.js", NewStringType::kNormal).ToLocalChecked();

	// Compile and run core library if loaded
	if (!coreLibraryCode.isEmpty()) {
		Local<String> coreLibSource = String::NewFromTwoByte(isolate, coreLibraryCode.utf16(), NewStringType::kNormal).ToLocalChecked();
		Local<String> coreLibOrigin = String::NewFromUtf8(isolate, "corelib.js", NewStringType::kNormal).ToLocalChecked();
		MaybeLocal<Value> coreLibResult = executeString(this, isolate, coreLibSource, coreLibOrigin);
		if (coreLibResult.IsEmpty())
			return ScriptResult::error(exceptions.join("\n\n"));
	}

	// Compile and run the javascript
	MaybeLocal<Value> result = executeString(this, isolate, source, origin);
	if (result.IsEmpty())
		return ScriptResult::error(exceptions.join("\n\n"));

	// Get output variable.
	MaybeLocal<Value> outputValue = context->Global()->Get(String::NewFromUtf8(isolate, "workspace"));
	if (outputValue.IsEmpty())
		return ScriptResult::error("Workspace error");

	QString resultString = Convert::toString(result.ToLocalChecked());
	QString outputString = Convert::toString(outputValue.ToLocalChecked());

	if (outputString.isEmpty())
		return ScriptResult::success(resultString);
	return ScriptResult::success(outputString);
}

QString WorkbenchEngine::resolveFilePath(const QString& fileName)
{
	// TODO - Add proper sanitization and escaping

	QString resolvedPath = "../scripts/" + fileName;
	if (!QFile::exists(resolvedPath))
		return QString();
	return resolvedPath;
}

void WorkbenchEngine::appendExceptionReport(TryCatch* trycatch)
{
	exceptions.append(buildExceptionReport(trycatch));
}

Local<Context> WorkbenchEngine::createGlobalContext(const QString& workspaceText)
{
	// Create a template for the global object.
	Local<ObjectTemplate> global = ObjectTemplate::New(isolate);

	// Register variable for workspace data
	global->Set(String::NewFromUtf8(isolate, "workspace"), String::NewFromUtf8(isolate, workspaceText.toUtf8()));

	// Register global functions
	global->Set(String::NewFromUtf8(isolate, "load"), FunctionTemplate::New(isolate, loadCallback, External::New(isolate, this)));

	// Register file manipulation functions
	Local<ObjectTemplate> fileObject = ObjectTemplate::New(isolate);
	fileObject->Set(String::NewFromUtf8(isolate, "read"), FunctionTemplate::New(isolate, readFileCallback, External::New(isolate, this)));
	global->Set(String::NewFromUtf8(isolate, "File"), fileObject);

	return Context::New(isolate, NULL, global);
}

QString WorkbenchEngine::buildExceptionReport(TryCatch* trycatch)
{
	HandleScope handle_scope(isolate);
	QString exceptionString = Convert::toString(trycatch->Exception());

	Local<Message> message = trycatch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error;
		return exceptionString;
	}

	// Print (filename):(line number): (message).
	QString fileName = Convert::toString(message->GetScriptOrigin().ResourceName());
	Local<Context> context(isolate->GetCurrentContext());
	int lineNumber = message->GetLineNumber(context).FromJust();

	QString report = QString("%1:%2: %3\n").arg(fileName).arg(lineNumber).arg(exceptionString);

	// Print line of source code.
	QString sourceLine = Convert::toString(message->GetSourceLine(context).ToLocalChecked());
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
	QString stackTrace = Convert::toString(trycatch->StackTrace(context).ToLocalChecked());
	if (!stackTrace.isEmpty()) {
		report.append(stackTrace);
		report.append("\n");
	}

	return report;
}

void throwException(Isolate* isolate, const QString& message)
{
	isolate->ThrowException(
		String::NewFromTwoByte(isolate, message.utf16(), NewStringType::kNormal).ToLocalChecked());
}

void loadCallback(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 1)
		return;

	WorkbenchEngine* workbenchEngine = reinterpret_cast<WorkbenchEngine*>(Local<External>::Cast(args.Data())->Value());
	HandleScope handle_scope(args.GetIsolate());

	QString filePath = workbenchEngine->resolveFilePath(Convert::toString(args[0]));
	if (filePath.isEmpty()) {
		throwException(args.GetIsolate(), "Invalid file parameter");
		return;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly)) {
		throwException(args.GetIsolate(), QString("Could not open file: %1").arg(filePath));
		return;
	}

	QByteArray fileContent = file.readAll();
	Local<String> source;
	bool ok = String::NewFromUtf8(args.GetIsolate(), fileContent.data(), NewStringType::kNormal, fileContent.length()).ToLocal(&source);
	if (!ok) {
		throwException(args.GetIsolate(), QString("File too large: %1").arg(filePath));
		return;
	}

	MaybeLocal<Value> result = executeString(workbenchEngine, args.GetIsolate(), source, args[0]);
	if (result.IsEmpty())
		return;

	args.GetReturnValue().Set(result.ToLocalChecked());
}

void readFileCallback(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 1)
		return;

	WorkbenchEngine* workbenchEngine = reinterpret_cast<WorkbenchEngine*>(Local<External>::Cast(args.Data())->Value());
	HandleScope handle_scope(args.GetIsolate());

	QString filePath = workbenchEngine->resolveFilePath(Convert::toString(args[0]));
	if (filePath.isEmpty()) {
		throwException(args.GetIsolate(), "Invalid file parameter");
		return;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly)) {
		throwException(args.GetIsolate(), QString("Could not open file: %1").arg(filePath));
		return;
	}

	QByteArray fileContent = file.readAll();
	Local<ArrayBuffer> buffer = ArrayBuffer::New(args.GetIsolate(), fileContent.size());
	memcpy(buffer->GetContents().Data(), fileContent.constData(), fileContent.size());

	args.GetReturnValue().Set(buffer);
}

MaybeLocal<Value> executeString(WorkbenchEngine* workbenchEngine, Isolate* isolate, Local<String> source, Local<Value> name)
{
	EscapableHandleScope handle_scope(isolate);
	TryCatch tryCatch(isolate);
	ScriptOrigin origin(name);
	Local<Context> context(isolate->GetCurrentContext());

	Local<Script> script;
	if (!Script::Compile(context, source, &origin).ToLocal(&script)) {
		workbenchEngine->appendExceptionReport(&tryCatch);
		return MaybeLocal<Value>();
	}

	Local<Value> result;
	if (!script->Run(context).ToLocal(&result)) {
		workbenchEngine->appendExceptionReport(&tryCatch);
		return MaybeLocal<Value>();
	}

	return handle_scope.Escape(result);
}
