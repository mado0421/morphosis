#pragma once

struct MeshData;

class Mesh {
public:
	Mesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	~Mesh();

public:
	void Render(ID3D12GraphicsCommandList * pd3dCommandList);

public:
	string m_MeshName;

protected:
	ID3D12Resource				*m_pd3dVertexBuffer			= NULL;
	ID3D12Resource				*m_pd3dVertexUploadBuffer	= NULL;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY	m_d3dPrimitiveTopology		= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT						m_nVertices					= 0;
	UINT						m_nStride					= 0;
};

class ModelMesh : public Mesh {
public:
	ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, MeshData& meshData);
};

class AnimatedModelMesh : public Mesh {
public:
	AnimatedModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, MeshData& meshData);
};

class UIMesh : public Mesh {
public:
	UIMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
};