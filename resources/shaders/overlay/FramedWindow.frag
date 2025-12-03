#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in vec2 UV; // in pixels
uniform ivec2 frame_size;
uniform usampler2D window_texture;

// uniform 

void
main(){

    ivec2 pixel_position = ivec2(int(UV.x), int(UV.y));

    int border_size = 10;

    //color = texture(window_texture, vec2(float(pixel_position.x) / float(frame_size.x), float(pixel_position.y) / float(frame_size.y) ) ).rgb;

    uvec3 color_int = texelFetch(window_texture, ivec2(3, 3), 0).rgb;

    //uvec3 color_int = texture(window_texture, vec2(0.5, 0.5)).rgb;

    color = vec3(float(color_int.r)/255.0, 0, 0);//texelFetch(window_texture, ivec2(3, 3), 0).rgb;



/*    if (pixel_position.y < border_size && pixel_position.x < border_size) {
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
    }*/
}
