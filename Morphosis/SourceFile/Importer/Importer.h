#pragma once
#include "stdafx.h"

struct ImportBone {
	std::string	m_Name;
	XMFLOAT4X4	m_GlobalTransform;
	XMFLOAT4X4* m_pToRootTransforms;
	int			m_nKeyframe;

	ImportBone& operator=(const ImportBone& b) {
		m_Name = b.m_Name;
		m_GlobalTransform = b.m_GlobalTransform;
		m_nKeyframe = b.m_nKeyframe;

		m_pToRootTransforms = new XMFLOAT4X4[m_nKeyframe];

		for (int i = 0; i < m_nKeyframe; ++i) {
			m_pToRootTransforms[i] = b.m_pToRootTransforms[i];
		}

		return *this;
	}
};

struct ImportAnimData {
public:
	ImportAnimData() = default;
	ImportAnimData(const ImportAnimData& a) {
		m_AnimName = a.m_AnimName;
		m_nBoneList = a.m_nBoneList;
		m_nKeyTime = a.m_nKeyTime;

		m_BoneList = new ImportBone[m_nBoneList];
		for (int i = 0; i < m_nBoneList; ++i) {
			m_BoneList[i] = a.m_BoneList[i];
		}

		m_KeyTime = new double[m_nKeyTime];
		for (int i = 0; i < m_nKeyTime; ++i) {
			m_KeyTime[i] = a.m_KeyTime[i];
		}
	}

public:
	void ImportFile(const char* fileName) {
		std::ifstream in;

		in.open(fileName, std::ios::in | std::ios::binary);
		char AnimName[32];
		in.read((char*)AnimName, sizeof(AnimName));
		m_AnimName = AnimName;

		m_nKeyTime = 0;
		in.read((char*)&m_nKeyTime, sizeof(int));
		m_KeyTime = new double[m_nKeyTime];
		in.read((char*)m_KeyTime, sizeof(double) * m_nKeyTime);

		in.read((char*)&m_nBoneList, sizeof(int));
		m_BoneList = new ImportBone[m_nBoneList];

		for (int i = 0; i < m_nBoneList; ++i) {
			char name[32];
			in.read((char*)name, sizeof(name));
			m_BoneList[i].m_Name = name;

			in.read((char*)&m_BoneList[i].m_GlobalTransform, sizeof(XMFLOAT4X4));
		}

		for (int i = 0; i < m_nBoneList; ++i) {
			m_BoneList[i].m_nKeyframe = m_nKeyTime;
			m_BoneList[i].m_pToRootTransforms = new XMFLOAT4X4[m_nKeyTime];
			for (int j = 0; j < m_nKeyTime; ++j) {
				in.read((char*)&m_BoneList[i].m_pToRootTransforms[j], sizeof(XMFLOAT4X4));
			}
		}

		in.close();
	}
	void Display() {
		std::cout << m_AnimName.c_str() << "\n\n";

		for (int i = 0; i < m_nBoneList; ++i) {
			std::cout << m_BoneList[i].m_Name.c_str() << "\n\n";
		}
	}

public:
	std::string m_AnimName;
	int			m_nBoneList;
	int			m_nKeyTime;

	ImportBone*	m_BoneList;
	double*		m_KeyTime;
};

struct ImportCtrlPoint {
	XMFLOAT3	xmf3Position;
	XMINT4		xmi4BoneIdx;
	XMFLOAT4	xmf4BoneWeight;

	ImportCtrlPoint()
		: xmi4BoneIdx(0, 0, 0, 0)
		, xmf4BoneWeight(0, 0, 0, 0)
	{}
};
struct ImportVertex {
	int			ctrlPointIdx;
	XMFLOAT3	xmf3Normal;
	XMFLOAT3	xmf3BiNormal;
	XMFLOAT3	xmf3Tangent;
	XMFLOAT2	xmf2UV;
};

struct ImportMeshData {
	std::string			m_MeshName;
	ImportCtrlPoint*	m_CtrlPointList;
	ImportVertex*		m_VertexList;

	int			m_nCtrlPointList;
	int			m_nVertexList;
};

struct ImportModelData {
public:
	void ImportFile(const char* fileName) {
		std::ifstream in;

		in.open(fileName, std::ios::in | std::ios::binary);
		char ModelName[32];
		in.read((char*)&ModelName, sizeof(ModelName));
		m_ModelName = ModelName;

		in.read((char*)&m_nMeshList, sizeof(int));
		m_MeshList = new ImportMeshData[m_nMeshList];

		for (int i = 0; i < m_nMeshList; ++i) {
			char name[32];
			in.read((char*)name, sizeof(name));
			m_MeshList[i].m_MeshName = name;


			in.read((char*)&m_MeshList[i].m_nCtrlPointList, sizeof(int));
			m_MeshList[i].m_CtrlPointList = new ImportCtrlPoint[m_MeshList[i].m_nCtrlPointList];

			in.read((char*)m_MeshList[i].m_CtrlPointList, sizeof(ImportCtrlPoint) * m_MeshList[i].m_nCtrlPointList);

			in.read((char*)&m_MeshList[i].m_nVertexList, sizeof(int));
			m_MeshList[i].m_VertexList = new ImportVertex[m_MeshList[i].m_nVertexList];

			in.read((char*)m_MeshList[i].m_VertexList, sizeof(ImportVertex) * m_MeshList[i].m_nVertexList);
		}
	}
	void Display() {
		std::cout << m_ModelName.c_str() << "\n";

		for (int i = 0; i < m_nMeshList; ++i) {
			std::cout << m_MeshList[i].m_MeshName.c_str() << "\n";
		}
	}

public:
	string GetModelName() { return m_ModelName; }
	int GetNumOfMesh() { return m_nMeshList; }
	ImportMeshData& GetMeshData(int i) { return m_MeshList[i]; }

private:
	std::string m_ModelName;

	ImportMeshData*	m_MeshList;
	int			m_nMeshList;
};

class CAnimationPlayerObject;
class CTexture;

class CImporter {

	/*****************************************************
	파일 이름만 넘겨주면 안에서 "_anim.dat"이랑 "_mesh.dat" 붙여서 함.

	ex) test_0530_016_Character_anim.dat
	ex) test_0530_016_Character_mesh.dat
	*****************************************************/
public:
	void ImportFile(
		const char* fileName,
		CTexture* pTexture,
		ID3D12Device *pd3dDevice,
		ID3D12GraphicsCommandList *pd3dCommandList,
		CAnimationPlayerObject& obj);
};