#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

out vec2 texture_position;

void
main() {

    texture_position = pos.xy;

    gl_Position = vec4(pos, 1);
}