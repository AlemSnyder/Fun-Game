#pragma once

#include "../../world.hpp"
#include "../data_structures/frame_buffer_multisample.hpp"
#include "../render/quad_renderer_multisample.hpp"
#include "../render/renderer.hpp"
#include "../render/shadow_map.hpp"
#include "../render/sky.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

namespace gui {

class Scene {
 private:
    World world_;
    FrameBufferMultisample fbo;

    // TODO put in gui::render:: ...

    MainRenderer MR;
    ShadowMap SM;
    QuadRendererMultisample QRMS;

    gui::sky::SkyRenderer SR;

 public:
    Scene(
        World world_, uint32_t width, uint32_t height, uint32_t shadow_map_width_height
    );
    GLuint get_scene();
    GLuint get_depth_texture();
    uint32_t get_shadow_width();
    uint32_t get_shadow_height();
    void updata(GLFWwindow* window);
    //~Scene();
};

// Scene::~Scene() {}

} // namespace gui
