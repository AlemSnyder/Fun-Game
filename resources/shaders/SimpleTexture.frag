#version 450 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D depth_texture;

in vec2 UV;

void
main() {
    color = texture(depth_texture, UV);
}
