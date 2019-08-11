#include "stdafx.h"
#include "AnimationController.h"
#include "Object.h"
#include "Player.h"
#include "Effect.h"
#include "ObjectManager.h"
#include "Collider.h"


/*********************************************************************
2019-06-17
CObject
*********************************************************************/
CObject::CObject()
{
	cout << "CObject ���� - " << sizeof(CObject) << "\n";
	
	SetPosition(XMFLOAT3(0, 0, 0));
	m_xmf4x4World = Matrix4x4::Identity();
	m_IsAlive = true;
	m_xmf3CollisionOffset = XMFLOAT3(0, 0, 0);
	m_xmf3CameraFocus = XMFLOAT3(0, 0, 0);
	m_AnimationState = static_cast<int>(0);
	m_Team = TEAM_DEFAULT;
	m_fHeightVelocity = 0.0f;
	m_IsGround = false;
}
CObject::~CObject()
{
	// stl container clear
	m_Collider.clear();
	m_ModelList.clear();
	std::queue<Collider*> empty;

	// uploadAddress delete
	if (m_pd3dcbAnimation) {
		m_pd3dcbAnimation->Unmap(0, nullptr);
		m_pd3dcbAnimation->Release();
		memset(m_pcbxmAnimation, NULL, (((sizeof(XMMATRIX) * g_nAnimBone) + 255) & ~255));
	}
	if (m_pd3dcbObject) {
		m_pd3dcbObject->Unmap(0, nullptr);
		m_pd3dcbObject->Release();
		memset(m_pcbMappedObject, NULL, ((sizeof(CB_OBJECT_INFO) + 255) & ~255));
	}
	//if(m_pcbxmAnimation) delete m_pcbxmAnimation;
	//if(m_pcbMappedObject) delete m_pcbMappedObject;

	// ID3D12Resource release
	if(m_pd3dcbAnimation) m_pd3dcbAnimation->Release();
	if(m_pd3dcbObject) m_pd3dcbObject->Release();

	// ��� ���߿� ���� ���� Ŭ�����ϸ鼭 ���� ���̹Ƿ� ���⼭�� �׳� NULL�� ����
	m_AnimationController = NULL;
}
void CObject::AddCollisionEffect(CObject * p)
{
	for (int i = 0; i < m_vecEffects.size(); ++i) {
		m_vecEffects[i]->Act(dynamic_cast<CPlayer*>(p));
	}
}
void CObject::SetMng(CObjectManager * mng)
{
	m_pObjMng = mng;
}
void CObject::SetAnimatedMatrix(float time)
{
	if (m_AnimationController) {
		for (int i = 0; i < m_AnimationController->m_AnimData[0]->m_nBoneList; ++i) {
			m_a[i] = XMMatrixTranspose(m_AnimationController->GetFinalMatrix(i, time));
		}
	}

}
void CObject::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}
void CObject::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{

}
void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
}
void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{

	if (!m_IsAlive) return;
	if (!m_ModelList.empty()) {
		if (m_AnimationController) {
			ChangeAnimClip();
			SetAnimatedMatrix(m_AnimationTime);
		}
		UpdateConstantBuffer(pd3dCommandList);
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {

			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
			(*p)->Render(pd3dCommandList, isDebug);
		}
	}
}
void CObject::AddCollider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion)
{
	m_Collider.emplace_back(offset, extents, quaternion);
	m_Collider[m_Collider.size() - 1].Update(GetPosition(), GetQuaternion());
}
void CObject::AddCollider(XMFLOAT3 offset, float radius)
{
	m_Collider.emplace_back(offset, radius);
	m_Collider[m_Collider.size() - 1].Update(GetPosition(), GetQuaternion());
}
void CObject::SetColliderTrigOff()
{
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].m_trigCollided = false;
}
void CObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CObject::SetPosition(const XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 const CObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));

}
XMFLOAT3 const CObject::GetLook()
{
	XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return Vector3::Normalize(vector);
}
XMFLOAT3 const CObject::GetUp()
{
	XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return Vector3::Normalize(vector);
}
XMFLOAT3 const CObject::GetRight()
{
	XMFLOAT3 vector = { m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };
	return Vector3::Normalize(vector);
}
XMMATRIX const CObject::GetAnimationMatrix(int boneIdx)
{
	if (m_AnimationController) return m_AnimationController->GetFinalMatrix(boneIdx, m_AnimationTime);
	else return XMMatrixIdentity();
}
int const CObject::GetNumAnimationBone()
{
	if (m_AnimationController) {
		if (!m_AnimationController->m_AnimData.empty())
			return m_AnimationController->m_AnimData.front()->m_nBoneList;
	}
	return 0;
}
void CObject::SetLook(XMFLOAT3 look)
{
	m_xmf4x4World._31 = look.x;
	m_xmf4x4World._32 = look.y;
	m_xmf4x4World._33 = look.z;
}
void CObject::SetUp(XMFLOAT3 up)
{
	m_xmf4x4World._21 = up.x;
	m_xmf4x4World._22 = up.y;
	m_xmf4x4World._23 = up.z;
}
void CObject::SetRight(XMFLOAT3 right)
{
	m_xmf4x4World._11 = right.x;
	m_xmf4x4World._12 = right.y;
	m_xmf4x4World._13 = right.z;
}
void CObject::SetCameraFocus(XMFLOAT3 pos)
{
	m_xmf3CameraFocus = pos;
}
void CObject::SetRotation(const XMFLOAT3& angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)));
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;

	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CObject::AddModel(CModel * model)
{
	m_ModelList.push_back(model);
}
void CObject::SetAnimCtrl(CAnimationController * animCtrl)
{
	m_AnimationController = animCtrl;
}
XMFLOAT3 const CObject::GetCameraFocus()
{
	return m_xmf3CameraFocus;
	//return Vector3::Add(GetPosition(), m_xmf3CameraFocus);
	//return m_xmf3CameraFocus;
}
const XMFLOAT4 CObject::GetQuaternion()
{
	return Vector4::QuatFromMtx(m_xmf4x4World);
}
const bool CObject::IsCollide(const CObject& other)
{
	/*********************************************************************
	2019-06-18
	other�� �浹ü��� �� �浹ü���� ���� ���ؾ� ��.
	*********************************************************************/
	for (int i = 0; i < m_Collider.size(); ++i)
		for (int j = 0; j < other.m_Collider.size(); ++j) if (m_Collider[i].IsCollide(other.m_Collider[j])) return true;

	return false;
}
const bool CObject::IsCollide(const Collider & other)
{
	for (int i = 0; i < m_Collider.size(); ++i)
		if (m_Collider[i].IsCollide(other)) return true;

	return false;
}
void CObject::Disable()
{
	SetAlive(false);
	for (int i = 0; i < m_vecEffects.size(); ++i) delete m_vecEffects[i];
	MemoryClear(m_vecEffects);
}
Collider * const CObject::GetCollisionCollider(Collider& other, bool isMakeAlign)
{
	for (int i = 0; i < m_Collider.size(); ++i) {
		if (isMakeAlign)
			other.SetOrientation(m_Collider[i].GetOrientation());
		if (m_Collider[i].IsCollide(other)) return &m_Collider[i];
	}


	return NULL;
}


//void CObjectManager::CollisionCheck()
//{
//	/*********************************************************************
//	2019-06-18
//	�浹�˻� ǥ
//	������������������������������������������������������������������������������������������������������
//	��		   ��    Prop    ��   Player   �� Projectile ��
//	������������������������������������������������������������������������������������������������������
//	��Prop	   ��			��	  O		 ��	   O	  ��
//	������������������������������������������������������������������������������������������������������
//	��Player	   ��	 O	    ��	  O		 ��	   O	  ��
//	������������������������������������������������������������������������������������������������������
//	��Projectile��	 O	    ��	  O		 ��			  ��
//	������������������������������������������������������������������������������������������������������
//	�����ó�� ���ƿ��� ����ź�� ������ ���� ���߰� �� �Ÿ� ��...
//
//	�÷��̾�� �� ������ ����ü�� �����̶� �ϸ� �� �� ����.
//	�浹�ϸ�? ���⼭ ó�� �� ��?
//
//	�浸���� �� ó����:
//	- �÷��̾ ���Ӱ� �浹�ϸ� �̵� ��ȿ. (�����̵�� ���߿� �����ϱ�)
//	- �÷��̾ �÷��̾�� �浹�ϸ� �̵� ��ȿ. (���� ��ġ��� ���߿� �����ϱ�)
//	- �÷��̾ ������Ÿ�ϰ� �浹�ϸ� ������Ÿ���� ȿ���� �÷��̾�� ����.
//	- ������Ÿ���� ���Ӱ� �浹�ϸ� ������Ÿ�� ��Ȱ��ȭ.
//
//	�̵� ��ȿ�� �Ϸ��� �÷��̾��� Ʈ���Ÿ� �������� �ǳ�?
//	�ٵ� �浹üũ�� �̵� �Ŀ� �˻��ؾ� �ϴµ�?
//	�׷� LateUpdate()�� ������! �浹�˻縦 �ϰ� �浹������ �� ��ü���� Ʈ���Ÿ� ���ְ�
//	LateUpdate()�� �ϰ� �ϱ�.
//	������ �浹�ߴ��� �׳� �������ֱ�� ��.
//	*********************************************************************/
//	/*
//	for (auto player = m_Players.begin(); player != m_Players.end(); ++player) {
//		// Ȱ��ȭ�� �ָ� ó���� ��.
//		if ((*player)->IsAlive()) {
//
//			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
//				if (IsCollidable((*player), (*prop)))
//					if ((*player)->IsCollide((*prop)))
//						(*player)->AddCollideInfo((*prop));
//
//			for (auto otherPlayer = m_Players.begin(); otherPlayer != m_Players.end(); ++otherPlayer)
//				if (IsCollidable((*player), (*otherPlayer)))
//					if ((*player)->IsCollide((*otherPlayer))) {
//						(*player)->AddCollideInfo((*otherPlayer));
//						(*otherPlayer)->AddCollideInfo((*player));
//					}
//			for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile)
//				if (IsCollidable((*player), (*projectile)))
//					if ((*player)->IsCollide((*projectile))) {
//						(*player)->AddCollideInfo((*projectile));
//						(*projectile)->Disable();
//					}
//		}
//	}
//	for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile) {
//		// Ȱ��ȭ�� �ָ� ó���� ��.
//		if ((*projectile)->IsAlive()) {
//			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
//				if (IsCollidable((*projectile), (*prop)))
//					if ((*projectile)->IsCollide((*prop))) {
//						(*projectile)->Disable();
//						break;
//					}
//
//		}
//	}
//	*/
//
//
//}

