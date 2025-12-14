#include "user_interface_setup.hpp"

#include "../the_buttons/bordered_window.hpp"
#include "../the_buttons/user_interface.hpp"
#include "util/files.hpp"
#include "util/png_image.hpp"

namespace gui {

void
setup(the_buttons::UserInterface& user_interface) {
    auto image_result = image::read_image(
        files::get_resources_path() / "textures" / "GenericBorder.png"
    );
    if (!image_result.has_value()) {
        LOG_ERROR(logging::file_io_logger, "Error Code {}", image_result.error());
        return;
    }
    std::shared_ptr<util::image::Image> image = image_result.value();

    std::shared_ptr<the_buttons::BorderedWindow> a_window =
        std::make_shared<the_buttons::BorderedWindow>(
            std::make_shared<render::WindowTexture>(image),

            glm::ivec2(70, 70), glm::ivec2(230, 230)
        );

    std::shared_ptr<the_buttons::BorderedWindow> a_second_window =
        std::make_shared<the_buttons::BorderedWindow>(
            std::make_shared<render::WindowTexture>(image),

            glm::ivec2(200, 200), glm::ivec2(400, 600)
        );


    user_interface.add(a_window);
    user_interface.add(a_second_window);


    // window_pipeline->data.push_back(scene.a_window.get());

    return;
}

} // namespace gui
