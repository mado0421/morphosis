#pragma once
#include "Mesh/Model.h"
#include "Camera/Camera.h"

struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};
struct LEVELDATA_DESC {
	std::string levelName = "";
	int nCollisionMaps = 0;
	int nTeam1SpawnPoint = 0;
	int nTeam2SpawnPoint = 0;
	bool isCapturePointExist = false;

	XMFLOAT3 BoundaryBoxPosition;
	XMFLOAT3 BoundaryBoxScale;
	XMFLOAT4 BoundaryBoxRotation;

	std::vector<XMFLOAT3> CollisionPosition;
	std::vector<XMFLOAT3> CollisionScale;
	std::vector<XMFLOAT4> CollisionRotation;

	std::vector<XMFLOAT3> Team1SpawnPointPosition;
	std::vector<XMFLOAT4> Team1SpawnPointRotation;

	std::vector<XMFLOAT3> Team2SpawnPointPosition;
	std::vector<XMFLOAT4> Team2SpawnPointRotation;

	XMFLOAT3 CapturePointPosition;
	XMFLOAT3 CapturePointScale;
	XMFLOAT4 CapturePointRotation;
};

class CAnimationController;
class CTexture;
struct AnimationClip;

struct CollisionBox {
	CollisionBox(BoundingOrientedBox b, XMFLOAT3 pos)
		: collisionBox(b)
		, InitPosition(pos) {}
	BoundingOrientedBox collisionBox;
	XMFLOAT3			InitPosition = XMFLOAT3(0, 0, 0);
};
struct CollisionSphere {
	CollisionSphere(BoundingSphere b, XMFLOAT3 pos)
		: collisionSphere(b)
		, InitPosition(pos) {}
	BoundingSphere		collisionSphere;
	XMFLOAT3			InitPosition = XMFLOAT3(0, 0, 0);
};


class CObject
{
public:
	CObject();
	~CObject();

public:
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL, bool isDebug = false);
	virtual void Update(float fTimeElapsed);
	virtual void LateUpdate(float fTimeElapsed);

	void AddModel(CModel* model) {
		m_ModelList.push_back(*model);
	}
	void AddAnimClip(AnimationClip* animClip);
	void AddCollider(BoundingOrientedBox box);
	void AddCollider(BoundingSphere sphere);
	void ChangeAnimClip(const char* animClipName);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 xmf3Position);

	const XMFLOAT3	GetPosition();
	const XMFLOAT3	GetLook();
	const XMFLOAT3	GetUp();
	const XMFLOAT3	GetRight();
	const XMMATRIX	GetAnimationMatrix(int boneIdx);
	const XMFLOAT3	GetCameraTargetPos();
	const int		GetNumAnimationBone();
	const int		GetTeam() const { return m_Team; }

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
	void SetCameraTargetOffset(XMFLOAT3 pos);
	void SetTeam(int team) { m_Team = team; }

	virtual void Enable() { SetAlive(true); }
	virtual void Disable() { SetAlive(false); }
	void SetAlive(bool state) { m_IsAlive = state; }

	const bool IsCollide(CObject* other);
	const bool IsAlive() const { return m_IsAlive; }
	virtual void ProcessInput(UCHAR* pKeysBuffer, float mouse);

	void AddCollideInfo(CObject* obj);

public:
	XMFLOAT4X4						m_xmf4x4World;
	Tag								m_Tag;

protected:
	bool							m_IsAlive;
	XMFLOAT3						m_xmf3CameraTargetOffset;
	int								m_Team;

	/*********************************************************************
	2019-06-15
	������ ���� �κ�
	*********************************************************************/
	std::vector<CModel>				m_ModelList;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
	ID3D12Resource					*m_pd3dcbObject			= NULL;

	/*********************************************************************
	2019-06-17
	�̵� ���� �κ�
	*********************************************************************/
	XMFLOAT3						m_xmf3CollisionOffset;
	float							m_fHeightVelocity;
	bool							m_IsGround;

	/*********************************************************************
	2019-06-15
	�ִϸ��̼� ���� �κ�
	*********************************************************************/
	CAnimationController			*m_AnimationController = NULL;
	int								m_AnimationState;

	/*********************************************************************
	2019-06-18
	�浹ü ���� �κ�
	*********************************************************************/
	std::vector<CollisionBox>		m_CollisionBox;
	std::vector<CollisionSphere>	m_CollisionSphere;
	std::queue<CObject*>			m_CollideInfo;
};

class CPlayer : public CObject {
public:
	CPlayer();

public:
	/*********************************************************************
	2019-06-18
	�÷��̾ ó�� �����ǰų�, ������ �� �� ó��
	- ü�� ����
	- ��ġ ����
	- �ٿ���ڽ� ����

	�÷��̾ ���� �� ó��
	- ������ Ÿ�̸� ����
	*********************************************************************/
	virtual void	Update(float fTimeElapsed);
	virtual void	LateUpdate(float fTimeElapsed);
	virtual void	ProcessInput(UCHAR* pKeysBuffer, float mouse);

	virtual void	Enable();
	virtual void	Disable();
	void			Shoot();
	void			SetSpawnPoint(const XMFLOAT3& pos) { m_xmf3SpawnPoint = pos; }
	bool			IsShootable();
	void			TakeDamage(int val) { m_HealthPoint -= val; }
	XMFLOAT4X4		GetHandMatrix();

protected:
	/*********************************************************************
	2019-06-18
	��� ź�� �� ��ġ���� ������ ���� �������?
	AnimationController���� �� ���� ��ġ�� �޾ƿ� �� ���� �������?
	*********************************************************************/
	void			TriggerOff();
	bool			IsMoving() {
		for (int i = 0; i < static_cast<int>(Move::count); ++i)
			if (m_trigInput[i]) return true;
		return false;
	}
	XMFLOAT3		Move(float fTimeElapsed);
	float			Rotate(float fTimeElapsed);

	void			SetHandMatrix();

	enum class		Move {
		W,
		A,
		S,
		D,
		Q,
		E,

		count
	};
	enum class		AnimationState {
		IDLE,
		RUN,
		FIRE,
		STARTJUMP,
		ENDJUMP,
		DIE,

		count
	};

protected:
	/*********************************************************************
	2019-06-18
	�̵��� �̵� ��ȿ
	*********************************************************************/
	XMFLOAT3		m_xmf3Move;
	float			m_fSpeed;
	bool			m_trigInput[static_cast<int>(Move::count)];
	float			m_rotationInput;

	/*********************************************************************
	2019-06-18
	���� ���� �κ�
	*********************************************************************/
	float			m_fRPM;	// 1 / Round Per Minute
	float			m_fRemainingTimeOfFire;
	XMFLOAT4X4		m_xmf4x4Hand;

	/*********************************************************************
	2019-06-18
	ü�°� ������ ���� �κ�
	*********************************************************************/
	XMFLOAT3		m_xmf3SpawnPoint;
	int				m_HealthPoint;
	float			m_fRemainingTimeOfRespawn;
	
};

class CProjectile : public CObject {
public:
	CProjectile();

public:
	void			Initialize(CObject* obj);
	virtual void	Update(float fTimeElapsed);
	virtual void	LateUpdate(float fTimeElapsed);
	void			Damage(CObject* obj);

private:
	bool			IsExpired() const { return m_fLifeTime <= 0; }

protected:
	/*********************************************************************
	2019-06-17
	�̵� ���� �κ�

	���� �Է��� �޾Ƽ� �̵��ϴ� ��ü�� �ְ� ������ ������ ��� �̵��ؾ� �ϴ�
	��ü�� �ִ�. ������Ʈ ��� ���� �ʹ�. ������ ������ ����.

	����ü�� ���� �̵��ϴ� �ӷ�, ����, �߷��� ������ �޴��� ���� ���� ������
	�Ѵ�.

	�߷��� ������ �� �޴� ���� �̵� ����ü�� ���� �÷��̾��� Look, Right, Up
	�� �״�� ���� �ȴ�. ��ֶ����� ������ ���ָ� �� ��.
	�߷��� ������ �޴� ���, ����ü�� �÷��̾��� RIght�� ���� �ް� Look���͸�
	Right���� ���� �������� ���� �ణ ȸ������ �� ������ �� ���� ������ Up ����
	�� �����ָ� �ȴ�.
	�ƴ� �׳� ���� y������ �������� �ع���. �� ��ư� ������. �׷���.
	*********************************************************************/
	XMFLOAT3		m_xmf3Direction;	//Normal Vector
	float			m_fSpeed;
	float			m_fLifeTime;
	float			m_BaseDamage;
};

class CObjectManager {
public:
	CObjectManager() = delete;
	CObjectManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
		: m_pd3dDevice(pd3dDevice)
		, m_pd3dCommandList(pd3dCommandList) 
	{
		m_nObjects			= 0;
		m_nProps			= 0;
		m_nPlayers			= 0;
		m_nProjectiles		= 0;
		CreateObjectData();
	}
	~CObjectManager();

public:
	void Render();
	void Update(float fTime);

	CObject* GetTarget(int i) {
		return m_Players[i];
		//return m_Objects[m_nProps + i];
	}
	ID3D12DescriptorHeap* GetDescriptorHeap() {
		return m_pd3dCbvSrvDescriptorHeap;
	}
	void AddPSO(ID3D12PipelineState* pso) {
		m_PSO.push_back(pso);
	}
	void ProcessInput(UCHAR* pKeysBuffer, float mouse);


private:
	void LateUpdate(float fTime);

	void CreateDescriptorHeap();
	void CreateConstantBufferResorce();
	void CreateConstantBufferView();
	void CreateTextureResourceView(CTexture* pTexture);
	void CreateObjectData();

	bool IsCollidable(CObject* src, CObject* dst) {
		if (!dst->IsAlive()) return false;
		if (IsAnotherTeam(src, dst)) return true;
		return false;
	}
	bool IsAnotherTeam(CObject* src, CObject* dst) const { return src->GetTeam() != dst->GetTeam(); }
	void CollisionCheck();

private:
	ID3D12Device					*m_pd3dDevice				= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList			= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	/*********************************************************************
	2019-06-13
	�̰� ������Ʈ������ ������ַ��� �� ���� ���̴µ� ���� Scene�� �� �ʿ� ����
	������Ʈ �Ŵ����� �ּ� �����ϰ� ����.
	*********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	/*********************************************************************
	2019-06-13
	m_nObjects�� ���Ӱ� �÷��̾�, ����ü�� ������ ���� ��.
	m_nAnimationMatrix�� m_nObject * g_NumAnimationBone�� �� ��.
	*********************************************************************/
	unsigned int		m_nObjects						= 0;
	unsigned int		m_nProps						= 0;
	unsigned int		m_nPlayers						= 0;
	unsigned int		m_nProjectiles					= 0;
	
	ID3D12Resource*		m_pd3dCBPropResource			= NULL;
	ID3D12Resource*		m_pd3dCBPlayersResource			= NULL;
	ID3D12Resource*		m_pd3dCBProjectilesResource		= NULL;

	CB_OBJECT_INFO*		m_pCBMappedPropObjects			= NULL;
	CB_OBJECT_INFO*		m_pCBMappedPlayers				= NULL;
	CB_OBJECT_INFO*		m_pCBMappedProjectiles			= NULL;

	/*********************************************************************
	2019-06-15
	������ ��ü���� ������ ����.
	*********************************************************************/
	vector<CObject*>	m_Props;
	vector<CObject*>	m_Players;
	vector<CObject*>	m_Projectiles;
	vector<CTexture*>	m_TextureList;


	/*********************************************************************
	2019-06-16
	PSO ������ �ؾ� Render()���� �׸� �� ���� ��.
	*********************************************************************/
	vector<ID3D12PipelineState*> m_PSO;


	/*********************************************************************
	2019-07-01
	LevelData ������ ���� ���� ����ü
	*********************************************************************/
	LEVELDATA_DESC m_LevelDataDesc;
};

