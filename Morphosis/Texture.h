#pragma once


struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};



class CTexture
{
public:
	CTexture() = delete;
	CTexture(UINT nResourceType = RESOURCE_TEXTURE2D);
	virtual ~CTexture();

public:
	string							m_strName						= "";

private:
	UINT							TextureType						= RESOURCE_TEXTURE2D;
	ID3D12Resource					*texture						= NULL;
	ID3D12Resource					*uploadBuffer					= NULL;
	SRVROOTARGUMENTINFO				*rootArgumentInfo				= NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		*SamplerGpuDescriptorHandles	= NULL;
	TEXTURETYPE						m_type							= TEXTURETYPE::ALBEDO;

public:
	void SetName(const char* name) { m_strName = name; }
	
	void SetType(TEXTURETYPE type) { m_type = type; }
	TEXTURETYPE GetType() { return m_type; }

	void SetRootArgument(UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE *d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName);

	int GetTextureCount() { return(TextureType); }
	ID3D12Resource *GetTexture() { return(texture); }
	UINT GetTextureType() { return(TextureType); }

	void ReleaseUploadBuffers();
};
