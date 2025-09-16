#include "scene_setup.hpp"

#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/screen_data.hpp"
#include "../render/structures/star_data.hpp"
#include "../render/structures/static_mesh.hpp"
#include "../render/structures/uniform_types.hpp"
#include "gui/render/structures/floating_instanced_i_mesh.hpp"
#include "manifest/object_handler.hpp"
#include "render_programs.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <functional>
#include <memory>

namespace gui {

// Setup a scene object.
// This is a helper function such that exists so that both programs with, and
// without imgui load the same scene.
// Its not in scene because I want to keep this out of the game engine part of
// the code base.
void
setup(
    Scene& scene, shader::ShaderHandler& shader_handler, world::World& world,
    world::Climate& climate
) {
    // assign map from all color ids to each color
    // to package as a texture
    terrain::TerrainColorMapping::assign_color_mapping(world.get_materials());
    // send color texture to gpu
    // terrain::TerrainColorMapping::assign_color_texture();
    // parallel program to generate a mesh for each chunk
    world.update_all_chunks_mesh();

    auto terrain_mesh = world.get_terrain_mesh();

    auto star_shape = climate.get_sun_data();

    auto star_data = climate.get_stars_data();

    // clang-format off

    ///////////////////////////////////////////////////////////////////////////
    //  Load all programs from files.                                        //
    ///////////////////////////////////////////////////////////////////////////

    // Renders the chunks
    shader::Program& chunks_render_program = shader_handler.load_program(
        "Render Chunks",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );
    // Renders the chunk' shadows
    shader::Program& chunks_shadow_program = shader_handler.load_program(
        "Shadow Chunks",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );

    // Renders the tile entities (flowers)
    shader::Program& tile_entity_render_program = shader_handler.load_program(
        "Tile Entity Render",
        files::get_resources_path() / "shaders" / "scene" / "TileEntity.vert",
        files::get_resources_path() / "shaders" / "scene" / "TileEntity.frag"
    );

    shader::Program& tile_entity_shadow_program = shader_handler.load_program(
        "Tile Entity Render",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTTEntity.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );

    // Renders the objects (trees)
    shader::Program& object_render_program = shader_handler.load_program(
        "Instanced Render",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMappingInstanced.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );

    // Renders the shadows of the objects
    shader::Program& object_shadow_program = shader_handler.load_program(
        "Instanced Shadow Map",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTTInstanced.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );

    // Renders the sky background
    shader::Program& sky_program = shader_handler.load_program(
        "Sky",
        files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    // Renders the  stars
    shader::Program& stars_program = shader_handler.load_program(
        "Stars",
        files::get_resources_path() / "shaders" / "background" / "Stars.vert",
        files::get_resources_path() / "shaders" / "background" / "Stars.frag"
    );

    // Renders the sun
    shader::Program& sun_program = shader_handler.load_program(
        "Sun",
        files::get_resources_path() / "shaders" / "background" / "Sun.vert",
        files::get_resources_path() / "shaders" / "background" / "Sun.frag"
    );

    // Renders the entities things not fixed to tile positions
    shader::Program& entity_render_program = shader_handler.load_program(
        "Entity",
        files::get_resources_path() / "shaders" / "scene" / "Entity.vert",
        files::get_resources_path() / "shaders" / "Red.frag"
    );

    ///////////////////////////////////////////////////////////////////////////
    // Initialize the uniforms                                               //
    ///////////////////////////////////////////////////////////////////////////

    auto matrix_view_projection_uniform =
        std::make_shared<render::MatrixViewProjection>(scene.get_inputs());

    auto view_matrix_uniform =
        std::make_shared<render::ViewMatrix>(scene.get_inputs());

    auto light_depth_projection_uniform =
        std::make_shared<render::LightDepthProjection>(&scene.get_shadow_map());

    auto light_depth_texture_projection_uniform =
        std::make_shared<render::LightDepthTextureProjection>(&scene.get_shadow_map());

    auto shadow_texture_uniform =
        std::make_shared<render::TextureUniform>(gpu_data::GPUDataType::SAMPLER_2D_SHADOW, 1);

    auto material_color_texture_uniform =
        std::make_shared<render::TextureUniform>(gpu_data::GPUDataType::SAMPLER_1D, 0);

    auto entity_color_texture_uniform =
        std::make_shared<render::TextureUniform>(gpu_data::GPUDataType::SAMPLER_2D, 0);

    auto spectral_light_color_uniform =
        std::make_shared<render::SpectralLight>(scene.get_lighting_environment());

    auto diffuse_light_color_uniform =
        std::make_shared<render::DiffuseLight>(scene.get_lighting_environment());

    auto light_direction_uniform =
        std::make_shared<render::LightDirection>(scene.get_lighting_environment());

    auto matrix_view_inverse_projection =
        std::make_shared<render::MatrixViewInverseProjection>(scene.get_inputs());

    auto pixel_projection =
        std::make_shared<render::PixelProjection>();

    auto star_rotation_uniform =
        std::make_shared<render::StarRotationUniform>(scene.get_lighting_environment());

    ///////////////////////////////////////////////////////////////////////////
    // Link all uniforms to their respective programs                        //
    ///////////////////////////////////////////////////////////////////////////

    chunks_render_program.set_uniform(matrix_view_projection_uniform, "MVP");
    chunks_render_program.set_uniform(view_matrix_uniform, "view_matrix");
    chunks_render_program.set_uniform(light_direction_uniform, "light_direction");
    chunks_render_program.set_uniform(light_depth_texture_projection_uniform, "depth_texture_projection");
    chunks_render_program.set_uniform(shadow_texture_uniform, "shadow_texture");
    chunks_render_program.set_uniform(material_color_texture_uniform, "material_color_texture");
    chunks_render_program.set_uniform(spectral_light_color_uniform, "direct_light_color");
    chunks_render_program.set_uniform(diffuse_light_color_uniform, "diffuse_light_color");

    chunks_shadow_program.set_uniform(light_depth_projection_uniform, "depth_MVP");

    tile_entity_render_program.set_uniform(matrix_view_projection_uniform, "MVP");
    tile_entity_render_program.set_uniform(view_matrix_uniform, "view_matrix");
    tile_entity_render_program.set_uniform(light_direction_uniform, "light_direction");
    tile_entity_render_program.set_uniform(light_depth_texture_projection_uniform, "depth_texture_projection");
    tile_entity_render_program.set_uniform(shadow_texture_uniform, "shadow_texture");
    tile_entity_render_program.set_uniform(entity_color_texture_uniform, "material_color_texture");
    tile_entity_render_program.set_uniform(spectral_light_color_uniform, "direct_light_color");
    tile_entity_render_program.set_uniform(diffuse_light_color_uniform, "diffuse_light_color");

    object_render_program.set_uniform(matrix_view_projection_uniform, "MVP");
    object_render_program.set_uniform(view_matrix_uniform, "view_matrix");
    object_render_program.set_uniform(light_direction_uniform, "light_direction");
    object_render_program.set_uniform(light_depth_texture_projection_uniform, "depth_texture_projection");
    object_render_program.set_uniform(shadow_texture_uniform, "shadow_texture");
    object_render_program.set_uniform(material_color_texture_uniform, "material_color_texture");
    object_render_program.set_uniform(spectral_light_color_uniform, "direct_light_color");
    object_render_program.set_uniform(diffuse_light_color_uniform, "diffuse_light_color");

    object_shadow_program.set_uniform(light_depth_projection_uniform, "depth_MVP");

    entity_render_program.set_uniform(matrix_view_projection_uniform, "MVP");
    // these will be added back when a more complete entity graphics program is written
    // entity_render_program.set_uniform(view_matrix_uniform, "view_matrix");
    // entity_render_program.set_uniform(light_direction_uniform, "light_direction");
    // entity_render_program.set_uniform(light_depth_texture_projection_uniform, "depth_texture_projection");
    // entity_render_program.set_uniform(shadow_texture_uniform, "shadow_texture");
    // entity_render_program.set_uniform(material_color_texture_uniform, "material_color_texture");
    // entity_render_program.set_uniform(spectral_light_color_uniform, "direct_light_color");
    // entity_render_program.set_uniform(diffuse_light_color_uniform, "diffuse_light_color");


    sky_program.set_uniform(matrix_view_inverse_projection, "MVIP");
    sky_program.set_uniform(light_direction_uniform, "light_direction");
    sky_program.set_uniform(spectral_light_color_uniform, "direct_light_color");

    stars_program.set_uniform(matrix_view_projection_uniform, "MVP");
    stars_program.set_uniform(pixel_projection, "pixel_projection");
    stars_program.set_uniform(star_rotation_uniform, "star_rotation");
    stars_program.set_uniform(light_direction_uniform, "light_direction");

    sun_program.set_uniform(matrix_view_projection_uniform, "MVP");
    sun_program.set_uniform(pixel_projection, "pixel_projection");
    sun_program.set_uniform(light_direction_uniform, "light_direction");
    sun_program.set_uniform(spectral_light_color_uniform, "direct_light_color");

    entity_render_program.set_uniform(matrix_view_projection_uniform, "MVP");

    ///////////////////////////////////////////////////////////////////////////
    // program setup functions                                               //
    ///////////////////////////////////////////////////////////////////////////
    // Anything the renders something in the world
    std::function<void()> chunk_render_setup = []() {
        // Cull back-facing triangles -> draw only front-facing triangles
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        // Enable the depth test, and enable drawing to the depth texture
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    };

    // Overwrites anything that was there before
    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    // the programs
    // chunks
    auto chunks_render_pipeline = std::make_shared<shader::ShaderProgram_MultiElements>(
        chunks_render_program, chunk_render_setup
    );

    // chunks shadow
    auto chunks_shadow_pipeline = std::make_shared<shader::ShaderProgram_MultiElements>(
        chunks_shadow_program, chunk_render_setup
    );

    auto object_render_pipeline = std::make_shared<shader::ShaderProgram_ElementsInstanced>(
        object_render_program, chunk_render_setup
    );

    auto object_shadow_pipeline = std::make_shared<shader::ShaderProgram_ElementsInstanced>(
        object_shadow_program, chunk_render_setup
    );

    auto tile_entity_shadow_pipeline = std::make_shared<shader::ShaderProgram_ElementsInstanced>(
        tile_entity_shadow_program, chunk_render_setup
    );

    auto tile_entity_render_pipeline = std::make_shared<shader::ShaderProgram_ElementsInstanced>(
        tile_entity_render_program, chunk_render_setup
    );

    auto entity_render_pipeline = std::make_shared<shader::ShaderProgram_ElementsInstanced>(
        entity_render_program, chunk_render_setup
    );

    // sky
    auto sky_pipeline = std::make_shared<shader::ShaderProgram_Standard>(
        sky_program, sky_render_setup
    );

    // star
    auto star_pipeline = std::make_shared<shader::ShaderProgram_Instanced>(
        stars_program, sky_render_setup
    );

    // sun
    auto sun_pipeline = std::make_shared<shader::ShaderProgram_Standard>(
        sun_program, sky_render_setup
    );
    // clang-format on

    // assign data
    sky_pipeline->data.push_back(scene.get_screen_data());
    star_pipeline->data.push_back(star_data);
    sun_pipeline->data.push_back(star_shape);

    terrain_mesh->set_shadow_texture(scene.get_shadow_map().get_depth_buffer()->value()
    );
    chunks_render_pipeline->data.push_back(terrain_mesh.get());
    chunks_shadow_pipeline->data.push_back(terrain_mesh.get());

    // TODO send terrain_mesh to a render program

    auto z = world.get_terrain_main().get_Z_solid(5, 5, 50);

    world.spawn_entity("base/Test_Entity", {5, 5, z + 1});

    render_programs_t object_render_programs{
        .entity_render_program = entity_render_pipeline,
        .tile_object_render_program = tile_entity_render_pipeline};

    // attach the world objects to the render program
    for (auto& [id, object] : *world.get_object_handler()) {
        if (!object) {
            continue;
        }

        object->init_render(object_render_programs);
    }

    // attach program to scene
    scene.shadow_attach(chunks_shadow_pipeline);
    scene.shadow_attach(object_shadow_pipeline);
    scene.shadow_attach(tile_entity_shadow_pipeline);

    scene.add_mid_ground_renderer(chunks_render_pipeline);
    scene.add_mid_ground_renderer(object_render_pipeline);
    scene.add_mid_ground_renderer(tile_entity_render_pipeline);
    scene.add_mid_ground_renderer(entity_render_pipeline);

    scene.add_background_ground_renderer(sky_pipeline);
    scene.add_background_ground_renderer(star_pipeline);
    scene.add_background_ground_renderer(sun_pipeline);
}

} // namespace gui
