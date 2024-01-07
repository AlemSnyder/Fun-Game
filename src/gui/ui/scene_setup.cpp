#include "scene_setup.hpp"

#include "../render/graphics_shaders/program_handler.hpp"

// #include "../../world.hpp"
// #include "../render/graphics_shaders/non_instanced_i_mesh_shadow.hpp"
// #include "../render/graphics_shaders/sky.hpp"
// #include "../render/graphics_shaders/star.hpp"
// #include "../render/graphics_shaders/sun.hpp"

#include "../render/data_structures/screen_data.hpp"
#include "../render/data_structures/star_data.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/uniform_types.hpp"

#include <functional>
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

    auto matrix_view_projection_uniform =
        std::make_shared<render::MatrixViewProjection>();

    auto view_matrix_uniform = std::make_shared<render::ViewMatrix>();

    auto light_depth_projection_uniform =
        std::make_shared<render::LightDepthProjection>(&scene.get_shadow_map());

    auto shadow_texture_uniform =
        std::make_shared<render::TextureUniform>("shadow_texture", 1);

    auto material_color_texture_uniform =
        std::make_shared<render::TextureUniform>("material_color_texture", 0);

    auto spectral_light_color_uniform =
        std::make_shared<render::SpectralLight>(scene.get_lighting_environment());

    auto diffuse_light_color_uniform =
        std::make_shared<render::DiffuseLight>(scene.get_lighting_environment());

    auto light_direction_uniform =
        std::make_shared<render::LightDirection>(scene.get_lighting_environment());

    shader::Uniforms chunks_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_projection_uniform, view_matrix_uniform,
             light_depth_projection_uniform, shadow_texture_uniform,
             material_color_texture_uniform, spectral_light_color_uniform,
             diffuse_light_color_uniform, light_direction_uniform}
        )
    );

    std::function<void()> chunk_render_setup = []() {
        // Cull back-facing triangles -> draw only front-facing triangles
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        // Enable the depth test, and enable drawing to the depth texture
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    };

    auto chunks_render_program = std::make_shared<shader::ShaderProgram_Elements>(
        render_program, chunk_render_setup, chunks_render_program_uniforms
    );

    auto chunks_shadow_program = std::make_shared<shader::ShaderProgram_Elements>(
        shadow_program, chunk_render_setup, chunks_render_program_uniforms
    );

    scene.shadow_attach(chunks_shadow_program);

    scene.add_mid_ground_renderer(chunks_render_program);

    // should check that this does what I want it to.
    // auto chunk_renderer =
    // std::make_shared<data_structures::NonInstancedIMeshRenderer>(
    //    render_program, scene.get_lighting_environment()
    //);

    // for (const auto& chunk_mesh : terrain_mesh) {
    //     chunks_render_program.data.push_back(chunk_mesh);
    // }

    // auto chunk_shadow =
    //     std::make_shared<data_structures::NonInstancedIMeshShadow>(shadow_program);

    // for (const auto& chunk_mesh : terrain_mesh) {
    //     chunk_shadow->add_mesh(chunk_mesh);
    // }

    // Renders the Shadow depth map
    // chunk_renderer->set_shadow_map(&scene.get_shadow_map());

    // scene.add_mid_ground_renderer(chunk_renderer);
    // scene.shadow_attach(chunk_shadow);

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

    auto matrix_view_inverse_projection =
        std::make_shared<render::MatrixViewInverseProjection>();

    auto pixel_projection = std::make_shared<render::PixelProjection>();

    auto star_rotation_uniform =
        std::make_shared<render::StarRotationUniform>(scene.get_lighting_environment());

    // sky

    shader::Uniforms sky_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {pixel_projection, matrix_view_inverse_projection, light_direction_uniform,
             spectral_light_color_uniform}
        )
    );

    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDepthMask(GL_FALSE);
    };

    // in this order
    auto sky_renderer = std::make_shared<shader::ShaderProgram_Elements>(
        sky_program, sky_render_setup, sky_render_program_uniforms
    );

    // star

    shader::Uniforms star_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_inverse_projection, pixel_projection, star_rotation_uniform,
             light_direction_uniform}
        )
    );

    std::function<void()> star_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDepthMask(GL_FALSE);
    };

    auto star_renderer = std::make_shared<shader::ShaderProgram_Instanced>(
        stars_program, star_render_setup, star_render_program_uniforms
    );

    // auto star_renderer = std::make_shared<render::StarRenderer>(
    //     scene.get_lighting_environment(), scene.get_lighting_environment(),
    //     stars_program
    //);

    // sun

    shader::Uniforms sun_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_inverse_projection, pixel_projection, light_direction_uniform,
             spectral_light_color_uniform}
        )
    );

    // auto sun_renderer = std::make_shared<render::SunRenderer>(
    //     scene.get_lighting_environment(), sun_program
    //);

    std::function<void()> sun_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDepthMask(GL_FALSE);
    };

    // in this order
    auto sun_renderer = std::make_shared<shader::ShaderProgram_Standard>(
        sun_program, sun_render_setup, sun_render_program_uniforms
    );

    scene.add_background_ground_renderer(sky_renderer);
    scene.add_background_ground_renderer(star_renderer);
    scene.add_background_ground_renderer(sun_renderer);
}

} // namespace gui
