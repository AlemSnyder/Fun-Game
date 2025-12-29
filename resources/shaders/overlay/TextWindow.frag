#version 450 core


// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D texture_id;
//uniform vec3 font_color;

in vec2 UV;

void
main() {
    color = vec3(0.6,0.2,0.1);
    // vec3 font_color = vec3(0,0,0);

    // float alpha = texture(texture_id, UV).a;
    // if (alpha > 0) {
    //     color = font_color;
    // } else {
    //     discard;
    // }
}
