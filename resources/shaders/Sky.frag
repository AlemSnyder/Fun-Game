#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler1D stars;
uniform mat4 MVP;
uniform mat4 V;
uniform int number_of_stars;

in vec2 EyeDirectionScreenSpace;

float
get_brightness(vec2 EyeDirectionScreen, vec2 StarDirectionScreen, float brightness){
    // computed in the screen

    // distance from pixel to star.
    float len = length(EyeDirectionScreen-StarDirectionScreen);

    // return the brightness
    return brightness - len;
}

void
main(){

    //int brightness = 0
    //for (int i = 0, i < number_of_stars, i++){
    //    vec3 star = texelFetch(stars, i, 0);
    //    vec3 direction = {cos(star.x), sin(star.x)cos(star.y), sin(star.x)sin(star.y)}

    //    StarDirection_cameraspace = vec3(0, 0, 0) - (MVP * direction).xyz;

    //    next_brightness = get_brightness(EyeDirectionScreenSpace, StarDirection_cameraspace.xy, star.z);
    //    if (next_brightness > brightness){
    //        brightness = next_brightness
    //    }
    //}

    //brightness = clamp(brightness, 0.0, 1.0);

    color = vec3(EyeDirectionScreenSpace.x, EyeDirectionScreenSpace.y, 1);

    //color = {brightness,brightness,brightness,1}
}
