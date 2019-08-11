#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "Camera.h"
#include "Texture.h"
//#include "Framework.h"
//#include "Importer.h"
//#include "PSO.h"
//#include "ObjectManager.h"


void CSceneMainPlay::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;

	int nProp = 1;
	int nUI = 1;
	int nObject = nProp + nUI;

	m_vecObject.resize(nProp);
	m_vecUIObject.resize(nUI);

	CreateDescriptorHeap(nObject);

	
	// �ؽ��ĵ� �Լ� �����ֱ�

	CreateConstantBufferResorce();
	CreateConstantBufferView();

	m_vecObject[0].m_xmf4x4World	= Matrix4x4::Identity();
	m_vecUIObject[0].m_xmf4x4World	= Matrix4x4::Identity();
	m_vecUIObject[0].CreateUIMesh(m_pd3dDevice, m_pd3dCommandList);

	m_pCamera = new CCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pCamera->SetPosition(XMFLOAT3(0,60,-40));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));


	//m_pd3dDevice		= pd3dDevice;
	//m_pd3dCommandList	= pd3dCommandList;
	//GetCursorPos(&m_ptOldCursorPos);

	//m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	//m_pCamera = new CCamera();
	//m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::MAINPLAY);
	//m_pCamera->SetTarget(m_ObjMng->GetTarget(0));
	//m_pCamera->SetOffset(XMFLOAT3(0, 60, -40));
	////m_pCamera->SetOffset(XMFLOAT3(0, 0, 0));
	//m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();

	//CPsoModel			ModelPso;
	//CPsoAnimatedModel	AnimatedModelPso;
	//CPsoFloatingUI		FloatingUIPso;
	//CPsoDefaultUI		DefaultUIPso;
	//ModelPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
	//AnimatedModelPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
	//FloatingUIPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
	//DefaultUIPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);

	//int i = 0;
	//while (i < 100) {
	//	//std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	//	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::MAINPLAY);
	//	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();
	//	if(m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	//	delete m_ObjMng;
	//	CPsoGenerator l_psoGenerator;
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoModel());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoAnimatedModel());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoFloatingUI());
	//	l_psoGenerator.Init(m_pd3dDevice, m_pd3dGraphicsRootSignature, new CPsoDefaultUI());
	//	g_vecPipelineStateObject.clear();
	//	//std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	//	i++;
	//	//system("cls");
	//	//cout << "[";
	//	//for (int j = 0; j < 10; ++j) {
	//	//	if (i > j * 10) cout << "/";
	//	//	else cout << ".";
	//	//}
	//	//cout << "] - " << i << "%\n";
	//	//cout << sec.count() << "sec\n";
	//}

	//g_System->playSound(g_vecSound[static_cast<int>(SOUND::BGM)], 0, false, &g_Channel);
	//g_Channel->setVolume(0.1f);
}

void CSceneMainPlay::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(g_pd3dGraphicsRootSignature);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	//// HLSL�� �־��� ī�޶� ���� ���źκ�
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::DefaultModel)]);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i].Render(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::FloatingUI)]);
	for (int i = 0; i < m_vecUIObject.size(); ++i) m_vecUIObject[i].Render(m_pd3dCommandList);

}

void CSceneMainPlay::Update(float fTimeElapsed)
{
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i].Update(fTimeElapsed);
	for (int i = 0; i < m_vecUIObject.size(); ++i) m_vecUIObject[i].Update(fTimeElapsed);
}


void CSceneMainPlay::ProcessInput(UCHAR * pKeysBuffer)
{
	//XMFLOAT2 cDelta{ 0,0 };
	//POINT ptCursorPos;
	////static float cameraOffsetZ = 60;
	////static float cameraOffsetY = 30;

	//if (pKeysBuffer[KEY::_1] & 0xF0) { g_IsMouseMode = false; }
	//if (pKeysBuffer[KEY::_2] & 0xF0) { g_IsMouseMode = true; }

	////if (pKeysBuffer[VK_UP] & 0xF0) { 
	////	cameraOffsetY += 0.1f; 
	////	m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	////}
	////if (pKeysBuffer[VK_DOWN] & 0xF0) { cameraOffsetY -= 0.1f; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	////}
	////if (pKeysBuffer[VK_LEFT] & 0xF0) { cameraOffsetZ += 0.1f; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	////}
	////if (pKeysBuffer[VK_RIGHT] & 0xF0) { cameraOffsetZ -= 0.1f; m_pCamera->SetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
	////}
	//


	//if (pKeysBuffer[KEY::_3] & 0xF0) { m_ObjMng->GetTarget(0)->Enable(); }



	//if (g_IsMouseMode) {
	//	GetCursorPos(&ptCursorPos);
	//	cDelta.x = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	//	//cDelta.x = 0;

	//	cDelta.y = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
	//	//cDelta.y = 0;
	//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//}

	//m_ObjMng->ProcessInput(pKeysBuffer, cDelta);
}

void CSceneMainPlay::Release()
{
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i].Release();
	for (int i = 0; i < m_vecUIObject.size(); ++i) m_vecUIObject[i].Release();
}

void CSceneMainPlay::CreateConstantBufferResorce()
{
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i].CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_vecUIObject.size(); ++i) m_vecUIObject[i].CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
}

void CSceneMainPlay::CreateConstantBufferView()
{
	int count = 0;
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i].CreateConstantBufferView(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle, count++);
	for (int i = 0; i < m_vecUIObject.size(); ++i) m_vecUIObject[i].CreateConstantBufferView(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle, count++);
}

//CScene::~CScene()
//{
//	Release();
//}

//void CScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
//{
//	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// �� ��Ʈ �ñ״�ó�� �� ��
//	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// �� ������ ���� �� ��
//
//	// HLSL�� �־��� ī�޶� ���� ���źκ�
//	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
//	m_pCamera->UpdateShaderVariables(pd3dCommandList);
//
//	m_ObjMng->Render();
//}

//void CScene::Release()
//{
//	for (int i = 0; i < g_vecPipelineStateObject.size(); ++i) g_vecPipelineStateObject[i]->Release();
//	
//	if (m_pFramework)							m_pFramework = NULL;
//	if (m_ObjMng)						delete	m_ObjMng;
//	if (m_pCamera)						delete	m_pCamera;
//	if (m_pd3dGraphicsRootSignature)			m_pd3dGraphicsRootSignature->Release();
//	if (m_pd3dCbvSrvDescriptorHeap)				m_pd3dCbvSrvDescriptorHeap->Release();
//}
//
//ID3D12RootSignature * CScene::CreateRootSignature(ID3D12Device * pd3dDevice)
//{
//	/*********************************************************************
//	2019-06-16
//	��Ʈ�Ķ���� �ε����� �������� �����ϱ� ���� �̷��� ���.
//	*********************************************************************/
//
//	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
//	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];	// Object, Texture and FloatingUI
//
//	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
//	pd3dDescriptorRanges[0].NumDescriptors = 1;
//	pd3dDescriptorRanges[0].BaseShaderRegister = g_RootParameterObject; //b1
//	pd3dDescriptorRanges[0].RegisterSpace = 0;
//	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;
//
//	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	pd3dDescriptorRanges[1].NumDescriptors = 1;
//	pd3dDescriptorRanges[1].BaseShaderRegister = g_RootParameterTexture; //t2
//	pd3dDescriptorRanges[1].RegisterSpace = 0;
//	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;
//
//	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
//	pd3dDescriptorRanges[2].NumDescriptors = 1;
//	pd3dDescriptorRanges[2].BaseShaderRegister = g_RootParameterUI; //b4
//	pd3dDescriptorRanges[2].RegisterSpace = 0;
//	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;
//
//	D3D12_ROOT_PARAMETER pd3dRootParameters[5];	//Camera, Obejct, texture, anim
//
//	//Camera
//	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	pd3dRootParameters[0].Descriptor.ShaderRegister = g_RootParameterCamera; //b0
//	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
//	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
//
//	//Objects
//	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
//	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
//	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
//
//	//Texture
//	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
//	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
//	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//	//Anim
//	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	pd3dRootParameters[3].Descriptor.ShaderRegister = g_RootParameterAnimation;	//b3
//	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
//	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//
//	//FloatingUI
//	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
//	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
//	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//
//	///*
//	//Root Signiature �߰� �ʿ�
//	//*/
//	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
//	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
//	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	d3dSamplerDesc.MipLODBias = 0;
//	d3dSamplerDesc.MaxAnisotropy = 1;
//	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//	d3dSamplerDesc.MinLOD = 0;
//	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
//	d3dSamplerDesc.ShaderRegister = 0;
//	d3dSamplerDesc.RegisterSpace = 0;
//	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
//	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
//	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
//	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
//	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
//	d3dRootSignatureDesc.NumStaticSamplers = 1;
//	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
//	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
//
//	ID3DBlob *pd3dSignatureBlob = NULL;
//	ID3DBlob *pd3dErrorBlob = NULL;
//	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
//	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
//	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
//	if (pd3dErrorBlob) pd3dErrorBlob->Release();
//
//	return(pd3dGraphicsRootSignature);
//}
//
//void CSceneLobby::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
//{
//	m_pd3dDevice = pd3dDevice;
//	m_pd3dCommandList = pd3dCommandList;
//	GetCursorPos(&m_ptOldCursorPos);
//
//
//	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);
//
//	m_pCamera = new CCamera();
//	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
//	m_pCamera->SetOffset(XMFLOAT3(0, 100.0f, -100.0f));
//
//	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList, SceneType::LOBBY);
//	//m_pCamera->SetTarget(m_ObjMng->GetTarget(0));
//	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();
//
//	CPsoModel			ModelPso;
//	CPsoAnimatedModel	AnimatedModelPso;
//	CPsoFloatingUI		FloatingUIPso;
//	CPsoDefaultUI		DefaultUIPso;
//	ModelPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
//	AnimatedModelPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
//	FloatingUIPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
//	DefaultUIPso.CreatePipelineStateDesc(pd3dDevice, m_pd3dGraphicsRootSignature);
//}
//
//void CSceneLobby::ProcessInput(UCHAR * pKeysBuffer)
//{
//	XMFLOAT2 cDelta{ 0,0 };
//	POINT ptCursorPos;
//
//	if (pKeysBuffer[KEY::_1] & 0xF0) { g_IsMouseMode = false; }
//	if (pKeysBuffer[KEY::_2] & 0xF0) { g_IsMouseMode = true; }
//
//	if (g_IsMouseMode) {
//		GetCursorPos(&ptCursorPos);
//		cDelta.x = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
//		cDelta.y = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
//		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
//	}
//
//	m_ObjMng->ProcessInput(pKeysBuffer, cDelta);
//}

void CScene::CreateDescriptorHeap(int nObject)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nObject + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * (nObject));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * (nObject));
}

void CScene::CreateTextureResourceView(CTexture * pTexture)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextureType = pTexture->GetTextureType();
	ID3D12Resource *pShaderResource = pTexture->GetTexture();
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
	m_pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::g_nCbvSrvDescriptorIncrementSize;

	pTexture->SetRootArgument(g_RootParameterTexture, d3dSrvGPUDescriptorHandle);
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::g_nCbvSrvDescriptorIncrementSize;
}