#include "stdafx.h"
#include "TitleScene.h"
#include "Camera.h"

TitleScene::~TitleScene()
{
	Release();
}

void TitleScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void TitleScene::Release()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	if (m_pCamera) delete m_pCamera;
}

void TitleScene::Render(ID3D12GraphicsCommandList * m_pd3dCommandList)
{
}

void TitleScene::Update(float fTimeElapsed)
{
}

void TitleScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void TitleScene::CreateConstantBufferResorceAndView()
{
}
