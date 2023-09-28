#ifndef BADNETEMULATOR_NETEMULATOR_H
#define BADNETEMULATOR_NETEMULATOR_H

#include <imgui.h>
#include <fmt/format.h>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <ifaddrs.h>

struct NetemParams {
    int duaration;
    int delay = 0;
    float packet_loss = 0;
    float packet_duplicate = 0;
    float packet_corrupt = 0;
};

struct TbfParams {
    unsigned int rate;
    unsigned int burst;
    unsigned int limit;
};

struct TestParams {
    NetemParams netem;
    TbfParams tbf;
};

class NetEmulator {
public:
    NetEmulator() {
        get_interfaces();
    }

    void get_interfaces() {
        struct ifaddrs *addrs, *tmp;

        getifaddrs(&addrs);
        tmp = addrs;

        while (tmp) {
            if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
                interfaces.emplace_back(tmp->ifa_name);
            tmp = tmp->ifa_next;
        }

        freeifaddrs(addrs);
    }

    void draw() {
        auto io = ImGui::GetIO();
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - 50,
                                        io.DisplaySize.y - 50));
        ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x - (io.DisplaySize.x - 50)) / 2,
                                       (io.DisplaySize.y - (io.DisplaySize.y - 50)) / 2), ImGuiCond_Always, {0, 0});
        if (ImGui::Begin("_NetEmulator_", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration)) {
            if (ImGui::BeginChild("_ControlMenu_", ImVec2(0, 90))) {
                const char *interfaces_c[20];
                for (int i = 0; i < 20 && i < interfaces.size(); ++i) {
                    interfaces_c[i] = interfaces[i].c_str();
                }
                static int item_current = 0;
                if (ImGui::Combo("net-interface", &item_current, interfaces_c, 2)) {
                    interface = interfaces[item_current];
                }
                ImGui::Spacing();
                if (ImGui::Button("Add stage")) {
                    netem_params.push_back({});
                }
                ImGui::EndChild();
            }

            if (ImGui::BeginChild("_ListView_", ImVec2(0, 200))) {
                int index = 0;
                if (!is_start) {
                    for (auto &item: netem_params) {
                        ImGui::PushID(index);
                        if (ImGui::TreeNode(fmt::format("stage {}", index).c_str())) {
                            ImGui::DragInt("event duaration (seconds)", &item.duaration, 1, 0, 60 * 5);
                            ImGui::DragInt("delay (ms)", &item.delay, 1, 0, 3000);
                            ImGui::DragFloat("packet loss (%)", &item.packet_loss, 1, 0, 100);
                            ImGui::DragFloat("packet corrupt (%)", &item.packet_corrupt, 1, 0, 100);
                            ImGui::DragFloat("packet duplicate (%)", &item.packet_duplicate, 1, 0, 100);
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                        index++;
                    }
                } else {
                    for (auto &item: netem_params) {
                        ImGui::PushID(index);
                        if (index == current_stage) {
                            ImGui::Text("stage %i is running with parameters delay %i, loss %f (left %li s)", index, item.delay, item.packet_loss,
                                        item.duaration - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - current_stage_start_time.load()).count());
                        } else if (index < current_stage) {
                            ImGui::Text("stage %i done", index);
                        } else {
                            ImGui::Text("stage %i wait", index);
                        }
                        ImGui::PopID();
                        index++;
                    }
                }
                ImGui::EndChild();
            }

            if (!netem_params.empty() && ImGui::Button("Start")) {
                start();
            }
            if (ImGui::Button("Stop")) {
                stop();
                is_start = false;
                if (worker.joinable())
                    worker.join();
            }

            plot_recived_bytes();
            ImGui::Spacing();
            plot_transmitted_bytes();
            ImGui::End();
        }
    }

    void plot_recived_bytes() {
        if (interface.empty())
            return;
        static std::array<float, 40> values = {};
        static int curr = values.size();

        if (plot_update_rx < std::chrono::system_clock::now()) {
            static float r_new = recived();
            static float r_old;
            r_old = r_new;
            r_new = recived();
            auto delta = r_new - r_old;
            if (curr < values.size()) {
                values[curr--] = delta;
            } else {
                std::rotate(values.begin(), values.begin() + 1, values.end());
                values.back() = delta;
            }
            plot_update_rx = std::chrono::system_clock::now() + std::chrono::milliseconds(600);
        }

        {
            float average = 0.0f;
            for (int n = 0; n < values.size(); n++)
                average += values[n];
            average /= (float) values.size();
            float max = *std::max_element(values.begin(), values.end());
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            ImGui::PlotLines(fmt::format("rx_bytes (max:{})", max).c_str(), values.data(), values.size(), 0, overlay, 0, max, ImVec2(0, 90.0f));
        }
    }

    void plot_transmitted_bytes() {
        if (interface.empty())
            return;
        static std::array<float, 40> values = {};
        static int curr = values.size();

        if (plot_update_tx < std::chrono::system_clock::now()) {
            static float r_new = transmitted();
            static float r_old;
            r_old = r_new;
            r_new = transmitted();
            auto delta = r_new - r_old;
            if (curr < values.size()) {
                values[curr--] = delta;
            } else {
                std::rotate(values.begin(), values.begin() + 1, values.end());
                values.back() = delta;
            }
            plot_update_tx = std::chrono::system_clock::now() + std::chrono::milliseconds(600);
        }

        {
            float average = 0.0f;
            for (int n = 0; n < values.size(); n++)
                average += values[n];
            average /= (float) values.size();
            float max = *std::max_element(values.begin(), values.end());
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            ImGui::PlotLines(fmt::format("tx_bytes (max:{})", max).c_str(), values.data(), values.size(), 0, overlay, 0, max, ImVec2(0, 90.0f));
        }
    }

    float recived() {
        float recive;
        std::ifstream stream(fmt::format("/sys/class/net/{}/statistics/rx_bytes", interface));
        if (stream.is_open()) {
            std::string line;
            std::getline(stream, line);
            std::istringstream linestream(line);
            linestream >> recive;
        }
        stream.close();
        return recive;
    }

    float transmitted() {
        float transmit;
        std::ifstream stream(fmt::format("/sys/class/net/{}/statistics/tx_bytes", interface));
        if (stream.is_open()) {
            std::string line;
            std::getline(stream, line);
            std::istringstream linestream(line);
            linestream >> transmit;
        }
        stream.close();
        return transmit;
    }

    void start() {
        if (interface.empty())
            return;
        if (worker.joinable())
            worker.join();
        is_start = true;
        copy_netem_params = netem_params;
        worker = std::thread([this]() {
            int index = 0;
            for (auto &item: copy_netem_params) {
                if (!is_start)
                    return;
                auto command = fmt::format("sudo tc qdisc add dev {} root netem delay {}ms loss {}% corrupt {}% duplicate {}%", interface, item.delay,
                                           item.packet_loss, item.packet_corrupt, item.packet_duplicate);
                int result = std::system(command.c_str());
                current_stage = index++;
                current_stage_start_time = std::chrono::system_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(item.duaration));
                stop();
            }
            is_start = false;
        });
    }

    void stop() {
        int result = std::system(fmt::format("sudo tc qdisc del dev {} root", interface).c_str());
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
    }

private:
    std::chrono::time_point<std::chrono::system_clock> plot_update_rx;
    std::chrono::time_point<std::chrono::system_clock> plot_update_tx;
    std::thread worker;
    std::atomic_bool is_start = false;
    std::atomic_int current_stage = 0;
    std::atomic<std::chrono::time_point<std::chrono::system_clock>> current_stage_start_time;
    std::vector<NetemParams> netem_params;
    std::vector<NetemParams> copy_netem_params;
    std::string interface = "";
    std::vector<std::string> interfaces;
};

#endif //BADNETEMULATOR_NETEMULATOR_H
