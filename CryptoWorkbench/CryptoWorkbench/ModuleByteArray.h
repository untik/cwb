#ifndef MODULE_BYTEARRAY_H
#define MODULE_BYTEARRAY_H

#include <QByteArray>
#include "include/v8.h"

////////////////////////////////////////////////////////////////////////////////////
///
/// Definitions for functions contained in javascript Tools object.
///
////////////////////////////////////////////////////////////////////////////////////
class ModuleByteArray
{
public:
	static void registerTemplates(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalObject);

	static v8::Local<v8::Object> wrapByteArray(v8::Isolate* isolate, const QByteArray& data);
	static QByteArray unwrapByteArray(v8::Isolate* isolate, v8::Local<v8::Object> obj);

private:
	ModuleByteArray() {}
};

#endif // MODULE_BYTEARRAY_H
