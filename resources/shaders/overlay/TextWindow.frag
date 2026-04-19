#version 450 core


// Ouput data
layout(location = 0) out vec3 color;

uniform usampler2D font_texture;
// right now only using the rgb components
// may in the future use the alpha component
uniform vec4 font_color;

in vec2 UV;

void
main() {
    uint alpha = texelFetch(font_texture, ivec2(UV), 0).r;

    if (alpha > 0) {
        color = font_color.rgb;
    } else {
        discard;
    }
}
