#pragma once
#include"Object.h"
class CSolidCube : public CGameObject
{
public:
	CSolidCube(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSolidCube();
};

