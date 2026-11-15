#version 450 core

// Screen and texture position of letter
layout(location = 0) in ivec4 pos;

// this is the size of the view 
// then switch to division
uniform ivec2 frame_size;
uniform int ui_scale;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void
main() {

    gl_Position = vec4(ui_scale * (vec2(pos.xy) / vec2(frame_size)) - 1, 1, 1);
    UV = vec2(pos.w, pos.z);
    //UV = vec2((pos.z - 304) * 20, pos.w * 2);
}

