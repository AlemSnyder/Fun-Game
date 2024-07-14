#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 star_corner;
uniform mat4 MVP;
uniform mat4 pixel_projection;
//uniform vec3 sun_position;
uniform vec3 light_direction;

float sun_size = 100;

void
main() {
    vec4 star_center_camera_space = MVP * vec4(light_direction, 0);

    vec4 position = star_center_camera_space / abs(star_center_camera_space.w)
                    + pixel_projection * vec4(star_corner * sun_size, 0, 0);
    // I have no idea why I have to do this.
    position.z = star_center_camera_space.z/3;

    gl_Position = position;

}
