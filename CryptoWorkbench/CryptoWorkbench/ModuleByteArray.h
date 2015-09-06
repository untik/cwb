#ifndef MODULE_BYTEARRAY_H
#define MODULE_BYTEARRAY_H

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

private:
	ModuleByteArray() {}
};

#endif // MODULE_BYTEARRAY_H
