#pragma once
#include "Mesh.h"
#include "Camera.h"


// 기본적인 오브젝트들 전용(크기 변동이 없는 기본적인 UI
struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CObject
{
public:
	XMFLOAT4X4						m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	CMaterial						*m_pMaterial = NULL;


	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	CB_OBJECT_INFO					*m_pcbMappedObject = NULL;
	ID3D12Resource					*m_pd3dcbObject = NULL;

public:
	CObject();
	~CObject();

public:
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update(float fTimeElapsed);

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetMaterial(CMaterial *pMaterial);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
};

class CCollideObejct : public CObject
{
protected:
	BoundingOrientedBox				m_collisionBox;


};

class CMovingObject : public CCollideObejct
{

};

class CDefaultUI : public CObject
{

};

class CFlexibleUI : public CDefaultUI
{

};