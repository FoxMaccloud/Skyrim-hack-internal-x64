#include "Cheats.h"
#include "hooks.h"
#include <sstream>

// Declare helper functions
void* detourFunction(void* pSource, void* pDestination, int dwLen);
bool Detour64(BYTE* src, BYTE* dst, const uintptr_t len);
bool hookPushRax(void* toHook, void* hk_func, int len);
bool rewriteOrigBytesEntList(void* toWriteBack, int len = 18);
bool rewriteOrigBytesLocalPlayerCordinates(void* toWriteBack, int len = 27);
MODULEINFO GetModuleInfo(const wchar_t* szModule);
uintptr_t FindPattern(const wchar_t* module, const char* pattern, const char* mask);
bool worldToScreenDXtoOGL(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight);
bool worldToScreen(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight);
float findDistance(vec3 self, vec3 entity);


HWND handle;
uintptr_t moduleBase;
uintptr_t entityList;
uintptr_t localPlayerCordinatesPtr;
uintptr_t viewMatrix;
uintptr_t localPlayerPtr;
playerEnt* localPlayer;




bool initalized = false;

bool Cheats::initalize()
{
	MODULEINFO info = GetModuleInfo(L"SkyrimSE.exe");

	do
	{
		entityList = FindPattern(
			L"SkyrimSE.exe",
			"\xF3\x44\x0F\x10\x57\x54\xF3\x44\x0F\x10\x5F\x58\xF3\x44\x0F\x10\x67\x5C",
			"xxxxxxxxxxxxxxxxxx"
		);
	} while (!entityList);


	entities.reserve(256);
	jmpBackAddyEntityList = (uintptr_t)entityList + 18;
	hookPushRax((void*)entityList, (void*)entHook, 18);

	do
	{
		localPlayerCordinatesPtr = FindPattern(
			L"SkyrimSE.exe",
			"\x4D\x8B\xE1\x48\x8B\x49\x20\x4C\x8D\x4C\x24\x30\x48\x8B\xDA\x0F\x29\xB4\x24\xD0\x00\x00\x00\x49\x8B\x46\x10",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxx"
		);
	} while (!localPlayerCordinatesPtr);

	jmpBacklocalPlayerCordinatesPtr = (uintptr_t)localPlayerCordinatesPtr + 27;
	hookPushRax((void*)localPlayerCordinatesPtr, (void*)playerCords, 27);

	moduleBase = (uintptr_t)info.lpBaseOfDll;

	viewMatrix = moduleBase + 0x2F4C910;
	localPlayerPtr = moduleBase + 0x2F3B040;
	localPlayer = *(playerEnt**)(moduleBase + 0x2F3B040);

	initalized = true;
	return true;
}

bool Cheats::shutdown()
{
	if ((rewriteOrigBytesEntList((void*)entityList, 18)) && 
		(rewriteOrigBytesLocalPlayerCordinates((void*)localPlayerCordinatesPtr, 27)))
		return true;
	return false;
}


float VM[16];

void Cheats::ESPLines(bool run, int width, int height, float lineWidth, float r, float g, float b, float a)
{
	if (!initalized)
	{
		Cheats::initalize();
	}

	if (run)
	{
		memcpy(&VM, (PBYTE*)viewMatrix, sizeof(VM)); //0x7FF68896C910 ? SkyrimSE.exe+2F4C910

		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i) != nullptr)
			{
				vec2 vScreen;
				if (worldToScreenDXtoOGL(entities.at(i)->xyz, vScreen, VM, width, height))
				{
					if (entities.at(i) != nullptr)
					{
						drawLine(width / 2, 0, vScreen.x, vScreen.y, lineWidth, r, g, b, a);
					}
				}
			}
		}
	}
}

void Cheats::ESPBox(bool run, int width, int height, float thicc, float r, float g, float b, float a)
{
	if (!initalized)
	{
		Cheats::initalize();
	}

	std::cout << "entlist: " << entityList << std::endl;

	if (run)
	{
		memcpy(&VM, (PBYTE*)viewMatrix, sizeof(VM));

		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i) != nullptr)
			{
				vec2 vScreen;
				if (worldToScreenDXtoOGL(entities.at(i)->xyz, vScreen, VM, width, height))
				{
					if (entities.at(i) != nullptr)
					{
						drawBox(vScreen.x, vScreen.y, 60.0f, 120.0f, thicc, r, g, b, a);
					}
				}
			}
		}
	}
}

void Cheats::ESPText(bool ESPText, bool range, int width, int height, const char* text, float r, float g, float b, float a)
{
	if (!initalized)
	{
		Cheats::initalize();
	}
	if (ESPText || range)
	{
		memcpy(&VM, (PBYTE*)viewMatrix, sizeof(VM));

		for (int i = 0; i < entities.size(); i++)
		{
			if (entities.at(i) != nullptr)
			{
				vec2 vScreen;
				if (worldToScreen(entities.at(i)->xyz, vScreen, VM, width, height))
				{
					if (entities.at(i) != nullptr)
					{
						if (ESPText)
							drawString(entities.at(i)->namePtr->name, vScreen.x, vScreen.y, r, g, b, a);
						if (range)
						{
							float dist = findDistance(localPlayer->xyz, entities.at(i)->xyz);
							dist = dist / 100;
							if (dist > 250.0f)
							{
								entities.at(i) = nullptr;
							}
							std::ostringstream ss;
							ss << dist;
							std::string s(ss.str());
							s = s + " m";
							char char_array[30];
							strcpy(char_array, s.c_str());

							drawString(char_array, vScreen.x, vScreen.y + 14.0f, r, g, b, a);
						}
					}
				}
			}
		}
	}
}

void Cheats::Teleport(vec3 cordinates)
{
	if (!initalized)
	{
		Cheats::initalize();
	}
	if (localPlayerCordinates != nullptr)
	{
		localPlayerCordinates->xyz = cordinates;
	}
}

vec3 getPlayerPos()
{
	if (!initalized)
	{
		Cheats::initalize();
	}
	if (localPlayerCordinates == nullptr)
	{
		vec3 fail = { 4,0,4 };
		return fail;
	}
	return localPlayerCordinates->xyz;
}




// Helper functions

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

bool rewriteOrigBytesEntList(void* toWriteBack, int len)
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

bool rewriteOrigBytesLocalPlayerCordinates(void* toWriteBack, int len)
{
	DWORD curProtection;
	VirtualProtect(toWriteBack, len, PAGE_EXECUTE_READWRITE, &curProtection);
	unsigned char orig[] = {

		0x4d, 0x89, 0xcc,									//mov r12, r9
		0x48, 0x8b, 0x49, 0x20,								//mov rcx,[rcx + 0x20]
		0x4c, 0x8d, 0x4c, 0x24, 0x30,						//lea r9,[rsp + 0x30]
		0x48, 0x89, 0xd3,									//mov rbx, rdx
		0x0f, 0x29, 0xb4, 0x24, 0xd0, 0x00, 0x00, 0x00,		//movaps[rsp + 0xD0], xmm6
		0x49, 0x8b, 0x46, 0x10								//mov rax,[r14 + 0x10]
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

bool worldToScreenDXtoOGL(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 120.0f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = (windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

bool worldToScreen(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 120.0f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

struct handle_data
{
	unsigned long process_id;
	HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lparam) {
	auto& data = *reinterpret_cast<handle_data*>(lparam);
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);

	if (data.process_id != process_id)
	{
		return TRUE;
	}
	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window()
{
	handle_data data{};

	data.process_id = GetCurrentProcessId();
	data.window_handle = nullptr;
	EnumWindows(enum_windows_callback, reinterpret_cast<LPARAM>(&data));

	return data.window_handle;
}

float findDistance(vec3 self, vec3 entity)
{
	float x = (entity.x - self.x);
	float y = (entity.y - self.y);
	float z = (entity.z - self.z);
	float dist = (x * x) + (y * y) + (z * z);
	return (sqrt(dist));
}
