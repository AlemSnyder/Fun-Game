#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

in vec2 UV;

void
main(){

    color = vec3(UV/50, 0.9);
}
