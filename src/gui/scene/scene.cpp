#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../handler.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define SAMPLES 4

gui::Scene::Scene(uint32_t window_width, uint32_t window_height,
    uint32_t shadow_map_width_height
) :
    fbo(window_width, window_height, SAMPLES), shadow_map_(shadow_map_width_height,shadow_map_width_height), SR(), QRMS()
    //SM(shadow_map_width_height, shadow_map_width_height),
    {
        
}

// add model attatch functions.

void
gui::Scene::update(GLFWwindow* window) {

    // clear the frame buffer each frame
    // fbo.clear()
    gui::FrameBufferHandler::bind_fbo(fbo.get_frame_buffer_name());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SR.render(window, fbo.get_depth_buffer_name());

    for (const auto& shadow : SMR){
        shadow->render_shadow_map(shadow_map_.get_shadow_width(), shadow_map_.get_shadow_height(), shadow_map_.get_frame_buffer());
    }

    for (const auto& render : FBR){
        render->render_frame_buffer(window, fbo.get_depth_buffer_name());
    }

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
    return shadow_map_.get_depth_texture();
}

uint32_t
gui::Scene::get_shadow_width() {
    return shadow_map_.get_shadow_width();
}

uint32_t
gui::Scene::get_shadow_height() {
    return shadow_map_.get_shadow_height();
}
