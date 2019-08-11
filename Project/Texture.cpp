#include "stdafx.h"
#include "Texture.h"

Texture::Texture(UINT nResourceType)
{
	TextureType = nResourceType;

	rootArgumentInfo = new SRVROOTARGUMENTINFO();
	SamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE();
}

Texture::~Texture()
{
	if (texture)
	{
		texture->Release();
		delete texture;
		texture = NULL;
	}
	if (uploadBuffer)
	{
		uploadBuffer->Release();
		delete uploadBuffer;
		uploadBuffer = NULL;
	}
	if (rootArgumentInfo)				delete rootArgumentInfo;
	if (SamplerGpuDescriptorHandles)	delete SamplerGpuDescriptorHandles;
}

void Texture::LoadTextureFromFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const wchar_t * pszFileName)
{
	texture = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &uploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Texture::SetRootArgument(UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle)
{
	rootArgumentInfo->m_nRootParameterIndex = nRootParameterIndex;
	rootArgumentInfo->m_d3dSrvGpuDescriptorHandle = d3dsrvGpuDescriptorHandle;
}

void Texture::SetSampler(D3D12_GPU_DESCRIPTOR_HANDLE * d3dSamplerGpuDescriptorHandle)
{
	SamplerGpuDescriptorHandles = d3dSamplerGpuDescriptorHandle;

}

void Texture::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(static_cast<int>(RootParameter::TEXTURE), rootArgumentInfo->m_d3dSrvGpuDescriptorHandle);

}