
#include "loading.hpp"

#include "world/entity/object_handler.hpp"

#include <json/json.h>

#include <global_context.hpp>

#include <iostream>
#include <string>

namespace util {

void
load_manifest() {
    auto manifest_folder = files::get_manifest_path();

    world::entity::ObjectHandler& object_handler =
        world::entity::ObjectHandler::instance();

    for (const auto& directory_entry :
         std::filesystem::directory_iterator(manifest_folder)) {
        Json::Value manifest;

        // path to manifest file
        auto manifest_file = directory_entry.path();

        auto contents = files::open_data_file(manifest_file);
        if (contents.has_value()) [[likely]] {
            // might fail
            // if the json has some problems this will throw

            Json::CharReaderBuilder builder;
            builder["collectComments"] = false;

            Json::String errs;
            if (!Json::parseFromStream(builder, contents.value(), &manifest, &errs)) {
                LOG_WARNING(logging::file_io_logger, "{}", errs.c_str());
                continue;
            }

        } else {
            // Warning is already logged when opening file
            // just move onto next file
            continue;
        }

        if (!manifest.isObject()) [[unlikely]] {
            LOG_WARNING(
                logging::file_io_logger,
                "Manifest file {} did not produce a valid JSON object.", manifest_file
            );
            continue;
        }

        // TODO @AlemSnyder log a bunch of information about the manifest
        // this information doesn't exist yet, but when it does

        // iterate through objects in manifest and queue them to be loaded
        for (const Json::Value& entity_data : manifest["entities"]) {
            std::filesystem::path entity_path =
                files::get_data_path() / entity_data["path"].asString();

            // Will check if path exists
            GlobalContext& context = GlobalContext::instance();
            auto future = context.submit_task([entity_path]() {
                world::entity::ObjectHandler& object_handler =
                    world::entity::ObjectHandler::instance();
                object_handler.read_object(entity_path);
            }

            );
        }
    }
}

} // namespace util
