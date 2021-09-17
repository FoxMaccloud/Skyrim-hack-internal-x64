#include "menu.h"

GLFWwindow* window;
const char* glsl_version;
RECT windowSize;
HWND hwnd;
WINDOWPLACEMENT windowPlacement;
static bool menuShow;
bool running;

int width;
int height;

int imguiWidth = 600;
int imguiHeight = 400;

//Creates menu context
void Menu::Create()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
}

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
    menuShow = true;

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

    Cheats::initalize();
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

// Movement
vec3 currentPosVec3;
static bool teleport = false;
static bool speedHack = false;
static float speed = 20.0f;

// Debug
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
        ImGui::Begin(XOR("Skyrim Hacks by FoxMaccloud"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        ImGui::BeginTabBar(XOR("Skyrim#TopBar"), ImGuiTabBarFlags_NoTabListScrollingButtons);


        // Misc
        if (ImGui::BeginTabItem(XOR("Misc"))) {

            ImGui::Text(XOR("DEL to unload or press")); ImGui::SameLine();
            if (ImGui::Button(XOR("Unload & Exit")))
            {
                running = false;
            }
            ImGui::Text(XOR("INS to open/close the menu"));

            ImGui::Text(XOR("V 0.4.3"));

            ImGui::EndTabItem();
        }

        // ESP
        if (ImGui::BeginTabItem(XOR("ESP"))) {

            ImGui::Text(XOR("ESP Line Colors"));
            {
                ImGui::SliderFloat(XOR("Red##Line"), &lineR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat(XOR("Green##Line"), &lineG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat(XOR("Blue##Line"), &lineB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat(XOR("Alpha##Line"), &lineA, 0.0f, 1.0f, "A = %.1f");
                ImGui::SliderFloat(XOR("Thickness##Line"), &lineThickness, 1.0f, 30.0f, "T = %.1f");
                ImGui::Checkbox(XOR("ESP Lines"), &ESPLines);
            }
            ImGui::Text(XOR("ESP Box Colors"));
            {
                ImGui::SliderFloat(XOR("Red##Box"), &boxR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat(XOR("Green##Box"), &boxG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat(XOR("Blue##Box"), &boxB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat(XOR("Alpha##Box"), &boxA, 0.0f, 1.0f, "A = %.1f");
                ImGui::SliderFloat(XOR("Thickness##Box"), &boxThicc, 1.0f, 30.0f, "T = %.1f");
                ImGui::Checkbox(XOR("ESP Box"), &ESPBox);
            }
            ImGui::Text(XOR("ESP Text Colors"));
            {
                ImGui::SliderFloat(XOR("Red##Text"), &textR, 0.0f, 1.0f, "R = %.1f");
                ImGui::SliderFloat(XOR("Green##Text"), &textG, 0.0f, 1.0f, "G = %.1f");
                ImGui::SliderFloat(XOR("Blue##Text"), &textB, 0.0f, 1.0f, "B = %.1f");
                ImGui::SliderFloat(XOR("Alpha##Text"), &textA, 0.0f, 1.0f, "A = %.1f");
                ImGui::Checkbox(XOR("ESP Text"), &ESPText); ImGui::SameLine();
                ImGui::Checkbox(XOR("ESP Range Finder"), &ESPRange);
            }
            ImGui::EndTabItem();
        }

        // Movement
        static float currentPos[3];
        if (!teleport)
        {
            currentPosVec3 = getPlayerPos();
            currentPos[0] = currentPosVec3.x;
            currentPos[1] = currentPosVec3.y;
            currentPos[2] = currentPosVec3.z;
        }

        if (ImGui::BeginTabItem(XOR("Movement"))) {

            ImGui::Text(XOR("Teleport"));

            vec3 cords = { currentPos[0], currentPos[1], currentPos[2] };
            if (ImGui::InputFloat3(XOR("XYZ"), currentPos))
                teleport = true;
            if (ImGui::Button(XOR("Go!")))
            { 
                Cheats::Teleport(cords);
                teleport = false;
            } ImGui::SameLine();
           
            vec3 mapMarker = getMapMarkerPos();
            mapMarker.x = mapMarker.x / realCordsScale;
            mapMarker.y = mapMarker.y / realCordsScale;
            mapMarker.z = mapMarker.z / realCordsScale;

            if (ImGui::Button(XOR("Teleport Map Marker")))
            {
                Cheats::Teleport(mapMarker);
            }

            ImGui::Text(XOR("SpeedHack"));
            ImGui::SliderFloat(XOR("Speed##slider"), &speed, 0.0f, 500.0f, "%.1f");
            ImGui::Checkbox(XOR("Speedhack"), &speedHack);


            ImGui::EndTabItem();
        }

        // Debug Console
        if (ImGui::BeginTabItem(XOR("Debug"))) {
            if (ImGui::Button(XOR("Debug Console")))
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

    }



    Cheats::ESPText(ESPText, ESPRange, width, height, textR, textG, textB, textA);


    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    Cheats::SpeedHack(speedHack, speed);
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