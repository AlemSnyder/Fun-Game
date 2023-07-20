#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../handler.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

gui::Scene::Scene(uint32_t window_width, uint32_t window_height,
    uint32_t shadow_map_width_height
) :
    fbo(window_width, window_height, 4)
    //SM(shadow_map_width_height, shadow_map_width_height),
    {
        

    // all this needs to be removed

        
    treesMesh(
        entity::generate_mesh(voxel_utility::VoxelObject(
            files::get_data_path() / "models" / "DefaultTree.qb"
        )),
        get_model_matrices_temp(world_)
    )
    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    world_.update_all_chunks_mesh();
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

    for (std::shared_ptr<data_structures::TerrainMesh> m : chunk_meshes) {
        SM.add_mesh(m);
        MR.add_mesh(m);
    }
    SM.add_mesh(std::make_shared<data_structures::StaticMesh>(treesMesh));

    // renders the world scene
    MR.set_light_direction(light_direction);
    MR.set_depth_projection_matrix(depth_projection_matrix);

    MR.add_mesh(std::make_shared<data_structures::StaticMesh>(treesMesh));
    MR.set_depth_texture(SM.get_depth_texture());

    gui::render::QuadRendererMultisample QRMS;

    gui::render::SkyRenderer SR;

    LOG_INFO(logging::opengl_logger, "Scene initialized");
}

// add model attatch functions.

void
gui::Scene::update(GLFWwindow* window) {
    SM.render_shadow_depth_buffer();
    // clear the frame buffer each frame
    gui::FrameBufferHandler::bind_fbo(fbo.get_frame_buffer_name());
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
