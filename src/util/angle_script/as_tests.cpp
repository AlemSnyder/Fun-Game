
#include "global_context.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"
#include "as_logging.hpp"
#include "scriptstdstring.h" // hm

#include <angelscript.h>

namespace as_test {

int logging_test() {
    as_logging::as_log_backtrace("Backtrace");
    LOG_ERROR(logging::lua_script_logger, "");

    asIScriptEngine* engine = asCreateScriptEngine();
    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    RegisterStdString(engine);
    as_logging::init_as_interface(engine);

    if (!engine) {
        LOG_ERROR(logging::main_logger, "Could no start Angle Script engine.");
        return 1;
    }

    asIScriptModule* mod = engine->GetModule("test_module", asGM_CREATE_IF_NOT_EXISTS);
    std::ostringstream script;
    auto file = files::open_file(files::get_resources_path() / "as" / "test.as");
    if (!file) {
        engine->ShutDownAndRelease();
        return 1;
    }

    script << file.value().rdbuf();
    mod->AddScriptSection("test.as", script.str().c_str());

    int result = mod->Build();
    if (result > 0) {
        engine->ShutDownAndRelease();
        return 1;
    }

    asIScriptFunction* funct1 =
        engine->GetModule("test_module")->GetFunctionByDecl("int test3()");


    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(funct1);
    //    ctx->SetArgDWord();
    result = ctx->Execute();
    if (result != asEXECUTION_FINISHED) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }
    ctx->Release();
    engine->ShutDownAndRelease();
    return 0;
}

int
test() {
    asIScriptEngine* engine = asCreateScriptEngine();
    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

    if (!engine) {
        LOG_ERROR(logging::main_logger, "Could no start Angle Script engine.");
        return 1;
    }

    asIScriptModule* mod = engine->GetModule("test_module", asGM_CREATE_IF_NOT_EXISTS);
    std::ostringstream script;
    auto file = files::open_file(files::get_resources_path() / "as" / "test.as");
    if (!file) {
        engine->ShutDownAndRelease();
        return 1;
    }

    script << file.value().rdbuf();
    mod->AddScriptSection("test.as", script.str().c_str());

    int result = mod->Build();
    if (result > 0) {
        engine->ShutDownAndRelease();
        return 1;
    }

    asIScriptFunction* funct1 =
        engine->GetModule("test_module")->GetFunctionByDecl("void test1()");

    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(funct1);
    //    ctx->SetArgDWord();
    result = ctx->Execute();
    if (result != asEXECUTION_FINISHED) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }

    asIScriptFunction* funct2 =
        engine->GetModule("test_module")->GetFunctionByDecl("int test2()");

    ctx->Prepare(funct2);
    //    ctx->SetArgDWord();
    result = ctx->Execute();
    if (result != asEXECUTION_FINISHED) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }
    int returnvalue = ctx->GetReturnDWord();
    if (returnvalue == 1 || returnvalue == 0) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }

    engine->ShutDownAndRelease();
    return 0;
}

int
as_load_tests() {
    GlobalContext& context = GlobalContext::instance();

    context.load_file("main", files::get_resources_path() / "as" / "test.as");

    context.load_file("Base", files::get_data_path() / "Base" / "biome_map.as");

    auto function = context.get_function("Base", "void do_something()");

    LocalContext& local_context = LocalContext::instance();
    int result = local_context.run_function(function);

    if (result != asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript Test");
        return 1;
    }

    auto type = context.get_type("Base", "Base::biomes::biome_map");

    int factory_count = type->GetFactoryCount();
    LOG_DEBUG(logging::main_logger, "Found {} factory functions", factory_count);

    auto factory_function = type->GetFactoryByDecl("Base::biomes::biome_map@ biome_map()");
    auto factory_function_2 = type->GetFactoryByIndex(0);
    
    auto declaration = factory_function_2->GetDeclaration(true, true, true);
    
    auto factory_function_3 = type->GetFactoryByDecl(declaration);

    LOG_DEBUG(logging::main_logger, "{}, {}, {}", factory_function != nullptr, factory_function_2 != nullptr, factory_function_3 != nullptr);

    LOG_DEBUG(logging::main_logger, "{}", declaration);

    result = local_context.run_function(factory_function);
    if (result != asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript getting biome map");
        return 1;
    }

    asIScriptObject *biome_map = local_context.get_return_object();
    if (biome_map == nullptr) {
        LOG_ERROR(logging::main_logger, "Failed to get object");
        return 1;
    }
    biome_map->AddRef();

    asIScriptFunction* method = type->GetMethodByDecl("int sample(int, int)");
    result = local_context.run_method(biome_map, method, 5, 5);
    if (result != asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript run sample");
        return 1;
    }

    int return_value;
    result = local_context.get_return_value(return_value);
    if (result != 0) {
        LOG_ERROR(logging::main_logger, "Failed to get result from sample. Error: {}", result);
        return 1;
    }

    LOG_DEBUG(logging::main_logger, "Got result {}.", return_value);
    biome_map->Release();
    return 0;
}

} // namespace as_test
