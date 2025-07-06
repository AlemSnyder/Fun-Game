#include "imgui_windows.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <numbers>

namespace {
constexpr float radtodeg = std::numbers::pi / 180;
}

namespace gui {

void
display_windows::display_data(
    std::map<const shader::ProgramData, shader::Program>& programs
) {
    ImGui::Begin("Shader Programs");

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
display_windows::display_data(std::shared_ptr<scene::Helio> helio, bool& show) {
    glm::vec3 light_direction = helio->get_light_direction();

    ImGui::Begin("Environment Data", &show);

    bool& manual_light_direction = helio->control_lighting();
    ImGui::Checkbox("Manually set light direction", &manual_light_direction);

    static float theta_phi[2];

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
display_windows::display_data(const gui::Scene& scene, bool& show) {
    ImGui::Begin("Render Time Data", &show);

    ImGui::Text("Queue Time: %ld", static_cast<int64_t>(scene.get_queue_time().count()));
    ImGui::Text("Update Time: %ld", static_cast<int64_t>(scene.get_update_time().count()));
    ImGui::Text("Shadow Render Time: %ld", static_cast<int64_t>(scene.get_shadow_render_time().count()));
    ImGui::Text("Background Render Time: %ld", static_cast<int64_t>(scene.get_background_render_time().count()));
    ImGui::Text("Mid Ground Render Time: %ld", static_cast<int64_t>(scene.get_mid_ground_render_time().count()));
    ImGui::Text("Foreground Render Time: %ld", static_cast<int64_t>(scene.get_foreground_render_time().count()));

    ImGui::End();

}


} // namespace gui
