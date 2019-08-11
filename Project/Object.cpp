#include "stdafx.h"
#include "Object.h"
#include "Model.h"

Object::Object()
{
	m_xmf4x4World = Matrix4x4::Identity();
}

Object::~Object()
{
	if (m_pd3dCBResource)
	{
		m_pd3dCBResource->Unmap(0, NULL);
		m_pd3dCBResource->Release();
	}
}

void Object::SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr)
{
	m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr;
}


void Object::CreateConstantBufferResourceAndView(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int offset)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void **)&m_pCBMappedObjects);
		d3dGpuVirtualAddress		= m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes		= ncbElementBytes;
		d3dCBVDesc.BufferLocation	= d3dGpuVirtualAddress;
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * offset++);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void Object::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_OBJECT_INFO	*pbMappedcbObject;
	UINT ncbElementBytes;

	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedObjects);
	memset(pbMappedcbObject, NULL, ncbElementBytes);
	XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}

void Object::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	UpdateConstantBuffer(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootDescriptorTable(static_cast<int>( RootParameter::OBJECT), m_d3dCbvGPUDescriptorHandle);
	if (!m_vecModelIdx.empty()) for (int i = 0; i < m_vecModelIdx.size(); ++i) g_vecModel[m_vecModelIdx[i]]->Render(pd3dCommandList);
}

void Object::Update(float elapsedTime)
{
}

void Object::LateUpdate(float elapsedTime)
{
}

void Object::AddModel(const char * modelName)
{
	for (int i = 0; i < g_vecModel.size(); ++i) if (g_vecModel[i]->m_ModelName == modelName) {
		m_vecModelIdx.push_back(i); 
		return;
	}
}

void Object::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

MovableObject::MovableObject() : Object()
{
}

MovableObject::~MovableObject()
{
}

void MovableObject::Update(float elapsedTime)
{
}

void MovableObject::LateUpdate(float elapsedTime)
{
}

void PlayerObject::CreateConstantBufferResourceAndView(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int offset)
{
	Object::CreateConstantBufferResourceAndView(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, offset);

	UINT ncbElementBytes = (((sizeof(XMMATRIX) * NUMANIMBONE) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbAnimation = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbAnimation->Map(0, NULL, (void **)&m_pcbxmAnimation);
}

void PlayerObject::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_pd3dcbAnimation)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbAnimation->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(static_cast<int>(RootParameter::ANIMATION), d3dcbBoneOffsetsGpuVirtualAddress);

		//for (int i = 0; i < m_pAnimCtrl->GetNumOfBone(); i++)
		//{
		//	m_pcbxmAnimation[i] = m_pAnimCtrl->GetAnimationMatrix(i, time);
		//}
	}
}

void PlayerObject::Update(float elapsedTime)
{
	m_fAnimTime += elapsedTime;
	if (m_AnimCtrlIdx != -1) {
		for (int i = 0; i < g_vecAnimCtrl[m_AnimCtrlIdx]->GetNumOfBone(); i++)
		{
			m_pcbxmAnimation[i] = g_vecAnimCtrl[m_AnimCtrlIdx]->GetAnimationMatrix(i, m_fAnimTime);
		}
	}
	else for(int i = 0; i < NUMANIMBONE; ++i) m_pcbxmAnimation[i] = XMMatrixIdentity();
}

void PlayerObject::LateUpdate(float elapsedTime)
{
}

void PlayerObject::SetAnimCtrl(const char* AnimCtrlName)
{
	for (int i = 0; i < g_vecAnimCtrl.size(); ++i) if (g_vecAnimCtrl[i]->m_AnimCtrlName == AnimCtrlName) m_AnimCtrlIdx = i;
}
