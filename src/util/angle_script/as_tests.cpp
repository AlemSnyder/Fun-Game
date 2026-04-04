
#include "logging.hpp"
#include <angelscript.h>
#include "util/files.hpp"
namespace as_test {

int
test() {
	asIScriptEngine *engine = asCreateScriptEngine();
    if (!engine) {
        LOG_ERROR(logging::main_logger, "Could no start Angle Script engine.");
        return 1;
    }

    asIScriptModule* mod = engine->GetModule("test_module", asGM_CREATE_IF_NOT_EXISTS);
    std::ostringstream script;
    auto file = files::open_file(files::get_resources_path() / "as" / "test.as");
    if (! file) {
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

    asIScriptFunction* funct1 = engine->GetModule("test_module")->GetFunctionByDecl("void test1()");

    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(funct1);
//    ctx->SetArgDWord();
    result = ctx->Execute();
    if (result != asEXECUTION_FINISHED) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }

    asIScriptFunction* funct2 = engine->GetModule("test_module")->GetFunctionByDecl("int test2()");

    ctx->Prepare(funct2);
//    ctx->SetArgDWord();
    result = ctx->Execute();
    if (result != asEXECUTION_FINISHED) {
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }
    int returnvalue = ctx->GetReturnDWord();
    if (returnvalue == 1 || returnvalue == 0){
        ctx->Release();
        engine->ShutDownAndRelease();
        return 1;
    }

    engine->ShutDownAndRelease();
    return 0;
}

}

