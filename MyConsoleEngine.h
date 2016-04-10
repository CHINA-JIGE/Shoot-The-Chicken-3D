#pragma once

#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

#include "Math.h"
#include "Renderer.h"
#include "Camera.h"
#include "LightManager.h"
#include "MeshLoader.h"
#include "Timer.h"


#define TRUE 1
#define FALSE 0
#define IS_KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define IS_KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

void DEBUG_MSG1(std::string msg)
{
	::MessageBoxA(0,msg.c_str(),0,0);
}