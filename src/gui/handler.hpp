// -*- lsst-c++ -*-
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
 * @file handler.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Define FrameBufferHandler class
 *
 * @ingroup GUI
 *
 */
// handles fbo, and other things if necessary

#pragma once

#include <GL/glew.h>

namespace gui {

class FrameBufferHandler {
 private:
    GLuint fbo_id;

    // Private CTOR as this is a singleton
    FrameBufferHandler() : fbo_id(0) {}

 public:
    // Delete all CTORs and CTOR-like operators
    FrameBufferHandler(FrameBufferHandler&&) = delete;
    FrameBufferHandler(FrameBufferHandler const&) = delete;

    void operator=(FrameBufferHandler&&) = delete;
    void operator=(FrameBufferHandler const&) = delete;

    // Instance accessor
    static inline FrameBufferHandler&
    getInstance() {
        static FrameBufferHandler obj;
        return obj;
    }

    inline GLuint
    get_current_fbo() {
        return fbo_id;
    };

    void bind_fbo(GLuint new_fbo_id);
};

} // namespace gui
