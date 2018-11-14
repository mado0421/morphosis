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

	m_pIAVertex = nullptr;
	m_nIAVertex = 0;
}

FBXDataManager::~FBXDataManager()
{
}

bool FBXDataManager::FileRead(std::string fileName)
{
	m_fileName = fileName;
	return FileRead();
}

bool FBXDataManager::FileRead(const char * fileName)
{
	m_fileName = fileName;
	return FileRead();
}

bool FBXDataManager::FileRead()
{
	std::ifstream fs;

	fs.open(m_fileName, std::ios::binary);
	if (fs.fail())return false;

	/*	Read Material	*/
	fs.read((char*)&m_nMaterial, sizeof(int));
	if (m_nMaterial > 0){
		m_pMaterial = new cv::Material[m_nMaterial];
		fs.read((char*)m_pMaterial, sizeof(cv::Material)*m_nMaterial);
	}
	else{
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

void FBXDataManager::Destroy()
{
	if (m_pMaterial)delete[] m_pMaterial;
	if (m_pName)delete[] m_pName;
	if (m_pVertex)delete[] m_pVertex;
	if (m_pUV)delete[] m_pUV;
	if (m_pIAVertex)delete[] m_pIAVertex;
}
