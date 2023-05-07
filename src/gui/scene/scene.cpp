#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../data_structures/static_mesh.hpp"
#include "../data_structures/terrain_mesh.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

gui::Scene::Scene(
    World& world, uint32_t window_width, uint32_t window_height,
    uint32_t shadow_map_width_height
) :
    world_(world),
    fbo(window_width, window_height, 4),
    SM(shadow_map_width_height, shadow_map_width_height),
    treesMesh(
        entity::generate_mesh(voxel_utility::VoxelObject(
            files::get_data_path() / "models" / "DefaultTree.qb"
        )),
        get_model_matrices_temp(world_)
    ) {
    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    world_.update_all_chunk_mesh();
    LOG_INFO(logging::opengl_logger, "End of world::get_mesh_greedy");
    auto chunk_meshes = world_.get_chunks_mesh();
    LOG_INFO(logging::opengl_logger, "Chunk meshes sent to graphics buffer.");

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f)) // direction
        * 128.0f;                                       // length

    glm::mat4 depth_projection_matrix =
        glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    // Renders the Shadow depth map
    SM.set_light_direction(light_direction);
    SM.set_depth_projection_matrix(depth_projection_matrix);

    for (std::shared_ptr<terrain::TerrainMesh> m : chunk_meshes) {
        SM.add_mesh(m);
        MR.add_mesh(m);
    }
    SM.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));

    // renders the world scene
    MR.set_light_direction(light_direction);
    MR.set_depth_projection_matrix(depth_projection_matrix);

    MR.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));
    MR.set_depth_texture(SM.get_depth_texture());

    gui::render::QuadRendererMultisample QRMS;

    gui::render::SkyRenderer SR;

    LOG_INFO(logging::opengl_logger, "Scene initialized");
}

void
gui::Scene::update(GLFWwindow* window) {
    glBindFramebuffer(GL_FRAMEBUFFER, SM.get_frame_buffer());
    glClear(GL_DEPTH_BUFFER_BIT);
    SM.render_shadow_depth_buffer();
    // clear the frame buffer each frame
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.get_frame_buffer_name());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // render the sky to the frame buffer
    SR.render(window, fbo.get_frame_buffer_name());
    // render the sene to the frame buffer
    MR.render(window, fbo.get_frame_buffer_name());

    QRMS.render(
        fbo.get_width(), fbo.get_height(), fbo.get_num_samples(),
        fbo.get_texture_name(), fbo.get_frame_buffer_single()
    );
}

GLuint
gui::Scene::get_scene() {
    return fbo.get_single_sample_texture();
}

GLuint
gui::Scene::get_depth_texture() {
    return SM.get_depth_texture();
}

uint32_t
gui::Scene::get_shadow_width() {
    return SM.get_shadow_width();
}

uint32_t
gui::Scene::get_shadow_height() {
    return SM.get_shadow_height();
}

std::vector<glm::ivec3>
gui::Scene::get_model_matrices_temp(World& world) {
    std::vector<glm::ivec3> model_matrices;
    // generate positions of trees
    for (unsigned int x = 0; x < world.get_terrain_main().get_X_MAX(); x += 40)
        for (unsigned int y = 0; y < world.get_terrain_main().get_Y_MAX(); y += 40) {
            unsigned int z = world.get_terrain_main().get_Z_solid(x, y) + 1;
            if (z != 1) { // if the position of the ground is not zero
                glm::ivec3 model(x, y, z);
                model_matrices.push_back(model);
            }
        }
    return model_matrices;
}
