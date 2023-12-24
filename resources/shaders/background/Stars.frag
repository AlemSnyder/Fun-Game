#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in float age;

void
main(){

    float red = mix(1.0, 0.7, age);
    float green = mix(0.8, 0.7, age);
    float blue = mix(0.5, 1.0, age);

    color = vec3(red, green, blue);
}
