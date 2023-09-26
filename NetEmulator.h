#ifndef BADNETEMULATOR_NETEMULATOR_H
#define BADNETEMULATOR_NETEMULATOR_H

#include <imgui.h>
#include <fmt/format.h>
#include <vector>

struct NetemParams {
    int duaration;
    int delay = 0;
    float packet_loss = 0;
    float packet_duplicate;
    float packet_corrupt;
};

struct TbfParams {
    unsigned int rate;
    unsigned int burst;
    unsigned int limit;
};

class NetEmulator {
public:
    void draw() {
        auto io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 50,
                                        io.DisplaySize.y - 50));
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - (io.DisplaySize.x - 50)) / 2,
                                       (io.DisplaySize.y - (io.DisplaySize.y - 50)) / 2), ImGuiCond_Always, {0, 0});
        if (ImGui::Begin("_NetEmulator_", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration)) {

            if (ImGui::BeginChild("_ControlMenu_", ImVec2(0, 30))) {
                if (ImGui::Button("Add stage")) {
                    netem_params.push_back({});
                }
                ImGui::EndChild();
            }

            if (ImGui::BeginChild("_ListView_", ImVec2(0, 200))) {
                int index = 0;
                for (auto &item: netem_params) {
                    ImGui::PushID(index);
                    if (ImGui::TreeNode(fmt::format("stage {}", index).c_str())) {
                        ImGui::DragInt("event duaration (seconds)", &item.duaration, 1, 0, 60 * 5);
                        ImGui::DragInt("delay", &item.delay, 1, 0, 3000);
                        ImGui::DragFloat("packet loss (%)", &item.packet_loss, 1, 0, 100);
                        ImGui::DragFloat("packet corrupt (%)", &item.packet_corrupt, 1, 0, 100);
                        ImGui::DragFloat("packet duplicate (%)", &item.packet_corrupt, 1, 0, 100);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                    index++;
                }
                ImGui::EndChild();
            }

            if (ImGui::Button("Start")) {
                start();
            }
            if (ImGui::Button("Stop")) {
                stop();
            }
            ImGui::End();
        }
    }

    void start() {
        for (auto &item: netem_params) {

        }
        //auto command = fmt::format("sudo tc qdisc add dev eno1 root netem delay {}ms loss {}%", delay, packet_loss);
        //int result = std::system(command.c_str());
        //fmt::print("test {}\n",result);
    }

    void stop() {
        int result = std::system("sudo tc qdisc del dev eno1 root netem");
        fmt::print("stop {}\n", result);
    }

    static void setTheme() {
        ImGuiStyle &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        style.GrabRounding = style.FrameRounding = 2.3f;
//    ImGuiStyle* style = &ImGui::GetStyle();
//    style->WindowPadding = ImVec2(10, 9);
//    style->WindowRounding = 5.0f;
//    style->FramePadding = ImVec2(3, 5);
//    style->FrameRounding = 3.0f;
//    style->ItemSpacing = ImVec2(10, 8);
//    style->ItemInnerSpacing = ImVec2(8, 6);
//    style->IndentSpacing = 25.0f;
//    style->ScrollbarSize = 12.0f;
//    style->ScrollbarRounding = 9.0f;
//    style->GrabMinSize = 6.0f;
//    style->GrabRounding = 3.0f;
//    style->AntiAliasedLinesUseTex = false;
//
//    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
//    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
//    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
//    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
//    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
//    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
//    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.324, 0.154, 0.415, 1.00f);
//    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.215f, 0.192f, 0.337f, 1.00f);
//    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.117f, 0.074f, 0.207f, 1.00f);
//    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.117f, 0.074f, 0.207f, 1.00f);
//    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.127f, 0.08f, 0.3f, 1.00f);
//    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.211f, 0.133f, 0.368f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
//    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
//    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.831f, 0.286f, 1.f, 0.58f);
//    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
//    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
//    style->Colors[ImGuiCol_Button] = ImVec4(0.263, 0.221, 0.485, 1.00f);
//    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.1f, 0.3f, 1.00f);
//    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.11f, 0.45f, 1.00f);
//    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.188f, 0.043f, 0.274f, 1.00f);
//    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.325f, 0.011f, 0.447f, 1.00f);
//    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.266f, 0.098f, 0.87f, 0.00f);
//    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.172f, 0.f, 1.f, 1.00f);
//    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
//    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
//    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
//    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
//    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
//    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
//    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.83f);
    }

private:
    std::vector<NetemParams> netem_params;
};

#endif //BADNETEMULATOR_NETEMULATOR_H
