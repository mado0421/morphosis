#pragma once

struct Block {
	XMFLOAT3 pos, extent;
};

class CLevelData
{
public:
	Block		*m_pLevelBlocks = NULL;
	int			m_nLevelBlocks = 0;

	Block		*m_pTargetPlaceBlocks = NULL;
	int			m_nTargetPlaceBlocks = 0;

	XMFLOAT3	*m_pSpawnPoints = NULL;
	int			m_nSpawnPoints = 0;

public:
	CLevelData();
	~CLevelData();

public:
	bool FileRead(const char* fileName);
};

