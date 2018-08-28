#pragma once
#include"CModelObject.h"
#include"Player.h"

struct Tile
{
	BoundingOrientedBox OOBB;
	int type;	// 기울어진 평면
	XMFLOAT3 normal;
};

class TerrainObject : public CModelObject
{
	int m_nTile;
	Tile* m_pTile;
	
public:
	TerrainObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FBXDataManager* fbxManager);
	~TerrainObject();

	int GetHeight();

	void CollideCheck(CPlayer& player);
};

