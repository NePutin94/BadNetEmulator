#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include "NetEmulator.h"

class TestApp
{
    glm::vec2 size = {900, 900};
    GLFWwindow* window;
    ImGuiContext* imgui_context;

    static void error_callback(int error, const char* description)
    {}
    NetEmulator emulator;
public:
    void init()
    {
        if(!glfwInit())
            return;
        glfwSetErrorCallback(error_callback);
        glfwDefaultWindowHints();
        window = glfwCreateWindow(size.x, size.y, "test", NULL, NULL);
        if(!window)
            return;

        glfwMakeContextCurrent(window);
        int version = gladLoadGL(glfwGetProcAddress);
        if(version == 0)
            return;

        glfwSwapInterval(0);

        IMGUI_CHECKVERSION();

        imgui_context = ImGui::CreateContext();
        ImGui::SetCurrentContext(imgui_context);

        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    ~TestApp()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(imgui_context);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void run()
    {
        NetEmulator::setTheme();
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();

            emulator.draw();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

};

int main()
{
    TestApp t;
    t.init();
    t.run();
    return 0;
}