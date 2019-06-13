#pragma once
#include "Mesh/Model.h"
#include "Camera/Camera.h"


struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CAnimationController;

class CObject
{
public:
	CObject();
	~CObject();

public:
	virtual void Initialize();

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL, bool isDebug = false);
	virtual void Update(float fTimeElapsed);

	void AddModel(CModel* model) {
		m_ModelList.push_back(*model);
	}
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 xmf3Position);

	const XMFLOAT3 GetPosition();
	const XMFLOAT3 GetLook();
	const XMFLOAT3 GetUp();
	const XMFLOAT3 GetRight();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);

	bool IsCollide(const BoundingOrientedBox& other);

private:
	void TriggerOff();
	XMFLOAT3 Move(float fTimeElapsed);
	float Rotate(float fTimeElapsed);
	void ProcessInput(UCHAR* pKeysBuffer);

	enum {
		W,
		A,
		S,
		D,
		Q,
		E,

		count
	};

private:
	bool							isAlive					= true;
	XMFLOAT4X4						m_xmf4x4World;
	std::vector<CModel>				m_ModelList;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
	ID3D12Resource					*m_pd3dcbObject			= NULL;

	/*************************************************************************
	이동 관련 파트*/
	XMFLOAT3						m_xmf3Variation			= XMFLOAT3(0, 0, 0);
	XMFLOAT3						m_xmf3RotateAngle		= XMFLOAT3(0, 0, 0);
	XMFLOAT3						m_xmf3CollisionOffset	= XMFLOAT3(0, 0, 0);
	float							m_fSpeed				= 100.0f;
	float							m_fGravityAccel			= 0;
	float							prevHeight				= 0;
	bool							isFalling				= true;
	bool							m_trigInput[count];

	/*************************************************************************
	애니메이션 관련 파트*/
	CAnimationController*			m_AnimationController = NULL;
};
//class CCollideObejct : public CObject
//{
//public:
//	CMesh							**m_ppTestMeshes;
//	int								m_nTestMeshes;
//	BoundingOrientedBox				m_collisionBox;
//
//	BoundingOrientedBox GetOOBB() { return m_collisionBox; }
//	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) {m_collisionBox = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);}
//	bool IsCollide(const BoundingOrientedBox& other) {return m_collisionBox.Intersects(other);}
//	void SetOOBBMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
//
//	virtual void TestRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
//
//};
//class CMovingObject : public CCollideObejct
//{
//public:
//	XMFLOAT3						m_xmf3Variation = XMFLOAT3(0,0,0);
//	XMFLOAT3						m_xmf3RotateAngle = XMFLOAT3(0, 0, 0);
//	float							m_fSpeed = 100.0f;
//
//	float							m_fGravityAccel = 0;
//	bool							isFalling = true;
//	float							prevHeight = 0;
//	XMFLOAT3						m_xmf3CollisionOffset = XMFLOAT3(0, 0, 0);
//public:
//	CMovingObject();
//
//	void MoveOOBB(float fTimeElapsed) {
//		m_collisionBox.Center.x += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
//		m_collisionBox.Center.y += m_xmf3Variation.y * fTimeElapsed * m_fSpeed;
//		m_collisionBox.Center.z += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;
//	}
//	void FallingOOBB(float fTimeElapsed) {
//		prevHeight = m_collisionBox.Center.y;
//		m_fGravityAccel += fTimeElapsed * static_cast<float>(G) * 1.5f;
//		m_collisionBox.Center.y -= m_fGravityAccel;
//	}
//
//	virtual void Update(float fTimeElapsed);
//
//	void AddPosVariation(XMFLOAT3 xmf3Velocity);
//	void AddRotateAngle(XMFLOAT3 xmf3Angle);
//};
//class CPlayerObject : public CMovingObject
//{
//public:
//	short m_hp = 100;
//	float m_timer = 0;
//	float m_attTimer = 0;
//	bool m_team = 0;
//	bool m_jump = false;
//
//public:
//	virtual void Initialize();
//	virtual void Update(float fTimeElapsed);
//
//	virtual void Attack();
//	virtual void Damaged(int val);
//	virtual void Jump();
//
//	virtual void SetTeam(bool team) { m_team = team; }
//	virtual bool IsDead() { return m_hp <= 0; }
//	virtual bool IsFireable() { return (m_attTimer <= 0) && !IsDead(); }
//};
//class CAnimationPlayerObject : public CPlayerObject
//{
//private:
//	enum {
//		IDLE,
//		RUN,
//		FIRE,
//		STARTJUMP,
//		ENDJUMP,
//		DIED
//	};
//
//public:
//	virtual void Update(float fTimeElapsed);
//	XMMATRIX GetAnimMtx(int boneIdx);
//
//	void AnimRun() {
//		if (!m_bIsMoving)
//		{
//			m_bIsMoving = true;
//			anim->m_AnimState = RUN;
//			m_fAnimTime = 0.0f;
//
//		}
//
//	}
//
//	bool IsMoving() {
//		return !Vector3::IsZero(m_xmf3Variation);
//	}
//
//
//public: 
//	CAnimationController*	anim = NULL;
//	float			m_fAnimTime = 0.0f;
//	bool			m_bIsMoving = false;
//};
//class CProjectileObject : public CMovingObject
//{
//public:
//	bool m_team = 0;
//	float m_fLifeTime = 1.0f;
//
//public:
//	virtual void Initialize();
//	virtual void Initialize(CMovingObject *user);
//	virtual void Update(float fTimeElapsed);
//
//	void SetTeam(bool team) { m_team = team; }
//};
//class CDefaultUI : public CObject
//{
//public:
//	CDefaultUI();
//	~CDefaultUI();
//
//public:
//	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void SetExtents(XMFLOAT2 extents);
//	virtual void SetExtents(float x, float y);
//
//	virtual void Initialize();
//	virtual void Initialize(CObject& other);
//	virtual void Update(float fTimeElapsed);
//	//위치, 크기(Extents), 텍스쳐 인덱스
//public:
//	XMFLOAT2 extents;
//	unsigned int texIdx;
//	float lifeTime = 3;
//	float curLife = 0;
//	bool isAlive = false;
//
//};


class CObjectManager {
public:
	void BuildObjectManager();

private:
	void CreateDescriptorHeap();
	void CreateConstantBufferResorce();
	void CreateConstantBufferView();
	void CreateObjectData();

private:
	ID3D12Device					*m_pd3dDevice				= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList			= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	/*********************************************************************
	2019-06-13
	이거 오브젝트같은거 만들어주려고 할 때만 쓰이는데 굳이 Scene에 둘 필요 없이
	오브젝트 매니저에 둬서 관리하게 하자.
	*********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	/*********************************************************************
	2019-06-13
	m_nObjects는 프롭과 플레이어, 투사체의 개수를 합한 값.
	m_nAnimationMatrix는 m_nObject * g_NumAnimationBone를 한 값.
	*********************************************************************/
	unsigned int	m_nObjects						= 0;
	unsigned int	m_nProps						= 0;
	unsigned int	m_nPlayers						= 0;
	unsigned int	m_nProjectiles					= 0;
	unsigned int	m_nAnimationMatrix				= 0;
	
	ID3D12Resource* m_pd3dCBPropResource			= NULL;
	ID3D12Resource* m_pd3dCBPlayersResource			= NULL;
	ID3D12Resource* m_pd3dCBProjectilesResource		= NULL;
	ID3D12Resource* m_pd3dCBAnimationMatrixResource = NULL;

	CB_OBJECT_INFO*	m_pCBMappedPropObjects			= NULL;
	CB_OBJECT_INFO* m_pCBMappedPlayers				= NULL;
	CB_OBJECT_INFO* m_pCBMappedProjectiles			= NULL;
	XMMATRIX*		m_pCBMappedAnimationMatrix		= NULL;
};

