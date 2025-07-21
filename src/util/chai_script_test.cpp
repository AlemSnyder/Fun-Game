#include "chai_script_test.hpp"
#include "files.hpp"

#include "chaiscript/chaiscript.hpp"
#include "global_context.hpp"
#include "logging.hpp"
#include "util/time.hpp"

#include <functional>
#include <chrono>

int
chai_script_test() {
    chaiscript::ChaiScript chai;
    chai.eval(R"(print("Hello World"))");
    //envi.eval("print(\"Some Text\")");

    std::function<void()>  print_test = chai.eval<std::function<void()>>("fun() { print(\"Some Text_2\");}");
    print_test();

    std::function<bool(int)>  prime_test = chai.eval_file<std::function<bool(int)>>(files::get_root_path() / "scripts" / "chaiscript_test_1.chai");

    std::function<bool(int)>  prime_test_1 = chai.eval<std::function<bool(int)>>("prime_test");

    prime_test(97);
    prime_test_1(97);


    return 0;
}

int chai_script_reload_test() {
    chaiscript::ChaiScript chai;

    chai.eval_file(files::get_root_path() / "scripts" / "chaiscript_test_2a.chai");

    std::function<int()>  get_1 = chai.eval<std::function<int()>>("return_a_number");

    LOG_INFO(logging::main_logger, "Got vale {}, expecting 1", get_1());

    if (get_1() != 1) {
        return 1;
    }

    chai.eval_file(files::get_root_path() / "scripts" / "chaiscript_test_2b.chai");

    std::function<int()>  get_2 = chai.eval<std::function<int()>>("return_a_number");

    LOG_INFO(logging::main_logger, "Got vale {}, expecting 2", get_2());

    if (get_2() != 2) {
        return 1;
    }

    LOG_INFO(logging::main_logger, "Got vale {}, expecting 1", get_1());

    if (get_1() != 1) {
        return 1;
    }

    return 0;
}

int chai_script_mp_test() {

    chaiscript::ChaiScript chai;

    std::function<bool(int)>  prime_test = chai.eval_file<std::function<bool(int)>>(files::get_root_path() / "scripts" / "chaiscript_test_1.chai");

    prime_test(97);

    // time 

    GlobalContext& ctx = GlobalContext::instance();

    ctx.push_task([&chai](){

        std::function<bool(int)>  prime_test = chai.eval_file<std::function<bool(int)>>(files::get_root_path() / "scripts" / "chaiscript_test_1.chai");

        prime_test(97);

    });

        std::future<int> future_result = ctx.submit_task([&chai]() {
        std::vector<std::chrono::nanoseconds> load_times;
        std::vector<std::chrono::nanoseconds> run_times;

        for (size_t y = 0; y < 101; y++) {
            auto l_start = time_util::get_time_nanoseconds();

            std::function<bool(int)> is_prime = chai.eval<std::function<bool(int)>>("prime_test");

            auto l_end = time_util::get_time_nanoseconds();

            load_times.push_back(l_end - l_start);

            auto r_start = time_util::get_time_nanoseconds();

            bool function_result = is_prime(97);

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


    return 0;
}
