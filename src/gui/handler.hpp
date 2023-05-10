// handles fbo, and other things if necessary

#include <GL/glew.h>

namespace gui{

class FrameBufferHandler {
 private:
    static GLuint fbo_name;

 public:
    static inline GLuint
    get_current_fbo() {
        return fbo_name;
    }

    static void
    bind_fbo(GLuint new_fbo);
};

}
