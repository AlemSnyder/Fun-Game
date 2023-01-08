#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

void
main() {
    gl_Position = depthMVP * vec4(vertexPosition_modelspace, 1);
}
