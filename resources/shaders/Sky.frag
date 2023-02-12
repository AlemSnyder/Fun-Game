#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler1D stars;
uniform mat4 MVP;
uniform mat4 pixel_projection;
uniform int number_of_stars;

in vec2 EyeDirectionScreenSpace;

float
get_brightness(vec2 EyeDirectionScreen, vec2 StarDirectionScreen, float brightness){
    // computed in the screen

    // distance from pixel to star.
    float len = length(EyeDirectionScreen-StarDirectionScreen);

    // return the brightness
    return clamp(brightness - len, 0.0, 1.0);
}

vec3
star_color(float brightness, float age){
    float red = mix(1, .7, age);
    float green = mix(.8, .7, age);
    float blue = mix(.5, 1, age);
    return vec3(brightness*red,brightness*green,brightness*blue);
    //return vec3(brightness,brightness,brightness);
}

void
main(){

    vec3 brightness = {0.0,0.0,0.0};
    for (int i = 0; i < number_of_stars; ++i){
        vec4 star = texelFetch(stars, i, 0);
        vec4 direction = {cos(radians(star.x))*sin(radians(star.y)), sin(radians(star.x))*sin(radians(star.y)), cos(radians(star.y)),0};

        vec4 direction_projected = (MVP * direction);

        if (direction_projected.z > 0){
            continue;
        }

        vec2 StarDirection_cameraspace = (pixel_projection * vec4((direction_projected).xy/direction_projected.w, 0, 1)).xy;

        float next_brightness = get_brightness(EyeDirectionScreenSpace, StarDirection_cameraspace, star.z);

        //next_color = star_color(next_brightness, star.w);

        brightness = brightness + star_color(next_brightness, star.w);

        //if (next_brightness > brightness){
        //    brightness = next_brightness;
        //}
    }

    brightness.x = clamp(brightness.x, 0.0, 1.0);
    brightness.y = clamp(brightness.y, 0.0, 1.0);
    brightness.z = clamp(brightness.z, 0.0, 1.0);

    color = brightness;//vec3(brightness,brightness,brightness);
    /*
    if (abs(EyeDirectionScreenSpace.x - EyeDirectionScreenSpace.y) < 4){
        color = vec3(1, 1, 1);
    }

    if (abs(EyeDirectionScreenSpace.x + EyeDirectionScreenSpace.y) < 4){
        color = vec3(1, 1, 1);
    }

    if (abs(EyeDirectionScreenSpace.x) < 2){
        color = vec3(.5, .5, 1);
    }
    if (abs(EyeDirectionScreenSpace.y) < 2){
        color = vec3(.5, 1, .5);
    }
    */
}
