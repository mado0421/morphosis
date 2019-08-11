#pragma once
#include "stdafx.h"

struct MeshData;

class CMesh {
public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices);
	virtual ~CMesh();

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

public:
	string name;

protected:
	ID3D12Resource					*m_pd3dVertexBuffer = NULL;
	ID3D12Resource					*m_pd3dVertexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
};

//class CModelMesh : public CMesh
//{
//public:
//	CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, MeshData& m, float scale = 1.0f);
//};
//
//class CAnimatedModelMesh : public CMesh {
//public:
//	CAnimatedModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, MeshData& m);
//};

class CUIMesh : public CMesh {
public:
	CUIMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
};