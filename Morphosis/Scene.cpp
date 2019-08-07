#include "stdafx.h"
#include "Framework.h"
#include "Scene.h"
#include "Importer.h"
#include "Object.h"
#include "Camera.h"
#include "PSO.h"
#include "Texture.h"


int g_RootParameterCamera;
int g_RootParameterObject;
int g_RootParameterTexture;
int g_RootParameterAnimation;
int g_RootParameterUI;
bool g_IsMouseMode = true;

ID3D12RootSignature * CSceneMainPlay::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	/*********************************************************************
	2019-06-16
	루트파라미터 인덱스를 전역에서 관리하기 위해 이렇게 사용.
	*********************************************************************/
	g_RootParameterCamera		= 0;
	g_RootParameterObject		= 1;
	g_RootParameterTexture		= 2;
	g_RootParameterAnimation	= 3;
	g_RootParameterUI			= 4;

	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];	// Object, Texture and FloatingUI

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = g_RootParameterObject; //b1
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = g_RootParameterTexture; //t2
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = g_RootParameterUI; //b4
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];	//Camera, Obejct, texture, anim

	//Camera
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = g_RootParameterCamera; //b0
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Objects
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//Anim
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = g_RootParameterAnimation;	//b3
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//FloatingUI
	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	///*
	//Root Signiature 추가 필요
	//*/
	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	/*HRESULT isSuccess = */D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	/*isSuccess = */pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CSceneMainPlay::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	GetCursorPos(&m_ptOldCursorPos);


	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//m_pCamera->SetOffset(XMFLOAT3(0, 30.0f, 50.0f));

	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::MAINPLAY);
	m_pCamera->SetTarget(m_ObjMng->GetTarget(0));
	m_pCamera->SetOffset(XMFLOAT3(0, 30, 60));
	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();
	CPsoGenerator l_psoGenerator;
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoModel());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoAnimatedModel());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoFloatingUI());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoDefaultUI());

	//int i = 0;
	//while (i < 100) {
	//	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	//	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::MAINPLAY);
	//	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();
	//	if(m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	//	delete m_ObjMng;
	//	CPsoGenerator l_psoGenerator;
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoModel());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoAnimatedModel());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoFloatingUI());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoDefaultUI());
	//	g_vecPSO.clear();
	//	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	//	i++;
	//	system("cls");
	//	cout << "[";
	//	for (int j = 0; j < 10; ++j) {
	//		if (i > j * 10) cout << "/";
	//		else cout << ".";
	//	}
	//	cout << "] - " << i << "%\n";
	//	cout << sec.count() << "sec\n";
	//}

	//FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_vecSound[static_cast<int>(SOUND::BGM)], 0, &channel);
	g_System->playSound(g_vecSound[static_cast<int>(SOUND::BGM)], 0, false, &g_Channel);
}


void CSceneMainPlay::ProcessInput(UCHAR * pKeysBuffer)
{
	float cxDelta = 0.0f/*, cyDelta = 0.0f*/;
	POINT ptCursorPos;
	static float cameraOffsetZ = 60;
	static float cameraOffsetY = 30;

	if (pKeysBuffer[KEY::_1] & 0xF0) { g_IsMouseMode = false; }
	if (pKeysBuffer[KEY::_2] & 0xF0) { g_IsMouseMode = true; }

	if (pKeysBuffer[VK_UP] & 0xF0) { 
		cameraOffsetY += 0.1; 
		m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	}
	if (pKeysBuffer[VK_DOWN] & 0xF0) { cameraOffsetY -= 0.1; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	}
	if (pKeysBuffer[VK_LEFT] & 0xF0) { cameraOffsetZ += 0.1; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	}
	if (pKeysBuffer[VK_RIGHT] & 0xF0) { cameraOffsetZ -= 0.1; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	}
	


	if (pKeysBuffer[KEY::_3] & 0xF0) { m_ObjMng->GetTarget(0)->Enable(); }



	if (g_IsMouseMode) {
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	m_ObjMng->ProcessInput(pKeysBuffer, cxDelta);
}

void CScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	m_ObjMng->Render();
}

void CScene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);
	m_ObjMng->Update(fTimeElapsed);
}

void CScene::Release()
{
	g_vecAnimController.clear();
	g_vecModel.clear();
	g_vecModel.clear();
	g_vecPSO.clear();
	g_vecTexture.clear();

	delete m_ObjMng;
	delete m_pCamera;
	m_pd3dGraphicsRootSignature->Release();
	m_pd3dCbvSrvDescriptorHeap->Release();

}

ID3D12RootSignature * CSceneLobby::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	/*********************************************************************
	2019-06-16
	루트파라미터 인덱스를 전역에서 관리하기 위해 이렇게 사용.
	*********************************************************************/
	g_RootParameterCamera = 0;
	g_RootParameterObject = 1;
	g_RootParameterTexture = 2;
	g_RootParameterAnimation = 3;
	g_RootParameterUI = 4;

	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];	// Object, Texture and FloatingUI

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = g_RootParameterObject; //b1
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = g_RootParameterTexture; //t2
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = g_RootParameterUI; //b4
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];	//Camera, Obejct, texture, anim

	//Camera
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = g_RootParameterCamera; //b0
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Objects
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//Anim
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = g_RootParameterAnimation;	//b3
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//FloatingUI
	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	///*
	//Root Signiature 추가 필요
	//*/
	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	/*HRESULT isSuccess = */D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	/*isSuccess = */pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CSceneLobby::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
	GetCursorPos(&m_ptOldCursorPos);


	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->SetOffset(XMFLOAT3(0, 100.0f, 100.0f));

	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::LOBBY);
	//m_pCamera->SetTarget(m_ObjMng->GetTarget(0));
	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();

	CPsoGenerator l_psoGenerator;
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoModel());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoAnimatedModel());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoFloatingUI());
	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoDefaultUI());
}

void CSceneLobby::ProcessInput(UCHAR * pKeysBuffer)
{
	float cxDelta = 0.0f/*, cyDelta = 0.0f*/;
	POINT ptCursorPos;

	if (pKeysBuffer[KEY::_1] & 0xF0) { g_IsMouseMode = false; }
	if (pKeysBuffer[KEY::_2] & 0xF0) { g_IsMouseMode = true; }

	if (g_IsMouseMode) {
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	m_ObjMng->ProcessInput(pKeysBuffer, cxDelta);
}
