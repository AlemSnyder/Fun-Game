#version 450 core


// Ouput data
layout(location = 0) out vec3 color;

uniform usampler2D font_texture;
//uniform vec3 font_color;

in vec2 UV;

void
main() {
    //color = vec3(0.6,0.2,0.1);
    vec3 font_color = vec3(0.6,0.2,0.1);

    uint alpha = texelFetch(font_texture, ivec2(UV), 0).r;
    //uint alpha = texelFetch(font_texture, ivec2(13, 0), 0).r;

    if (alpha > 0) {
        color = font_color;
    } else {
        discard;
    }
}
