#include "stdafx.h"
#include "MainPlayScene.h"
#include "Importer.h"
#include "Object.h"
#include "Camera.h"
#include "PipelineStateObject.h"

MainPlayScene::~MainPlayScene()
{
	Release();
}

void MainPlayScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	Importer importer(pd3dDevice, pd3dCommandList);
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	m_pCamera = new Camera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->SetPosition(0, 100, -10);
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));

	int nObject = 1;
	CreateDescriptorHeap(nObject);


	importer.ImportMesh("PaperBox", "PaperBox", MeshType::MODEL);
	importer.ImportTexture(L"PaperBox", "PaperBox");
	for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);
	CreateModel("PaperBox_box_1", "PaperBox", "PaperBox");



	for (int i = 0; i < nObject; ++i) {
		Object* obj = new Object();
		obj->AddModel("PaperBox");
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + g_nCbvSrvDescriptorIncrementSize * i);
		m_vecObject.push_back(obj);
	}


	CreateConstantBufferResorceAndView();
}

void MainPlayScene::Release()
{
	for (int i = 0; i < m_vecObject.size(); ++i) delete m_vecObject[i];

	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	if (m_pCamera) delete m_pCamera;
}

void MainPlayScene::Render(ID3D12GraphicsCommandList * m_pd3dCommandList)
{
	m_pd3dCommandList->SetGraphicsRootSignature(g_pd3dGraphicsRootSignature);
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	m_pCamera->UpdateShaderVariables(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObjects[static_cast<int>(PSO::MODEL)]);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->Render(m_pd3dCommandList);
}

void MainPlayScene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->Update(fTimeElapsed);
}

void MainPlayScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void MainPlayScene::CreateConstantBufferResorceAndView()
{
	int offset = 0;
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->CreateConstantBufferResourceAndView(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, offset++);
}
