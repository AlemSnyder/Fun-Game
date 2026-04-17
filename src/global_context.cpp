#include "global_context.hpp"

#include "local_context.hpp"
#include "logging.hpp"
#include "scriptstdstring.h"
#include "util/angle_script/as_logging.hpp"
#include "util/files.hpp"
#include "world/terrain/generation/lua_interface.hpp"

// Implement a simple message callback function
void
MessageCallback(const asSMessageInfo* msg, void* param) {
    if (msg->type == asMSGTYPE_ERROR) {
        LOG_ERROR(
            logging::lua_script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
    } else if (msg->type == asMSGTYPE_WARNING) {
        LOG_WARNING(
            logging::lua_script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
    } else if (msg->type == asMSGTYPE_INFORMATION) {
        LOG_WARNING(
            logging::lua_script_logger, "[ {} :({}, {}) ] - {}", msg->section, msg->row,
            msg->col, msg->message
        );
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
    asPrepareMultithread();
    engine_ = asCreateScriptEngine();
    engine_->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    RegisterStdString(engine_);
    terrain::generation::init_as_interface(engine_);
    as_logging::init_as_interface(engine_);
}

GlobalContext::~GlobalContext() {
    engine_->ShutDownAndRelease();
}

std::optional<sol::object>
GlobalContext::get_from_lua(const std::string& command) {
    LOG_BACKTRACE(logging::lua_logger, "Attempting to index {}.", command);

    std::stringstream command_stream(command);

    std::string key;

    std::getline(command_stream, key, '\\');

    auto raw_result = lua_.get<sol::optional<sol::object>>(key);

    if (!raw_result) {
        LOG_BACKTRACE(logging::lua_logger, "{} not valid.", key);
        return {};
    }

    sol::table result;

    while (std::getline(command_stream, key, '\\')) {
        if (!raw_result->is<sol::table>()) {
            LOG_BACKTRACE(logging::lua_logger, "{} not index of table.", key);
            return {};
        }
        result = raw_result.value();

        if (!result.valid()) {
            LOG_BACKTRACE(logging::lua_logger, "Could not find {}.", key);
            return {};
        }

        if (result == sol::lua_nil) {
            LOG_BACKTRACE(
                logging::lua_logger, "Attempting to index {}. nil value at {}.",
                command, key
            );
            return {};
        }

        if (!result.is<sol::table>()) {
            LOG_BACKTRACE(
                logging::lua_logger, "Attempting to index {}. {} not index of table.",
                command, key
            );
            return {};
        }

        raw_result = result.get<sol::optional<sol::object>>(key);

        if (!raw_result) {
            LOG_BACKTRACE(logging::lua_logger, "{} not valid.", key);
            return {};
        }
    }

    // a sol object
    return raw_result.value();
}

void
GlobalContext::load_script_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        LOG_WARNING(logging::file_io_logger, "File {} does not exists.", path);
        return;
    }
    // i think you also want this on the result valid but it might not matter
    std::scoped_lock lock(global_lua_mutex_);
    auto result = lua_.safe_script_file(path.lexically_normal().string());

    if (!result.valid()) {
        sol::error err = result; // who designed this?
        std::string what = err.what();
        LOG_ERROR(logging::lua_logger, "{}", what);
    }
}

// TODO this needs to return a status
void
GlobalContext::load_file(std::string module, std::filesystem::path path) {
    asIScriptModule* mod =
        engine_->GetModule(module.c_str(), asGM_CREATE_IF_NOT_EXISTS);

    std::ostringstream script;
    auto file = files::open_file(path);
    if (!file) {
        return;
    }

    script << file.value().rdbuf();
    mod->AddScriptSection(path.filename().c_str(), script.str().c_str());

    int result = mod->Build();
    if (result > 0) {
        LOG_ERROR(logging::lua_logger, "{}", result);
        return;
    }
}

asIScriptFunction*
GlobalContext::get_function(std::string module, std::string function_signature) const {
    asIScriptFunction* function = engine_->GetModule(module.c_str())
                                      ->GetFunctionByDecl(function_signature.c_str());
    return function;
}

asITypeInfo*
GlobalContext::get_type(std::string module, std::string type_signature) const {
    // TODO check that the module exists. In this and the above function.
    asITypeInfo* type =
        engine_->GetModule(module.c_str())->GetTypeInfoByDecl(type_signature.c_str());
    return type;
}
