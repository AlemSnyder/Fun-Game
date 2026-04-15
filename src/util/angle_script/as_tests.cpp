
#include "global_context.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"

#include <angelscript.h>

namespace as_test {

int
test() {
    asIScriptEngine* engine = asCreateScriptEngine();
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

    auto type = context.get_type("Base::biomes", "biome_map");

    auto factory_function = type->GetFactoryByDecl("biome_map @biome_map()");

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

    asIScriptFunction* method = type->GetMethodByDecl("int sample(double, double)");
    local_context.run_function();
    return 0;
}

} // namespace as_test
