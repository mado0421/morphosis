#include "stdafx.h"

#include "FBXDataManager.h"


FBXDataManager::FBXDataManager()
{
	m_fileName;

	m_pMaterial		= nullptr;
	m_nMaterial		= 0;

	m_pName			= nullptr;
	m_nName			= 0;

	m_pVertex		= nullptr;
	m_nVertex		= 0;
	
	m_pUV			= nullptr;
	m_nUV			= 0;

	m_pIAVertex		= nullptr;
	m_nIAVertex		= 0;

	m_pBone			= nullptr;
	m_nBone			= 0;

	m_pCurveNode	= nullptr;
	m_nCurveNode	= 0;
}

FBXDataManager::~FBXDataManager()
{
}

//	MeshFile만 읽음
bool FBXDataManager::MeshFileRead()
{
	std::ifstream fs;

	fs.open(m_fileName, std::ios::binary);
	if (fs.fail())return false;

	/*	Read Material	*/
	fs.read((char*)&m_nMaterial, sizeof(int));
	if (m_nMaterial > 0) {
		m_pMaterial = new cv::Material[m_nMaterial];
		fs.read((char*)m_pMaterial, sizeof(cv::Material)*m_nMaterial);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Texture File Name	*/
	fs.read((char*)&m_nName, sizeof(int));
	if (m_nName > 0) {
		m_pName = new cv::Name[m_nName];
		fs.read((char*)m_pName, sizeof(cv::Name)*m_nName);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Vertex	*/
	fs.read((char*)&m_nVertex, sizeof(int));
	if (m_nVertex > 0) {
		m_pVertex = new cv::Vertex[m_nVertex];
		fs.read((char*)m_pVertex, sizeof(cv::Vertex)*m_nVertex);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read UV	*/
	fs.read((char*)&m_nUV, sizeof(int));
	if (m_nUV > 0) {
		m_pUV = new cv::UV[m_nUV];
		fs.read((char*)m_pUV, sizeof(cv::UV)*m_nUV);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Mesh	*/
	fs.read((char*)&m_nIAVertex, sizeof(int));
	//	Vertex Index
	if (m_nIAVertex > 0) {
		m_pIAVertex = new cv::IAVertex[m_nIAVertex];
		fs.read((char*)m_pIAVertex, sizeof(cv::IAVertex)*m_nIAVertex);
	}
	else {
		return FileErrorShutDown(fs);
	}

	fs.close();
	return true;
}

//	Mesh와 Animation 전부 읽음
bool FBXDataManager::FBXFileRead()
{
	std::ifstream fs;

	fs.open(m_fileName, std::ios::binary);
	if (fs.fail())return false;

	/*	Read Material	*/
	fs.read((char*)&m_nMaterial, sizeof(int));
	if (m_nMaterial > 0) {
		m_pMaterial = new cv::Material[m_nMaterial];
		fs.read((char*)m_pMaterial, sizeof(cv::Material)*m_nMaterial);
	}
	else {
		//return FileErrorShutDown(fs);
	}

	/*	Read Texture File Name	*/
	fs.read((char*)&m_nName, sizeof(int));
	if (m_nName > 0) {
		m_pName = new cv::Name[m_nName];
		fs.read((char*)m_pName, sizeof(cv::Name)*m_nName);
	}
	else {
		//return FileErrorShutDown(fs);
	}

	/*	Read Vertex	*/
	fs.read((char*)&m_nVertex, sizeof(int));
	if (m_nVertex > 0) {
		m_pVertex = new cv::Vertex[m_nVertex];
		fs.read((char*)m_pVertex, sizeof(cv::Vertex)*m_nVertex);
	}
	else {
		//return FileErrorShutDown(fs);
	}

	/*	Read UV	*/
	fs.read((char*)&m_nUV, sizeof(int));
	if (m_nUV > 0) {
		m_pUV = new cv::UV[m_nUV];
		fs.read((char*)m_pUV, sizeof(cv::UV)*m_nUV);
	}
	else {
		//return FileErrorShutDown(fs);
	}

	/*	Read Mesh	*/
	fs.read((char*)&m_nIAVertex, sizeof(int));
	//	Vertex Index
	if (m_nIAVertex > 0) {
		m_pIAVertex = new cv::IAVertex[m_nIAVertex];
		fs.read((char*)m_pIAVertex, sizeof(cv::IAVertex)*m_nIAVertex);
	}
	else {
		//return FileErrorShutDown(fs);
	}

	/*	Read Bone	*/
	fs.read((char*)&m_nBone, sizeof(int));
	if (m_nBone > 0) {
		m_pBone = new cv::CBone[m_nBone];
		fs.read((char*)m_pBone, sizeof(cv::CBone)*m_nBone);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read CurveNode	*/
	fs.read((char*)&m_nCurveNode, sizeof(int));
	if (m_nCurveNode > 0) {
		m_pCurveNode = new cv::CurveNode[m_nCurveNode];
		ZeroMemory(m_pCurveNode, sizeof(cv::CurveNode)*m_nCurveNode);
		for (int i = 0; i < m_nCurveNode; ++i)
		{
			//	Read Connected Bone Index
			fs.read((char*)&m_pCurveNode[i].bone_index, sizeof(int));

			int num;

			fs.read((char*)&num, sizeof(int));
			m_pCurveNode[i].nTranslation = num;
			if (num > 0)
			{
				m_pCurveNode[i].pTranslation = new cv::Keyframe[num];
				fs.read((char*)m_pCurveNode[i].pTranslation, sizeof(cv::Keyframe)*num);
			}

			fs.read((char*)&num, sizeof(int));
			m_pCurveNode[i].nRotation = num;
			if (num > 0)
			{
				m_pCurveNode[i].pRotation = new cv::Keyframe[num];
				fs.read((char*)m_pCurveNode[i].pRotation, sizeof(cv::Keyframe)*num);
			}

			fs.read((char*)&num, sizeof(int));
			m_pCurveNode[i].nScale = num;
			if (num > 0)
			{
				m_pCurveNode[i].pScale = new cv::Keyframe[num];
				fs.read((char*)m_pCurveNode[i].pScale, sizeof(cv::Keyframe)*num);
			}
		}
	}
	else {
		return FileErrorShutDown(fs);
	}

	fs.close();
	return true;
}

//	아직 추가 안함
bool FBXDataManager::AnimFileRead()
{
	return false;
}

//	format에 따라 Read 함수 호출해줌
bool FBXDataManager::FileRead(FBX_DATA format)
{
	switch (format)
	{
	case FBX_DATA::Mesh:
		return MeshFileRead();
		break;
	case FBX_DATA::FBX:
		return FBXFileRead();
		break;
	case FBX_DATA::Anim:
		return AnimFileRead();
		break;
	default:
		break;
	}
}

void FBXDataManager::Destroy()
{
	if (m_pMaterial)delete[] m_pMaterial;
	if (m_pName)delete[] m_pName;
	if (m_pVertex)delete[] m_pVertex;
	if (m_pUV)delete[] m_pUV;
	if (m_pIAVertex)delete[] m_pIAVertex;
}

//================================================================================//
/* 여러 인자로 받기 위한 용도 */
bool FBXDataManager::FileRead(FBX_DATA format, std::string fileName)
{
	m_fileName = fileName;
	return FileRead(format);
}

bool FBXDataManager::FileRead(FBX_DATA format, const char * fileName)
{
	m_fileName = fileName;
	return FileRead(format);
}

bool FBXDataManager::MeshFileRead(std::string fileName)
{
	m_fileName = fileName;
	return MeshFileRead();
}

bool FBXDataManager::MeshFileRead(const char * fileName)
{
	m_fileName = fileName;
	return MeshFileRead();
}

bool FBXDataManager::FBXFileRead(std::string fileName)
{
	m_fileName = fileName;
	return FBXFileRead();
}

bool FBXDataManager::FBXFileRead(const char * fileName)
{
	m_fileName = fileName;
	return FBXFileRead();
}

bool FBXDataManager::AnimFileRead(std::string fileName)
{
	m_fileName = fileName;
	return AnimFileRead();
}

bool FBXDataManager::AnimFileRead(const char * fileName)
{
	m_fileName = fileName;
	return AnimFileRead();
}
