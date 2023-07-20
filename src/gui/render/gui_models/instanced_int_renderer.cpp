#include "instanced_int_renderer.hpp"

#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "../../shader.hpp"
#include "../data/instanced_int.hpp"
#include "individual_int_renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace models {

template <class T>
InstancedIntRenderer<T>::InstancedIntRenderer(ShaderHandeler shader_handler) {
    // TODO should pass these paths
    // non-indexed program
    this->programID_render_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "ShadowMappingInstanced.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // indexed program
    this->programID_shadow_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "DepthRTTInstanced.vert",
        files::get_resources_path() / "shaders" / "DepthRTT.frag"
    );
}

template <class T>
void
InstancedIntRenderer<T>::load_transforms_buffer(std::shared_ptr<T> mesh) const {
    // 4nd attribute buffer : transform
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_model_transforms());
    glVertexAttribIPointer(
        3,       // attribute
        3,       // size
        GL_INT,  // type
        0,       // stride
        (void*)0 // array buffer offset
    );
    glVertexAttribDivisor(3, 1);
    // to undo the vertex attribute divisor do
    // glVertexAttribDivisor(3, 0);
}

template <class T>
void
InstancedIntRenderer<T>::render_frame_buffer(GLFWwindow* window, GLuint frame_buffer)
    const {
    // Render to the screen
    gui::FrameBufferHandler::bind_fbo(frame_buffer);

    // get he window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    IndividualIntRenderer<T>::setup_render();

    for (std::shared_ptr<T> mesh : this->meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        load_vertex_buffer(mesh);
        load_color_buffers(mesh);

        load_transforms_buffer();

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0,                 // element array buffer offset
            mesh->get_num_models()
        );
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
    }
}

template <class T>
void
InstancedIntRenderer<T>::render_frame_buffer_multisample(
    GLFWwindow* window, GLuint frame_buffer
) const {
    render_frame_buffer(window, frame_buffer);
}

template <class T>
void
InstancedIntRenderer<T>::render_shadow_map(
    int shadow_width_, int shadow_height_, GLuint frame_buffer_name_
) const {
    gui::FrameBufferHandler::bind_fbo(frame_buffer_name_);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, shadow_width_, shadow_height_);

    IndividualIntRenderer<T>::setup_shadow();

    // draw the indexed meshes
    for (std::shared_ptr<T> mesh : this->meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        IndividualIntRenderer<T>::load_vertex_buffer(mesh);

        load_transforms_buffer(mesh);

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0,                 // element array buffer offset
            mesh->get_num_models()
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(3);
    }
}

} // namespace models

} // namespace gui
