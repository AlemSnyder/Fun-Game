#include "files.hpp"
#include "time.hpp"
#include <iostream>


#include <quill/Quill.h>

namespace logging_util {

quill::Handler* current_handler;

/**
 * @brief Create a Handler pointer
 * 
 * @details Creates a folder and file for the log message. This should not be 
 * called more than once when the program is run.
 * 
 * @return quill::Handler* 
 */
inline quill::Handler*
create_log_handler() {
    auto log_folder = files::get_root_path() / "logs";
    if (!std::filesystem::is_directory(log_folder)) {
        // create directory
        std::filesystem::create_directory(log_folder);
    }

    auto time_formatter = quill::detail::TimestampFormatter("%Y-%m-%d - %H-%M-%S.log", quill::Timezone::LocalTime);

    //auto time_string = time_formatter.format_timestamp(time_util::get_time_nanoseconds());

    auto log_file_path = files::get_root_path() / "logs" / time_formatter.format_timestamp(time_util::get_time_nanoseconds());

    std::cout << log_file_path;

    current_handler = quill::file_handler(log_file_path, "w");
    return current_handler;
}

inline quill::Handler*
get_current_log_handler(){
    return current_handler;
}

} // namespace logging_util