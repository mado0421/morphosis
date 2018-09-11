#pragma once
#include "Mesh.h"
#include "Camera.h"


// 기본적인 오브젝트들 전용(크기 변동이 없는 기본적인 UI
struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CObject
{
public:
	XMFLOAT4X4						m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	CMaterial						*m_pMaterial = NULL;


	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	CB_OBJECT_INFO					*m_pcbMappedObject = NULL;
	ID3D12Resource					*m_pd3dcbObject = NULL;

public:
	CObject();
	~CObject();

public:
	virtual void Initialize();

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update(float fTimeElapsed);

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetMaterial(CMaterial *pMaterial);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);


};

class CCollideObejct : public CObject
{
public:
	BoundingOrientedBox				m_collisionBox;

	BoundingOrientedBox GetOOBB() { return m_collisionBox; }
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation)
	{
		m_collisionBox = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}
	bool IsCollide(const BoundingOrientedBox& other) {
		return m_collisionBox.Intersects(other);
	}
};

#define G (9.8)
class CMovingObject : public CCollideObejct
{
public:
	XMFLOAT3						m_xmf3Variation;
	XMFLOAT3						m_xmf3RotateAngle;
	float							m_fSpeed = 100.0f;

	float							m_fGravityAccel = 0;

public:
	virtual void Update(float fTimeElapsed);

	void AddPosVariation(XMFLOAT3 xmf3Velocity);
	void AddRotateAngle(XMFLOAT3 xmf3Angle);

	/*
	충돌체크
	*/
	bool IsOnGround() { return GetPosition().y <= 0; }
};

#define TIMER_ATT 0.05
#define TIMER_RESPANW 5
class CPlayerObject : public CMovingObject
{
public:
	short m_hp = 100;
	float m_timer = 0;
	float m_attTimer = 0;
	bool m_team = 0;

public:
	virtual void Initialize();
	virtual void Update(float fTimeElapsed);

	void Attack();
	void Damaged(int val);

	void SetTeam(bool team) { m_team = team; }

	bool IsDead() { return m_hp <= 0; }
	bool IsFireable() { return (m_attTimer <= 0) && !IsDead(); }
};

class CProjectileObject : public CMovingObject
{
public:
	bool m_team = 0;
	bool m_alive = false;
	float m_fLifeTime = 1.0f;

public:
	virtual void Initialize();
	virtual void Initialize(CMovingObject *user);
	virtual void Update(float fTimeElapsed);

	void SetTeam(bool team) { m_team = team; }
	bool IsDead() { return !m_alive; }
};

class CDefaultUI : public CObject
{

};

class CFlexibleUI : public CDefaultUI
{

};