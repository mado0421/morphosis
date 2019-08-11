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

	int nProp = 1;
	int nPlayer = 1;
	int nObject = nProp + nPlayer;
	CreateDescriptorHeap(nObject);


	importer.ImportMesh("PaperBox", "PaperBox", MeshType::MODEL);
	importer.ImportMesh("Character001", "Character001", MeshType::ANIMATEDMODEL);

	importer.ImportTexture(L"PaperBox", "PaperBox");
	importer.ImportTexture(L"Character001", "Character001");
	for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);

	importer.ImportAnimClip("CharacterIdle_anim", "CharacterIdle", true);
	importer.ImportAnimClip("CharacterRun_anim", "CharacterRun", true);
	importer.ImportAnimClip("CharacterFire_anim", "CharacterFire", false);
	importer.ImportAnimClip("CharacterStartJump_anim", "CharacterStartJump", false);
	importer.ImportAnimClip("CharacterEndJump_anim", "CharacterEndJump", false);
	importer.ImportAnimClip("CharacterDied_anim", "CharacterDied", false);

	CreateModel("PaperBox_box_1", "PaperBox", "PaperBox");
	CreateModel("Character_body", "Character001", "Character_body");
	CreateModel("Character_jumper", "Character001", "Character_jumper");
	CreateModel("Character_mask", "Character001", "Character_mask");

	CreateAnimCtrl("CharacterAnimCtrl");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterIdle");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterRun");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterFire");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterStartJump");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterEndJump");
	AddAnimClipToCtrl("CharacterAnimCtrl", "CharacterDied");


	int count = 0;
	for (int i = 0; i < nObject; ++i) {
		Object* obj = new Object();
		obj->AddModel("PaperBox");
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + g_nCbvSrvDescriptorIncrementSize * count++);
		m_vecObject.push_back(obj);
	}
	for (int i = 0; i < nObject; ++i) {
		PlayerObject* obj = new PlayerObject();
		obj->AddModel("Character_body");
		obj->AddModel("Character_jumper");
		obj->AddModel("Character_mask");
		obj->SetAnimCtrl("CharacterAnimCtrl");
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + g_nCbvSrvDescriptorIncrementSize * count++);
		m_vecPlayerObject.push_back(obj);
	}


	CreateConstantBufferResorceAndView();
}

void MainPlayScene::Release()
{
	for (int i = 0; i < m_vecObject.size(); ++i) delete m_vecObject[i];
	for (int i = 0; i < m_vecPlayerObject.size(); ++i) delete m_vecPlayerObject[i];

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

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObjects[static_cast<int>(PSO::ANIMATEDMODEL)]);
	for (int i = 0; i < m_vecPlayerObject.size(); ++i) m_vecPlayerObject[i]->Render(m_pd3dCommandList);
}

void MainPlayScene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->Update(fTimeElapsed);
	for (int i = 0; i < m_vecPlayerObject.size(); ++i) m_vecPlayerObject[i]->Update(fTimeElapsed);
}

void MainPlayScene::ProcessInput(UCHAR * pKeysBuffer)
{
}

void MainPlayScene::CreateConstantBufferResorceAndView()
{
	int offset = 0;
	for (int i = 0; i < m_vecObject.size(); ++i) m_vecObject[i]->CreateConstantBufferResourceAndView(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, offset++);
	for (int i = 0; i < m_vecPlayerObject.size(); ++i) m_vecPlayerObject[i]->CreateConstantBufferResourceAndView(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, offset++);
}
