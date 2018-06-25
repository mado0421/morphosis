#pragma once
#ifndef _FSTREAM_
#include<fstream>
#endif // !_FSTREAM_
#include"ConverterNameSpace.h"

#include<Windows.h>
class FBXDataManager
{
public:
	FBXDataManager();
	virtual ~FBXDataManager();

	enum class FBX_DATA { Mesh, FBX, Anim };

	bool FileRead(FBX_DATA format, std::string fileName);
	bool FileRead(FBX_DATA format, const char* fileName);
	bool FileRead(FBX_DATA format);

	bool MeshFileRead(std::string fileName);
	bool MeshFileRead(const char* fileName);
	bool MeshFileRead();

	bool FBXFileRead(std::string fileName);
	bool FBXFileRead(const char* fileName);
	bool FBXFileRead();

	bool AnimFileRead(std::string fileName);
	bool AnimFileRead(const char* fileName);
	bool AnimFileRead();

	void SetTextureName(const char* name) {
		if (m_pName != nullptr)strcpy(m_pName->string, name);}
	void SetName(std::string name) { m_fileName = name; }
	std::string GetName()const { return m_fileName; }
	
	inline bool FileErrorShutDown(std::ifstream& fs) {
		fs.close();
		Destroy();
		return false;
	};

	void Destroy();

public:
	std::string m_fileName;

	cv::Material * m_pMaterial;
	int m_nMaterial;

	cv::Name* m_pName;
	int m_nName;

	cv::Vertex* m_pVertex;
	int m_nVertex;

	cv::UV* m_pUV;
	int m_nUV;

	cv::IAVertex* m_pIAVertex;
	int m_nIAVertex;

	cv::Bone* m_pBone;
	int m_nBone;

	cv::CurveNode* m_pCurveNode;
	int m_nCurveNode;
};



