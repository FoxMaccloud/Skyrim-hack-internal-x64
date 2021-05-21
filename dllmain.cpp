// dllmain.cpp : Defines the entry point for the DLL application.
#include "includesAndVars.h"
#include "entityHook.h"
#include "Menu.h"

bool randomTeleport = false;
bool ESP = false;


void* detourFunction(void* pSource, void* pDestination, int dwLen)
{
	DWORD MinLen = 14;

	if (dwLen < MinLen)
		return NULL;

	BYTE stub[] =
	{
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     // ptr
	};

	void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD dwOld = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	// trampoline
	memcpy(stub + 6, &pDestination, 8);

	memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);
	memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

	// orig
	memcpy(stub + 6, &pTrampoline, 8);
	memcpy(pSource, stub, sizeof(stub));

	for (int i = MinLen; i < dwLen; i++)
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;

	VirtualProtect(pSource, dwLen, dwOld, &dwOld);

	return (void*)((DWORD_PTR)pSource + dwLen);
}

bool Detour64(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 14) return false;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	*(uintptr_t*)((uintptr_t)src) = (uintptr_t)0x25ff;
	memcpy_s(src + 6, 8, &dst, 8);

	if (len > 14)
	{
		memset(src + 14, '\x90', len - 14);
	}

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

bool hookPushRax(void* toHook, void* hk_func, int len)
{
	if (len < 13) {                                            //if we try to hook less than 13 bytes...abort
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	//memset(toHook, 0x90, len);
	//                   //push rax   //0x50
	//                  //mov rax,hk_func  //0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	//                   //filling with nops our future jmp
	//                   //jmp rax  //0xFF, 0xE0
	//                    //we need to pop RAX inside our hook before anything else otherwise we crash
	unsigned char patch[] = {
		0x50, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xE0
	};
	*(DWORD64*)&patch[3] = (DWORD64)hk_func; //replacing zeros with our function address
	memcpy((void*)toHook, patch, sizeof(patch));
	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);
	return true;
}

bool rewriteOrigBytes(void* toWriteBack, int len = 18)
{
	DWORD curProtection;
	VirtualProtect(toWriteBack, len, PAGE_EXECUTE_READWRITE, &curProtection);
	unsigned char orig[] = {
		0xF3, 0x44, 0x0F, 0x10, 0x57, 0x54, // movss xmm10, [rdi+0x54]
		0xF3, 0x44, 0x0F, 0x10, 0x5F, 0x58, // movss xmm11, [rdi+0x58]
		0xF3, 0x44, 0x0F, 0x10, 0x67, 0x5C  // movss xmm12, [rdi+0x5c]
	};
	memcpy((void*)toWriteBack, orig, sizeof(orig));
	DWORD temp;
	VirtualProtect(toWriteBack, len, curProtection, &temp);
	return true;
}


MODULEINFO GetModuleInfo(const wchar_t* szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}

uintptr_t FindPattern(const wchar_t* module, const char* pattern, const char* mask)
{
	MODULEINFO mInfo = GetModuleInfo(module);
	uintptr_t base = (uintptr_t)mInfo.lpBaseOfDll;
	uintptr_t size = (uintptr_t)mInfo.SizeOfImage;
	uintptr_t patternLength = (uintptr_t)strlen(mask);

	for (uintptr_t i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (uintptr_t j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}
		if (found)
		{
			return base + i;
		}
	}
	return NULL;
}

extern bool run;

uintptr_t WINAPI MainThread(HMODULE hModule)
{
	//AllocConsole();
	//FILE* f;
	//freopen_s(&f, "CONOUT$", "w", stdout);

	Menu::Create();
	Menu::Initialize();

	uintptr_t entityList = FindPattern(
		L"SkyrimSE.exe",
		"\xF3\x44\x0F\x10\x57\x54\xF3\x44\x0F\x10\x5F\x58\xF3\x44\x0F\x10\x67\x5C",
		"xxxxxxxxxxxxxxxxxx"
	);

	if (!entityList)
	{
		std::cout << "Failed to get signature!" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Addr: " << std::hex << entityList << std::endl;
	}

	entities.reserve(256);
	jmpBackAddy = (uintptr_t)entityList + 18;
	hookPushRax((void*)entityList, (void*)entHook, 18);
	//Detour64((BYTE*)entityList, (BYTE*)entHook, 18);
	//detourFunction((LPVOID)entityList, (LPVOID)entHook, 18);

	while (true)
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

		if (GetAsyncKeyState(VK_HOME) & 1)
		{
			std::cout << "Entities:" << std::endl;
			for (int i = 0; i < entities.size(); i++)
			{
				if (entities.at(i) != nullptr)
				{
					std::cout
						<< "Entity" << std::dec << i << ": " << entities.at(i)
						<< " name: " << entities.at(i)->namePtr->name
						<< ", location: " << entities.at(i)->locationPtr->location 
						<< ", X: " << entities.at(i)->xyz.x
						<< ", Y: " << entities.at(i)->xyz.y
						<< ", Z: " << entities.at(i)->xyz.z
						<< std::endl;
				}
			}
		}	
		// To not kill the cpu sleep for 1 ms
		Sleep(1);
	}
	rewriteOrigBytes((void*)entityList, 18);
	Menu::Shutdown();
	//fclose(f);
	//FreeConsole();
	Sleep(100);
	FreeLibraryAndExitThread(hModule, 0);

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