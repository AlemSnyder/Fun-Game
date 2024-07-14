#include "controls.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES // Include math constants
#include <cmath>

namespace {

glm::mat4 view_matrix;
glm::mat4 projection_matrix;

int width = 1;
int height = 1;
// Initial position : on +Z
glm::vec3 position = glm::vec3(80, 80, 80);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 1.57f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 10.0f; // 3 units / second
float mouseSpeed = 0.005f;

} // namespace

namespace controls {

glm::mat4
get_view_matrix() {
    return view_matrix;
}

glm::mat4
get_projection_matrix() {
    return projection_matrix;
}

glm::vec3
get_position_vector() {
    return position;
}

void
computeMatricesFromInputs(GLFWwindow* window) {
    glfwGetWindowSize(window, &width, &height);

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    float speed_boost = 1.0f;

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    // Compute new orientation
    horizontalAngle -= mouseSpeed * float(width / 2 - xpos);

    verticalAngle -= mouseSpeed * float(height / 2 - ypos);
    if (verticalAngle < 1.6)
        verticalAngle = 1.6; // no going up-side-down
    if (verticalAngle > 4.4)
        verticalAngle = 4.4;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        cos(verticalAngle) * cos(horizontalAngle), sin(verticalAngle)
    );

    // Right vector
    glm::vec3 screen_right = glm::vec3(
        sin(horizontalAngle - M_PI / 2.0f), cos(horizontalAngle - M_PI / 2.0f), 0
    );

    // Up vector
    glm::vec3 screen_up = glm::cross(screen_right, direction);

    // speed boost
    speed_boost = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 10.0f : 1.0f;
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed * speed_boost;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed * speed_boost;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += screen_right * deltaTime * speed * speed_boost;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= screen_right * deltaTime * speed * speed_boost;
    }
    // Strafe up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += screen_up * deltaTime * speed * speed_boost;
    }
    // Strafe down
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        position -= screen_up * deltaTime * speed * speed_boost;
    }

    float FoV = initialFoV;
    // set up call back to allow for scrolling to see things

    // Projection matrix : 45° Field of View, 4:3 ratio,
    projection_matrix = glm::perspective(
        glm::radians(FoV),
        // ratio defined by screen width and height
        static_cast<float>(width) / static_cast<float>(height),
        0.1f, // display range min
        1000.0f
    ); // display range max
    // Camera matrix
    view_matrix = glm::lookAt(
        position,             // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        screen_up             // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

} // namespace controls
