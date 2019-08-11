#pragma once
#include "stdafx.h"

class CObject;
class CTexture;
class CModel;
class CAnimationController;
struct LEVELDATA_DESC;

struct ImportBone {
	std::string	m_Name;
	XMFLOAT4X4	m_GlobalTransform;
	XMFLOAT4X4	*m_pToRootTransforms;
	int			m_nKeyframe;

	ImportBone()
		: m_Name("")
		, m_GlobalTransform(Matrix4x4::Identity())
		, m_pToRootTransforms(NULL)
		, m_nKeyframe(0)
	{
		cout << "ImportBone 생성 - " << sizeof(ImportBone) << "\n";
	}

	~ImportBone() {
		if (NULL != m_pToRootTransforms) delete[] m_pToRootTransforms;
	}

	ImportBone& operator=(const ImportBone& b)
	{
		m_Name				= b.m_Name;
		m_GlobalTransform	= b.m_GlobalTransform;
		m_nKeyframe			= b.m_nKeyframe;

		XMFLOAT4X4* new_pToRootTransforms = new XMFLOAT4X4[m_nKeyframe];
		std::copy_n(b.m_pToRootTransforms, m_nKeyframe, new_pToRootTransforms);

		delete[] m_pToRootTransforms;

		m_pToRootTransforms = new_pToRootTransforms;

		return *this;
	}
};
struct ImportCtrlPoint {
	XMFLOAT3	xmf3Position = XMFLOAT3(0,0,0);
	XMINT4		xmi4BoneIdx;
	XMFLOAT4	xmf4BoneWeight;

	ImportCtrlPoint()
		: xmi4BoneIdx(0, 0, 0, 0)
		, xmf4BoneWeight(0, 0, 0, 0)
	{
		cout << "ImportCtrlPoint 생성 - " << sizeof(ImportCtrlPoint) << "\n";
	}
};
struct ImportVertex {
	ImportVertex() {
		cout << "ImportVertex 생성 - " << sizeof(ImportVertex) << "\n";
	}
	int			ctrlPointIdx;
	XMFLOAT3	xmf3Normal;
	XMFLOAT3	xmf3BiNormal;
	XMFLOAT3	xmf3Tangent;
	XMFLOAT2	xmf2UV;
};
struct ImportMeshData {

	ImportMeshData() {
		cout << "ImportMeshData 생성 - " << sizeof(ImportMeshData) << "\n";

	}
	~ImportMeshData() {
		if (m_CtrlPointList) delete[] m_CtrlPointList;
		if (m_VertexList) delete[] m_VertexList;
	}


	std::string			m_MeshName;
	ImportCtrlPoint*	m_CtrlPointList;
	ImportVertex*		m_VertexList;

	int			m_nCtrlPointList;
	int			m_nVertexList;
};
struct ImportModelData {
public:
	ImportModelData() 
		: m_ModelName("")
		, m_MeshList(NULL)
		, m_nMeshList(0)
	{}

	~ImportModelData() {
		if (NULL != m_MeshList) delete[] m_MeshList;
	}

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

		//ChangeZSign();
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
				//std::swap(m_MeshList[i].m_CtrlPointList[j].xmf3Position.z, m_MeshList[i].m_CtrlPointList[j].xmf3Position.y);
				m_MeshList[i].m_CtrlPointList[j].xmf3Position.z *= -1;
			//for (int j = 0; j < m_MeshList[i].m_nVertexList; ++j) {
			//	std::swap(m_MeshList[i].m_VertexList[j].xmf3Normal.z, m_MeshList[i].m_VertexList[j].xmf3Normal.y);
			//	std::swap(m_MeshList[i].m_VertexList[j].xmf3BiNormal.z, m_MeshList[i].m_VertexList[j].xmf3BiNormal.y);
			//	std::swap(m_MeshList[i].m_VertexList[j].xmf3Tangent.z, m_MeshList[i].m_VertexList[j].xmf3Tangent.y);


			//	//m_MeshList[i].m_VertexList[j].xmf3Normal.z		*= -1;
			//	//m_MeshList[i].m_VertexList[j].xmf3BiNormal.z	*= -1;
			//	//m_MeshList[i].m_VertexList[j].xmf3Tangent.z		*= -1;
			//}
		}
	}

public:
	string GetModelName() { return m_ModelName; }
	int GetNumOfMesh() { return m_nMeshList; }
	ImportMeshData& GetMeshData(int i) { return m_MeshList[i]; }

private:
	std::string		m_ModelName;

	ImportMeshData	*m_MeshList;
	int				m_nMeshList;
};

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

	//CTexture*				GetTextureByName(const char* name);
	//CModel*					GetModelByName(const char* name);
	//CAnimationController*	GetAnimCtrlByName(const char* name);

	void ImportModel(const char* fileName, const char* textureName, ModelType type, const char* modelName = "", float scale = 1.0f);
	void ImportTexture(const wchar_t* fileName, const char* textureName = "");
	void ImportAnimController(const char* animCtrlName = "");
	void ImportAnimClip(const char* fileName, const char* animCtrlName, bool IsLoop, const char* animClipName = "");
	void ImportLevel(const char* fileName, LEVELDATA_DESC& desc);
private:
	ID3D12Device*				m_pd3dDevice		= NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList	= NULL;
};