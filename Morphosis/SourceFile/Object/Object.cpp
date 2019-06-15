#include "stdafx.h"
#include "Object.h"
#include "Animation/AnimationController.h"
#include "Mesh/Mesh.h"
#include "Importer/Importer.h"


CObject::CObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
}


CObject::~CObject()
{
}

void CObject::Initialize()
{

}

void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::OBJECT, m_d3dCbvGPUDescriptorHandle);
}

void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{
	if (!isAlive) return;
	if (!m_ModelList.empty()) {
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {
			pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::OBJECT, m_d3dCbvGPUDescriptorHandle);
			p->Render(pd3dCommandList, isDebug);
		}
	}
}

void CObject::Update(float fTimeElapsed)
{
	if (!isAlive) return;
	XMFLOAT3 xmf3Right	= XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up		= XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look	= XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	XMFLOAT3 xmf3Move	= Move(fTimeElapsed);

	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(Rotate(fTimeElapsed)));
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
	m_xmf4x4World._41 += xmf3Move.x;
	m_xmf4x4World._42 += xmf3Move.y;
	m_xmf4x4World._43 += xmf3Move.z;

	TriggerOff();
}

void CObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CObject::SetPosition(const XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 const CObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));

}

XMFLOAT3 const CObject::GetLook()
{
	XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return Vector3::Normalize(vector);
}

XMFLOAT3 const CObject::GetUp()
{
	XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return Vector3::Normalize(vector);
}

XMFLOAT3 const CObject::GetRight()
{
	XMFLOAT3 vector = { m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };
	return Vector3::Normalize(vector);
}

void CObject::SetLook(XMFLOAT3 look)
{
	m_xmf4x4World._31 = look.x;
	m_xmf4x4World._32 = look.y;
	m_xmf4x4World._33 = look.z;
}

void CObject::SetUp(XMFLOAT3 up)
{
	m_xmf4x4World._21 = up.x;
	m_xmf4x4World._22 = up.y;
	m_xmf4x4World._23 = up.z;
}

void CObject::SetRight(XMFLOAT3 right)
{
	m_xmf4x4World._11 = right.x;
	m_xmf4x4World._12 = right.y;
	m_xmf4x4World._13 = right.z;
}

void CObject::TriggerOff()
{
	for (int i = 0; i < count; i++) m_trigInput[i] = false;
}

XMFLOAT3 CObject::Move(float fTimeElapsed)
{
	XMFLOAT3 temp(0,0,0);
	if (m_trigInput[W]) { temp = Vector3::Add(temp, Vector3::ScalarProduct(GetLook(), fTimeElapsed   * m_fSpeed, false)); }
	if (m_trigInput[A]) { temp = Vector3::Add(temp, Vector3::ScalarProduct(GetRight(), -fTimeElapsed * m_fSpeed, false)); }
	if (m_trigInput[S]) { temp = Vector3::Add(temp, Vector3::ScalarProduct(GetLook(), -fTimeElapsed  * m_fSpeed, false)); }
	if (m_trigInput[D]) { temp = Vector3::Add(temp, Vector3::ScalarProduct(GetRight(), fTimeElapsed  * m_fSpeed, false)); }
	return temp;
}

float CObject::Rotate(float fTimeElapsed)
{
	float temp = 0;
	if (m_trigInput[Q]) temp += fTimeElapsed * m_fSpeed * -1;
	if (m_trigInput[E]) temp += fTimeElapsed * m_fSpeed;

	return temp;
}

void CObject::ProcessInput(UCHAR * pKeysBuffer)
{
	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[W] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[A] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[S] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[D] = true;
	if (pKeysBuffer[KEY::Q] & 0xF0) m_trigInput[Q] = true;
	if (pKeysBuffer[KEY::E] & 0xF0) m_trigInput[E] = true;
}


//CMovingObject::CMovingObject()
//{
//	m_xmf3Variation = XMFLOAT3(0, 0, 0);
//	m_xmf3RotateAngle = XMFLOAT3(0, 0, 0);
//	m_xmf3CollisionOffset = XMFLOAT3(0, 0, 0);
//}
//
//void CMovingObject::Update(float fTimeElapsed)
//{
//	if (!isAlive) return;
//	//m_xmf4x4World._41 = m_collisionBox.Center.x - m_xmf3CollisionOffset.x;
//	//m_xmf4x4World._42 = m_collisionBox.Center.y - m_xmf3CollisionOffset.y;
//	//m_xmf4x4World._43 = m_collisionBox.Center.z - m_xmf3CollisionOffset.z;
//	
//	/*
//	m_xmf4x4World._41 += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
//	m_xmf4x4World._42 += m_xmf3Variation.y * fTimeElapsed * m_fSpeed - m_fGravityAccel * fTimeElapsed;
//	m_xmf4x4World._43 += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;*/
//
//	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
//	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
//	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
//
//	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAngle.y * fTimeElapsed));
//	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
//	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);
//
//	xmf3Look = Vector3::Normalize(xmf3Look);
//	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
//	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);
//
//	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
//	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
//	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
//	m_xmf4x4World._41 += m_xmf3Variation.x;
//	m_xmf4x4World._42 += m_xmf3Variation.y;
//	m_xmf4x4World._43 += m_xmf3Variation.z;
//
//	m_xmf3RotateAngle.x = m_xmf3RotateAngle.y = m_xmf3RotateAngle.z = 0;
//	m_xmf3Variation.x = m_xmf3Variation.y = m_xmf3Variation.z = 0;
//
//	XMFLOAT3 center = XMFLOAT3(
//		m_xmf4x4World._41 + m_xmf3CollisionOffset.x,
//		m_xmf4x4World._42 + m_xmf3CollisionOffset.y,
//		m_xmf4x4World._43 + m_xmf3CollisionOffset.z);
//	m_collisionBox.Center = center;
//	XMStoreFloat4(&m_collisionBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_collisionBox.Orientation)));
//
//
//}
//
//void CMovingObject::AddPosVariation(XMFLOAT3 xmf3Velocity)
//{
//	m_xmf3Variation.x += xmf3Velocity.x;
//	m_xmf3Variation.y += xmf3Velocity.y;
//	m_xmf3Variation.z += xmf3Velocity.z;
//}
//
//void CMovingObject::AddRotateAngle(XMFLOAT3 xmf3Angle)
//{
//	m_xmf3RotateAngle.x += xmf3Angle.x;
//	m_xmf3RotateAngle.y += xmf3Angle.y;
//	m_xmf3RotateAngle.z += xmf3Angle.z;
//}
//
//void CPlayerObject::Initialize()
//{
//	CObject::Initialize();
//	m_hp = 100;
//}
//
//void CPlayerObject::Update(float fTimeElapsed)
//{
//	if (!isAlive) {
//		if (m_timer < 0) {
//			Initialize();
//			//Init
//			return;
//		}
//		m_timer -= fTimeElapsed;
//
//	}
//	else
//	{
//		CMovingObject::Update(fTimeElapsed);
//		if (m_jump) {
//			m_fGravityAccel = -fTimeElapsed * static_cast<float>( G )* 450;
//			m_jump = false;
//		}
//		if (!IsFireable()) m_attTimer -= fTimeElapsed;
//	}
//
//}
//
//void CPlayerObject::Attack()
//{
//	m_attTimer = static_cast<float>(TIMER_ATT);
//	//투사체 생성은 Scene 키입력 받을 때 해주자
//}
//
//void CPlayerObject::Damaged(int val)
//{
//	m_hp -= val;
//}
//
//void CPlayerObject::Jump()
//{
//	if (!isFalling && !m_jump) m_jump = true;
//}
//
//void CProjectileObject::Initialize()
//{
//	CObject::Initialize();
//	m_fSpeed = 550;
//	m_fLifeTime = 1.0f;
//
//}
//
//void CProjectileObject::Initialize(CMovingObject *user)
//{
//	CProjectileObject::Initialize();
//	XMFLOAT3 pos = user->GetPosition();
//	/*pos.y += 30;*/
//	SetPosition(pos);
//	SetLook(user->GetLook());
//	SetUp(user->GetUp());
//	SetRight(user->GetRight());
//	m_collisionBox.Center = pos;
//
//}
//
//void CProjectileObject::Update(float fTimeElapsed)
//{
//	if (isAlive)
//	{
//		CMovingObject::Update(fTimeElapsed);
//		m_xmf3Variation = GetLook();
//		m_fLifeTime -= fTimeElapsed;
//		if (m_fLifeTime <= 0) {
//			isAlive = false;
//		}
//	}
//
//}
//
//void CCollideObejct::SetOOBBMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
//{
//	CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList, m_collisionBox.Center, m_collisionBox.Extents);
//
//	if (m_ppTestMeshes)
//	{
//		m_ppTestMeshes[0] = pTestMesh;
//		m_nTestMeshes = 1;
//	}
//	else
//	{
//		m_ppTestMeshes = new CMesh*;
//		m_ppTestMeshes[0] = pTestMesh;
//		m_nTestMeshes = 1;
//	}
//}
//
//void CCollideObejct::TestRender(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
//{
//	if (!isAlive) return;
//	if (m_ppTestMeshes && (m_nTestMeshes > 0))
//	{
//		SetRootParameter(pd3dCommandList);
//
//		for (int i = 0; i < m_nTestMeshes; i++)
//		{
//			if (m_ppTestMeshes[i]) m_ppTestMeshes[i]->Render(pd3dCommandList);
//		}
//	}
//}
//
//CDefaultUI::CDefaultUI()
//{
//	extents.x = 0;
//	extents.y = 0;
//	texIdx = 0;
//}
//
//CDefaultUI::~CDefaultUI()
//{
//}
//
//void CDefaultUI::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
//{
//	pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::UI, m_d3dCbvGPUDescriptorHandle);
//}
//
//void CDefaultUI::SetExtents(XMFLOAT2 ext)
//{
//	SetExtents(ext.x, ext.y);
//}
//
//void CDefaultUI::SetExtents(float x, float y)
//{
//	extents.x = x;
//	extents.x = y;
//}
//
//void CDefaultUI::Initialize()
//{
//	SetPosition(0, 0, 0);
//	curLife = lifeTime;
//	isAlive = true;
//}
//
//void CDefaultUI::Initialize(CObject& other)
//{
//	other.GetPosition();
//	SetPosition(other.GetPosition());
//
//
//}
//
//void CDefaultUI::Update(float fTimeElapsed)
//{
//	if (isAlive) {
//		m_xmf4x4World._42 += fTimeElapsed * 10;
//		curLife -= fTimeElapsed;
//		if (curLife < 0) isAlive = false;
//	}
//}

//
//void CAnimationPlayerObject::Update(float fTimeElapsed)
//{
//	if (IsDead()) {
//		anim->m_AnimState = DIED;
//		anim->isLoop = false;
//		m_fAnimTime = 0.0f;
//		return;
//	}
//	if (m_bIsMoving) {
//		if (m_fAnimTime > anim->GetEndTime()) {
//			m_bIsMoving = false;
//			anim->m_AnimState = IDLE;
//			m_fAnimTime = 0.0f;
//		}
//	}
//	if (!IsMoving() && m_bIsMoving) {
//		m_bIsMoving = false;
//		anim->m_AnimState = IDLE;
//		m_fAnimTime = 0.0f;
//	}
//	CPlayerObject::Update(fTimeElapsed);
//
//
//
//	m_fAnimTime += fTimeElapsed;
//}
//
//XMMATRIX CAnimationPlayerObject::GetAnimMtx(int boneIdx)
//{
//	return anim->GetFinalMatrix(boneIdx, m_fAnimTime);
//}

CObjectManager::~CObjectManager()
{
	for (int i = 0; i < m_Objects.size(); ++i) delete m_Objects[i];
	m_Objects.erase(m_Objects.begin(), m_Objects.end());
	for (int i = 0; i < m_TextureList.size(); ++i) delete m_TextureList[i];
	m_TextureList.erase(m_TextureList.begin(), m_TextureList.end());
}

void CObjectManager::Render()
{
	for (int i = 0; i < m_Objects.size(); ++i) {
		m_Objects[i]->Render(m_pd3dCommandList);
	}
}

void CObjectManager::Update(float fTime)
{
	for (int i = 0; i < m_Objects.size(); ++i) {
		m_Objects[i]->Update(fTime);
	}
}

void CObjectManager::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = m_nObjects + g_NumAnimationBone + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects + g_NumAnimationBone));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects + g_NumAnimationBone));
}

void CObjectManager::CreateConstantBufferResorce()
{
	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProps,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nPlayers,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProjectiles,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dCBProjectilesResource->Map(0, NULL, (void **)&m_pCBMappedProjectiles);

	ncbElementBytes = ((sizeof(XMMATRIX) + 255) & ~255);

	m_pd3dCBAnimationMatrixResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nAnimationMatrix,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dCBAnimationMatrixResource->Map(0, NULL, (void **)&m_pCBMappedAnimationMatrix);
}

void CObjectManager::CreateConstantBufferView()
{
	UINT count = 0;
	UINT ncbElementBytes;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	d3dCBVDesc.SizeInBytes = ncbElementBytes;

	d3dGpuVirtualAddress = m_pd3dCBPropResource->GetGPUVirtualAddress();
	for (unsigned int i = 0; i < m_nProps; i++) {
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * count);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
		m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
	d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
	for (unsigned int i = 0; i < m_nPlayers; i++) {
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * count);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
		m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
	d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
	for (unsigned int i = 0; i < m_nProjectiles; i++) {
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * count);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
		m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}

	ncbElementBytes = ((sizeof(XMMATRIX) + 255) & ~255);
	d3dCBVDesc.SizeInBytes = ncbElementBytes;

	d3dGpuVirtualAddress = m_pd3dCBAnimationMatrixResource->GetGPUVirtualAddress();
	for (unsigned int i = 0; i < m_nAnimationMatrix; i++) {
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * count);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
		m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CObjectManager::CreateTextureResourceView()
{
	for (int i = 0; i < m_TextureList.size(); ++i) {
		D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
		int nTextures = m_TextureList[i]->GetTextureCount();
		int nTextureType = m_TextureList[i]->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource *pShaderResource = m_TextureList[i]->GetTexture();
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			m_pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);

			m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			m_TextureList[i]->SetRootArgument(2, d3dSrvGPUDescriptorHandle);
			m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
}

void CObjectManager::CreateObjectData()
{
	/*********************************************************************
	2019-06-14
	프롭을 생성해야 한다. 레벨 데이터에서 프롭의 종류와 위치 등을 빼와야 함.
	지금은 없으니까 대충 만들자. 
	*********************************************************************/



	/*********************************************************************
	2019-06-15
	서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
	m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);
	m_nProps = 10;
	m_nPlayers = 1;
	m_nProjectiles = m_nPlayers * 30;
	m_nObjects = m_nProps + m_nPlayers + m_nProjectiles;
	m_nAnimationMatrix = (m_nProps + m_nPlayers) * g_NumAnimationBone;

	CreateDescriptorHeap();
	/*********************************************************************
	2019-06-15
	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?
	*********************************************************************/
	int nTexture = 1;
	for (int i = 0; i < nTexture; ++i) {
		wstring fileName = LASSETPATH;
		fileName += L"0615_Box_diff.dds";
		CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
		texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, fileName.c_str());
		m_TextureList.push_back(texture);
	}

	/*********************************************************************
	2019-06-15
	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?
	*********************************************************************/
	CreateConstantBufferResorce();
	CreateConstantBufferView();

	int count = 0;
	for (unsigned int i = 0; i < m_nProps; i++) {
		CObject* obj = new CObject();

		importer.ImportModel("0615_Box", m_TextureList[0], obj);
		obj->SetPosition(0, 0, i * 64.0f);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Objects.push_back(obj);
	}
	for (unsigned int i = m_nProps; i < m_nPlayers; i++) {
		CObject* obj = new CObject();

		importer.ImportModel("0615_Box", m_TextureList[0], obj);
		obj->SetPosition(100, 0, i * 64.0f);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Objects.push_back(obj);
	}
	for (unsigned int i = m_nPlayers; i < m_nProjectiles; i++) {
		CObject* obj = new CObject();

		importer.ImportModel("0615_Box", m_TextureList[0], obj);
		obj->SetPosition(200, 0, i * 64.0f);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Objects.push_back(obj);
	}
}
