#include "stdafx.h"
#include "FBXData.h"


bool CFBXData::ReadFile(const char * fileName)
{

	FILE * fp;

	fp = fopen(fileName, "rb");
	if (NULL == fp) return false;

	/*	Read Material	*/
	fread(&m_nMaterial, sizeof(int), 1, fp);
	fread(&m_pMaterial, sizeof(FBX::Material), m_nMaterial, fp);

	/*	Read Texture File Name	*/
	fread(&m_nName, sizeof(int), 1, fp);
	fread(&m_pName, sizeof(FBX::Name), m_nName, fp);

	/*	Read Vertex	*/
	fread(&m_nVertex, sizeof(int), 1, fp);
	fread(&m_pVertex, sizeof(FBX::Vertex), m_nVertex, fp);

	/*	Read UV	*/
	fread(&m_nUV, sizeof(int), 1, fp);
	fread(&m_pUV, sizeof(FBX::UV), m_nUV, fp);

	/*	Read Mesh	*/
	fread(&m_nIAVertex, sizeof(int), 1, fp);

	//	Vertex Index
	fread(&m_pIAVertex, sizeof(FBX::IAVertex), m_nIAVertex, fp);

	fclose(fp);
	return true;
}

void CFBXData::Destroy()
{
	if (m_pMaterial)delete[] m_pMaterial;
	if (m_pName)delete[] m_pName;
	if (m_pVertex)delete[] m_pVertex;
	if (m_pUV)delete[] m_pUV;
	if (m_pIAVertex)delete[] m_pIAVertex;
}

CFBXData::CFBXData()
{
	//m_fileName;

	m_pMaterial = nullptr;
	m_nMaterial = 0;

	m_pName = nullptr;
	m_nName = 0;

	m_pVertex = nullptr;
	m_nVertex = 0;

	m_pUV = nullptr;
	m_nUV = 0;

	m_pIAVertex = nullptr;
	m_nIAVertex = 0;
}


CFBXData::~CFBXData()
{
}
