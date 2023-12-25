#include "scene_setup.hpp"

#include "../../world.hpp"
#include "../render/graphics_shaders/non_instanced_i_mesh_shadow.hpp"
#include "../render/graphics_shaders/sky.hpp"
#include "../render/graphics_shaders/star.hpp"
#include "../render/graphics_shaders/sun.hpp"

#include <memory>

namespace gui {

void
setup(Scene& scene, shader::ShaderHandler& shader_handler, World& world) {
    // assign map from all color ids to each color
    // to package as a texture

    terrain::TerrainColorMapping::assign_color_mapping(world.get_materials());
    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();
    world.update_all_chunks_mesh();

    auto terrain_mesh = world.get_chunks_mesh();

    shader::Program& render_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );
    shader::Program& shadow_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );

    // should check that this does what I want it to.
    auto chunk_renderer = std::make_shared<
        models::NonInstancedIMeshRenderer<data_structures::TerrainMesh>>(
        render_program, scene.get_lighting_environment()
    );

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_renderer->add_mesh(chunk_mesh);
    }

    auto chunk_shadow =
        std::make_shared<models::NonInstancedIMeshShadow<data_structures::TerrainMesh>>(
            shadow_program
        );

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_shadow->add_mesh(chunk_mesh);
    }

    // Renders the Shadow depth map
    chunk_renderer->set_shadow_map(&scene.get_shadow_map());

    scene.add_mid_ground_renderer(chunk_renderer);
    scene.shadow_attach(chunk_shadow);

    shader::Program& sky_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    shader::Program& stars_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Stars.vert",
        files::get_resources_path() / "shaders" / "background" / "Stars.frag"
    );

    shader::Program& sun_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Sun.vert",
        files::get_resources_path() / "shaders" / "background" / "Sun.frag"
    );

    // in this order
    auto sky_renderer = std::make_shared<render::SkyRenderer>(
        scene.get_lighting_environment(), sky_program
    );

    auto star_renderer = std::make_shared<render::StarRenderer>(
        scene.get_lighting_environment(), scene.get_lighting_environment(),
        stars_program
    );
    auto sun_renderer = std::make_shared<render::SunRenderer>(
        scene.get_lighting_environment(), sun_program
    );

    scene.add_background_ground_renderer(sky_renderer);
    scene.add_background_ground_renderer(star_renderer);
    scene.add_background_ground_renderer(sun_renderer);
}

} // namespace gui
