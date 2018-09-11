#include "stdafx.h"
#include "LevelData.h"


CLevelData::CLevelData()
{
}


CLevelData::~CLevelData()
{
}

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

}
