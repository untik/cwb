#include "ModuleByteArray.h"
#include "Utility.h"
#include <QDebug>

using namespace v8;

//static Global<ObjectTemplate> ByteArrayTemplate;

//void deleteByteArrayCallback(const WeakCallbackData<Object, QByteArray>& callbackInfo)

void deleteByteArrayCallback(const WeakCallbackData<Object, QByteArray>& data)
{
	qDebug() << "DELETE QByteArray";


	//Point *point = static_cast<Point *> (parameter);

	////remove it from the map
	//jsPoints.erase(point);

	////javascript no longer uses the object
	////if we are certain C++ isn't using it, we could "Let JavaScript destroy it":
	//delete point;

	////clear the reference to it
	//object.Dispose();
	//object.Clear();
}

QByteArray* constructByteArrayFromString(const FunctionCallbackInfo<Value>& args)
{
	// TODO - parse input formats

	QString source = Utility::toString(args[0]);
	return new QByteArray(source.toLatin1());
}

QByteArray* constructByteArrayFromArrayBuffer(const FunctionCallbackInfo<Value>& args)
{
	QByteArray source = Utility::toByteArray(args[0]);
	return new QByteArray(source);
}

void constructByteArray(const FunctionCallbackInfo<Value>& args)
{
	if (!args.IsConstructCall()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionMustCallAsConstructor);
		return;
	}
	if (args.Length() == 0) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}

	QByteArray data;

	if (args[0]->IsArrayBuffer()) {
		data = Utility::toByteArray(args[0]);
	}
	else if (args[0]->IsString()) {
		data = Utility::toString(args[0]).toLatin1();
	}
	else {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	HandleScope handle_scope(args.GetIsolate());

	Local<Object> wrapper = args.Holder();

	wrapper->Set(String::NewFromUtf8(args.GetIsolate(), "buffer"), Utility::toV8ArrayBuffer(args.GetIsolate(), data));

	// Return the constructed object
	args.GetReturnValue().Set(wrapper);



	//qDebug() << "NEW QByteArray";

	//

	//// Fetch the template for creating ByteArray wrappers.
	////Local<ObjectTemplate> localTemplate = Local<ObjectTemplate>::New(args.GetIsolate(), ByteArrayTemplate);

	//// Create an empty ByteArray wrapper.
	////Local<Object> wrapper = localTemplate->NewInstance(args.GetIsolate()->GetCurrentContext()).ToLocalChecked();
	//Local<Object> wrapper = args.Holder();

	//// Wrap the raw C++ pointer in an External so it can be referenced from within JavaScript.
	//Local<External> external = External::New(args.GetIsolate(), arrayPointer);

	//// Store the pointer in the JavaScript wrapper.
	//wrapper->SetInternalField(0, external);

	//// Create persistent handle to wrapper object
	//Persistent<Object> persistentWrapper(args.GetIsolate(), wrapper);
	//persistentWrapper.SetWeak(arrayPointer, deleteByteArrayCallback);
	//persistentWrapper.MarkIndependent();


}

QByteArray* unwrapByteArray(Local<Object> obj)
{
	Local<External> field = Local<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<QByteArray*>(ptr);
}

void hex(const FunctionCallbackInfo<Value>& args)
{
	Local<Value> buffer = args.Holder()->Get(String::NewFromUtf8(args.GetIsolate(), "buffer"));
	v8::ArrayBuffer::Contents c = Local<ArrayBuffer>::Cast(buffer)->GetContents();
	QByteArray data(reinterpret_cast<const char*>(c.Data()), c.ByteLength());

	//// Extract the C++ QByteArray object from the JavaScript wrapper.
	//QByteArray* data2 = unwrapByteArray(args.Holder());
	//if (data2 == NULL) {
	//	qDebug() << "ByteArray.hex NULL pointer";
	//	return;
	//}

	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), QString::fromLatin1(data.toHex())));
}

void ModuleByteArray::registerTemplates(v8::Isolate* isolate, Local<ObjectTemplate> globalObject)
{
	HandleScope handle_scope(isolate);
	Local<ObjectTemplate> object = ObjectTemplate::New(isolate);

	// Create function template for our constructor it will call the constructByteArray function
	Local<FunctionTemplate> constructorTemplate = FunctionTemplate::New(isolate, constructByteArray);
	constructorTemplate->SetClassName(String::NewFromUtf8(isolate, "ByteArray"));

	// Define function added to each instance
	Local<ObjectTemplate> constructorInstanceTemplate = constructorTemplate->InstanceTemplate();
	constructorInstanceTemplate->SetInternalFieldCount(1);
	constructorInstanceTemplate->Set(String::NewFromUtf8(isolate, "hex"), FunctionTemplate::New(isolate, hex));

	// Store template
	//ByteArrayTemplate.Reset(isolate, constructorInstanceTemplate);

	// Set the function in the global scope -- that is, set "ByteArray" to the constructor
	globalObject->Set(String::NewFromUtf8(isolate, "ByteArray"), constructorTemplate);
}
