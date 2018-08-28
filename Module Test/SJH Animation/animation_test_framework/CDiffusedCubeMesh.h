#pragma once
class CDiffusedCubeMesh : public CMesh
{
public:
	CDiffusedCubeMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,float fSize);
	CDiffusedCubeMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth);
	~CDiffusedCubeMesh();
};

