#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D texture_id;

in vec2 UV;

void
main() {
    color = texture(texture_id, UV).rgb;
}
