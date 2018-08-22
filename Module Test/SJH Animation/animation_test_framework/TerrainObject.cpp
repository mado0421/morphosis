#include "stdafx.h"
#include "TerrainObject.h"


TerrainObject::TerrainObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FBXDataManager* fbxManager)
	: CModelObject(pd3dDevice,pd3dCommandList,pd3dGraphicsRootSignature,fbxManager)
{
	m_nTile = 0;
	m_pTile = new Tile[m_nTile];

	/*
	m_pOOBB[0].Center = XMFLOAT3(0, 0, 0);
	m_pOOBB[0].Extents = XMFLOAT3(0, 0, 0);
	m_pOOBB[0].Orientation = XMFLOAT4(0, 0, 0, 1);

	...

	m_pOOBB[m_nOOBB-1].Center = XMFLOAT3(0, 0, 0);
	m_pOOBB[m_nOOBB-1].Extents = XMFLOAT3(0, 0, 0);
	m_pOOBB[m_nOOBB-1].Orientation = XMFLOAT4(0, 0, 0, 1);
	*/

	/*
	OR
	FileLoad Map OOBBs;
	*/
}




TerrainObject::~TerrainObject()
{
	if (m_pTile)delete[] m_pTile;
}

void TerrainObject::CollideCheck(CPlayer& player)
{
	for (int i = 0; i < m_nTile; ++i)
	{
		//m_pTile[i]
	}
}
