#include "lua_interface.hpp"

#include "noise.hpp"
#include "worley_noise.hpp"

#include <sol/sol.hpp>

namespace terrain {
namespace generation {
void
init_lua_interface(sol::state& lua) {
    lua.new_usertype<FractalNoise>(
        "FractalNoise", sol::meta_function::construct,
        sol::factories(
            // FractalNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int num_octaves, double persistence, int prime_index) {
                return std::make_shared<FractalNoise>(
                    num_octaves, persistence, prime_index
                );
            },
            // FractalNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int num_octaves, double persistence, int prime_index) {
                return std::make_shared<FractalNoise>(
                    num_octaves, persistence, prime_index
                );
            }
        ),
        // FractalNoise(...) syntax, only
        sol::call_constructor,
        sol::factories([](int num_octaves, double persistence, int prime_index) {
            return std::make_shared<FractalNoise>(
                num_octaves, persistence, prime_index
            );
        }),
        "sample", &FractalNoise::get_noise
    );

    lua.new_usertype<WorleyNoise>(
        "WorleyNoise", sol::meta_function::construct,
        sol::factories(
            // WorleyNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int tile_size, double radius) {
                return std::make_shared<WorleyNoise>(tile_size, radius);
            },
            // WorleyNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int tile_size, double radius) {
                return std::make_shared<WorleyNoise>(tile_size, radius);
            }
        ),
        // WorleyNoise(...) syntax, only
        sol::call_constructor, sol::factories([](int tile_size, double radius) {
            return std::make_shared<WorleyNoise>(tile_size, radius);
        }),
        "sample", &WorleyNoise::get_noise
    );

    lua.new_usertype<AlternativeWorleyNoise>(
        "AlternativeWorleyNoise", sol::meta_function::construct,
        sol::factories(
            // AlternativeWorleyNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int tile_size, double positive_chance, double radius) {
                return std::make_shared<AlternativeWorleyNoise>(
                    tile_size, positive_chance, radius
                );
            },
            // AlternativeWorleyNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int tile_size, double positive_chance, double radius) {
                return std::make_shared<AlternativeWorleyNoise>(
                    tile_size, positive_chance, radius
                );
            }
        ),
        // AlternativeWorleyNoise(...) syntax, only
        sol::call_constructor,
        sol::factories([](int tile_size, double positive_chance, double radius) {
            return std::make_shared<AlternativeWorleyNoise>(
                tile_size, positive_chance, radius
            );
        }),
        "sample", &AlternativeWorleyNoise::get_noise
    );
}

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
