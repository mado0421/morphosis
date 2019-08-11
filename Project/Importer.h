#pragma once

struct CtrlPointData {
	XMFLOAT3	m_xmf3Position;
	XMINT4		m_xmi4BoneIdx;
	XMFLOAT4	m_xmf4BoneWeight;
};

struct VertexData {
	int			ctrlPointIdx;
	XMFLOAT3	m_xmf3Normal;
	XMFLOAT3	m_xmf3BiNormal;
	XMFLOAT3	m_xmf3Tangent;
	XMFLOAT2	m_xmf2TexCoord;
};

struct MeshData {
	string					meshName;
	vector<CtrlPointData>	vecCtrlPoint;
	vector<VertexData>		vecVertex;
};

class Importer {
public:
	Importer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
public:
	void ImportMesh(const char* fileName, const char* meshName, MeshType type);
	void ImportTexture(const wchar_t* fileName, const char* textureName);
	void ImportAnimClip(const char* fileName, const char* animClipName, bool IsLoop);
	void ImportLevelData(const char* fileName);
private:
	ID3D12Device*				m_pd3dDevice		= NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList	= NULL;
};