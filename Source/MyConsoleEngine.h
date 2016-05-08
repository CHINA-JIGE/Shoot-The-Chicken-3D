#pragma once

#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <random>

#include "Math.h"
#include "Types_def.h"

#include "PointCollection.h"
#include "FileLoader.h"
#include "Picture2D.h"
#include "_GeometryMeshGenerator.h"
#include "Camera.h"
#include "Mesh.h"
#include "BulletMgr.h"
#include "Timer.h"
#include "RenderPipeline3D.h"
#include "Renderer.h"


#define IS_KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define IS_KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define DEBUG_MSG1(msg)	::MessageBoxA(0,msg,0,0);

typedef unsigned char BYTE;

