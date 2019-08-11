//#pragma once
//#include "stdafx.h"
//
//struct LEVELDATA_DESC;
//
//struct CtrlPointData {
//	XMFLOAT3	xmf3Position;
//	XMINT4		xmi4BoneIdx;
//	XMFLOAT4	xmf4BoneWeight;
//};
//struct VertexData {
//	int			ctrlPointIdx;
//	XMFLOAT3	xmf3Normal;
//	XMFLOAT3	xmf3BiNormal;
//	XMFLOAT3	xmf3Tangent;
//	XMFLOAT2	xmf2UV;
//};
//struct MeshData {
//	~MeshData() {
//		if (m_CtrlPointList) {
//			delete[] m_CtrlPointList; m_CtrlPointList = NULL;
//		}
//		if (m_VertexList) {
//			delete[] m_VertexList; m_VertexList = NULL;
//		}
//	}
//
//	std::string		m_MeshName;
//	CtrlPointData*	m_CtrlPointList;
//	VertexData*		m_VertexList;
//
//	int			m_nCtrlPointList;
//	int			m_nVertexList;
//};
//struct ModelData {
//public:
//	string GetModelName()			{ return m_ModelName; }
//	int GetNumOfMesh()				{ return m_nMeshList; }
//	MeshData& GetMeshData(int i)	{ return m_MeshList[i]; }
//
//	string		m_ModelName;
//
//	MeshData	*m_MeshList;
//	int			m_nMeshList;
//};
//
//class CImporter {
//public:
//	CImporter() = delete;
//	CImporter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
//		: m_pd3dDevice(pd3dDevice)
//		, m_pd3dCommandList(pd3dCommandList)
//	{}
//
//	void ImportMesh(const char* fileName, const char* meshName, MeshType type);
//	void ImportTexture(const wchar_t* fileName, const char* textureName = "");
//	void ImportAnimClip(const char* fileName, const char* animClipName, bool IsLoop);
//	void ImportLevel(const char* fileName, LEVELDATA_DESC& desc);
//
//private:
//	ID3D12Device*				m_pd3dDevice		= NULL;
//	ID3D12GraphicsCommandList*	m_pd3dCommandList	= NULL;
//};