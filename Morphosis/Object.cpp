#include "stdafx.h"
#include "Object.h"


CObject::CObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
}


CObject::~CObject()
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

void CMovingObject::Update(float fTimeElapsed)
{
	m_xmf4x4World._41 += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
	m_xmf4x4World._42 += m_xmf3Variation.y * fTimeElapsed * m_fSpeed;
	m_xmf4x4World._43 += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;

	m_xmf3Variation.x = m_xmf3Variation.y = m_xmf3Variation.z = 0;

	XMMATRIX temp = DirectX::XMLoadFloat4x4(&m_xmf4x4World);
	XMMATRIX rotateXAxis = DirectX::XMMatrixRotationAxis(XMVECTOR{ 1, 0, 0 }, DirectX::XMConvertToRadians(m_xmf3RotateAngle.x * fTimeElapsed));
	XMMATRIX rotateYAxis = DirectX::XMMatrixRotationAxis(XMVECTOR{ 0, 1, 0 }, DirectX::XMConvertToRadians(m_xmf3RotateAngle.y * fTimeElapsed));
	XMMATRIX rotateZAxis = DirectX::XMMatrixRotationAxis(XMVECTOR{ 0, 0, 1 }, DirectX::XMConvertToRadians(m_xmf3RotateAngle.z * fTimeElapsed));
	rotateYAxis = DirectX::XMMatrixMultiply(rotateYAxis, rotateZAxis);
	rotateXAxis = DirectX::XMMatrixMultiply(rotateXAxis, rotateYAxis);
	temp = DirectX::XMMatrixMultiply(temp, rotateXAxis);
	DirectX::XMStoreFloat4x4(&m_xmf4x4World, temp);

	m_xmf3RotateAngle.x = m_xmf3RotateAngle.y = m_xmf3RotateAngle.z = 0;


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


