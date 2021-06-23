// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"
#include "menu.h"

extern bool running;

uintptr_t WINAPI MainThread(HMODULE hModule)
{
	Menu::Create();
	Menu::Initialize();

	//AllocConsole();
	//FILE* f;
	//freopen_s(&f, "CONOUT$", "w", stdout);

	while (running)
	{
		Menu::Render();
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			Menu::ToggleMenu();
		}

		if (GetAsyncKeyState(VK_DELETE) & 1)
		{
			break;
		}
		Sleep(1);
	}
	Menu::Shutdown(hModule);
	//fclose(f);
	//FreeConsole();

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}