#include "controls.hpp"

#include "logging.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#define _USE_MATH_DEFINES // Include math constants
#include <cmath>

namespace gui {

namespace scene {
void
Controls::handle_pooled_inputs(GLFWwindow* window) {
    glfwGetWindowSize(window, &width_, &height_);

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - glfw_previous_time_);
    float speed_boost = 1.0f;

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width_ / 2, height_ / 2);

    // Compute new orientation
    horizontal_angle_ -= mouse_speed_ * float(width_ / 2 - xpos) * field_of_view_ / 45;

    vertical_angle_ -= mouse_speed_ * float(height_ / 2 - ypos) * field_of_view_ / 45;
    if (vertical_angle_ < 1.6)
        vertical_angle_ = 1.6; // no going up-side-down
    if (vertical_angle_ > 4.4)
        vertical_angle_ = 4.4;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(vertical_angle_) * sin(horizontal_angle_),
        cos(vertical_angle_) * cos(horizontal_angle_), sin(vertical_angle_)
    );

    // Right vector
    glm::vec3 screen_right = glm::vec3(
        sin(horizontal_angle_ - M_PI / 2.0f), cos(horizontal_angle_ - M_PI / 2.0f), 0
    );

    // Up vector
    glm::vec3 screen_up = glm::cross(screen_right, direction);

    // speed boost
    speed_boost = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 10.0f : 1.0f;
    // Move forward
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::FORWARD])) == GLFW_PRESS) {
        position_ += direction * deltaTime * speed_ * speed_boost;
    }
    // Move backward
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::BACKWARD]))
        == GLFW_PRESS) {
        position_ -= direction * deltaTime * speed_ * speed_boost;
    }
    // Strafe right
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::RIGHT])) == GLFW_PRESS) {
        position_ += screen_right * deltaTime * speed_ * speed_boost;
    }
    // Strafe left
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::LEFT])) == GLFW_PRESS) {
        position_ -= screen_right * deltaTime * speed_ * speed_boost;
    }
    // Strafe up
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::UP])) == GLFW_PRESS) {
        position_ += screen_up * deltaTime * speed_ * speed_boost;
    }
    // Strafe down
    if (glfwGetKey(window, static_cast<int>(key_map_[Action::DOWN])) == GLFW_PRESS) {
        position_ -= screen_up * deltaTime * speed_ * speed_boost;
    }

    // TODO
    // glfwSetWindowSizeCallback(window, window_size_callback);
    // Projection matrix : 45° Field of View, 4:3 ratio,
    projection_matrix_ = glm::perspective(
        glm::radians(field_of_view_),
        // ratio defined by screen width and height
        static_cast<float>(width_) / static_cast<float>(height_),
        0.1f, // display range min
        1000.0f
    ); // display range max
    // Camera matrix
    view_matrix_ = glm::lookAt(
        position_, // Camera is here
        position_
            + direction, // and looks here : at the same position_, plus "direction"
        screen_up        // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    glfw_previous_time_ = currentTime;
}

void
Controls::handle_mouse_scroll_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset
) {
    //LOG_DEBUG(logging::opengl_logger, "Mouse scroll amount: {}", yoffset);
    // set up call back to allow for scrolling to see things
    field_of_view_ *= (10 + yoffset) / 10;
    if (field_of_view_ < 0.1) {
        field_of_view_ = 0.1;
    } else if (field_of_view_ > 60) {
        field_of_view_ = 60;
    } 
}

void
Controls::setup([[maybe_unused]] GLFWwindow* window) {
    // want to call this
    // at least reset periodically
    glfw_previous_time_ = glfwGetTime();
}

void
Controls::cleanup([[maybe_unused]] GLFWwindow* window) {}

} // namespace scene

} // namespace gui
