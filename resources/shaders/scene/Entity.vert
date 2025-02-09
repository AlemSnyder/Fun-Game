#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertexPosition_modelspace;
layout(location = 1) in uint vertex_color_id;
layout(location = 2) in ivec3 vertexNormal_modelspace;

layout(location = 6) in vec4 model_matrix_transform_1;
layout(location = 7) in vec4 model_matrix_transform_2;
layout(location = 8) in vec4 model_matrix_transform_3;
layout(location = 9) in vec4 model_matrix_transform_4;
//layout(location = 6) in mat4 model_matrix_transform;
// Output data ; will be interpolated for each fragment.
// out uint Vertex_color_id;
// out vec3 Position_worldspace;
// out vec3 Normal_cameraspace;
// out vec3 EyeDirection_cameraspace;
// out vec3 LightDirection_cameraspace;
// out vec4 ShadowCoord;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void
main() {
   mat4 model_matrix_transform = mat4(model_matrix_transform_1,
                                      model_matrix_transform_2,
                                      model_matrix_transform_3,
                                      model_matrix_transform_4);

    vec4 vertex_postion_world_space = model_matrix_transform * vec4(vertexPosition_modelspace, 1);

    vec4 vertexNormal_modelspace_rotated = model_matrix_transform * vec4(vertexNormal_modelspace, 0);

    // Output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vertex_postion_world_space;

    // 
    //ShadowCoord = depth_MVP * (vertex_postion_world_space + vertexNormal_modelspace_rotated * .5);

    // Position of the vertex, in worldspace : M * position
    //Position_worldspace = (vertex_postion_world_space).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    //EyeDirection_cameraspace =
    //    vec3(0, 0, 0) - (view_matrix * vertex_postion_world_space).xyz;

    // Vector that goes from the vertex to the light, in camera space
    //LightDirection_cameraspace = (view_matrix * vec4(light_direction, 0)).xyz;

    // Normal of the the vertex, in camera space
    // Normal_cameraspace = (view_matrix * vertexNormal_modelspace_rotated)
    //                         .xyz; // Only correct if ModelMatrix does not scale the
                                   // model ! Use its inverse transpose if not.

    // UV of the vertex. No special space for this one.
    // Vertex_color_id = vertex_color_id;
}
