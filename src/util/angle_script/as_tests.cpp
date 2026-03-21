
#include "logging.hpp"
#include <angelscript.h> // TODO linker error determine how to build AS
namespace as_test {

int
test() {
	asIScriptEngine *engine = asCreateScriptEngine();
    if (!engine) {
        LOG_ERROR(logging::main_logger, "Could no start Angle Script engine.");
        return 1;
    }

    engine->ShutDownAndRelease();
    return 0;
}

}

