#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "AnimationController.h"

void CModel::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (meshIdx == -1) { cout << "¸Þ½¬ ÀÎµ¦½º -1\n"; return; }



	g_vecTexture[textureDesc.albedoIdx]->UpdateShaderVariables(pd3dCommandList);
	g_vecTexture[textureDesc.normalIdx]->UpdateShaderVariables(pd3dCommandList);
	g_vecMesh[meshIdx]->Render(pd3dCommandList);
	

	//if (m_Texture) m_Texture->UpdateShaderVariables(pd3dCommandList);
	//if (m_Mesh) m_Mesh->Render(pd3dCommandList);
}
