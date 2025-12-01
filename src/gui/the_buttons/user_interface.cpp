

#include "user_interface.hpp"

#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/uniform_types.hpp"
#include "manifest/object_handler.hpp"

namespace gui {
namespace the_buttons {

UserInterface::UserInterface(shader::ShaderHandler& shader_handler, uint8_t ui_scale) :
    frame_size_uniform_(std::make_shared<render::FrameSizeUniform>()),
    ui_scale_uniform_(std::make_shared<render::UIScaleUniform>(ui_scale)),
    frame_texture_uniform_(
        std::make_shared<render::TextureUniform>(gpu_data::GPUArayType::SAMPLER_2D, 0)
    ),
    texture_regions_(std::make_shared<render::TextureRegionsUniform>()) {
    shader::Program& window_render_program = shader_handler.load_program(
        "Windows", files::get_resources_path() / "shaders" / "overlay" / "Widget.vert",
        files::get_resources_path() / "shaders" / "overlay" / "FramedWindow.frag"
    );

    // auto image_result = image::read_image(files::get_resources_path() / "textures" /
    // "GenericBorder.png"); if (!image_result.has_value()) {
    //     LOG_ERROR(logging::file_io_logger, "Error Code {}", image_result.error());
    //     return;
    // }
    // std::shared_ptr<util::image::Image> image = image_result.value();

    // gpu_data::Texture2D border_texture(image, gui::gpu_data::TextureSettings{},
    // false);

    // frame_texture_uniform_ = std::make_shared<>();

    // Overwrites anything that was there before
    std::function<void()> render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    // uniforms
    // stars_program.set_uniform(matrix_view_projection_uniform, "MVP");
    window_render_program.set_uniform(frame_size_uniform_, "frame_size");
    window_render_program.set_uniform(ui_scale_uniform_, "ui_scale");
    window_render_program.set_uniform(frame_texture_uniform_, "texture");
    window_render_program.set_uniform(texture_regions_, "texture_locations");

    // windows
    window_pipeline_ = std::make_shared<shader::ShaderProgram_Windows>(
        window_render_program, render_setup
    );
}

void
UserInterface::update(screen_size_t width, screen_size_t height) {
    // cascade update frames using width and height

    FrameBufferHandler::instance().bind_fbo(0); // the screen

    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& frame : frames_) {
        if (!frame->do_render()) {
            continue;
        }
        // frame->update_position();

        texture_regions_->set_texture_regions({0, 0, 5, 5, 0, 5, 5, 1, 0, 6, 5, 1,
                                               5, 0, 1, 5, 5, 5, 1, 1, 6, 5, 5, 1,
                                               6, 0, 5, 5, 6, 5, 5, 1, 6, 6, 5, 5});

        const auto bounding_box = frame->get_bounding_box();
        frame_size_uniform_->set_frame_size(glm::ivec2(
            bounding_box[2] - bounding_box[0], bounding_box[3] - bounding_box[1]
        ));
        window_pipeline_->render(
            bounding_box[0], bounding_box[1], bounding_box[2], bounding_box[3], 0,
            frame.get()
        );
    }
}

} // namespace the_buttons

} // namespace gui
