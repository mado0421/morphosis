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
	�̵� ���� �κ�
	*********************************************************************/
	XMFLOAT3						m_xmf3CollisionOffset;

	/*********************************************************************
	2019-06-15
	�ִϸ��̼� ���� �κ�
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
};

