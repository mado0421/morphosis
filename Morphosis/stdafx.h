#pragma once

/**********************************************************************
Library
**********************************************************************/
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 

/**********************************************************************
Define
**********************************************************************/
#define WIN32_LEAN_AND_MEAN
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
#define CAM_Y_DISTANCE				0
#define CAM_MOVE_SPEED				3
#define CAM_ROTATE_SPEED			3
#define MOUSE_XSPEED				10
#define MOVE_SPEED					0.15f
#define ROTATE_SPEED				300
#define PO_PER_PLAYER				16
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define ASSETPATH					"Assets/"
#define LASSETPATH					L"Assets/"
#define	g_RootParameterCamera		0
#define	g_RootParameterObject		1
#define	g_RootParameterTexture		2
#define	g_RootParameterAnimation	3
#define	g_RootParameterUI			4
#define	g_fLevelScale				40.0f
#define	g_fDefaultUnitScale			20.0f
#define	g_fDefaultPlayerSpeed		(g_fDefaultUnitScale * 6)
#define	g_fDefaultProjectileSpeed   (g_fDefaultPlayerSpeed * 3.0f)
#define	g_nAnimBone					64
#define	g_nProjectilePerPlayer		30
#define	TIMER_ATT					0.05f
#define	TIMER_RESPANW				5.0f
#define	TEAM_DEFAULT				0
#define	TEAM_RED					1
#define	TEAM_BLUE					2
#define	g_DefaultHealthPoint		100
#define	g_RespawnDelay				3.0f
#define	g_DefaultDamage				10
#define	g_DefaultRPM				550
#define	g_DefaultProjectileLifeTime 5.0f
#define	g_Gravity					9.8f
#define	g_MouseInputSensitivity		50.0f

/**********************************************************************
Default Header
**********************************************************************/
#include "targetver.h"
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/**********************************************************************
C/C++ Header
**********************************************************************/
#include <vector>
#include <queue>
#include <list>
#include <set>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <string>
#include <wrl.h>
#include <shellapi.h>
#include "fmod.hpp"
#include "MathHelper.h"
#include "Defines.h"

/**********************************************************************
DirectX Header
**********************************************************************/
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader12.h"

/**********************************************************************
Namespace
**********************************************************************/
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
using Microsoft::WRL::ComPtr;

/**********************************************************************
extern
**********************************************************************/
//class	CTexture;
//class	CModel;
//class	CMesh;
//class	CAnimCtrl;
//struct	AnimClip;
//struct	AINode;

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);
extern void CreateAnimCtrl(const char * animControllerName);
extern void AddAnimClipToCtrl(const char * animCtrlName, const char * animClipName);
extern void CreateUIMesh(const char* meshName, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
extern void CreateModel(const char * meshName, const char* textureName, const char* modelName);



//extern vector<CTexture*>				g_vecTexture;
//extern vector<CMesh*>					g_vecMesh;
//extern vector<CModel*>					g_vecModel;
//extern vector<CAnimCtrl*>				g_vecAnimCtrl;
//extern vector<AnimClip*>				g_vecAnimClip;
//extern vector<AINode*>					g_vecAINode;
//extern vector<ID3D12PipelineState*>		g_vecPipelineStateObject;

class	FMOD::Sound;
class	FMOD::System;
class	FMOD::Channel;
extern	vector<FMOD::Sound *>			g_vecSound;
extern	FMOD::System					*g_System;
extern	FMOD::Channel					*g_Channel;

extern unsigned int	g_nCbvSrvDescriptorIncrementSize;
extern bool			g_IsMouseMode;

enum class SOUND { BGM,SHOT,count };
enum class ColliderTag { PROP,PROJECTILE,PLAYER };
enum class ColliderType{ BOX,SPHERE };
enum class MeshType { DefaulModel, AnimatedModel};
enum class ModelType {DefaultModel,AnimatedModel,FloatingUI,ETC};
enum class PSO {DefaultModel,AnimatedModel,FloatingUI,DefaultUI,Debug,count};
enum class Move {W,A,S,D,Q,E,SPACE,count};
enum class AnimationState {IDLE,RUN,FIRE,STARTJUMP,ENDJUMP,DIE,count};
enum class SceneType {LOBBY,MAINPLAY,count};
enum class MouseState {NONE,OVER,CLICKED};
namespace KEY {
	enum {
		A =65,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
		_0=48,_1,_2,_3,_4,_5,_6,_7,_8,_9
	};
}




struct	ID3D12PipelineState;
class CPipelineStateObject;
extern vector<ID3D12PipelineState*>		g_vecPipelineStateObject;
extern ID3D12RootSignature				*g_pd3dGraphicsRootSignature;
extern void CreateRootSignature(ID3D12Device *pd3dDevice);
extern void CreatePipelineStateObject(ID3D12Device * pd3dDevice, CPipelineStateObject* pso);
