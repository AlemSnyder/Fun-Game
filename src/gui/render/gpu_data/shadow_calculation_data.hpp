#include "frame_buffer_shadow_map.hpp"
#include "screen_data.hpp"

namespace gui {

namespace gpu_data {

class ShadowCalculationData : public virtual ScreenData {
 private:
    GLuint back_shadow_map_;
    GLuint front_shadow_map_;

 public:
    ShadowCalculationData(
        GLuint back_shadow_map, GLuint front_shadow_map
    ) :
        back_shadow_map_(back_shadow_map),
        front_shadow_map_(front_shadow_map) {}

    inline virtual void bind() const override {
        ScreenData::bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, back_shadow_map_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, front_shadow_map_);
    }
};

} // namespace gpu_data

} // namespace gui
