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
	CTexture(const wchar_t* fileName, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nResourceType = RESOURCE_TEXTURE2D);
	~CTexture();

public:
	string							m_Name							= "";

private:
	UINT							TextureType						= RESOURCE_TEXTURE2D;
	ID3D12Resource					*texture						= NULL;
	ID3D12Resource					*uploadBuffer					= NULL;
	SRVROOTARGUMENTINFO				*rootArgumentInfo				= NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		*SamplerGpuDescriptorHandles	= NULL;


public:
	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName);
	void SetRootArgument(UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE *d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	int GetTextureCount() { return(TextureType); }
	ID3D12Resource *GetTexture() { return(texture); }
	UINT GetTextureType() { return(TextureType); }

	void ReleaseUploadBuffers();
};