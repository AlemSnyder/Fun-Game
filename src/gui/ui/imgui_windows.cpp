#include "imgui_windows.hpp"

namespace gui {

namespace shader {
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
} // namespace shader

} // namespace gui
