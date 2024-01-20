#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

uniform mat4 MVIP;
uniform mat4 pixel_projection;
uniform vec3 light_direction; // same as sun position

// Output data ; will be interpolated for each fragment.
out vec2 EyeDirectionScreenSpace;

out flat vec3 sunlight_direction_unit;

void
main() {
    sunlight_direction_unit = light_direction/length(light_direction);
    EyeDirectionScreenSpace = pos.xy;
    gl_Position = vec4(pos, 1);
}
