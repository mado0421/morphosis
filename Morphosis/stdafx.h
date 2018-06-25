#pragma once

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define WIN32_LEAN_AND_MEAN
//#define FRAME_BUFFER_WIDTH			1920
//#define FRAME_BUFFER_HEIGHT			1080
#define FRAME_BUFFER_WIDTH			800
#define FRAME_BUFFER_HEIGHT			600

#include "targetver.h"

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

extern UINT	gnCbvSrvDescriptorIncrementSize;

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
//using Microsoft::WRL::ComPtr;