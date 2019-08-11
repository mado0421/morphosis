#pragma once
#include "Object.h"
class CCamera;
class CTexture;

// Scene 자체는 안 쓸 예정이니까 추상클래스로?
// 인터페이스?
class CScene
{
public:
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) = 0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void ProcessInput(UCHAR * pKeysBuffer) = 0;
	virtual void Release() = 0;

protected:
	virtual void CreateDescriptorHeap(int nObject);
	virtual void CreateTextureResourceView(CTexture* pTexture);
	virtual void CreateConstantBufferResorce() = 0;
	virtual void CreateConstantBufferView() = 0;

protected:
	ID3D12Device					*m_pd3dDevice				= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList			= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	CCamera							*m_pCamera = NULL;
};

class CSceneMainPlay : public CScene {
public:
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void ProcessInput(UCHAR * pKeysBuffer);
	virtual void Release();

protected:
	virtual void CreateConstantBufferResorce();
	virtual void CreateConstantBufferView();

private:
	vector<CObject>		m_vecObject;
	vector<CUIObject>	m_vecUIObject;
};

//class CSceneLobby : public CScene {
//public:
//	CSceneLobby() {
//	}
//	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//};