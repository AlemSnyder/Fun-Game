

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
    frame_texture_uniform_(std::make_shared<render::TextureUniform>(
        gpu_data::GPUArayType::UNSIGNED_INT_SAMPLER_2D, 0
    )),
    border_sizes_(std::make_shared<render::FrameBorderSizeUniform>()),
    side_lengths_(std::make_shared<render::FrameSideLengthsUniform>()),
    inner_pattern_size_(std::make_shared<render::InnerPatternSizeUniform>()),
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
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    // uniforms
    // stars_program.set_uniform(matrix_view_projection_uniform, "MVP");
    window_render_program.set_uniform(frame_size_uniform_, "frame_size");
    window_render_program.set_uniform(ui_scale_uniform_, "ui_scale");
    window_render_program.set_uniform(frame_texture_uniform_, "window_texture");
    window_render_program.set_uniform(border_sizes_, "border_size");
    window_render_program.set_uniform(side_lengths_, "side_lengths");
    window_render_program.set_uniform(inner_pattern_size_, "inner_pattern_size");
    window_render_program.set_uniform(texture_regions_, "positions[0]");

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
        render_frame(*frame, 0, 0);
    }
}

void
UserInterface::render_frame(
    const Frame& frame, screen_size_t x_frame_position, screen_size_t y_frame_position
) const {
    if (!frame.do_render()) {
        return;
    }

    border_sizes_->set_border_size(glm::ivec4(5,5,5,5));
    side_lengths_->set_side_lengths(glm::ivec4(1,1,1,1));
    inner_pattern_size_->set_inner_pattern_size(glm::ivec2(1,1));
    texture_regions_->set_texture_regions({glm::ivec2(0, 0), glm::ivec2(5, 0), glm::ivec2(6, 0),
                                           glm::ivec2(0, 5), glm::ivec2(5, 5), glm::ivec2(6, 5),
                                           glm::ivec2(0, 6), glm::ivec2(5, 6), glm::ivec2(6, 6)});

    const auto bounding_box = frame.get_bounding_box();
    // add offset
    frame_size_uniform_->set_frame_size(
        glm::ivec2(bounding_box[2] - bounding_box[0], bounding_box[3] - bounding_box[1])
    );
    window_pipeline_->render(
        bounding_box[0] + x_frame_position, bounding_box[1] + y_frame_position,
        bounding_box[2] - bounding_box[0], bounding_box[3] - bounding_box[1], 0,
        static_cast<const gpu_data::GPUData*>(&frame)
    );

    for (const auto& frame_child : frame) {
        render_frame(
            *frame_child, frame_child->get_x_position() + x_frame_position,
            frame_child->get_y_position() + y_frame_position
        );
    }
}

std::pair<std::weak_ptr<const Frame>, std::weak_ptr<const Frame>>
UserInterface::get_frame(screen_size_t mouse_position_x, screen_size_t mouse_position_y)
    const {
    // iterate from back to front
    auto frame_outer = frames_.end();
    // might be able to do this with control flow
    bool found_frame_at_mouse_position = false;
    do {
        frame_outer--;
        if ((*frame_outer)->is_interior(mouse_position_x, mouse_position_y)) {
            found_frame_at_mouse_position = true;
            break;
        }
    } while (frame_outer != frames_.begin());

    if (!found_frame_at_mouse_position) {
        return {};
    }

    auto x_offset = (*frame_outer)->get_x_position();
    auto y_offset = (*frame_outer)->get_y_position();

    std::weak_ptr<const Frame> new_frame_outer = *frame_outer;
    std::weak_ptr<const Frame> frame_inner =
        (*frame_outer)
            ->get_child_at_position(
                mouse_position_x - x_offset, mouse_position_y - y_offset
            );

    // if there are no children then set to parent.
    if (frame_inner.expired()) {
        frame_inner = new_frame_outer;
    }

    return std::make_pair<std::weak_ptr<const Frame>, std::weak_ptr<const Frame>>(
        std::move(new_frame_outer), std::move(frame_inner)
    );
}

void
UserInterface::reselect_frame(GLFWwindow* window) {
    // forward to
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    screen_size_t height;
    glfwGetFramebufferSize(window, nullptr, &height);

    auto selected_frames = get_frame(screen_size_t(floor(xpos)), screen_size_t(height - floor(ypos)));

    if (std::shared_ptr<Frame> outer_frame = selected_frames.first.lock()) {
        // move to back
        if (outer_frame != frames_.back() && !outer_frame->is_fixed()) {
            frames_.remove(outer_frame);
            frames_.push_back(outer_frame);
        }
    }

    if (std::shared_ptr<Frame> inner_frame = selected_frames.second.lock()) {
        selected_frame_ = inner_frame;
    } else {
        selected_frame_ = nullptr;
    }
}

void
UserInterface::handle_key_event_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key,
    [[maybe_unused]] int scancode, [[maybe_unused]] int action,
    [[maybe_unused]] int mods
) {
    if (selected_frame_) {
        selected_frame_->handle_key_event_input(window, key, scancode, action, mods);
    }
}

void
UserInterface::handle_text_input_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint
) {
    if (selected_frame_) {
        selected_frame_->handle_text_input_input(window, codepoint);
    }
}

void
UserInterface::handle_mouse_event_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xpos,
    [[maybe_unused]] double ypos
) {
    if (selected_frame_) {
        selected_frame_->handle_mouse_event_input(window, xpos, ypos);
    }
}

void
UserInterface::handle_mouse_enter_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int entered
) {} // don't bother

void
UserInterface::handle_mouse_button_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int button,
    [[maybe_unused]] int action, [[maybe_unused]] int mods
) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        reselect_frame(window);
    }
    if (selected_frame_) {
        selected_frame_->handle_mouse_button_input(window, button, action, mods);
    }
}

void
UserInterface::handle_mouse_scroll_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset,
    [[maybe_unused]] double yoffset
) {
    // possibly forward
    if (selected_frame_) {
        selected_frame_->handle_mouse_scroll_input(window, xoffset, yoffset);
    }
    // forward to top
    // selected position then forward
}

void
UserInterface::handle_joystick_input(
    [[maybe_unused]] int jid, [[maybe_unused]] int event
) {}

void
UserInterface::handle_file_drop_input(
    [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int count,
    [[maybe_unused]] const char** paths
) {
    // drop onto mouse position
    reselect_frame(window);
    if (selected_frame_) {
        selected_frame_->handle_file_drop_input(window, count, paths);
    }
}

void
UserInterface::handle_pooled_inputs([[maybe_unused]] GLFWwindow* window) {
    if (selected_frame_) {
        selected_frame_->handle_pooled_inputs(window);
    }
}

void
UserInterface::setup([[maybe_unused]] GLFWwindow* window) {}

void
UserInterface::cleanup([[maybe_unused]] GLFWwindow* window) {}

} // namespace the_buttons

} // namespace gui
