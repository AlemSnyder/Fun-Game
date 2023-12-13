#version 450 core

// Input data
in vec3 color_in;

// Ouput data
layout(location = 0) out vec3 color;

void
main(){

    color = color_in;
}
