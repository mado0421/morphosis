#pragma once
class CFramework;
class CCamera;
class CObjectManager;

struct CB_OBJECT_INFO;

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

	int								m_nMaterial = 0;

	POINT							m_ptOldCursorPos;

	CFramework						*m_pFramework = NULL;

public:
	CScene();
	~CScene();

public:
	// Scene�� �⺻���� �Լ���
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)=0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList)=0;
	virtual void Update(float fTimeElapsed)=0;

	// Scene ���� Ű Input�� �޸� ó���ϱ� ���� �Լ���
	virtual void ProcessInput(UCHAR * pKeysBuffer)=0;
	virtual void OnProcessingMouseMessage()=0;
	virtual void OnProcessingKeyboardMessage()=0;

	// ��� ���۸� ����� �����ϴ� �Լ���
	//virtual void CreateObjectBuffers()=0;
	//virtual void UpdateObjectBuffers()=0;
	//virtual void ReleaseObjectBuffers()=0;

	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	//virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void ReleaseShaderVariables();

	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice) = 0;
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

//class CGroundScene : public CScene 
//{
//protected:
//	ID3D12PipelineState		**m_ppPSO					= nullptr;
//
//public:
//	// Scene�� �⺻���� �Լ���
//	//virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//
//	//virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);
//
//	// ��� ���۸� ����� �����ϴ� �Լ���
//	virtual void CreateObjectBuffers();
//	virtual void UpdateObjectBuffers();
//	virtual void ReleaseObjectBuffers();
//
//	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
//	virtual void CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews);
//	virtual void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
//	//virtual void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);
//
//	virtual void ReleaseShaderVariables();
//
//};

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
	virtual void CreateDescriptorHeap();
	virtual void CreateConstantView();

private:
	bool isTimeflow = true;

	ID3D12PipelineState ** pso							= NULL;
	ID3D12Resource		* interpolatedMatrixResource	= NULL;
	XMMATRIX			* pCBMappedMatrix				= NULL;
	float				ttt = 0.0f;
	ID3D12PipelineState		**m_ppPSO = nullptr;

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