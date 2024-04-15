#version 450 core

flat in uint Vertex_color_id; // color id of vertex/face
flat in uint ModelTextureID;  // texture row id for entire model
// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
//in vec4 Normal_cameraspace;
//in vec4 EyeDirection_cameraspace;
//in vec4 LightDirection_cameraspace;
in vec4 ShadowCoord;
in flat float cosTheta;
in flat vec3 Vertex_color;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2DShadow shadow_texture;
uniform vec3 direct_light_color;
uniform vec3 diffuse_light_color;

const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790)
);

// Returns a random number based on a vec3 and an int.
float
random(vec3 seed, int i) {
    vec4 seed4 = vec4(seed, i);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

const int num_samples = 1;

void
main() {

    // Material properties
    vec3 MaterialDiffuseColor = Vertex_color * 0.6;
    vec3 MaterialAmbientColor = diffuse_light_color * MaterialDiffuseColor;

    float visibility = 1.0;

    // TODO Fixed bias
    float bias = 0.005 * cosTheta;

    // Sample the shadow map 4 times
    for (int i = 0; i < 4; i++) {
        int index = i;
        // being fully in the shadow will eat up 4*0.3 = 1.2
        // 0.2 potentially remain, which is quite dark.

        vec3 texture_map_position = vec3(
            ShadowCoord.xy + poissonDisk[index] / (10000.0),
            (ShadowCoord.z - bias) / ShadowCoord.w
        );

        visibility -= 0.3 * (1.0 - texture( shadow_texture, texture_map_position ));
    }

    visibility = max(visibility, 0.0);

    color = // base_color
            //  Ambient : simulates indirect lighting
        MaterialAmbientColor +
        // Diffuse : "color" of the object
        visibility * MaterialDiffuseColor * direct_light_color * cosTheta;
        

    //color = vec3(.2, 0.3, 0.4); good 16 frame time
    //color = MaterialDiffuseColor + MaterialAmbientColor; // 16.6 framne time

}
