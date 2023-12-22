#version 450 core

// Input data
//in vec3 color_in;

uniform vec3 sunlight_color;

// Ouput data
layout(location = 0) out vec3 color;

void
main(){

    color = sunlight_color*2;
}
