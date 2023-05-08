#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in float age;

void
main(){

    float red = mix(1, .7, age);
    float green = mix(.8, .7, age);
    float blue = mix(.5, 1, age);

    color = vec3(red,green,blue);
}
