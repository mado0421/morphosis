#pragma once

class Collider {
public:
	Collider();

public:
	void Add(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 orientation);
	void Add(XMFLOAT3 offset, float radius);

private:
	vector<BoundingOrientedBox> m_vecOOBB;
	vector<BoundingSphere>		m_vecSphere;
};