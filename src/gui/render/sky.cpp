#include "sky.hpp"

#include "../../entity/terrain_mesh.hpp"
#include "../../util/files.hpp"
#include "../controls.hpp"
#include "../meshloader.hpp"
#include "../shader.hpp"
#include "../sky_data.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

namespace sky {

SkyRenderer::SkyRenderer(SkyData sky_data, ScreenData screen_data) :
    sky_data_(sky_data), screen_data_(screen_data) {
    // get the program
    programID_ = load_shaders(
        files::get_resources_path() / "shaders" / "Sky.vert",
        files::get_resources_path() / "shaders" / "Sky.frag"
    );
    // ---- set uniforms ----
    matrix_ID_ = glGetUniformLocation(programID_, "MVP");
    view_matrix_ID_ = glGetUniformLocation(programID_, "V");
    star_texture_ = glGetUniformLocation(programID_, "stars");
    start_num_ID_ = glGetUniformLocation(programID_, "number_of_stars");


    // The quad's FBO. Used only for visualizing the shadow map.
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexbuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data,
        GL_STATIC_DRAW
    );

}

SkyRenderer::~SkyRenderer() {
    glDeleteProgram(programID_);
}

void
SkyRenderer::render(GLFWwindow* window) const {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Use our shader
    glUseProgram(programID_);


    // first attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0,
                    // but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting
    // at 0 -> 2 triangles
    glDisableVertexAttribArray(0);

/*


    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // get he window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    // Cull back-facing triangles -> draw only front-facing triangles

    // glCullFace(GL_BACK);

    // Clear the screen
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID_);

    // Compute the MVP matrix from keyboard and mouse input
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_ID_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &view_matrix[0][0]);

    glUniform1i(start_num_ID_, sky_data_.get_num_stars());

    // Bind Shadow Texture to Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, sky_data_.get_stars_texture());
    glUniform1i(star_texture_, 1);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, screen_data_.get_vertex_buffer());
    glVertexAttribPointer(
        0,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_data_.get_element_buffer());

    // Draw the triangles !
    glDrawElements(
        GL_TRIANGLE_STRIP,               // mode
        screen_data_.get_num_vertices(), // count
        GL_UNSIGNED_SHORT,               // type
        (void*)0                         // element array buffer offset
    );
*/
    glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);
    // glDisableVertexAttribArray(2);

    // Use our shader
    glUseProgram(programID_);
}

} // namespace sky

} // namespace gui
