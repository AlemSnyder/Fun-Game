void test1() {
    int x = 5;
    return;
}

int test2() {
    int x = 1 * 2 * 3 * 4 * 5 * 6;
    if (x % 2 == 0) {
        return 2;
    }
    return 1;
}

int test3() {
    LOGGING::LOG_BACKTRACE("Backtrace log 1");
    LOGGING::LOG_BACKTRACE("Backtrace log 2");

    LOGGING::LOG_INFO("Welcome to AngelScript!");
    LOGGING::LOG_ERROR("An error message. error code 123");
    LOGGING::LOG_WARNING("A warning message.");
    LOGGING::LOG_CRITICAL("A critical error.");
    LOGGING::LOG_DEBUG("Debugging foo 1234");

    return 0;
}