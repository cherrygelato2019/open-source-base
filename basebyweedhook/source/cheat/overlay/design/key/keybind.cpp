#include "keybind.h"

void Keybind(int* keybind_ptr, const std::string& label, bool* hotkey_activation, std::string unique, int offset) {
    if (!label.empty() && label.find("##") == std::string::npos) {
        ImGui::Text(label.c_str());
        ImGui::SameLine();
    }

    unique = "##" + unique;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 153));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    std::string button_label = "[" + std::string(keys::key_names[*keybind_ptr]) + "]" + unique;
    std::string popup_label = "##popup" + label;

    if (!(*hotkey_activation)) {
        ImGui::Dummy(ImVec2(0, 0));

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(button_label.c_str()).x - ImGui::GetStyle().FramePadding.x * 2 + ImGui::CalcTextSize(unique.c_str()).x - offset);

        if (ImGui::Button(button_label.c_str())) {
            *hotkey_activation = true;
        }

        if (ImGui::BeginPopupContextItem(popup_label.c_str())) {
            if (ImGui::MenuItem("Remove Key")) {
                *keybind_ptr = 0x0;
            }

            ImGui::EndPopup();
        }
    }
    else {
        std::string empty = "[-]" + unique;
        ImGui::Dummy(ImVec2(0, 0));
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(empty.c_str()).x - ImGui::GetStyle().FramePadding.x * 2 + ImGui::CalcTextSize(unique.c_str()).x - offset);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 153));
        ImGui::Button(empty.c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        for (auto& key : keys::key_codes) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (key == 0x1) {
                    Sleep(50);
                    if (GetAsyncKeyState(key) & 0x8000) {
                        *keybind_ptr = key;
                        *hotkey_activation = false;
                    }
                    continue;
                }
                *keybind_ptr = key;
                *hotkey_activation = false;
                break;
            }
        }
    }

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
}
