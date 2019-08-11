#include "stdafx.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Importer.h"

Mesh::Mesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

Mesh::~Mesh()
{
	if (m_pd3dVertexBuffer)			m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer)	m_pd3dVertexUploadBuffer->Release();
}

void Mesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

ModelMesh::ModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, MeshData& meshData)
	:Mesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(ModelVertex);
	m_nVertices = static_cast<UINT>( meshData.vecVertex.size() );
	
	ModelVertex* arrVertex = new ModelVertex[m_nVertices];

	for (UINT i = 0; i < m_nVertices; ++i) {
		arrVertex[i].m_xmf3Position = meshData.vecCtrlPoint[meshData.vecVertex[i].ctrlPointIdx].m_xmf3Position;
		arrVertex[i].m_xmf3Normal	= meshData.vecVertex[i].m_xmf3Normal;
		arrVertex[i].m_xmf2TexCoord = meshData.vecVertex[i].m_xmf2TexCoord;
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		arrVertex, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	= m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nStride * m_nVertices;

	delete[] arrVertex;
}

AnimatedModelMesh::AnimatedModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, MeshData& meshData)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(AnimatedModelVertex);
	m_nVertices = static_cast<UINT>( meshData.vecVertex.size() );

	AnimatedModelVertex* arrVertex = new AnimatedModelVertex[m_nVertices];

	for (UINT i = 0; i < m_nVertices; ++i) {
		arrVertex[i].m_xmf3Position		= meshData.vecCtrlPoint[meshData.vecVertex[i].ctrlPointIdx].m_xmf3Position;
		arrVertex[i].m_xmf3Normal		= meshData.vecVertex[i].m_xmf3Normal;
		arrVertex[i].m_xmf3Tangent		= meshData.vecVertex[i].m_xmf3Tangent;
		arrVertex[i].m_xmf2TexCoord		= meshData.vecVertex[i].m_xmf2TexCoord;
		arrVertex[i].m_xmf4BoneWeight	= meshData.vecCtrlPoint[meshData.vecVertex[i].ctrlPointIdx].m_xmf4BoneWeight;
		arrVertex[i].m_xmi4BoneIdx		= meshData.vecCtrlPoint[meshData.vecVertex[i].ctrlPointIdx].m_xmi4BoneIdx;

		arrVertex[i].m_xmf3Position.z	*= -1;
		arrVertex[i].m_xmf3Normal.z		*= -1;
		arrVertex[i].m_xmf3Tangent.z	*= -1;
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		arrVertex, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	= m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nStride * m_nVertices;

	delete[] arrVertex;
}

UIMesh::UIMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
	: Mesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(UIVertex);
	m_nVertices = 6;
	int i = 0;
	XMFLOAT2  l_texCoord[6];
	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);

	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);
	l_texCoord[i++] = XMFLOAT2(0, 1);

	UIVertex* arrVertex = new UIVertex[m_nVertices];

	for (UINT i = 0; i < m_nVertices; ++i) {
		arrVertex[i].m_xmf3Position = XMFLOAT3(0,0,0);
		arrVertex[i].m_xmf2TexCoord = l_texCoord[i];
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		arrVertex, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	= m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nStride * m_nVertices;

	delete[] arrVertex;
}
