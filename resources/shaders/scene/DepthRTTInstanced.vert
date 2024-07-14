#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertex_position_modelspace;
layout(location = 3) in ivec3 model_matrix_transform;

// Values that stay constant for the whole mesh.
uniform mat4 depth_MVP;

mat4 rotate =
    mat4(vec4(0, -1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 0));

void
main() {
    vec4 vertex_position_modelspace_rotated =
        rotate * vec4(vertex_position_modelspace - vec3(.5, .5, 0), 1)
        + vec4(.5, .5, 0, 0);

    vec4 vertex_postion_worldspace =
        vertex_position_modelspace_rotated + vec4(model_matrix_transform, 1);

    gl_Position = depth_MVP * vertex_postion_worldspace;
}
