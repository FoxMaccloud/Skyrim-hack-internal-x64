#include "drawing.h"

extern int width;
extern int height;

unsigned int shader;
bool createdShader = false;


template <typename T, typename W>
float pixToPointWidth(T x, W width)
{
    return ((2*(x)/width)-1);
}

template <typename T, typename H>
float pixToPointHeight(T y, H height)
{
    return ((2*(y) / height) - 1);
}


static int compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* message = (char*)alloca(len * sizeof(char));
        glGetShaderInfoLog(id, len, &len, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "faragment") << "  shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void createShader()
{
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "layout(location = 1) in vec4 a_color;\n"
        "\n"
        "out vec4 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    v_color = a_color;\n"
        "    gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "in vec4 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    color = v_color;\n"
        "}\n";

    shader = createShader(vertexShader, fragmentShader);
    createdShader = true;
}

//float pos[12];
void drawLine(float x1, float y1, float x2, float y2, float lineWidth, float r, float g, float b, float a)
{
    x1 = pixToPointWidth(x1, width);
    y1 = pixToPointHeight(y1, height);
    x2 = pixToPointWidth(x2, width);
    y2 = pixToPointHeight(y2, height);

    //pos[0]  =   { x1 };
    //pos[1]  =   { y1 };
    //pos[2]  =   { r };
    //pos[3]  =   { g };
    //pos[4]  =   { b };
    //pos[5]  =   { a };
    //pos[6]  =   { x2 };
    //pos[7]  =   { y2 };
    //pos[8]  =   { r };
    //pos[9]  =   { g };
    //pos[10] =   { b };
    //pos[11] =   { a };

    float pos[12] = {
        x1, y1, r, g, b, a,
        x2, y2, r, g, b, a
    };
    
    static unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glLineWidth(lineWidth);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void*)8);


    if (!createdShader)
    {
        createShader();
    }

    glUseProgram(shader);

    glDrawArrays(GL_LINES, 0, 2);

    glDeleteBuffers(1, &buffer);
}

void drawBox(float x, float y, float w, float h, float thicc, float r, float g, float b, float a)
{
    drawLine(x - w / 2, y, x + w / 2, y, thicc, r, g, b, a);
    drawLine(x + w / 2, y, x + w / 2, y + h, thicc, r, g, b, a);
    drawLine(x + w / 2, y + h, x - w / 2, y + h, thicc, r, g, b, a);
    drawLine(x - w / 2, y + h, x - w / 2, y, thicc, r, g, b, a);
}



//ImGuiWindow* window = nullptr;
void drawString(const char* string, float x, float y, float r, float g, float b, float a)
{
    //float pos[6] = { x, y, r, g, b, a };
    
    //HDC hdc = wglGetCurrentDC();
    //HFONT hFont = CreateFontA(-(height), 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");
    //HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    //wglUseFontBitmaps(hdc, 32, 96, 96);
    //SelectObject(hdc, hOldFont);
    //DeleteObject(hFont);

    // Fuck this^ Let's cheat with Imgui.

    if (!string)
        return;

    auto window = ImGui::GetBackgroundDrawList();

    //window = ImGui::GetCurrentWindow();
    
    ImVec2 imScreen = { x, y };
    ImVec4 color = { r, g, b, a };

    auto size = ImGui::CalcTextSize(string);
    imScreen.x -= size.x * 0.5f;
    imScreen.y -= size.y;

    window->AddText(nullptr, 0.f,
        { imScreen.x - 1.f, imScreen.y + 1.f },
        ImGui::GetColorU32(IM_COL32_BLACK), string);

    window->AddText(nullptr, 0.f, imScreen, ImGui::GetColorU32(color), string);
}