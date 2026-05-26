

#pragma once

#include "angelscript.h"

namespace util {
namespace scripting {

AngelScript::asERetCodes check_SetDefaultNamespace(int);

AngelScript::asERetCodes check_RegisterGlobalFunction(int);

AngelScript::asERetCodes check_RegisterObjectType(int);

AngelScript::asERetCodes check_RegisterObjectBehaviour(int);

AngelScript::asERetCodes check_RegisterObjectMethod(int);

AngelScript::asERetCodes check_RegisterObjectType(int);

} // namespace scripting

} // namespace util