#include "stdafx.h"
#include "FBXData.h"


bool CFBXData::ReadFile(const char * fileName)
{
	std::ifstream fs;

	fs.open(fileName, std::ios::binary);
	if (fs.fail())return false;

	/*	Read Material	*/
	fs.read((char*)&m_nMaterial, sizeof(int));
	if (m_nMaterial > 0) {
		m_pMaterial = new FBX::Material[m_nMaterial];
		fs.read((char*)m_pMaterial, sizeof(FBX::Material)*m_nMaterial);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Texture File Name	*/
	fs.read((char*)&m_nName, sizeof(int));
	if (m_nName > 0) {
		m_pName = new FBX::Name[m_nName];
		fs.read((char*)m_pName, sizeof(FBX::Name)*m_nName);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Vertex	*/
	fs.read((char*)&m_nVertex, sizeof(int));
	if (m_nVertex > 0) {
		m_pVertex = new FBX::Vertex[m_nVertex];
		fs.read((char*)m_pVertex, sizeof(FBX::Vertex)*m_nVertex);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*
	여기서 버텍스 다 회전시켜줘야 함
	*/
	XMFLOAT3 xmf3Up = {1, 0, 0};
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(-90));
	XMFLOAT3 temp;
	float MaxZ = 0;
	for (int i = 0; i < m_nVertex; ++i) {
		if (m_pVertex[i].pos.z > MaxZ) MaxZ = m_pVertex[i].pos.z;
	}

	MaxZ /= 2.0;

	for (int i = 0; i < m_nVertex; ++i) {
		temp = { m_pVertex[i].pos.x, m_pVertex[i].pos.y ,m_pVertex[i].pos.z };
		temp = Vector3::TransformNormal(temp, xmmtxRotate);
		m_pVertex[i].pos.x = temp.x;
		m_pVertex[i].pos.y = temp.y - MaxZ;
		m_pVertex[i].pos.z = temp.z;
	}


	/*	Read UV	*/
	fs.read((char*)&m_nUV, sizeof(int));
	if (m_nUV > 0) {
		m_pUV = new FBX::UV[m_nUV];
		fs.read((char*)m_pUV, sizeof(FBX::UV)*m_nUV);
	}
	else {
		return FileErrorShutDown(fs);
	}

	/*	Read Mesh	*/
	fs.read((char*)&m_nIAVertex, sizeof(int));

	//	Vertex Index
	if (m_nIAVertex > 0) {
		m_pIAVertex = new FBX::IAVertex[m_nIAVertex];
		fs.read((char*)m_pIAVertex, sizeof(FBX::IAVertex)*m_nIAVertex);
	}
	else {
		return FileErrorShutDown(fs);
	}

	fs.close();
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
