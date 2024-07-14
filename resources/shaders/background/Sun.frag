#version 450 core

// Input data
//in vec3 color_in;

uniform vec3 direct_light_color;

// Ouput data
layout(location = 0) out vec3 color;

void
main(){

    color = direct_light_color*2;
}
