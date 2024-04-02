#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertexPosition_modelspace;
layout(location = 1) in uint vertex_color_id;
layout(location = 2) in ivec3 vertexNormal_modelspace;
layout(location = 3) in ivec4 model_matrix_transform;
layout(location = 4) in uint model_texture_id;
// Output data ; will be interpolated for each fragment.
out uint Vertex_color_id;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;
flat out uint ModelTextureID;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 view_matrix;
uniform vec3 light_direction;
uniform mat4 depth_MVP;

//uniform uint scalr_divisor; // How much to shrink the mesh

uint scalr_divisor = 16;

mat4 rotate = mat4(vec4(0,-1,0,0),vec4(1,0,0,0),vec4(0,0,1,0),vec4(0,0,0,0));

void
main() {

    ModelTextureID = model_texture_id;

    mat4 rotation = mat4(1);
    // TODO optimize
//    for (int r = 0; r < model_matrix_transform.w; r++){
//        rotation = rotation * rotate;
//    }

    vec4 vertexPosition_modelspace_rotated = rotation * vec4(vertexPosition_modelspace / float(scalr_divisor) - vec3(.5,.5,0), 0) + vec4(.5,.5,0,0);

    vec4 vertex_postion_model_space_instanced = vertexPosition_modelspace_rotated + 
        vec4(model_matrix_transform.xyz, 1);

    vec4 vertexNormal_modelspace_rotated = rotate * vec4(vertexNormal_modelspace, 0);

    // Output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vertex_postion_model_space_instanced;

    // possibly use the below to get less dotted ness on some surfaes
    //ShadowCoord = depth_MVP * (vertex_postion_model_space_instanced+vertexNormal_modelspace_rotated/10);
    ShadowCoord = depth_MVP * (vertex_postion_model_space_instanced + vertexNormal_modelspace_rotated * .5);

    // Position of the vertex, in worldspace : M * position
    Position_worldspace = (vertex_postion_model_space_instanced).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    EyeDirection_cameraspace =
        vec3(0, 0, 0) - (view_matrix * vertex_postion_model_space_instanced).xyz;

    // Vector that goes from the vertex to the light, in camera space
    LightDirection_cameraspace = (view_matrix * vec4(light_direction, 0)).xyz;

    // Normal of the the vertex, in camera space
    Normal_cameraspace = (view_matrix * vertexNormal_modelspace_rotated)
                             .xyz; // Only correct if ModelMatrix does not scale the
                                   // model ! Use its inverse transpose if not.

    // UV of the vertex. No special space for this one.
    Vertex_color_id = vertex_color_id;
}
