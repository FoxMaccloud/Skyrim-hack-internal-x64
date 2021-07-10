#pragma once

#include "includes.h"
#include "menu.h"

#include <cstdint>

struct point
{
	float x = 0.0f;
	float y = 0.0f;
};

struct color_rgba
{
	uint8_t r = 0;	//SA: unsigned 8 bit integer
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};


//SA: I grouped the VAO, VBO, and number of vertices as that's really all thats needed to redraw this
struct LineArt
{
	GLuint vao = 0;
	GLuint vbo = 0;
	size_t count = 0;
};

//SA: Hold onto that VAO so you dont have to reupload all of the data each time
LineArt makeLine(point p0, point p1, float lineWidth, color_rgba color);

//SA: Group your parameters into structs!
void drawLine(const LineArt& line);


void drawBox(point center, float width, float height, float thicc, color_rgba color);
void drawString(const char* string, point center, color_rgba color);
