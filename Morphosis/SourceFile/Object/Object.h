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

	const XMFLOAT3 GetPosition();
	const XMFLOAT3 GetLook();
	const XMFLOAT3 GetUp();
	const XMFLOAT3 GetRight();
	XMMATRIX GetAnimationMatrix(int boneIdx);
	int GetNumAnimationBone();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
	void SetCameraTargetPos(XMFLOAT3 pos);
	XMFLOAT3 GetCameraTargetPos();

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

public:
	XMFLOAT4X4						m_xmf4x4World;


private:
	bool							m_IsAlive;
	std::vector<CModel>				m_ModelList;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
	ID3D12Resource					*m_pd3dcbObject			= NULL;

	XMFLOAT3						m_xmf3CameraTargetPos;

	/*************************************************************************
	이동 관련 파트*/
	XMFLOAT3						m_xmf3Variation;
	XMFLOAT3						m_xmf3RotateAngle;
	XMFLOAT3						m_xmf3CollisionOffset;
	float							m_fSpeed;
	bool							m_trigInput[count];

	/*************************************************************************
	애니메이션 관련 파트*/
	CAnimationController			*m_AnimationController = NULL;

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
		//m_nAnimationMatrix	= 0;
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
	//unsigned int		m_nAnimationMatrix				= 0;
	
	ID3D12Resource*		m_pd3dCBPropResource			= NULL;
	ID3D12Resource*		m_pd3dCBPlayersResource			= NULL;
	ID3D12Resource*		m_pd3dCBProjectilesResource		= NULL;
	//ID3D12Resource*		m_pd3dCBAnimationMatrixResource = NULL;

	CB_OBJECT_INFO*		m_pCBMappedPropObjects			= NULL;
	CB_OBJECT_INFO*		m_pCBMappedPlayers				= NULL;
	CB_OBJECT_INFO*		m_pCBMappedProjectiles			= NULL;
	//XMMATRIX*			m_pCBMappedAnimationMatrix		= NULL;

	/*********************************************************************
	2019-06-15
	생성한 객체들을 관리할 벡터.
	*********************************************************************/
	//vector<CObject*>	m_Objects;
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

