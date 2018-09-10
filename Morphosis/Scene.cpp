#include "stdafx.h"
#include "Scene.h"

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

CGroundScene::CGroundScene()
{
}

CGroundScene::~CGroundScene()
{
}

void CGroundScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	GetCursorPos(&m_ptOldCursorPos);
}

void CGroundScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGroundScene::Update(float fTimeElapsed)
{
	// 여기서 GPU에서 사용할 변수들 갱신을 해줘야 한다!
	// 카메라, 오브젝트 등등
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
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
		m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, d3dSrvGPUDescriptorHandle);
		m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
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
}

void CEnterRoomScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CEnterRoomScene::Update(float fTimeElapsed)
{
}

void CEnterRoomScene::ProcessInput(UCHAR * pKeysBuffer)
{
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

#define PO_PER_PLAYER 16
void CPlayScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	CGroundScene::Initialize(pd3dDevice, pd3dCommandList, pContext);

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;

	m_nObjects = 2;
	m_ppObjects = new CObject*[m_nObjects];

	m_nPlayers = 2;
	m_ppPlayers = new CPlayerObject*[m_nPlayers];

	m_nProjectileObjects = m_nPlayers * PO_PER_PLAYER;
	m_ppProjectileObjects = new CProjectileObject*[m_nProjectileObjects];

	m_nDebugObjects = 0;
//	m_nDebugObjects = m_nObjects + m_nPlayers + m_nProjectileObjects;
	m_ppDebugObjects = new CObject*[m_nDebugObjects];

	int nObjects = m_nObjects + m_nPlayers + m_nProjectileObjects + m_nDebugObjects;

	// Camera 초기화
	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// RootSignature 초기화
	m_pd3dGraphicsRootSignature = CreateRootSignature(m_pd3dDevice);

	// PSO 초기화
	// 저기서 RootSignature 쓰니까 그 전에 RootSignature 만들어줘야 함
	m_nPipelineStates = PSO::count;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_ppCPSOs = new CPSO*[m_nPipelineStates];
	m_ppCPSOs[PSO::TEXTURE] = new CTexturedPSO();
	m_ppCPSOs[PSO::ILLUMINATEDTEXTURE] = new CTexturedIlluminatedPSO();
	m_ppCPSOs[PSO::MODEL] = new CModelPSO();
	m_ppCPSOs[PSO::DEBUG] = new CDebugPSO();

	m_ppCPSOs[PSO::TEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_ppCPSOs[PSO::ILLUMINATEDTEXTURE]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_ppCPSOs[PSO::MODEL]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_ppCPSOs[PSO::DEBUG]->Initialize(pd3dDevice, m_pd3dGraphicsRootSignature);

	m_ppPipelineStates[PSO::TEXTURE] = m_ppCPSOs[PSO::TEXTURE]->GetPipelineState();
	m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE] = m_ppCPSOs[PSO::ILLUMINATEDTEXTURE]->GetPipelineState();
	m_ppPipelineStates[PSO::MODEL] = m_ppCPSOs[PSO::MODEL]->GetPipelineState();
	m_ppPipelineStates[PSO::DEBUG] = m_ppCPSOs[PSO::DEBUG]->GetPipelineState();

	// 메쉬만드는 곳
	CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList);
	CTestMesh *pTestMesh2 = new CTestMesh(pd3dDevice, pd3dCommandList, 5);
	CModelMesh *pTestModelMesh = new CModelMesh(pd3dDevice, pd3dCommandList, "Assets/Models/character_2_com4");

	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	CreateCbvAndSrvDescriptorHeaps(m_pd3dDevice, m_pd3dCommandList, nObjects, 1);
	CreateShaderVariables(m_pd3dDevice, m_pd3dCommandList, nObjects);
	CreateConstantBufferViews(m_pd3dDevice, m_pd3dCommandList, nObjects, m_pd3dcbObjects, ncbElementBytes);

	// 마테리얼에 텍스처 등록하는 곳
	m_nMaterial = 2;
	m_ppMaterial = new CMaterial*[m_nMaterial];
	{
		CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Textures/TEST/box_diff.dds", 0);
		CMaterial *mat = new CMaterial();
		mat->SetTexture(pTexture);
		mat->SetReflection(1);
		m_ppMaterial[0] = mat;
		CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, pTexture, RootParameter::TEXTURE, false);
	}
	{
		CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
		pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Textures/TEST/character_2_diff_test3.dds", 0);
		CMaterial *mat = new CMaterial();
		mat->SetTexture(pTexture);
		mat->SetReflection(1);
		m_ppMaterial[1] = mat;
		CreateShaderResourceViews(m_pd3dDevice, m_pd3dCommandList, pTexture, RootParameter::TEXTURE, false);
	}

	/*여기가 박스 모델*/
	for(int i = 0; i < m_nObjects; i++) {
		CObject *pObj = new CObject();

		pObj->SetMesh(0, pTestMesh);
		pObj->SetPosition(25.0f * (i + 1), 0.0f, 0.0f);
		pObj->SetMaterial(m_ppMaterial[0]);
		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
		m_ppObjects[i] = pObj;
	}

	/*여기가 움직이는 모델*/
	for (int i = 0; i < m_nPlayers; i++) {
		CPlayerObject *pObj = new CPlayerObject();

		XMFLOAT3 pos = XMFLOAT3(20.0f * i, 0.0f, 100.0f * i);
		XMFLOAT3 extents = XMFLOAT3(15.0f, 25.0f, 15.0f);			//반지름 아니고 지름임
		XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

		pObj->SetMesh(0, pTestModelMesh);
		pObj->SetPosition(pos);
		pObj->SetMaterial(m_ppMaterial[1]);
		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects) + i));

		pObj->Initialize();
		pObj->SetTeam(i % 2);
		pos.y += 30;
		pObj->SetOOBB(pos, extents, orientation);
		pObj->AddRotateAngle(XMFLOAT3(0.0f, 90.0f * i, 0.0f));
		m_ppPlayers[i] = pObj;
	}

	for (int i = 0; i < m_nProjectileObjects; i++) {
		CProjectileObject *pObj = new CProjectileObject();

		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 extents = XMFLOAT3(5.0f, 5.0f, 5.0f);			//반지름 아니고 지름임
		XMFLOAT4 orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	//w가 1.0f 아니면 터짐

		pObj->SetMesh(0, pTestMesh2);
		pObj->SetPosition(0.0f, 0.0f, 0.0f);
		pObj->SetMaterial(m_ppMaterial[0]);
		pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects + m_nPlayers) + i));

		pObj->Initialize();
		pObj->SetTeam((i / PO_PER_PLAYER) % 2);
		pObj->m_alive = false;

		pObj->SetOOBB(pos, extents, orientation);

		m_ppProjectileObjects[i] = pObj;
	}

	///*여기가 디버그 모델*/
	//for (int i = 0; i < m_nDebugObjects; i++) {
	//	CObject *pObj = new CObject();
	//	CTestMesh *pDebugMesh;
	//	if (i < m_nObjects) pDebugMesh = new CTestMesh(pd3dDevice, pd3dCommandList, 10.5);
	//	else if (i < m_nObjects + m_nPlayers) pDebugMesh = new CTestMesh(pd3dDevice, pd3dCommandList, 20.5);
	//	else pDebugMesh = new CTestMesh(pd3dDevice, pd3dCommandList, 5.5);

	//	pObj->SetMesh(0, pDebugMesh);
	//	pObj->SetPosition(0.0f, 0.0f, 0.0f);
	//	pObj->SetMaterial(m_ppMaterial[0]);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * ((m_nObjects + m_nPlayers + m_nProjectileObjects) + i));
	//	m_ppDebugObjects[i] = pObj;
	//}

	// 처음 따라갈 캐릭터 정해주기
	m_pCamera->SetTarget(m_ppPlayers[0]);

}

void CPlayScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//문제였었던 부분 칙쇼~~~~~~~~
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	//테스트용 Object들 내용 갱신부분
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	for (int i = 0; i < m_nObjects; i++)
	{
		CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
	}

	for (int i = 0; i < m_nPlayers; i++)
	{
		CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects) * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppPlayers[i]->m_xmf4x4World)));
	}

	for (int i = 0; i < m_nProjectileObjects; i++)
	{
		CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + ((i + m_nObjects + m_nPlayers) * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppProjectileObjects[i]->m_xmf4x4World)));
	}

	for (int i = 0; i < m_nDebugObjects; i++)
	{
		CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (((m_nObjects + m_nPlayers + m_nProjectileObjects) + i) * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppDebugObjects[i]->m_xmf4x4World)));
	}

	//if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::MODEL]);
	for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->Render(pd3dCommandList, m_pCamera);

	if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::ILLUMINATEDTEXTURE]);
	for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList, m_pCamera);
	for (int i = 0 ; i < m_nProjectileObjects; ++i) if(!m_ppProjectileObjects[i]->IsDead()) m_ppProjectileObjects[i]->Render(pd3dCommandList, m_pCamera);

	//if (m_ppPipelineStates) pd3dCommandList->SetPipelineState(m_ppPipelineStates[PSO::DEBUG]);
	//for (int i = 0; i < m_nDebugObjects; ++i) m_ppDebugObjects[i]->Render(pd3dCommandList, m_pCamera);

}

void CPlayScene::Update(float fTimeElapsed)
{
	//가만히 있는 오브젝트를 갱신을 해줘야 할까? 저는 아니라고 생각합니다.
	for (int i = 0; i < m_nPlayers; i++) if (!m_ppPlayers[i]->IsDead()) m_ppPlayers[i]->Update(fTimeElapsed);
	for (int i = 0; i < m_nProjectileObjects; i++) if (!m_ppProjectileObjects[i]->IsDead()) m_ppProjectileObjects[i]->Update(fTimeElapsed);

	for (int i = 0; i < m_nPlayers; i++)
		if (!m_ppPlayers[i]->IsDead())
			for(int j = 0; j < m_nProjectileObjects; ++j)
				if(m_ppPlayers[i]->m_team != m_ppProjectileObjects[j]->m_team)
					if(!m_ppProjectileObjects[j]->IsDead())
						if (m_ppPlayers[i]->IsCollide(m_ppProjectileObjects[j]->m_collisionBox)) {
							printf("col");
							m_ppProjectileObjects[j]->m_alive = false;
							m_ppPlayers[i]->Damaged(10);
							break;
						}
	//for (int i = 0; i < m_nDebugObjects; i++) {
	//	if (i < m_nObjects) { m_ppDebugObjects[i]->SetPosition(m_ppObjects[i]->GetPosition()); }
	//	else if (i < m_nObjects + m_nPlayers) {m_ppDebugObjects[i]->SetPosition(m_ppPlayers[i - m_nObjects]->GetOOBB().Center);}
	//	else { m_ppDebugObjects[i]->SetPosition(m_ppProjectileObjects[i - m_nObjects - m_nPlayers]->GetOOBB().Center); }
	//}

	m_pCamera->Update(fTimeElapsed);
}

#define MOUSE_XSPEED 20
void CPlayScene::ProcessInput(UCHAR * pKeysBuffer)
{
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	GetCursorPos(&ptCursorPos);
	cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
	cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);

	if (cxDelta) m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, cxDelta * MOUSE_XSPEED, 0 });

	XMFLOAT3 xmf3temp;
	if (pKeysBuffer[KEY::W] & 0xF0) { m_ppPlayers[0]->AddPosVariation(m_ppPlayers[0]->GetLook()); }
	if (pKeysBuffer[KEY::A] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetRight(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -1)); }
	if (pKeysBuffer[KEY::S] & 0xF0) { xmf3temp = m_ppPlayers[0]->GetLook(); m_ppPlayers[0]->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -1)); }
	if (pKeysBuffer[KEY::D] & 0xF0) { m_ppPlayers[0]->AddPosVariation(m_ppPlayers[0]->GetRight()); }
	if (pKeysBuffer[KEY::Q] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, -40, 0 }); }
	if (pKeysBuffer[KEY::E] & 0xF0) { m_ppPlayers[0]->AddRotateAngle(XMFLOAT3{ 0, 40, 0 }); }

	if (pKeysBuffer[KEY::_1] & 0xF0)
		if(m_pCamera->GetTarget() != m_ppPlayers[0])
			m_pCamera->SetTarget(m_ppPlayers[0]);

	if (pKeysBuffer[KEY::_2] & 0xF0)
		if (m_pCamera->GetTarget() != m_ppPlayers[1])
			m_pCamera->SetTarget(m_ppPlayers[1]);

	if (pKeysBuffer[VK_LBUTTON] & 0xF0) { 
		//attack

		if (m_ppPlayers[0]->IsFireable()) {
			m_ppPlayers[0]->Attack();
			for (int i = 0; i < PO_PER_PLAYER; ++i) {
				if (!m_ppProjectileObjects[(m_nPlayers * 0) + i]->m_alive) {
					m_ppProjectileObjects[(m_nPlayers * 0) + i]->m_alive = true;
					m_ppProjectileObjects[(m_nPlayers * 0) + i]->Initialize(m_ppPlayers[0]);
					printf("fire");
					break;
				}
			}
		}
	}


	/*for Test*/
	if (pKeysBuffer[VK_SPACE] & 0xF0) {
		XMFLOAT4X4 matrix = m_ppPlayers[0]->m_xmf4x4World;
		printf("matrix is\n");
		printf("%f %f %f %f\n", matrix._11, matrix._12, matrix._13, matrix._14);
		printf("%f %f %f %f\n", matrix._21, matrix._22, matrix._23, matrix._24);
		printf("%f %f %f %f\n", matrix._31, matrix._32, matrix._33, matrix._34);
		printf("%f %f %f %f\n", matrix._41, matrix._42, matrix._43, matrix._44);
	}
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

void CPSO::CreatePipelineStateDesc(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_pd3dPipelineState);


	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

D3D12_INPUT_LAYOUT_DESC CPSO::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CPSO::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CPSO::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CPSO::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CPSO::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CPSO::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CPSO::CompileShaderFromFile(const WCHAR * pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob ** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT result = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	//	assert(result == S_OK);
	if (result != S_OK)
	{
		printf("aaa");
	}
	char *pErrorString = NULL;
	if (pd3dErrorBlob)pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

ID3D12PipelineState * CPSO::GetPipelineState()
{
	return m_pd3dPipelineState;
}

CPSO::CPSO()
{
}

CPSO::~CPSO()
{
}

void CPSO::Initialize(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CreatePipelineStateDesc(pd3dDevice, pd3dGraphicsRootSignature);
}

D3D12_INPUT_LAYOUT_DESC CTexturedPSO::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0,	DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedPSO::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"VertexShader.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedPSO::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"PixelShader.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CTexturedIlluminatedPSO::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0,	DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0,	DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0,	DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTexturedIlluminatedPSO::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"VertexShader.hlsl", "VSTexturedIlluminated", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedIlluminatedPSO::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"PixelShader.hlsl", "PSTexturedIlluminated", "ps_5_1", ppd3dShaderBlob));

}

D3D12_INPUT_LAYOUT_DESC CModelPSO::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "VERTEXIDX",	0, DXGI_FORMAT_R32_SINT,		0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "UVIDX",		0, DXGI_FORMAT_R32_SINT,		0, 4,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CModelPSO::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"VertexShader.hlsl", "VSModel", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CModelPSO::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"PixelShader.hlsl", "PSModel", "ps_5_1", ppd3dShaderBlob));

}

D3D12_INPUT_LAYOUT_DESC CDebugPSO::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0,	DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CDebugPSO::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}


D3D12_SHADER_BYTECODE CDebugPSO::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"VertexShader.hlsl", "VSDebug", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CDebugPSO::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"PixelShader.hlsl", "PSDebug", "ps_5_1", ppd3dShaderBlob));
}
