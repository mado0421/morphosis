#pragma once
#include "Object.h"
#include "Camera.h"

/*
GPU로 전달해주기 위한 상수버퍼 형식 구조체들
위치, 재질

크기가 변하는 UI 같은 경우는 전체 크기와 변화량을 같이 넣어줘야 한다.
*/

//// 기본적인 오브젝트들 전용(크기 변동이 없는 기본적인 UI
//struct CB_OBJECT_INFO {
//	XMFLOAT4X4	m_xmf4x4World;
//	UINT		m_nMaterialIndex;
//};
//
//struct CB_FLEXIBLEUI_INFO {
//	XMFLOAT4X4	m_xmf4x4World;
//	UINT		m_nMaterialIndex;
//	XMFLOAT4	m_xmf4Rect;			// var top left bottom right
//};

/*
EnterRoomScene과 PlayScene에서 사용할 클래스
조명, 오브젝트, 캐릭터, UI, 레벨 데이터, 파티클, 이펙트, 투사체 등의 배열이 필요
이를 총괄할 ID3D12Resource 인터페이스가 필요함

Scene에서 PSO를 가져야 할까?
PSO 리스트를 Scene에서 가져야 하는가?
Shader 클래스에서 가지고 있던 것은?
서술자 핸들도 갖고 있네 뭐지
*/
namespace PSO {
	enum {
		//CHAR = 0,
		//OBJECT,
		//UI,
		//FLEXIBLE_UI,
		//FLOATING_UI,
		//FLOATING_FLEXIBLE_UI,
		//LEVEL,
		//EFFECT,
		//PARTICLE,
		TEXTURE=0,
		ILLUMINATEDTEXTURE,
		MODEL,

		count
	};
}

class CPSO
{
public:
	CPSO();
	~CPSO();

public:
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);


	// PSO를 만드는 부분
	virtual void						CreatePipelineStateDesc(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

	// PSO를 만들 때 어떤 Shader.hlsl을 사용할 지 정하는 부분들
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	ID3D12PipelineState					*GetPipelineState();

public:
	ID3D12PipelineState					*m_pd3dPipelineState = NULL;
};

class CTexturedPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CTexturedIlluminatedPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CModelPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

};



// Scene 자체는 안 쓸 예정이니까 추상클래스로?
// 인터페이스?
class CScene
{
protected:
	/*
	Scene이 가져야 할 것들.
	루트 시그니처
	오브젝트 리스트?
	리소스 포인터

	*/
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

	CMaterial						*m_pMaterial = NULL;

	//======================================
	// 테스트 용도
	CTexturedPSO			*TPSO	= NULL;
	CTexturedIlluminatedPSO *TLPSO	= NULL;
	CModelPSO				*MPSO	= NULL;

public:
	CScene();
	~CScene();

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)=0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList)=0;
	virtual void Update(float fTimeElapsed)=0;

	// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer)=0;
	virtual void OnProcessingMouseMessage()=0;
	virtual void OnProcessingKeyboardMessage()=0;

	// 상수 버퍼를 만들고 관리하는 함수들
	virtual void CreateObjectBuffers()=0;
	virtual void UpdateObjectBuffers()=0;
	virtual void ReleaseObjectBuffers()=0;

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);
};

class CGroundScene : public CScene 
{
protected:
	/////////////////////////////////////////////////////////////
	// 테스트용으로 만든 Object 변수들
	CObject		**m_ppObjects	= NULL;
	int			m_nObjects		= 0;
	/////////////////////////////////////////////////////////////



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
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

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
