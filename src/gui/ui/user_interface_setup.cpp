#include "user_interface_setup.hpp"

#include "../the_buttons/bordered_widget.hpp"
#include "../the_buttons/bordered_window.hpp"
#include "../the_buttons/user_interface.hpp"
#include "gui/render/structures/window_texture.hpp"
#include "util/files.hpp"
#include "util/png_image.hpp"
#include "../the_buttons/button_widget.hpp"

namespace gui {

void
setup(the_buttons::UserInterface& user_interface) {
    auto texture_data_1 = files::read_json_from_file<render::window_texture_data_t>(
        files::get_resources_path() / "textures" / "GenericBorder.json"
    );

    if (!texture_data_1) {
        LOG_ERROR(logging::file_io_logger, "Failed to load texture");
        return;
    }

    auto image_result = image::read_image(
        files::get_resources_path() / "textures" / texture_data_1->texture_file
    );
    if (!image_result.has_value()) {
        LOG_ERROR(logging::file_io_logger, "Error Code {}", image_result.error());
        return;
    }
    std::shared_ptr<util::image::Image> image = image_result.value();

    std::shared_ptr<the_buttons::BorderedWindow> a_window =
        std::make_shared<the_buttons::BorderedWindow>(
            std::make_shared<render::WindowTexture>(image, texture_data_1.value()),

            glm::ivec2(70, 70), glm::ivec2(230, 230)
        );

    std::shared_ptr<the_buttons::BorderedWindow> a_second_window =
        std::make_shared<the_buttons::BorderedWindow>(
            std::make_shared<render::WindowTexture>(image, texture_data_1.value()),

            glm::ivec2(200, 200), glm::ivec2(400, 600)
        );

    auto texture_data_2 = files::read_json_from_file<render::window_texture_data_t>(
        files::get_resources_path() / "textures" / "GenericBorder_2.json"
    );

    if (!texture_data_2) {
        LOG_ERROR(logging::file_io_logger, "Failed to load texture");
        return;
    }

    auto image_result_2 = image::read_image(
        files::get_resources_path() / "textures" / texture_data_2->texture_file
    );
    if (!image_result_2.has_value()) {
        LOG_ERROR(logging::file_io_logger, "Error Code {}", image_result_2.error());
        return;
    }
    std::shared_ptr<util::image::Image> image_2 = image_result_2.value();

        
    std::shared_ptr<the_buttons::BorderedWidget> a_widget =
        a_second_window->make<the_buttons::BorderedWidget>(
            std::make_shared<render::WindowTexture>(image_2, texture_data_2.value()),

            glm::ivec2(20, 20), glm::ivec2(180, 300)
        );



    auto texture_data_3 = files::read_json_from_file<render::window_texture_data_t>(
        files::get_resources_path() / "textures" / "GenericButton.json"
    );

    if (!texture_data_3) {
        LOG_ERROR(logging::file_io_logger, "Failed to load texture");
        return;
    }

    auto image_result_3 = image::read_image(
        files::get_resources_path() / "textures" / texture_data_3->texture_file
    );
    if (!image_result_3.has_value()) {
        LOG_ERROR(logging::file_io_logger, "Error Code {}", image_result_3.error());
        return;
    }
    std::shared_ptr<util::image::Image> image_3 = image_result_3.value();


    auto a_button =
        a_widget->make<the_buttons::ButtonWidget>(
            std::make_shared<render::WindowTexture>(image_3, texture_data_3.value()),

            glm::ivec2(20, 20), glm::ivec2(140, 100),
            std::function<void()>([](){
                LOG_INFO(logging::main_logger, "Button Was Pressed");
            })
        );


    user_interface.add(a_window);
    user_interface.add(a_second_window);

    // window_pipeline->data.push_back(scene.a_window.get());

    return;
}

} // namespace gui
