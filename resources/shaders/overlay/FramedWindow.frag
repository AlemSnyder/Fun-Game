#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in vec2 UV; // in pixels
uniform ivec2 frame_size; // error
uniform usampler2D window_texture;
//uniform int texture_locations[36]; // error
uniform int ui_scale;

// uniform 

void
main(){

    int border_size = 5;

    ivec2 position_1 = ivec2(0,0);
    ivec2 position_2 = ivec2(5,0);
    ivec2 position_3 = ivec2(6,0);
    ivec2 position_4 = ivec2(0,5);
    ivec2 position_5 = ivec2(5,5);
    ivec2 position_6 = ivec2(6,5);
    ivec2 position_7 = ivec2(0,6);
    ivec2 position_8 = ivec2(5,6);
    ivec2 position_9 = ivec2(6,6);

    int width_2 = 1;
    int height_4 = 1;
    int width_5 = 1;
    int height_5 = 1;
    int height_6 = 1;
    int width_8 = 1;

    ivec2 pixel_position = ivec2(int(UV.x), int(UV.y));
    ivec2 ui_position = pixel_position / ui_scale;
    ivec2 frame_size_px = frame_size / ui_scale;

    ivec2 texture_offset;

//    ivec2 texture_position = ivec2(texture_locations[3], texture_locations[2]);

    if (ui_position.y < border_size && ui_position.x < border_size) { // 1

        ivec2 local_position = ivec2(ui_position.x, ui_position.y);

        texture_offset = local_position + position_1;

    }
    else if (ui_position.y < border_size && frame_size_px.x - ui_position.x - 1 < border_size) { // 3

        ivec2 local_position = ivec2(ui_position.x - frame_size_px.x + border_size, ui_position.y);

        texture_offset = local_position + position_3;
    }
    else if ((frame_size_px.y - ui_position.y - 1 < border_size) && ui_position.x < border_size) { // 7
        ivec2 local_position = ivec2(ui_position.x, ui_position.y - frame_size_px.y + border_size);

        texture_offset = local_position + position_7;
    }
    else if ((frame_size_px.y - ui_position.y - 1 < border_size) && (frame_size_px.x - ui_position.x - 1 < border_size)) { // 9
        ivec2 local_position = ivec2(ui_position.x - frame_size_px.x + border_size, ui_position.y - frame_size_px.y + border_size);

        texture_offset = local_position + position_9;
    }
    else if (ui_position.x < border_size) { // 4
        ivec2 local_position = ivec2(ui_position.x, ui_position.y - border_size);
        local_position.y = local_position.y % height_4;

        texture_offset = local_position + position_4;
    }
    else if (ui_position.y < border_size) { // 2
        ivec2 local_position = ivec2(ui_position.x, ui_position.y);
        local_position.x = local_position.x % width_2;

        texture_offset = local_position + position_2;
    }
    else if ((frame_size_px.x - ui_position.x) <= border_size) { // 6

        ivec2 local_position = ivec2(border_size - frame_size_px.x + ui_position.x, ui_position.y - border_size);
        local_position.y = local_position.y % height_6;

        texture_offset = local_position + position_6;
    }
    else if ((frame_size_px.y - ui_position.y) <= border_size) { // 8 idk

        ivec2 local_position = ivec2(ui_position.x - border_size, frame_size_px.y - ui_position.y-1);
        local_position.x = local_position.x % width_8;

        texture_offset = local_position + position_2;
    } else { // 5
        texture_offset = position_5;
    }

    uvec4 color_int = texelFetch(window_texture, texture_offset, 0);
    if (color_int.a == 0) {
        discard;
    }

    color = vec3(color_int.rgb)/255.0;
//    uvec4 color_int = texelFetch(window_texture, texture_position, 0);

//    if (color_int.a == 0) {
//        discard;
//    }

//    vec3 color_float = vec3(color_int.rgba);

    //uvec3 color_int = texture(window_texture, vec2(0.5, 0.5)).rgb;

//    color = color_float / 255;//texelFetch(window_texture, ivec2(3, 3), 0).rgb;

}
