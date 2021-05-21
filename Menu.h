#pragma once

#ifndef MENU_H
#define MENU_H

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
//#include <Psapi.h>

namespace Menu
{
	void Create();
	void Initialize();
	void Render();
	void Shutdown();
	void ToggleMenu();
};

#endif