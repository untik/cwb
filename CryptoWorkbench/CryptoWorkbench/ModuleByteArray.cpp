#include "ModuleByteArray.h"
#include "Utility.h"
#include <QCryptographicHash>
#include <QDebug>

using namespace v8;

static Global<ObjectTemplate> ByteArrayTemplate;


QByteArray byteArrayFromString(const QString& source, int format)
{
	switch (format) {
		case 1:
			return source.toUtf8();
		case 2:
			return QByteArray::fromHex(source.toLatin1());
		case 3:
			return QByteArray::fromBase64(source.toLatin1());
	}
	return source.toLatin1();
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
		int format = 0;
		if (args.Length() >= 2 && args[1]->IsInt32())
			format = args[1]->Int32Value();
		if (format < 0 || format > 3) {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentValue);
			return;
		}
		data = byteArrayFromString(Utility::toString(args[0]).toLatin1(), format);
	}
	else {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	HandleScope handle_scope(args.GetIsolate());

	// Return the constructed object
	args.GetReturnValue().Set(ModuleByteArray::wrapByteArray(args.GetIsolate(), data));
}

void hex(const FunctionCallbackInfo<Value>& args)
{
	QByteArray data = ModuleByteArray::unwrapByteArray(args.GetIsolate(), args.Holder());

	int format = 0;
	if (args.Length() >= 1 && args[0]->IsInt32())
		format = args[0]->Int32Value();
	if (format < 0 || format > 2) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentValue);
		return;
	}

	QByteArray hexData = data.toHex();
	QString result;

	switch (format) {
		case 1:
			for (int i = 0; i < hexData.count(); i++) {
				if (i > 0 && i % 2 == 0)
					result.append(" ");
				result.append(hexData.at(i));
			}
			break;

		case 2:
			// TODO - Show in columns
			result = QString::fromLatin1(hexData);
			break;

		default:
			result = QString::fromLatin1(hexData);
			break;
	}

	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void base64(const FunctionCallbackInfo<Value>& args)
{
	QByteArray data = ModuleByteArray::unwrapByteArray(args.GetIsolate(), args.Holder());
	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), QString::fromLatin1(data.toBase64())));
}

void hash(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 1) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!args[0]->IsInt32()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	QByteArray input = ModuleByteArray::unwrapByteArray(args.GetIsolate(), args.Holder());
	int algorithm = args[0]->Int32Value();

	HandleScope handle_scope(args.GetIsolate());

	QByteArray hashValue;
	switch (algorithm) {
		case QCryptographicHash::Md4:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Md4);
			break;
		case QCryptographicHash::Md5:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Md5);
			break;
		case QCryptographicHash::Sha1:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha1);
			break;
		case QCryptographicHash::Sha224:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha224);
			break;
		case QCryptographicHash::Sha256:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha256);
			break;
		case QCryptographicHash::Sha384:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha384);
			break;
		case QCryptographicHash::Sha512:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha512);
			break;
		case QCryptographicHash::Sha3_224:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha3_224);
			break;
		case QCryptographicHash::Sha3_256:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha3_256);
			break;
		case QCryptographicHash::Sha3_384:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha3_384);
			break;
		case QCryptographicHash::Sha3_512:
			hashValue = QCryptographicHash::hash(input, QCryptographicHash::Sha3_512);
			break;

		default: {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentValue);
			return;
		}
	}

	args.GetReturnValue().Set(ModuleByteArray::wrapByteArray(args.GetIsolate(), hashValue));
}

void printable(const FunctionCallbackInfo<Value>& args)
{
	QByteArray data = ModuleByteArray::unwrapByteArray(args.GetIsolate(), args.Holder());

	QString placeholder = ".";
	if (args.Length() >= 1) {
		if (!args[0]->IsString()) {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
			return;
		}
		placeholder = Utility::toString(args[0]);
	}

	QString result;
	result.reserve(data.length());

	for (int i = 0; i < data.length(); i++) {
		char c = data.at(i);
		if (QChar::isPrint(c))
			result.append(c);
		else
			result.append(placeholder);
	}

	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void toString(const FunctionCallbackInfo<Value>& args)
{
	QByteArray data = ModuleByteArray::unwrapByteArray(args.GetIsolate(), args.Holder());
	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), QString::fromUtf8(data)));
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
	constructorInstanceTemplate->Set(String::NewFromUtf8(isolate, "base64"), FunctionTemplate::New(isolate, base64));
	constructorInstanceTemplate->Set(String::NewFromUtf8(isolate, "hash"), FunctionTemplate::New(isolate, hash));
	constructorInstanceTemplate->Set(String::NewFromUtf8(isolate, "printable"), FunctionTemplate::New(isolate, printable));
	constructorInstanceTemplate->Set(String::NewFromUtf8(isolate, "toString"), FunctionTemplate::New(isolate, toString));

	// Store template
	ByteArrayTemplate.Reset(isolate, constructorInstanceTemplate);

	// Set the function in the global scope -- that is, set "ByteArray" to the constructor
	globalObject->Set(String::NewFromUtf8(isolate, "ByteArray"), constructorTemplate);
}

Local<Object> ModuleByteArray::wrapByteArray(Isolate* isolate, const QByteArray& data)
{
	EscapableHandleScope handle_scope(isolate);

	// Fetch the template for creating ByteArray wrappers.
	Local<ObjectTemplate> localTemplate = Local<ObjectTemplate>::New(isolate, ByteArrayTemplate);

	// Create an empty ByteArray wrapper.
	Local<Object> wrapper = localTemplate->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();

	// Store data in ArrayBuffer
	wrapper->Set(String::NewFromUtf8(isolate, "buffer"), Utility::toV8ArrayBuffer(isolate, data));

	return handle_scope.Escape(wrapper);
}

QByteArray ModuleByteArray::unwrapByteArray(v8::Isolate* isolate, Local<Object> obj)
{
	Local<Value> buffer = obj->Get(String::NewFromUtf8(isolate, "buffer"));
	if (!buffer->IsArrayBuffer()) {
		Utility::throwException(isolate, "ByteArray.buffer must be ArrayBuffer");
		return QByteArray();
	}

	v8::ArrayBuffer::Contents c = Local<ArrayBuffer>::Cast(buffer)->GetContents();
	return QByteArray(reinterpret_cast<const char*>(c.Data()), c.ByteLength());
}