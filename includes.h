#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_WARNINGS

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
#include <cmath>


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

#define realCordsScale 70.753f

#define ABS(x) ((x < 0) ? (-x) : (x))
#define TORAD(x) ((x) * 0.01745329252)