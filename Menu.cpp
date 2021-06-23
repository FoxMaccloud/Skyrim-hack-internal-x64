#include "menu.h"
#define 	GLFW_MOUSE_PASSTHROUGH   0x0002000D

GLFWwindow* window;
const char* glsl_version;
RECT windowSize;
HWND hwnd;
WINDOWPLACEMENT windowPlacement;
static bool menuShow;
bool running;


//void drawLine(float x1, float y1, float x2, float y2)
//{
//    // Make line
//    float line[] = {
//        x1, y1,
//        x2, x2
//    };
//
//    unsigned int buffer; // The ID, kind of a pointer for VRAM
//    glGenBuffers(1, &buffer); // Allocate memory for the triangle
//    glBindBuffer(GL_ARRAY_BUFFER, buffer); // Set the buffer as the active array
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, line, GL_STATIC_DRAW); // Fill the buffer with data
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0); // Specify how the buffer is converted to vertices
//    glEnableVertexAttribArray(0); // Enable the vertex array
//
//    // Clear previous
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    // Draw the line
//    glDrawArrays(GL_LINES, 0, 2);
//
//    //// Poll for and process events
//    glfwPollEvents();
//
//}






//Creates menu context
void Menu::Create()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
}

int width;
int height;

int imguiWidth = 600;
int imguiHeight = 400;

void getWindowSize()
{
    GetWindowRect(hwnd, &windowSize);
    width = windowSize.right - windowSize.left -7;
    height = windowSize.bottom - windowSize.top -7;
    if (window)
    {
        glfwSetWindowPos(window, windowSize.left, windowSize.top);
        //glfwSetWindowPos(window, windowPlacement.ptMinPosition.x, windowPlacement.ptMinPosition.y);
        glfwSetWindowSize(window, width, height);
        glfwSetWindowPos(window, windowSize.left, windowSize.top);
    }
}

void Menu::Initialize()
{
    running = true;
    menuShow = false;

    // TODO: make a re-init func; Do shutdown then recreate/reinit.
    hwnd = find_main_window();
    //hwnd = FindWindow(0, L"Skyrim Special Edition");

    if (!glfwInit())
        fprintf(stderr, "failed to init!\n");

    getWindowSize();

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


    if ((width+7 == monitorWidth) && (height+7 == monitorHeight))
    {
        glfwWindowHint(GLFW_MAXIMIZED, true);
        window = glfwCreateWindow(monitorWidth, monitorHeight, "", NULL, NULL);
    }
    else
    {
        window = glfwCreateWindow(width, height, "", NULL, NULL);
        GetWindowPlacement(hwnd, &windowPlacement);
        glfwSetWindowPos(window, windowPlacement.ptMaxPosition.x, windowPlacement.ptMaxPosition.y);
        //glfwSetWindowPos(window, windowPlacement.ptMinPosition.x, windowPlacement.ptMinPosition.y);
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



// ESP Line & colors
static float lineR = 0.4f;
static float lineG = 0.1f;
static float lineB = 0.8f;
static float lineA = 1.0f;
static float lineThickness = 1.0f;
static bool ESPLines = false;

// ESP Box & colors
static float boxR = 0.4f;
static float boxG = 0.1f;
static float boxB = 0.8f;
static float boxA = 1.0f;
static float boxThicc = 1.0f;
static bool ESPBox = false;

// ESP Text & colors
static float textR = 0.4f;
static float textG = 0.1f;
static float textB = 0.8f;
static float textA = 1.0f;
static bool ESPText = false;
static bool ESPRange = false;

static bool debugConsole = false;
FILE* f;

void Menu::Render()
{

    getWindowSize();

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (menuShow)
    {
        ImGui::Begin("Skyrim Hacks by FoxMaccloud", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        ImGui::BeginTabBar("Skyrim#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);


        // Misc
        if (ImGui::BeginTabItem("Misc")) {

            ImGui::Text("DEL to unload or press"); ImGui::SameLine();
            if (ImGui::Button("Unload & Exit"))
            {
                running = false;
            }
            ImGui::EndTabItem();
        }

        // ESP
        if (ImGui::BeginTabItem("ESP")) {

            ImGui::Text("ESP Line Colors");
            {
                ImGui::SliderFloat("Red##Line", &lineR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat("Green##Line", &lineG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat("Blue##Line", &lineB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat("Alpha##Line", &lineA, 0.0f, 1.0f, "A = %.1f");
                ImGui::SliderFloat("Thickness##Line", &lineThickness, 1.0f, 30.0f, "T = %.1f");
                ImGui::Checkbox("ESP Lines", &ESPLines);
            }
            ImGui::Text("ESP Box Colors");
            {
                ImGui::SliderFloat("Red##Box", &boxR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat("Green##Box", &boxG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat("Blue##Box", &boxB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat("Alpha##Box", &boxA, 0.0f, 1.0f, "A = %.1f");
                ImGui::SliderFloat("Thickness##Box", &boxThicc, 1.0f, 30.0f, "T = %.1f");
                ImGui::Checkbox("ESP Box", &ESPBox);
            }
            ImGui::Text("ESP Text Colors");
            {
                ImGui::SliderFloat("Red##Text", &textR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat("Green##Text", &textG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat("Blue##Text", &textB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat("Alpha##Text", &textA, 0.0f, 1.0f, "A = %.1f");
                ImGui::Checkbox("ESP Text", &ESPText); ImGui::SameLine();
                ImGui::Checkbox("ESP Range Finder", &ESPRange);
            }
            ImGui::EndTabItem();
        }

        // Debug Console
        if (ImGui::BeginTabItem("Debug")) {
            if (ImGui::Button("Debug Console"))
            {
                debugConsole = !debugConsole;
                if (debugConsole)
                {
                    AllocConsole();
                    freopen_s(&f, "CONOUT$", "w", stdout);
                }
                else
                {
                    fclose(f);
                    FreeConsole();
                }
            }
            ImGui::EndTabItem();
        }

        //ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + imguiWidth - 336, ImGui::GetCursorPosY() + imguiHeight - 275)); //250
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::EndTabBar();
        ImGui::End();
            
        //{
        //    static char text[1024 * 16];
        //    static char buf[64];
        //    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

        //    ImGui::Begin("Textbox");

        //    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        //    ImGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));

        //    ImGui::End();
        //}

    }


    Cheats::ESPText(ESPText, ESPRange, width, height, "TEST", textR, textG, textB, textA);


    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    
    Cheats::ESPLines(ESPLines, width, height, lineThickness, lineR, lineG, lineB, lineA);
    Cheats::ESPBox(ESPBox, width, height, boxThicc, boxR, boxG, boxB, boxA);
    
    glfwSwapBuffers(window);
}

void Menu::Shutdown(HMODULE hModule)
{
    // Cleanup
    while (!Cheats::shutdown());

    if (debugConsole)
    {
        fclose(f);
        FreeConsole();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    Sleep(100);
    FreeLibraryAndExitThread(hModule, 0);
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
    style->WindowMinSize = ImVec2(imguiWidth, imguiHeight);
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

    style->Colors[ImGuiCol_Tab] = ImColor(136, 0, 20, 140);
    style->Colors[ImGuiCol_TabHovered] = ImColor(136, 0, 20, 160);
    style->Colors[ImGuiCol_TabActive] = ImColor(136, 0, 20, 220);

    style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255, 255);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 255);
}