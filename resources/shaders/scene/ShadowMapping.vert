#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertexPosition_modelspace;
layout(location = 1) in uint vertex_color_id;
layout(location = 2) in ivec3 vertexNormal_modelspace_int;

// Output data ; will be interpolated for each fragment.
out uint Vertex_color_id;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 view_matrix;
uniform vec3 light_direction;
uniform mat4 depth_texture_projection;

void
main() {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    vec3 vertexNormal_modelspace = vec3(vertexNormal_modelspace_int);

    ShadowCoord = depth_texture_projection * vec4(vertexPosition_modelspace + vertexNormal_modelspace * .5, 1);

    // Position of the vertex, in worldspace : M * position
    Position_worldspace = (vec4(vertexPosition_modelspace, 1)).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    EyeDirection_cameraspace =
        vec3(0, 0, 0) - (view_matrix * vec4(vertexPosition_modelspace, 1)).xyz;

    // Vector that goes from the vertex to the light, in camera space
    LightDirection_cameraspace = (view_matrix * vec4(light_direction, 0)).xyz;

    // Normal of the the vertex, in camera space
    Normal_cameraspace = (view_matrix * vec4(vertexNormal_modelspace, 0))
                             .xyz; // Only correct if ModelMatrix does not scale the
                                   // model ! Use its inverse transpose if not.

    // UV of the vertex. No special space for this one.
    Vertex_color_id = vertex_color_id;
}
