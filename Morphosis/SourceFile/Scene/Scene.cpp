#include "stdafx.h"
#include "Framework/Framework.h"
#include "Scene.h"
#include "Importer/Importer.h"
#include "Object/Object.h"
#include "Camera/Camera.h"
#include "LevelData.h"
#include "PSO.h"
#include "Material/Texture.h"

#define MOUSE_XSPEED 10
#define MOVE_SPEED 0.15f
#define ROTATE_SPEED 300
#define PO_PER_PLAYER 16

int g_RootParameterCamera;
int g_RootParameterObject;
int g_RootParameterTexture;
int g_RootParameterAnimation;


CScene::CScene()
{
}


CScene::~CScene()
{
}

//void CGroundScene::Render(ID3D12GraphicsCommandList *pd3dCommandList)
//{
//	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
//	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것
//
//	// HLSL에 넣어줄 카메라 정보 갱신부분
//	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
//	m_pCamera->UpdateShaderVariables(pd3dCommandList);
//
//	if (m_ppPSO) pd3dCommandList->SetPipelineState(m_ppPSO[0]);
//
//	//m_pObjectMng->RenderTerrainMeshes(pd3dCommandList);
//	//m_pObjectMng->RenderCollideObjects(pd3dCommandList);
//	//m_pObjectMng->RenderMovingObjects(pd3dCommandList);
//	//m_pObjectMng->RenderParticles(pd3dCommandList);
//	//m_pObjectMng->RenderEffects(pd3dCommandList);
//}
//
//void CGroundScene::Update(float fTimeElapsed)
//{
//	// 여기서 GPU에서 사용할 변수들 갱신을 해줘야 한다!
//	// 카메라, 오브젝트 등등
//	//m_pObjectMng->Update(fTimeElapsed);
//}
//
//void CGroundScene::CreateObjectBuffers()
//{
//}
//
//void CGroundScene::UpdateObjectBuffers()
//{
//}
//
//void CGroundScene::ReleaseObjectBuffers()
//{
//}
//
//void CGroundScene::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
//{
//	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
//	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
//	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	d3dDescriptorHeapDesc.NodeMask = 0;
//	HRESULT result = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
//
//	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
//	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
//	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
//}
//
//void CGroundScene::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
//{
//	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
//	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
//	d3dCBVDesc.SizeInBytes = nStride;
//	for (int j = 0; j < nConstantBufferViews; j++)
//	{
//		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
//		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
//		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
//		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
//	}
//}
//
//void CGroundScene::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews)
//{
//	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255); //256의 배수
//	m_pd3dcbObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * nConstantBufferViews, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
//
//	m_pd3dcbObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
//}
//
//
//void CGroundScene::ReleaseShaderVariables()
//{
//	CScene::ReleaseShaderVariables();
//}

void CScene::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{


}

void CScene::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews)
{


}

void CScene::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
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


	/*********************************************************************
	2019-06-16
	루트파라미터 인덱스를 전역에서 관리하기 위해 이렇게 사용.
	*********************************************************************/
	g_RootParameterCamera		= 0;
	g_RootParameterObject		= 1;
	g_RootParameterTexture		= 2;
	g_RootParameterAnimation	= 3;

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

	D3D12_ROOT_PARAMETER pd3dRootParameters[4];	//Camera, Obejct, texture, anim

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
	pd3dRootParameters[3].Descriptor.ShaderRegister = g_RootParameterAnimation;
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

	m_pFramework		= (CFramework*)pContext;
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	GetCursorPos(&m_ptOldCursorPos);

	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);

	m_pCamera = new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->SetOffset(XMFLOAT3(0, 10.0f, 10.0f));

	MakePSO();

	m_ObjMng = new CObjectManager(m_pd3dDevice, m_pd3dCommandList);
	m_pCamera->SetTarget(m_ObjMng->GetPlayer(0));
	m_pd3dCbvSrvDescriptorHeap = m_ObjMng->GetDescriptorHeap();

	//nPlayers = 1;
	//ppPlayers = new CAnimationPlayerObject*[nPlayers];
	//CImporter importer(m_pd3dDevice, m_pd3dCommandList);
	////CAnimationController* animData = new CAnimationController();
	////importer.ImportFile("test_0602_018_SingleMesh_RunningAnimation_Character", animData);
	//m_nPlayers = 1;
	//m_nObjects += m_nPlayers;
	////m_nObjects += m_nPlayers * m_nBulletPerPlayer;
	////m_nObjects += animData->m_AnimData->m_nBoneList;
	//m_ppObjects = new CObject*[m_nObjects];
	//CreateDescriptorHeap();
	//CTexture **textures = new CTexture*[2];
	//textures[0] = new CTexture(RESOURCE_TEXTURE2D);
	//textures[0]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Textures/TEST/test_0602_018_SingleMesh_RunningAnimation_Character_tex.dds");
	//CreateShaderResourceViews(pd3dDevice, pd3dCommandList, textures[0], 2, false);
	//textures[1] = new CTexture(RESOURCE_TEXTURE2D);
	//textures[1]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Textures/TEST/box_diff.dds");
	//CreateShaderResourceViews(pd3dDevice, pd3dCommandList, textures[1], 2, false);
	//CreateConstantView();
	// 오브젝트 내용 채우기
	//for (int i = 0; i < m_nPlayers; i++) {
	//	CAnimationPlayerObject *pObj = new CAnimationPlayerObject();
	//	//CTexture *textures = new CTexture();
	//	//textures = new CTexture(RESOURCE_TEXTURE2D);
	//	//textures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Textures/TEST/test_0602_018_SingleMesh_RunningAnimation_Character_tex.dds");
	//	//CreateShaderResourceViews(pd3dDevice, pd3dCommandList, textures, 2, false);
	//	AnimationClip** animData = new AnimationClip*[5];
	//	
	//	importer.ImportFile("0603_CharacterIdle", textures[0], pd3dDevice, pd3dCommandList, *pObj);
	//	
	//	animData[0] = new AnimationClip(); memset(animData[0], NULL, sizeof(AnimationClip)); importer.ImportFile("0603_CharacterRun",		animData[0]);	pObj->anim->AddAnimData(animData[0]);
	//	animData[1] = new AnimationClip(); memset(animData[1], NULL, sizeof(AnimationClip));importer.ImportFile("0603_CharacterFire",		animData[1]);	pObj->anim->AddAnimData(animData[1]);
	//	animData[2] = new AnimationClip(); memset(animData[2], NULL, sizeof(AnimationClip));importer.ImportFile("0603_CharacterStartJump",	animData[2]);	pObj->anim->AddAnimData(animData[2]);
	//	animData[3] = new AnimationClip(); memset(animData[3], NULL, sizeof(AnimationClip));importer.ImportFile("0603_CharacterEndJump",	animData[3]);	pObj->anim->AddAnimData(animData[3]);
	//	animData[4] = new AnimationClip(); memset(animData[4], NULL, sizeof(AnimationClip));importer.ImportFile("0603_CharacterDied",		animData[4]);	pObj->anim->AddAnimData(animData[4]);
	//	pObj->SetPosition(0.0f, 0.0f, i * 100.0f);
	//	pObj->m_xmf3CollisionOffset.y = 15;
	//	pObj->SetOOBB(pObj->m_xmf3CollisionOffset, XMFLOAT3(5.0f, 15.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//	pObj->SetOOBBMesh(pd3dDevice, pd3dCommandList);
	//	pObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
	//	m_ppObjects[i] = pObj;
	//	delete[] animData;
	//}
	/****************************************************************************************************
	본 애니메이션 테스트를 위한 부분.
	****************************************************************************************************/
	//for (int i = m_nPlayers; i < m_nObjects; i++) {
	//	CObject* o = new CObject;
	//	XMFLOAT4X4 mat;
	//	//XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(animData->m_AnimData->m_BoneList[i - m_nPlayers].m_pToRootTransforms));
	//	//XMStoreFloat4x4(&mat, XMMatrixInverse(&det, XMLoadFloat4x4(animData->m_AnimData->m_BoneList[i - m_nPlayers].m_pToRootTransforms)));
	//	mat = animData->m_AnimData->m_BoneList[i - m_nPlayers].m_GlobalTransform;
	//*	XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&animData->m_AnimData->m_BoneList[i - m_nPlayers].m_GlobalTransform));
	//	XMStoreFloat4x4(&mat, XMMatrixInverse(&det, XMLoadFloat4x4(&animData->m_AnimData->m_BoneList[i - m_nPlayers].m_GlobalTransform)));*/
	//	CMesh* mesh = new CTestMesh(m_pd3dDevice, m_pd3dCommandList, 0.6f);
	//	CModel *model = new CModel();
	//	model->SetTexture(textures[0]);
	//	model->AddMesh(mesh);
	//	o->SetModel(model);
	//	o->m_xmf4x4World = mat;
	//	o->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
	//	m_ppObjects[i] = o;
	//}
	//delete animData;
	// 처음 따라갈 캐릭터 정해주기
	//m_pCamera->SetTarget(m_ppObjects[0]);
}

void CTestGroundScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);	// 이 루트 시그니처를 쓸 것
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);	// 이 서술자 힙을 쓸 것
	//pd3dCommandList->SetGraphicsRootShaderResourceView(3, interpolatedMatrixResource->GetGPUVirtualAddress());
	//pd3dCommandList->SetGraphicsRootConstantBufferView(3, interpolatedMatrixResource->GetGPUVirtualAddress());

	// HLSL에 넣어줄 카메라 정보 갱신부분
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//for (int i = 0; i < m_nObjects; i++)
	//{
	//	CB_OBJECT_INFO *pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (i * ncbElementBytes));
	//	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
	//}

	pd3dCommandList->SetPipelineState(pso[1]);
	m_ObjMng->Render();

	//for (int i = 0; i < m_nPlayers; ++i) m_ppObjects[i]->Render(pd3dCommandList, m_pCamera);
	//if (m_bDebug) {
	//	pd3dCommandList->SetPipelineState(pso[2]);
	//	for (int i = 0; i < m_nPlayers; ++i) {
	//		CAnimationPlayerObject* player = dynamic_cast<CAnimationPlayerObject*>(m_ppObjects[i]);
	//		player->TestRender(pd3dCommandList, m_pCamera);
	//	}
	//}
	/****************************************************************************************************
	본 애니메이션 테스트를 위한 부분.
	****************************************************************************************************/
	//if (m_bShowBones) {
	//	pd3dCommandList->SetPipelineState(pso[1]);
	//	for (int i = m_nPlayers; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList, m_pCamera);
	//}

}

void CTestGroundScene::Update(float fTimeElapsed)
{
	static float time = 0.0001f;
	if (isTimeflow)	time += fTimeElapsed;
	else {
		time += ttt * fTimeElapsed;
		ttt = 0;
	}

	m_pCamera->Update(fTimeElapsed);
	m_ObjMng->Update(fTimeElapsed);


	// 한 오브젝트의 애니메이션 정보만 올라감. 이건 플레이어의 모델만 애니메이트시킴.
	// 어떻게 여러 오브젝트를 하지?
	//XMMATRIX *pbMappedcbObject = new XMMATRIX[64];
	//for (int i = 0; i < 1; ++i) {
	//	CAnimationPlayerObject* player = dynamic_cast<CAnimationPlayerObject*>(m_ppObjects[i]);
	//	if (player) {
	//		for (int j = 0; j < g_NumAnimationBone; ++j) {
	//			if (player->anim->m_AnimData[0]->m_nBoneList > j && m_bUploadAnimMtx)
	//				pbMappedcbObject[j] = XMMatrixTranspose(player->GetAnimMtx(j));
	//			else
	//				pbMappedcbObject[j] = XMMatrixIdentity();
	//		}
	//	}
	//}

	/****************************************************************************************************
	본 애니메이션 테스트를 위한 부분.
	****************************************************************************************************/
	// 다른 본들은 추가로 위치를 바꿔줘야 함.
	//CAnimationPlayerObject* player = dynamic_cast<CAnimationPlayerObject*>(m_ppObjects[0]);
	//XMFLOAT4X4 mat;
	//XMVECTOR det;
	//for (int i = m_nPlayers; i < m_nObjects; ++i) {
	//	if (OFFSET == m_ShowBonesMode) {
	//		XMStoreFloat4x4(&mat, player->anim->GetOffset(i - 1));
	//	}
	//	else if (OFFSETINV == m_ShowBonesMode) {
	//		det = XMMatrixDeterminant(player->anim->GetOffset(i - 1));
	//		XMStoreFloat4x4(&mat, XMMatrixInverse(&det, player->anim->GetOffset(i-1)));
	//	}
	//	else if (TOROOT == m_ShowBonesMode) {
	//		XMStoreFloat4x4(&mat, player->anim->GetInterpolatedToRootMtx(i - 1, time));
	//	}
	//	else if (TOROOTINV == m_ShowBonesMode) {
	//		det = XMMatrixDeterminant(player->anim->GetInterpolatedToRootMtx(i - 1, time));
	//		XMStoreFloat4x4(&mat, XMMatrixInverse(&det, player->anim->GetInterpolatedToRootMtx(i - 1, time)));
	//	}
	//	m_ppObjects[i]->m_xmf4x4World = mat;
	//}

	//memcpy(pCBMappedMatrix, pbMappedcbObject, sizeof(XMMATRIX) * g_NumAnimationBone);
	//delete[] pbMappedcbObject;
	//for (int j = 0; j < m_nObjects; ++j) m_ppObjects[j]->Update(fTimeElapsed);
	//m_pCamera->Update(fTimeElapsed);
}

void CTestGroundScene::ProcessInput(UCHAR * pKeysBuffer)
{
	//XMFLOAT3 xmf3temp;
	//CAnimationPlayerObject* player = dynamic_cast<CAnimationPlayerObject*>(m_ppObjects[0]);

	//if (pKeysBuffer[KEY::W] & 0xF0) { xmf3temp = player->GetLook();		player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED));  player->AnimRun();}
	//if (pKeysBuffer[KEY::A] & 0xF0) { xmf3temp = player->GetRight();	player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); player->AnimRun();}
	//if (pKeysBuffer[KEY::S] & 0xF0) { xmf3temp = player->GetLook();		player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); player->AnimRun();}
	//if (pKeysBuffer[KEY::D] & 0xF0) { xmf3temp = player->GetRight();	player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED));  player->AnimRun();}
	//if (pKeysBuffer[KEY::Z] & 0xF0) { xmf3temp = player->GetUp();		player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, -MOVE_SPEED)); player->AnimRun();}
	//if (pKeysBuffer[KEY::X] & 0xF0) { xmf3temp = player->GetUp();		player->AddPosVariation(Vector3::ScalarProduct(xmf3temp, MOVE_SPEED));  player->AnimRun();}
	//if (pKeysBuffer[KEY::Q] & 0xF0) {player->AddRotateAngle(XMFLOAT3{ 0, -ROTATE_SPEED, 0 }); }
	//if (pKeysBuffer[KEY::E] & 0xF0) {player->AddRotateAngle(XMFLOAT3{ 0, ROTATE_SPEED, 0 }); }
	//
	////if (pKeysBuffer[KEY::W] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetLook(), MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::A] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetRight(), -MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::S] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetLook(), -MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::D] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetRight(), MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::Z] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetUp(), -MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::X] & 0xF0) { m_pCamera->Move(Vector3::ScalarProduct(m_pCamera->GetUp(), MOVE_SPEED, false)); }
	////if (pKeysBuffer[KEY::Q] & 0xF0) { m_pCamera->Rotate(0, -MOVE_SPEED, 0); }
	////if (pKeysBuffer[KEY::E] & 0xF0) { m_pCamera->Rotate(0, MOVE_SPEED, 0); }

	//if (pKeysBuffer[VK_SPACE] & 0xF0) if (isTimeflow) isTimeflow = false; else isTimeflow = true;
	//if (pKeysBuffer[KEY::_5] & 0xF0) m_bDebug = true;
	//if (pKeysBuffer[KEY::_6] & 0xF0) m_bDebug = false;
	//if (pKeysBuffer[KEY::_7] & 0xF0) m_bUploadAnimMtx = false;
	//if (pKeysBuffer[KEY::_8] & 0xF0) m_bUploadAnimMtx = true;

	//if (pKeysBuffer[KEY::_1] & 0xF0) m_ShowBonesMode = OFFSET;
	//if (pKeysBuffer[KEY::_2] & 0xF0) m_ShowBonesMode = OFFSETINV;
	//if (pKeysBuffer[KEY::_3] & 0xF0) m_ShowBonesMode = TOROOT;
	//if (pKeysBuffer[KEY::_4] & 0xF0) m_ShowBonesMode = TOROOTINV;

	//if (pKeysBuffer[VK_LEFT] & 0xF0) if (!isTimeflow) ttt -= 1;
	//if (pKeysBuffer[VK_RIGHT] & 0xF0) if (!isTimeflow) ttt += 1;
}

void CTestGroundScene::OnProcessingMouseMessage()
{
}

void CTestGroundScene::OnProcessingKeyboardMessage()
{
}

void CTestGroundScene::MakePSO()
{
	pso = new ID3D12PipelineState*[3];

	CPSO ** tempPso = new CPSO*[3];
	tempPso[0] = new CAnimatedPSO();
	tempPso[0]->Initialize(m_pd3dDevice, m_pd3dGraphicsRootSignature);
	tempPso[1] = new CTexturedIlluminatedPSO();
	tempPso[1]->Initialize(m_pd3dDevice, m_pd3dGraphicsRootSignature);
	tempPso[2] = new CDebugPSO();
	tempPso[2]->Initialize(m_pd3dDevice, m_pd3dGraphicsRootSignature);

	pso[0] = tempPso[0]->GetPipelineState();
	pso[1] = tempPso[1]->GetPipelineState();
	pso[2] = tempPso[2]->GetPipelineState();
}

void CTestGroundScene::CreateDescriptorHeap()
{
	//// 서술자 힙 생성
	//D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	//d3dDescriptorHeapDesc.NumDescriptors = m_nObjects + g_NumAnimationBone + m_nSRV;
	//d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//d3dDescriptorHeapDesc.NodeMask = 0;
	//HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	//HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	//m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects + g_NumAnimationBone));
	//m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects + g_NumAnimationBone));

}

void CTestGroundScene::CreateConstantView()
{
	//UINT ncbElementBytes; 

	//ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	//// 상수버퍼 매핑
	//m_pd3dcbObjects = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nObjects,
	//	D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	//m_pd3dcbObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);

	//// 오브젝트마다 상수버퍼 뷰를 생성
	//D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbObjects->GetGPUVirtualAddress();
	//D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	//d3dCBVDesc.SizeInBytes = ncbElementBytes;
	//for (int j = 0; j < m_nObjects; j++)
	//{
	//	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * j);
	//	D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
	//	d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
	//	m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	//}

	//ncbElementBytes = ((sizeof(XMMATRIX) + 255) & ~255);

	//XMMATRIX *pInterpolatedMatrix = new XMMATRIX[g_NumAnimationBone];
	//for (UINT i = 0; i < g_NumAnimationBone; i++) pInterpolatedMatrix[i] = XMMatrixIdentity();

	//interpolatedMatrixResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, pInterpolatedMatrix, sizeof(XMMATRIX) * 64,
	//	D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	//interpolatedMatrixResource->Map(0, NULL, (void **)&pCBMappedMatrix);


	//d3dGpuVirtualAddress = interpolatedMatrixResource->GetGPUVirtualAddress();
	//d3dCBVDesc.SizeInBytes = ncbElementBytes;
	//for (int j = 0; j < g_NumAnimationBone; j++)
	//{
	//	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * j);
	//	D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
	//	d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (j + m_nObjects));
	//	m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	//}
}
