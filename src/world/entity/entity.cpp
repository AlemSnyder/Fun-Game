#include "entity.hpp"

#include "glm/gtx/transform.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"

#include <sol/sol.hpp>

namespace world {

namespace entity {

Entity::Entity(const Mesh& mesh) :
    mesh_and_positions_(std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(
        mesh, std::vector<glm::mat4>()
    )) {
    LOG_WARNING(
        logging::main_logger,
        "Entity constructor Entity(const Mesh& mesh) is depreciated!"
    );
}

Entity::Entity(
    const object_t& object_data, const manifest::descriptor_t& identification_data
) :
    name_(object_data.name),
    identification_(identification_data.identification) {
    const auto& model_data = object_data.models[0];
    // read mesh from path
    std::filesystem::path object_path_copy = identification_data.path;

    voxel_utility::VoxelObject model(
        files::get_data_path() / object_path_copy.remove_filename() / model_data.path
    );

    auto mesh = ambient_occlusion_mesher(model);

    mesh_and_positions_ =
        std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(mesh);

    if (object_data.ai) {
        std::filesystem::path ai_path = files::get_data_path()
                                        / object_path_copy.remove_filename()
                                        / object_data.ai.value();

        // load into All Lua
        // need access control
        GlobalContext& context = GlobalContext::instance();
        context.load_script_file(ai_path);

        has_ai_ = true;
    }
}

bool
Entity::remove(size_t index) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_.erase(local_positions_.begin() + index);
    // for (){}
    //  TODO update entity data_position
    return true;
}

bool
Entity::assign(size_t index, glm::mat4& data) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_[index] = data;
    return true;
}

std::string
Entity::identification() const {
    return identification_;
}

void
Entity::init_render(render_programs_t& programs) const {
    programs.entity_render_program->data.push_back(mesh_and_positions_.get());
}

void
Entity::update() {
    mesh_and_positions_->update_transforms_array(local_positions_, 0);
}

glm::vec3
Entity::decision(EntityInstance* entity_instance) {
    LocalContext& local_context = LocalContext::instance();
    std::optional<sol::object> update_function_query =
        local_context.get_from_lua(identification_ + "\\update");
    if (!update_function_query) {
        LOG_ERROR(
            logging::lua_logger, "Could not find Update function for entity {}.",
            identification_
        );
        return entity_instance->get_position();
    } else if (!update_function_query->is<sol::protected_function>()) {
        LOG_ERROR(
            logging::lua_logger, "Update is not a function for entity {}.",
            identification_
        );
        return entity_instance->get_position();
    }

    sol::protected_function update_function = update_function_query.value();
    if (!update_function.valid()) {
        LOG_ERROR(
            logging::lua_logger, "Update function for entity {} not valid.",
            identification_
        );
        return entity_instance->get_position();
    }
    sol::protected_function_result result = update_function(entity_instance);

    if (!result.valid()) {
        sol::error err = result;
        sol::call_status status = result.status();
        LOG_ERROR(logging::lua_logger, "{}: {}", sol::to_string(status), err.what());
        return entity_instance->get_position();
    }

    sol::table vector_table = result;
    glm::vec3 position;
    position.x = vector_table["x"];
    position.y = vector_table["y"];
    position.z = vector_table["z"];
    return position;
}

EntityInstance::EntityInstance(std::shared_ptr<Entity> entity_type) :
    entity_type_(entity_type), data_position_(entity_type->add()) {}

EntityInstance::~EntityInstance() {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->remove(data_position_);
    }
}

void
EntityInstance::update() {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        glm::vec3 position = entity_type->decision(this);
        set_position(position);
    }
}

void
EntityInstance::update(glm::mat4&& data) {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->assign(data_position_, data);
    }
}

void
EntityInstance::destroy() {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->remove(data_position_);
    }
    data_position_ = -1; // ya I know assigning -1 to size_t
}

glm::vec3
EntityInstance::get_position() const {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        return position_;
    }

    return {0, 0, 0};
}

void
EntityInstance::set_position(glm::vec3 position) {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        position_ = position;
        glm::mat4 transformation(1.0);
        glm::mat4 data = glm::translate(transformation, position);
        entity_type->assign(data_position_, data);
    }
}

std::shared_ptr<Object>
EntityInstance::get_object() {
    return entity_type_.lock();
}

std::shared_ptr<const Object>
EntityInstance::get_object() const {
    return entity_type_.lock();
}

} // namespace entity

} // namespace world
