#pragma once
#include "Timer.h"
class Scene;

class Framework {
public:
	Framework();
	~Framework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

public:
	void Render();
	void Update();

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	void ProcessInput();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildScenes();
	void ReleaseScenes();
	
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnResizeBackBuffers();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;
	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4				*m_pdxgiFactory				= NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain			= NULL;
	ID3D12Device				*m_pd3dDevice				= NULL;

	bool						m_bMsaa4xEnable				= false;
	UINT						m_nMsaa4xQualityLevels		= 0;

	static const UINT			m_nSwapChainBuffers			= NUMSWAPCHAINBUFFER;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Fence					*m_pd3dFence				= NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator		= NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue			= NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList			= NULL;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap	= NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer	= NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap	= NULL;
	UINT						m_nDsvDescriptorIncrementSize;


	Timer						m_MainTimer;
	vector<Scene*>				m_vecScenes;
	int							m_CurSceneIdx				= 0;
};