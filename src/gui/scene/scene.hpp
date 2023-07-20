#pragma once

// #include "../../world.hpp"

#include "../render/data_structures/frame_buffer_multisample.hpp"
#include "../render/gui_models/gui_render_types.hpp"
#include "../render/gui_models/individual_int_renderer.hpp"
#include "../render/gui_models/instanced_int_renderer.hpp"
#include "../render/gui_models/quad_renderer_multisample.hpp"
#include "../render/gui_models/sky.hpp"
#include "../render/data_structures/shadow_map.hpp"

// # include "../render/gui_models/**"

// #include "../data_structures/frame_buffer_multisample.hpp"
// #include "data_structures/static_mesh.hpp"
// #include "data_structures/terrain_mesh.hpp"
// #include "render/quad_renderer_multisample.hpp"
// #include "../render/renderer.hpp"
// #include "../render/shadow_map.hpp"
// #include "../render/sky.hpp"

#include <GLFW/glfw3.h>

namespace gui {

class Scene {
    // TODO nameing convenction
 private:
    data_structures::FrameBufferMultisample fbo;
    // need to add a data_structure::ShadowMap
    data_structures::ShadowMap shadow_map_;

    // models::IndividualIntRenderer IdIR;
    // models::InstancedIntRenderer InIR;

    // backgroudn
    render::SkyRenderer SR;

    // "mid" ground
    std::vector<std::shared_ptr<render_to::frame_buffer>> FBR;
    std::vector<std::shared_ptr<render_to::frame_buffer_multisample>> FBMR;
    std::vector<std::shared_ptr<render_to::shadow_map>> SMR;

    // foreground, maybe

    // render::MainRenderer MR;
    // render::ShadowMap SM;
    render::QuadRendererMultisample QRMS;

    // TODO these should be saved in world
    // data_structures::StaticMesh treesMesh;

 public:
    Scene(uint32_t width, uint32_t height, uint32_t shadow_map_width_height);
    GLuint get_scene();
    GLuint get_depth_texture();
    uint32_t get_shadow_width();
    uint32_t get_shadow_height();
    void update(GLFWwindow* window);

    // model attatch

};

} // namespace gui
