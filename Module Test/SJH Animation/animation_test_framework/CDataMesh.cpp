#include "stdafx.h"
#include "CDataMesh.h"
#include "FBXDataManager.h"

CDataMesh::CDataMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FBXDataManager* fbxManager)
	:CMesh(pd3dDevice,pd3dCommandList)
{
	m_nStride = sizeof(cv::IAVertex);
	m_nVertices = fbxManager->m_nIAVertex;

	CMesh::CreateVertexBuffer(pd3dDevice, pd3dCommandList, fbxManager->m_pIAVertex);

	/*

	Shader Resource
	
	Vertex, UI data

	*/
	UINT nStride;
	UINT nNumber;

	cv::Vertex* pVertex = nullptr;
	nStride = sizeof(cv::Vertex);

	/*Read From Data*/
	nNumber = fbxManager->m_nVertex;
	pVertex = fbxManager->m_pVertex;
	//
	m_pVertexRes = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pVertex, nStride*nNumber,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	cv::UV* pUV = nullptr;
	nStride = sizeof(cv::UV);

	/*Read From Data*/
	nNumber = fbxManager->m_nUV;
	pUV = fbxManager->m_pUV;
	/*Resource State 바뀌어야 할 지도 모름*/
	m_pUVRes = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pUV, nStride*nNumber,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

}


CDataMesh::~CDataMesh()
{
	if (m_pVertexRes)
	{
		m_pVertexRes->Release();
	}

	if (m_pUVRes)
	{
		m_pUVRes->Release();
	}
}

void CDataMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(ROOT_PARAMETER_VERTEX, m_pVertexRes->GetGPUVirtualAddress());
	pd3dCommandList->SetGraphicsRootShaderResourceView(ROOT_PARAMETER_UV, m_pUVRes->GetGPUVirtualAddress());
	CMesh::Render(pd3dCommandList);
}
