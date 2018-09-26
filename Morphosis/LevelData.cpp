#include "stdafx.h"
#include "LevelData.h"


CLevelData::CLevelData()
{
}


CLevelData::~CLevelData()
{
}
#define LEVELSIZE 20
bool CLevelData::FileRead(const char * fileName)
{
	std::ifstream fs;

	fs.open(fileName, std::ios::binary);
	if (fs.fail())return false;

	fs.read((char*)&m_nLevelBlocks, sizeof(int));
	if (m_nLevelBlocks > 0) {
		m_pLevelBlocks = new Block[m_nLevelBlocks];

		for (int i = 0; i < m_nLevelBlocks; ++i) {
			Block temp;
			fs.read((char*)&temp.pos.x, sizeof(float));
			fs.read((char*)&temp.pos.y, sizeof(float));
			fs.read((char*)&temp.pos.z, sizeof(float));
			fs.read((char*)&temp.extent.x, sizeof(float));
			fs.read((char*)&temp.extent.y, sizeof(float));
			fs.read((char*)&temp.extent.z, sizeof(float));
			m_pLevelBlocks[i] = temp;
		}
	}

	fs.read((char*)&m_nSpawnPoints, sizeof(int));
	if (m_nSpawnPoints > 0) {
		m_pSpawnPoints = new XMFLOAT3[m_nSpawnPoints];

		for (int i = 0; i < m_nSpawnPoints; ++i) {
			XMFLOAT3 temp;
			fs.read((char*)&temp.x, sizeof(float));
			fs.read((char*)&temp.y, sizeof(float));
			fs.read((char*)&temp.z, sizeof(float));
			m_pSpawnPoints[i] = temp;
		}
	}

	fs.read((char*)&m_nTargetPlaceBlocks, sizeof(int));
	if (m_nTargetPlaceBlocks > 0) {
		m_pTargetPlaceBlocks = new Block[m_nTargetPlaceBlocks];

		for (int i = 0; i < m_nTargetPlaceBlocks; ++i) {
			Block temp;
			fs.read((char*)&temp.pos.x, sizeof(float));
			fs.read((char*)&temp.pos.y, sizeof(float));
			fs.read((char*)&temp.pos.z, sizeof(float));
			fs.read((char*)&temp.extent.x, sizeof(float));
			fs.read((char*)&temp.extent.y, sizeof(float));
			fs.read((char*)&temp.extent.z, sizeof(float));
			m_pTargetPlaceBlocks[i] = temp;
		}
	}

	fs.close();

	for (int i = 0; i < m_nLevelBlocks; ++i) 
	{
		m_pLevelBlocks[i].pos.x		*= LEVELSIZE;
		m_pLevelBlocks[i].pos.y		*= LEVELSIZE;
		m_pLevelBlocks[i].pos.z		*= LEVELSIZE;
		m_pLevelBlocks[i].extent.x	*= LEVELSIZE/2.0;
		m_pLevelBlocks[i].extent.y	*= LEVELSIZE/2.0;
		m_pLevelBlocks[i].extent.z	*= LEVELSIZE/2.0;
	}

	for (int i = 0; i < m_nSpawnPoints; ++i)
	{
		m_pSpawnPoints[i].x *= LEVELSIZE;
		m_pSpawnPoints[i].y *= LEVELSIZE;
		m_pSpawnPoints[i].z *= LEVELSIZE;
	}

	for (int i = 0; i < m_nTargetPlaceBlocks; ++i)
	{
		m_pTargetPlaceBlocks[i].pos.x		*= LEVELSIZE;
		m_pTargetPlaceBlocks[i].pos.y		*= LEVELSIZE;
		m_pTargetPlaceBlocks[i].pos.z		*= LEVELSIZE;
		m_pTargetPlaceBlocks[i].extent.x	*= LEVELSIZE/2.0;
		m_pTargetPlaceBlocks[i].extent.y	*= LEVELSIZE/2.0;
		m_pTargetPlaceBlocks[i].extent.z	*= LEVELSIZE/2.0;
	}

}
