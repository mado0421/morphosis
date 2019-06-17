#include "stdafx.h"
#include "Model.h"
#include "Mesh/Mesh.h"
#include "Material/Texture.h"
#include "Material/Material.h"
#include "Animation/AnimationController.h"
#include "Importer/Importer.h"

void CModel::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool isDebug)
{
	if (m_Texture) m_Texture->UpdateShaderVariables(pd3dCommandList);
	//if (isDebug) 
	//	if (m_CollisionBox) m_CollisionBox->Render(pd3dCommandList);
	if (m_Mesh) m_Mesh->Render(pd3dCommandList);
}

void CModel::SetAnimatedMatrix(CAnimationController * a)
{
	m_Mesh->SetAnimatedMatrix(a);
}

void CModel::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_Mesh->CreateConstantBufferResource(pd3dDevice, pd3dCommandList);
}

void CModel::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_Mesh->UpdateConstantBuffer(pd3dCommandList);
}
