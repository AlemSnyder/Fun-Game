#version 450 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2DMS texture;
uniform uint tex_samples;
uniform uint height;
uniform uint width;

in vec2 UV;

vec3 texture_MS(sampler2DMS tex, ivec2 coord){
    vec3 color = vec3(0.0);
    for (int i=0; i < tex_samples; i++){
        color += texelFetch(tex, coord, i).rgb;
    }
    return color / float(tex_samples);
}

void
main() {

    //texelFetch(texture, UV, UV height)

    color = texture_MS(texture, ivec2(int(UV.x* width), int(UV.y*height)));
    //color = vec3(0,1,0);
}
