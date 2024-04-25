#include "scene_setup.hpp"

#include "../render/gpu_data/screen_data.hpp"
#include "../render/gpu_data/shadow_calculation_data.hpp"
#include "../render/gpu_data/star_data.hpp"
#include "../render/gpu_data/static_mesh.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/uniform_types.hpp"
#include "world/entity/object_handler.hpp"

#include <functional>
#include <memory>

namespace gui {

// Setup a scene object.
// This is a helper function such that exists so that both programs with, and
// without imgui load the same scene.
// Its not in scene because I want to keep this out of the game engine part of
// the code base.
void
setup(Scene& scene, shader::ShaderHandler& shader_handler, world::World& world) {
    // assign map from all color ids to each color
    // to package as a texture
    terrain::TerrainColorMapping::assign_color_mapping(world.get_materials());
    // send color texture to gpu
    // terrain::TerrainColorMapping::assign_color_texture();
    // parallel program to generate a mesh for each chunk
    world.update_all_chunks_mesh();

    auto terrain_mesh = world.get_chunks_mesh();

    auto star_shape = std::make_shared<gpu_data::StarShape>();

    auto star_data =
        std::make_shared<gpu_data::StarData>(files::get_data_path() / "stars.json");

    auto screen_data = std::make_shared<gpu_data::ScreenData>();

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

    // TODO need to write another program like this
    // needs to have sub block spacing

    shader::Program& tile_entity_render_program = shader_handler.load_program(
        "Tile Entity Render",
        files::get_resources_path() / "shaders" / "scene" / "TileEntity.vert",
        files::get_resources_path() / "shaders" / "scene" / "TileEntity.frag"
    );

    shader::Program& entity_render_program = shader_handler.load_program(
        "Instanced Render",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMappingInstanced.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );
    shader::Program& entity_shadow_program = shader_handler.load_program(
        "Instanced Shadow Map",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTTInstanced.vert",
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

    // Program to render final shadow quadrate.
    shader::Program& shadow_mid_program = shader_handler.load_program(
        "Shadow middle",
        files::get_resources_path() / "shaders" / "util" / "screen.vert",
        files::get_resources_path() / "shaders" / "util" / "average.frag"
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

    auto shadow_texture_back_uniform = std::make_shared<render::TextureUniform>(
        "shadow_texture_back", "sampler2DShadow", 0
    );

    auto shadow_texture_front_uniform = std::make_shared<render::TextureUniform>(
        "shadow_texture_front", "sampler2DShadow", 1
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

    shader::UniformsVector shadow_texture_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>(
            {shadow_texture_back_uniform, shadow_texture_front_uniform}
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

    // program setup functions
    std::function<void()> chunk_render_setup_back = []() {
        // Cull back-facing triangles -> draw only front-facing triangles
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
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
    auto chunks_front_shadow_program = std::make_shared<shader::ShaderProgram_Elements>(
        shadow_program, chunk_render_setup, chunks_shadow_program_uniforms
    );

    auto chunks_back_shadow_program = std::make_shared<shader::ShaderProgram_Elements>(
        shadow_program, chunk_render_setup_back, chunks_shadow_program_uniforms
    );

    auto entity_render_program_execute =
        std::make_shared<shader::ShaderProgram_ElementsInstanced>(
            entity_render_program, chunk_render_setup, chunks_render_program_uniforms
        );

    auto entity_shadow_front_program_execute =
        std::make_shared<shader::ShaderProgram_ElementsInstanced>(
            entity_shadow_program, chunk_render_setup, chunks_shadow_program_uniforms
        );

    auto entity_shadow_back_program_pipeline =
        std::make_shared<shader::ShaderProgram_ElementsInstanced>(
            entity_shadow_program, chunk_render_setup_back,
            chunks_shadow_program_uniforms
        );

    auto tile_entity_render_pipeline =
        std::make_shared<shader::ShaderProgram_ElementsInstanced>(
            tile_entity_render_program, chunk_render_setup,
            chunks_render_program_uniforms
        );

    // shadow map
    auto shadow_map_avg_pipeline =
        std::make_shared<shader::ShaderProgram_Standard>(
            shadow_mid_program, sky_render_setup, shadow_texture_uniforms
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
        chunk_mesh->set_shadow_texture(scene.get_shadow_map().get_back_texture());
        chunks_render_program->data.push_back(chunk_mesh);
    }

    for (const auto& chunk_mesh : terrain_mesh) {
        chunks_front_shadow_program->data.push_back(chunk_mesh);
        chunks_back_shadow_program->data.push_back(chunk_mesh);
    }

    voxel_utility::VoxelObject default_trees_voxel(
        files::get_data_path() / "base" / "models" / "DefaultTree.qb"
    );

    auto mesh_trees = world::entity::ambient_occlusion_mesher(default_trees_voxel);

    std::vector<glm::ivec4> model_matrices;
    // generate positions of trees
    for (size_t x = 0; x < world.get_terrain_main().get_X_MAX(); x += 40)
        for (size_t y = 0; y < world.get_terrain_main().get_Y_MAX(); y += 40) {
            size_t z = world.get_terrain_main().get_Z_solid(x, y) + 1;
            if (z != 1) { // if the position of the ground is not zero
                glm::ivec4 model(x, y, z, x / 40 + y / 40);
                model_matrices.push_back(model);
            }
        }

    // static because the mesh does not have moving parts
    // this generates the buffer that holds the mesh data
    // auto gpu_trees_data =
    //    std::make_shared<gpu_data::StaticMesh>(mesh_trees, model_matrices);
    // entity_shadow_program_execute->data.push_back(gpu_trees_data);
    // entity_render_program_execute->data.push_back(gpu_trees_data);

    // attach the world objects to the render program
    world::entity::ObjectHandler& object_handler =
        world::entity::ObjectHandler::instance();
    for (auto& [id, object] : object_handler.get_objects()) {
        for (auto& mesh : object) {
            // I'm so sorry for what I have done.
            auto mesh_ptr = std::shared_ptr<world::entity::ModelController>(
                &mesh, [](world::entity::ModelController*) {}
            );

            // entity_shadow_program_execute->data.push_back(mesh_ptr);
            tile_entity_render_pipeline->data.push_back(mesh_ptr);
        }
    }

    auto shadow_calculation_data = std::make_shared<gpu_data::ShadowCalculationData>(
        scene.get_shadow_map().get_back_texture(),
        scene.get_shadow_map().get_front_texture()
    );

    shadow_map_avg_pipeline->data.push_back(shadow_calculation_data);

    // attach program to scene
    scene.shadow_attach(chunks_front_shadow_program, chunks_back_shadow_program);
    //    scene.shadow_attach(entity_shadow_program_execute,
    //    entity_shadow_program_execute);

    scene.shadow_average_shader(shadow_map_avg_pipeline);

    scene.add_mid_ground_renderer(chunks_render_program);
    //    scene.add_mid_ground_renderer(entity_render_program_execute);
    scene.add_mid_ground_renderer(tile_entity_render_pipeline);

    scene.add_background_ground_renderer(sky_renderer);
    scene.add_background_ground_renderer(star_renderer);
    scene.add_background_ground_renderer(sun_renderer);
}

} // namespace gui
