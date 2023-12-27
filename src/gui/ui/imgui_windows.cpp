#include "imgui_windows.hpp"

namespace gui {

namespace shader {
void
display_windows::display_data(std::map<const shader::ProgramData, shader::Program>& programs
) {
    ImGui::Begin("Shader Programs");

    if (ImGui::BeginTable("table_sorting", 4, 0, ImVec2(0.0f, 24 * 15), 0.0f)) {
        // Declare columns
        // We use the "user_id" parameter of TableSetupColumn() to specify a user id
        // that will be stored in the sort specifications. This is so our sort function
        // can identify a column given our own identifier. We could also identify them
        // based on their index! Demonstrate using a mixture of flags among available
        // sort-related flags:
        // - ImGuiTableColumnFlags_DefaultSort
        // - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending /
        // ImGuiTableColumnFlags_NoSortDescending
        // - ImGuiTableColumnFlags_PreferSortAscending /
        // ImGuiTableColumnFlags_PreferSortDescending
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Action");
        ImGui::TableSetupColumn("Quantity");
        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        // Sort our data if sort specs have been changed!
        // if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
        //    if (sort_specs->SpecsDirty) {
        //        MyItem::SortWithSortSpecs(sort_specs, items.Data, items.Size);
        //        sort_specs->SpecsDirty = false;
        //    }

        for (auto& program_pair : programs) {
            auto& program = program_pair.second;
            // Display a data item
            ImGui::PushID(program.get_program_ID());
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", program.get_program_ID());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("name");
            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Reload")) {
                program.reload();
            }
            ImGui::TableNextColumn();
            ImGui::Text(program.get_status_string().first.c_str());
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
} // namespace shader

} // namespace gui
