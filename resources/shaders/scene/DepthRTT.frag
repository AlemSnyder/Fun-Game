#version 450 core

// Ouput data
layout(location = 0) out float fragment_depth;

void
main() {
    fragment_depth = gl_FragCoord.z;
}
