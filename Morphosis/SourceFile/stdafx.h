#pragma once

// 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//테스트를 위한 콘솔창
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif



#define WIN32_LEAN_AND_MEAN

// 헤더
#include "targetver.h"

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <list>
#include <set>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "../MathHelper.h"
#include "../Defines.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
//using Microsoft::WRL::ComPtr;


//GPU에서 알아야 하는 정보들
namespace RootParameter {
	enum {
		CAMERA = 0,
		OBJECT,
		MATERIAL,
		LIGHT,
		TEXTURE,
		VERTEX,
		UV,
		UI,
		ANIM,

		count
	};
}

//GPU에서 알아야 하는 정보들
namespace RootDescriptor {
	enum {
		OBJECT = 0,
		TEXTURE,
		UI,

		count
	};
}

namespace Scenes {
	enum {
		TITLE = 0,
		ENTERROOM,
		MATCHING,
		PLAY,
		RESULT,
		TEST,

		count	
	};
}

namespace KEY {
	enum {
		A = 65,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		_0 = 48,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9
	};
}

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

