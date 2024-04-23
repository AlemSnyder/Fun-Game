#version 450 core

// Ouput data
layout(location = 0) out float color;

in vec2 texture_position;

uniform sampler2D shadow_texture_back;
uniform sampler2D shadow_texture_front;

void
main() {

    float back = texture(shadow_texture_back, ( texture_position + vec2(1,1) ) / 2).x;
    float front = texture(shadow_texture_front, ( texture_position + vec2(1,1) ) / 2).x;

    if (front > back) {
        color = back/2;
    }
    else {
        color = back/2 + front/2;
    }
}
