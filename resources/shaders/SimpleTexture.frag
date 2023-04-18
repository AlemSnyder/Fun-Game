#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D depth_texture;

in vec2 UV;

void
main() {
    color = texture(depth_texture, UV).rgb;
    //color = vec3(0,1,0);
}
