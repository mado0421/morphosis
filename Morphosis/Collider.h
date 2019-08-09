#pragma once
#include "stdafx.h"

class Collider {
public:
	Collider();
	Collider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion);
	Collider(XMFLOAT3 offset, float radius);
	void Update(XMFLOAT3 position, XMFLOAT4 rotation);
	bool IsCollide(const Collider& other);
	void TriggerOff() { m_trigCollided = false; }
	void SetOrientation(const XMFLOAT4& orientation);

	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();
	XMFLOAT4 GetOrientation();

	BoundingOrientedBox m_Box;
	BoundingSphere		m_Sphere;
	ColliderType		m_Type;
	bool				m_trigCollided = false;

private:
	XMFLOAT3 GetRotatedOffset(XMFLOAT4 rotation);
	void SetPosition(XMFLOAT3 position, XMFLOAT3 rotatedOffset);
	void SetRotation(XMFLOAT4 rotation);



private:
	XMFLOAT3			m_xmf3Offset;
	XMFLOAT4			m_xmf4OrigOrientaion;
};

// AB 순서로 넣으면 A에서 B로 가는 벡터 반환
inline XMFLOAT3 GetBetweenVector(const Collider & A, const Collider & B)
{
	if (A.m_Type == ColliderType::BOX) {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center, A.m_Box.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Box.Center);
	}
	else {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center, A.m_Sphere.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Sphere.Center);
	}
	return XMFLOAT3(0, 0, 0);
}