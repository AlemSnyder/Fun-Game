#include "imgui_windows.hpp"

#include <glm/gtc/type_ptr.hpp>

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
            ImGui::Text(program.get_status_string().first.c_str());
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
                ImGui::SetTooltip(program.get_status_string().second.c_str());
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

    ImGui::Begin("Scene Data", &show);

    bool& manual_light_direction = helio->control_lighting();
    ImGui::Checkbox("Manually set light direction", &manual_light_direction);

    if (manual_light_direction) {
        ImGui::DragFloat3("Light Direction", glm::value_ptr(helio->control_light_direction()));
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

} // namespace gui
