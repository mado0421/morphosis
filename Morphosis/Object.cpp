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
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes && (m_nMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CObject::Update(float fTimeElapsed)
{
}

void CObject::SetMesh(int nIndex, CMesh * pMesh)
{
	if (m_ppMeshes)
	{
		m_ppMeshes[nIndex] = pMesh;
		m_nMeshes = 1;
	}
	else
	{
		m_ppMeshes = new CMesh*;
		m_ppMeshes[nIndex] = pMesh;
		m_nMeshes = 1;
	}
}

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

void CObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));

}

XMFLOAT3 CObject::GetLook()
{
	XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return Vector3::Normalize(vector);
}

XMFLOAT3 CObject::GetUp()
{
	XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return Vector3::Normalize(vector);
}

XMFLOAT3 CObject::GetRight()
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
	m_xmf4x4World._41 += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
	m_xmf4x4World._42 += m_xmf3Variation.y * fTimeElapsed * m_fSpeed - m_fGravityAccel * fTimeElapsed;
	m_xmf4x4World._43 += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;

	if (!m_bStand) {
		m_bStand = IsOnGround();
		if(!m_bStand) m_fGravityAccel += fTimeElapsed * G * 12;
	}
	else m_fGravityAccel = 0.0f;

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

	XMFLOAT3 center = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	m_collisionBox.Center = center;
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
	if (IsDead()) {
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
	pos.y += 30;
	SetPosition(pos);
	SetLook(user->GetLook());
	SetUp(user->GetUp());
	SetRight(user->GetRight());
	m_collisionBox.Center = pos;

}

void CProjectileObject::Update(float fTimeElapsed)
{
	if (!IsDead())
	{
		CMovingObject::Update(fTimeElapsed);
		m_xmf3Variation = GetLook();
		m_fLifeTime -= fTimeElapsed;
		if (m_fLifeTime <= 0) {
			m_alive = false;
		}
	}

}

void CCollideObejct::TestRender(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	if (m_ppTestMeshes && (m_nTestMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nTestMeshes; i++)
		{
			if (m_ppTestMeshes[i]) m_ppTestMeshes[i]->Render(pd3dCommandList);
		}
	}
}
