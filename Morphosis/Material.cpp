#include "stdafx.h"
#include "Material.h"

CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) delete m_pTexture;
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	m_pTexture = pTexture;
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}