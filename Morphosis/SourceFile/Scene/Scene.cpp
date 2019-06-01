#include "stdafx.h"
#include "Framework/Framework.h"
#include "Scene.h"
#include "Importer/Importer.h"


#define MOUSE_XSPEED 10
#define MOVE_SPEED 1.5
#define ROTATE_SPEED 300
#define PO_PER_PLAYER 16


D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

CScene::CScene()
{
}


CScene::~CScene()
{
}



ID3D12RootSignature * CScene::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[RootDescriptor::count];

	pd3dDescriptorRanges[RootDescriptor::OBJECT].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[RootDescriptor::OBJECT].NumDescriptors = 1;
	pd3dDescriptorRanges[RootDescriptor::OBJECT].BaseShaderRegister = RootParameter::OBJECT; //Game Objects
	pd3dDescriptorRanges[RootDescriptor::OBJECT].RegisterSpace = 0;
	pd3dDescriptorRanges[RootDescriptor::OBJECT].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[RootDescriptor::TEXTURE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[RootDescriptor::TEXTURE].NumDescriptors = 1;
	pd3dDescriptorRanges[RootDescriptor::TEXTURE].BaseShaderRegister = RootParameter::TEXTURE; //Texture
	pd3dDescriptorRanges[RootDescriptor::TEXTURE].RegisterSpace = 0;
	pd3dDescriptorRanges[RootDescriptor::TEXTURE].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[RootDescriptor::UI].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[RootDescriptor::UI].NumDescriptors = 1;
	pd3dDescriptorRanges[RootDescriptor::UI].BaseShaderRegister = RootParameter::UI; //UI
	pd3dDescriptorRanges[RootDescriptor::UI].RegisterSpace = 0;
	pd3dDescriptorRanges[RootDescriptor::UI].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[RootParameter::count];

	pd3dRootParameters[RootParameter::CAMERA].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[RootParameter::CAMERA].Descriptor.ShaderRegister = RootParameter::CAMERA; //CCamera
	pd3dRootParameters[RootParameter::CAMERA].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::CAMERA].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[RootParameter::OBJECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[RootParameter::OBJECT].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[RootParameter::OBJECT].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[RootDescriptor::OBJECT]; //Game Objects
	pd3dRootParameters[RootParameter::OBJECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[RootParameter::MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[RootParameter::MATERIAL].Descriptor.ShaderRegister = RootParameter::MATERIAL; //Materials
	pd3dRootParameters[RootParameter::MATERIAL].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[RootParameter::LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[RootParameter::LIGHT].Descriptor.ShaderRegister = RootParameter::LIGHT; //Lights
	pd3dRootParameters[RootParameter::LIGHT].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[RootParameter::TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[RootParameter::TEXTURE].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[RootParameter::TEXTURE].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[RootDescriptor::TEXTURE]; //Texture
	pd3dRootParameters[RootParameter::TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[RootParameter::VERTEX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[RootParameter::VERTEX].Descriptor.ShaderRegister = RootParameter::VERTEX; //t5
	pd3dRootParameters[RootParameter::VERTEX].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::VERTEX].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[RootParameter::UV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[RootParameter::UV].Descriptor.ShaderRegister = RootParameter::UV; //t6
	pd3dRootParameters[RootParameter::UV].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::UV].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[RootParameter::UI].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[RootParameter::UI].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[RootParameter::UI].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[RootDescriptor::UI]; //Game UI
	pd3dRootParameters[RootParameter::UI].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[RootParameter::ANIM].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[RootParameter::ANIM].Descriptor.ShaderRegister = RootParameter::ANIM; //t8
	pd3dRootParameters[RootParameter::ANIM].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[RootParameter::ANIM].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	/*
	Root Signiature 추가 필요
	*/
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
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	//	assert(isSuccess == S_OK);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CGroundScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	m_pFramework = (CFramework*)pContext;
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
	GetCursorPos(&m_ptOldCursorPos);

	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	m_ppPSO = new ID3D12PipelineState*[GS::PSO::count];

	CPSO **tempPso = new CPSO*[GS::PSO::count];
	tempPso[0] = new CDefaultPSO();
	tempPso[0]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_ppPSO[0] = tempPso[0]->GetPipelineState();
	tempPso[1] = new CAnimatedPSO();
	tempPso[1]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_ppPSO[1] = tempPso[1]->GetPipelineState();

	//===================================================================================
	// Create ObjectMng 
	//===================================================================================

	//m_pObjectMng = new ObjectManager();

	//===================================================================================
	// Descriptor Heap
	//===================================================================================

	//int nObjects = m_pObjectMng->GetNumTotalObjects();
		//= m_nObjCollTerrain
		//+ m_nObjProp
		//+ m_nObjRenderTerrain
		//+ m_nObjProjectile
		//+ m_nObjPlayer;]
	int nObjects = 0;
	int nMaxBone = 64;
	int nMaxMat = 64;
	int nMaxLight = 64;
	int nSRVForTextrue = 1;
	int nDescriptors = nObjects + nMaxBone + nMaxMat + nMaxLight + nSRVForTextrue;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nDescriptors;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT result = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nDescriptors - nSRVForTextrue));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nDescriptors - nSRVForTextrue));

	//m_pObjectMng->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr);

	//===================================================================================
	// Textures
	//===================================================================================

	int nTexture = 6;

	CTexture **textures = new CTexture*[nTexture];
	wchar_t **text = new wchar_t*[nTexture];
	text[0] = L"Assets/Textures/TEST/box_diff.dds";
	text[1] = L"Assets/Textures/TEST/character_2_diff_test3.dds";
	text[2] = L"Assets/Textures/TEST/wall_test_diff.dds";
	text[3] = L"Assets/Textures/TEST/crosshair_test_diff.dds";
	text[4] = L"Assets/Textures/TEST/hpbar_in.dds";
	text[5] = L"Assets/Textures/TEST/hpbar_out.dds";
	for (int i = 0; i < nTexture; ++i) {
		textures[i] = new CTexture(RESOURCE_TEXTURE2D);
		textures[i]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, text[i]);
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, textures[i], GS::RootParameter::TEXTURE, false);
		//m_pObjectMng->AddTexture(textures[i]);
	}

	//===================================================================================
	// Meshes
	//===================================================================================

	int nMeshes = 1;

	CTestMesh *pMesh = new CTestMesh(pd3dDevice, pd3dCommandList, 100.0f);
	//m_pObjectMng->AddMesh(pMesh);

	//===================================================================================
	// Create CB Objects
	//===================================================================================

	//AllocUploadBuffer(
	//	m_cbDescObj, sizeof(CB_OBJECT_INFO) * nObjects,
	//	m_pcbUploadBufferResource, UploadBufferCurrentIdx, m_pcbMappedObj);
	//AllocUploadBuffer(
	//	m_cbDescAnim, sizeof(XMMATRIX) * nMaxBone,
	//	m_pcbUploadBufferResource, UploadBufferCurrentIdx, m_pcbMappedAnim);
	//AllocUploadBuffer(
	//	m_cbDescMat, sizeof(XMMATRIX) * nMaxMat,
	//	m_pcbUploadBufferResource, UploadBufferCurrentIdx, m_pcbMappedMat);
	//AllocUploadBuffer(
	//	m_cbDescLight, sizeof(XMMATRIX) * nMaxLight,
	//	m_pcbUploadBufferResource, UploadBufferCurrentIdx, m_pcbMappedLight);
	//D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_cbDescObj.view_desc.BufferLocation;
	//for (int j = 0; j < nPlayers; j++)
	//{
	//	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (m_cbDescObj.nMappedData * j);
	//	D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
	//	d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
	//	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	//}

	//m_pObjectMng->Initialize(pd3dDevice, pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle);

	//m_pCamera->SetTarget(m_pObjectMng->GetTarget(0));
}

void CGroundScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_ppPSO) pd3dCommandList->SetPipelineState(m_ppPSO[0]);

	//m_pObjectMng->RenderTerrainMeshes(pd3dCommandList);
	//m_pObjectMng->RenderCollideObjects(pd3dCommandList);
	//m_pObjectMng->RenderMovingObjects(pd3dCommandList);
	//m_pObjectMng->RenderParticles(pd3dCommandList);
	//m_pObjectMng->RenderEffects(pd3dCommandList);
}

void CGroundScene::Update(float fTimeElapsed)
{
	// 여기서 GPU에서 사용할 변수들 갱신을 해줘야 한다!
	// 카메라, 오브젝트 등등
	//m_pObjectMng->Update(fTimeElapsed);
}

ID3D12RootSignature * CGroundScene::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[GS::RootDescriptor::count];	// Object and Texture

	pd3dDescriptorRanges[GS::RootDescriptor::OBJECT].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[GS::RootDescriptor::OBJECT].NumDescriptors = 1;
	pd3dDescriptorRanges[GS::RootDescriptor::OBJECT].BaseShaderRegister = 1; //b1
	pd3dDescriptorRanges[GS::RootDescriptor::OBJECT].RegisterSpace = 0;
	pd3dDescriptorRanges[GS::RootDescriptor::OBJECT].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE].NumDescriptors = 1;
	pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE].BaseShaderRegister = 2; //t2
	pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE].RegisterSpace = 0;
	pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[GS::RootParameter::count];	//Camera, Obejct, texture, anim, mat, light

	//Camera
	pd3dRootParameters[GS::RootParameter::CAMERA].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[GS::RootParameter::CAMERA].Descriptor.ShaderRegister = 0; //b0
	pd3dRootParameters[GS::RootParameter::CAMERA].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[GS::RootParameter::CAMERA].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Objects
	pd3dRootParameters[GS::RootParameter::OBJECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[GS::RootParameter::OBJECT].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[GS::RootParameter::OBJECT].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[GS::RootDescriptor::OBJECT];
	pd3dRootParameters[GS::RootParameter::OBJECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture
	pd3dRootParameters[GS::RootParameter::TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[GS::RootParameter::TEXTURE].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[GS::RootParameter::TEXTURE].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[GS::RootDescriptor::TEXTURE];
	pd3dRootParameters[GS::RootParameter::TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//Anim
	pd3dRootParameters[GS::RootParameter::ANIM].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[GS::RootParameter::ANIM].Descriptor.ShaderRegister = 3;
	pd3dRootParameters[GS::RootParameter::ANIM].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[GS::RootParameter::ANIM].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//Mat
	pd3dRootParameters[GS::RootParameter::MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[GS::RootParameter::MATERIAL].Descriptor.ShaderRegister = 4;
	pd3dRootParameters[GS::RootParameter::MATERIAL].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[GS::RootParameter::MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Light
	pd3dRootParameters[GS::RootParameter::LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[GS::RootParameter::LIGHT].Descriptor.ShaderRegister = 5;
	pd3dRootParameters[GS::RootParameter::LIGHT].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[GS::RootParameter::LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CGroundScene::CreateObjectBuffers()
{
}

void CGroundScene::UpdateObjectBuffers()
{
}

void CGroundScene::ReleaseObjectBuffers()
{
}

void CGroundScene::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT result = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

void CGroundScene::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CGroundScene::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews)
{
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * nConstantBufferViews, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}


void CGroundScene::CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture();
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);




		m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootArgument((bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, d3dSrvGPUDescriptorHandle);
		m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

void CGroundScene::ReleaseShaderVariables()
{
	CScene::ReleaseShaderVariables();
}

CLoadingScene::CLoadingScene()
{
}

CLoadingScene::~CLoadingScene()
{
}

void CLoadingScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
}

void CLoadingScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CLoadingScene::Update(float fTimeElapsed)
{
}

void CLoadingScene::CreateObjectBuffers()
{
}

void CLoadingScene::UpdateObjectBuffers()
{
}

void CLoadingScene::ReleaseObjectBuffers()
{
}

CEnterRoomScene::CEnterRoomScene()
{
}

CEnterRoomScene::~CEnterRoomScene()
{
}

void CEnterRoomScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	//CGroundScene::Initialize(pd3dDevice, pd3dCommandList, pContext);

	//m_pLevel = new CLevelData();
	//m_pLevel->FileRead("Assets/Levels/Level_2");

	//m_pd3dDevice = pd3dDevice;
	//m_pd3dCommandList = pd3dCommandList;

	//m_nObjects = m_pLevel->m_nLevelBlocks + 3;
	//m_ppObjects = new CCollideObejct*[m_nObjects];

	//m_nPlayers = m_pLevel->m_nSpawnPoints;
	//m_ppPlayers = new CPlayerObject*[m_nPlayers];

	//m_nProjectileObjects = m_nPlayers * PO_PER_PLAYER;
	//m_ppProjectileObjects = new CProjectileObject*[m_nProjectileObjects];

	//int nObjects = m_nObjects + m_nPlayers + m_nProjectileObjects;

	//// Camera 초기화
	//m_pCamera = new CFollowCamera();
	//m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//// RootSignature 초기화
	//m_pd3dGraphicsRootSignature = CreateRootSignature(m_pd3dDevice);

	//// PSO 초기화
	//// 저기서 RootSignature 쓰니까 그 전에 RootSignature 만들어줘야 함
	//m_nPipelineStates = PSO::count;
	//m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	//CPSO ** ppCPSOs = new CPSO*[m_nPipelineStates];
	//ppCPSOs[PSO::TEXTURE] = new CTexturedPSO();
	//ppCPSOs[PSO::ILLUMINATEDTEXTURE] = new CTexturedIlluminatedPSO();
	//ppCPSOs[PSO::MODEL] = new CModelPSO();
	//ppCPSOs[PSO::DEBUG] = new CDebugPSO();

	//ppCPSOs[PSO::TEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::ILLUMINATEDTEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::MODEL]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::DEBUG]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);

	//m_ppPipelineStates[PSO::TEXTURE] = ppCPSOs[PSO::TEXTURE]->GetPipelineState();
	//m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE] = ppCPSOs[PSO::ILLUMINATEDTEXTURE]->GetPipelineState();
	//m_ppPipelineStates[PSO::MODEL] = ppCPSOs[PSO::MODEL]->GetPipelineState();
	//m_ppPipelineStates[PSO::DEBUG] = ppCPSOs[PSO::DEBUG]->GetPipelineState();


	//// 메쉬만드는 곳
	//CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList);
	//CTestMesh *pTestMesh2 = new CTestMesh(pd3dDevice, pd3dCommandList, 5);
	//CModelMesh *pTestModelMesh = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/character_2_com3");
	//CModelMesh **ppLevelMeshes = NULL;

	//int nLevelMeshes = 6;
	//ppLevelMeshes = new CModelMesh*[nLevelMeshes];
	////ppLevelMeshes[0] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_box1");
	////ppLevelMeshes[1] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_oilTank001");
	////ppLevelMeshes[2] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_oilTank002");
	////ppLevelMeshes[3] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_upFloor");
	////ppLevelMeshes[4] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_level");
	////ppLevelMeshes[5] = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/map_lobby_stair1");


	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//CreateCbvAndSrvDescriptorHeaps(m_pd3dDevice, m_pd3dCommandList, nObjects, 1);
	//CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList, nObjects);
	//CreateConstantBufferViews(m_pd3dDevice, m_pd3dCommandList, nObjects, m_pd3dcbObjects, ncbElementBytes);

	//// 마테리얼에 텍스처 등록하는 곳
	//m_nMaterial = 3;
	//m_ppMaterial = new CMaterial*[m_nMaterial];
	//{
	//	CTexture *pTexture = new CTexture(RESOURCE_TEXTURE2D);
	//	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Textures/TEST/box_diff.dds");
	//	CMaterial *mat = new CMaterial();
	//	mat->SetTexture(pTexture);
	//	mat->SetReflection(1);
	//	m_ppMaterial[0] = mat;
	//	CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, pTexture, RootParameter::TEXTURE, false);
	//}
	//{
	//	CTexture *pTexture = new CTexture(RESOURCE_TEXTURE2D);
	//	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Textures/TEST/character_2_diff_test3.dds");
	//	CMaterial *mat = new CMaterial();
	//	mat->SetTexture(pTexture);
	//	mat->SetReflection(1);
	//	m_ppMaterial[1] = mat;
	//	CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, pTexture, RootParameter::TEXTURE, false);
	//}
	//{
	//	CTexture *pTexture = new CTexture(RESOURCE_TEXTURE2D);
	//	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Textures/TEST/wall_test_diff.dds");
	//	CMaterial *mat = new CMaterial();
	//	mat->SetTexture(pTexture);
	//	mat->SetReflection(1);
	//	m_ppMaterial[2] = mat;
	//	CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, pTexture, RootParameter::TEXTURE, false);
	//}

	///*여기가 박스 모델*/
	//for (int i = 0; i < m_nObjects; i++) {
	//	if (i < m_pLevel->m_nLevelBlocks) {

	//		CCollideObejct *pObj = new CCollideObejct();
	//		CTestMesh *pLevelMesh = new CTestMesh(pd3dDevice, pd3dCommandList, m_pLevel->m_pLevelBlocks[i].extent);
	//		XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//		CModel *model = new CModel();
	//		model->AddMesh(pLevelMesh);

	//		pObj->SetModel(model);
	//		pObj->SetPosition(m_pLevel->m_pLevelBlocks[i].pos);
	//		pObj->SetOOBB(m_pLevel->m_pLevelBlocks[i].pos, m_pLevel->m_pLevelBlocks[i].extent, orientation);
	//		pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);

	//		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
	//		m_ppObjects[i] = pObj;
	//	}
	//}

	///*여기가 움직이는 모델*/
	//for (int i = 0; i < m_nPlayers; i++) {
	//	CPlayerObject *pObj = new CPlayerObject();

	//	XMFLOAT3 pos = XMFLOAT3(m_pLevel->m_pSpawnPoints[i]);
	//	XMFLOAT3 extents = pTestModelMesh->GetExtents();			//반지름 아니고 지름임
	//	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//	CModel *model = new CModel();
	//	model->AddMesh(pTestModelMesh);

	//	pObj->SetModel(model);
	//	//pObj->SetMesh(0, pTestModelMesh);
	//	pObj->SetPosition(pos);
	//	pObj->SetMaterial(m_ppMaterial[1]);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects)+i));

	//	pObj->Initialize();
	//	pObj->SetTeam(i % 2);
	//	pObj->SetOOBB(pos, extents, orientation);
	//	pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);
	//	pObj->AddRotateAngle(XMFLOAT3(0.0f, 90.0f * i, 0.0f));
	//	m_ppPlayers[i] = pObj;
	//}

	//for (int i = 0; i < m_nProjectileObjects; i++) {
	//	CProjectileObject *pObj = new CProjectileObject();

	//	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	XMFLOAT3 extents = XMFLOAT3(5.0f, 5.0f, 5.0f);			//반지름 아니고 지름임
	//	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//	CModel *model = new CModel();
	//	model->AddMesh(pTestMesh2);

	//	pObj->SetModel(model);

	//	//pObj->SetMesh(0, pTestMesh2);
	//	pObj->SetPosition(0.0f, 0.0f, 0.0f);
	//	pObj->SetMaterial(m_ppMaterial[0]);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects + m_nPlayers) + i));

	//	pObj->Initialize();
	//	pObj->SetTeam((i / PO_PER_PLAYER) % 2);
	//	pObj->isAlive = false;

	//	pObj->SetOOBB(pos, extents, orientation);
	//	pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);

	//	m_ppProjectileObjects[i] = pObj;
	//}

	//// 처음 따라갈 캐릭터 정해주기
	//m_pCamera->SetTarget(m_ppPlayers[0]);

}

void CEnterRoomScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	//// HLSL에 넣어줄 카메라 정보 갱신부분
	//m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//m_pCamera->UpdateShaderVariables(pd3dCommandList);

	////테스트용 Object들 내용 갱신부분
	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//for (int i = 0; i < m_nObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (i * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
	//}

	//for (int i = 0; i < m_nPlayers; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects) * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppPlayers[i]->m_xmf4x4World)));
	//}

	//for (int i = 0; i < m_nProjectileObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects + m_nPlayers) * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppProjectileObjects[i]->m_xmf4x4World)));
	//}

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::MODEL]);
	//for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->Render(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE]);
	//for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList, m_pCamera);
	//for (int i = 0; i < m_nProjectileObjects; ++i) if (!m_ppProjectileObjects[i]->isAlive) m_ppProjectileObjects[i]->Render(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::DEBUG]);
	//for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->TestRender(pd3dCommandList, m_pCamera);
	//for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->TestRender(pd3dCommandList, m_pCamera);
	//for (int i = 0; i < m_nProjectileObjects; ++i) if (!m_ppProjectileObjects[i]->isAlive) m_ppProjectileObjects[i]->TestRender(pd3dCommandList, m_pCamera);

}

void CEnterRoomScene::Update(float fTimeElapsed)
{
	//for (int i = 0; i < m_nProjectileObjects; i++) if (!m_ppProjectileObjects[i]->isAlive) m_ppProjectileObjects[i]->MoveOOBB(fTimeElapsed);

	//for (int i = 0; i < m_nPlayers; i++)
	//	if (!m_ppPlayers[i]->IsDead()) {
	//		/*탄과 충돌체크*/
	//		for (int j = 0; j < m_nProjectileObjects; ++j)
	//			if (m_ppPlayers[i]->m_team != m_ppProjectileObjects[j]->m_team)
	//				if (!m_ppProjectileObjects[j]->isAlive)
	//					if (m_ppPlayers[i]->IsCollide(m_ppProjectileObjects[j]->m_collisionBox)) {
	//						printf("col");
	//						m_ppProjectileObjects[j]->isAlive = false;
	//						m_ppPlayers[i]->Damaged(10);
	//						break;
	//					}

	//		//먼저 이동
	//		m_ppPlayers[i]->MoveOOBB(fTimeElapsed);
	//		/*지형과 충돌체크*/
	//		for (int j = 0; j < m_nObjects; ++j)
	//			if (m_ppPlayers[i]->IsCollide(m_ppObjects[j]->m_collisionBox)) {
	//				//m_ppPlayers[i]->isCollide = true;
	//				//충돌했으니까 이동은 무효로
	//				m_ppPlayers[i]->MoveOOBB(-fTimeElapsed);
	//				break;
	//			}

	//		//먼저 떨어짐
	//		m_ppPlayers[i]->FallingOOBB(fTimeElapsed);
	//		m_ppPlayers[i]->isFalling = true;

	//		for (int j = 0; j < m_nObjects; ++j)
	//			if (m_ppPlayers[i]->IsCollide(m_ppObjects[j]->m_collisionBox)) {
	//				//충돌했으니까 이동은 무효로
	//				m_ppPlayers[i]->isFalling = false;
	//				m_ppPlayers[i]->m_collisionBox.Center.y = m_ppPlayers[i]->prevHeight;
	//				m_ppPlayers[i]->m_fGravityAccel = 0;
	//				break;
	//			}
	//	}

	////가만히 있는 오브젝트를 갱신을 해줘야 할까? 저는 아니라고 생각합니다.
	//for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->Update(fTimeElapsed);
	//for (int i = 0; i < m_nProjectileObjects; i++) if (!m_ppProjectileObjects[i]->isAlive) m_ppProjectileObjects[i]->Update(fTimeElapsed);

	//m_pCamera->Update(fTimeElapsed);
}

void CEnterRoomScene::ProcessInput(UCHAR * pKeysBuffer)
{
	//float cxDelta = 0.0f, cyDelta = 0.0f;
	//POINT ptCursorPos;

	//GetCursorPos(&ptCursorPos);
	//cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
	////	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);

	////if (cxDelta) m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, cxDelta * MOUSE_XSPEED, 0 });

	//XMFLOAT3 xmf3temp;
	//if (pKeysBuffer[KEY::W] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetLook(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED)); }
	//if (pKeysBuffer[KEY::A] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetRight(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); }
	//if (pKeysBuffer[KEY::S] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetLook(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); }
	//if (pKeysBuffer[KEY::D] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetRight(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED)); }
	//if (pKeysBuffer[KEY::Q] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, -ROTATE_SPEED, 0 }); }
	//if (pKeysBuffer[KEY::E] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, ROTATE_SPEED, 0 }); }

	//if (pKeysBuffer[KEY::_1] & 0xF0)
	//	if (m_pCamera->GetTarget() != m_ppPlayers[0])
	//		m_pCamera->SetTarget(m_ppPlayers[0]);

	//if (pKeysBuffer[KEY::_2] & 0xF0)
	//	if (m_pCamera->GetTarget() != m_ppPlayers[1])
	//		m_pCamera->SetTarget(m_ppPlayers[1]);

	//if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
	//	//attack

	//	if (m_ppPlayers[0]->IsFireable()) {
	//		m_ppPlayers[0]->Attack();
	//		for (int i = 0; i < PO_PER_PLAYER; ++i) {
	//			if (!m_ppProjectileObjects[(m_nPlayers * 0) + i]->isAlive) {
	//				m_ppProjectileObjects[(m_nPlayers * 0) + i]->isAlive = true;
	//				m_ppProjectileObjects[(m_nPlayers * 0) + i]->Initialize(m_ppPlayers[0]);
	//				printf("fire");
	//				break;
	//			}
	//		}
	//	}
	//}

	//if (pKeysBuffer[VK_SPACE] & 0xF0) {
	//	//jump
	//	if (!m_ppPlayers[0]->isFalling) m_ppPlayers[0]->Jump();
	//}

	//if (pKeysBuffer[VK_RETURN] & 0xF0) {
	//	//jump
	//	m_pFramework->ChangeScene(Scenes::PLAY, m_pFramework);

	//	//XMFLOAT4X4 matrix = m_ppPlayers[0]->m_xmf4x4World;
	//	//printf("matrix is\n");
	//	//printf("%f %f %f %f\n", matrix._11, matrix._12, matrix._13, matrix._14);
	//	//printf("%f %f %f %f\n", matrix._21, matrix._22, matrix._23, matrix._24);
	//	//printf("%f %f %f %f\n", matrix._31, matrix._32, matrix._33, matrix._34);
	//	//printf("%f %f %f %f\n", matrix._41, matrix._42, matrix._43, matrix._44);
	//}

}

void CEnterRoomScene::OnProcessingMouseMessage()
{
}

void CEnterRoomScene::OnProcessingKeyboardMessage()
{
}

void CEnterRoomScene::CreateObjectBuffers()
{
}

void CEnterRoomScene::UpdateObjectBuffers()
{
}

void CEnterRoomScene::ReleaseObjectBuffers()
{
}

CPlayScene::CPlayScene()
{
}

CPlayScene::~CPlayScene()
{
}

void CPlayScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	CGroundScene::Initialize(pd3dDevice, pd3dCommandList, pContext);
	//CGroundScene::Initialize(pd3dDevice, pd3dCommandList, pContext);
	//m_pd3dDevice = pd3dDevice;
	//m_pd3dCommandList = pd3dCommandList;

	////LoadLevel("Level_0");
	//m_pLevel = new CLevelData();
	//m_pLevel->FileRead("Assets/Levels/Level_0.dat");

	////CreateObjects();
	//m_nObjects = m_pLevel->m_nLevelBlocks;
	//m_ppObjects = new CCollideObejct*[m_nObjects];

	//m_nPlayers = m_pLevel->m_nSpawnPoints;
	//m_ppPlayers = new CPlayerObject*[m_nPlayers];

	//m_nProjectileObjects = m_nPlayers * PO_PER_PLAYER;
	//m_ppProjectileObjects = new CProjectileObject*[m_nProjectileObjects];

	//int nObjects = m_nObjects + m_nPlayers + m_nProjectileObjects;

	//m_nUIObjects = 3;
	//m_ppUIObjects = new CDefaultUI*[m_nUIObjects];



	//// Camera 초기화
	////InitCamera();
	//m_pCamera = new CFollowCamera();
	//m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//// RootSignature 초기화
	//m_pd3dGraphicsRootSignature = CreateRootSignature(m_pd3dDevice);

	//// PSO 초기화
	//// 저기서 RootSignature 쓰니까 그 전에 RootSignature 만들어줘야 함












	////InitPSOs();
	//m_nPipelineStates = PSO::count;
	//m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	//CPSO ** ppCPSOs = new CPSO*[m_nPipelineStates];
	//ppCPSOs[PSO::TEXTURE] = new CTexturedPSO();
	//ppCPSOs[PSO::ILLUMINATEDTEXTURE] = new CTexturedIlluminatedPSO();
	//ppCPSOs[PSO::MODEL] = new CModelPSO();
	//ppCPSOs[PSO::DEBUG] = new CDebugPSO();
	//ppCPSOs[PSO::UI] = new CUIPSO();

	//ppCPSOs[PSO::TEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::ILLUMINATEDTEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::MODEL]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::DEBUG]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	//ppCPSOs[PSO::UI]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);

	//m_ppPipelineStates[PSO::TEXTURE] = ppCPSOs[PSO::TEXTURE]->GetPipelineState();
	//m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE] = ppCPSOs[PSO::ILLUMINATEDTEXTURE]->GetPipelineState();
	//m_ppPipelineStates[PSO::MODEL] = ppCPSOs[PSO::MODEL]->GetPipelineState();
	//m_ppPipelineStates[PSO::DEBUG] = ppCPSOs[PSO::DEBUG]->GetPipelineState();
	//m_ppPipelineStates[PSO::UI] = ppCPSOs[PSO::UI]->GetPipelineState();

	//// 메쉬만드는 곳
	//// 메쉬 만드는 것도 levelData 만들면서 모델 임포트 하고 마테리얼 임포트하고 해야하는거 아님?
	//// 아 유니티 쓰지 말고 언리얼 쓰자
	//CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList);
	//CTestMesh *pTestMesh2 = new CTestMesh(pd3dDevice, pd3dCommandList, 5);
	//CModelMesh *pTestModelMesh = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/character_2_com3");
	//CUITextured *pUIMesh = new CUITextured(pd3dDevice, pd3dCommandList, XMFLOAT2(10, 50));

	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//CreateCbvAndSrvDescriptorHeaps(m_pd3dDevice, m_pd3dCommandList, nObjects, 1);
	//CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList, nObjects);
	//CreateConstantBufferViews(m_pd3dDevice, m_pd3dCommandList, nObjects, m_pd3dcbObjects, ncbElementBytes);



	//// 마테리얼에 텍스처 등록하는 곳

	///*
	//필요한 텍스처 개수를 알아내야 함
	//캐릭터 텍스처(고정), 무기 텍스처(고정), 터레인 텍스처(변동), UI텍스처(고정), 파티클 및 이펙트 텍스처(고정)
	//터레인 데이터에 있어야 하는 것:
	//- position
	//- extents
	//- textureIdx
	//터레인 데이터를 읽을 때 텍스처 이름도 저장해야 함
	//*/
	//int numOfTexture = 6;
	//CTexture **textures = new CTexture*[numOfTexture];
	//wchar_t **text = new wchar_t*[numOfTexture];
	//text[0] = L"Assets/Textures/TEST/box_diff.dds";
	//text[1] = L"Assets/Textures/TEST/character_2_diff_test3.dds";
	//text[2] = L"Assets/Textures/TEST/wall_test_diff.dds";
	//text[3] = L"Assets/Textures/TEST/crosshair_test_diff.dds";
	//text[4] = L"Assets/Textures/TEST/hpbar_in.dds";
	//text[5] = L"Assets/Textures/TEST/hpbar_out.dds";

	//for (int i = 0; i < numOfTexture; ++i) {
	//	textures[i] = new CTexture(RESOURCE_TEXTURE2D);
	//	textures[i]->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, text[i]);
	//	CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, textures[i], RootParameter::TEXTURE, false);
	//}


	///*여기가 박스 모델*/
	//for (int i = 0; i < m_nObjects; i++) {
	//		CCollideObejct *pObj = new CCollideObejct();
	//		CTestMesh *pLevelMesh = new CTestMesh(pd3dDevice, pd3dCommandList, m_pLevel->m_pLevelBlocks[i].extent);
	//		XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//		CModel *model = new CModel();
	//		model->AddMesh(pLevelMesh);
	//		model->SetTexture(textures[0]);

	//		pObj->SetModel(model);
	//		pObj->SetPosition(m_pLevel->m_pLevelBlocks[i].pos);
	//		pObj->SetOOBB(m_pLevel->m_pLevelBlocks[i].pos, m_pLevel->m_pLevelBlocks[i].extent, orientation);
	//		pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);

	//		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
	//		m_ppObjects[i] = pObj;
	//}

	///*여기가 움직이는 모델*/
	//for (int i = 0; i < m_nPlayers; i++) {
	//	CPlayerObject *pObj = new CPlayerObject();

	//	XMFLOAT3 pos = XMFLOAT3(m_pLevel->m_pSpawnPoints[i]);
	//	XMFLOAT3 extents = pTestModelMesh->GetExtents();			//반지름 아니고 지름임
	//	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//	CModel *model = new CModel();
	//	model->AddMesh(pTestModelMesh);
	//	model->SetTexture(textures[1]);

	//	pObj->SetModel(model);

	//	pObj->SetPosition(pos);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects) + i));

	//	pObj->Initialize();
	//	pObj->SetTeam(i % 2);
	//	pObj->SetOOBB(pos, extents, orientation);
	//	pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);
	//	pObj->AddRotateAngle(XMFLOAT3(0.0f, 90.0f * i, 0.0f));
	//	m_ppPlayers[i] = pObj;
	//}

	//for (int i = 0; i < m_nProjectileObjects; i++) {
	//	CProjectileObject *pObj = new CProjectileObject();

	//	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	XMFLOAT3 extents = XMFLOAT3(5.0f, 5.0f, 5.0f);			//반지름 아니고 지름임
	//	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//	CModel *model = new CModel();
	//	model->AddMesh(pTestMesh2);
	//	model->SetTexture(textures[0]);

	//	pObj->SetModel(model);
	//	pObj->SetPosition(0.0f, 0.0f, 0.0f);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects + m_nPlayers) + i));

	//	pObj->Initialize();
	//	pObj->SetTeam((i / PO_PER_PLAYER) % 2);
	//	pObj->isAlive = false;

	//	pObj->SetOOBB(pos, extents, orientation);
	//	pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);

	//	m_ppProjectileObjects[i] = pObj;
	//}

	//for (int i = 0; i < m_nUIObjects; ++i) {
	//	CDefaultUI *pUI = new CDefaultUI();

	//	XMFLOAT3 pos = m_ppPlayers[0]->GetPosition();
	//	XMFLOAT3 extents = XMFLOAT3(5.0f, 5.0f, 5.0f);			//반지름 아니고 지름임
	//	XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

	//	pos.y += 50;

	//	CModel *model = new CModel();
	//	model->AddMesh(pUIMesh);
	//	model->SetTexture(textures[3]);



	//	pUI->SetModel(model);
	//	pUI->SetPosition(pos);
	//	pUI->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects + m_nPlayers + m_nProjectileObjects) + i));
	//
	//	pUI->Initialize();
	//	pUI->isAlive = true;
	//	m_ppUIObjects[i] = pUI;
	//}

	//// 처음 따라갈 캐릭터 정해주기
	//m_pCamera->SetTarget(m_ppPlayers[0]);

}

void CPlayScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	CGroundScene::Render(pd3dCommandList);
	//pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	//// HLSL에 넣어줄 카메라 정보 갱신부분
	//m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//m_pCamera->UpdateShaderVariables(pd3dCommandList);

	////테스트용 Object들 내용 갱신부분
	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//for (int i = 0; i < m_nObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (i * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
	//}

	//for (int i = 0; i < m_nPlayers; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects) * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppPlayers[i]->m_xmf4x4World)));
	//}

	//for (int i = 0; i < m_nProjectileObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects + m_nPlayers) * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppProjectileObjects[i]->m_xmf4x4World)));
	//}
	//UINT8 * p = ((UINT8 *)m_pcbMappedGameObjects + ((m_nProjectileObjects + m_nObjects + m_nPlayers) * ncbElementBytes));

	//for (int i = 0; i < m_nUIObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects + m_nPlayers + m_nProjectileObjects) * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppUIObjects[i]->m_xmf4x4World)));
	//}

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::MODEL]);
	//for (int i = 0; i < m_nPlayers; i++) m_ppPlayers[i]->Render(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE]);
	//for (int i = m_pLevel->m_nLevelBlocks; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList, m_pCamera);
	//for (int i = 0 ; i < m_nProjectileObjects; ++i) m_ppProjectileObjects[i]->Render(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::DEBUG]);
	//for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->TestRender(pd3dCommandList, m_pCamera);
	//for (int i = 0; i < m_nPlayers; i++) m_ppPlayers[i]->TestRender(pd3dCommandList, m_pCamera);
	//for (int i = 0; i < m_nProjectileObjects; ++i) m_ppProjectileObjects[i]->TestRender(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::UI]);
	//for (int i = 0; i < m_nUIObjects; i++) m_ppUIObjects[i]->Render(pd3dCommandList, m_pCamera);


}

void CPlayScene::Update(float fTimeElapsed)
{
	CGroundScene::Update(fTimeElapsed);
	////	for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->MoveOOBB(fTimeElapsed);
	//	for (int i = 0; i < m_nProjectileObjects; i++) if (m_ppProjectileObjects[i]->isAlive) m_ppProjectileObjects[i]->MoveOOBB(fTimeElapsed);
	//	XMFLOAT3 pos = m_ppPlayers[0]->GetPosition();
	//	pos.y += 50;
	//	for (int i = 0; i < m_nUIObjects; i++) m_ppUIObjects[i]->SetPosition(pos);
	//
	//	for (int i = 0; i < m_nPlayers; i++)
	//		if (!m_ppPlayers[i]->IsDead()) {
	//			/*탄과 충돌체크*/
	//			for(int j = 0; j < m_nProjectileObjects; ++j)
	//				if(m_ppPlayers[i]->m_team != m_ppProjectileObjects[j]->m_team)
	//					if(m_ppProjectileObjects[j]->isAlive)
	//						if (m_ppPlayers[i]->IsCollide(m_ppProjectileObjects[j]->m_collisionBox)) {
	//							printf("col");
	//							m_ppProjectileObjects[j]->isAlive = false;
	//							m_ppPlayers[i]->Damaged(10);
	//
	//							if (m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->isAlive = false;
	//							break;
	//						}
	//
	//			//먼저 이동
	//			m_ppPlayers[i]->MoveOOBB(fTimeElapsed);
	//			/*지형과 충돌체크*/
	//			for(int j = 0; j < m_nObjects; ++j)
	//				if (m_ppPlayers[i]->IsCollide(m_ppObjects[j]->m_collisionBox)) {
	//					//m_ppPlayers[i]->isCollide = true;
	//					//충돌했으니까 이동은 무효로
	//					m_ppPlayers[i]->MoveOOBB(-fTimeElapsed);
	//					break;
	//				}
	//
	//			//먼저 떨어짐
	//			m_ppPlayers[i]->FallingOOBB(fTimeElapsed);
	//			m_ppPlayers[i]->isFalling = true;
	//
	//			for (int j = 0; j < m_nObjects; ++j)
	//				if (m_ppPlayers[i]->IsCollide(m_ppObjects[j]->m_collisionBox)) {
	//					//충돌했으니까 이동은 무효로
	//					m_ppPlayers[i]->isFalling = false;
	//					m_ppPlayers[i]->m_collisionBox.Center.y = m_ppPlayers[i]->prevHeight;
	//					m_ppPlayers[i]->m_fGravityAccel = 0;
	//					break;
	//				}
	//		}
	//
	//	//가만히 있는 오브젝트를 갱신을 해줘야 할까? 저는 아니라고 생각합니다.
	//	for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->Update(fTimeElapsed);
	//	for (int i = 0; i < m_nProjectileObjects; i++) m_ppProjectileObjects[i]->Update(fTimeElapsed);
	//
	//	m_pCamera->Update(fTimeElapsed);
}


void CPlayScene::ProcessInput(UCHAR * pKeysBuffer)
{
	//	float cxDelta = 0.0f, cyDelta = 0.0f;
	//	POINT ptCursorPos;
	//
	//	GetCursorPos(&ptCursorPos);
	//	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	//	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
	////	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//
	//	//if (cxDelta) m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, cxDelta * MOUSE_XSPEED, 0 });
	//
	//	XMFLOAT3 xmf3temp;
	//	if (pKeysBuffer[KEY::W] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetLook(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp,	MOVE_SPEED)); }
	//	if (pKeysBuffer[KEY::A] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetRight(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp,  -MOVE_SPEED)); }
	//	if (pKeysBuffer[KEY::S] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetLook(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp,   -MOVE_SPEED)); }
	//	if (pKeysBuffer[KEY::D] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetRight(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp,   MOVE_SPEED)); }
	//	if (pKeysBuffer[KEY::Q] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, -ROTATE_SPEED, 0 }); }
	//	if (pKeysBuffer[KEY::E] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, ROTATE_SPEED, 0 }); }
	//
	//	if (pKeysBuffer[KEY::_1] & 0xF0)
	//		if(m_pCamera->GetTarget() != m_ppPlayers[0])
	//			m_pCamera->SetTarget(m_ppPlayers[0]);
	//
	//	if (pKeysBuffer[KEY::_2] & 0xF0)
	//		if (m_pCamera->GetTarget() != m_ppPlayers[1])
	//			m_pCamera->SetTarget(m_ppPlayers[1]);
	//
	//	if (pKeysBuffer[VK_LBUTTON] & 0xF0) { 
	//		//attack
	//
	//		if (m_ppPlayers[0]->IsFireable()) {
	//			m_ppPlayers[0]->Attack();
	//			for (int i = 0; i < PO_PER_PLAYER; ++i) {
	//				if (!m_ppProjectileObjects[(m_nPlayers * 0) + i]->isAlive) {
	//					m_ppProjectileObjects[(m_nPlayers * 0) + i]->isAlive = true;
	//					m_ppProjectileObjects[(m_nPlayers * 0) + i]->Initialize(m_ppPlayers[0]);
	//					printf("fire");
	//					break;
	//				}
	//			}
	//		}
	//	}
	//
	//	if (pKeysBuffer[VK_RBUTTON] & 0xF0) {
	//		//attack
	//
	//		m_ppPlayers[1]->Damaged(100);
	//		if (m_ppPlayers[1]->IsDead()) m_ppPlayers[1]->isAlive = false;
	//
	//
	//	}
	//
	//	if (pKeysBuffer[VK_SPACE] & 0xF0) {
	//		//jump
	//		if(!m_ppPlayers[0]->isFalling) m_ppPlayers[0]->Jump();
	//
	//		//XMFLOAT4X4 matrix = m_ppPlayers[0]->m_xmf4x4World;
	//		//printf("matrix is\n");
	//		//printf("%f %f %f %f\n", matrix._11, matrix._12, matrix._13, matrix._14);
	//		//printf("%f %f %f %f\n", matrix._21, matrix._22, matrix._23, matrix._24);
	//		//printf("%f %f %f %f\n", matrix._31, matrix._32, matrix._33, matrix._34);
	//		//printf("%f %f %f %f\n", matrix._41, matrix._42, matrix._43, matrix._44);
	//	}
	//
	//	if (pKeysBuffer[VK_RETURN] & 0xF0) {
	//		//jump
	//		m_pFramework->ChangeScene(Scenes::ENTERROOM, m_pFramework);
	//	}
	//
}

void CPlayScene::OnProcessingMouseMessage()
{
}

void CPlayScene::OnProcessingKeyboardMessage()
{
}

void CPlayScene::CreateObjectBuffers()
{
}

void CPlayScene::UpdateObjectBuffers()
{
}

void CPlayScene::ReleaseObjectBuffers()
{
}

CTitleScene::CTitleScene()
{
}

CTitleScene::~CTitleScene()
{
}

void CTitleScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
}

void CTitleScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CTitleScene::Update(float fTimeElapsed)
{
}

void CTitleScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void CTitleScene::OnProcessingMouseMessage()
{
}

void CTitleScene::OnProcessingKeyboardMessage()
{
}

void CTitleScene::CreateObjectBuffers()
{
}

void CTitleScene::UpdateObjectBuffers()
{
}

void CTitleScene::ReleaseObjectBuffers()
{
}

CMatchingScene::CMatchingScene()
{
}

CMatchingScene::~CMatchingScene()
{
}

void CMatchingScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
}

void CMatchingScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CMatchingScene::Update(float fTimeElapsed)
{
}

void CMatchingScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void CMatchingScene::OnProcessingMouseMessage()
{
}

void CMatchingScene::OnProcessingKeyboardMessage()
{
}

void CMatchingScene::CreateObjectBuffers()
{
}

void CMatchingScene::UpdateObjectBuffers()
{
}

void CMatchingScene::ReleaseObjectBuffers()
{
}

CResultScene::CResultScene()
{
}

CResultScene::~CResultScene()
{
}

void CResultScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
}

void CResultScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CResultScene::Update(float fTimeElapsed)
{
}

void CResultScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void CResultScene::OnProcessingMouseMessage()
{
}

void CResultScene::OnProcessingKeyboardMessage()
{
}

void CResultScene::CreateObjectBuffers()
{
}

void CResultScene::UpdateObjectBuffers()
{
}

void CResultScene::ReleaseObjectBuffers()
{
}

void CScene::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{


}

void CScene::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews)
{


}

void CScene::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
{
}

void CScene::CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement)
{
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbObjects) {
		m_pd3dcbObjects->Unmap(0, NULL);
		m_pd3dcbObjects->Release();
	}
}

ID3D12RootSignature * CTestGroundScene::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];	// Object and Texture

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 1; //b1
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 2; //t2
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[4];	//Camera, Obejct, texture, anim

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

	//Texture
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//Anim
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3;
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

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
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CTestGroundScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	m_pFramework = (CFramework*)pContext;
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
	GetCursorPos(&m_ptOldCursorPos);

	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	MakePSO();


	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	//Importer tmpImporter;



	nPlayers = 1;
	ppPlayers = new CAnimationPlayerObject*[nPlayers];

	// 서술자 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nPlayers + /*animData.nBones*/64 + 1; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT result = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	HRESULT reason = pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nPlayers + /*animData.nBones*/64));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (nPlayers + /*animData.nBones*/64));

	CTexture **textures = new CTexture*[1];
	textures[0] = new CTexture(RESOURCE_TEXTURE2D);
	textures[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Textures/TEST/box_diff.dds");
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, textures[0], 2, false);

	// 상수버퍼 매핑
	m_pd3dcbObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * nPlayers,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);

	// 오브젝트마다 상수버퍼 뷰를 생성
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbObjects->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = ncbElementBytes;
	for (int j = 0; j < nPlayers; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}

	// 오브젝트 내용 채우기
	for (int i = 0; i < nPlayers; i++) {

		CImporter importer;
		CAnimationPlayerObject *pObj = new CAnimationPlayerObject();

		importer.ImportFile("test_0530_016_Character", textures[0], pd3dDevice, pd3dCommandList, *pObj);


		XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐
		//tmpImporter.ImportFile(pd3dDevice, pd3dCommandList, textures[0], "Assets/test_0429_015_Character", *pObj);
		//tmpImporter.ImportFile(pd3dDevice, pd3dCommandList, textures[0], "Assets/TestAnimation3.dat", *pObj);

		//tmpImporter.ImportFile(pd3dDevice, pd3dCommandList, textures[0], "Assets/TestAnimation4test_y-up.dat", *pObj);
		//CModel *model = new CModel();
		//model->AddMesh(pAnimTest);
		//model->SetTexture(textures[0]);

		//pObj->SetModel(model);
		pObj->SetPosition(0.0f, 0.0f, i * 100.0f);
		pObj->SetOOBB(XMFLOAT3(0.0f, 0.0f, i * 100.0f), XMFLOAT3(20.0f, 20.0f, 20.0f), orientation);
		pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);

		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
		ppPlayers[i] = pObj;
	}

	UINT nStride = sizeof(XMMATRIX);

	XMMATRIX *pInterpolatedMatrix = new XMMATRIX[/*animData.nBones*/64];
	for (UINT i = 0; i < /*animData.nBones*/64; i++) pInterpolatedMatrix[i] = XMMatrixIdentity();

	interpolatedMatrixResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pInterpolatedMatrix, nStride * 64,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	interpolatedMatrixResource->Map(0, NULL, (void **)&pCBMappedMatrix);

	ncbElementBytes = ((sizeof(XMMATRIX) + 255) & ~255);

	d3dGpuVirtualAddress = interpolatedMatrixResource->GetGPUVirtualAddress();
	d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = ncbElementBytes;
	for (int j = 0; j < /*animData.nBones*/64; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (j + nPlayers));
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
	// 처음 따라갈 캐릭터 정해주기
	m_pCamera->SetTarget(ppPlayers[0]);
}

void CTestGroundScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것
	//pd3dCommandList->SetGraphicsRootShaderResourceView(3, interpolatedMatrixResource->GetGPUVirtualAddress());
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, interpolatedMatrixResource->GetGPUVirtualAddress());

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	for (int i = 0; i < nPlayers; i++)
	{
		CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&ppPlayers[i]->m_xmf4x4World)));
	}

	if (pso) pd3dCommandList->SetPipelineState(pso[0]);
	for (int i = 0; i < nPlayers; ++i) ppPlayers[i]->Render(pd3dCommandList, m_pCamera);
}

void CTestGroundScene::Update(float fTimeElapsed)
{
	static float time = 0.0001f;
	if (isTimeflow)	time += fTimeElapsed;
	else {
		time += ttt * fTimeElapsed;
		ttt = 0;
	}

	//ppPlayers[0]->Animate(time);

	XMMATRIX *pbMappedcbObject = new XMMATRIX[64];
	for (int i = 0; i < 64; ++i) {
		if(ppPlayers[0]->anim->m_AnimData->m_nBoneList > i)
			pbMappedcbObject[i] = XMMatrixTranspose(ppPlayers[0]->anim->GetFinalMatrix(i, time));
		else pbMappedcbObject[i] = XMMatrixIdentity();
	}
	memcpy(pCBMappedMatrix, pbMappedcbObject, sizeof(XMMATRIX) * 64);
	delete[] pbMappedcbObject;

	for (int j = 0; j < nPlayers; ++j) if (!ppPlayers[j]->IsDead()) ppPlayers[j]->Update(fTimeElapsed);
}

void CTestGroundScene::ProcessInput(UCHAR * pKeysBuffer)
{
	XMFLOAT3 xmf3temp;
	if (pKeysBuffer[KEY::W] & 0xF0) { xmf3temp = ppPlayers[0]->GetLook(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED)); }
	if (pKeysBuffer[KEY::A] & 0xF0) { xmf3temp = ppPlayers[0]->GetRight(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); }
	if (pKeysBuffer[KEY::S] & 0xF0) { xmf3temp = ppPlayers[0]->GetLook(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); }
	if (pKeysBuffer[KEY::D] & 0xF0) { xmf3temp = ppPlayers[0]->GetRight(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED)); }
	if (pKeysBuffer[KEY::Z] & 0xF0) { xmf3temp = ppPlayers[0]->GetUp(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); }
	if (pKeysBuffer[KEY::X] & 0xF0) { xmf3temp = ppPlayers[0]->GetUp(); ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED)); }
	if (pKeysBuffer[KEY::Q] & 0xF0) { ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, -ROTATE_SPEED, 0 }); }
	if (pKeysBuffer[KEY::E] & 0xF0) { ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, ROTATE_SPEED, 0 }); }

	if (pKeysBuffer[VK_SPACE] & 0xF0) if (isTimeflow) isTimeflow = false; else isTimeflow = true;
	if (pKeysBuffer[VK_LEFT] & 0xF0) if (!isTimeflow) ttt -= 1;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) if (!isTimeflow) ttt += 1;
}

void CTestGroundScene::OnProcessingMouseMessage()
{
}

void CTestGroundScene::OnProcessingKeyboardMessage()
{
}

void CTestGroundScene::MakePSO()
{
	pso = new ID3D12PipelineState*[1];

	CPSO ** tempPso = new CPSO*[1];
	tempPso[0] = new CAnimatedPSO();
	tempPso[0]->Initialize(m_pd3dDevice, m_pd3dGraphicsRootSignature);
	pso[0] = tempPso[0]->GetPipelineState();
}
