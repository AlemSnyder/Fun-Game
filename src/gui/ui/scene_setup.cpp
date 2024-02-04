#include "scene_setup.hpp"

#include "../render/array_buffer/screen_data.hpp"
#include "../render/array_buffer/star_data.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/uniform_types.hpp"

#include <functional>
#include <memory>

namespace gui {

// Setup a scene object.
// This is a helper function such that exists so that both programs with, and
// without imgui load the same scene.
// Its not in scene because I want to keep this out of the game engine part of
// the code base.
void
setup(Scene& scene, shader::ShaderHandler& shader_handler, World& world) {
    // assign map from all color ids to each color to package as a texture
    terrain::TerrainColorMapping::assign_color_mapping(world.get_materials());
    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();
    // parallel program to generate a mesh for each chunk
    world.update_all_chunks_mesh();

    auto terrain_mesh = world.get_chunks_mesh();

    auto star_shape = std::make_shared<array_buffer::StarShape>();

    auto star_data =
        std::make_shared<array_buffer::StarData>(files::get_data_path() / "stars.json");

    auto screen_data = std::make_shared<array_buffer::ScreenData>();

    // Load programs from files
    // clang-format off
    shader::Program& render_program = shader_handler.load_program(
        "Render Chunks",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );
    shader::Program& shadow_program = shader_handler.load_program(
        "Shadow Chunks",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );

    shader::Program& sky_program = shader_handler.load_program(
        "Sky",
        files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    shader::Program& stars_program = shader_handler.load_program(
        "Stars",
        files::get_resources_path() / "shaders" / "background" / "Stars.vert",
        files::get_resources_path() / "shaders" / "background" / "Stars.frag"
    );

    shader::Program& sun_program = shader_handler.load_program(
        "Sun",
        files::get_resources_path() / "shaders" / "background" / "Sun.vert",
        files::get_resources_path() / "shaders" / "background" / "Sun.frag"
    );
    // clang-format on

    // These are uniforms
    auto matrix_view_projection_uniform =
        std::make_shared<render::MatrixViewProjection>();

    auto view_matrix_uniform = std::make_shared<render::ViewMatrix>();

    auto light_depth_projection_uniform =
        std::make_shared<render::LightDepthProjection>(&scene.get_shadow_map());

    auto light_depth_texture_projection_uniform =
        std::make_shared<render::LightDepthTextureProjection>(&scene.get_shadow_map());

    auto shadow_texture_uniform = std::make_shared<render::TextureUniform>(
        "shadow_texture", "sampler2DShadow", 1
    );

    auto material_color_texture_uniform = std::make_shared<render::TextureUniform>(
        "material_color_texture", "sampler1D", 0
    );

    auto spectral_light_color_uniform =
        std::make_shared<render::SpectralLight>(scene.get_lighting_environment());

    auto diffuse_light_color_uniform =
        std::make_shared<render::DiffuseLight>(scene.get_lighting_environment());

    auto light_direction_uniform =
        std::make_shared<render::LightDirection>(scene.get_lighting_environment());

    auto matrix_view_inverse_projection =
        std::make_shared<render::MatrixViewInverseProjection>();

    auto pixel_projection = std::make_shared<render::PixelProjection>();

    auto star_rotation_uniform =
        std::make_shared<render::StarRotationUniform>(scene.get_lighting_environment());

    // Uniforms as a vector
    shader::UniformsVector chunks_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_projection_uniform, view_matrix_uniform,
             light_depth_texture_projection_uniform, shadow_texture_uniform,
             material_color_texture_uniform, spectral_light_color_uniform,
             diffuse_light_color_uniform, light_direction_uniform}
        )
    );

    shader::UniformsVector chunks_shadow_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>({light_depth_projection_uniform})
    );

    shader::UniformsVector sun_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_projection_uniform, pixel_projection, light_direction_uniform,
             spectral_light_color_uniform}
        )
    );

    shader::UniformsVector sky_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_inverse_projection, light_direction_uniform,
             spectral_light_color_uniform}
        )
    );

    shader::UniformsVector star_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {matrix_view_projection_uniform, pixel_projection, star_rotation_uniform,
             light_direction_uniform}
        )
    );

    // program setup functions
    std::function<void()> chunk_render_setup = []() {
        // Cull back-facing triangles -> draw only front-facing triangles
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        // Enable the depth test, and enable drawing to the depth texture
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    };

    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    // the programs
    // chunks
    auto chunks_render_program = std::make_shared<shader::ShaderProgram_Elements>(
        render_program, chunk_render_setup, chunks_render_program_uniforms
    );

    // chunks shadow
    auto chunks_shadow_program = std::make_shared<shader::ShaderProgram_Elements>(
        shadow_program, chunk_render_setup, chunks_shadow_program_uniforms
    );

    // sky
    auto sky_renderer = std::make_shared<shader::ShaderProgram_Standard>(
        sky_program, sky_render_setup, sky_render_program_uniforms
    );

    // star
    auto star_renderer = std::make_shared<shader::ShaderProgram_Instanced>(
        stars_program, sky_render_setup, star_render_program_uniforms
    );

    // sun
    auto sun_renderer = std::make_shared<shader::ShaderProgram_Standard>(
        sun_program, sky_render_setup, sun_render_program_uniforms
    );

    // assign data
    sky_renderer->data.push_back(screen_data);
    star_renderer->data.push_back(star_data);
    sun_renderer->data.push_back(star_shape);

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_mesh->set_shadow_texture(scene.get_shadow_map().get_depth_texture());
        chunks_render_program->data.push_back(chunk_mesh);
    }

    for (const auto& chunk_mesh : terrain_mesh) {
        chunks_shadow_program->data.push_back(chunk_mesh);
    }

    // attach program to scene
    scene.shadow_attach(chunks_shadow_program);
    scene.add_mid_ground_renderer(chunks_render_program);

    scene.add_background_ground_renderer(sky_renderer);
    scene.add_background_ground_renderer(star_renderer);
    scene.add_background_ground_renderer(sun_renderer);
}

} // namespace gui
