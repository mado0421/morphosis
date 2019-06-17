#pragma once
#include "Mesh/Model.h"
#include "Camera/Camera.h"


struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CAnimationController;
struct AnimationClip;

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

	void AddModel(CModel* model) {
		m_ModelList.push_back(*model);
	}
	void AddAnimClip(AnimationClip* animClip);
	void ChangeAnimClip(const char* animClipName);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 xmf3Position);

	const XMFLOAT3	GetPosition();
	const XMFLOAT3	GetLook();
	const XMFLOAT3	GetUp();
	const XMFLOAT3	GetRight();
	XMMATRIX		GetAnimationMatrix(int boneIdx);
	int				GetNumAnimationBone();
	XMFLOAT3		GetCameraTargetPos();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
	void SetCameraTargetPos(XMFLOAT3 pos);
	void SetAlive(bool state) { m_IsAlive = state; }

	bool IsCollide(const BoundingOrientedBox& other);
	bool IsAlive() { return m_IsAlive; }
	virtual void ProcessInput(UCHAR* pKeysBuffer);

protected:


public:
	XMFLOAT4X4						m_xmf4x4World;

protected:
	bool							m_IsAlive;
	std::vector<CModel>				m_ModelList;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
	ID3D12Resource					*m_pd3dcbObject			= NULL;

	XMFLOAT3						m_xmf3CameraTargetPos;

	/*********************************************************************
	2019-06-17
	이동 관련 부분
	*********************************************************************/
	XMFLOAT3						m_xmf3CollisionOffset;

	/*********************************************************************
	2019-06-15
	애니메이션 관련 부분
	*********************************************************************/
	CAnimationController			*m_AnimationController = NULL;
	int								m_AnimationState;
};

class CPlayer : public CObject {
public:
	CPlayer();

public:
	virtual void	Update(float fTimeElapsed);
	virtual void	ProcessInput(UCHAR* pKeysBuffer);

protected:
	virtual bool	IsMoving() {
		for (int i = 0; i < static_cast<int>(Move::count); ++i)
			if (m_trigInput[i]) return true;
		return false;
	}
	void			TriggerOff();
	XMFLOAT3		Move(float fTimeElapsed);
	float			Rotate(float fTimeElapsed);

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
	float			m_fSpeed;
	bool			m_trigInput[static_cast<int>(Move::count)];

};

class CProjectile : public CObject {
public:
	CProjectile();

protected:
	/*********************************************************************
	2019-06-17
	이동 관련 부분

	직접 입력을 받아서 이동하는 객체가 있고 고정된 값으로 계속 이동해야 하는
	객체도 있다. 컴포넌트 방식 쓰고 싶다. 하지만 지금은 무리.

	투사체의 경우는 이동하는 속력, 방향, 중력의 영향을 받는지 여부 등을 가져야
	한다.

	중력의 영향을 안 받는 직선 이동 투사체의 경우는 플레이어의 Look, Right, Up
	을 그대로 쓰면 된다. 노멀라이즈 정도만 해주면 될 듯.
	중력의 영향을 받는 경우, 투사체는 플레이어의 RIght를 먼저 받고 Look벡터를
	Right벡터 축을 기준으로 위로 약간 회전시켜 준 다음에 그 둘을 가지고 Up 벡터
	를 구해주면 된다.
	아니 그냥 점점 y축으로 떨어지게 해버려. 뭘 어렵게 생각해. 그러게.
	*********************************************************************/
	XMFLOAT3						m_xmf3Direction;


};

class CTexture;

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
	void ProcessInput(UCHAR * pKeysBuffer);

private:
	void CreateDescriptorHeap();
	void CreateConstantBufferResorce();
	void CreateConstantBufferView();
	void CreateTextureResourceView(CTexture* pTexture);
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
	생성한 객체들을 관리할 벡터.
	*********************************************************************/
	vector<CObject*>	m_Props;
	vector<CObject*>	m_Players;
	vector<CObject*>	m_Projectiles;
	vector<CTexture*>	m_TextureList;


	/*********************************************************************
	2019-06-16
	PSO 관리를 해야 Render()에서 그릴 수 있을 듯.
	*********************************************************************/
	vector<ID3D12PipelineState*> m_PSO;
};

