#pragma once
#include "cheats.h"


#ifndef MENU_H
#define MENU_H


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