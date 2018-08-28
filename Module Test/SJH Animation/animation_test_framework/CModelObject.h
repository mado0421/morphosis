#pragma once
#include"Object.h"
#include"FBXDataManager.h"
class CModelObject : public CGameObject
{
public:
	BoundingOrientedBox m_OOBB;	// temp position
	CModelObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, FBXDataManager* fbxManager);
	virtual ~CModelObject();
};

