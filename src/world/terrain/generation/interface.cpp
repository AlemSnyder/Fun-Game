#include "interface.hpp"

#include "logging.hpp"
#include "noise.hpp"
#include "util/angel_script/error_checks.hpp"
#include "worley_noise.hpp"

namespace terrain {
namespace generation {

namespace {

FractalNoise*
FractalNoise_factory(int x, double y, int z) {
    // The class constructor is initializing the reference counter to 1
    return new FractalNoise(x, y, z);
}

WorleyNoise*
WorleyNoise_factory(NoisePosition x, NoisePosition y) {
    // The class constructor is initializing the reference counter to 1
    return new WorleyNoise(x, y);
}

AlternativeWorleyNoise*
AlternativeWorleyNoise_factory(NoisePosition x, double y, NoisePosition z) {
    // The class constructor is initializing the reference counter to 1
    return new AlternativeWorleyNoise(x, y, z);
}

} // namespace

void
init_as_interface(AngelScript::asIScriptEngine* engine) {
    int r = engine->SetDefaultNamespace("TerrainGeneration");

    if (util::scripting::check_SetDefaultNamespace(r)) {
        return;
    }
    r = engine->RegisterObjectType("FractalNoise", 0, AngelScript::asOBJ_REF);

    if (util::scripting::check_RegisterObjectType(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", AngelScript::asBEHAVE_FACTORY,
        "FractalNoise@ f(int, double, int)",
        AngelScript::asFUNCTION(FractalNoise_factory), AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", AngelScript::asBEHAVE_ADDREF, "void f()",
        AngelScript::asMETHOD(FractalNoise, add_ref), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", AngelScript::asBEHAVE_RELEASE, "void f()",
        AngelScript::asMETHOD(FractalNoise, release_ref), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectMethod(
        "FractalNoise", "double sample(double, double)",
        AngelScript::asMETHOD(FractalNoise, get_noise), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectMethod(r)) {
        return;
    }
    r = engine->RegisterObjectType("WorleyNoise", 0, AngelScript::asOBJ_REF);

    if (util::scripting::check_RegisterObjectType(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", AngelScript::asBEHAVE_FACTORY, "WorleyNoise@ f(double, double)",
        AngelScript::asFUNCTION(WorleyNoise_factory), AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", AngelScript::asBEHAVE_ADDREF, "void f()",
        AngelScript::asMETHOD(WorleyNoise, add_ref), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", AngelScript::asBEHAVE_RELEASE, "void f()",
        AngelScript::asMETHOD(WorleyNoise, release_ref), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectMethod(
        "WorleyNoise", "double sample(double, double)",
        AngelScript::asMETHOD(WorleyNoise, get_noise), AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectMethod(r)) {
        return;
    }
    r = engine->RegisterObjectType("AlternativeWorleyNoise", 0, AngelScript::asOBJ_REF);
    if (util::scripting::check_RegisterObjectType(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", AngelScript::asBEHAVE_FACTORY,
        "AlternativeWorleyNoise@ f(double, double, double)",
        AngelScript::asFUNCTION(AlternativeWorleyNoise_factory),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", AngelScript::asBEHAVE_ADDREF, "void f()",
        AngelScript::asMETHOD(AlternativeWorleyNoise, add_ref),
        AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", AngelScript::asBEHAVE_RELEASE, "void f()",
        AngelScript::asMETHOD(AlternativeWorleyNoise, release_ref),
        AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectBehaviour(r)) {
        return;
    }
    r = engine->RegisterObjectMethod(
        "AlternativeWorleyNoise", "double sample(double, double)",
        AngelScript::asMETHOD(AlternativeWorleyNoise, get_noise),
        AngelScript::asCALL_THISCALL
    );
    if (util::scripting::check_RegisterObjectMethod(r)) {
        return;
    }
}

} // namespace generation
} // namespace terrain
