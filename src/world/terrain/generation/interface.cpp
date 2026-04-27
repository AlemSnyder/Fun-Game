#include "interface.hpp"

#include "noise.hpp"
#include "worley_noise.hpp"

namespace terrain {
namespace generation {

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

void
init_as_interface(asIScriptEngine* engine) {
    int r = engine->SetDefaultNamespace("TerrainGeneration");
    assert(r >= 0);

    // Registering the class method
    r = engine->RegisterObjectType("FractalNoise", 0, asOBJ_REF);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", asBEHAVE_FACTORY, "FractalNoise@ f(int, double, int)",
        asFUNCTION(FractalNoise_factory), asCALL_CDECL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", asBEHAVE_ADDREF, "void f()", asMETHOD(FractalNoise, add_ref),
        asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "FractalNoise", asBEHAVE_RELEASE, "void f()",
        asMETHOD(FractalNoise, release_ref), asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectMethod(
        "FractalNoise", "double sample(double, double)",
        asMETHOD(FractalNoise, get_noise), asCALL_THISCALL
    );
    assert(r >= 0);

    r = engine->RegisterObjectType("WorleyNoise", 0, asOBJ_REF);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", asBEHAVE_FACTORY, "WorleyNoise@ f(double, double)",
        asFUNCTION(WorleyNoise_factory), asCALL_CDECL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", asBEHAVE_ADDREF, "void f()", asMETHOD(WorleyNoise, add_ref),
        asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "WorleyNoise", asBEHAVE_RELEASE, "void f()", asMETHOD(WorleyNoise, release_ref),
        asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectMethod(
        "WorleyNoise", "double sample(double, double)",
        asMETHOD(WorleyNoise, get_noise), asCALL_THISCALL
    );
    assert(r >= 0);

    r = engine->RegisterObjectType("AlternativeWorleyNoise", 0, asOBJ_REF);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", asBEHAVE_FACTORY,
        "AlternativeWorleyNoise@ f(double, double, double)",
        asFUNCTION(AlternativeWorleyNoise_factory), asCALL_CDECL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", asBEHAVE_ADDREF, "void f()",
        asMETHOD(AlternativeWorleyNoise, add_ref), asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour(
        "AlternativeWorleyNoise", asBEHAVE_RELEASE, "void f()",
        asMETHOD(AlternativeWorleyNoise, release_ref), asCALL_THISCALL
    );
    assert(r >= 0);
    r = engine->RegisterObjectMethod(
        "AlternativeWorleyNoise", "double sample(double, double)",
        asMETHOD(AlternativeWorleyNoise, get_noise), asCALL_THISCALL
    );
    assert(r >= 0);
}

} // namespace generation
} // namespace terrain
