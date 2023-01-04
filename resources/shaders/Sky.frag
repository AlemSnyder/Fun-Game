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
    return brightness/10 - len;
}

void
main(){

    float brightness = 0;
    for (int i = 0; i < number_of_stars; ++i){
        vec3 star = texelFetch(stars, i, 0).rgb;
        vec4 direction = {cos(radians(star.x)), sin(radians(star.x))*cos(radians(star.y)), sin(radians(star.x))*sin(radians(star.y)),0};

        vec2 StarDirection_cameraspace = (V * vec4(-(MVP * direction).xy, 0, 1)).xy;

        float next_brightness = get_brightness(EyeDirectionScreenSpace, StarDirection_cameraspace, star.z);
        if (next_brightness > brightness){
            brightness = next_brightness;
        }
    }

    brightness = clamp(brightness, 0.0, 1.0);

    //color = vec3(EyeDirectionScreenSpace.x, EyeDirectionScreenSpace.y, 1);

    color = vec3(brightness,brightness,brightness);
}
