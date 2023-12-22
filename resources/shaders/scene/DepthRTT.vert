#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertex_position;

// Values that stay constant for the whole mesh.
uniform mat4 depth_MVP;

void
main() {
    gl_Position = depth_MVP * vec4(vertex_position, 1);
}
