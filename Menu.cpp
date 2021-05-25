#include "menu.h"
#define 	GLFW_MOUSE_PASSTHROUGH   0x0002000D

GLFWwindow* window;
const char* glsl_version;
RECT windowSize;
HWND hwnd;
WINDOWPLACEMENT windowPlacement;
bool menuShow;



struct handle_data {
    unsigned long process_id;
    HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lparam) {
    auto& data = *reinterpret_cast<handle_data*>(lparam);
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);

    if (data.process_id != process_id) {
        return TRUE;
    }
    data.window_handle = handle;
    return FALSE;
}

HWND find_main_window() {
    handle_data data{};

    data.process_id = GetCurrentProcessId();
    data.window_handle = nullptr;
    EnumWindows(enum_windows_callback, reinterpret_cast<LPARAM>(&data));

    return data.window_handle;
}

//Creates menu context
void Menu::Create()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
}

void Menu::Initialize()
{

    menuShow = false;

    // TODO: make a re-init func; Do shutdown then recreate/reinit.
    hwnd = find_main_window();
    //hwnd = FindWindow(0, L"Skyrim Special Edition");

    if (!glfwInit())
        fprintf(stderr, "failed to init!\n");

    GetWindowRect(hwnd, &windowSize);
    int width = windowSize.right - windowSize.left;
    int height = windowSize.bottom - windowSize.top;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int monitorWidth = (int)(glfwGetVideoMode(monitor)->width);
    int monitorHeight = (int)(glfwGetVideoMode(monitor)->height);

    glsl_version = "#version 130";

    glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
    glfwWindowHint(GLFW_FOCUSED, false);
    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);
    //glfwWindowHint(GLFW_MAXIMIZED, true);


    if ((width == monitorWidth) && (height == monitorHeight))
    {
        glfwWindowHint(GLFW_MAXIMIZED, true);
        window = glfwCreateWindow(monitorWidth, monitorHeight, "", NULL, NULL);
    }
    else
    {
        window = glfwCreateWindow(width, height, "", NULL, NULL);
        GetWindowPlacement(hwnd, &windowPlacement);
        glfwSetWindowPos(window, windowPlacement.ptMinPosition.x, windowPlacement.ptMinPosition.y);
    }

    //if ((width == glfwGetVideoMode(monitor)->width) && (height == glfwGetVideoMode(monitor)->height))
    //{
    //    glfwWindowHint(GLFW_MAXIMIZED, true);
    //}
    //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    //int width = glfwGetVideoMode(monitor)->width;
    //int height = glfwGetVideoMode(monitor)->height;

    //if (!monitor)
    //{
    //    fprintf(stderr, "Failed to get monitor!\n");
    //}
    //
    //window = glfwCreateWindow(width, height, "", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        std::cout << "failed to create window!" << std::endl;
    }
    glfwSetWindowAttrib(window, GLFW_DECORATED, false);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (gl3wInit() != 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    }

    //ImGui::StyleColorsDark();
    Menu::Theme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Menu::Render()
{

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (menuShow)
    {
        Gui::basicMenu();
    }


    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Menu::Shutdown()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Menu::ToggleMenu()
{
    menuShow = !menuShow;
    if (menuShow)
    {
        glfwFocusWindow(window);
        //SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        //SetFocus(window);
    }
    else
    {
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetFocus(hwnd);
    }
}

void Menu::Theme()
{
    ImGuiStyle* style = &ImGui::GetStyle();

    style->ChildBorderSize = 1;
    style->WindowMinSize = ImVec2(600, 400);
    style->WindowTitleAlign = ImVec2(0.5, 0.5);
    style->AntiAliasedFill = true;

    style->WindowRounding = 0;
    style->ChildRounding = 0;
    style->FrameRounding = 0;
    style->ScrollbarRounding = 0;
    style->TabRounding = 0;
    style->GrabRounding = 0;
    style->ScrollbarSize = 0.9;

    style->Colors[ImGuiCol_TitleBg] = ImColor(25, 25, 25, 230);
    style->Colors[ImGuiCol_TitleBgActive] = ImColor(25, 25, 25, 230);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(25, 25, 25, 130);

    style->Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 230);
    style->Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_ChildBg] = ImColor(20, 20, 20, 255);

    style->Colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25, 230);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 25, 230);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 25, 230);

    style->Colors[ImGuiCol_Header] = ImColor(35, 35, 35, 230);
    style->Colors[ImGuiCol_HeaderActive] = ImColor(35, 35, 35, 230);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(35, 35, 35, 230);

    style->Colors[ImGuiCol_ResizeGrip] = ImColor(35, 35, 35, 255);
    style->Colors[ImGuiCol_ResizeGripActive] = ImColor(35, 35, 35, 255);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(35, 35, 35, 255);

    style->Colors[ImGuiCol_Button] = ImColor(37, 37, 37, 255);
    style->Colors[ImGuiCol_ButtonActive] = ImColor(41, 41, 41, 255);
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(37, 37, 37, 255);

    style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 255);
}