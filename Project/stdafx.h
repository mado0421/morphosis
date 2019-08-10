#pragma once
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NUMSWAPCHAINBUFFER	2
#define FRAME_BUFFER_WIDTH	960
#define FRAME_BUFFER_HEIGHT	540
#define MAX_SAMPLE_COUNT	50 // Maximum frame time sample count

/*======================================================================
Default Header
======================================================================*/
#include <SDKDDKVer.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/*======================================================================
C/C++ Header
======================================================================*/
#include <vector>

/*======================================================================
DirectX Header
======================================================================*/
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"

/*======================================================================
Library
======================================================================*/
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

/*======================================================================
Namespace
======================================================================*/
using namespace std;
//using namespace DirectX;

/*======================================================================
Enumerator
======================================================================*/
enum class SceneType { TITLE/*, LOBBY*/, MAINPLAY/*, RESULT*/, count };

extern unsigned int	g_nCbvSrvDescriptorIncrementSize;
