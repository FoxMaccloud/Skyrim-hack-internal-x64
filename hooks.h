#include "includes.h"

class localPlayerBase
{
public:
	class playerEnt* playerEnt; //0x0000
}; //Size: 0x0008

class playerEnt
{
public:
	char pad_0000[84]; //0x0000
	vec3 xyz; //0x0054
	class locationPtrClass* locationPtr; //0x0060
	char pad_0068[392]; //0x0068
	class namePtrClass* namePtr; //0x01F0
	char pad_0238[3160]; //0x0238
	class accelerationBase* accelerationBase; //0x0E50
};

class locationPtrClass
{
public:
	char pad_0000[40]; //0x0000
	char* location; //0x0028
	char pad_0030[24]; //0x0030
}; //Size: 0x0048

class namePtrClass
{
public:
	char pad_0000[40]; //0x0000
	char* name; //0x0028
	char pad_0030[24]; //0x0030
}; //Size: 0x0048


class accelerationBase
{
public:
	char pad_0000[176]; //0x0000
	float rightLeftSpeed; //0x00B0
	float forwardBackwardSpeed; //0x00B4
};

playerEnt* localPlayer;


std::vector<playerEnt*> entities;
//playerEnt* ents[255];
playerEnt* entsptr;
uintptr_t entityObjStart = 0x0;
uintptr_t jmpBackAddyEntityList = 0x0;

__declspec(naked) void entHook()
{

	__asm {
		// orig rax got pushed onto the stack, RAX is currently the address to this func.
		// We therefor have to pop it off the stack to get its orig value.
		pop rax
		mov[entsptr], rdi
		movss xmm10, [rdi + 0x54]
		movss xmm11, [rdi + 0x58]
		movss xmm12, [rdi + 0x5c]
	}

	__asm {
		push rax; save current rax
		push rbx; save current rbx
		push rcx; save current rcx
		push rdx; save current rdx
		push rbp; save current rbp
		push rdi; save current rdi
		push rsi; save current rsi
		push r8; save current r8
		push r9; save current r9
		push r10; save current r10
		push r11; save current r11
		push r12; save current r12
		push r13; save current r13
		push r14; save current r14
		push r15; save current r15
	}

	bool alreadyThere = false;

	if (entsptr == nullptr)
	{
		goto GIVE_UP;
	}

	for (int i = 0; i < entities.size(); i++)
	{
		if (entities.at(i) == entsptr)
		{
			entities.at(i) = entsptr; // insert
			alreadyThere = true;
			break;
		}
	}

	if (alreadyThere)
	{
		goto GIVE_UP;
	}
	else
	{
		entities.push_back(entsptr);
	}

GIVE_UP:
	__asm {
		pop r15; restore current r15
		pop r14; restore current r14
		pop r13; restore current r13
		pop r12; restore current r12
		pop r11; restore current r11
		pop r10; restore current r10
		pop r9; restore current r9
		pop r8; restore current r8
		pop rsi; restore current rsi
		pop rdi; restore current rdi
		pop rbp; restore current rbp
		pop rdx; restore current rdx
		pop rcx; restore current rcx
		pop rbx; restore current rbx
		pop rax; restore current rax
		jmp[jmpBackAddyEntityList]
	}
}

class playerCordinates
{
public:
	char pad_0000[288]; //0x0000
	vec3 xyz; //0x0120
};

uintptr_t jmpBacklocalPlayerCordinatesPtr = 0x0;
playerCordinates* localPlayerCordinates;

__declspec(naked) void playerCords()
{
	__asm {
		pop rax
		mov [localPlayerCordinates], rcx
		// orig code
		mov r12, r9
		mov rcx, [rcx + 0x20]
		lea r9, [rsp + 0x30]
		mov rbx, rdx
		movaps [rsp + 0xD0], xmm6
		mov rax, [r14 + 0x10]
		jmp [jmpBacklocalPlayerCordinatesPtr]
	}
}

//class currentEnt
//{
//public:
//	uintptr_t self;
//};

uintptr_t jmpBacklocalPlayerSpeedPtr = 0x0;
//currentEnt* currentEntUpdateSpeed;
accelerationBase* currentEntUpdateSpeed;
float moveSpeed  = 20.0f;
bool speedHackOn = false;
// Modern Problems require Stupid Solutions or something like that...
float savexmm0;
float savexmm1;
float savexmm2;
float savexmm3;
float savexmm4;
float savexmm5;
float savexmm6;
float savexmm7;
float savexmm8;

__declspec(naked) void speedHack()
{
	__asm {

		mov[currentEntUpdateSpeed], rdi

		push rax; save current rax
		push rbx; save current rbx
		push rcx; save current rcx
		push rdx; save current rdx
		push rbp; save current rbp
		push rdi; save current rdi
		push rsi; save current rsi
		push r8; save current r8
		push r9; save current r9
		push r10; save current r10
		push r11; save current r11
		push r12; save current r12
		push r13; save current r13
		push r14; save current r14
		push r15; save current r15
		movss[savexmm0], xmm0
		movss[savexmm2], xmm2
		movss[savexmm3], xmm3
		movss[savexmm4], xmm4
		movss[savexmm5], xmm5
		movss[savexmm6], xmm6
		movss[savexmm7], xmm7
		movss[savexmm8], xmm8
	}

	if ((speedHackOn) &&
		(localPlayer->accelerationBase->forwardBackwardSpeed > 1.0f) &&
		(localPlayer->accelerationBase == currentEntUpdateSpeed))
	{
		localPlayer->accelerationBase->forwardBackwardSpeed = moveSpeed;
	}

	__asm {
		pop r15; restore current r15
		pop r14; restore current r14
		pop r13; restore current r13
		pop r12; restore current r12
		pop r11; restore current r11
		pop r10; restore current r10
		pop r9; restore current r9
		pop r8; restore current r8
		pop rsi; restore current rsi
		pop rdi; restore current rdi
		pop rbp; restore current rbp
		pop rdx; restore current rdx
		pop rcx; restore current rcx
		pop rbx; restore current rbx
		pop rax; restore current rax
		movss xmm8, [savexmm8]
		movss xmm7, [savexmm7]
		movss xmm6, [savexmm6]
		movss xmm5, [savexmm5]
		movss xmm4, [savexmm4]
		movss xmm3, [savexmm3]
		movss xmm2, [savexmm2]
		movss xmm0, [savexmm0]
	}

	__asm {
		movss xmm1, [rdi + 0x000000B4]
		movss [rbp+0x00000110], xmm1
		movss xmm3, [rbp + 0x00000110]

		jmp [jmpBacklocalPlayerSpeedPtr]
	}
}