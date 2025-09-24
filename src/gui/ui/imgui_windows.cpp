#include "imgui_windows.hpp"

#include "gui/render/gl_enums.hpp"
#include "imgui.h"
#include "manifest/object_handler.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <numbers>

namespace {
constexpr float radtodeg = std::numbers::pi / 180;
}

namespace gui {

namespace display_windows {

void
display_data(
    std::map<const shader::ProgramData, shader::Program>& programs, bool& show
) {
    ImGui::Begin("Shader Programs", &show);

    if (ImGui::BeginTable("table_sorting", 4, 0, ImVec2(0.0f, 24 * 15), 0.0f)) {
        // Declare columns
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("Quantity");
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        for (auto& program_pair : programs) {
            auto& program = program_pair.second;
            // Display a data item
            ImGui::PushID(program.get_program_ID());
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", program.get_program_ID());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(program.get_name().c_str());
            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Reload")) {
                program.reload();
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", program.get_status_string().first.c_str());
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
                ImGui::SetTooltip("%s", program.get_status_string().second.c_str());
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void
display_data(std::shared_ptr<scene::Helio> helio, bool& show) {
    glm::vec3 light_direction = helio->get_light_direction();

    ImGui::Begin("Scene Data", &show);

    bool& manual_light_direction = helio->control_lighting();
    ImGui::Checkbox("Manually set light direction", &manual_light_direction);

    static float theta_phi[2] = {30.0, 0.0};

    if (manual_light_direction) {
        ImGui::DragFloat2("Light Direction", theta_phi);

        helio->control_light_direction() = glm::vec3(
            cos(theta_phi[0] * radtodeg) * sin(theta_phi[1] * radtodeg),
            cos(theta_phi[0] * radtodeg) * cos(theta_phi[1] * radtodeg),
            sin(theta_phi[0] * radtodeg)
        );

    } else {
        ImGui::Text("Sun angle %.3f", helio->sun_angle);
        ImGui::Text("Earth angle %.3f", helio->earth_angle);
        ImGui::Text("Total angle %.3f", helio->total_angle);
    }
    glm::vec3 color = helio->get_specular_light();

    ImGui::TextColored({color.r, color.g, color.b, 1}, "■");

    ImGui::Text(
        "Light Direction <%.3f, %.3f, %.3f>", light_direction.x, light_direction.y,
        light_direction.z
    );

    ImGui::End();
}

void
display_data(const manifest::ObjectHandler& object_handler, bool& show) {
    ImGui::Begin("Shader Objects", &show);

    if (ImGui::BeginTable("table_sorting", 3, 0, ImVec2(0.0f, 24 * 15), 0.0f)) {
        // Declare columns
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        //        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("Number of models");
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        for (auto& [id, object] : object_handler) {
            // Display a data item
            ImGui::PushID(std::hash<std::string>{}(id)
            ); // maybe not grate to call hashes like this
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(id.c_str()); // ID
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(object->get_name().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%ld", object->num_models()); // number of models
            // include info on models
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void
display_data(
    Scene& scene, bool& show, screen_size_t window_width, screen_size_t window_height
) {
    ImGui::Begin("Scene Data", &show);

    static int xy[2];
    ImGui::DragInt2("Sample Position", xy, 1.0, 0, 2000);

    const auto& mid_ground = scene.get_mid_ground_framebuffer();
    std::shared_ptr<util::image::Image> read_data =
        mid_ground.read_data(xy[0], xy[1], 1, 1);
    auto float_data =
        std::dynamic_pointer_cast<util::image::FloatMonochromeImage>(read_data);

    FrameBufferHandler& fbh = FrameBufferHandler::instance();
    fbh.bind_fbo(0);

    ImGui::Text("Texture ID %i", mid_ground.get_depth_buffer()->value());
    ImGui::Image(
        reinterpret_cast<ImTextureID>(mid_ground.get_depth_buffer()->value()),
        ImVec2(
            mid_ground.get_width() / 8,
            mid_ground.get_width() / 8 * window_height / window_width
        ),
        ImVec2(0, float(window_width) / float(mid_ground.get_width())),
        ImVec2(float(window_height) / float(mid_ground.get_height()), 0)
    );

    ImGui::Text("Texture ID %i", mid_ground.get_texture(0)->value());
    ImGui::Image(
        reinterpret_cast<ImTextureID>(mid_ground.get_texture(0)->value()),
        ImVec2(
            mid_ground.get_width() / 8,
            mid_ground.get_width() / 8 * window_height / window_width
        ),
        ImVec2(0, float(window_height) / float(mid_ground.get_height())),
        ImVec2(float(window_width) / float(mid_ground.get_width()), 0)
    );

    if (!float_data) {
        ImGui::Text("Cannot display depth. Something went wrong.");

    } else {
        float depth_value = float_data->get_data(0, 0);

        ImGui::Text("Depth 32 %f", depth_value);

        const auto controls = scene.get_inputs();
        glm::mat4 inverse_view_projection = controls->get_inverse_view_projection();
        // For reference: https://www.songho.ca/opengl/gl_viewport.html
        glm::vec4 screen_position(
            static_cast<float>(xy[0]) / static_cast<float>(controls->get_width()) * 2
                - 1,
            static_cast<float>(xy[1]) / static_cast<float>(controls->get_height()) * 2
                - 1,
            depth_value * 2 - 1, 1
        );

        glm::vec4 world_position = inverse_view_projection * screen_position;
        world_position = world_position / world_position.w;

        ImGui::Text(
            "Screen position [%f, %f, %f, %f]", screen_position.x, screen_position.y,
            screen_position.z, screen_position.w
        );

        ImGui::Text(
            "World position [%f, %f, %f, %f]", world_position.x, world_position.y,
            world_position.z, world_position.w
        );
    }

    ImGui::End();
}

} // namespace display_windows

} // namespace gui
