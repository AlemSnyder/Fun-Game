#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in vec2 UV; // in pixels
uniform ivec2 frame_size;
uniform sampler2D window_texture;

void
main(){

    ivec2 pixel_position = ivec2(int(UV.x), int(UV.y));

    int border_size = 10;

    if (pixel_position.y < border_size && pixel_position.x < border_size) {
        color = vec3(0.8, 0.0, 0.4);
    }
    else if (pixel_position.y < border_size && frame_size.x - pixel_position.x - 1 < border_size) {
        color = vec3(0.0, 0.5, 0.0);
    }
    else if ((frame_size.y - pixel_position.y - 1 < border_size) && pixel_position.x < border_size) {
        color = vec3(0.4, 0.8, 0.0);
    }
    else if ((frame_size.y - pixel_position.y - 1 < border_size) && (frame_size.x - pixel_position.x - 1 < border_size)) {
        color = vec3(0.5, 0.0, 0.0);
    }
    else if (pixel_position.x < border_size) {
        color = vec3(0.0, 0.0, 0.5);
    }
    else if (pixel_position.y < border_size) {
        color = vec3(0.5, 0.0, 0.5);
    }
    else if ((frame_size.x - pixel_position.x - 1) < border_size) {
        color = vec3(0.0, 0.4, 0.8);
    }
    else if ((frame_size.y - pixel_position.y - 1) < border_size) {
        color = vec3(0.5, 0.8, 0.5);
    } else {
        color = vec3(pixel_position, 0.8);
    }
}
