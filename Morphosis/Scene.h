#pragma once
class CFramework;
class CCamera;
class Object;

// Scene 자체는 안 쓸 예정이니까 추상클래스로?
// 인터페이스?
class CScene
{
protected:
	ID3D12RootSignature				*m_pd3dGraphicsRootSignature	= NULL;
	ID3D12Device					*m_pd3dDevice					= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList				= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap		= NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	POINT							m_ptOldCursorPos;
	CCamera							*m_pCamera = NULL;
	CFramework						*m_pFramework = NULL;

public:
	CScene();
	CScene(CFramework*);

	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) = 0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void ProcessInput(UCHAR * pKeysBuffer) = 0;
	virtual void Release() = 0;

	// 초기화 함수
	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice) = 0;

protected:
	void CreateDescriptorHeap(int nObject);
	void CreateTextureResourceViews();
	void CreateTextureResourceView(void*);
	virtual void CreateConstantBufferResorce() = 0;
	virtual void CreateConstantBufferView() = 0;
};

class TestScene : public CScene {
public:
	TestScene();
	TestScene(CFramework*);

	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void Release();

	// 초기화 함수
	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);

protected:
	virtual void CreateConstantBufferResorce();
	virtual void CreateConstantBufferView();

	vector<Object*> m_vecObject;
};