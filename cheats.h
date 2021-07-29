#pragma once
#include "includes.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
//#include <GL/GL.h>
//#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "drawing.h"

HWND find_main_window();
vec3 getPlayerPos();

namespace Cheats
{
	bool initalize();
	bool shutdown();

	void ESPLines(bool run, int width, int height, float lineWidth, float r, float g, float b, float a);
	void ESPBox(bool ESPBox, int width, int height, float thicc, float boxR, float boxG, float boxB, float boxA);
	void ESPText(bool ESPText, bool range, int width, int height, float textR, float textG, float textB, float textA);
	void Teleport(vec3 cordinates);
	void SpeedHack(bool run, float speed);
}