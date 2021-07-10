#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_WARNINGS

struct vec4
{
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};
struct vec3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
};
struct vec2
{
	int x = 0.0f, y = 0.0f;
};

#include <Windows.h>
#include <stdio.h>
#include <iostream>
//#include <TlHelp32.h>
#include <d3d9.h>
#include <Psapi.h>
#include <time.h>
//#include "hooks.h"
#include <vector>
#include <TlHelp32.h>