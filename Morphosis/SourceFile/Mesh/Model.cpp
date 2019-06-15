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
	if (isDebug) {
		if (m_CollisionBox) m_CollisionBox->Render(pd3dCommandList);
	}
	else {
		if (m_Mesh) m_Mesh->Render(pd3dCommandList);
	}
}

void CCollisionBox::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
}
