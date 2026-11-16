#include "object_handler.hpp"

#include "util/files.hpp"
#include "util/voxel.hpp"
#include "world/object/entity/entity.hpp"
#include "world/object/entity/tile_object.hpp"

#include <utility>

namespace manifest {

std::shared_ptr<world::object::entity::Object>
ObjectHandler::get_object(const std::string& id) {
    auto value = ided_objects.find(id);
    if (value == ided_objects.end()) {
        return nullptr;
    } else {
        return value->second;
    }
}

std::shared_ptr<const world::object::entity::Object>
ObjectHandler::get_object(const std::string& id) const {
    return const_cast<const ObjectHandler*>(this)->get_object(id);
}

void
ObjectHandler::read_biome(const descriptor_t& biome_descriptor) {
    GlobalContext& context = GlobalContext::instance();

    std::filesystem::path path_copy = biome_descriptor.path;

    auto biome_data = files::read_json_from_file<terrain::generation::biome_data_t>(
        files::get_data_path() / path_copy
    );
    if (!biome_data.has_value()) {
        return;
    }

    auto biome_data_value_pointer = std::make_shared<terrain::generation::biome_data_t>(
        std::move(biome_data.value())
    );

    biome_data_value_pointer->map_generator_path =
        files::get_data_path() / path_copy.remove_filename()
        / biome_data_value_pointer->map_generator_path;

    biome_data_value_pointer->image_path = files::get_data_path()
                                           / path_copy.remove_filename()
                                           / biome_data_value_pointer->image_path;

    biome_data_value_pointer->materials_path =
        files::get_data_path() / path_copy.remove_filename()
        / biome_data_value_pointer->materials_path;

    ided_biomes.insert(
        std::make_pair(biome_descriptor.identification, biome_data_value_pointer)
    );

    //    context.load_file(
    //        manifest_name, files::get_data_path() / path_copy.remove_filename()
    //                    / biome_data->map_generator_path
    //    );
}

std::shared_ptr<terrain::generation::biome_data_t>
ObjectHandler::get_biome(const std::string& biome_id) {
    auto value = ided_biomes.find(biome_id);
    if (value == ided_biomes.end()) {
        return nullptr;
    } else {
        return value->second;
    }
}

} // namespace manifest