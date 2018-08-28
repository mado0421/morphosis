#pragma once
#include "Mesh.h"
class FBXDataManager;
class CDataMesh :public CMesh
{
public:
	CDataMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FBXDataManager* fbxManager);
	virtual ~CDataMesh();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	ID3D12Resource  *m_pVertexRes = NULL;
	ID3D12Resource	*m_pUVRes = NULL;


};

