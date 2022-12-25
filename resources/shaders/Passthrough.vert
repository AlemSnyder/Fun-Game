#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertexPosition_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void
main() {
    gl_Position = vec4(vertexPosition_modelspace, 1);
    UV = (vertexPosition_modelspace.xy + vec2(1, 1)) / 2.0;
}
