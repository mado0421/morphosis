#include "stdafx.h"
#include "Mesh.h"
#include "AnimationController.h"
#include "Importer.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices)
{
	m_nStride = sizeof(CVertex);
	m_nVertices = nVertices;

	CVertex *pVertices = new CVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CVertex(pxmf3Positions[i]);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CVertex) * m_nVertices;

	delete[]pVertices;
}
CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer)			m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer)	m_pd3dVertexUploadBuffer->Release();
}
void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}
void CMesh::CreateVertexBuffer(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pData)
{
	assert(m_nVertices&&m_nStride&&"m_nVertices or m_nStride is zero\n");
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pData, m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

}

void CModelMesh::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}
void CModelMesh::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;

	if (NULL == pnIndices) {
		cout << "NULL\n";
		exit(1);
	}

	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}
void CModelMesh::CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}
void CModelMesh::CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}
CModelMesh::CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}
CModelMesh::CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(CModelVertex);
	m_nVertices = nVertices;

	CModelVertex *pVertices = new CModelVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CModelVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2UVs[i]);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[]pVertices;
}
CModelMesh::CModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ImportMeshData & m, float scale) : CMesh(pd3dDevice, pd3dCommandList)
{
	cout << "CModelMesh 생성 - " << sizeof(CModelMesh) << "\n";

	int nCtrlPoint = static_cast<int>(m.m_nCtrlPointList);
	int nPolyongVertex = static_cast<int>(m.m_nVertexList);

	m_nStride = sizeof(CModelVertex);
	m_nVertices = nPolyongVertex;

	XMFLOAT3* pos = new XMFLOAT3[nCtrlPoint];
	for (int i = 0; i < nCtrlPoint; ++i) pos[i] = Vector3::Multiply(scale, m.m_CtrlPointList[i].xmf3Position);

	XMFLOAT2* uv = new XMFLOAT2[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i) uv[i] = m.m_VertexList[i].xmf2UV;

	XMFLOAT3* normal = new XMFLOAT3[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i) normal[i] = m.m_VertexList[i].xmf3Normal;


	int* posIdx = new int[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i) posIdx[i] = m.m_VertexList[i].ctrlPointIdx;

	CModelVertex* Vertex = new CModelVertex[nPolyongVertex];

	for (int i = 0; i < nPolyongVertex; ++i) Vertex[i] = CModelVertex(pos[posIdx[i]], normal[i], uv[i]);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		Vertex, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	delete[] pos;
	delete[] posIdx;
	delete[] uv;
	delete[] normal;
	delete[] Vertex;

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

//CTestMesh::CTestMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) : CModelMesh(pd3dDevice, pd3dCommandList)
//{
//	m_nVertices = 36;
//	m_nStride = sizeof(CModelVertex);
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = 10.0f, fy = 10.0f, fz = 10.0f;
//
//	XMFLOAT3 pxmf3Positions[36];
//	int i = 0;
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	XMFLOAT2 pxmf2TexCoords[36];
//	i = 0;
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	XMFLOAT3 pxmf3Normals[36];
//	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, NULL, 0);
//
//	m_pnIndices = new UINT[m_nVertices];
//	m_pVertices = new CModelVertex[36];
//	for (int i = 0; i < 36; i++) m_pVertices[i] = CModelVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i]);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//
//	delete[]m_pnIndices;
//	delete[]m_pVertices;
//
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
//}
//CTestMesh::CTestMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float size) : CModelMesh(pd3dDevice, pd3dCommandList)
//{
//	m_nVertices = 36;
//	m_nStride = sizeof(CModelVertex);
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = size/2, fy = size/2, fz = size/2;
//
//	XMFLOAT3 pxmf3Positions[36];
//	int i = 0;
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	XMFLOAT2 pxmf2TexCoords[36];
//	i = 0;
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	XMFLOAT3 pxmf3Normals[36];
//	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, NULL, 0);
//
//	m_pVertices = new CModelVertex[36];
//	CModelVertex pVertices[36];
//	for (int i = 0; i < 36; i++) pVertices[i] = CModelVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i]);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
//
//	m_pVertices = pVertices;
//
//	delete[]m_pVertices;
//
//}
//CTestMesh::CTestMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT3 extents) : CModelMesh(pd3dDevice, pd3dCommandList)
//{
//	m_nVertices = 36;
//	m_nStride = sizeof(CModelVertex);
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = extents.x, fy = extents.y, fz = extents.z;
//
//	XMFLOAT3 pxmf3Positions[36];
//	int i = 0;
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	XMFLOAT2 pxmf2TexCoords[36];
//	i = 0;
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	XMFLOAT3 pxmf3Normals[36];
//	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, NULL, 0);
//
//	m_pVertices = new CModelVertex[36];
//	CModelVertex pVertices[36];
//	for (int i = 0; i < 36; i++) pVertices[i] = CModelVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i]);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
//
//	m_pVertices = pVertices;
//	delete[]m_pVertices;
//}
//CTestMesh::CTestMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT3 position, XMFLOAT3 extents) : CModelMesh(pd3dDevice, pd3dCommandList)
//{
//	m_nVertices = 36;
//	m_nStride = sizeof(CModelVertex);
//	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//	float fx = extents.x, fy = extents.y, fz = extents.z;
//
//	XMFLOAT3 pxmf3Positions[36];
//	int i = 0;
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//
//	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
//	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
//
//	for (int i = 0; i < 36; ++i) {
//		pxmf3Positions[i].x += position.x;
//		pxmf3Positions[i].y += position.y;
//		pxmf3Positions[i].z += position.z;
//	}
//
//	XMFLOAT2 pxmf2TexCoords[36];
//	i = 0;
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
//	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
//
//	XMFLOAT3 pxmf3Normals[36];
//	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, NULL, 0);
//
//	m_pVertices = new CModelVertex[36];
//	CModelVertex pVertices[36];
//	for (int i = 0; i < 36; i++) pVertices[i] = CModelVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i]);
//
//	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
//
//	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
//	m_d3dVertexBufferView.StrideInBytes = m_nStride;
//	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
//
//	m_pVertices = pVertices;
//	delete[]m_pVertices;
//}
//CTestMesh::~CTestMesh()
//{
//	CMesh::~CMesh();
//	if (m_pVertices) delete[] m_pVertices;
//}

CAnimatedModelMesh::CAnimatedModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ImportMeshData & m) : CMesh(pd3dDevice, pd3dCommandList)
{
	cout << "CAnimatedModelMesh 생성 - " << sizeof(CAnimatedModelMesh) << "\n";


	int nCtrlPoint = static_cast<int>(m.m_nCtrlPointList);
	int nPolyongVertex = static_cast<int>(m.m_nVertexList);

	UINT nStride = sizeof(CAnimatedVertex);
	m_nVertices = nPolyongVertex;

	XMFLOAT3* pos = new XMFLOAT3[nCtrlPoint];
	for (int i = 0; i < nCtrlPoint; ++i) {
		pos[i] = m.m_CtrlPointList[i].xmf3Position;
		pos[i].z *= -1;
	}
	XMFLOAT4* weight = new XMFLOAT4[nCtrlPoint];
	for (int i = 0; i < nCtrlPoint; ++i) weight[i] = m.m_CtrlPointList[i].xmf4BoneWeight;
	XMINT4* boneIdx = new XMINT4[nCtrlPoint];
	for (int i = 0; i < nCtrlPoint; ++i) boneIdx[i] = m.m_CtrlPointList[i].xmi4BoneIdx;



	XMFLOAT2* uv = new XMFLOAT2[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i) uv[i] = m.m_VertexList[i].xmf2UV;
	int* posIdx = new int[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i) posIdx[i] = m.m_VertexList[i].ctrlPointIdx;
	XMFLOAT3* normal = new XMFLOAT3[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i)
	{
		normal[i] = m.m_VertexList[i].xmf3Normal;
		normal[i].z *= -1;
	}
	XMFLOAT3* tangent = new XMFLOAT3[nPolyongVertex];
	for (int i = 0; i < nPolyongVertex; ++i)
	{
		tangent[i] = m.m_VertexList[i].xmf3Tangent;
		tangent[i].z *= -1;
	}


	CAnimatedVertex* animVertex = new CAnimatedVertex[nPolyongVertex];


	for (int i = 0; i < nPolyongVertex; ++i) 
		animVertex[i] = CAnimatedVertex(pos[posIdx[i]], normal[i], tangent[i], uv[i], weight[posIdx[i]], boneIdx[posIdx[i]]);


	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		animVertex, sizeof(CAnimatedVertex) * nPolyongVertex,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	delete[] pos;
	delete[] weight;
	delete[] boneIdx;
	delete[] uv;
	delete[] posIdx;
	delete[] normal;
	delete[] tangent;
	delete[] animVertex;

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = nStride;
	m_d3dVertexBufferView.SizeInBytes = nStride * nPolyongVertex;
}

CUIMesh::CUIMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float width, float height) : CMesh(pd3dDevice, pd3dCommandList)
{
	cout << "CUIMesh 생성 - " << sizeof(CUIMesh) << "\n";

	m_nStride = sizeof(CUIVertex);
	m_nVertices = 6;
	int i = 0;
	XMFLOAT3 l_position[6];
	l_position[i++] = XMFLOAT3(0, 0, 0);
	l_position[i++] = XMFLOAT3(0, width, 0);
	l_position[i++] = XMFLOAT3(0, width, -height);

	l_position[i++] = XMFLOAT3(0, 0, 0);
	l_position[i++] = XMFLOAT3(0, width, -height);
	l_position[i++] = XMFLOAT3(0, 0, -height);
	i = 0;
	XMFLOAT2  l_texCoord[6];
	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);

	l_texCoord[i++] = XMFLOAT2(0, 0);
	l_texCoord[i++] = XMFLOAT2(1, 1);
	l_texCoord[i++] = XMFLOAT2(0, 1);


	CUIVertex *pVertices = new CUIVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CUIVertex(l_position[i], l_texCoord[i]);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation	= m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nStride * m_nVertices;

	delete[]pVertices;
}
