#include "menu.h"
#define 	GLFW_MOUSE_PASSTHROUGH   0x0002000D

GLFWwindow* window;
const char* glsl_version;
bool menuShow;

//Creates menu context
void Menu::Create()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
}

void Menu::Initialize()
{

    menuShow = true;

    if (!glfwInit())
        fprintf(stderr, "failed to init!\n");

    glsl_version = "#version 130";

    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int width = glfwGetVideoMode(monitor)->width;
    int height = glfwGetVideoMode(monitor)->height;

    if (!monitor)
    {
        fprintf(stderr, "Failed to get monitor!\n");
    }
    window = glfwCreateWindow(width, height, "", NULL, NULL);

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

    ImGui::StyleColorsDark();
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

    static int counter = 0;

    if (menuShow)
    {
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();



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
}