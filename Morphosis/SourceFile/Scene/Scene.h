#pragma once
class CFramework;
class CCamera;
class CObjectManager;

struct CB_OBJECT_INFO;

// Scene 자체는 안 쓸 예정이니까 추상클래스로?
// 인터페이스?
class CScene
{
protected:
	ID3D12RootSignature				*m_pd3dGraphicsRootSignature	= NULL;
	ID3D12Resource					*m_pd3dcbObjects				= NULL;
	CB_OBJECT_INFO					*m_pcbMappedGameObjects			= NULL;

	ID3D12PipelineState				**m_ppPipelineStates			= NULL;
	int								m_nPipelineStates				= 0;

	ID3D12Device					*m_pd3dDevice					= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList				= NULL;

	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap		= NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	CCamera							*m_pCamera = NULL;

	//CMaterial						**m_ppMaterial = NULL;
	int								m_nMaterial = 0;

	POINT							m_ptOldCursorPos;

	CFramework						*m_pFramework = NULL;

public:
	CScene();
	~CScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)=0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList)=0;
	virtual void Update(float fTimeElapsed)=0;

	//virtual void LoadLevel(const char * fileName) = 0;
	//virtual void CreateObjects() = 0;
	//virtual void InitCamera() = 0;
	//virtual void InitPSOs() = 0;


	// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer)=0;
	virtual void OnProcessingMouseMessage()=0;
	virtual void OnProcessingKeyboardMessage()=0;

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers()=0;
	virtual void UpdateObjectBuffers()=0;
	virtual void ReleaseObjectBuffers()=0;

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	//virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void ReleaseShaderVariables();

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);
};

struct CB_DESC {
	UINT								nMappedData;
	LPVOID								pMappedPtr;
	UINT								RootParamIdx;
	D3D12_CONSTANT_BUFFER_VIEW_DESC		view_desc;
	D3D12_CPU_DESCRIPTOR_HANDLE			hCPUDescTable;
	D3D12_GPU_DESCRIPTOR_HANDLE			hGPUDescTable;
};

inline bool AllocUploadBuffer(
	CB_DESC& cb_desc, 
	UINT nBytes, 
	ID3D12Resource *resource, 
	UINT *UploadBufferCurrentIdx, 
	void *UploadBufferMappedPtr) 
{
	if (nullptr == resource) return false;
	UINT ncbElementBytes = ((nBytes + 255) & ~255);

	cb_desc.nMappedData = ncbElementBytes;
	cb_desc.view_desc.SizeInBytes = ncbElementBytes;
	cb_desc.view_desc.BufferLocation = resource->GetGPUVirtualAddress() + (*UploadBufferCurrentIdx);
	cb_desc.pMappedPtr = reinterpret_cast<LPVOID>(
		(*UploadBufferCurrentIdx) + reinterpret_cast<long long>(UploadBufferMappedPtr));

	(*UploadBufferCurrentIdx) += ncbElementBytes;
	return true;
}

class CGroundScene : public CScene 
{
protected:
	/////////////////////////////////////////////////////////////
	// 테스트용으로 만든 Object 변수들
	//CCollideObejct		**m_ppObjects			= NULL;
	//int					m_nObjects				= 0;
	//CObject				**m_ppDebugObjects		= NULL;
	//int					m_nDebugObjects			= 0;
	//CPlayerObject		**m_ppPlayers			= NULL;
	//int					m_nPlayers				= 0;
	//CProjectileObject	**m_ppProjectileObjects = NULL;
	//int					m_nProjectileObjects	= 0;
	//CDefaultUI			**m_ppUIObjects = NULL;
	//int					m_nUIObjects = 0;
	/////////////////////////////////////////////////////////////

	//CLevelData				*m_pLevelData				= nullptr;
	ID3D12PipelineState		**m_ppPSO					= nullptr;
	//ObjectManager			*m_pObjectMng				= nullptr;

	//CCollideObejct			**m_ppObjCollTerrain		= nullptr;
	//CCollideObejct			**m_ppObjProp				= nullptr;
	//CObject					**m_ppObjRenderTerrain		= nullptr;
	//CMovingObject			**m_ppObjProjectile			= nullptr;
	//CPlayerObject			**m_ppObjPlayer				= nullptr;

	//short					m_nObjCollTerrain			= 0;
	//short					m_nObjProp					= 0;
	//short					m_nObjRenderTerrain			= 0;
	//short					m_nObjProjectile			= 0;
	//short					m_nObjPlayer				= 0;


	//ID3D12Resource			*m_pcbUploadBufferResource	= nullptr;
	//UINT					UploadBufferCurrentIdx = 0;

	//CB_DESC					m_cbDescObj;
	//CB_DESC					m_cbDescAnim;
	//CB_DESC					m_cbDescMat;
	//CB_DESC					m_cbDescLight;

	//CB_OBJECT_INFO			*m_pcbMappedObj			= nullptr;
	//XMMATRIX				*m_pcbMappedAnim		= nullptr;
	//XMMATRIX				*m_pcbMappedMat			= nullptr;
	//XMMATRIX				*m_pcbMappedLight		= nullptr;

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	//virtual void ProcessInput(UCHAR * pKeysBuffer);
	//virtual void OnProcessingMouseMessage();
	//virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	//virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void ReleaseShaderVariables();

};

class CLoadingScene : public CScene
{
protected:

public:
	CLoadingScene();
	~CLoadingScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	//virtual void ProcessInput(UCHAR * pKeysBuffer);
	//virtual void OnProcessingMouseMessage();
	//virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

//class CEnterRoomScene : public CGroundScene
//{
//protected:
//
//public:
//	CEnterRoomScene();
//	~CEnterRoomScene();
//
//public:
//	// Scene의 기본적인 함수들
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//	virtual void OnProcessingMouseMessage();
//	virtual void OnProcessingKeyboardMessage();
//
//	// 상수 버퍼를 만들고 관리하는 함수들
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//};
//
//class CPlayScene : public CGroundScene
//{
//protected:
//
//public:
//	CPlayScene();
//	~CPlayScene();
//
//public:
//	// Scene의 기본적인 함수들
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//	virtual void OnProcessingMouseMessage();
//	virtual void OnProcessingKeyboardMessage();
//
//	// 상수 버퍼를 만들고 관리하는 함수들
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//};
//
//class CTitleScene : public CLoadingScene
//{
//protected:
//
//public:
//	CTitleScene();
//	~CTitleScene();
//
//public:
//	// Scene의 기본적인 함수들
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//	virtual void OnProcessingMouseMessage();
//	virtual void OnProcessingKeyboardMessage();
//
//	// 상수 버퍼를 만들고 관리하는 함수들
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//};
//
//class CMatchingScene : public CLoadingScene
//{
//protected:
//
//public:
//	CMatchingScene();
//	~CMatchingScene();
//
//public:
//	// Scene의 기본적인 함수들
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//	virtual void OnProcessingMouseMessage();
//	virtual void OnProcessingKeyboardMessage();
//
//	// 상수 버퍼를 만들고 관리하는 함수들
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//};
//
//class CResultScene : public CLoadingScene
//{
//protected:
//
//public:
//	CResultScene();
//	~CResultScene();
//
//public:
//	// Scene의 기본적인 함수들
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//	virtual void OnProcessingMouseMessage();
//	virtual void OnProcessingKeyboardMessage();
//
//	// 상수 버퍼를 만들고 관리하는 함수들
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//};

class CTestGroundScene : public CGroundScene {
public:
	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);

	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

private:
	virtual void MakePSO();
	virtual void CreateDescriptorHeap();
	virtual void CreateConstantView();



private:
	bool isTimeflow = true;

	ID3D12PipelineState ** pso							= NULL;
	ID3D12Resource		* interpolatedMatrixResource	= NULL;
	XMMATRIX			* pCBMappedMatrix				= NULL;
	//Anim				animData;

	//CAnimationPlayerObject		**ppPlayers = NULL;
	//int					nPlayers = 0;
	float				ttt = 0.0f;



private:
	//CObject**	m_ppObjects = NULL;
	//int			m_nObjects = 0;
	//int			m_nBulletPerPlayer = 32;
	//int			m_nPlayers = 0;
	//int			m_nSRV = 1;
	
	CObjectManager* m_ObjMng = NULL;

	enum {
		OFFSET,
		OFFSETINV,
		TOROOT,
		TOROOTINV
	};

	
	bool		m_bDebug = false;
	bool		m_bUploadAnimMtx = true;
	int			m_ShowBonesMode = OFFSET;
	
};