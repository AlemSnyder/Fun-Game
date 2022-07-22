#include <memory>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"
#include "meshloader.hpp"
#include "controls.hpp"
#include "../Terrain/terrain_mesh.hpp"

class MainRenderer {
private:
    GLuint programID_single_; // def in class
    GLuint programID_multi_; //def in class
    // TODO -- need to be defined for multi --
    GLuint MatrixID_; // def in class 
    GLuint view_matrix_ID_; // def in class
    GLuint DepthBiasID_; // def in class
    GLuint ShadowMapID_; // def in class
    GLuint light_direction_ID_; // def in class
    // ---------------------------------------
    GLuint MatrixID_multi_; // def in class 
    GLuint view_matrix_ID_multi_; // def in class
    GLuint DepthBiasID_multi_; // def in class
    GLuint ShadowMapID_multi_; // def in class
    GLuint light_direction_ID_multi_; // def in class
    GLuint depth_texture_; // added to class
    // TODO all of these things should be defined somewhere else and sent to this class.
    glm::vec3 light_direction_; //! def in class
    int windowFrameWidth; //! def in class
    int windowFrameHeight; //! def in class
    glm::mat4 depth_projection_matrix_; //! def in class
    glm::mat4 depth_view_matrix_; //! def in class
    std::vector<std::shared_ptr<MeshLoader::SingleComplexMesh>> singles_meshes_;
    //std::vector<std::array<unsigned int, 5>> single_meshes_uint_;
    //TerrainMesh& terrain_mesh_;
    //std::unique_ptr<MeshLoader::SingleComplexMesh> terrain_mesh_;
    std::vector<std::shared_ptr<MeshLoader::MultiComplexMesh>> multis_meshes_;
public:

    MainRenderer(){//(std::unique_ptr<MeshLoader::SingleComplexMesh> tm) {
        //terrain_mesh_ = std::move(tm);
        programID_single_ =
            LoadShaders("../src/GUI/Shaders/ShadowMapping.vert", "../src/GUI/Shaders/ShadowMapping.frag");

        programID_multi_ =
            LoadShaders("../src/GUI/Shaders/ShadowMappingInstanced.vert", "../src/GUI/Shaders/ShadowMappingInstanced.frag");
        
        MatrixID_ = glGetUniformLocation(programID_single_, "MVP");
        view_matrix_ID_ = glGetUniformLocation(programID_single_, "V");
        DepthBiasID_ = glGetUniformLocation(programID_single_, "DepthBiasMVP");
        ShadowMapID_ = glGetUniformLocation(programID_single_, "shadowMap");
        light_direction_ID_ =
            glGetUniformLocation(programID_single_, "LightInvDirection_worldspace");

        MatrixID_multi_ = glGetUniformLocation(programID_multi_, "MVP");
        view_matrix_ID_multi_ = glGetUniformLocation(programID_multi_, "V");
        DepthBiasID_multi_ = glGetUniformLocation(programID_multi_, "DepthBiasMVP");
        ShadowMapID_multi_ = glGetUniformLocation(programID_multi_, "shadowMap");
        light_direction_ID_multi_ =
            glGetUniformLocation(programID_multi_, "LightInvDirection_worldspace");

        light_direction_ =
            glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f))// direction
            * 128.0f; // length

        // Compute the MVP matrix from the light's point of view
        depth_projection_matrix_ =
            glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);
        depth_view_matrix_ =
            glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    }

    ~MainRenderer(){
        glDeleteProgram(programID_single_);
        glDeleteProgram(programID_multi_);
    }

    void add_mesh(std::shared_ptr<MeshLoader::SingleComplexMesh> mesh){
        singles_meshes_.push_back(std::move(mesh));
    }

    void add_mesh(std::shared_ptr<MeshLoader::MultiComplexMesh> mesh){
        multis_meshes_.push_back(mesh);
    }

    void set_depth_texture(GLuint texture_id){
        depth_texture_ = texture_id;
    }
    
    void set_window_size(int w, int h){
        windowFrameHeight = h;
        windowFrameWidth = w;
    }

    void render(GLFWwindow* window) const{

        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(
            0, 0, windowFrameWidth,
            windowFrameHeight); // Render on the whole framebuffer, complete
                                // from the lower left corner to the upper right

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only
                             // front-facing triangles

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID_single_);

        // Compute the MVP matrix from the light's point of view
        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        glm::mat4 depthMVP =
            depth_projection_matrix_ * depth_view_matrix_ * depthModelMatrix;

        // Compute the MVP matrix from keyboard and mouse input
        controls::computeMatricesFromInputs(window);
        glm::mat4 projection_matrix = controls::get_projection_matrix();
        glm::mat4 view_matrix = controls::get_view_matrix();
        //glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = projection_matrix * view_matrix;// * ModelMatrix;

        glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                             0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

        glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID_, 1, GL_FALSE, &MVP[0][0]);
        //glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(DepthBiasID_, 1, GL_FALSE, &depthBiasMVP[0][0]);

        glUniform3f(light_direction_ID_, light_direction_.x, light_direction_.y, light_direction_.z);

        // Bind our texture in Texture Unit 0
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, TextureID);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        //glUniform1i(TextureID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_texture_);
        glUniform1i(ShadowMapID_, 1);

        //std::unique_ptr<MeshLoader::SingleComplexMesh>& terrain_mesh_ = singles_meshes_[0];

        for (std::shared_ptr<MeshLoader::SingleComplexMesh> mesh : singles_meshes_){

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
            glVertexAttribPointer(0,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // 2nd attribute buffer : colors
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_color_buffer());
            glVertexAttribPointer(1,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normal_buffer());
            glVertexAttribPointer(2,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_element_buffer());

            // Draw the triangles !
            glDrawElements(GL_TRIANGLES,      // mode
                        mesh->get_num_vertices(),    // count
                        GL_UNSIGNED_SHORT, // type
                        (void *)0          // element array buffer offset
            );

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);

        }

        // Use our shader
        glUseProgram(programID_multi_);

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID_multi_, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(view_matrix_ID_multi_, 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(DepthBiasID_multi_, 1, GL_FALSE, &depthBiasMVP[0][0]);

        glUniform3f(light_direction_ID_multi_, light_direction_.x, light_direction_.y, light_direction_.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_texture_);
        glUniform1i(ShadowMapID_, 1);

        for (std::shared_ptr<MeshLoader::MultiComplexMesh> mesh : multis_meshes_){
            
            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
            glVertexAttribPointer(0,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_color_buffer());
            glVertexAttribPointer(1,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // 3rd attribute buffer : normals
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normal_buffer());
            glVertexAttribPointer(2,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );

            // 4th attribute buffer : transform
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->get_model_transforms());
            glVertexAttribPointer(3,        // attribute
                                3,        // size
                                GL_FLOAT, // type
                                GL_FALSE, // normalized?
                                0,        // stride
                                (void *)0 // array buffer offset
            );
            glVertexAttribDivisor(3,1);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_element_buffer());

            // Draw the triangles !
            glDrawElementsInstanced(GL_TRIANGLES,      // mode
                        mesh->get_num_vertices(),    // count
                        GL_UNSIGNED_SHORT, // type
                        (void *)0,          // element array buffer offset
                        mesh->get_num_models()
            );
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
    };
};
