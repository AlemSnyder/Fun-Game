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
    GLuint FramebufferName;
    // TODO all of these things should be defined somewhere else and sent to
    // this class.
    glm::vec3 light_direction_;          //! def in class
    int windowFrameWidth;                //! def in class
    int windowFrameHeight;               //! def in class
    glm::mat4 depth_projection_matrix_;  //! def in class
    std::vector<std::shared_ptr<MeshLoader::SingleMesh>> singles_meshes_;
    std::vector<std::shared_ptr<MeshLoader::MultiMesh>> multi_meshes_;

   public:
    ShadowMap();

    void add_mesh(std::shared_ptr<MeshLoader::SingleMesh> mesh);

    void add_mesh(std::shared_ptr<MeshLoader::MultiMesh> mesh);

    GLuint& get_depth_texture();

    GLuint& get_frame_buffer();

    void render_shadow_depth_buffer() const;
};