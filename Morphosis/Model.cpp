#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "AnimationController.h"
#include "Importer.h"

void CModel::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool isDebug)
{
	if (m_Texture) m_Texture->UpdateShaderVariables(pd3dCommandList);
	if (m_Mesh) m_Mesh->Render(pd3dCommandList);
}

void CModel::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_Mesh) m_Mesh->CreateConstantBufferResource(pd3dDevice, pd3dCommandList);
}

void CModel::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_Mesh) m_Mesh->UpdateConstantBuffer(pd3dCommandList);
}
