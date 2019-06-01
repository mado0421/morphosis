#pragma once
#include "../Scene/Scene.h"
#include "../Timer/Timer.h"

#define NUMSWAPCHAINBUFFER 2
#define NUMDEFAULTSTRINGCHAR 64

class CFramework
{
private:
	/////////////////////////////////////////////////////////////////////////////////////
	// 扩档快 芒 积己何
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;
	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
	_TCHAR						m_pszFrameRate[NUMDEFAULTSTRINGCHAR];

	/////////////////////////////////////////////////////////////////////////////////////
	// DirectX 12 积己何
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = NUMSWAPCHAINBUFFER;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	UINT						m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	/////////////////////////////////////////////////////////////////////////////////////
	// 扁鸥
	CTimer						m_MainTimer;
	CScene						**m_ppScenes		= NULL;
	CScene						*m_pCurrentScene	= NULL;
	int							m_nScenes			= 0;

public:
	CFramework();
	~CFramework();

public:
	void Render();
	void Update();

public:
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList();

	void OnResizeBackBuffers();

	void BuildScenes();
	void ChangeScene(int targetSceneIdx, void * subData = NULL);
	void ReleaseScenes();

	void ProcessInput();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void WaitForGpuComplete();
	void MoveToNextFrame();
};

