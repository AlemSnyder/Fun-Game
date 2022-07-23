#include <memory>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "meshloader.hpp"
#include "shader.hpp"

class ShadowMap {
   private:
    GLuint programID_;              // def in class
    GLuint depth_matrix_ID_;        // def in class
    GLuint programID_multi_;        // def in class
    GLuint depth_matrix_ID_multi_;  // def in class
    GLuint depthTexture;            // def in class
    GLuint FramebufferName; // def in class
    // ------ the below are added to the class ------
    glm::vec3 light_direction_;          // added to class
    int windowFrameWidth;                // added to class
    int windowFrameHeight;               // added to class
    glm::mat4 depth_projection_matrix_;  // added to class
    std::vector<std::shared_ptr<MeshLoader::SingleMesh>> singles_meshes_;
    std::vector<std::shared_ptr<MeshLoader::MultiMesh>> multi_meshes_;

   public:
    ShadowMap(int w, int h);

    void add_mesh(std::shared_ptr<MeshLoader::SingleMesh> mesh);

    void add_mesh(std::shared_ptr<MeshLoader::MultiMesh> mesh);

    GLuint& get_depth_texture();

    GLuint& get_frame_buffer();

    void set_light_direction(glm::vec3 light_direction);

    void set_depth_projection_matrix(glm::mat4 depth_projection_matrix);

    void render_shadow_depth_buffer() const;
};