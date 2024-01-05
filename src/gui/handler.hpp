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

#pragma once

#include <GL/glew.h>

namespace gui {

class BinderBase {
 protected:
    // Private CTOR as this is a singleton
    BinderBase() {}

 public:
    // Delete all CTORs and CTOR-like operators
    BinderBase(BinderBase&&) = delete;
    BinderBase(BinderBase const&) = delete;

    void operator=(BinderBase&&) = delete;
    void operator=(BinderBase const&) = delete;
};

/**
 * @brief Handles frame buffer on gpu
 *
 * @details Binds frame buffer, and guaranties that bound buffers are not bound
 * again.
 */
class FrameBufferHandler : public BinderBase {
 private:
    GLuint fbo_id;

    FrameBufferHandler() : fbo_id(0) {}

 public:
    inline GLuint
    get_current_fbo() {
        return fbo_id;
    };

    void bind_fbo(GLuint new_fbo_id);

    // Instance accessor
    static inline FrameBufferHandler&
    instance() {
        static FrameBufferHandler obj;
        return obj;
    }
};

/**
 * @brief Handles vertex buffer on gpu
 *
 * @details Binds vertex buffer, and guaranties that bound buffers are not bound
 * again.
 */
class VertexBufferHandler : public BinderBase {
 private:
    GLuint vertex_buffer;

    VertexBufferHandler() : vertex_buffer(0) {}

 public:
    inline GLuint
    get_current_vertex_buffer() {
        return vertex_buffer;
    };

    void bind_vertex_buffer(GLuint new_vertex_buffer);

    // Instance accessor
    static inline VertexBufferHandler&
    instance() {
        static VertexBufferHandler obj;
        return obj;
    }
};

} // namespace gui
