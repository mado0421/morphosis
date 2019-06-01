#include <DirectXMath.h>
#include <d3d12.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Print(const XMFLOAT4X4& m) {
	std::cout << std::fixed << std::setprecision(5) <<
		std::setw(9) << m._11 << " " << std::setw(9) << m._12 << " " << std::setw(9) << m._13 << " " << std::setw(9) << m._14 << "\n" <<
		std::setw(9) << m._21 << " " << std::setw(9) << m._22 << " " << std::setw(9) << m._23 << " " << std::setw(9) << m._24 << "\n" <<
		std::setw(9) << m._31 << " " << std::setw(9) << m._32 << " " << std::setw(9) << m._33 << " " << std::setw(9) << m._34 << "\n" <<
		std::setw(9) << m._41 << " " << std::setw(9) << m._42 << " " << std::setw(9) << m._43 << " " << std::setw(9) << m._44 << "\n\n";
}

struct AnimBone {
	std::string	m_Name;
	XMFLOAT4X4	m_GlobalTransform;
	XMFLOAT4X4* m_pToRootTransforms;
};

struct AnimData {
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
		m_BoneList = new AnimBone[m_nBoneList];

		for (int i = 0; i < m_nBoneList; ++i) {
			char name[32];
			in.read((char*)name, sizeof(name));
			m_BoneList[i].m_Name = name;

			in.read((char*)&m_BoneList[i].m_GlobalTransform, sizeof(XMFLOAT4X4));
		}

		for (int i = 0; i < m_nBoneList; ++i) {
			m_BoneList[i].m_pToRootTransforms = new XMFLOAT4X4[m_nKeyTime];
			for (int j = 0; j < m_nKeyTime; ++j) {
				in.read((char*)&m_BoneList[i].m_pToRootTransforms[j], sizeof(XMFLOAT4X4));
			}
		}

		in.close();
	}
	void Display() {
		std::cout << m_AnimName << "\n\n";

		for (int i = 0; i < m_nBoneList; ++i) {
			std::cout << m_BoneList[i].m_Name << "\n\n";
		}
	}

private:
	std::string m_AnimName;
	int			m_nBoneList;
	int			m_nKeyTime;

	AnimBone*	m_BoneList;
	double*		m_KeyTime;
};

struct CtrlPoint {
	XMFLOAT3	xmf3Position;
	XMINT4		xmi4BoneIdx;
	XMFLOAT4	xmf4BoneWeight;

	CtrlPoint()
		: xmi4BoneIdx(0, 0, 0, 0)
		, xmf4BoneWeight(0, 0, 0, 0)
	{}
};
struct Vertex {
	int			ctrlPointIdx;
	XMFLOAT3	xmf3Normal;
	XMFLOAT3	xmf3BiNormal;
	XMFLOAT3	xmf3Tangent;
	XMFLOAT2	xmf2UV;
};

struct MeshData {
	std::string m_MeshName;
	CtrlPoint*	m_CtrlPointList;
	Vertex*		m_VertexList;

	int			m_nCtrlPointList;
	int			m_nVertexList;
};

struct ModelData {
public:
	void ImportFile(const char* fileName) {
		std::ifstream in;

		in.open(fileName, std::ios::in | std::ios::binary);
		char ModelName[32];
		in.read((char*)&ModelName, sizeof(ModelName));
		m_ModelName = ModelName;

		in.read((char*)&m_nMeshList, sizeof(int));
		m_MeshList = new MeshData[m_nMeshList];

		for (int i = 0; i < m_nMeshList; ++i) {
			char name[32];
			in.read((char*)name, sizeof(name));
			m_MeshList[i].m_MeshName = name;

			
			in.read((char*)&m_MeshList[i].m_nCtrlPointList, sizeof(int));
			m_MeshList[i].m_CtrlPointList = new CtrlPoint[m_MeshList[i].m_nCtrlPointList];

			in.read((char*)m_MeshList[i].m_CtrlPointList, sizeof(CtrlPoint) * m_MeshList[i].m_nCtrlPointList);

			in.read((char*)&m_MeshList[i].m_nVertexList, sizeof(int));
			m_MeshList[i].m_VertexList = new Vertex[m_MeshList[i].m_nVertexList];

			in.read((char*)m_MeshList[i].m_VertexList, sizeof(Vertex) * m_MeshList[i].m_nVertexList);
		}
	}
	void Display() {
		std::cout << m_ModelName << "\n";

		for (int i = 0; i < m_nMeshList; ++i) {
			std::cout << m_MeshList[i].m_MeshName << "\n";
		}
	}


private:
	std::string m_ModelName;

	MeshData*	m_MeshList;
	int			m_nMeshList;
};

//test_0530_016_Character_mesh.dat

int main()
{
	AnimData a;

	a.ImportFile("test_0530_016_Character_anim.dat");
	a.Display();

	ModelData aa;

	aa.ImportFile("test_0530_016_Character_mesh.dat");
	aa.Display();

	return 0;
}
