#include "stdafx.h"
#include "Mesh.h"
#include "CDiffusedCubeMesh.h"

CDiffusedCubeMesh::CDiffusedCubeMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float fSize) 
	:CMesh(pd3dDevice, pd3dCommandList)
{
	CDiffusedCubeMesh::CDiffusedCubeMesh(pd3dDevice, pd3dCommandList, fSize, fSize, fSize);
}

CDiffusedCubeMesh::CDiffusedCubeMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, float fWidth, float fHeight, float fDepth)
	: CMesh(pd3dDevice, pd3dCommandList)
{
	UINT i = 0;
	float hfWidth = fWidth / 2;
	float hfHeight = fHeight / 2;
	float hfDepth = fDepth / 2;

	m_nStride = sizeof(CDiffusedVertex);
	m_nVertices = 8;
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	CDiffusedVertex* pVertices = new CDiffusedVertex[m_nVertices];

	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-hfWidth, +hfHeight, -hfDepth),XMFLOAT4(1,0,0,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-hfWidth, +hfHeight, +hfDepth),XMFLOAT4(0,1,0,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+hfWidth, +hfHeight, +hfDepth),XMFLOAT4(0,0,1,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+hfWidth, +hfHeight, -hfDepth),XMFLOAT4(1,0,1,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-hfWidth, -hfHeight, -hfDepth),XMFLOAT4(1,1,0,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-hfWidth, -hfHeight, +hfDepth),XMFLOAT4(0,1,1,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+hfWidth, -hfHeight, +hfDepth),XMFLOAT4(0,0,1,1));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+hfWidth, -hfHeight, -hfDepth),XMFLOAT4(0,0,0,1));

	CreateVertexBuffer(pd3dDevice, pd3dCommandList, pVertices);

	m_nIndices = 36;
	UINT pnIndices[36];

	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	CreateIndexBuffer(pd3dDevice, pd3dCommandList, pnIndices);


}

CDiffusedCubeMesh::~CDiffusedCubeMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}
