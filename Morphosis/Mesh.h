#pragma once
#include "stdafx.h"

struct ImportMeshData;
class AnimationMesh;
class CAnimationController;

class CVertex {
public:
	XMFLOAT3	m_xmf3Position;
public:
	CVertex() = default;
	CVertex(XMFLOAT3 position) :m_xmf3Position(position) {}
};
class CModelVertex : public CVertex {
public:
	XMFLOAT3	m_xmf3Normal;
	XMFLOAT2	m_xmf2TexCoord;
public:
	CModelVertex() = default;
	CModelVertex(XMFLOAT3 position,
				 XMFLOAT3 normal,
				 XMFLOAT2 texcoord) 
				 : CVertex(position)
				 , m_xmf3Normal(normal)
				 , m_xmf2TexCoord(texcoord) {}
};
class CAnimatedVertex : public CVertex {
public:
	XMFLOAT3	m_xmf3Normal;
	XMFLOAT3	m_xmf3Tangent;
	XMFLOAT2	m_xmf2TexCoord;
	XMFLOAT4	m_xmf4BoneWeight;
	XMINT4		m_xmi4BoneIdx;
public:
	CAnimatedVertex() = default;
	CAnimatedVertex(XMFLOAT3 position,
					XMFLOAT3 normal,
					XMFLOAT3 tangent,
					XMFLOAT2 texcoord,
					XMFLOAT4 BoneWeight,
					XMINT4 BoneIdx)
					: CVertex(position)
					, m_xmf3Normal(normal)
					, m_xmf3Tangent(tangent)
					, m_xmf2TexCoord(texcoord)
					, m_xmf4BoneWeight(BoneWeight)
					, m_xmi4BoneIdx(BoneIdx) {}
};
class CUIVertex : public CVertex {
public:
	XMFLOAT2	m_xmf2TexCoord;
public:
	CUIVertex() = default;
	CUIVertex(XMFLOAT3 position,
			  XMFLOAT2 texcoord)
			  : CVertex(position)
			  , m_xmf2TexCoord(texcoord) {}
};


class CMesh {
protected:
	ID3D12Resource					*m_pd3dVertexBuffer = NULL;
	ID3D12Resource					*m_pd3dVertexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nVertices = 0;
	UINT							m_nStride	= 0;
public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices);
	virtual ~CMesh();

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateVertexBuffer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData);
};
class CModelMesh : public CMesh
{
public:
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);

public:
	CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	CModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ImportMeshData& m, float scale = 1.0f);
};
class CAnimatedModelMesh : public CMesh {
public:
	CAnimatedModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ImportMeshData& m);
};
class CUIMesh : public CMesh {
public:
	CUIMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float width = 0, float height = 0);
};
//class CTestMesh : public CModelMesh
//{
//private:
//	//정점을 픽킹을 위하여 저장한다(정점 버퍼를 Map()하여 읽지 않아도 되도록).
//	CModelVertex * m_pVertices = NULL;
//	//메쉬의 인덱스를 저장한다(인덱스 버퍼를 Map()하여 읽지 않아도 되도록).
//	UINT	*m_pnIndices = NULL;
//public:
//	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
//	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float size);
//	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 extents);
//	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 position, XMFLOAT3 extents);
//	~CTestMesh();
//};
