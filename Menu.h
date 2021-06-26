#pragma once
#include "cheats.h"


#ifndef MENU_H
#define MENU_H
#define GLFW_MOUSE_PASSTHROUGH   0x0002000D



namespace Menu
{
	void Create();
	void Initialize();
	void Render();
	void Shutdown(HMODULE hModule);
	void ToggleMenu();
	void Theme();
};

#endif