#include "GLGuiSystem.h"

GLGuiSystem::GLGuiSystem(IWindow* window)
    : m_window(window)
{

}

GLGuiSystem::~GLGuiSystem()
{
    //ImGui_ImplOpenGL2_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
}

void GLGuiSystem::init()
{
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //ImGui::StyleColorsClassic();
    //ImGuiStyle& style = ImGui::GetStyle();

    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    //ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(m_window->getHandle()), true);
    //ImGui_ImplOpenGL2_Init();
}

void GLGuiSystem::render()
{
    //begin();

    //for (auto layer : m_layers)
    //{
    //    layer.second->context(getContext());
    //    layer.second->render();
    //}

    //end();
}

void GLGuiSystem::render(IGuiLayer& layer)
{
    //begin();

    //layer.context(getContext());
    //layer.render();

    //end();
}

void GLGuiSystem::setContext(void* context)
{
    //ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(context));
}

void* GLGuiSystem::getContext()
{
    return nullptr;
    //return ImGui::GetCurrentContext();
}

void GLGuiSystem::begin()
{
    //ImGui_ImplOpenGL2_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();
}

void GLGuiSystem::end()
{
    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize = ImVec2(static_cast<float>(m_window->getWidth()), static_cast<float>(m_window->getHeight()));

    //ImGui::Render();
    //ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    GLFWwindow* backup = glfwGetCurrentContext();
    //    ImGui::UpdatePlatformWindows();
    //    ImGui::RenderPlatformWindowsDefault();
    //    glfwMakeContextCurrent(backup);
    //}
}

void GLGuiSystem::pushLayer(uint32_t index, IGuiLayer& layer)
{
    //m_layers.emplace(index, &layer);
}

void GLGuiSystem::popLayer(uint32_t index)
{
    //auto it = m_layers.find(index);

    //if (it != m_layers.end())
    //    m_layers.erase(index);
}

void GLGuiSystem::turnDocking(bool enable)
{
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //if (enable)
    //    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //else
    //    io.ConfigFlags &= ImGuiConfigFlags_DockingEnable;
}

void GLGuiSystem::turnVieports(bool enable)
{
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //if (enable)
    //    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //else
    //    io.ConfigFlags &= ImGuiConfigFlags_ViewportsEnable;
}

bool GLGuiSystem::wantCapture()
{
    return false;
    //return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}
