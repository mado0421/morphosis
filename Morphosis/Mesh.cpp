#include "stdafx.h"
#include "Mesh.h"
#include "Vertex.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices)
{
	m_nStride = sizeof(CVertex);
	m_nVertices = nVertices;

	CVertex *pVertices = new CVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) {
		pVertices[i].m_xmf3Position = pxmf3Positions[i];
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CVertex) * m_nVertices;

	delete[]pVertices;
}
CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) {
		m_pd3dVertexBuffer->Unmap(0, NULL);
		m_pd3dVertexBuffer->Release();
		m_pd3dVertexBuffer = NULL;
	}
	if (m_pd3dVertexUploadBuffer) {
		m_pd3dVertexUploadBuffer->Unmap(0, NULL);
		m_pd3dVertexUploadBuffer->Release();
		m_pd3dVertexUploadBuffer = NULL;
	}
}
void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

//CModelMesh::CModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, MeshData & m, float scale) : CMesh(pd3dDevice, pd3dCommandList)
//{
//	int nCtrlPoint		= static_cast<int>(m.m_nCtrlPointList);
//	int nPolyongVertex	= static_cast<int>(m.m_nVertexList);
//
//	m_nStride = sizeof(CModelVertex);
//	m_nVertices = nPolyongVertex;
//
//	XMFLOAT3* pos = new XMFLOAT3[nCtrlPoint];
//	for (int i = 0; i < nCtrlPoint; ++i) pos[i] = Vector3::Multiply(scale, m.m_CtrlPointList[i].xmf3Position);
//
//	XMFLOAT2* uv = new XMFLOAT2[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i) uv[i] = m.m_VertexList[i].xmf2UV;
//
//	XMFLOAT3* normal = new XMFLOAT3[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i) normal[i] = m.m_VertexList[i].xmf3Normal;
//
//
//	int* posIdx = new int[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i) posIdx[i] = m.m_VertexList[i].ctrlPointIdx;
//
//	CModelVertex* Vertex = new CModelVertex[nPolyongVertex];
//
//	for (int i = 0; i < nPolyongVertex; ++i) { 
//		Vertex[i].m_xmf3Position	= pos[posIdx[i]]; 
//		Vertex[i].m_xmf3Normal		= normal[i];
//		Vertex[i].m_xmf2TexCoord	= uv[i];
//	}
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
//		Vertex, m_nStride * m_nVertices,
//		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//
//	delete[] pos;
//	delete[] posIdx;
//	delete[] uv;
//	delete[] normal;
//	delete[] Vertex;
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
//}
//
//
//
//CAnimatedModelMesh::CAnimatedModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, MeshData & m) : CMesh(pd3dDevice, pd3dCommandList)
//{
//
//
//	int nCtrlPoint = static_cast<int>(m.m_nCtrlPointList);
//	int nPolyongVertex = static_cast<int>(m.m_nVertexList);
//
//	UINT nStride = sizeof(CAnimatedVertex);
//	m_nVertices = nPolyongVertex;
//
//	XMFLOAT3* pos = new XMFLOAT3[nCtrlPoint];
//	for (int i = 0; i < nCtrlPoint; ++i) {
//		pos[i] = m.m_CtrlPointList[i].xmf3Position;
//		pos[i].z *= -1;
//	}
//	XMFLOAT4* weight = new XMFLOAT4[nCtrlPoint];
//	for (int i = 0; i < nCtrlPoint; ++i) weight[i] = m.m_CtrlPointList[i].xmf4BoneWeight;
//	XMINT4* boneIdx = new XMINT4[nCtrlPoint];
//	for (int i = 0; i < nCtrlPoint; ++i) boneIdx[i] = m.m_CtrlPointList[i].xmi4BoneIdx;
//
//
//
//	XMFLOAT2* uv = new XMFLOAT2[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i) uv[i] = m.m_VertexList[i].xmf2UV;
//	int* posIdx = new int[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i) posIdx[i] = m.m_VertexList[i].ctrlPointIdx;
//	XMFLOAT3* normal = new XMFLOAT3[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i)
//	{
//		normal[i] = m.m_VertexList[i].xmf3Normal;
//		normal[i].z *= -1;
//	}
//	XMFLOAT3* tangent = new XMFLOAT3[nPolyongVertex];
//	for (int i = 0; i < nPolyongVertex; ++i)
//	{
//		tangent[i] = m.m_VertexList[i].xmf3Tangent;
//		tangent[i].z *= -1;
//	}
//
//
//	CAnimatedVertex* animVertex = new CAnimatedVertex[nPolyongVertex];
//
//	for (int i = 0; i < nPolyongVertex; ++i) {
//		animVertex[i].m_xmf3Position	= pos[posIdx[i]];
//		animVertex[i].m_xmf3Normal		= normal[i];
//		animVertex[i].m_xmf3Tangent		= tangent[i];
//		animVertex[i].m_xmf2TexCoord	= uv[i];
//		animVertex[i].m_xmf4BoneWeight	= weight[posIdx[i]];
//		animVertex[i].m_xmi4BoneIdx		= boneIdx[posIdx[i]];
//	}
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
//		animVertex, sizeof(CAnimatedVertex) * nPolyongVertex,
//		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
//		&m_pd3dVertexUploadBuffer);
//
//	delete[] pos;
//	delete[] weight;
//	delete[] boneIdx;
//	delete[] uv;
//	delete[] posIdx;
//	delete[] normal;
//	delete[] tangent;
//	delete[] animVertex;
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = nStride;
//	m_d3dVertexBufferView.SizeInBytes = nStride * nPolyongVertex;
//}

CUIMesh::CUIMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{

	m_nStride = sizeof(CUIVertex);
	m_nVertices = 6;
	int i = 0;

	XMFLOAT2  l_texCoord[6];
	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);

	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);
	l_texCoord[i++] = XMFLOAT2(0, 1);

	CUIVertex *pVertices = new CUIVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) {
		pVertices[i].m_xmf3Position = XMFLOAT3(0, 0, 0);
		pVertices[i].m_xmf2TexCoord = l_texCoord[i];
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	= m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nStride * m_nVertices;

	delete[] pVertices;
}
