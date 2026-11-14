#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in vec2 UV; // in pixels
uniform ivec2 frame_size;
uniform usampler2D window_texture;
uniform int ui_scale;

// uniform 
uniform ivec4 border_size;
uniform ivec4 side_lengths;
uniform ivec2 inner_pattern_size;
uniform ivec2 positions[9];

void
main(){
    int width_2 = side_lengths[0];
    int height_4 = side_lengths[1];
    int width_5 = inner_pattern_size[0];
    int height_5 = inner_pattern_size[1];
    int height_6 = side_lengths[2];
    int width_8 = side_lengths[3];

    ivec2 pixel_position = ivec2(int(UV.x), int(UV.y));
    ivec2 ui_position = pixel_position / ui_scale;
    ivec2 frame_size_px = frame_size / ui_scale;

    ivec2 texture_offset;

// The window is arranged like this
//  0 | 1 | 2
// ---+---+---
//  3 | 4 | 5
// ---+---+---
//  6 | 7 | 8
//
    if (ui_position.y < border_size[1] && ui_position.x < border_size[0]) { // 0
        ivec2 local_position = ivec2(ui_position.x, ui_position.y);
        texture_offset = local_position + positions[0];
    }
    else if (ui_position.y < border_size[1] && frame_size_px.x - ui_position.x - 1 < border_size[2]) { // 2
        ivec2 local_position = ivec2(ui_position.x - frame_size_px.x + border_size[2], ui_position.y);
        texture_offset = local_position + positions[2];
    }
    else if ((frame_size_px.y - ui_position.y - 1 < border_size[3]) && ui_position.x < border_size[0]) { // 6
        ivec2 local_position = ivec2(ui_position.x, ui_position.y - frame_size_px.y + border_size[3]);
        texture_offset = local_position + positions[6];
    }
    else if ((frame_size_px.y - ui_position.y - 1 < border_size[3]) && (frame_size_px.x - ui_position.x - 1 < border_size[2])) { // 8
        ivec2 local_position = ivec2(ui_position.x - frame_size_px.x + border_size[2], ui_position.y - frame_size_px.y + border_size[3]);
        texture_offset = local_position + positions[8];
    }
    else if (ui_position.x < border_size[0]) { // 3
        ivec2 local_position = ivec2(ui_position.x, ui_position.y - border_size[1]);
        local_position.y = local_position.y % height_4;
        texture_offset = local_position + positions[3];
    }
    else if (ui_position.y < border_size[1]) { // 1
        ivec2 local_position = ivec2(ui_position.x, ui_position.y);
        local_position.x = local_position.x % width_2;
        texture_offset = local_position + positions[1];
    }
    else if ((frame_size_px.x - ui_position.x) <= border_size[3]) { // 5
        ivec2 local_position = ivec2(border_size[2] - frame_size_px.x + ui_position.x, ui_position.y - border_size[3]);
        local_position.y = local_position.y % height_6;
        texture_offset = local_position + positions[5];
    }
    else if ((frame_size_px.y - ui_position.y) <= border_size[3]) { // 7
        ivec2 local_position = ivec2(ui_position.x - border_size[2], ui_position.y - frame_size_px.y + border_size[3]);
        local_position.x = local_position.x % width_8;
        texture_offset = local_position + positions[7];
    } else { // 4
        ivec2 local_position = ivec2(ui_position.x - border_size[0], ui_position.y - border_size[1]);
        local_position.x = local_position.x % width_5;
        local_position.y = local_position.y % height_5;
        texture_offset = positions[4] + local_position;
    }

    uvec4 color_int = texelFetch(window_texture, texture_offset, 0);
    if (color_int.a == 0) {
        discard;
    }

    color = vec3(color_int.rgb)/255.0;
}
