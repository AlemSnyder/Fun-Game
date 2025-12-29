#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec4 pos;
//layout(location = 1) in vec2 vertex_position_screenspace;

// this is the size of the view 
// then switch to division
uniform ivec2 frame_size;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void
main() {

    //vec2 projection_matrix = vec2(1/250, 1/250);

    gl_Position = vec4( 2 * pos.xy / vec2(frame_size) - 1, 1, 1);
    UV = pos.zw;
}

