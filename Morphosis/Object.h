#pragma once
#include "stdafx.h"
/***********************************************************************
Ŭ���� ���

- ������Ʈ�� UI

������Ʈ
\ ������
\ ��ġ

�̵��ϴ� ������Ʈ
\ �̵� ����� �ӷ�

�ִϸ��̼��� �ִ� ������Ʈ
\ �ִϸ��̼� ��Ʈ�ѷ�->�̰� �������� �ص� �ǰڴµ�??
\ �ִϸ��̼� Ÿ��(float�� ����)
\ �ִϸ��̼� ����(int�� ����)

������Ʈ�� ����:
����, ����, �Ѿ�, �÷��̾�, �� NPC, ������ ����Ʈ, Ʈ����, ��ƼŬ
***********************************************************************/
/***********************************************************************
������

�� ����Ʈ�� ������ ��(������� �� ����)
�浹ü ����Ʈ�� ������ ��(������� �� ����)

��

�޽��� ����
�ؽ�ó�� ����(���� ��, �⺻ �ؽ�ó ���)

������ ����Ʈ�� 0���� �ƹ� �͵� ���� ������.
***********************************************************************/
/***********************************************************************
�ִϸ��̼�

�ִϸ��̼� Ŭ������ ����. ��¥�� ������ ����� ����?

�޸ӳ��̵� �ִϸ��̼� ��Ʈ�ѷ�(FSM)

- ���̵�
- ���̵� Ư���ൿ
- ������ �ȱ�/�޸���
- ��������/���������� �ȱ�/�޸���
- �ڷ� �ȱ�/�޸���
(����14ó�� �ع�����? ĳ���� ������ �����ϴ� ��)
- ���� ����
- Ȱ�� ��
- ���� ��(���� ���)
- ��������
- ���

�޸ӳ��̵尡 �ƴ� �ִϸ��̼� ��Ʈ�ѷ��� �ʿ��Ѱ�?
��? ��鸮�� ����? Ǯ? ���? 

***********************************************************************/

//class Mesh;
//class Texture;
//class Collider;
//
//class Model;
//
//class Prefab {
//	vector<Model>		m_vecModel;
//	vector<Collider>	m_vecCollider;
//};

struct CB_OBJECT_INFO {
	XMFLOAT4X4	xmf4x4World;
};

class Object {
public:
	void Rotate(const XMFLOAT3& angle);
	void Move(const XMFLOAT3& velocity);

public:
	/*******************************************************************
	�����Ӹ��� ȣ��� �Լ�
	*******************************************************************/
	virtual void Update(float fTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList);	// ������ �ϱ� ���� ȣ��
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList); // ������ �ϱ� ���� ȣ��


	/*******************************************************************
	���� �Լ�
	*******************************************************************/
	void SetPrefabIdx(int idx) { m_idxPrefab = idx; }

	/*******************************************************************
	�ʱ�ȭ �Լ�
	*******************************************************************/
	Object();
	virtual void CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void CreateConstantBufferView(ID3D12Device * pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int& offset);
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr);

protected:
	int							m_idxPrefab = 0;
	XMFLOAT4X4					m_xmf4x4World;

	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;

};

class MovingObject : public Object {
protected:
	XMFLOAT3	m_xmf3LocalVelocity;
};

class AnimatedMovindObject : public MovingObject {
protected:
	///AnimationController g_AnimCtrl; // �̰� �������� ����.
	int			m_idxAnimState;
	float		m_fAnimTime;
};

class Terrain : public Object {
public:
	virtual void Update(float fTime);
};

class Player : public AnimatedMovindObject {
public:
	virtual void Update(float fTime);
};

class Bullet : public MovingObject {
public:
	virtual void Update(float fTime);
};
