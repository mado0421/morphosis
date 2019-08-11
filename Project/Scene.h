#pragma once

class Camera;
class Texture;

class Scene {
public:
	Scene();
	~Scene();

public:
	virtual void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void Release();

	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void ProcessInput(UCHAR * pKeysBuffer) = 0;

protected:
	void CreateDescriptorHeap(int nObject);
	void CreateTextureResourceView(Texture* pTexture);
	virtual void CreateConstantBufferResorceAndView() = 0;

protected:
	ID3D12Device					*m_pd3dDevice					= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList				= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap		= NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	Camera							*m_pCamera = NULL;
};



//class LobbyScene : public Scene {
//public:
//	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//};
//
//
//
//class ResultScene : public Scene {
//public:
//	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//};