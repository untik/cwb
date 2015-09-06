#include "WorkbenchEngine.h"
#include "include/libplatform/libplatform.h"
#include <QFile>
#include "ModuleTools.h"
#include "ModuleByteArray.h"
#include "Utility.h"

using namespace v8;

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


WorkbenchEngine::WorkbenchEngine(const Environment& engineEnvironment)
	: environment(engineEnvironment)
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
	QFile file(environment.coreLibraryPath + environment.coreLibraryName);
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

	// Compile and run core library if loaded
	if (!coreLibraryCode.isEmpty()) {
		MaybeLocal<Value> coreLibResult = executeString(this, isolate, 
														Utility::toV8String(isolate, coreLibraryCode),
														Utility::toV8String(isolate, environment.coreLibraryName));
		if (coreLibResult.IsEmpty())
			return ScriptResult::error(exceptions.join("\n\n"));
	}

	// Compile and run the javascript
	MaybeLocal<Value> result = executeString(this, isolate, 
											 Utility::toV8String(isolate, scriptText),
											 Utility::toV8String(isolate, environment.currentScriptName));
	if (result.IsEmpty())
		return ScriptResult::error(exceptions.join("\n\n"));

	// Get output variable.
	MaybeLocal<Value> outputValue = context->Global()->Get(Utility::toV8String(isolate, environment.workspaceName));
	if (outputValue.IsEmpty())
		return ScriptResult::error("Workspace error");

	QString resultString = Utility::toString(result.ToLocalChecked());
	QString outputString = Utility::toString(outputValue.ToLocalChecked());

	if (outputString.isEmpty())
		return ScriptResult::success(resultString);
	return ScriptResult::success(outputString);
}

QString WorkbenchEngine::resolveScriptFilePath(const QString& fileName)
{
	// TODO - Add proper sanitization and escaping

	QString resolvedPath = environment.scriptLoadPath + fileName;
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
	EscapableHandleScope handle_scope(isolate);

	// Create a template for the global object.
	Local<ObjectTemplate> globalObject = ObjectTemplate::New(isolate);

	// Register variable for workspace data
	globalObject->Set(Utility::toV8String(isolate, environment.workspaceName), Utility::toV8String(isolate, workspaceText));

	// Register global functions
	globalObject->Set(String::NewFromUtf8(isolate, "load"), FunctionTemplate::New(isolate, loadCallback, External::New(isolate, this)));

	// Register file manipulation functions
	Local<ObjectTemplate> fileObject = ObjectTemplate::New(isolate);
	fileObject->Set(String::NewFromUtf8(isolate, "read"), FunctionTemplate::New(isolate, readFileCallback, External::New(isolate, this)));
	globalObject->Set(String::NewFromUtf8(isolate, "File"), fileObject);

	// Register workbench functions
	ModuleTools::registerTemplates(isolate, globalObject);
	ModuleByteArray::registerTemplates(isolate, globalObject);

	return handle_scope.Escape(Context::New(isolate, NULL, globalObject));
}

QString WorkbenchEngine::buildExceptionReport(TryCatch* trycatch)
{
	HandleScope handle_scope(isolate);
	QString exceptionString = Utility::toString(trycatch->Exception());

	Local<Message> message = trycatch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error;
		return exceptionString;
	}

	// Print (filename):(line number): (message).
	QString fileName = Utility::toString(message->GetScriptOrigin().ResourceName());
	Local<Context> context(isolate->GetCurrentContext());
	int lineNumber = message->GetLineNumber(context).FromJust();

	QString report = QString("%1:%2: %3\n").arg(fileName).arg(lineNumber).arg(exceptionString);

	// Print line of source code.
	QString sourceLine = Utility::toString(message->GetSourceLine(context).ToLocalChecked());
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
	MaybeLocal<Value> stackTrace = trycatch->StackTrace(context);
	if (!stackTrace.IsEmpty()) {
		report.append(Utility::toString(stackTrace.ToLocalChecked()));
		report.append("\n");
	}

	return report;
}

void loadCallback(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 1)
		return;

	WorkbenchEngine* workbenchEngine = reinterpret_cast<WorkbenchEngine*>(Local<External>::Cast(args.Data())->Value());
	HandleScope handle_scope(args.GetIsolate());

	QString filePath = workbenchEngine->resolveScriptFilePath(Utility::toString(args[0]));
	if (filePath.isEmpty()) {
		Utility::throwException(args.GetIsolate(), "Invalid file parameter");
		return;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly)) {
		Utility::throwException(args.GetIsolate(), QString("Could not open file: %1").arg(filePath));
		return;
	}

	QByteArray fileContent = file.readAll();
	Local<String> source;
	bool ok = String::NewFromUtf8(args.GetIsolate(), fileContent.data(), NewStringType::kNormal, fileContent.length()).ToLocal(&source);
	if (!ok) {
		Utility::throwException(args.GetIsolate(), QString("File too large: %1").arg(filePath));
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

	QString filePath = workbenchEngine->resolveScriptFilePath(Utility::toString(args[0]));
	if (filePath.isEmpty()) {
		Utility::throwException(args.GetIsolate(), "Invalid file parameter");
		return;
	}

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly)) {
		Utility::throwException(args.GetIsolate(), QString("Could not open file: %1").arg(filePath));
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
