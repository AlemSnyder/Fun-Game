#pragma once

#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/uniform_types.hpp"
#include "frame.hpp"
#include "gui/render/structures/uniform_types.hpp"

#include <list>
#include <memory>

namespace gui {
namespace the_buttons {

class UserInterface : public virtual scene::Inputs {
 private:
    // uniform
    std::shared_ptr<render::FrameSizeUniform> frame_size_uniform_;
    std::shared_ptr<render::UIScaleUniform> ui_scale_uniform_;
    std::shared_ptr<render::TextureUniform> frame_texture_uniform_;
    std::shared_ptr<render::TextureRegionsUniform> texture_regions_;

    // widget renderer
    std::shared_ptr<shader::ShaderProgram_Windows> window_pipeline_;

    std::list<std::shared_ptr<Frame>> frames_;

    std::shared_ptr<Frame> selected_frame_; // widget?

    [[nodiscard]] std::pair<std::weak_ptr<const Frame>, std::weak_ptr<const Frame>>
    get_frame(screen_size_t mouse_position_x, screen_size_t mouse_position_y) const;

    [[nodiscard]] inline std::pair<std::weak_ptr<Frame>, std::weak_ptr<Frame>>
    get_frame(screen_size_t mouse_position_x, screen_size_t mouse_position_y) {
        auto got_frames = get_frame(mouse_position_x, mouse_position_y);
        return std::make_pair<std::weak_ptr<Frame>, std::weak_ptr<Frame>>(
            std::const_pointer_cast<Frame>(got_frames.first.lock()),
            std::const_pointer_cast<Frame>(got_frames.second.lock())
        );
    }

    void reselect_frame(GLFWwindow* window);

 public:
    UserInterface(shader::ShaderHandler& shader_handler, uint8_t ui_scale);

    void update(screen_size_t width, screen_size_t height);

    inline void
    set_ui_scale(uint8_t ui_scale) {
        ui_scale_uniform_->set_ui_scale(ui_scale);
    }

    inline void
    add(std::shared_ptr<Frame> frame) {
        auto pos = frames_.begin();
        frames_.insert(pos, frame);
    }

    void render_frame(
        const Frame& frame, screen_size_t x_frame_position,
        screen_size_t y_frame_position
    ) const;

    /**
     * @brief Handle key input including mouse keys
     *
     * @param GLFWwindow* window window event came from
     * @param int key GLFW key enum
     * @param int scancode GLFW scancode enum
     * @param int action GLFW action one of GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
     * @param int mods GLFW mods enum
     */
    virtual void handle_key_event_input(
        GLFWwindow* window, int key, int scancode, int action, int mods
    );

    /**
     * @brief Handle text input
     *
     * @param GLFWwindow* window window to listen on
     * @param unsigned int codepoint unicode 32 character.
     */

    virtual void handle_text_input_input(GLFWwindow* window, unsigned int codepoint);

    /**
     * @brief Handle mouse movement events.
     *
     * @param GLFWwindow* window window to listen on
     * @param double xpos x position in window coordinates
     * @param double ypos y position in window coordinates.
     */
    virtual void handle_mouse_event_input(GLFWwindow* window, double xpos, double ypos);

    /**
     * @brief Handle mouse enter window events
     *
     * @param GLFWwindow* window window to listen on
     * @param int entered 1 if the curser entered the window, 0 if it exited.
     */
    virtual void handle_mouse_enter_input(GLFWwindow* window, int entered);

    /**
     * @brief Handle mouse enter window events
     *
     * @param GLFWwindow* window window to listen on
     * @param int button
     * @param int action
     * @param int mods
     */
    virtual void
    handle_mouse_button_input(GLFWwindow* window, int button, int action, int mods);

    /**
     * @brief Handle mouse scroll events
     *
     * @param GLFWwindow* window window to listen on
     * @param double xoffset x offset
     * @param double yoffset y offset (usually 0)
     */
    virtual void
    handle_mouse_scroll_input(GLFWwindow* window, double xoffset, double yoffset);

    /**
     * @brief Handle joystick event
     *
     * @param int jid joystick id
     * @param int event
     */
    virtual void handle_joystick_input(int jid, int event);

    /**
     * @brief Handle file drop event
     *
     * @param GLFWwindow* window window to listen on
     * @param int count number of files passed
     * @param const char** paths file paths
     */
    virtual void
    handle_file_drop_input(GLFWwindow* window, int count, const char** paths);

    /**
     * @brief Handle all pooled inputs
     *
     * @param GLFWwindow* window window
     */
    virtual void handle_pooled_inputs(GLFWwindow* window);

    /**
     * @brief Setup so this objects handles inputs correctly
     *
     * @param GLFWwindow* window window
     */
    virtual void setup(GLFWwindow* window);

    /**
     * @brief Cleanup to original state
     *
     * @param GLFWwindow* window window
     */
    virtual void cleanup(GLFWwindow* window);
};

} // namespace the_buttons

} // namespace gui
