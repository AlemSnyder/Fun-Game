

#pragma once

#include "angelscript.h"

namespace util {
namespace scripting {

bool check_SetDefaultNamespace(int);

bool check_ScriptModule_Build(int);

bool check_RegisterGlobalFunction(int);

bool check_RegisterObjectBehaviour(int);

bool check_RegisterObjectMethod(int);

bool check_RegisterObjectType(int);

} // namespace scripting

} // namespace util