#pragma once
#include "Object.h"
#include "Camera.h"
#include "LevelData.h"
#include "PSO.h"

class CFramework;


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

	CMaterial						**m_ppMaterial = NULL;
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
	virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void ReleaseShaderVariables();

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);
};

class CGroundScene : public CScene 
{
protected:
	/////////////////////////////////////////////////////////////
	// 테스트용으로 만든 Object 변수들
	CCollideObejct		**m_ppObjects			= NULL;
	int					m_nObjects				= 0;

	CObject				**m_ppDebugObjects		= NULL;
	int					m_nDebugObjects			= 0;

	CPlayerObject		**m_ppPlayers			= NULL;
	int					m_nPlayers				= 0;

	CProjectileObject	**m_ppProjectileObjects = NULL;
	int					m_nProjectileObjects	= 0;

	CDefaultUI			**m_ppUIObjects = NULL;
	int					m_nUIObjects = 0;
	/////////////////////////////////////////////////////////////

	CLevelData *m_pLevel = NULL;


public:
	CGroundScene();
	~CGroundScene();

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

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

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

class CEnterRoomScene : public CGroundScene
{
protected:

public:
	CEnterRoomScene();
	~CEnterRoomScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

class CPlayScene : public CGroundScene
{
protected:

public:
	CPlayScene();
	~CPlayScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

class CTitleScene : public CLoadingScene
{
protected:

public:
	CTitleScene();
	~CTitleScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

class CMatchingScene : public CLoadingScene
{
protected:

public:
	CMatchingScene();
	~CMatchingScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

class CResultScene : public CLoadingScene
{
protected:

public:
	CResultScene();
	~CResultScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};

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
	ID3D12PipelineState * pso = NULL;

};