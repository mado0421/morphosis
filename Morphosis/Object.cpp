#include "Object.h"
#include "stdafx.h"
#include "Prefab.h"

void Object::Rotate(const XMFLOAT3& angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)));
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
}

void Object::Move(const XMFLOAT3& velocity)
{
	m_xmf4x4World._41 += velocity.x;
	m_xmf4x4World._42 += velocity.y;
	m_xmf4x4World._43 += velocity.z;
}

void Object::Update(float fTime)
{
}

void Object::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	SetRootParameter(pd3dCommandList);
	UpdateConstantBuffer(pd3dCommandList);
	g_vecPrefab[m_idxPrefab]->Render(pd3dCommandList);
}

Object::Object()
{
	m_xmf4x4World = Matrix4x4::Identity();
}

void Object::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
}

void Object::CreateConstantBufferView(ID3D12Device * pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int & offset)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void **)&m_pCBMappedObjects);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * offset++);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void Object::SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr)
{
	m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr;
}

void Object::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
}

void Object::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	memset(m_pCBMappedObjects, NULL, ncbElementBytes);
	XMStoreFloat4x4(&m_pCBMappedObjects->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}

void Terrain::Update(float fTime)
{
}

void Player::Update(float fTime)
{
}

void Bullet::Update(float fTime)
{
}
