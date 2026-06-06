#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 vertex_position_screenspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void
main() {
    gl_Position = vec4(pos, 1);
    UV = vertex_position_screenspace;
}

