#include "error_checks.hpp"

#include "angelscript.h"
#include "logging.hpp"

namespace util {

namespace scripting {

AngelScript::asERetCodes
check_SetDefaultNamespace(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The namespace is invalid.");
            break;
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

AngelScript::asERetCodes
check_RegisterGlobalFunction(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The namespace is null.");
            break;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The namespace is invalid.");
            break;
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

AngelScript::asERetCodes
check_RegisterObjectType(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The flags are invalid.");
            break;
        case AngelScript::asERetCodes::asINVALID_NAME:
            LOG_ERROR(logging::as_logger, "The name is invalid.");
            break;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger, "Another type of the same name already exists."
            );
            break;
        case AngelScript::asERetCodes::asNAME_TAKEN:
            LOG_ERROR(
                logging::as_logger, "The name conflicts with other symbol names."
            );
            break;
        case AngelScript::asERetCodes::asLOWER_ARRAY_DIMENSION_NOT_REGISTERED:
            LOG_ERROR(
                logging::as_logger, "When registering an array type the array element "
                                    "must be a primitive or a registered type."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(logging::as_logger, "The array type was not properly formed.");
            break;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger, "The array type is not supported, or already in "
                                    "use preventing it from being overloaded."
            );
            break;
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

AngelScript::asERetCodes
check_RegisterObjectBehaviour(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asWRONG_CONFIG_GROUP:
            LOG_ERROR(
                logging::as_logger,
                "The object type was registered in a different configuration group."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(
                logging::as_logger,
                "obj is not set, or a global behaviour is given in behaviour, or the "
                "objForThiscall pointer wasn't set according to calling convention."
            );
            break;
        case AngelScript::asERetCodes::asWRONG_CALLING_CONV:
            LOG_ERROR(
                logging::as_logger,
                "The function's calling convention isn't compatible with callConv."
            );
            break;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger,
                "The calling convention or the behaviour signature is not supported."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(
                logging::as_logger, "The obj parameter is not a valid object name."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The declaration is invalid.");
            break;
        case AngelScript::asERetCodes::asILLEGAL_BEHAVIOUR_FOR_TYPE:
            LOG_ERROR(
                logging::as_logger, "The behaviour is not allowed for this type."
            );
            break;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger,
                "The behaviour is already registered with the same signature."
            );
            break;
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

AngelScript::asERetCodes
check_RegisterObjectMethod(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asWRONG_CONFIG_GROUP:
            LOG_ERROR(
                logging::as_logger,
                "The object type was registered in a different configuration group."
            );
            break;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(logging::as_logger, "The calling convention is not supported.");
            break;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(
                logging::as_logger, "The obj parameter is not a valid object name."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The declaration is invalid.");
            break;
        case AngelScript::asERetCodes::asNAME_TAKEN:
            LOG_ERROR(logging::as_logger, "The name conflicts with other members.");
            break;
        case AngelScript::asERetCodes::asWRONG_CALLING_CONV:
            LOG_ERROR(
                logging::as_logger,
                "The function's calling convention isn't compatible with callConv."
            );
            break;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger,
                "The method has already been registered with the same parameter list."
            );
            break;
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(
                logging::as_logger,
                "The auxiliary pointer wasn't set according to calling convention."
            );
            break;
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

AngelScript::asERetCodes
check_ScriptModule_Build(int r) {
    switch (r) {
        case AngelScript::asINVALID_CONFIGURATION:
            LOG_ERROR(logging::as_logger, "The engine configuration is invalid.");
        case AngelScript::asERROR:
            LOG_ERROR(logging::as_logger, "The script failed to build.");
        case AngelScript::asBUILD_IN_PROGRESS:
            LOG_ERROR(logging::as_logger, "Another thread is currently building.");
        case AngelScript::asINIT_GLOBAL_VARS_FAILED:
            LOG_ERROR(
                logging::as_logger, "It was not possible to initialize at least one of "
                                    "the global variables."
            );
        case AngelScript::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger, "Compiler support is disabled in the engine."
            );
        case AngelScript::asMODULE_IS_IN_USE:
            LOG_ERROR(
                logging::as_logger,
                "The code in the module is still being used and and cannot be removed."
            );
        case AngelScript::asERetCodes::asSUCCESS:
            break;
        default:
            LOG_WARNING(
                logging::as_logger,
                "Error {} does not match function call. Are you using the correct "
                "error handler?",
                r
            );
            break;
    }
    return static_cast<AngelScript::asERetCodes>(r);
}

} // namespace scripting

} // namespace util
