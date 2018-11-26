#include "stdafx.h"
#include "Object.h"


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

//void CObject::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
//{
//	XMStoreFloat4x4(&m_pcbMappedObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
//	if (m_pMaterial) m_pcbMappedObject->m_nMaterialIndex = m_pMaterial->m_nReflection;
//
//}

void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::OBJECT, m_d3dCbvGPUDescriptorHandle);
}

void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	if (!isAlive) return;
	if (model) {
		model->UpdateShaderVar(pd3dCommandList);
		pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::OBJECT, m_d3dCbvGPUDescriptorHandle);
		model->Render(pd3dCommandList);
	}
	//if (m_pMaterial)
	//{
	//	if (m_pMaterial->m_pTexture)
	//	{
	//		m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
	//	}
	//}

	//if (m_ppMeshes && (m_nMeshes > 0))
	//{
	//	SetRootParameter(pd3dCommandList);

	//	for (int i = 0; i < m_nMeshes; i++)
	//	{
	//		if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
	//	}
	//}
}

void CObject::Update(float fTimeElapsed)
{
	if (!isAlive) return;
}

void CObject::SetModel(CModel * model)
{
	this->model = model;
}

//void CObject::SetMesh(int nIndex, CMesh * pMesh)
//{
//	if (model) {
//
//	}
//	if (m_ppMeshes)
//	{
//		m_ppMeshes[nIndex] = pMesh;
//		m_nMeshes = 1;
//	}
//	else
//	{
//		m_ppMeshes = new CMesh*;
//		m_ppMeshes[nIndex] = pMesh;
//		m_nMeshes = 1;
//	}
//}

void CObject::SetMaterial(CMaterial * pMaterial)
{
	m_pMaterial = pMaterial;
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


void CMovingObject::Update(float fTimeElapsed)
{
	if (!isAlive) return;
	m_xmf4x4World._41 = m_collisionBox.Center.x;
	m_xmf4x4World._42 = m_collisionBox.Center.y;
	m_xmf4x4World._43 = m_collisionBox.Center.z;
	
	/*
	m_xmf4x4World._41 += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
	m_xmf4x4World._42 += m_xmf3Variation.y * fTimeElapsed * m_fSpeed - m_fGravityAccel * fTimeElapsed;
	m_xmf4x4World._43 += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;*/

	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAngle.y * fTimeElapsed));
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;

	m_xmf3RotateAngle.x = m_xmf3RotateAngle.y = m_xmf3RotateAngle.z = 0;
	m_xmf3Variation.x = m_xmf3Variation.y = m_xmf3Variation.z = 0;

	//XMFLOAT3 center = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	//m_collisionBox.Center = center;
	XMStoreFloat4(&m_collisionBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_collisionBox.Orientation)));


}

void CMovingObject::AddPosVariation(XMFLOAT3 xmf3Velocity)
{
	m_xmf3Variation.x += xmf3Velocity.x;
	m_xmf3Variation.y += xmf3Velocity.y;
	m_xmf3Variation.z += xmf3Velocity.z;
}

void CMovingObject::AddRotateAngle(XMFLOAT3 xmf3Angle)
{
	m_xmf3RotateAngle.x += xmf3Angle.x;
	m_xmf3RotateAngle.y += xmf3Angle.y;
	m_xmf3RotateAngle.z += xmf3Angle.z;
}

void CPlayerObject::Initialize()
{
	CObject::Initialize();
	m_hp = 100;
}

void CPlayerObject::Update(float fTimeElapsed)
{
	if (!isAlive) {
		if (m_timer < 0) {
			Initialize();
			//Init
			return;
		}
		m_timer -= fTimeElapsed;

	}
	else
	{
		CMovingObject::Update(fTimeElapsed);
		if (m_jump) {
			m_fGravityAccel = -fTimeElapsed * G * 450;
			m_jump = false;
		}
		if (!IsFireable()) m_attTimer -= fTimeElapsed;
	}

}

void CPlayerObject::Attack()
{
	m_attTimer = TIMER_ATT;
	//투사체 생성은 Scene 키입력 받을 때 해주자
}

void CPlayerObject::Damaged(int val)
{
	m_hp -= val;
}

void CPlayerObject::Jump()
{
	if (!isFalling && !m_jump) m_jump = true;
}

void CProjectileObject::Initialize()
{
	CObject::Initialize();
	m_fSpeed = 550;
	m_fLifeTime = 1.0f;

}

void CProjectileObject::Initialize(CMovingObject *user)
{
	CProjectileObject::Initialize();
	XMFLOAT3 pos = user->GetPosition();
	/*pos.y += 30;*/
	SetPosition(pos);
	SetLook(user->GetLook());
	SetUp(user->GetUp());
	SetRight(user->GetRight());
	m_collisionBox.Center = pos;

}

void CProjectileObject::Update(float fTimeElapsed)
{
	if (isAlive)
	{
		CMovingObject::Update(fTimeElapsed);
		m_xmf3Variation = GetLook();
		m_fLifeTime -= fTimeElapsed;
		if (m_fLifeTime <= 0) {
			isAlive = false;
		}
	}

}

void CCollideObejct::TestRender(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	if (!isAlive) return;
	if (m_ppTestMeshes && (m_nTestMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nTestMeshes; i++)
		{
			if (m_ppTestMeshes[i]) m_ppTestMeshes[i]->Render(pd3dCommandList);
		}
	}
}

CDefaultUI::CDefaultUI()
{
	extents.x = 0;
	extents.y = 0;
	texIdx = 0;
}

CDefaultUI::~CDefaultUI()
{
}

void CDefaultUI::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(RootParameter::UI, m_d3dCbvGPUDescriptorHandle);
}

void CDefaultUI::SetExtents(XMFLOAT2 ext)
{
	SetExtents(ext.x, ext.y);
}

void CDefaultUI::SetExtents(float x, float y)
{
	extents.x = x;
	extents.x = y;
}

void CDefaultUI::Initialize()
{
	SetPosition(0, 0, 0);
	curLife = lifeTime;
	isAlive = true;
}

void CDefaultUI::Initialize(CObject& other)
{
	other.GetPosition();
	SetPosition(other.GetPosition());


}

void CDefaultUI::Update(float fTimeElapsed)
{
	if (isAlive) {
		m_xmf4x4World._42 += fTimeElapsed * 10;
		curLife -= fTimeElapsed;
		if (curLife < 0) isAlive = false;
	}
}

void Object::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (!m_pModel) return;

	XMStoreFloat4x4(&m_pcbMappedObjectAddress->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRootDescriptorTable(m_iRootParameterIdx, m_d3dCbvGPUDescriptorHandle);
	m_pModel->Render(pd3dCommandList);
}

void Object::Update(float fTimeElapsed)
{
	m_fAnimTime += fTimeElapsed;
}

Object::Object()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	m_pModel		= nullptr;
	m_fAnimTime		= 0;
	m_iRootParameterIdx = GS::RootParameter::OBJECT;
}

void MovingObejct::Update(float fTimeElapsed)
{
	Object::Update(fTimeElapsed);

	XMMATRIX tempWorld = XMLoadFloat4x4(&m_xmf4x4World);
	XMFLOAT3 position = GetPosition();
	XMVECTOR rotateOrigin = XMLoadFloat3(&position);
	XMVECTOR rotation = XMQuaternionRotationRollPitchYawFromVector(XMVectorScale(XMLoadFloat3(&m_xmf3RotationRad), fTimeElapsed));
	XMVECTOR translation = XMVectorScale(XMLoadFloat3(&m_xmf3TranslationValue), fTimeElapsed);

	tempWorld = XMMatrixAffineTransformation(XMVectorSplatOne(), rotateOrigin, rotation, translation);
	XMStoreFloat4x4(&m_xmf4x4World, tempWorld);

	// 변화량을 초기화시킴
	m_xmf3RotationRad		= XMFLOAT3(0, 0, 0);
	m_xmf3TranslationValue	= XMFLOAT3(0, 0, 0);
}

MovingObejct::MovingObejct() : CollideObject()
{
	m_fGravityFactor		= 1.0f;
	m_xmf3RotationRad		= XMFLOAT3(0, 0, 0);
	m_xmf3TranslationValue	= XMFLOAT3(0, 0, 0);
}

CollideObject::CollideObject() : Object()
{
}

void ObjectManager::Update(float fTimeElapsed)
{
	for (auto p = m_terrainMeshes.begin(); p != m_terrainMeshes.end(); ++p) p->Update(fTimeElapsed);

	for (auto p = m_props.begin(); p != m_props.end(); ++p) p->Update(fTimeElapsed);
	for (auto p = m_terrainCollisionBoxes.begin(); p != m_terrainCollisionBoxes.end(); ++p) p->Update(fTimeElapsed);
	for (auto p = m_skillJudgeObjects.begin(); p != m_skillJudgeObjects.end(); ++p) p->Update(fTimeElapsed);

	for (auto p = m_players.begin(); p != m_players.end(); ++p) p->Update(fTimeElapsed);
	for (auto p = m_bullets.begin(); p != m_bullets.end(); ++p) p->Update(fTimeElapsed);
	for (auto p = m_skillProjectiles.begin(); p != m_skillProjectiles.end(); ++p) p->Update(fTimeElapsed);

	for (auto p = m_particles.begin(); p != m_particles.end(); ++p) p->Update(fTimeElapsed);
	for (auto p = m_effects.begin(); p != m_effects.end(); ++p) p->Update(fTimeElapsed);
}

void ObjectManager::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE	d3dCbvCPUDescriptorStartHandle)
{
	CreateResource(pd3dDevice, pd3dCommandList);
	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	BuildObjects(pd3dDevice, pd3dCommandList);
}

void ObjectManager::CreateConstantBufferView(ID3D12Device * pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE	d3dCbvCPUDescriptorStartHandle)
{
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbResource->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = ncbElementBytes;
	for (int i = 0; i < GetNumTotalObjects(); ++i)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void ObjectManager::CreateResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dcbResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * GetNumTotalObjects(),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbResource->Map(0, NULL, (void **)&m_pcbMappedObjects);
}

void ObjectManager::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT totalIdx = 0;
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	//m_terrainMeshes.resize(m_levelData.nTerrainMeshes);
	m_terrainMeshes.resize(1);
	for (auto p = m_terrainMeshes.begin(); p != m_terrainMeshes.end(); ++p) 
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));
	

	m_props.resize(0);
	for (auto p = m_props.begin(); p != m_props.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

	m_terrainCollisionBoxes.resize(1);
	for (auto p = m_terrainCollisionBoxes.begin(); p != m_terrainCollisionBoxes.end(); ++p)
		p->Initialize(XMFLOAT3(0, -100, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(100, 1, 100), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

	m_skillJudgeObjects.resize(MAX_PLAYER * SKILLJUDGE_PER_PL);
	for (auto p = m_skillJudgeObjects.begin(); p != m_skillJudgeObjects.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));


	m_players.resize(MAX_PLAYER);
	for (auto p = m_players.begin(); p != m_players.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1),m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

	m_bullets.resize(MAX_PLAYER * BULLET_PER_PL);
	for (auto p = m_bullets.begin(); p != m_bullets.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

	m_skillProjectiles.resize(MAX_PLAYER * PROJECTILE_PER_PL);
	for (auto p = m_skillProjectiles.begin(); p != m_skillProjectiles.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));


	m_particles.resize(MAX_PARTICLE);
	for (auto p = m_particles.begin(); p != m_particles.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), XMFLOAT3(1, 1, 1), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

	m_effects.resize(MAX_EFFECT);
	for (auto p = m_effects.begin(); p != m_effects.end(); ++p)
		p->Initialize(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0), m_pcbMappedObjects + (totalIdx++ * ncbElementBytes));

}
