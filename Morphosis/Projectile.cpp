#include "stdafx.h"
#include "Projectile.h"
#include "Player.h"
#include "Collider.h"

/*********************************************************************
2019-06-17
CProjectile
*********************************************************************/
CProjectile::CProjectile() : CObject()
{

	m_IsAlive = false;
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	m_fSpeed = g_fDefaultProjectileSpeed;
	m_fLifeTime = g_DefaultProjectileLifeTime;
	m_BaseDamage = g_DefaultDamage;
}
CProjectile::~CProjectile()
{

}
void CProjectile::Initialize(CObject * obj)
{
	/*********************************************************************
	2019-06-17
	���⼭ ����� �ϴ� ����
	���ڷ� �޾ƿ� ������Ʈ�κ��� ��ġ�� ����, ���� �޾ƾ� �Ѵٴ� ��.

	�� ����ü�� ȸ���ϸ鼭 �����;� �ϸ� ��. �׷��� ��. ������ ��.

	�浹ü ��ġ�� �ٲ���� ��.
	*********************************************************************/
	XMFLOAT3 pos = obj->GetPosition();
	pos.y += 10;
	SetPosition(pos);
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());


	/*********************************************************************
	2019-06-17
	�������� �޽��� ����
	*********************************************************************/
	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction = Vector3::Normalize(obj->GetLook());
	m_fLifeTime = g_DefaultProjectileLifeTime;


	/*********************************************************************
	2019-06-18
	���� ����� ���Դ�. �׷� �� ��ķκ��� ��ġ�� ������ �̾Ƴ���?

	������ �̿ϼ�. �����ɷ� ��� ����.
	*********************************************************************/
	//XMFLOAT4X4 hand = dynamic_cast<CPlayer*>(obj)->GetHandMatrix();
	//XMFLOAT3 xmf3Right		= XMFLOAT3(hand._11, hand._12, hand._13);
	//XMFLOAT3 xmf3Up			= XMFLOAT3(hand._21, hand._22, hand._23);
	//XMFLOAT3 xmf3Look		= XMFLOAT3(hand._31, hand._32, hand._33);
	//XMFLOAT3 xmf3Position	= XMFLOAT3(hand._41, hand._42, hand._43);
	//xmf3Position = Vector3::Add(xmf3Position, obj->GetPosition());
	//SetPosition(xmf3Position);
	//for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
	//	myb->Center = xmf3Position;
	//for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
	//	mys->Center = xmf3Position;
	//SetLook(xmf3Look);
	//SetUp(xmf3Up);
	//SetRight(xmf3Right);
}
void CProjectile::Initialize(CObject * obj, const char * modelName, Effect * effect)
{
	XMFLOAT3 pos = obj->GetPosition();
	pos.y += 10;
	SetPosition(pos);
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());


	m_vecModelIdx.clear();
	AddModel(modelName);
	m_vecEffects.push_back(effect);

	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction = Vector3::Normalize(obj->GetLook());
	m_fLifeTime = g_DefaultProjectileLifeTime;
}
void CProjectile::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	/*********************************************************************
	2019-06-18
	LifeTime ����
	*********************************************************************/
	m_fLifeTime -= fTimeElapsed;
	if (IsExpired()) { Disable(); return; }

	XMFLOAT3 xmf3Move = m_xmf3Direction;
	xmf3Move = Vector3::Multiply(m_fSpeed, xmf3Move);
	xmf3Move = Vector3::Multiply(fTimeElapsed, xmf3Move);

	m_xmf4x4World._41 += xmf3Move.x;
	m_xmf4x4World._42 += xmf3Move.y;
	m_xmf4x4World._43 += xmf3Move.z;

	//if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

	///*********************************************************************
	//2019-06-18
	//�浹ü ��Ʈ
	//*********************************************************************/
	//for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CProjectile::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CProjectile::Damage(CObject* obj)
{
	dynamic_cast<CPlayer*>(obj)->TakeDamage(static_cast<int>(m_BaseDamage));
}