#include "stdafx.h"
#include "CTest.h"


Object::Object()
{
}


Object::~Object()
{
	
}

void ObjectManager::CreateCbvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nObjects;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT result = pDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&pCbvDescriptorHeap);

	CPUDescriptorStartHandle = pCbvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	GPUDescriptorStartHandle = pCbvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void ObjectManager::CreateObjectResource()
{
	//UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255); //256의 배수
	//m_pd3dcbObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * nConstantBufferViews, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_pd3dcbObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void ObjectManager::CreateConstantBufferViews()
{
}

ObjectManager::ObjectManager()
{
	// 서술자 힙을 만듦

	// 버퍼 리소스를 만듦(그래야 맵핑하고 내용 갱신해주니까)

	// 상수버퍼 서술자를 만듦





}

ObjectManager::ObjectManager(ID3D12GraphicsCommandList * CommandList)
{
	pCommandList = CommandList;

	CreateCbvDescriptorHeaps();
	CreateObjectResource();
	CreateConstantBufferViews();
}

ObjectManager::~ObjectManager()
{
}
