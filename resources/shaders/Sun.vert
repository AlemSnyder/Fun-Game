#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 2) in vec2 star_corner;
uniform mat4 MVP;
uniform mat4 pixel_projection;
uniform vec3 sun_position;

float sun_size = 100;
out vec3 color_in;

void
main() {
    vec4 star_center_camera_space = MVP * vec4(sun_position, 0);

    vec4 position = star_center_camera_space / star_center_camera_space.w
                    + pixel_projection * vec4(star_corner * sun_size, 0, 0);
    // I have no idea why I have to do this.
    position.z = star_center_camera_space.z/3;

    gl_Position = position;

    float height = dot(sun_position, vec3(0,0,1) );
    float color_changer = sin( ( height * .7 + .3 ) * 3.141592 );

    float red = .6 + .4 * color_changer;
    float green = .5 + .4 * color_changer;
    float blue = .4 + .3 * color_changer;

    color_in = vec3(red, green, blue);

}
