#include "input.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/imgui.h>

namespace gui {

namespace scene {

GLFWwindow* InputHandler::window_ = nullptr;
std::shared_ptr<Inputs> InputHandler::forward_inputs_ = nullptr;
bool InputHandler::escape_pressed_ = false;

void
InputHandler::set_window(GLFWwindow* window) {
    assert(window_ == nullptr && "Cannot change bound window");
    window_ = window;
    glfwSetKeyCallback(window_, handle_key_event);
    glfwSetCharCallback(window_, handle_text_input);
    glfwSetCursorPosCallback(window_, handle_mouse_event);
    glfwSetMouseButtonCallback(window_, handle_mouse_button);
    glfwSetScrollCallback(window_, handle_mouse_scroll);
    glfwSetJoystickCallback(handle_joystick);
    glfwSetDropCallback(window_, handle_file_drop);
}

void
InputHandler::forward_inputs_to(std::shared_ptr<Inputs> forward_to) {
    assert(
        window_ != nullptr && "Window must be set befor you can forward things from it."
    );

    if (forward_inputs_) {
        forward_inputs_->cleanup(window_);
    }
    forward_inputs_ = forward_to;
    if (forward_to == nullptr) {
        // somehow prevent forwarding
        // this is ostensibly not a good idea
        assert(forward_to != nullptr && "Must forward inputs to valid Inputs object.");
    } else {
        forward_inputs_->setup(window_);
    }
}

bool
InputHandler::imgui_capture() {
    ImGuiIO& io = ImGui::GetIO();
    return (io.WantCaptureKeyboard || io.WantCaptureMouse || io.WantTextInput);
}

// after the sixth function I began to ask my self "Is this to many pointer
// dereferences?" I have come to the conclusion:
//        - "maybe"

// "it not as bad as it used to be"
//      - every programmer ever

void
InputHandler::handle_key_event(
    GLFWwindow* window, int key, int scancode, int action, int mods
) {
    if (imgui_capture()) {
        return;
    }
    // always forward escape to the global InputHandler
    if (key == GLFW_KEY_ESCAPE) {
        if (action == GLFW_PRESS) {
            escape_pressed_ = true;
        } else if (action == GLFW_RELEASE) {
            escape_pressed_ = false;
        }
    }
    forward_inputs_->handle_key_event_input(window, key, scancode, action, mods);
}

void
InputHandler::handle_text_input(GLFWwindow* window, unsigned int codepoint) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_text_input_input(window, codepoint);
}

void
InputHandler::handle_mouse_event(GLFWwindow* window, double xpos, double ypos) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_mouse_event_input(window, xpos, ypos);
}

void
InputHandler::handle_mouse_enter(GLFWwindow* window, int enter) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_mouse_enter_input(window, enter);
}

void
InputHandler::handle_mouse_button(
    GLFWwindow* window, int button, int action, int mods
) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_mouse_button_input(window, button, action, mods);
}

void
InputHandler::handle_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_mouse_scroll_input(window, xoffset, yoffset);
}

void
InputHandler::handle_joystick(int jid, int event) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_joystick_input(jid, event);
}

void
InputHandler::handle_file_drop(GLFWwindow* window, int count, const char** paths) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_file_drop_input(window, count, paths);
}

void
InputHandler::handle_pooled_inputs(GLFWwindow* window) {
    if (imgui_capture()) {
        return;
    }
    forward_inputs_->handle_pooled_inputs(window);
}

} // namespace scene

} // namespace gui
