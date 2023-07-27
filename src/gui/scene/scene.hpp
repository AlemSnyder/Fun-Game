#include "../render/data_structures/frame_buffer_multisample.hpp"
#include "../render/data_structures/shadow_map.hpp"
#include "../render/graphics_shaders/gui_render_types.hpp"
#include "../render/graphics_shaders/individual_int_renderer.hpp"
#include "../render/graphics_shaders/instanced_int_renderer.hpp"
#include "../render/graphics_shaders/quad_renderer_multisample.hpp"
#include "../render/graphics_shaders/sky.hpp"

#include <GLFW/glfw3.h>

#pragma once

namespace gui {

class Scene {
    // TODO nameing convenction
 private:
    data_structures::FrameBufferMultisample fbo;
    // need to add a data_structure::ShadowMap
    data_structures::ShadowMap shadow_map_;

    // backgroudn
    render::SkyRenderer SR;

    // "mid" ground
    std::vector<std::shared_ptr<render_to::frame_buffer>> FBR;
    std::vector<std::shared_ptr<render_to::frame_buffer_multisample>> FBMR;
    std::vector<std::shared_ptr<render_to::shadow_map>> SMR;

    // foreground, maybe

    // other
    render::QuadRendererMultisample QRMS;

 public:
    Scene(screen_size_t width, screen_size_t height, uint32_t shadow_map_width_height);
    GLuint get_scene();
    GLuint get_depth_texture();
    uint32_t get_shadow_width();
    uint32_t get_shadow_height();
    void update(GLFWwindow* window);

    // model attach

    void shadow_attach(const std::shared_ptr<render_to::shadow_map>& shadow);

    void frame_buffer_attach(const std::shared_ptr<render_to::frame_buffer>& render);

    void
    frame_buffer_multisample_attach(
        const std::shared_ptr<render_to::frame_buffer_multisample>& render
    );

    /**
     * @brief Set the light direction vector
     *
     * @param light_direction the direction of the light
     */
    void set_shadow_light_direction(glm::vec3 light_direction);

    /**
     * @brief Set the depth projection matrix
     *
     * @param depth_projection_matrix the projection matrix
     */
    void set_shadow_depth_projection_matrix(glm::mat4 depth_projection_matrix);

};

} // namespace gui
