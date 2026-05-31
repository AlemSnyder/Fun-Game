#include "error_checks.hpp"

#include "angelscript.h"
#include "logging.hpp"

namespace util {

namespace scripting {

bool
check_SetDefaultNamespace(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The namespace is invalid.");
            return true;
        case AngelScript::asERetCodes::asSUCCESS:
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

bool
check_RegisterGlobalFunction(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The namespace is null.");
            return true;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The namespace is invalid.");
            return true;
        case AngelScript::asERetCodes::asSUCCESS:
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

bool
check_RegisterObjectType(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(logging::as_logger, "The flags are invalid.");
            return true;
        case AngelScript::asERetCodes::asINVALID_NAME:
            LOG_ERROR(logging::as_logger, "The name is invalid.");
            return true;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger, "Another type of the same name already exists."
            );
            return true;
        case AngelScript::asERetCodes::asNAME_TAKEN:
            LOG_ERROR(
                logging::as_logger, "The name conflicts with other symbol names."
            );
            return true;
        case AngelScript::asERetCodes::asLOWER_ARRAY_DIMENSION_NOT_REGISTERED:
            LOG_ERROR(
                logging::as_logger, "When registering an array type the array element "
                                    "must be a primitive or a registered type."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(logging::as_logger, "The array type was not properly formed.");
            return true;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger, "The array type is not supported, or already in "
                                    "use preventing it from being overloaded."
            );
            return true;
        case AngelScript::asERetCodes::asSUCCESS:
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

bool
check_RegisterObjectBehaviour(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asWRONG_CONFIG_GROUP:
            LOG_ERROR(
                logging::as_logger,
                "The object type was registered in a different configuration group."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(
                logging::as_logger,
                "obj is not set, or a global behaviour is given in behaviour, or the "
                "objForThiscall pointer wasn't set according to calling convention."
            );
            return true;
        case AngelScript::asERetCodes::asWRONG_CALLING_CONV:
            LOG_ERROR(
                logging::as_logger,
                "The function's calling convention isn't compatible with callConv."
            );
            return true;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger,
                "The calling convention or the behaviour signature is not supported."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(
                logging::as_logger, "The obj parameter is not a valid object name."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The declaration is invalid.");
            return true;
        case AngelScript::asERetCodes::asILLEGAL_BEHAVIOUR_FOR_TYPE:
            LOG_ERROR(
                logging::as_logger, "The behaviour is not allowed for this type."
            );
            return true;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger,
                "The behaviour is already registered with the same signature."
            );
            return true;
        case AngelScript::asERetCodes::asSUCCESS:
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

bool
check_RegisterObjectMethod(int r) {
    switch (r) {
        case AngelScript::asERetCodes::asWRONG_CONFIG_GROUP:
            LOG_ERROR(
                logging::as_logger,
                "The object type was registered in a different configuration group."
            );
            return true;
        case AngelScript::asERetCodes::asNOT_SUPPORTED:
            LOG_ERROR(logging::as_logger, "The calling convention is not supported.");
            return true;
        case AngelScript::asERetCodes::asINVALID_TYPE:
            LOG_ERROR(
                logging::as_logger, "The obj parameter is not a valid object name."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_DECLARATION:
            LOG_ERROR(logging::as_logger, "The declaration is invalid.");
            return true;
        case AngelScript::asERetCodes::asNAME_TAKEN:
            LOG_ERROR(logging::as_logger, "The name conflicts with other members.");
            return true;
        case AngelScript::asERetCodes::asWRONG_CALLING_CONV:
            LOG_ERROR(
                logging::as_logger,
                "The function's calling convention isn't compatible with callConv."
            );
            return true;
        case AngelScript::asERetCodes::asALREADY_REGISTERED:
            LOG_ERROR(
                logging::as_logger,
                "The method has already been registered with the same parameter list."
            );
            return true;
        case AngelScript::asERetCodes::asINVALID_ARG:
            LOG_ERROR(
                logging::as_logger,
                "The auxiliary pointer wasn't set according to calling convention."
            );
            return true;
        case AngelScript::asERetCodes::asSUCCESS:
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

bool
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
            return false;
        default:
            if (r < 0) {
                LOG_WARNING(
                    logging::as_logger,
                    "Error {} does not match function call. Are you using the correct "
                    "error handler?",
                    r
                );
                return true;
            }
            return false;
    }
}

} // namespace scripting

} // namespace util
