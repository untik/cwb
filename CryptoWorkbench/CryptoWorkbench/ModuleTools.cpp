#include "ModuleTools.h"
#include "Utility.h"
#include <QCryptographicHash>

using namespace v8;

void hash(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 2) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!(args[0]->IsString() || args[0]->IsArrayBuffer()) || !args[1]->IsInt32()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	QByteArray input = Utility::toByteArray(args[0]);
	int algorithm = args[1]->Int32Value();

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

	args.GetReturnValue().Set(Utility::toV8ArrayBuffer(args.GetIsolate(), hashValue));
}

void ModuleTools::registerTemplates(v8::Isolate* isolate, Local<ObjectTemplate> globalObject)
{
	HandleScope handle_scope(isolate);
	Local<ObjectTemplate> object = ObjectTemplate::New(isolate);

	object->Set(String::NewFromUtf8(isolate, "hash"), FunctionTemplate::New(isolate, hash));

	//fileObject->Set(String::NewFromUtf8(isolate, "read"), FunctionTemplate::New(isolate, readFileCallback, External::New(isolate, this)));

	globalObject->Set(String::NewFromUtf8(isolate, "Tools"), object);
}
