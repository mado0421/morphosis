#include "stdafx.h"
#include "Collider.h"

Collider::Collider()
{
}

/*********************************************************************
2019-07-05
�浹ü ��ü�� ���ʹϾ��� �������� ��.
��ü�� ȸ������ ���� ��ü�� ȸ������ �������� �ϴµ� �̰� ��� �����?
Quaternion ���ϴ� �Լ��� �ֳ�?
*********************************************************************/
Collider::Collider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion)
	: m_xmf3Offset(offset)
	, m_Box(BoundingOrientedBox(XMFLOAT3(0, 0, 0), extents, XMFLOAT4(0, 0, 0, 0)))
	, m_Type(ColliderType::BOX)
	, m_xmf4OrigOrientaion(quaternion)
{

}

Collider::Collider(XMFLOAT3 offset, float radius)
	: m_xmf3Offset(offset)
	, m_Type(ColliderType::SPHERE)
	, m_Sphere(BoundingSphere(XMFLOAT3(0, 0, 0), radius))
{

}

void Collider::Update(XMFLOAT3 position, XMFLOAT4 rotation)
{
	SetPosition(position, GetRotatedOffset(rotation));
	SetRotation(rotation);
}

bool Collider::IsCollide(const Collider & other)
{
	/*********************************************************************
	2019-07-05
	���⸮�Ϲ��� ���� �ʹ�!

	if�� ���ʺ��� �˻��ϰ� �ƴϸ� �н����״ϱ� ������ �ص� ��������???
	*********************************************************************/
	if (m_Type == ColliderType::BOX) {
		if (other.m_Type == ColliderType::BOX)  return m_Box.Intersects(other.m_Box);
		else									return m_Box.Intersects(other.m_Sphere);
	}
	else {
		if (other.m_Type == ColliderType::BOX)  return m_Sphere.Intersects(other.m_Box);
		else									return m_Sphere.Intersects(other.m_Sphere);
	}
	return false;
}

void Collider::SetOrientation(const XMFLOAT4 & orientation)
{
	if (m_Type == ColliderType::BOX)	m_Box.Orientation = orientation;
}

XMFLOAT3 Collider::GetLook()
{
	XMFLOAT3 look(0, 0, 1);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&look, XMVector3Rotate(XMLoadFloat3(&look), XMLoadFloat4(&m_Box.Orientation)));

	return look;
}

XMFLOAT3 Collider::GetUp()
{
	XMFLOAT3 up(0, 1, 0);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&up), XMLoadFloat4(&m_Box.Orientation)));

	return up;
}

XMFLOAT3 Collider::GetCenter()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Center;
	else								return m_Sphere.Center;
}

XMFLOAT3 Collider::GetExtents()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Extents;
	else return XMFLOAT3(0, 0, 0);
}

XMFLOAT4 Collider::GetOrientation()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Orientation;
	else return XMFLOAT4(0, 0, 0, 1);
}



XMFLOAT3 Collider::GetRotatedOffset(XMFLOAT4 rotation)
{
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Rotate(XMLoadFloat3(&m_xmf3Offset), XMLoadFloat4(&rotation)));
	return result;
}

void Collider::SetPosition(XMFLOAT3 position, XMFLOAT3 rotatedOffset)
{
	if (m_Type == ColliderType::BOX)	m_Box.Center = Vector3::Add(position, rotatedOffset);
	else								m_Sphere.Center = Vector3::Add(position, rotatedOffset);

}

void Collider::SetRotation(XMFLOAT4 rotation)
{
	if (m_Type == ColliderType::BOX) {
		XMFLOAT4 objRotation = rotation;
		XMStoreFloat4(&m_Box.Orientation, XMQuaternionRotationMatrix(XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&objRotation)), XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4OrigOrientaion)))));


		//m_Box.Orientation = rotation;
	}

}
