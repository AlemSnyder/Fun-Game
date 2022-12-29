#version 450 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler1D stars;

in vec3 EyeDirectionModelSpace;

int
get_brightness(vec3 EyeDirection, vec3 star){
    // rad from cross product

    // direction to star from angle
    vec3 n = {cos(star.x), sin(star.x)cos(star.y), sin(star.x)sin(star.y)}

    // Direction of the light (from the fragment to the light)
    vec3 l = normalize(EyeDirection);
    // Cosine of the angle between the normal and the light direction,
    // clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendiular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n, l), 0, 1);

    // not just 2-angle
    return star.z-cosTheta;
}

void
main(){

    int brightness = 0
    for (int i = 0, i < some_length, i++){
        vec3 star = texelFetch(stars, i, 0);
        next_brightness = get_brightness(EyeDirectionModelSpace, star);
        if (next_brightness > brightness){
            brightness = next_brightness
        }
    }

    brightness = clamp(brightness, 0.0, 1.0);

    color = {brightness,brightness,brightness,1}
}