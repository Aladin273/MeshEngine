#include "GLGuiSystem.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <spdlog/spdlog.h>

GLGuiSystem::GLGuiSystem(Window* window)
    : m_window(window)
{

}

GLGuiSystem::~GLGuiSystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GLGuiSystem::init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(m_window->getHandle()), true);
    ImGui_ImplOpenGL3_Init();

    spdlog::info("GLGuiSystem inited successful");
}

void GLGuiSystem::render()
{
    begin();

    for (auto layer : m_layers)
    {
        layer.second->render();
    }

    end();
}

void GLGuiSystem::render(GuiLayer& layer)
{
    begin();
    layer.render();
    end();
}

void GLGuiSystem::begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GLGuiSystem::end()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(m_window->getWidth()), static_cast<float>(m_window->getHeight()));

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

void GLGuiSystem::pushLayer(uint32_t index, GuiLayer& layer)
{
    m_layers.emplace(index, &layer);
}

void GLGuiSystem::popLayer(uint32_t index)
{
    auto it = m_layers.find(index);

    if (it != m_layers.end())
        m_layers.erase(index);
}

bool GLGuiSystem::wantCaptureMouse()
{
    return ImGui::GetIO().WantCaptureMouse;
}

bool GLGuiSystem::wantCaptureKeyboard()
{
    return ImGui::GetIO().WantCaptureKeyboard;
}