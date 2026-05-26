
#include "as_logging.hpp"
#include "global_context.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "scriptstdstring.h" // hm
#include "util/files.hpp"
#include "util/time.hpp"

#include <angelscript.h>

namespace util {
namespace scripting {

int
as_loadtime_test() {
    LOG_INFO(logging::main_logger, "Getting global and local context.");
    GlobalContext& context = GlobalContext::instance();
    LocalContext& local_context = LocalContext::instance();

    {
        context.load_file(
            "test_module", files::get_resources_path() / "as" / "test.as"
        );
        auto is_prime_function =
            context.get_function("test_module", "bool is_prime(int)");

        LOG_INFO(logging::main_logger, "Got as function, calling.");

        auto function_result = local_context.run_function(is_prime_function, 97);

        // TODO write a better error logging mechanism
        if (function_result != AngelScript::asEXECUTION_FINISHED) {
            return 1;
        }

        bool is_prime_result;
        local_context.get_return_value(is_prime_result);

        std::string log_value = is_prime_result ? "correct" : "incorrect";
        LOG_INFO(
            logging::main_logger, "Got {} from as function. Is {} value.",
            is_prime_result, log_value
        );
    }

    {
        std::vector<std::chrono::nanoseconds> load_times;
        std::vector<std::chrono::nanoseconds> run_times;

        for (size_t y = 0; y < 100; y++) {
            auto l_start = time_util::get_time_nanoseconds();

            auto is_prime_function =
                context.get_function("test_module", "bool is_prime(int)");
            auto l_end = time_util::get_time_nanoseconds();

            auto r_start = time_util::get_time_nanoseconds();

            auto function_result = local_context.run_function(is_prime_function, 97);

            // TODO write a better error logging mechanism
            if (function_result != AngelScript::asEXECUTION_FINISHED) {
                return 1;
            }

            bool is_prime_result;
            local_context.get_return_value(is_prime_result);

            auto r_end = time_util::get_time_nanoseconds();

            load_times.push_back(l_end - l_start);

            run_times.push_back(r_end - r_start);
        }

        std::chrono::nanoseconds r_mean(0);
        for (size_t i = 1; i < run_times.size(); i++) {
            std::chrono::nanoseconds duration = run_times[i];
            r_mean += duration;
        }
        r_mean /= (run_times.size() - 1);

        std::chrono::nanoseconds l_mean(0);
        for (size_t i = 1; i < load_times.size(); i++) {
            std::chrono::nanoseconds duration = load_times[i];
            l_mean += duration;
        }
        l_mean /= (load_times.size() - 1);

        LOG_INFO(
            logging::main_logger,
            "Mean load time of {} samples is {}ns. First load time is {}ns",
            (load_times.size() - 1), int64_t(l_mean.count()), load_times[0].count()
        );

        LOG_INFO(
            logging::main_logger,
            "Mean execution time of {} samples is {}ns. First execution time is {}ns.",
            (run_times.size() - 1), int64_t(r_mean.count()), run_times[0].count()
        );
    }

    return 0;
}

int
logging_test() {
    GlobalContext& context = GlobalContext::instance();
    LocalContext& local_context = LocalContext::instance();

    auto result_1 = context.load_file("test_module", files::get_resources_path() / "as" / "test.as");

    AngelScript::asIScriptFunction* funct1 =
       context.get_function("test_module", "int test3()");

    if (funct1 == nullptr) {
        return 1;
    }
    auto result_2 = local_context.run_function(funct1);
    if (result_2 != AngelScript::asEXECUTION_FINISHED) {
        return 1;
    }
    return 0;
}

int
test() {
    LOG_ERROR(logging::script_logger, "");

    GlobalContext& context = GlobalContext::instance();
    LocalContext& local_context = LocalContext::instance();

    auto result_1 = context.load_file("test_module", files::get_resources_path() / "as" / "test.as");

    AngelScript::asIScriptFunction* funct1 =
       context.get_function("test_module", "int test1()");

    if (funct1 == nullptr) {
        return 1;
    }
    auto result_2 = local_context.run_function(funct1);
    if (result_2 != AngelScript::asEXECUTION_FINISHED) {
        return 1;
    }
    AngelScript::asIScriptFunction* funct2 =
       context.get_function("test_module", "int test1()");

    if (funct2 == nullptr) {
        return 1;
    }
    result_2 = local_context.run_function(funct2);
    if (result_2 != AngelScript::asEXECUTION_FINISHED) {
        return 1;
    }

    return 0;
}

int
as_threading() {
    GlobalContext& context = GlobalContext::instance();
    context.load_file("test_module", files::get_resources_path() / "as" / "test.as");

    std::future<int> future = context.submit_task([]() {
        GlobalContext& context = GlobalContext::instance();
        LocalContext& local_context = LocalContext::instance();

        auto function = context.get_function("test_module", "int text3()");
        int result = local_context.run_function(function);
        return result;
    });

    int result = future.get();

    return result;
}

int
as_load_tests() {
    GlobalContext& context = GlobalContext::instance();
    LocalContext& local_context = LocalContext::instance();

    context.load_file("main", files::get_resources_path() / "as" / "test.as");
    context.load_file("Base", files::get_data_path() / "Base" / "biome_map.as");

    auto function = context.get_function("Base", "void do_something()");
    int result = local_context.run_function(function);
    if (result != AngelScript::asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript Test");
        return 1;
    }

    auto type = context.get_type("Base", "Base::biomes::biome_map");
    int factory_count = type->GetFactoryCount();
    LOG_DEBUG(logging::main_logger, "Found {} factory functions", factory_count);

    auto factory_function =
        type->GetFactoryByDecl("Base::biomes::biome_map@ biome_map()");
    auto factory_function_2 = type->GetFactoryByIndex(0);

    auto declaration = factory_function_2->GetDeclaration(true, true, true);

    auto factory_function_3 = type->GetFactoryByDecl(declaration);

    LOG_DEBUG(
        logging::main_logger, "{}, {}, {}", factory_function != nullptr,
        factory_function_2 != nullptr, factory_function_3 != nullptr
    );

    LOG_DEBUG(logging::main_logger, "{}", declaration);

    result = local_context.run_function(factory_function);
    if (result != AngelScript::asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript getting biome map");
        return 1;
    }

    AngelScript::asIScriptObject* biome_map = local_context.get_return_object();
    if (biome_map == nullptr) {
        LOG_ERROR(logging::main_logger, "Failed to get object");
        return 1;
    }
    biome_map->AddRef();

    AngelScript::asIScriptFunction* method =
        type->GetMethodByDecl("int sample(int, int)");
    result = local_context.run_method(biome_map, method, 5, 5);
    if (result != AngelScript::asEXECUTION_FINISHED) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript run sample");
        return 1;
    }

    int return_value;
    result = local_context.get_return_value(return_value);
    if (result != 0) {
        LOG_ERROR(
            logging::main_logger, "Failed to get result from sample. Error: {}", result
        );
        return 1;
    }

    LOG_DEBUG(logging::main_logger, "Got result {}.", return_value);
    biome_map->Release();
    return 0;
}

} // namespace scripting

} // namespace util
