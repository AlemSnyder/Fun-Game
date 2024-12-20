# version 450 core

// Output data
layout(location = 0) out vec3 color;

uniform mat4 MVIP;
uniform vec3 direct_light_color;

in vec2 EyeDirectionScreenSpace;
in vec3 sunlight_direction_unit;

float r = 6.0; // earth radious
float d = 0.5; // thickness of atmosphere
float c = pow(r, 2.0) - pow(r + d, 2.0);

float
atmosphere_thickness(float slope_above_horizon){
    float a = 1 + pow(slope_above_horizon, 2);
    float b = 2 * r * slope_above_horizon;

    float thickness = (-b + pow(pow(b, 2.0) - 4 * a * c, 0.5)) / (2 * a) * pow(a, 0.5);
    
    return thickness;
};

float diffusion = 3.5;

vec3 convolution = vec3(0.124, 0.08, 0.04);
vec3 color_intensities = vec3(0.8, 0.85, 0.9);

void
main(){
    vec4 world_space_direction = MVIP * vec4(EyeDirectionScreenSpace, -1, 1);

    //float run = pow(dot(world_space_direction.xy, world_space_direction.xy), 0.5);

    float cos_diffraction_angle = dot(world_space_direction.xyz, sunlight_direction_unit)/ (length(world_space_direction.xyz));

    color = 2 * length(direct_light_color) * color_intensities * (0.5 + (pow(cos_diffraction_angle, 2.0) - 0.5) * convolution);
}
