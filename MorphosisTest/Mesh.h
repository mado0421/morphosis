#pragma once
#include "stdafx.h"
#include "Material.h"

class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};
class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};
class CIlluminatedVertex : public CVertex
{
protected:
	XMFLOAT3						m_xmf3Normal;

public:
	CIlluminatedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf3Normal = xmf3Normal; }
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal; }
	~CIlluminatedVertex() { }
};
class CIlluminatedTexturedVertex : public CIlluminatedVertex
{
protected:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CIlluminatedTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf3Normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; }
	CIlluminatedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; }
	~CIlluminatedTexturedVertex() { }
};
class CAnimVertex {
public:
	CAnimVertex(
		XMFLOAT3	pos = XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT2	uv = XMFLOAT2(0.0f, 0.0f),
		XMFLOAT4	weights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		XMINT4		boneIdx = XMINT4(0, 0, 0, 0)
	)
	{
		this->pos = pos;
		this->uv = uv;
		this->weights = weights;
		this->boneIdx = boneIdx;
	}
	CAnimVertex(
		float x, float y, float z,
		XMFLOAT2	uv = XMFLOAT2(0.0f, 0.0f),
		XMFLOAT4	weights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		XMINT4		boneIdx = XMINT4(0, 0, 0, 0)
	)
	{
		this->pos = XMFLOAT3(x, y, z);
		this->uv = uv;
		this->weights = weights;
		this->boneIdx = boneIdx;
	}
	~CAnimVertex() {}

	void Init(XMFLOAT3 p, XMFLOAT2 u, XMFLOAT4 w, XMINT4 b) {
		pos = p;
		uv = u;
		weights = w;
		boneIdx = b;
	}
private:
	XMFLOAT3	pos;
	XMFLOAT2	uv;
	XMFLOAT4	weights;
	XMINT4		boneIdx;
};

class AnimationMesh;
struct ImportMeshData;
class CAnimationController;

class CMesh
{
protected:
	ID3D12Resource					*m_pd3dVertexBuffer			= NULL;
	ID3D12Resource					*m_pd3dVertexUploadBuffer	= NULL;

	ID3D12Resource					*m_pd3dIndexBuffer			= NULL;
	ID3D12Resource					*m_pd3dIndexUploadBuffer	= NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology		= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot			= 0;
	UINT							m_nVertices		= 0;
	UINT							m_nStride		= 0;
	UINT							m_nOffset		= 0;

	UINT							m_nIndices		= 0;
	UINT							m_nStartIndex	= 0;
	int								m_nBaseVertex	= 0;

public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices);
	virtual ~CMesh();

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateVertexBuffer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData);
	virtual void CreateIndexBuffer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData);

	virtual void SetAnimatedMatrix(CAnimationController* a);
	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);
};
class CTexturedMesh : public CMesh
{
public:
	CTexturedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CTexturedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CTexturedMesh();
};
class CIlluminatedMesh : public CMesh
{
public:
	CIlluminatedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CIlluminatedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, UINT nIndices, UINT *pnIndices);
	virtual ~CIlluminatedMesh();

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);
};
class CIlluminatedTexturedMesh : public CIlluminatedMesh
{
public:
	CIlluminatedTexturedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CIlluminatedTexturedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CIlluminatedTexturedMesh();
};
class CAnimatedMesh : public CMesh {
public:
	CAnimatedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nVertices, XMFLOAT3 *pxmf3Positions, UINT nIndices, UINT *pnIndices) : CMesh(pd3dDevice, pd3dCommandList)
	{
		CreateConstantBufferResource(pd3dDevice, pd3dCommandList);
		for (int i = 0; i < g_NumAnimationBone; ++i) m_a[i] = XMMatrixIdentity();
		UINT nStride = sizeof(CAnimVertex);
		this->nVertices = nVertices;

		CAnimVertex *pVertices = new CAnimVertex[nVertices];
		for (UINT i = 0; i < nVertices; i++) pVertices[i] = CAnimVertex(pxmf3Positions[i]);

		pVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
			pVertices, nStride * nVertices,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			&pVertexUploadBuffer);

		vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = nStride;
		vertexBufferView.SizeInBytes = nStride * nVertices;

		if (nIndices > 0)
		{
			this->nIndices = nIndices;

			pIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * nIndices,
				D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
				&pIndexUploadBuffer);

			indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
			indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			indexBufferView.SizeInBytes = sizeof(UINT) * nIndices;
		}
	}
	~CAnimatedMesh()
	{
		if (pVertexBuffer)			pVertexBuffer->Release();
		if (pIndexBuffer)			pIndexBuffer->Release();
		if (pVertexUploadBuffer)	pVertexUploadBuffer->Release();
		if (pIndexUploadBuffer)		pIndexUploadBuffer->Release();
	}

	CAnimatedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ImportMeshData& m);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList)
	{
		UpdateConstantBuffer(pd3dCommandList);

		pd3dCommandList->IASetPrimitiveTopology(primitiveTopology);
		pd3dCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		if (pIndexBuffer)
		{
			pd3dCommandList->IASetIndexBuffer(&indexBufferView);
			pd3dCommandList->DrawIndexedInstanced(nIndices, 1, 0, 0, 0);
		}
		else
		{
			pd3dCommandList->DrawInstanced(nVertices, 1, 0, 0);
		}
	}

	virtual void SetAnimatedMatrix(CAnimationController* a);
	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);
private:
	ID3D12Resource					*pVertexBuffer = NULL;
	ID3D12Resource					*pVertexUploadBuffer = NULL;

	ID3D12Resource					*pIndexBuffer = NULL;
	ID3D12Resource					*pIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			indexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT nIndices = 0;
	UINT nVertices = 0;

	ID3D12Resource					*m_pd3dcbAnimation = NULL;
	XMMATRIX						*m_pcbxmAnimation = NULL;
	XMMATRIX						m_a[g_NumAnimationBone];
};
class CTestMesh : public CIlluminatedTexturedMesh
{
private:
	//정점을 픽킹을 위하여 저장한다(정점 버퍼를 Map()하여 읽지 않아도 되도록).
	CIlluminatedTexturedVertex * m_pVertices = NULL;
	//메쉬의 인덱스를 저장한다(인덱스 버퍼를 Map()하여 읽지 않아도 되도록).
	UINT	*m_pnIndices = NULL;
public:
	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float size);
	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 extents);
	CTestMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 position, XMFLOAT3 extents);
	~CTestMesh();

public:
//	virtual int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float *pfNearHitDistance);

};

using BasicMesh = CMesh;
using UIMesh = CTexturedMesh;
using TerrainMesh = CIlluminatedTexturedMesh;
using DebugMesh = CTestMesh;
//using AnimatedMesh = AnimatedMesh;