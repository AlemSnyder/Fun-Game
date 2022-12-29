#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 inverseViewProgection;

// Output data ; will be interpolated for each fragment.
out vec3 EyeDirectionModelSpace;

void
main() {
    gl_Position = vec4(vertexPosition_modelspace, 1);
    EyeDirectionModelSpace = (vertexPosition_modelspace * inverseViewProgection).xy;
}
