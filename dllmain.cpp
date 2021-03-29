// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "entityHook.h"

/*
void placeJMP(uintptr_t* Address, uintptr_t jumpTo, uintptr_t length = 14)
{
	DWORD dwOldProtect, dwBkup, dwRelAddr;

	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(Address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
	dwRelAddr = (uintptr_t)(jumpTo - (uintptr_t)Address) - 14;

	// Write the JMP opcode @ our jump position...
	*Address = 0xE9;

	// Write the offset to where we're gonna jump
	//The instruction will then become JMP ff002123 for example
	*((uintptr_t*)(Address + 0x1)) = dwRelAddr;

	// Overwrite the rest of the bytes with NOPs
	//ensuring no instruction is Half overwritten(To prevent any crashes)
	for (uintptr_t x = 0x14; x < length; x++)
		*(Address + x) = 0x90;

	VirtualProtect(Address, length, dwOldProtect, &dwBkup);
}

void* DetourFunction64(void* pSource, void* pDestination, int dwLen)
{
	DWORD MinLen = 14;

	if (dwLen < MinLen) return NULL;

	BYTE stub[] = {
	0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
	};

	void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD dwOld = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	DWORD64 retto = (DWORD64)pSource + dwLen;

	// trampoline
	memcpy(stub + 6, &retto, 8);
	memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);
	memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

	// orig
	memcpy(stub + 6, &pDestination, 8);
	memcpy(pSource, stub, sizeof(stub));

	for (int i = MinLen; i < dwLen; i++)
	{
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
	}

	VirtualProtect(pSource, dwLen, dwOld, &dwOld);
	return (void*)((DWORD_PTR)pTrampoline);
}
*/
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

uintptr_t WINAPI HackThread(HMODULE hModule)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

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

	/*
	int hookLenght = 14;
	uintptr_t jmpBackAddy = entityList + hookLenght;
	//Ent Hook
	DetourFunction64((uintptr_t*)entityList, entHook, hookLenght);
	*/


	detourFunction((LPVOID)entityList, (LPVOID)entHook, 18);


	std::cout << "Entityhook started!" << std::endl;
	

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			std::cout << "Exiting!" << std::endl;
			Sleep(1000);
			fclose(f);
			FreeConsole();
			FreeLibraryAndExitThread(hModule, 0);
			return 0;
		}
	}
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
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
		//CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)initHooks, hModule, 0, nullptr);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}