#include "stdafx.h"

#include "CDiffusedShader.h"
#include "CDiffusedCubeMesh.h"
#include "CSolidCube.h"


CSolidCube::CSolidCube(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature):CGameObject()
{
	m_nMeshes = 1;
	CMaterial *pMaterial = NULL;

	pMaterial = new CMaterial();
	pMaterial->m_xmf4Albedo = XMFLOAT4(0,1,1,1);

	CDiffusedCubeMesh* pMesh = new CDiffusedCubeMesh(pd3dDevice, pd3dCommandList, 50,50,50);
	CGameObject::SetMesh(0,pMesh);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CDiffusedShader *pShader = new CDiffusedShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 0);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);


	pMaterial->SetShader(pShader);

	CGameObject::SetMaterial(pMaterial);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	
}


CSolidCube::~CSolidCube()
{
	CGameObject::~CGameObject();
}
