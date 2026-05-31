// -*- lsst-glsl -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file DepthRTT.frag
 *
 * @author @AlemSnyder
 *
 * @brief Render depth to texture from eneity model with size 1/16
 *
 * @ingroup SHADERS SCENE
 *
 */

#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in ivec3 vertex_position_modelspace;
layout(location = 3) in ivec3 model_matrix_transform;

// Values that stay constant for the whole mesh.
uniform mat4 depth_MVP;

uint scalr_divisor = 16;

mat4 rotate =
    mat4(vec4(0, -1, 0, 0), vec4(1, 0, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 0));

void
main() {
    vec4 vertex_position_modelspace_rotated =
        rotate * vec4(vertex_position_modelspace / scalr_divisor - vec3(.5, .5, 0), 1)
        + vec4(.5, .5, 0, 0);

    vec4 vertex_postion_worldspace =
        vertex_position_modelspace_rotated + vec4(model_matrix_transform, 1);

    gl_Position = depth_MVP * vertex_postion_worldspace;
}
