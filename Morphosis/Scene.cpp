#include "stdafx.h"
#include "Framework.h"
#include "Scene.h"
#include "Importer.h"
#include "Camera.h"
#include "PSO.h"
#include "Texture.h"

#include "Mesh.h"
#include "Model.h"
#include "Prefab.h"
#include "Object.h"




CScene::CScene()
{
}

CScene::CScene(CFramework * pFramework)
{
	m_pFramework = pFramework;
}

void CScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

}

void CScene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);
}

void CScene::Release()
{
	delete m_pCamera;
	m_pd3dGraphicsRootSignature->Release();
	m_pd3dCbvSrvDescriptorHeap->Release();
}

/*
	텍스처 임포트 후에 호출 할 것.
	중간에 서술자 개수를 적어주는 곳에서 텍스처 개수를 받는다.
	힙손상의 원인이 되므로 호출 순서에 주의.
*/
void CScene::CreateDescriptorHeap(int nObject)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nObject + static_cast<UINT>( g_vecTexture.size() ) + 2;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(
		&d3dDescriptorHeapDesc, 
		__uuidof(ID3D12DescriptorHeap), 
		(void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nObject));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nObject));
	m_d3dUavCPUDescriptorStartHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	m_d3dUavGPUDescriptorStartHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
}

void CScene::SetComputeShaderResource()
{
	D3D12_RESOURCE_DESC d3dBufferResourceDesc;
	::ZeroMemory(&d3dBufferResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dBufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dBufferResourceDesc.Alignment = 0;
	d3dBufferResourceDesc.Width = 1920/*m_nWndClientWidth*/;
	d3dBufferResourceDesc.Height = 1080/*m_nWndClientHeight*/;
	d3dBufferResourceDesc.DepthOrArraySize = 1;
	d3dBufferResourceDesc.MipLevels = 1;
	d3dBufferResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dBufferResourceDesc.SampleDesc.Count = 1;
	d3dBufferResourceDesc.SampleDesc.Quality = 0;
	d3dBufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dBufferResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	HRESULT hResult = m_pd3dDevice->CreateCommittedResource(
		&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
		&d3dBufferResourceDesc, D3D12_RESOURCE_STATE_COMMON,
		NULL, __uuidof(ID3D12Resource), (void **)&m_pd3dComputeResource);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_pd3dComputeResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = m_pd3dComputeResource->GetDesc().Format;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	m_pd3dDevice->CreateShaderResourceView(m_pd3dComputeResource, &srvDesc, m_d3dSrvCPUDescriptorStartHandle);
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_d3dUavCPUDescriptorStartHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr;

	m_pd3dDevice->CreateUnorderedAccessView(m_pd3dComputeResource, NULL, &uavDesc, m_d3dUavCPUDescriptorStartHandle);
	//m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	//m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}

void CScene::CreateTextureResourceViews()
{
	for (int i = 0; i < g_vecTexture.size(); ++i)
		CreateTextureResourceView(g_vecTexture[i]);
}

/*+ static_cast<int>(static_cast<CTexture*>(pTexture)->GetType() )*/

void CScene::CreateTextureResourceView(void * pTexture)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;

	int nTextureType = static_cast<CTexture*>(pTexture)->GetTextureType();
	ID3D12Resource *pShaderResource = static_cast<CTexture*>(pTexture)->GetTexture();
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);

	m_pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);

	static_cast<CTexture*>(pTexture)->SetRootArgument(g_RootParameterTexture + static_cast<int>(static_cast<CTexture*>(pTexture)->GetType()), d3dSrvGPUDescriptorHandle);
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}

TestScene::TestScene()
{
}

TestScene::TestScene(CFramework * pFramework) : CScene(pFramework)
{
}

void TestScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	// 초기화 부분
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(m_pd3dDevice);
	m_pd3dComputeRootSignature = CreateComputeRootSignature(m_pd3dDevice);




	// 카메라 설정
	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->SetPosition(XMFLOAT3(0, 0, -200));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));


	// 실제적인 오브젝트 빌드

	// 필요한 리소스 로드해
	CImporter imp(m_pd3dDevice, m_pd3dCommandList);
	imp.ImportTexture(L"Textures/CobblestoneMedieval12_albedo",		"albedo",		TEXTURETYPE::ALBEDO);
	imp.ImportTexture(L"Textures/CobblestoneMedieval12_normal",		"normal",		TEXTURETYPE::NORMAL);
	imp.ImportTexture(L"Textures/CobblestoneMedieval12_ao",			"ao",			TEXTURETYPE::AO);
	imp.ImportTexture(L"Textures/CobblestoneMedieval12_height",		"height",		TEXTURETYPE::HEIGHT);
	imp.ImportTexture(L"Textures/CobblestoneMedieval12_roughness",	"roughness",	TEXTURETYPE::ROUGHNESS);

	//오브젝트 몇 개 쓸거야
	int nObject = 1;
	int count = 0;
	// 서술자 힙 생성
	CreateDescriptorHeap(nObject);

	CTestMesh* tempMesh = new CTestMesh(m_pd3dDevice, m_pd3dCommandList);
	tempMesh->name = "Plane";
	g_vecMesh.push_back(tempMesh);

	CModel* tempModel = new CModel();
	tempModel->SetMeshIdx(GetMeshIdx( "Plane" ));
	tempModel->SetAlbedoIdx(GetTextureIdx( "albedo" ));
	tempModel->SetNormalIdx(GetTextureIdx( "normal" ));
	tempModel->name = "TestPlaneModel";
	g_vecModel.push_back(tempModel);

	CPrefab* tempPrefab = new CPrefab();
	tempPrefab->m_vecModelIdx.push_back(GetModelIdx( "TestPlaneModel" ));
	tempPrefab->name = "TestPlanePrefab";
	g_vecPrefab.push_back(tempPrefab);

	Object* tempObj = new Object();
	tempObj->SetPrefabIdx(GetPrefabIdx( "TestPlanePrefab" ));
	tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);

	//tempObj->Move(XMFLOAT3(0, -50, 0));
	tempObj->Rotate(XMFLOAT3(-90, 0, 0));

	m_vecObject.push_back(tempObj);



	// 필수적인 생성 부분
	CreateTextureResourceViews();
	CreateConstantBufferResorce();
	CreateConstantBufferView();

	SetComputeShaderResource();

	// 파이프라인 스테이트 오브젝트 생성
	CPsoModel pso;
	//TestPSO pso;
	pso.Initialize(m_pd3dDevice, m_pd3dGraphicsRootSignature);
	ComputePSO cpso;
	cpso.Initialize(m_pd3dDevice, m_pd3dComputeRootSignature);
}

void TestScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPSO[0]);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->Render(pd3dCommandList);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = g_vecTexture.front()->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	pd3dCommandList->SetComputeRootSignature(m_pd3dComputeRootSignature);

	d3dResourceBarrier.Transition.pResource = g_vecTexture.front()->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	d3dResourceBarrier.Transition.pResource = m_pd3dComputeResource;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pd3dCommandList->SetPipelineState(g_vecPSO[1]);
	m_pd3dCommandList->SetComputeRootDescriptorTable(0, m_d3dSrvGPUDescriptorStartHandle);
	m_pd3dCommandList->SetComputeRootDescriptorTable(1, m_d3dUavGPUDescriptorStartHandle);
	UINT numGroupsX = (UINT)ceilf(1920 / 256.0f);
	m_pd3dCommandList->Dispatch(numGroupsX, 1080, 1);

	d3dResourceBarrier.Transition.pResource = m_pd3dComputeResource;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

}

void TestScene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->Update(fTimeElapsed);
}

void TestScene::ProcessInput(UCHAR * pKeysBuffer)
{
	if (pKeysBuffer[87] & 0xF0) { m_pCamera->MoveForward(); }	// W
	if (pKeysBuffer[65] & 0xF0) { m_pCamera->MoveLeft(); }		// A
	if (pKeysBuffer[68] & 0xF0) { m_pCamera->MoveRight(); }		// D
	if (pKeysBuffer[83] & 0xF0) { m_pCamera->MoveBackward(); }	// S
	//if (pKeysBuffer[VK_SPACE] & 0xF0) { g_temp = 0; }
}

void TestScene::Release()
{
}

ID3D12RootSignature * TestScene::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[4];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 1; //b1
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 2; //b2
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 4; //t4
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 5; //t5
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

	//Camera
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //b0
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Objects
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//FloatingUI
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Anim
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3;	//b3
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//DiffTexture
	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//NormalTexture
	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	///*
	//Root Signiature 추가 필요
	//*/
	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
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
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature * TestScene::CreateComputeRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pd3dRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6;
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7;
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	//DiffTexture
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[0].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//UAV Texture Resource
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dRootSignature);
}

void TestScene::CreateConstantBufferResorce()
{
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
}

void TestScene::CreateConstantBufferView()
{
	int count = 0;
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->CreateConstantBufferView(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle, count);
}
