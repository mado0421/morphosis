#pragma once
/*======================================================================
Define
======================================================================*/
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define NUMSWAPCHAINBUFFER			2
#define FRAME_BUFFER_WIDTH			960
#define FRAME_BUFFER_HEIGHT			540
#define MAX_SAMPLE_COUNT			50 // Maximum frame time sample count
#define ASSET_PATH					"Assets/"
#define LASSET_PATH					L"Assets/"
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
#define NUMANIMBONE					64
/*======================================================================
Library
======================================================================*/
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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
//#include <array>
//#include <iostream>
#include <fstream>
//#include <chrono>
//#include <cmath>
#include <string>

/*======================================================================
DirectX Header
======================================================================*/
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader12.h"
#include "MathHelper.h"

/*======================================================================
Namespace
======================================================================*/
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

/*======================================================================
Enumerator
======================================================================*/
enum class SceneType { TITLE/*, LOBBY*/, MAINPLAY/*, RESULT*/, count };
enum class RootParameter { CAMERA, OBJECT, TEXTURE, ANIMATION, UI, count };
enum class PSO { MODEL, ANIMATEDMODEL/*, UI*/, count };
enum class MeshType { MODEL, ANIMATEDMODEL, UI, count };

/*======================================================================
Extern
======================================================================*/
class Mesh;
class Texture;
class Model;
class AnimClip;
class AnimCtrl;

extern unsigned int					g_nCbvSrvDescriptorIncrementSize;
extern ID3D12RootSignature*			g_pd3dGraphicsRootSignature;
extern vector<ID3D12PipelineState*> g_vecPipelineStateObjects;
extern vector<Mesh*>				g_vecMesh;
extern vector<Texture*>				g_vecTexture;
extern vector<Model*>				g_vecModel;
extern vector<AnimClip*>			g_vecAnimClip;
extern vector<AnimCtrl*>			g_vecAnimCtrl;

extern void CreateModel(const char* meshName, const char* textureName, const char* modelName);
extern void CreateAnimCtrl(const char* animCtrlName);
extern void AddAnimClipToCtrl(const char* animCtrlName, const char* animClipName);

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);
