#include "scene_setup.hpp"

#include "../../world.hpp"

#include <memory>

namespace gui {

void
setup(Scene& scene, World& world) {
    ShaderHandler shader_handler;

    // assign map from all color ids to each color
    // to package as a texture
    terrain::TerrainColorMapping::assign_color_mapping(world.get_materials());
    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();
    world.update_all_chunks_mesh();

    auto terrain_mesh = world.get_chunks_mesh();

    // should check that this does what I want it to.
    auto chunk_renderer = std::make_shared<
        models::NonInstancedIMeshRenderer<data_structures::TerrainMesh>>(scene.get_lighting_environment(), shader_handler
    );

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_renderer->add_mesh(chunk_mesh);
    }

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 40.2f, 40.4)) // direction
        * 128.0f;                                       // length

    // Renders the Shadow depth map
    scene.set_shadow_light_direction(light_direction);

    scene.frame_buffer_multisample_attach(chunk_renderer);
    scene.shadow_attach(chunk_renderer);

}



} // namespace gui
