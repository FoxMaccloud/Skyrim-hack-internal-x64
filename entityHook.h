#include "includes.h"
/*
		movss xmm10, [rdi+0x54]
		movss xmm11, [rdi+0x58]
		movss xmm12, [rdi+0x5c]
*/

class playerEnt
{
public:
	char pad_0000[84]; //0x0000
	vec3 xyz; //0x0054
	class locationPtrClass* locationPtr; //0x0060
	char pad_0068[392]; //0x0068
	class namePtrClass* namePtr; //0x01F0
	char pad_01F8[584]; //0x01F8
}; //Size: 0x0440

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

std::vector<playerEnt*> entities;
//playerEnt* ents[255];
playerEnt* entsptr;
uintptr_t entityObjStart = 0x0;
uintptr_t jmpBackAddy = 0x0;

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
		jmp[jmpBackAddy]
	}
}


//extern "C" void hook(uintptr_t EntityObjStart);
//extern "C" void getEntities(uintptr_t entityObjStart, playerEnt* entsptr);
//extern "C" void giveUp();
//extern "C" void entityHook(playerEnt* entsptr);

//__attribute__((naked)) void setObjStart()
//{
//
//	__asm {
//		// Not neccesary as the hook will fix the stolen bytes.
//		//movss xmm10, [rdi + 0x54]
//		//movss xmm11, [rdi + 0x58]
//		mov entityObjStart, rdi
//		push rax
//		push rcx
//		push rdx
//		push rbx
//		push rsp
//		push rbp
//		push rsi
//		push rdi
//		push r8
//		push r9
//		push r10
//		push r11
//		push r12
//		push r13
//		push r14
//		push r15
//	}
//}
//
//__attribute__((naked)) void setEntsptr()
//{
//	__asm {
//		mov rax, entityObjStart
//		mov[entsptr], rax
//	}
//}
//
//__attribute__((naked)) void giveUp()
//{
//	__asm {
//		pop r15
//		pop r14
//		pop r13
//		pop r12
//		pop r11
//		pop r10
//		pop r9
//		pop r8
//		pop rdi
//		pop rsi
//		pop rbp
//		pop rsp
//		pop rbx
//		pop rdx
//		pop rcx
//		pop rax
//	}
//}