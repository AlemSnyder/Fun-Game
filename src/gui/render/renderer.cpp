#include "renderer.hpp"

#include "../../util/files.hpp"
#include "../data_structures/terrain_mesh.hpp"
#include "../handler.hpp"
#include "../meshloader.hpp"
#include "../scene/controls.hpp"
#include "../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace render {

MainRenderer::MainRenderer() {
    // non-indexed program
    programID_single_ = load_shaders(
        files::get_resources_path() / "shaders" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // indexed program
    programID_multi_ = load_shaders(
        files::get_resources_path() / "shaders" / "ShadowMappingInstanced.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // ---- non-indexed program ----
    matrix_ID_ = glGetUniformLocation(programID_single_, "MVP");
    view_matrix_ID_ = glGetUniformLocation(programID_single_, "V");
    depth_bias_ID_ = glGetUniformLocation(programID_single_, "DepthBiasMVP");
    shadow_map_ID_ = glGetUniformLocation(programID_single_, "shadowMap");
    color_map_ID_ = glGetUniformLocation(programID_single_, "meshColors");
    light_direction_ID_ =
        glGetUniformLocation(programID_single_, "LightInvDirection_worldspace");
    // ------ indexed program ------
    matrix_ID_multi_ = glGetUniformLocation(programID_multi_, "MVP");
    view_matrix_ID_multi_ = glGetUniformLocation(programID_multi_, "V");
    depth_bias_ID_multi_ = glGetUniformLocation(programID_multi_, "DepthBiasMVP");
    shadow_map_ID_multi_ = glGetUniformLocation(programID_multi_, "shadowMap");
    color_map_ID_multi_ = glGetUniformLocation(programID_multi_, "meshColors");
    light_direction_ID_multi_ =
        glGetUniformLocation(programID_multi_, "LightInvDirection_worldspace");
}

MainRenderer::~MainRenderer() {
    glDeleteProgram(programID_single_);
    glDeleteProgram(programID_multi_);
}

void
MainRenderer::add_mesh(std::shared_ptr<MeshData::SingleComplexMesh> mesh) {
    singles_meshes_.push_back(std::move(mesh));
}

void
MainRenderer::add_mesh(std::shared_ptr<MeshData::MultiComplexMesh> mesh) {
    multis_meshes_.push_back(std::move(mesh));
}

void
MainRenderer::set_depth_texture(GLuint texture_id) {
    depth_texture_ = texture_id;
}

void
MainRenderer::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void
MainRenderer::set_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
    depth_projection_matrix_ = depth_projection_matrix;
}

void
MainRenderer::render(GLFWwindow* window, GLuint frame_buffer) const {
    // Render to the screen
    gui::FrameBufferHandler::bind_fbo(frame_buffer);

    // get he window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    // Cull back-facing triangles -> draw only front-facing triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Enable the depth test, and enable drawing to the depth texture
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Use our shader
    glUseProgram(programID_single_);

    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Compute the MVP matrix from keyboard and mouse input
    // controls::computeMatricesFromInputs(window);
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    // glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    // Shadow bias matrix of-sets the shadows
    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depth_bias_MVP = bias_matrix * depthMVP;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_ID_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(depth_bias_ID_, 1, GL_FALSE, &depth_bias_MVP[0][0]);

    // set the light direction uniform
    glUniform3f(
        light_direction_ID_, light_direction_.x, light_direction_.y, light_direction_.z
    );

    // Bind Shadow Texture to Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glUniform1i(shadow_map_ID_, 1);

    for (std::shared_ptr<MeshData::SingleComplexMesh> mesh : singles_meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, mesh->get_color_texture());
        glUniform1i(color_map_ID_, 2);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
        glVertexAttribIPointer(
            0,       // attribute
            3,       // size
            GL_INT,  // type
            0,       // stride
            (void*)0 // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_color_buffer());
        glVertexAttribIPointer(
            1,                 // attribute
            1,                 // size
            GL_UNSIGNED_SHORT, // type
            0,                 // stride
            (void*)0           // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normal_buffer());
        glVertexAttribIPointer(
            2,       // attribute
            3,       // size
            GL_BYTE, // type
            0,       // stride
            (void*)0 // array buffer offset
        );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_element_buffer());

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0                  // element array buffer offset
        );
    }

    // Use our shader
    glUseProgram(programID_multi_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_ID_multi_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_ID_multi_, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(depth_bias_ID_multi_, 1, GL_FALSE, &depth_bias_MVP[0][0]);
    glUniform3f(
        light_direction_ID_multi_, light_direction_.x, light_direction_.y,
        light_direction_.z
    );

    // Bind our texture in Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glUniform1i(shadow_map_ID_multi_, 1);

    for (std::shared_ptr<MeshData::MultiComplexMesh> mesh : multis_meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, mesh->get_color_texture());
        glUniform1i(color_map_ID_multi_, 2);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
        glVertexAttribIPointer(
            0,       // attribute
            3,       // size
            GL_INT,  // type
            0,       // stride
            (void*)0 // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_color_buffer());
        glVertexAttribIPointer(
            1,                 // attribute
            1,                 // size
            GL_UNSIGNED_SHORT, // type
            0,                 // stride
            (void*)0           // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normal_buffer());
        glVertexAttribIPointer(
            2,       // attribute
            3,       // size
            GL_BYTE, // type
            0,       // stride
            (void*)0 // array buffer offset
        );

        // 4th attribute buffer : transform
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

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_element_buffer());

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0,                 // element array buffer offset
            mesh->get_num_models()
        );
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glVertexAttribDivisor(3, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace render

} // namespace gui