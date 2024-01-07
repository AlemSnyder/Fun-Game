

#pragma once

#include "../../../types.hpp"
#include "../../handler.hpp"
#include "../graphics_data/gpu_data.hpp"
#include "gui_render_types.hpp"
#include "opengl_program.hpp"
#include "uniform.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace gui {

namespace shader {

// struct program_setup {
//     void operator()() const;
// };

//
// template <data_structures::GPUdata_or_something T>
class ShaderProgram_Standard :
    virtual public render_to::FrameBuffer,
    virtual public OpenGLProgramExecuter {
 private:
    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUData>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_Standard(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        OpenGLProgramExecuter(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        attach_uniforms(uniforms_);
    }

    // template <data_structures::GPUdata_or_something T>
    // void render(screen_size_t width, screen_size_t height, GLuint //framebuffer_ID)
    // const;

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        use_program();

        setup_();

        uniforms_.bind();

        for (std::shared_ptr<data_structures::GPUData> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            // test if T inherits from Instancing or not

            // Draw the triangles !
            // glDrawElements(
            //    GL_TRIANGLES,             // mode
            //    mesh->get_num_vertices(), // count
            //    mesh->get_element_type(), // type
            //    (void*)0                  // element array buffer offset
            //);

            // Draw the triangles !
            glDrawArrays(
                GL_TRIANGLE_STRIP,       // mode
                0,                       // start
                mesh->get_num_vertices() // number of vertices

            );

            mesh->release();
        }
    }

    virtual void
    reload_program() override {
        attach_uniforms(uniforms_);
    }
};

class ShaderProgram_Elements :
    virtual public render_to::FrameBuffer,
    virtual public OpenGLProgramExecuter {
 private:
    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataElements>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_Elements(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        OpenGLProgramExecuter(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        attach_uniforms(uniforms_);
    }

    // template <data_structures::GPUdata_or_something T>
    // void render(screen_size_t width, screen_size_t height, GLuint //framebuffer_ID)
    // const;

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        use_program();

        setup_();

        uniforms_.bind();

        for (std::shared_ptr<data_structures::GPUDataElements> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            // test if T inherits from Instancing or not

            // Draw the triangles !
            glDrawElements(
                GL_TRIANGLES,             // mode
                mesh->get_num_vertices(), // count
                mesh->get_element_type(), // type
                (void*)0                  // element array buffer offset
            );

            // Draw the triangles !
            // glDrawArrays(
            //    GL_TRIANGLE_STRIP,     // mode
            //    0,                     // start
            //    mesh->get_num_vertices // number of vertices

            //            );

            mesh->release();
        }
    }

    virtual void
    reload_program() override {
        attach_uniforms(uniforms_);
    }
};

class ShaderProgram_Instanced :
    virtual public render_to::FrameBuffer,
    virtual public OpenGLProgramExecuter {
 private:
    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataInstanced>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_Instanced(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        OpenGLProgramExecuter(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        attach_uniforms(uniforms_);
    }

    // template <data_structures::GPUdata_or_something T>
    // void render(screen_size_t width, screen_size_t height, GLuint //framebuffer_ID)
    // const;

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        use_program();

        setup_();

        uniforms_.bind();

        for (std::shared_ptr<data_structures::GPUDataInstanced> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            // Draw the triangles !
            glDrawArraysInstanced(
                GL_TRIANGLE_STRIP,        // mode
                0,                        // start
                mesh->get_num_vertices(), // number of vertices
                mesh->get_num_models()    // number of models

            );

            mesh->release();
        }
    }

    virtual void
    reload_program() override {
        attach_uniforms(uniforms_);
    }
};

class ShaderProgram_ElementsInstanced :
    virtual public render_to::FrameBuffer,
    virtual public OpenGLProgramExecuter {
 private:
    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataElementsInstanced>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_ElementsInstanced(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        OpenGLProgramExecuter(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        attach_uniforms(uniforms_);
    }

    // template <data_structures::GPUdata_or_something T>
    // void render(screen_size_t width, screen_size_t height, GLuint //framebuffer_ID)
    // const;

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        use_program();

        setup_();

        uniforms_.bind();

        for (std::shared_ptr<data_structures::GPUDataElementsInstanced> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            // Draw the triangles !
            glDrawElementsInstanced(
                GL_TRIANGLES,             // mode
                mesh->get_num_vertices(), // count
                mesh->get_element_type(), // type
                (void*)0,                 // element array buffer offset
                mesh->get_num_models()
            );

            mesh->release();
        }
    }
};

} // namespace shader
} // namespace gui