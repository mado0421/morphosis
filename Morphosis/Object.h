#pragma once
#include "Model.h"
#include "Camera.h"

class CAnimationController;
class CTexture;
class CObjectManager;
class Collider;
class Effect;
class CTinMan;
struct AnimationClip;
class CFramework;
// AB ������ ������ A���� B�� ���� ���� ��ȯ
XMFLOAT3 GetBetweenVector(const Collider& A, const Collider& B);

struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	//XMFLOAT4X4	m_xmf4x4WorldNoTranspose;
	//UINT		m_nMaterialIndex;
};
struct CB_UI_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	XMFLOAT2	m_xmf2Size;
};

struct LEVELDATA_DESC {
	~LEVELDATA_DESC() {
		MemoryClear(CollisionPosition);
		MemoryClear(CollisionScale);
		MemoryClear(CollisionRotation);
		MemoryClear(Team1SpawnPointPosition);
		MemoryClear(Team1SpawnPointRotation);
		MemoryClear(Team2SpawnPointPosition);
		MemoryClear(Team2SpawnPointRotation);
	}

	std::string levelName = "";
	int nCollisionMaps		= 0;
	int nTeam1SpawnPoint	= 0;
	int nTeam2SpawnPoint	= 0;
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
class Collider {
public:
	Collider();
	Collider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion, bool trig = false);
	Collider(XMFLOAT3 offset, float radius, bool trig = false);
	void Update(XMFLOAT3 position, XMFLOAT4 rotation);
	bool IsCollide(const Collider& other);
	void TriggerOff() { m_trigCollided = false; }
	void SetOrientation(const XMFLOAT4& orientation);

	bool TriggerCheck(const Collider& other);

	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetCenter();
	XMFLOAT3 GetExtents();
	XMFLOAT4 GetOrientation();

	BoundingOrientedBox m_Box;
	BoundingSphere		m_Sphere;
	ColliderType		m_Type;
	bool				m_trigCollided = false;

	bool				m_IsTrigger;
private:
	XMFLOAT3 GetRotatedOffset(XMFLOAT4 rotation);
	void SetPosition(XMFLOAT3 position, XMFLOAT3 rotatedOffset);
	void SetRotation(XMFLOAT4 rotation);



private:
	XMFLOAT3			m_xmf3Offset;
	XMFLOAT4			m_xmf4OrigOrientaion;
};

struct Test {
	Collider* col;
	XMFLOAT3 different;
	XMFLOAT3 prevMove;
	XMFLOAT3 nextMove;
};




class CObject
{
public:
	// �����ڿ� �Ҹ���
	CObject();
	~CObject();
	// GPU �����ϴ� �Լ�
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle)	{ m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr)					{ m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle()									{ return(m_d3dCbvGPUDescriptorHandle); }
	virtual void SetAnimatedMatrix(CAnimationController* a, float time);
	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);

	// RootParameter�� ���ϴ� �Լ�
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	// �⺻���� �۵��Լ�
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL, bool isDebug = false);
	virtual void Update(float fTimeElapsed) {}
	virtual void LateUpdate(float fTimeElapsed) {}
	virtual void ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse) {}
	// �ܺ� ���� �Լ�
	void AddCollisionEffect(CObject* p);

	void SetMng(CObjectManager* mng);
	void AddCollider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion, bool trig = false);
	void AddCollider(XMFLOAT3 offset, float radius, bool trig = false);
	void SetColliderTrigOff() { for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].m_trigCollided = false; }
	void SetPosition(float x, float y, float z);
	void SetCameraTargetOffset(XMFLOAT3 pos);
	void SetPosition(const XMFLOAT3 xmf3Position);
	void SetRotation(const XMFLOAT3& angle);
	void AddModel(CModel* model)						{	m_ModelList.push_back(model); 	}
	void SetAnimCtrl(CAnimationController* animCtrl)	{ m_AnimationController = animCtrl; }
	void SetTeam(int team)								{ m_Team = team; }
	void SetAlive(bool state)							{ m_IsAlive = state; }
	virtual void Enable()								{ SetAlive(true); }
	virtual void Disable()								{ 
		SetAlive(false); 
		m_vecEffects.clear();
	}
	// �ܺ� ���� �Լ�
	Collider* const GetCollisionCollider(Collider& other, bool isMakeAlign);
	const XMFLOAT3	GetPosition();
	virtual const XMFLOAT3	GetLook();
	virtual const XMFLOAT3	GetUp();
	virtual const XMFLOAT3	GetRight();
	const XMMATRIX	GetAnimationMatrix(int boneIdx);
	const XMFLOAT3	GetCameraTargetPos();
	const XMFLOAT4	GetQuaternion();
	const int		GetNumAnimationBone();
	const int		GetTeam() const { return m_Team; }
	// ���� �Լ�
	const bool IsCollide(const CObject& other, bool trig = false);
	const bool IsCollide(const Collider& other, bool trig = false);
	const bool IsAlive() const { return m_IsAlive; }
protected:
	// ���� ���� �Լ�
	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
	virtual void ChangeAnimClip() {}

public:
	// ���� ��ȯ ���
	XMFLOAT4X4						m_xmf4x4World;

protected:
	// ���̷�ƮX ����
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
	ID3D12Resource					*m_pd3dcbObject			= NULL;
	//������
	XMFLOAT3						m_xmf3CameraTargetOffset;
	std::vector<CModel*>			m_ModelList;
	//�̵�
	XMFLOAT3						m_xmf3CollisionOffset;
	float							m_fHeightVelocity;
	bool							m_IsGround;
	//�ִϸ��̼�
	CAnimationController			*m_AnimationController	= NULL;
	float							m_AnimationTime			= 0;
	int								m_AnimationState;
	ID3D12Resource					*m_pd3dcbAnimation		= NULL;
	XMMATRIX						*m_pcbxmAnimation		= NULL;
	XMMATRIX						m_a[g_nAnimBone];

	//�浹ü
	std::vector<Collider>			m_Collider;
	std::queue<Collider*>			m_CollideInfo;
	//������Ʈ �Ŵ����� �Ʒ��� �����ϱ� ���� ������
	CObjectManager*					m_pObjMng = NULL;
	// ��ü ����
	bool							m_IsAlive;
	int								m_Team;

	std::vector<Effect*>			m_vecEffects;
	
};

class CPlayer : public CObject {
public:
	CPlayer();
	~CPlayer();

public:
	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	// �⺻���� �۵��Լ�
	virtual void	Update(float fTimeElapsed) override;
	virtual void	LateUpdate(float fTimeElapsed) override;
	virtual void	ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse) override;

	const int GetHP() { return m_HealthPoint; }

	// �ܺ� ���� �Լ�
	virtual void	Enable()override;
	virtual void	Disable()override;
	void			SetSpawnPoint(const XMFLOAT3& pos) { m_xmf3SpawnPoint = pos; }
	void			SetLookAt(const XMFLOAT3& point) {
		SetLook(Vector3::Normalize(Vector3::Subtract(point, GetPosition())));
		SetUp(XMFLOAT3(0, 1, 0));
		SetRight(Vector3::Normalize(Vector3::CrossProduct(GetUp(), GetLook())));
	}
	// �ܺ� �۵� �Լ�
	void			MoveForwardTrigOn() { m_trigInput[static_cast<int>(Move::W)] = true; }
	void			Shoot();
	void			Skill(int idx = 0);
	void			TakeDamage(int val) { m_HealthPoint -= val; if (m_HealthPoint < 0) m_HealthPoint = 0; }
	void			Slow();
	// ���� �Լ�
	bool			IsShootable();
	bool			IsSkillUseable(int idx = 0);
	bool			IsMoving() {
		for (int i = 0; i < static_cast<int>(Move::count); ++i)
			if (m_trigInput[i]) return true;
		return false;
	}
	virtual const XMFLOAT3	GetLook()override;
	virtual const XMFLOAT3	GetUp()override;
	const XMFLOAT3 GetUnRotatedLook();
	const XMFLOAT3 GetUnRotatedUp();

	bool			IsJump();
	bool			IsOnAir();
	bool			IsShot();
	bool			IsDied();

	//void			Stun();
	//void			Heal();
	//void			Push();
	//void			MakeGrenade(vector<Component*>);





protected:
	// ���� ���� �Լ�
	void			TriggerOff();
	XMFLOAT3		Move(float fTimeElapsed);
	XMFLOAT2			Rotate(float fTimeElapsed);
	void			ChangeAnimClip();



public:
	CTinMan*		m_AIBrain = NULL;
	int				playerId = 0;

	//CSkill			*m_Skill0 = NULL;
	bool			m_IsDied = false;
protected:


	// �̵�
	XMFLOAT3		m_xmf3Move;
	XMFLOAT3		m_xmf2RotatedLook = XMFLOAT3(0,0,1);
	XMFLOAT3		m_xmf2RotatedUp = XMFLOAT3(0,1,0);
	float			m_fSpeed;
	float			m_fSlowFactor = 1.0f;
	float			m_fRemainingTimeOfSlow = 0;
	float			m_fXAxisRotation = 0;
	// Ű�Է�
	bool			m_trigInput[static_cast<int>(Move::count)];
	XMFLOAT2		m_rotationInput;
	// ���� ��ü ����
	float			m_fRPM;	// 1/Round Per Minute
	float			m_fRemainingTimeOfFire;
	XMFLOAT3		m_xmf3SpawnPoint;
	int				m_HealthPoint;
	float			m_fRemainingTimeOfRespawn;
	float			m_fRemainingTimeOfSkill1;
	bool			m_IsOnGround = false;
};

class CProjectile : public CObject {
public:
	CProjectile();
	~CProjectile();

public:
	void			Initialize(CObject* obj);
	void			Initialize(CObject* obj, const char* modelName, Effect* effect);
	//void			Initialize(CObject* obj, const char* modelName, CSkill* skill);
	virtual void	Update(float fTimeElapsed) override;
	virtual void	LateUpdate(float fTimeElapsed) override;
	void			Damage(CObject* obj);

private:
	bool			IsExpired() const { return m_fLifeTime <= 0; }

protected:
	XMFLOAT3		m_xmf3Direction;	//Normal Vector
	float			m_fSpeed;
	float			m_fLifeTime;
	float			m_BaseDamage;
	float			fallingVelocity = 0;

	//CSkill			*m_Skill = NULL;
};

class CUI : public CObject {
public:
	CUI();
public:
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL, bool isDebug = false);

	virtual void	Initialize(XMFLOAT2 size);
	void	SetScale(XMFLOAT2 scale);

	const XMFLOAT2 GetSize();

	bool	IsMouseEnter();
	bool	IsMouseOver();
	bool	IsMouseLeave();
	bool	IsClicked();

private:
	XMFLOAT2	m_xmf2Size;
	XMFLOAT2	m_xmf2Scale;
	MouseState	m_MouseState;
};

class CDamageUI : public CUI {
public:

public:
	virtual void	Initialize(XMFLOAT2 size, int num);
	virtual void	Update(float fTimeElapsed) override;


private:
	XMFLOAT2	m_xmf2Size;
	XMFLOAT2	m_xmf2Scale;
	MouseState	m_MouseState;
	float		m_fLifeTime = 1.0f;
};

class CObjectManager {
public:
	CObjectManager() = delete;
	CObjectManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, SceneType sceneType)
		: m_pd3dDevice(pd3dDevice)
		, m_pd3dCommandList(pd3dCommandList) 
		, m_SceneType(sceneType)
	{
		CreateObjectData();
	}
	~CObjectManager();

public:
	void Render();
	void Update(float fTime);

	CObject* GetTarget(int i) {
		if(m_Players.size() > i) return m_Players[i];
		else return NULL;
		//return m_Objects[m_nProps + i];
	}
	ID3D12DescriptorHeap* GetDescriptorHeap() {
		return m_pd3dCbvSrvDescriptorHeap;
	}
	void ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse);

	Collider* GetCollider(Collider& myCollider, ColliderTag targetTag, bool isMakeAlign = false);
	void ColliderTrigInit(ColliderTag targetTag);

	void SetFramework(CFramework*p);


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
	//void CollisionCheck();

	void MakeDamageUI(int damage, CObject *obj);

private:
	bool							m_Pause = false;
	SceneType						m_SceneType;

	CFramework						*m_pFramework				= NULL;
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
	m_nAnimationMatrix�� m_nObject * g_nAnimBone�� �� ��.
	*********************************************************************/
	int		m_nObjects									= 0;
	
	ID3D12Resource*		m_pd3dCBPropResource			= NULL;
	ID3D12Resource*		m_pd3dCBPlayersResource			= NULL;
	ID3D12Resource*		m_pd3dCBProjectilesResource		= NULL;
	ID3D12Resource*		m_pd3dCBFloatingUIsResource		= NULL;
	ID3D12Resource*		m_pd3dCBDefaultUIsResource		= NULL;

	CB_OBJECT_INFO*		m_pCBMappedPropObjects			= NULL;
	CB_OBJECT_INFO*		m_pCBMappedPlayers				= NULL;
	CB_OBJECT_INFO*		m_pCBMappedProjectiles			= NULL;
	CB_UI_INFO*			m_pCBMappedFloatingUIs			= NULL;
	CB_UI_INFO*			m_pCBMappedDefaultUIs			= NULL;

	/*********************************************************************
	2019-06-15
	������ ��ü���� ������ ����.
	*********************************************************************/
	vector<CObject*>		m_Props;
	vector<CPlayer*>		m_Players;
	vector<CProjectile*>	m_Projectiles;
	vector<CUI*>			m_FloatingUI;
	vector<CDamageUI*>		m_DamageUI;
	vector<CUI*>			m_DefaultUI;

	/*********************************************************************
	2019-07-01
	LevelData ������ ���� ���� ����ü
	*********************************************************************/
	LEVELDATA_DESC m_LevelDataDesc;
};

