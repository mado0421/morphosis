#include "stdafx.h"
#include "Texture.h"


CTexture::CTexture( UINT nTextureType)
{
	TextureType = nTextureType;

	rootArgumentInfo = new SRVROOTARGUMENTINFO();
	SamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE();
}

CTexture::~CTexture()
{
	if (texture)						texture->Release();
	if (uploadBuffer)					uploadBuffer->Release();
	if (rootArgumentInfo)				delete rootArgumentInfo;
	if (SamplerGpuDescriptorHandles)	delete SamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	rootArgumentInfo->m_nRootParameterIndex = nRootParameterIndex;
	rootArgumentInfo->m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE *d3dSamplerGpuDescriptorHandle)
{
	SamplerGpuDescriptorHandles = d3dSamplerGpuDescriptorHandle;
}

/*g_RootParameterTexture + static_cast<int>( m_type )*/

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(
		rootArgumentInfo->m_nRootParameterIndex,
		rootArgumentInfo->m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (uploadBuffer)
	{
		uploadBuffer->Release();
		delete uploadBuffer;
		uploadBuffer = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName)
{
	texture = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &uploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}