#include "global_context.hpp"

#include "local_context.hpp"
#include "logging.hpp"
#include "scriptstdstring.h"
#include "util/angel_script/as_logging.hpp"
#include "util/files.hpp"
#include "world/terrain/generation/interface.hpp"

// Implement a simple message callback function
void
MessageCallback(const AngelScript::asSMessageInfo* msg, void* param) {
    if (msg->type == AngelScript::asMSGTYPE_ERROR) {
        LOG_ERROR(
            logging::script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
    } else if (msg->type == AngelScript::asMSGTYPE_WARNING) {
        LOG_WARNING(
            logging::script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
    } else if (msg->type == AngelScript::asMSGTYPE_INFORMATION) {
        LOG_INFO(
            logging::script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
    } else {
        LOG_ERROR(logging::script_logger, "Unknown message type.");
    }
}

void
GlobalContext::run_opengl_queue() {
    std::lock_guard<std::mutex> lock(opengl_queue_mutex);
    if (!opengl_functions.empty()) [[unlikely]] {
#if 0
        LOG_DEBUG(
            logging::opengl_logger, "Running {} functions from opengl function pool.",
            opengl_functions.size()
        );
#endif

        while (!opengl_functions.empty()) {
            const auto& function = opengl_functions.front();
            function(); // run function
            opengl_functions.pop();
        }
        LOG_DEBUG_LIMIT(
            std::chrono::seconds{60}, logging::opengl_logger, "Completed opengl queue."
        );
    }
}

GlobalContext::GlobalContext() :
    thread_pool_([] { quill::detail::set_thread_name("BS Thread"); }) {
    AngelScript::asPrepareMultithread();
    engine_ = AngelScript::asCreateScriptEngine();
    engine_->SetMessageCallback(
        AngelScript::asFUNCTION(MessageCallback), 0, AngelScript::asCALL_CDECL
    );
    RegisterStdString(engine_);
    terrain::generation::init_as_interface(engine_);
    util::scripting::init_as_interface(engine_);
}

GlobalContext::~GlobalContext() {
    engine_->ShutDownAndRelease();
}

AngelScript::asERetCodes
GlobalContext::load_file(const std::string& mod_name, std::filesystem::path path) {
    AngelScript::asIScriptModule* mod;
    if (mod = engine_->GetModule(mod_name.c_str(), AngelScript::asGM_ONLY_IF_EXISTS)) {
        LOG_BACKTRACE(
            logging::as_logger, "Loading file from \"{}\" into module \"{}\".",
            path.lexically_normal().string(), mod_name
        );
    } else if (mod = engine_->GetModule(
                   mod_name.c_str(), AngelScript::asGM_CREATE_IF_NOT_EXISTS
               )) {
        LOG_BACKTRACE(
            logging::as_logger, "Creating module \"{}\" from file \"{}\".", mod_name,
            path.lexically_normal().string()
        );
    } else {
        LOG_ERROR(
            logging::as_logger, "Could not find or create module \"{}\".", mod_name
        );
        return AngelScript::asERetCodes::asERROR;
    }

    std::ostringstream script;
    auto file = files::open_file(path);
    if (!file) {
        LOG_ERROR(logging::as_logger, "Could not open file.");
        return AngelScript::asERetCodes::asERROR;
    }

    script << file.value().rdbuf();
    mod->AddScriptSection(path.filename().c_str(), script.str().c_str());

    AngelScript::asERetCodes result =
        static_cast<AngelScript::asERetCodes>(mod->Build());

    switch (result) {
        case AngelScript::asINVALID_CONFIGURATION:
            LOG_ERROR(logging::as_logger, "The engine configuration is invalid.");
            return result;
        case AngelScript::asERROR:
            LOG_ERROR(logging::as_logger, "The script failed to build.");
            return result;
        case AngelScript::asBUILD_IN_PROGRESS:
            LOG_ERROR(logging::as_logger, "Another thread is currently building.");
            return result;
        case AngelScript::asINIT_GLOBAL_VARS_FAILED:
            LOG_ERROR(
                logging::as_logger, "It was not possible to initialize at least one of "
                                    "the global variables."
            );
            return result;
        case AngelScript::asNOT_SUPPORTED:
            LOG_ERROR(
                logging::as_logger, "Compiler support is disabled in the engine."
            );
            return result;
        case AngelScript::asMODULE_IS_IN_USE:
            LOG_ERROR(
                logging::as_logger,
                "The code in the module is still being used and and cannot be removed."
            );
            return result;

        default:
            break;
    }

    return AngelScript::asERetCodes::asSUCCESS;
}

AngelScript::asIScriptFunction*
GlobalContext::get_function(
    const std::string& module, std::string function_signature
) const {
    // check that the module exists.
    AngelScript::asIScriptModule* mod;
    if ((mod = engine_->GetModule(module.c_str())) == nullptr) {
        return nullptr;
    }
    AngelScript::asIScriptFunction* function =
        mod->GetFunctionByDecl(function_signature.c_str());
    return function;
}

AngelScript::asITypeInfo*
GlobalContext::get_type(const std::string& module, std::string type_signature) const {
    // check that the module exists.
    AngelScript::asIScriptModule* mod;
    if ((mod = engine_->GetModule(module.c_str())) == nullptr) {
        return nullptr;
    }
    AngelScript::asITypeInfo* type = mod->GetTypeInfoByDecl(type_signature.c_str());
    return type;
}
