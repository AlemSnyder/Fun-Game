#include <memory>
#include <stdexcept>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include "meshloader.hpp"
#include "shader.hpp"
#include "shadow_map.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

ShadowMap::ShadowMap(int w, int h) {
    depth_texture_ = 0;
    frame_buffer_name_ = 0;
    programID_ = load_shaders(
        "./resources/shaders/DepthRTT.vert", "./resources/shaders/DepthRTT.frag"
    );
    programID_multi_ = load_shaders(
        "./resources/shaders/DepthRTTInstanced.vert",
        "./resources/shaders/DepthRTTInstanced.frag"
    );

    // Get a handle for our "MVP" uniform
    depth_matrix_ID_ = glGetUniformLocation(programID_, "depthMVP");
    depth_matrix_ID_multi_ = glGetUniformLocation(programID_multi_, "depthMVP");

    // Compute the MVP matrix from the light's point of view
    // depth_projection_matrix_ =
    //    glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    shadow_width_ = w;
    shadow_height_ = h;

    // ---------------------------------------------
    // Render Shadow to Texture - specific code begins here
    // ---------------------------------------------

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth
    // buffer.
    frame_buffer_name_ = 0;
    glGenFramebuffers(1, &frame_buffer_name_);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_name_);

    // Depth texture. Slower than a depth buffer, but you can sample it later in
    // your shader

    glGenTextures(1, &depth_texture_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width_, shadow_height_, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture_, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::invalid_argument("Framebuffer is not ok");
    }
}

void
ShadowMap::add_mesh(std::shared_ptr<MeshLoader::SingleMesh> mesh) {
    singles_meshes_.push_back(std::move(mesh));
}

void
ShadowMap::add_mesh(std::shared_ptr<MeshLoader::MultiMesh> mesh) {
    multi_meshes_.push_back(std::move(mesh));
}

void
ShadowMap::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void
ShadowMap::set_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
    depth_projection_matrix_ = depth_projection_matrix;
}

void
ShadowMap::render_shadow_depth_buffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_name_);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, shadow_width_, shadow_height_);

    // Cull back-facing triangles -> draw only front-facing triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID_);

    // glm::mat4 depth_view_matrix =
    //     glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //  or, for spot light :
    //  glm::vec3 lightPos(5, 20, 20);
    //  glm::mat4 depthProjectionMatrix =
    //  glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f); glm::mat4
    //  depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir,
    //  glm::vec3(0,1,0));

    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform (Model View Projection)
    glUniformMatrix4fv(depth_matrix_ID_, 1, GL_FALSE, &depthMVP[0][0]);

    // draw the non-indexed meshes
    for (std::shared_ptr<MeshLoader::SingleMesh> mesh : singles_meshes_) {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
        glVertexAttribPointer(
            0,        // The attribute we want to configure
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void*)0  // array buffer offset
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

        glDisableVertexAttribArray(0);
    }

    // Use our shader
    glUseProgram(programID_multi_);

    // glm::mat4 depth_view_matrix =
    //     glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1,
    //     0));
    //  or, for spot light :
    //  glm::vec3 lightPos(5, 20, 20);
    //  glm::mat4 depthProjectionMatrix =
    //  glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f); glm::mat4
    //  depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir,
    //  glm::vec3(0,1,0));

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(depth_matrix_ID_multi_, 1, GL_FALSE, &depthMVP[0][0]);

    // draw the indexed meshes
    for (std::shared_ptr<MeshLoader::MultiMesh> mesh : multi_meshes_) {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
        glVertexAttribPointer(
            0,        // The attribute we want to configure
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void*)0  // array buffer offset
        );

        // 2nd attribute buffer : transform
        glEnableVertexAttribArray(3);
        // this is 3 because reasons
        // so I don't know how to make a buffer not indexed after it is set to
        // be indexed. Because attribute 3 is indexed somewhere else there is
        // no problem here
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_model_transforms());
        glVertexAttribPointer(
            3,        // attribute
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void*)0  // array buffer offset
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

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    // Clear the screen  -  for some reason this broke things
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
