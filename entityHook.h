#include "framework.h"

struct vec4
{
	float x, y, z, w;
};
struct vec3
{
	float x, y, z;
};
struct vec2
{
	int x, y;
};

class playerEnt
{
public:
	char pad_0000[96]; //0x0000
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

playerEnt* ents[255];
playerEnt* entsptr;
uintptr_t entityObjStart = 0x0;


extern "C" void entityHook(playerEnt* entsptr);
//extern "C" void entityHook(uintptr_t entityObjStart, playerEnt * entsptr);
//extern "C" void entityHook();

void __fastcall entHook()
{
	
	entityHook(entsptr);
	//entityHook(entityObjStart, entsptr);
	//entityHook();


	bool alreadyThere = false;

	if (entsptr == nullptr)
	{
		goto GIVE_UP;
	}

	for (int i = 0; i < 254; i++)
	{
		if (ents[i] == entsptr)
		{
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
		for (int i = 0; i < 254; i++)
		{
			if (ents[i] == 0)
			{
				ents[i] = entsptr;
				break;
			}
		}
	}

GIVE_UP:
	std::cout << "Given up!" << std::endl;
}