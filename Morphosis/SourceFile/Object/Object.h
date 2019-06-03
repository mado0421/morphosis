#pragma once
#include "Mesh/Model.h"
#include "Camera/Camera.h"


// 기본적인 오브젝트들 전용(크기 변동이 없는 기본적인 UI
struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

struct CB_UI_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CObject
{
public:
	XMFLOAT4X4						m_xmf4x4World;

	CModel							*model = NULL;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	CB_OBJECT_INFO					*m_pcbMappedObject = NULL;
	ID3D12Resource					*m_pd3dcbObject = NULL;

	bool							isAlive = true;


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

	void SetModel(CModel *model);
	void SetMaterial(CMaterial *pMaterial);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 xmf3Position);

	const XMFLOAT3 GetPosition();
	const XMFLOAT3 GetLook();
	const XMFLOAT3 GetUp();
	const XMFLOAT3 GetRight();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
};

class CCollideObejct : public CObject
{
public:
	CMesh							**m_ppTestMeshes;
	int								m_nTestMeshes;
	BoundingOrientedBox				m_collisionBox;

	BoundingOrientedBox GetOOBB() { return m_collisionBox; }
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) {m_collisionBox = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);}
	bool IsCollide(const BoundingOrientedBox& other) {return m_collisionBox.Intersects(other);}
	void SetOOBBMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) {
		CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList, m_collisionBox.Center, m_collisionBox.Extents);

		if (m_ppTestMeshes)
		{
			m_ppTestMeshes[0] = pTestMesh;
			m_nTestMeshes = 1;
		}
		else
		{
			m_ppTestMeshes = new CMesh*;
			m_ppTestMeshes[0] = pTestMesh;
			m_nTestMeshes = 1;
		}
	}

	virtual void TestRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

};

#define G (0.05 * 9.8)
class CMovingObject : public CCollideObejct
{
public:
	XMFLOAT3						m_xmf3Variation = XMFLOAT3(0,0,0);
	XMFLOAT3						m_xmf3RotateAngle = XMFLOAT3(0, 0, 0);
	float							m_fSpeed = 100.0f;

	float							m_fGravityAccel = 0;
	bool							isFalling = true;
	float							prevHeight = 0;
	XMFLOAT3						m_xmf3CollisionOffset = XMFLOAT3(0, 0, 0);
public:
	CMovingObject();

	void MoveOOBB(float fTimeElapsed) {
		m_collisionBox.Center.x += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
		m_collisionBox.Center.y += m_xmf3Variation.y * fTimeElapsed * m_fSpeed;
		m_collisionBox.Center.z += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;
	}
	void FallingOOBB(float fTimeElapsed) {
		prevHeight = m_collisionBox.Center.y;
		m_fGravityAccel += fTimeElapsed * static_cast<float>(G) * 1.5f;
		m_collisionBox.Center.y -= m_fGravityAccel;
	}

	virtual void Update(float fTimeElapsed);

	void AddPosVariation(XMFLOAT3 xmf3Velocity);
	void AddRotateAngle(XMFLOAT3 xmf3Angle);
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
	bool m_jump = false;

public:
	virtual void Initialize();
	virtual void Update(float fTimeElapsed);

	virtual void Attack();
	virtual void Damaged(int val);
	virtual void Jump();

	virtual void SetTeam(bool team) { m_team = team; }
	virtual bool IsDead() { return m_hp <= 0; }
	virtual bool IsFireable() { return (m_attTimer <= 0) && !IsDead(); }
};

class AnimationData;

class CAnimationPlayerObject : public CPlayerObject
{
private:
	enum {
		IDLE,
		RUN,
		FIRE,
		STARTJUMP,
		ENDJUMP,
		DIED
	};

public:
	virtual void Update(float fTimeElapsed);
	XMMATRIX GetAnimMtx(int boneIdx);

	void AnimRun() {
		if (!m_bIsMoving)
		{
			m_bIsMoving = true;
			anim->m_AnimState = RUN;
			m_fAnimTime = 0.0f;

		}

	}

	bool IsMoving() {
		return !Vector3::IsZero(m_xmf3Variation);
	}


public: 
	AnimationData*	anim = NULL;
	float			m_fAnimTime = 0.0f;
	bool			m_bIsMoving = false;
};

class CProjectileObject : public CMovingObject
{
public:
	bool m_team = 0;
	float m_fLifeTime = 1.0f;

public:
	virtual void Initialize();
	virtual void Initialize(CMovingObject *user);
	virtual void Update(float fTimeElapsed);

	void SetTeam(bool team) { m_team = team; }
};

class CDefaultUI : public CObject
{
public:
	CDefaultUI();
	~CDefaultUI();

public:
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetExtents(XMFLOAT2 extents);
	virtual void SetExtents(float x, float y);

	virtual void Initialize();
	virtual void Initialize(CObject& other);
	virtual void Update(float fTimeElapsed);
	//위치, 크기(Extents), 텍스쳐 인덱스
public:
	XMFLOAT2 extents;
	unsigned int texIdx;
	float lifeTime = 3;
	float curLife = 0;
	bool isAlive = false;

};

class CFlexibleUI : public CDefaultUI
{

};
