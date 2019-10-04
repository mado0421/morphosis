#pragma once

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


class Object {
public:
	virtual void Update(float fTime);
	virtual void Render();

protected:
	int			m_idxPrefab = 0;
	//XMMATRIX	m_xmf4x4World;
};

class MovingObject : public Object {
protected:
	//XMFLOAT3	m_xmf3LocalVelocity;
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
