#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D texture;

in vec2 UV;

void main(){
	color = texture(texture, UV);
}
