#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

void Model::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	g_vecTexture[m_TextureIdx]->UpdateShaderVariables(pd3dCommandList);
	g_vecMesh[m_MeshIdx]->Render(pd3dCommandList);
}
