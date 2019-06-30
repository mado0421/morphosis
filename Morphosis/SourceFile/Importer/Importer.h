#pragma once
#include "stdafx.h"

struct ImportBone {
	std::string	m_Name;
	XMFLOAT4X4	m_GlobalTransform = Matrix4x4::Identity();
	XMFLOAT4X4* m_pToRootTransforms = NULL;
	int			m_nKeyframe = 0;

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
struct AnimationClip {
public:
	AnimationClip() = default;
	AnimationClip(const AnimationClip& a) {
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
	int			m_nBoneList = 0;
	int			m_nKeyTime = 0;

	ImportBone*	m_BoneList = NULL;
	double*		m_KeyTime = 0;
};
struct ImportCtrlPoint {
	XMFLOAT3	xmf3Position = XMFLOAT3(0,0,0);
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

		ChangeZSign();
	}
	void Display() {
		std::cout << m_ModelName.c_str() << "\n";

		for (int i = 0; i < m_nMeshList; ++i) {
			std::cout << m_MeshList[i].m_MeshName.c_str() << "\n";
		}
	}
	void ChangeZSign() {
		for (int i = 0; i < m_nMeshList; ++i) {
			for (int j = 0; j < m_MeshList[i].m_nCtrlPointList; ++j)
				m_MeshList[i].m_CtrlPointList[j].xmf3Position.z *= -1;
		}
	}

public:
	string GetModelName() { return m_ModelName; }
	int GetNumOfMesh() { return m_nMeshList; }
	ImportMeshData& GetMeshData(int i) { return m_MeshList[i]; }

private:
	std::string m_ModelName;

	ImportMeshData*	m_MeshList = NULL;
	int			m_nMeshList;
};

class CObject;
class CTexture;

class CImporter {

	/*********************************************************************
	파일 이름만 넘겨주면 안에서 "_anim.dat"이랑 "_mesh.dat" 붙여서 함.

	ex) test_0530_016_Character_anim.dat
	ex) test_0530_016_Character_mesh.dat
	*********************************************************************/
public:
	/*********************************************************************
	2019-06-12
	파일 하나에 여러 모델이 있을 수 있다. 따라서 모델을 반환하지 말고 파라미터로
	오브젝트의 포인터를 받아와서 그 자리에서 직접 추가하도록 한다.
	*********************************************************************/
	CImporter() = delete;
	CImporter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
		: m_pd3dDevice(pd3dDevice)
		, m_pd3dCommandList(pd3dCommandList)
	{

	}

	//void ImportAnimClip(const char* fileName, CObject* obj);
	//void ImportModel(const char* fileName, CObject* obj);
	void ImportModel(const char* fileName, CTexture* texture ,CObject* obj);
	void ImportAnimClip(const char* fileName, CObject* obj);
	
private:
	ID3D12Device*				m_pd3dDevice		= NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList	= NULL;
};