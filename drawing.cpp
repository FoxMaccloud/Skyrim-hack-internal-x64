#include "drawing.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <string_view>

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


static int compileShader(unsigned int type, const std::string_view& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.data();
	const auto len = (GLint)source.size();
	glShaderSource(id, 1, &src, &len);
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

static unsigned int createShader(const std::string_view& vertexShader, const std::string_view& fragmentShader)
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


//SA: Raw string literals make the shader input easy
constexpr auto vertexShader = R"(
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;

void main()
{
	v_color = a_color;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
};
)";

constexpr auto fragmentShader = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;

void main()
{
    color = v_color;
};
)";


void createShader()
{
	shader = createShader(vertexShader, fragmentShader);
	createdShader = true;
}



LineArt makeLine(point p0, point p1, float lineWidth, color_rgba color)
{
	auto& [x1, y1] = p0;
	auto& [x2, y2] = p1;

	x1 = pixToPointWidth(x1, width);
	y1 = pixToPointHeight(y1, height);
	x2 = pixToPointWidth(x2, width);
	y2 = pixToPointHeight(y2, height);

	struct Vertex
	{
		point pos;
		color_rgba col;
	};
	std::array<Vertex, 2> verts
	{
		Vertex{ p0, color },
		Vertex{ p1, color }
	};

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glLineWidth(lineWidth);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	
	auto _at = 0;
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	_at += sizeof(point);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)_at);

	glBindVertexArray(0);

	LineArt out{};
	out.vao = vao;
	out.vbo = vbo;
	out.count = verts.size();
	return out;
};






void drawLine(const LineArt& line)
{
	if (!createdShader)
	{
		createShader();
		glUseProgram(shader);
	};

	//SA: Now you can just bind the VAO and draw the data in its bound VBOs
	glBindVertexArray(line.vao);
	
	glDrawArrays(GL_LINES, 0, line.count);
	
	glBindVertexArray(0);

};


std::array<point, 4> find_corners(point center, float w, float h)
{
	const auto [x, y] = center;

	const float left = x - (w / 2);
	const float right = x + (w / 2);
	const float top = y + (h / 2);
	const float bottom = y - (h / 2);

	std::array<point, 4> out{};
	out[0] = point{ left, bottom };
	out[1] = point{ left, top };
	out[2] = point{ right, top };
	out[3] = point{ right, bottom };
	return out;
};

void drawBox(point center, float width, float height, float thicc, color_rgba color)
{
	const auto corners = find_corners(center, width, height);
	const auto [lb, lt, rt, rb] = corners;

	drawLine(lb, lt, thicc, color);
	drawLine(lt, rt, thicc, color);
	drawLine(rt, rb, thicc, color);
	drawLine(rb, lb, thicc, color);
};



//ImGuiWindow* window = nullptr;
void drawString(const char* string, point center, color_rgba _color)
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
