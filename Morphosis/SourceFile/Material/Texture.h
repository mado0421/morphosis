#pragma once
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class CTexture
{
public:
	CTexture(UINT nResourceType = RESOURCE_TEXTURE2D);
	virtual ~CTexture();

private:
	UINT							TextureType						= RESOURCE_TEXTURE2D;
	ID3D12Resource					*texture						= NULL;
	ID3D12Resource					*uploadBuffer					= NULL;
	SRVROOTARGUMENTINFO				*rootArgumentInfo				= NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		*SamplerGpuDescriptorHandles	= NULL;


public:

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