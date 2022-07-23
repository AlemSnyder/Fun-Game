#include <memory>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Terrain/terrain_mesh.hpp"
#include "controls.hpp"
#include "meshloader.hpp"
#include "shader.hpp"

class MainRenderer {
   private:
    GLuint programID_single_;    // def in class
    GLuint MatrixID_;            // def in class
    GLuint view_matrix_ID_;      // def in class
    GLuint DepthBiasID_;         // def in class
    GLuint ShadowMapID_;         // def in class
    GLuint light_direction_ID_;  // def in class

    GLuint programID_multi_;           // def in class
    GLuint MatrixID_multi_;            // def in class
    GLuint view_matrix_ID_multi_;      // def in class
    GLuint DepthBiasID_multi_;         // def in class
    GLuint ShadowMapID_multi_;         // def in class
    GLuint light_direction_ID_multi_;  // def in class

    GLuint depth_texture_;  // added to class
    // TODO all of these things should be defined somewhere else and sent to
    // this class.
    glm::vec3 light_direction_;          //! def in class
    int windowFrameWidth;                // added to class
    int windowFrameHeight;               // added to class
    glm::mat4 depth_projection_matrix_;  //! def in class
    glm::mat4 depth_view_matrix_;        //! def in class
    std::vector<std::shared_ptr<MeshLoader::SingleComplexMesh>> singles_meshes_;
    std::vector<std::shared_ptr<MeshLoader::MultiComplexMesh>> multis_meshes_;

   public:
    MainRenderer();

    ~MainRenderer();

    void add_mesh(std::shared_ptr<MeshLoader::SingleComplexMesh> mesh);

    void add_mesh(std::shared_ptr<MeshLoader::MultiComplexMesh> mesh);

    void set_depth_texture(GLuint texture_id);

    void set_window_size(int w, int h);

    void set_light_direction(glm::vec3 light_direction);

    void render(GLFWwindow* window) const;
};
