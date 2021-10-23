// Dear ImGui: standalone example application for GLFW + OpenGL3, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context
// creation, etc.) If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class ImguiOpenGL3Viewer
{
public:
    inline ImguiOpenGL3Viewer(const std::string title, const int height, const int width)
        : m_window_(nullptr),
          m_show_demo_window_(false),
          m_show_another_window_(true),
          m_clear_color_(0.45f, 0.55f, 0.60f, 1.00f)
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) return;

        m_window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (m_window_ == nullptr) return;

        glfwMakeContextCurrent(m_window_);
        glfwSwapInterval(1);  // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window_, true);
        ImGui_ImplOpenGL3_Init();
    }

    void render()
    {
        // Main loop
        while (!glfwWindowShouldClose(m_window_))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use
            // your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on
            // those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            update();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(m_window_, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(m_clear_color_.x * m_clear_color_.w, m_clear_color_.y * m_clear_color_.w,
                         m_clear_color_.z * m_clear_color_.w, m_clear_color_.w);
            glClear(GL_COLOR_BUFFER_BIT);

            // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using
            // imgui_impl_opengl3.cpp!!), you may need to backup/reset/restore other state, e.g. for current shader
            // using the commented lines below.
            // GLint last_program;
            // glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
            // glUseProgram(0);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // glUseProgram(last_program);

            glfwMakeContextCurrent(m_window_);
            glfwSwapBuffers(m_window_);
        }
        close();
    }

private:
    void update()
    {
        // 基本的にユーザがいじる関数

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code
        // to learn more about Dear ImGui!).
        if (m_show_demo_window_) ImGui::ShowDemoWindow(&m_show_demo_window_);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");  // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &m_show_demo_window_);  // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &m_show_another_window_);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button(
                    "Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (m_show_another_window_)
        {
            ImGui::Begin("Another Window",
                         &m_show_another_window_);  // Pass a pointer to our bool variable (the window will have a
                                                    // closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) m_show_another_window_ = false;
            ImGui::End();
        }

        // 4. plot widget
        {
            std::vector<float> data{1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 5, 4, 3, 2, 1, 0};
            for (auto& d : data)
            {
                d /= 10.0f;
            }
            const auto ulim = *std::max_element(data.begin(), data.end());
            const auto llim = *std::min_element(data.begin(), data.end());

            ImGui::PlotHistogram("Histogram", data.data(), static_cast<int>(data.size()), 0, NULL, llim, ulim,
                                 ImVec2(0, 80.0f));
        }

        // 5. draw line
        {
            ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Example: Custom rendering"))
            {
                ImGui::End();
                return;
            }
            auto draw_list = ImGui::GetWindowDrawList();
            static float sz = 36.0f;
            static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            {
                const ImVec2 p = ImGui::GetCursorScreenPos();
                const ImU32 col32 = ImColor(col);
                float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;

                draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32);
                x += sz + spacing;
                draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32);
                x += sz + spacing;
                draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f);
                x += sz + spacing;
                draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f),
                                             ImVec2(x, y + sz - 0.5f), col32);
                x += sz + spacing;
                draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(0, 0, 0),
                                                   ImColor(255, 0, 0), ImColor(255, 255, 0), ImColor(0, 255, 0));
            }
        }
    }

    void close()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_window_);
        glfwTerminate();
    }

    GLFWwindow* m_window_;

    bool m_show_demo_window_;

    bool m_show_another_window_;

    ImVec4 m_clear_color_;
};

int main(int, char**)
{
    auto viewer = ImguiOpenGL3Viewer(std::string("OpenGL3 sample"), 760, 1280);
    viewer.render();

    return 0;
}
