#pragma once
#include "Object.h"
#include "Camera.h"
#include "LevelData.h"

class CFramework;

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
		DEBUG,

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

	// PSO�� ����� �κ�
	virtual void						CreatePipelineStateDesc(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

	// PSO�� ���� �� � Shader.hlsl�� ����� �� ���ϴ� �κе�
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

class CDebugPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};


// Scene ��ü�� �� �� �����̴ϱ� �߻�Ŭ������?
// �������̽�?
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

	CPSO **m_ppCPSOs	= NULL;

public:
	CScene();
	~CScene();

public:
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)=0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList)=0;
	virtual void Update(float fTimeElapsed)=0;

	//virtual void LoadLevel(const char * fileName) = 0;
	//virtual void CreateObjects() = 0;
	//virtual void InitCamera() = 0;
	//virtual void InitPSOs() = 0;


	// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer)=0;
	virtual void OnProcessingMouseMessage()=0;
	virtual void OnProcessingKeyboardMessage()=0;

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// �׽�Ʈ������ ���� Object ������
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	//virtual void ProcessInput(UCHAR * pKeysBuffer);
	//virtual void OnProcessingMouseMessage();
	//virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	//virtual void ProcessInput(UCHAR * pKeysBuffer);
	//virtual void OnProcessingMouseMessage();
	//virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
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
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);

	//// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void OnProcessingMouseMessage();
	virtual void OnProcessingKeyboardMessage();

	// ��� ���۸� ����� �����ϴ� �Լ���
	virtual void CreateObjectBuffers();
	virtual void UpdateObjectBuffers();
	virtual void ReleaseObjectBuffers();
};
