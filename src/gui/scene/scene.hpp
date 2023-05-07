#pragma once

#include "../../world.hpp"
#include "../data_structures/frame_buffer_multisample.hpp"
#include "../data_structures/static_mesh.hpp"
#include "../data_structures/terrain_mesh.hpp"
#include "../render/quad_renderer_multisample.hpp"
#include "../render/renderer.hpp"
#include "../render/shadow_map.hpp"
#include "../render/sky.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

namespace gui {

class Scene {
 private:
    World& world_;
    FrameBufferMultisample fbo;

    // TODO put renders in gui::render:: namespace
    MainRenderer MR;
    ShadowMap SM;
    QuadRendererMultisample QRMS;

    gui::sky::SkyRenderer SR;

    // TODO these should be saved in world
    terrain::StaticMesh treesMesh;

 public:
    Scene(
        World& world_, uint32_t width, uint32_t height, uint32_t shadow_map_width_height
    );
    GLuint get_scene();
    GLuint get_depth_texture();
    uint32_t get_shadow_width();
    uint32_t get_shadow_height();
    void update(GLFWwindow* window);
    //~Scene();

    std::vector<glm::ivec3> get_model_matrices_temp(World& world);
};

// Scene::~Scene() {}

} // namespace gui
