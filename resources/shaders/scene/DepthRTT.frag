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
 * @brief Render depth to texture
 *
 * @ingroup SHADERS SCENE
 *
 */


#version 450 core

// Ouput data
layout(location = 0) out float fragment_depth;

void
main() {
    fragment_depth = gl_FragCoord.z;
}
