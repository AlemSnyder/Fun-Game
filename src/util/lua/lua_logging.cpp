#include "lua_logging.hpp"

#include "global_context.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"
#include "util/time.hpp"

#include <future>

namespace lua_logging {

int
lua_loadtime_test() {
    LOG_INFO(logging::main_logger, "Getting Local Lua State.");
    LocalContext& local_context = LocalContext::instance();
    sol::state& lua = local_context.get_lua_state();
    LOG_INFO(logging::main_logger, "Got Local Lua State.");
    LOG_INFO(logging::main_logger, "Loading Lua File.");

    std::filesystem::path lua_script_path =
        files::get_resources_path() / "lua" / "is_prime_test.lua";

    sol::table result =
        lua.require_file("is_prime_test", lua_script_path.string(), false);

    if (!result.valid()) {
        LOG_WARNING(logging::main_logger, "is prime test failed to import.");
        return 1;
    }

    for (const auto& key : result) {
        if (key.first.is<std::string>()) {
            std::string string_key = key.first.as<std::string>();
            LOG_INFO(logging::main_logger, "{}", string_key);
        }
    }

    {
        sol::protected_function is_prime_function = result["tests"]["is_prime"];

        LOG_INFO(logging::main_logger, "Got Lua function, calling.");

        auto function_result = is_prime_function.call(97);

        if (!function_result.valid()) {
            sol::error err = function_result;
            std::string what = err.what();
            LOG_DEBUG(logging::main_logger, "{}", what);
            return 1;
        }

        int is_prime_result = function_result;

        std::string log_value = is_prime_result == 1 ? "correct" : "incorrect";
        LOG_INFO(
            logging::main_logger, "Got Lua {} from Lua function. Is {} value.",
            is_prime_result, log_value
        );
    }

    {
        std::vector<std::chrono::nanoseconds> load_times;
        std::vector<std::chrono::nanoseconds> run_times;

        for (size_t y = 0; y < 100; y++) {
            auto l_start = time_util::get_time_nanoseconds();

            std::filesystem::path prime_test_file_path =
                files::get_resources_path() / "lua" / "is_prime_test.lua";

            sol::table biome_library =
                lua.require_file("is_prime_test", prime_test_file_path.string(), false);

            sol::protected_function is_prime_function =
                biome_library["tests"]["is_prime"];

            auto l_end = time_util::get_time_nanoseconds();
            load_times.push_back(l_end - l_start);

            auto r_start = time_util::get_time_nanoseconds();

            auto function_result = is_prime_function.call(97);

            if (!function_result.valid()) {
                sol::error err = function_result;
                std::string what = err.what();
                LOG_DEBUG(logging::main_logger, "{}", what);
                return 1;
            }

            auto r_end = time_util::get_time_nanoseconds();

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
lua_log_test() {
    LocalContext& local_context = LocalContext::instance();
    sol::state& lua = local_context.get_lua_state();

    sol::protected_function lua_log_critical = lua["Logging"]["LOG_CRITICAL"];

    auto result = lua_log_critical.call("Critical message from cpp!!");

    if (!result.valid()) {
        sol::error err = result; // who designed this?
        std::string what = err.what();
        LOG_DEBUG(logging::main_logger, "{}", what);
    }

    return 0;
}

int
lua_transfertime_test() {
    LOG_INFO(logging::main_logger, "Loading Lua File.");

    std::filesystem::path lua_script_path =
        files::get_resources_path() / "lua" / "is_prime_test.lua";

    GlobalContext& context = GlobalContext::instance();
    context.load_script_file(lua_script_path);

    {
        std::optional<sol::object> result = context.get_from_lua("tests\\is_prime");

        if (!result) {
            return 1;
        }
        if (!result->is<sol::protected_function>()) {
            return 1;
        }

        sol::protected_function is_prime_function =
            result->as<sol::protected_function>();

        auto function_result = is_prime_function.call(97);

        if (!function_result.valid()) {
            sol::error err = function_result;
            std::string what = err.what();
            LOG_DEBUG(logging::main_logger, "{}", what);
            return 1;
        }

        int is_prime_result = function_result;

        std::string log_value = is_prime_result == 1 ? "correct" : "incorrect";
        LOG_INFO(
            logging::main_logger, "Got Lua {} from Lua function. Is {} value.",
            is_prime_result, log_value
        );
    }

    std::future<int> future_result = context.submit_task([]() {
        std::vector<std::chrono::nanoseconds> load_times;
        std::vector<std::chrono::nanoseconds> run_times;

        for (size_t y = 0; y < 101; y++) {
            auto l_start = time_util::get_time_nanoseconds();

            LocalContext& local_context = LocalContext::instance();

            std::optional<sol::object> result =
                local_context.get_from_lua("tests\\is_prime");

            if (!result) {
                LOG_CRITICAL(logging::lua_logger, "Could not find is_prime.");
                return 1;
            }
            if (!result->is<sol::protected_function>()) {
                return 1;
            }

            sol::protected_function is_prime_function =
                result->as<sol::protected_function>();

            auto l_end = time_util::get_time_nanoseconds();

            load_times.push_back(l_end - l_start);

            auto r_start = time_util::get_time_nanoseconds();

            auto function_result = is_prime_function.call(97);

            if (!function_result.valid()) {
                sol::error err = function_result;
                std::string what = err.what();
                LOG_DEBUG(logging::main_logger, "{}", what);
                return 1;
            }

            auto r_end = time_util::get_time_nanoseconds();

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

        return 0;
    });

    int subprocess_status = future_result.get();

    return subprocess_status;
}

int
lua_load_tests() {
    // load is_prime_test.lua
    LOG_INFO(logging::main_logger, "Loading Lua File.");

    std::filesystem::path lua_script_path =
        files::get_resources_path() / "lua" / "is_prime_test.lua";

    GlobalContext& context = GlobalContext::instance();
    context.load_script_file(lua_script_path);

    // this should work
    std::future<int> future_1 = context.submit_task([]() {
        LocalContext& local_context = LocalContext::instance();
        bool return_status = local_context.load_into_this_lua_state("tests");
        // true when no erro -> 0 (!true)
        return static_cast<int>(!return_status);
    });

    int value_1 = future_1.get();
    if (value_1 != 0) {
        LOG_ERROR(logging::lua_logger, "load_into_this_lua_state failed");
        return 1;
    }

    // this should also work
    std::future<int> future_2 = context.submit_task([]() {
        LocalContext& local_context = LocalContext::instance();
        bool return_status = local_context.load_into_this_lua_state("tests");
        if (!return_status) {
            return 1;
        }
        std::optional<sol::object> tests_table =
            local_context.get_from_this_lua_state("tests");
        if (tests_table) {
            if (tests_table->is<sol::table>()) {
                sol::table tests_table_table = tests_table.value();
                for (auto& [key, value] : tests_table_table) {
                    LOG_INFO(logging::lua_logger, "key: {}", key.as<std::string>());
                }
            }
        }

        std::optional<sol::object> is_prime_function =
            local_context.get_from_this_lua_state("tests\\is_prime");
        if (!is_prime_function) {
            LOG_ERROR(logging::lua_logger, "Could not load tests table.");
            return 1;
        }
        local_context.set_to_this_lua_state(
            "tests\\is_not_not_prime", is_prime_function.value()
        );
        return 0;
    });

    int value_2 = future_2.get();
    if (value_2 != 0) {
        LOG_ERROR(logging::lua_logger, "get from and set to this lua state failed");
        return 1;
    }
    return 0;
}

} // namespace lua_logging
