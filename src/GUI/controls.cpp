// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;



void computeMatricesFromInputs(){

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
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle -= mouseSpeed * float(1024/2 - xpos );
	
	verticalAngle   -= mouseSpeed * float( 768/2 - ypos );
	if (verticalAngle < 1.6) verticalAngle = 1.6; // no going up-side down
	if (verticalAngle > 4.4) verticalAngle = 4.4;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		cos(verticalAngle) * cos(horizontalAngle),
		sin(verticalAngle)
	);
	
	// Right vector
	glm::vec3 screen_right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		cos(horizontalAngle - 3.14f/2.0f),
		0
	);
	
	// Up vector
	glm::vec3 screen_up = glm::cross( screen_right, direction );


	// speed boost
	if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		speed_boost = 10.0f;
	}
	else {speed_boost = 1.0f;}
	// Move forward
	if (glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS){
		position += direction * deltaTime * speed * speed_boost;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_F ) == GLFW_PRESS){
		position -= direction * deltaTime * speed * speed_boost;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += screen_right * deltaTime * speed * speed_boost;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= screen_right * deltaTime * speed * speed_boost;
	}
	// Strafe up
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += screen_up * deltaTime * speed * speed_boost;
	}
	// Strafe down
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= screen_up * deltaTime * speed * speed_boost;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								screen_up           // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
