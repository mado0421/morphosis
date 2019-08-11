#pragma once

class Model;
class Collider;

struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	//XMFLOAT4X4	m_xmf4x4WorldNoTranspose;
	//UINT		m_nMaterialIndex;
};

class Object {
public:
	Object();
	~Object();

public:
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr);

	virtual void CreateConstantBufferResourceAndView(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int offset);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float elapsedTime);
	virtual void LateUpdate(float elapsedTime);

public:
	void AddModel(const char* modelName);
	void SetPosition(float x, float y, float z);

public:
	XMFLOAT4X4		m_xmf4x4World;

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*					m_pd3dCBResource	= NULL;
	CB_OBJECT_INFO*					m_pCBMappedObjects	= NULL;


	vector<int>		m_vecModelIdx;
	Collider*		m_pCollider;
};

class MovableObject : public Object {
public:
	MovableObject();
	~MovableObject();

public:
	virtual void	Update(float elapsedTime) override;
	virtual void	LateUpdate(float elapsedTime) override;

protected:
	XMFLOAT3	m_xmf3MovingDir;
	float		m_fSpeed;
};

class ProjectileObject : public MovableObject {

};

class PlayerObject : public MovableObject {

};