#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QByteArray>
#include "include/v8.h"

////////////////////////////////////////////////////////////////////////////////////
///
/// Collection of utility methods for v8 engine.
///
////////////////////////////////////////////////////////////////////////////////////
class Utility
{
public:
	enum ExceptionType
	{
		ExceptionInvalidArgumentCount,
		ExceptionInvalidArgumentType,
		ExceptionInvalidArgumentValue,
		ExceptionMustCallAsConstructor,
	};

	static void throwException(v8::Isolate* isolate, ExceptionType exception)
	{
		switch (exception) {
			case ExceptionInvalidArgumentCount:
				throwException(isolate, "Invalid argument count");
				break;
			case ExceptionInvalidArgumentType:
				throwException(isolate, "Invalid argument type");
				break;
			case ExceptionInvalidArgumentValue:
				throwException(isolate, "Invalid argument value");
				break;
			case ExceptionMustCallAsConstructor:
				throwException(isolate, "Function must be called as constructor");
				break;
		}
	}

	static void throwException(v8::Isolate* isolate, const QString& message)
	{
		isolate->ThrowException(
			v8::String::NewFromTwoByte(isolate, message.utf16(), v8::NewStringType::kNormal).ToLocalChecked());
	}

	static QString toString(const v8::Local<v8::Value>& obj, const QString& defaultValue = QString())
	{
		v8::String::Utf8Value value(obj);
		return (*value == NULL) ? defaultValue : QString::fromUtf8(*value, value.length());
	}

	static v8::Local<v8::String> toV8String(v8::Isolate* isolate, const QString& string)
	{
		return v8::String::NewFromTwoByte(isolate, string.utf16(), v8::NewStringType::kNormal).ToLocalChecked();
	}

	static QByteArray toByteArray(const v8::Local<v8::Value>& obj, const QByteArray& defaultValue = QByteArray())
	{
		if (obj->IsArrayBuffer()) {
			v8::ArrayBuffer::Contents c = v8::Local<v8::ArrayBuffer>::Cast(obj)->GetContents();
			return QByteArray(reinterpret_cast<const char*>(c.Data()), c.ByteLength());
		}

		v8::String::Utf8Value value(obj);
		return (*value == NULL) ? defaultValue : QByteArray(*value, value.length());
	}

	static v8::Local<v8::ArrayBuffer> toV8ArrayBuffer(v8::Isolate* isolate, const QByteArray& data)
	{
		v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, data.size());
		memcpy(buffer->GetContents().Data(), data.constData(), data.size());
		return buffer;
	}
};

#endif // UTILITY_H
