#pragma once

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class Texture {
public:
	Texture(UINT nResourceType = RESOURCE_TEXTURE2D);
	~Texture();

public:
	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName);
	void SetRootArgument(UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE *d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	int GetTextureCount() { return(TextureType); }
	ID3D12Resource *GetTexture() { return(texture); }
	UINT GetTextureType() { return(TextureType); }

public:
	string m_textureName;

private:
	UINT							TextureType = RESOURCE_TEXTURE2D;
	ID3D12Resource					*texture = NULL;
	ID3D12Resource					*uploadBuffer = NULL;
	SRVROOTARGUMENTINFO				*rootArgumentInfo = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		*SamplerGpuDescriptorHandles = NULL;
};