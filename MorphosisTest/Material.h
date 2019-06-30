#pragma once
#include "Texture.h"

class CMaterial
{
public:
	XMFLOAT4	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT		m_nReflection = 0;
	CTexture	*m_pTexture = NULL;

public:
	CMaterial();
	virtual ~CMaterial();

public:
	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};