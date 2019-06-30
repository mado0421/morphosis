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

	// Scene 별로 키 Input을 달리 처리하기 위한 함수들
	virtual void ProcessInput(UCHAR * pKeysBuffer)=0;
	virtual void OnProcessingMouseMessage()=0;
	virtual void OnProcessingKeyboardMessage()=0;

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);

	virtual void ReleaseShaderVariables();

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice) = 0;
};

class CTestGroundScene : public CScene {
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

private:
	bool isTimeflow = true;

	vector<ID3D12PipelineState*> m_PSO;

	float					ttt			= 0.0f;

private:
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