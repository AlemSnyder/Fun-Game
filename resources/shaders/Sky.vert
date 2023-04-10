#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 pos;
layout(location = 1) in float age_in;
layout(location = 2) in vec2 star_corner;
uniform mat4 MVP;
uniform mat4 pixel_projection;

// Output data ; will be interpolated for each fragment.
out float age;

void
main() {
    vec4 star_center_camera_space = MVP * vec4(pos.xyz, 0);

    //star_center_camera_space = star_center_camera_space / star_center_camera_space.w

    gl_Position = vec4(star_center_camera_space.xy / star_center_camera_space.w + (pixel_projection * vec4(star_corner * pos.w, 0, 0)).xy, 9, 1);

    //gl_Position = star_center_camera_space;//MVP * vec4(star_corner*100,-1,1);

    age = age_in;
}
