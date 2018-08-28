#include "stdafx.h"
#include"CDataMesh.h"
#include"CDataShader.h"
#include "CModelObject.h"


CModelObject::CModelObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FBXDataManager* fbxManager):
	CGameObject()
{
	m_nMeshes = 1;
	CMaterial *pMaterial = NULL;

	pMaterial = new CMaterial();
	pMaterial->m_xmf4Albedo = XMFLOAT4(0, 1, 1, 1);

	CDataMesh* pMesh = new CDataMesh(pd3dDevice, pd3dCommandList, fbxManager);
	CGameObject::SetMesh(0, pMesh);

	TCHAR pstrPathName[128] = { '\0' };
	char name[260];
	strcpy(name, fbxManager->m_pName[0].string);
	//name[strlen(name)-5] = '\0';
	_tcscpy_s(pstrPathName, 128, _T("../Assets/Model/"));

	USES_CONVERSION;
	_tcscat_s(pstrPathName, 128, A2T(name));
	//_tcscat_s(pstrPathName, 128, _T("box_diff"));
	_tcscat_s(pstrPathName, 128, _T(".dds"));

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pstrPathName, 0);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("../Assets/Model/body_test_0_flip.dds"), 0);

	pMaterial->SetTexture(pTexture);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CDataShader *pShader = new CDataShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	pMaterial->SetShader(pShader);

	CGameObject::SetMaterial(pMaterial);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
}

CModelObject::~CModelObject()
{
}
