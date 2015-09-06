#ifndef MODULETOOLS_H
#define MODULETOOLS_H

#include "include/v8.h"

////////////////////////////////////////////////////////////////////////////////////
///
/// Definitions for functions contained in javascript Tools object.
///
////////////////////////////////////////////////////////////////////////////////////
class ModuleTools
{
public:
	static void registerTemplates(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalObject);

private:
	ModuleTools() {}
};

#endif // MODULETOOLS_H
