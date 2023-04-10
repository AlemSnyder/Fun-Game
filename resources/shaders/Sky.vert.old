#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;
uniform mat4 MVP;
uniform mat4 pixel_projection;

// Output data ; will be interpolated for each fragment.
out vec2 EyeDirectionScreenSpace;

void
main() {
    gl_Position = vec4(pos, 1);
    EyeDirectionScreenSpace = (pixel_projection * vec4(pos, 1)).xy;
}
