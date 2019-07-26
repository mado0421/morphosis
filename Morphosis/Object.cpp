#include "stdafx.h"
#include "Object.h"
#include "AnimationController.h"
#include "Mesh.h"
#include "Importer.h"
#include "Texture.h"

/*********************************************************************
2019-06-17
CObject
*********************************************************************/
CObject::CObject()
{
	SetPosition(XMFLOAT3(0, 0, 0));
	m_xmf4x4World			= Matrix4x4::Identity();
	m_IsAlive				= true;
	m_xmf3CollisionOffset	= XMFLOAT3(0, 0, 0);
	m_xmf3CameraTargetOffset= XMFLOAT3(0, 0, 0);
	m_AnimationState		= static_cast<int>(0);
	m_Team					= TEAM_DEFAULT;
	m_Tag					= ObjectTag::Prop;
	m_fHeightVelocity		= 0.0f;
	m_IsGround				= false;
}
CObject::~CObject()
{
}
void CObject::SetMng(CObjectManager * mng)
{
	m_pObjMng = mng;
}
void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
}
void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{
	if (!m_IsAlive) return;
	if (!m_ModelList.empty()) {
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {
			if (m_AnimationController) p->SetAnimatedMatrix(m_AnimationController);
			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
			p->Render(pd3dCommandList, isDebug);
		}
	}
}
void CObject::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;

	/*********************************************************************
	2019-06-17
	�̵� ��Ʈ
	*********************************************************************/


	/*********************************************************************
	2019-06-17
	�ִϸ��̼� ��Ʈ
	*********************************************************************/
	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);
}
void CObject::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;
}

void CObject::SetAnimCtrl(CAnimationController * animCtrl)
{
	m_AnimationController = animCtrl;
}

//void CObject::AddAnimClip(AnimationClip * animClip)
//{
//	/*********************************************************************
//	2019-06-16
//	�ִϸ��̼� ��Ʈ�ѷ��� ������ �����ϰ� �߰����ش�.
//	*********************************************************************/
//	if (NULL == m_AnimationController)
//		m_AnimationController = new CAnimationController();
//	m_AnimationController->AddAnimData(animClip);
//}
void CObject::AddCollider(XMFLOAT3 center, XMFLOAT3 extents, XMFLOAT4 quaternion, XMFLOAT3 offset, ColliderTag tag)
{
	m_Collider.emplace_back(center, extents, quaternion, offset, tag);
	m_Collider[m_Collider.size()-1].Update(GetPosition(), GetQuaternion());
}
void CObject::AddCollider(XMFLOAT3 center, float radius, XMFLOAT3 offset, ColliderTag tag)
{
	m_Collider.emplace_back(center, radius, offset, tag);
	m_Collider[m_Collider.size() - 1].Update(GetPosition(), GetQuaternion());
}
void CObject::ChangeAnimClip(const char * animClipName)
{
	m_AnimationController->ChangeAnimClip(animClipName);
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
	if (m_AnimationController) return m_AnimationController->GetFinalMatrix(boneIdx);
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
void CObject::SetCameraTargetOffset(XMFLOAT3 pos)
{
	m_xmf3CameraTargetOffset = pos;
}
void CObject::SetRotation(const XMFLOAT3& angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)));
	XMFLOAT3 xmf3Right	= XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up		= XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look	= XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3Look	= Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right	= Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look	= Vector3::Normalize(xmf3Look);
	xmf3Right	= Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up		= Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;

	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
XMFLOAT3 const CObject::GetCameraTargetPos()
{
	return Vector3::Add(GetPosition(), m_xmf3CameraTargetOffset);
	//return m_xmf3CameraTargetOffset;
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
Collider * CObject::GetCollisionCollider(const Collider & other)
{
	for (int i = 0; i < m_Collider.size(); ++i)
		if (m_Collider[i].IsCollide(other)) return &m_Collider[i];


	return NULL;
}
void CObject::ProcessInput(UCHAR * pKeysBuffer, float mouse)
{
}
//void CObject::AddCollideInfo(CObject * obj)
//{
//	m_CollideInfo.push(obj);
//}

/*********************************************************************
2019-06-17
CPlayer
*********************************************************************/
CPlayer::CPlayer() : CObject()
{
	//m_ComprehensiveGroundCollider	= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 2.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	//m_DetailedGroundCollider		= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 4.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	m_fSpeed					= g_fDefaultPlayerSpeed;
	m_fRemainingTimeOfFire		= 0.0f;
	m_fRPM						= (1 / static_cast<float>(g_DefaultRPM)) * 60.0f;
	m_Tag						= ObjectTag::Player;
	m_xmf3Move					= XMFLOAT3(0, 0, 0);
	m_xmf3SpawnPoint			= GetPosition();
	m_HealthPoint				= g_DefaultHealthPoint;
	m_fRemainingTimeOfRespawn	= 0.0f;
	m_xmf4x4Hand				= Matrix4x4::Identity();
	m_rotationInput				= 0.0f;
	for (int i = 0; i < static_cast<int>(Move::count); ++i) m_trigInput[i] = false;
}
void CPlayer::Update(float fTimeElapsed)
{
	if (!m_IsAlive) {
		m_fRemainingTimeOfRespawn -= fTimeElapsed;
		if (m_fRemainingTimeOfRespawn <= 0.0f) {
			CPlayer::Enable();
		}
		return;
	}

	/*********************************************************************
	2019-06-18
	���� Ÿ�̸� ��Ʈ
	*********************************************************************/
	m_fRemainingTimeOfFire -= fTimeElapsed;
	//SetHandMatrix();

	/*********************************************************************
	2019-06-17
	�ִϸ��̼� ��Ʈ
	*********************************************************************/
	if (m_AnimationController) {
		m_AnimationController->Update(fTimeElapsed);

		if (static_cast<int>(AnimationState::IDLE) == m_AnimationState) {
			if (IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::RUN);
				m_AnimationController->ChangeAnimClip("PlayerRun");
			}
		}
		else if (static_cast<int>(AnimationState::RUN) == m_AnimationState) {
			if (!IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationController->ChangeAnimClip("PlayerIdle");
			}
		}
		else if (static_cast<int>(AnimationState::FIRE) == m_AnimationState) {
			if (m_AnimationController->IsClipEnd()) {
				m_AnimationState = static_cast<int>(AnimationState::RUN);
				m_AnimationController->ChangeAnimClip("PlayerRun");
			}
		}
	}
}
void CPlayer::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;



	/*********************************************************************
	2019-06-17
	�̵� ��Ʈ

	2019-07-13
	�̰��Ǿ����ϴ�.
	*********************************************************************/
	// �߷� ���ӵ� ����
	//if (m_IsGround) m_fHeightVelocity = 0.0f;
	//else m_fHeightVelocity -= fTimeElapsed /** g_Gravity*/;
	m_fHeightVelocity -= fTimeElapsed * g_Gravity;

	/*********************************************************************
	2019-07-15
	1. �̵� ������ ���ϰ� �� ���� ������ �浹ü�� ���� �浹ü ����
	2. ������ �浹ü�� ������ �ֺ��� �浹�ϴ� �ֵ��� ���� ã��.
	3. �³׵��� ������� ������ �浹ü�� �˻�

	//m_ComprehensiveGroundCollider	= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 2.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	//m_DetailedGroundCollider		= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 4.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	*********************************************************************/

	// Queue Clear
	std::queue<Collider*> empty;
	std::swap(m_CollideInfo, empty);

	// ���� ���� ����
	m_xmf3Move = Move(fTimeElapsed);

	int s = 0;
	//vector<XMFLOAT3> test;
	static queue<XMFLOAT3> test;
	static queue<int> test2;
	while (true) {

		Collider DetailedGroundCollider(Vector3::Add(GetPosition(), m_xmf3Move), g_fDefaultUnitScale / 2.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
		Collider* collider = m_pObjMng->GetCollider(DetailedGroundCollider, ColliderTag::PROP);
		if (NULL == collider) break;

		s++;
		test.push(m_xmf3Move);
		test2.push(s);
		if (test.size() > 100) test.pop();
		if (test2.size() > 100) test2.pop();
		XMFLOAT3 look = collider->GetLook();
		XMFLOAT3 up = collider->GetUp();
		XMFLOAT3 right = Vector3::CrossProduct(up, look);

		XMVECTOR planeXY = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&look));
		XMVECTOR planeXZ = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&up));
		XMVECTOR planeYZ = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&right));

		XMVECTOR temp;
		XMFLOAT3 xmf3Temp;
		XMFLOAT3 xmf3MyExtents;



		temp = XMPlaneDotCoord(planeYZ, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.x = xmf3Temp.x;

		temp = XMPlaneDotCoord(planeXY, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.z = xmf3Temp.x;

		temp = XMPlaneDotCoord(planeXZ, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.y = xmf3Temp.x;
		/*********************************************************************
		2019-07-15
		������� �� ��
		- MyExtents�� Collider�� Extents�� ��
		�� ���� �� �鸸 �浹�ϰų�(����), �� ���� �浹�ϰų�(�����ڸ�), �� ���� �浹�� �� ����(�𼭸�)
		�˻� 6���� �ϸ�

		2019-07-18
		���� �� ��ġ�� �������� ���ϰ� �����Ͽ����ϴ�~

		���밪�� ���ؼ� MyExtents�� Extents���� Ŀ�� ��.
		abs(MyExtent) - Extent �� ���� ���� ū ���� �浹��.
		MyExtent�� ��ȣ�� ���� �浹���� ������ ����.

		���� ���� ���� �� �ִ�. �׷� ��~�� ���� ���� ������ ��. �߿��� �͵� �ƴϰ�

		*********************************************************************/
		XMFLOAT3 colliderExtents = collider->GetExtents();
		XMFLOAT3 dir(0, 0, 0);

		float max = abs(xmf3MyExtents.x) - colliderExtents.x; int colliderDir = 0;
		if (abs(xmf3MyExtents.y) - colliderExtents.y > max) { max = abs(xmf3MyExtents.y) - colliderExtents.y; colliderDir = 1; }
		if (abs(xmf3MyExtents.z) - colliderExtents.z > max) { max = abs(xmf3MyExtents.z) - colliderExtents.z; colliderDir = 2; }

		XMVECTOR parallel, perpendicular;
		if(colliderDir == 0){ 
			if(xmf3MyExtents.x >0)	dir = Vector3::Normalize(right);
			else					dir = Vector3::Normalize(Vector3::Multiply(-1, right));
		}
		else if (colliderDir == 1) {
			if (xmf3MyExtents.y > 0)	dir = Vector3::Normalize(up);
			else						dir = Vector3::Normalize(Vector3::Multiply(-1, up));
		}
		else {
			if (xmf3MyExtents.z > 0)	dir = Vector3::Normalize(look);
			else						dir = Vector3::Normalize(Vector3::Multiply(-1, look));
		}

		XMVector3ComponentsFromNormal(&parallel, &perpendicular, XMLoadFloat3(&m_xmf3Move), XMLoadFloat3(&dir));


		//if (IsIn(xmf3MyExtents.x, colliderExtents.x, colliderExtents.x + g_fDefaultUnitScale / 2.0f))		dir = Vector3::Add(dir, right);	
		//if (IsIn(xmf3MyExtents.y, colliderExtents.y, colliderExtents.y + g_fDefaultUnitScale / 2.0f))		dir = Vector3::Add(dir, up);		
		//if (IsIn(xmf3MyExtents.z, colliderExtents.z, colliderExtents.z + g_fDefaultUnitScale / 2.0f))		dir = Vector3::Add(dir, look);	
		//if (IsIn(xmf3MyExtents.x, -colliderExtents.x - g_fDefaultUnitScale / 2.0f, -colliderExtents.x))	dir = Vector3::Add(dir, Vector3::Multiply(-1, right));
		//if (IsIn(xmf3MyExtents.y, -colliderExtents.y - g_fDefaultUnitScale / 2.0f, -colliderExtents.y))	dir = Vector3::Add(dir, Vector3::Multiply(-1, up));
		//if (IsIn(xmf3MyExtents.z, -colliderExtents.z - g_fDefaultUnitScale / 2.0f, -colliderExtents.z))	dir = Vector3::Add(dir, Vector3::Multiply(-1, look));
		//if (IsIn(xmf3MyExtents.x, -colliderExtents.x - g_fDefaultUnitScale / 2.0f, -colliderExtents.x))	Vector3::Add(dir, Vector3::Multiply(-1, right));
		//if (IsIn(xmf3MyExtents.y, -colliderExtents.y - g_fDefaultUnitScale / 2.0f, -colliderExtents.y))	Vector3::Add(dir, Vector3::Multiply(-1, up));
		//if (IsIn(xmf3MyExtents.z, -colliderExtents.z - g_fDefaultUnitScale / 2.0f, -colliderExtents.z))	Vector3::Add(dir, Vector3::Multiply(-1, look));

		//dir = Vector3::Normalize(dir);


		//XMVector3ComponentsFromNormal(&parallel, &perpendicular, XMLoadFloat3(&m_xmf3Move), XMLoadFloat3(&dir));

		XMStoreFloat3(&m_xmf3Move, perpendicular);

		//m_xmf3Move = Vector3::Add(m_xmf3Move, Vector3::Multiply(restore, dir));


		// ���� ����
		if (Vector3::DotProduct(dir, XMFLOAT3(0, 1, 0)) > 0.8) {
			m_fHeightVelocity = fTimeElapsed * g_Gravity;
		}

		//system("cls");
		//cout << "Collsion Time: " << s << "\nMovind Dir: (" << m_xmf3Move.x << ", " << m_xmf3Move.y << ", " << m_xmf3Move.z << ")\n";

	}
	s++;
	test.push(m_xmf3Move);
	test2.push(s);
	if (test.size() > 100) test.pop();
	if (test2.size() > 100) test2.pop();

	XMFLOAT3 xmf3Right	= XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up		= XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look	= XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	float rotationAngle = Rotate(fTimeElapsed);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle));
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
	m_xmf4x4World._41 += m_xmf3Move.x;
	m_xmf4x4World._42 += m_xmf3Move.y;
	m_xmf4x4World._43 += m_xmf3Move.z;

	XMStoreFloat3(&m_xmf3CameraTargetOffset, XMVector3Rotate(XMLoadFloat3(&m_xmf3CameraTargetOffset), XMQuaternionRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle))));













	//bool alreadyRestore = false;	// �̵��� ��ȿ ó�� �ߴ��� ����
	//while (!m_CollideInfo.empty()) {
	//	if (!alreadyRestore && ObjectTag::Prop == m_CollideInfo.front()->m_Tag) alreadyRestore = RestorePosition();
	//	if (!alreadyRestore && ObjectTag::Player == m_CollideInfo.front()->m_Tag) alreadyRestore = RestorePosition();	
	//	if (ObjectTag::Projectile == m_CollideInfo.front()->m_Tag) dynamic_cast<CProjectile*>(m_CollideInfo.front())->Damage(this);
	//	
	//	m_CollideInfo.pop();
	//}



	/*********************************************************************
	2019-06-18
	�浹ü ��Ʈ
	
	2019-07-06
	�浹ü�� �����ؼ� �����ϸ� ������Ʈ �κ� ����
	
	�ƴ� ���� �׳� LateUpdate()���� ������.
	*********************************************************************/
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());



	TriggerOff();
	if (m_HealthPoint <= 0) CPlayer::Disable();
}
void CPlayer::ProcessInput(UCHAR * pKeysBuffer, float mouse)
{
	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[static_cast<int>(Move::W)] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[static_cast<int>(Move::A)] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[static_cast<int>(Move::S)] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[static_cast<int>(Move::D)] = true;

	if (g_IsMouseMode) {
		m_rotationInput = mouse;
	}
	else {
		if (pKeysBuffer[KEY::Q] & 0xF0) m_trigInput[static_cast<int>(Move::Q)] = true;
		if (pKeysBuffer[KEY::E] & 0xF0) m_trigInput[static_cast<int>(Move::E)] = true;
	}
}
void CPlayer::Enable()
{
	m_HealthPoint = g_DefaultHealthPoint;
	SetPosition(m_xmf3SpawnPoint);
	m_fHeightVelocity = 0;

	/*********************************************************************
	2019-07-03
	��ġ�� �ʱ�ȭ��ų ��, �÷��̾��� ��ġ�� �����¸�ŭ ���ؼ�(�����ǥ ����) �Ű��ش�.

	2019-07-06
	�浹ü ��ġ ���� ���� ������µ� ���� Update()���� �ڵ����� ������ �����̹Ƿ� ������.

	*********************************************************************/
	CObject::Enable();
}
void CPlayer::Disable()
{
	m_fRemainingTimeOfRespawn = g_RespawnDelay;
	CObject::Disable();
}
void CPlayer::Shoot()
{
	m_AnimationState = static_cast<int>(AnimationState::FIRE);
	if(m_AnimationController) m_AnimationController->ChangeAnimClip("PlayerFire");
	m_fRemainingTimeOfFire = m_fRPM;
}
bool CPlayer::IsShootable()
{
	return m_fRemainingTimeOfFire <= 0;
}
XMFLOAT4X4 CPlayer::GetHandMatrix()
{
	return m_xmf4x4Hand;
}
void CPlayer::AddGroundCollider(XMFLOAT3 center, float radius, XMFLOAT3 offset)
{
	AddCollider(center, radius, offset, ColliderTag::GROUNDCHECK);
}
void CPlayer::TriggerOff()
{
	for (int i = 0; i < static_cast<int>(Move::count); i++) m_trigInput[i] = false;
	m_rotationInput = 0;
}
XMFLOAT3 CPlayer::Move(float fTimeElapsed)
{
	XMFLOAT3 temp(0, 0, 0);
	if (m_trigInput[static_cast<int>(Move::W)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetLook())); }
	if (m_trigInput[static_cast<int>(Move::A)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetRight()))); }
	if (m_trigInput[static_cast<int>(Move::S)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetLook()))); }
	if (m_trigInput[static_cast<int>(Move::D)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetRight())); }
	temp = Vector3::Normalize(temp);

	temp.y += m_fHeightVelocity;

	return Vector3::Multiply(fTimeElapsed * m_fSpeed, temp);
}
float CPlayer::Rotate(float fTimeElapsed)
{
	float temp = 0;
	if (g_IsMouseMode) {
		temp += m_rotationInput * fTimeElapsed * g_MouseInputSensitivity;
	}
	else {
		if (m_trigInput[static_cast<int>(Move::Q)]) temp += fTimeElapsed * m_fSpeed * -1;
		if (m_trigInput[static_cast<int>(Move::E)]) temp += fTimeElapsed * m_fSpeed;
	}

	return temp;
}
void CPlayer::SetHandMatrix()
{
	if(m_AnimationController) m_xmf4x4Hand = m_AnimationController->GetPositionOfBone("Bip001 L Hand");
}

bool CPlayer::RestorePosition()
{
	m_xmf4x4World._41 -= m_xmf3Move.x;
	m_xmf4x4World._42 -= m_xmf3Move.y;
	m_xmf4x4World._43 -= m_xmf3Move.z;
	return true;
}

/*********************************************************************
2019-06-17
CProjectile
*********************************************************************/
CProjectile::CProjectile() : CObject()
{
	m_IsAlive		= false;
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	m_fSpeed		= g_fDefaultProjectileSpeed;
	m_fLifeTime		= g_DefaultProjectileLifeTime;
	m_Tag			= ObjectTag::Projectile;
	m_BaseDamage	= g_DefaultDamage;
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
	pos.y += 20;
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

	m_xmf3Direction		= Vector3::Normalize(obj->GetLook());
	m_fLifeTime			= g_DefaultProjectileLifeTime;


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
void CProjectile::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	/*********************************************************************
	2019-06-18
	LifeTime ����
	*********************************************************************/
	m_fLifeTime -= fTimeElapsed;
	if (IsExpired()) { m_IsAlive = false; return; }

	XMFLOAT3 xmf3Move = m_xmf3Direction;
	xmf3Move = Vector3::Multiply(m_fSpeed, xmf3Move);
	xmf3Move = Vector3::Multiply(fTimeElapsed, xmf3Move);

	m_xmf4x4World._41 += xmf3Move.x;
	m_xmf4x4World._42 += xmf3Move.y;
	m_xmf4x4World._43 += xmf3Move.z;

	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

	/*********************************************************************
	2019-06-18
	�浹ü ��Ʈ
	*********************************************************************/
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CProjectile::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;

}
void CProjectile::Damage(CObject* obj)
{
	dynamic_cast<CPlayer*>(obj)->TakeDamage(static_cast<int>(m_BaseDamage));
}

/*********************************************************************
2019-06-17
CObjectManager
*********************************************************************/
CObjectManager::~CObjectManager()
{
	for (int i = 0; i < m_Props.size(); ++i) delete m_Props[i];
	m_Props.erase(m_Props.begin(), m_Props.end());

	for (int i = 0; i < m_Projectiles.size(); ++i) delete m_Projectiles[i];
	m_Projectiles.erase(m_Projectiles.begin(), m_Projectiles.end());

	for (int i = 0; i < m_Players.size(); ++i) delete m_Players[i];
	m_Players.erase(m_Players.begin(), m_Players.end());

	//for (int i = 0; i < m_TextureList.size(); ++i) delete m_TextureList[i];
	//m_TextureList.erase(m_TextureList.begin(), m_TextureList.end());
}
void CObjectManager::Render()
{
	/*********************************************************************
	2019-06-16
	���� �ϱ� ���� ��ġ ��� ���� hlsl���� �� �� �ְ� map�� �ּҷ� ��������� ��.
	*********************************************************************/
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	CB_OBJECT_INFO	*pbMappedcbObject;


	/*********************************************************************
	2019-06-17
	�� ��ü�� Render�� Update �Լ����� IsAlive�� üũ�ϰ� �ֱ� ������ hlsl��
	������ ������Ʈ �ϴ� �κп��� IsAlive�� üũ�ϰ� �߰��Ͽ���.
	*********************************************************************/

	//XMVECTOR det;
	//XMMATRIX temp;

	for (int i = 0; i < m_nProps; i++) {
		if (!m_Props[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPropObjects + (i * ncbElementBytes));
		memset(pbMappedcbObject, NULL, ncbElementBytes);
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));

		//XMMatrixDeterminant(XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));
		//temp = XMMatrixInverse(&det, XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Props[i]->m_xmf4x4World)));


	}
	for (int i = 0; i < m_nPlayers; i++) {
		if (!m_Players[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPlayers + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Players[i]->m_xmf4x4World)));
		//XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&m_Players[i]->m_xmf4x4World));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Players[i]->m_xmf4x4World)));


	}
	for (int i = 0; i < m_nProjectiles; i++) {
		if (!m_Projectiles[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedProjectiles + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Projectiles[i]->m_xmf4x4World)));
		//XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&m_Projectiles[i]->m_xmf4x4World));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Projectiles[i]->m_xmf4x4World)));


	}

	m_pd3dCommandList->SetPipelineState(m_PSO[1]);
	m_Props[1]->Render(m_pd3dCommandList);
	//for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Render(m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(m_PSO[0]);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Render(m_pd3dCommandList);
}
void CObjectManager::Update(float fTime)
{
	//static float y = 0;
	//y += fTime;
	m_Props[1]->SetRotation(XMFLOAT3(0, fTime * 20, 0));

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Update(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Update(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Update(fTime);

	LateUpdate(fTime);
}
void CObjectManager::ProcessInput(UCHAR * pKeysBuffer, float mouse)
{
	if (m_Players[0]->IsAlive()) m_Players[0]->ProcessInput(pKeysBuffer, mouse);

	if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
		/*********************************************************************
		2019-06-17
		���콺�� Ŭ���ϸ� ����� �ϴ� �͵�.

		����, IsAlive == false�� ����ü�� ã�´�.
		ã������ �� ��ü�� �ʱ�ȭ�ϰ�(��ġ ����, ���� ���� ���)
		IsAlive = true�� �����Ѵ�.

		Render�� Update�� IsAlive�� true �� ���� �ǰ� �ؾ� �Ѵ�.
		*********************************************************************/
		if (dynamic_cast<CPlayer*>(m_Players[0])->IsShootable()) {
			dynamic_cast<CPlayer*>(m_Players[0])->Shoot();
			auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
			if (iter != m_Projectiles.end()) {
				dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[0]);
				(*iter)->SetAlive(true);
			}
		}
	}
}
void CObjectManager::GetColliderList(const Collider & myCollider, std::queue<Collider*>& myColliderQueue, ColliderTag targetTag)
{
	Collider* temp = NULL;
	switch (targetTag)
	{
	case ColliderTag::PROP: 
		for (int i = 0; i < m_Props.size(); ++i) {
			temp = m_Props[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) {
				myColliderQueue.push(temp);
			}
		}
		break;
	case ColliderTag::PROJECTILE: 
		for (int i = 0; i < m_Projectiles.size(); ++i) {
			temp = m_Projectiles[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) {
				myColliderQueue.push(temp);
			}
		}
		break;
	case ColliderTag::PLAYER:
		for (int i = 0; i < m_Players.size(); ++i) {
			temp = m_Players[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) {
				myColliderQueue.push(temp);
			}
		}
		break;
	default:break;
	}
}
Collider * CObjectManager::GetCollider(const Collider & myCollider, ColliderTag targetTag)
{
	Collider* temp = NULL;
	switch (targetTag)
	{
	case ColliderTag::PROP:
		for (int i = 0; i < m_Props.size(); ++i) {
			temp = m_Props[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) return temp;
		}
		break;
	case ColliderTag::PROJECTILE:
		for (int i = 0; i < m_Projectiles.size(); ++i) {
			temp = m_Projectiles[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) return temp;
		}
		break;
	case ColliderTag::PLAYER:
		for (int i = 0; i < m_Players.size(); ++i) {
			temp = m_Players[i]->GetCollisionCollider(myCollider);
			if (temp != NULL) return temp;
		}
		break;
	default:break;
	}
	return NULL;
}
void CObjectManager::LateUpdate(float fTime)
{
	//CollisionCheck();

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->LateUpdate(fTime);
}
void CObjectManager::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = m_nObjects + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects));
}
void CObjectManager::CreateConstantBufferResorce()
{
	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProps,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPropResource)	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nPlayers,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPlayersResource) m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProjectiles,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBProjectilesResource) m_pd3dCBProjectilesResource->Map(0, NULL, (void **)&m_pCBMappedProjectiles);
}
void CObjectManager::CreateConstantBufferView()
{
	UINT ncbElementBytes;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	d3dCBVDesc.SizeInBytes = ncbElementBytes;

	/*********************************************************************
	2019-06-17
	���� count�� �� �ʿ䰡 ���� �� ������??
	�� ���� �ּҴ� ��¥�� �ٷ� ������ �޾ƿ��ݾ�.
	*********************************************************************/
	if (nullptr != m_pd3dCBPropResource) {
		d3dGpuVirtualAddress = m_pd3dCBPropResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_nProps; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBPlayersResource) {
		d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_nPlayers; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + m_nProps));
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBProjectilesResource) {
		d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_nProjectiles; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + m_nProps + m_nPlayers));
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
}
void CObjectManager::CreateTextureResourceView(CTexture * pTexture)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextureType = pTexture->GetTextureType();
	ID3D12Resource *pShaderResource = pTexture->GetTexture();
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
	m_pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	pTexture->SetRootArgument(g_RootParameterTexture, d3dSrvGPUDescriptorHandle);
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}
void CObjectManager::CreateObjectData()
{
	/*********************************************************************
	2019-06-14
	������ �����ؾ� �Ѵ�. ���� �����Ϳ��� ������ ������ ��ġ ���� ���;� ��.
	������ �����ϱ� ���� ������.

	2019-07-01
	���� ���� �����Ͱ� �ִ�. �浹���� ������.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);

	importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);


	/*********************************************************************
	2019-06-15
	������ ���� �����ϱ� ���� �������� �����ش�. ������ ���Ƿ� ������ ���߿���
	m_nProps�� LevelData���� �о����, �������� Defines.h���� ������ ��.
	*********************************************************************/
	m_nProps = 2;
	m_nPlayers = 2;
	m_nProjectiles = m_nPlayers * g_nProjectilePerPlayer;
	m_nObjects = m_nProps + m_nPlayers + m_nProjectiles;

	CreateDescriptorHeap();
	/*********************************************************************
	2019-06-15
	�ؽ�ó�� ���⼭ �־�� �� �� ����. �ؽ�ó�� ���� �����д�.
	�ؽ�ó�� ������ �� ����� ���� �ؾ� �Ǵ°� �Ƴ�?

	2019-07-21
	�ؽ�ó�� ���� ���ͷ� �����ϱ� ����.
	*********************************************************************/

	g_vecTexture.clear();
	importer.ImportTexture(L"0615_Box_diff",			"Texture_PaperBox");
	importer.ImportTexture(L"character_2_diff_test3",	"Texture_Character");
	importer.ImportTexture(L"0618_LevelTest_diff",		"Texture_Level");
	importer.ImportTexture(L"box_diff",					"Texture_StandardBox");
	importer.ImportTexture(L"2B_diff",					"Texture_2B");
	for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);
	
	importer.ImportModel("0618_LevelTest",						"Texture_Level",		ImportType::DefaultMesh,	"Model_Level");
	importer.ImportModel("0725_Character",						"Texture_Character",	ImportType::DefaultMesh,	"Model_CharacterStatic", 2.0f);
	importer.ImportModel("0725_Character",						"Texture_Character",	ImportType::AnimatedMesh,	"Model_Character");
	importer.ImportModel("0725_PaperBox_NoSpitPerVertexNormal", "Texture_PaperBox",		ImportType::DefaultMesh,	"Model_PaperBox");
	importer.ImportModel("box",									"Texture_StandardBox",	ImportType::DefaultMesh,	"Model_Box1");
	importer.ImportModel("box2",								"Texture_StandardBox",	ImportType::DefaultMesh,	"Model_Box2");
	//importer.ImportModel("0723_Box_SN",						"Texture_PaperBox",		ImportType::DefaultMesh,	"Model_PaperBox_Resize", 0.5f);
	importer.ImportModel("2b",									"Texture_2B",			ImportType::DefaultMesh,	"Model_2B");

	importer.ImportAnimController("AnimCtrl_Character");

	importer.ImportAnimClip("0603_CharacterIdle",		"AnimCtrl_Character", true);
	importer.ImportAnimClip("0603_CharacterRun",		"AnimCtrl_Character", true);
	importer.ImportAnimClip("0603_CharacterFire",		"AnimCtrl_Character", false);
	importer.ImportAnimClip("0603_CharacterStartJump",	"AnimCtrl_Character", false);
	importer.ImportAnimClip("0603_CharacterEndJump",	"AnimCtrl_Character", false);
	importer.ImportAnimClip("0603_CharacterDied",		"AnimCtrl_Character", false);

	CreateConstantBufferResorce();
	CreateConstantBufferView();

	int count = 0;
	for (int i = 0; i < m_nProps; i++) {
		CObject* obj = new CObject();
		obj->SetMng(this);
		if (0 == i) {
			obj->AddModel(importer.GetModelByName("Model_Level_Box042"));
			obj->AddModel(importer.GetModelByName("Model_Level_Box045"));
			obj->AddModel(importer.GetModelByName("Model_Level_Box047"));
			for (int j = 0; j < m_LevelDataDesc.nCollisionMaps; ++j) {
				obj->AddCollider(
					XMFLOAT3(0, 0, 0),
					m_LevelDataDesc.CollisionScale[j],
					m_LevelDataDesc.CollisionRotation[j],
					m_LevelDataDesc.CollisionPosition[j],
					ColliderTag::PROP);
			}
		}
		else {
			//obj->AddModel(importer.GetModelByName("Model_Box2_Box001"));
			//obj->AddModel(importer.GetModelByName("Model_2B_body"));
			obj->AddModel(importer.GetModelByName("Model_CharacterStatic_body"));
			obj->AddModel(importer.GetModelByName("Model_CharacterStatic_jumper"));
			obj->AddModel(importer.GetModelByName("Model_CharacterStatic_mask"));
			obj->SetPosition(0.0f, 0.0f, 0.0f);
		}

		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Props.push_back(obj);
	}
	for (int i = 0; i < m_nPlayers; i++) {
		CPlayer* obj = new CPlayer();
		obj->SetMng(this);
		obj->AddModel(importer.GetModelByName("Model_Character_body"));
		obj->AddModel(importer.GetModelByName("Model_Character_jumper"));
		obj->AddModel(importer.GetModelByName("Model_Character_mask"));

		obj->SetAnimCtrl(importer.GetAnimCtrlByName("AnimCtrl_Character"));

		obj->SetPosition(0, 100, i * g_fDefaultUnitScale * 3);
		obj->SetSpawnPoint(obj->GetPosition());
		obj->AddCollider(
			XMFLOAT3(0, 0, 0),
			XMFLOAT3(5, 5, 5),
			XMFLOAT4(0, 0, 0, 1)
		);
		obj->SetCameraTargetOffset(XMFLOAT3(0, 47, -21));

		//XMFLOAT3 pos = obj->GetPosition();
		//obj->AddCollider(pos, XMFLOAT3(8, 2.577f, 8), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0,0,0), ColliderTag::GROUNDCHECK);
		//pos = obj->GetPosition();
		//pos.y += 16.807f;
		//obj->AddCollider(pos, XMFLOAT3(11.766f / 2, 16.807f / 2, 13.198f / 2), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0, 0, 0));

		//CModel *model = new CModel();
		//CTestMesh *mesh = new CTestMesh(m_pd3dDevice, m_pd3dCommandList, 10);
		//model->SetMesh(mesh);
		//model->SetTexture(m_TextureList[3]);
		//obj->AddModel(model);
		//obj->SetPosition(0, 100, 0);
		//obj->SetSpawnPoint(obj->GetPosition());
		//obj->AddCollider(
		//	XMFLOAT3(0, 0, 0),
		//	XMFLOAT3(5, 5, 5),
		//	XMFLOAT4(0, 0, 0, 1)
		//);
		//obj->SetCameraTargetOffset(XMFLOAT3(0, 0, 0));


		obj->SetTeam((i % 2) + 1);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Players.push_back(obj);
	}
	for (int i = 0; i < m_nProjectiles; i++) {
		CProjectile* obj = new CProjectile();
		obj->SetMng(this);
		obj->AddModel(importer.GetModelByName("Model_PaperBox_box_1"));

		obj->SetAlive(false);
		obj->AddCollider(XMFLOAT3(0,0,0), 10.0f, XMFLOAT3(0, 0, 0));
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Projectiles.push_back(obj);
	}
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

Collider::Collider()
{
}

/*********************************************************************
2019-07-05
�浹ü ��ü�� ���ʹϾ��� �������� ��.
��ü�� ȸ������ ���� ��ü�� ȸ������ �������� �ϴµ� �̰� ��� �����?
Quaternion ���ϴ� �Լ��� �ֳ�?
*********************************************************************/
Collider::Collider(XMFLOAT3 center, XMFLOAT3 extents, XMFLOAT4 quaternion, XMFLOAT3 offset, ColliderTag tag)
	: m_xmf3Offset(offset)
	, m_Box(BoundingOrientedBox(center, extents, quaternion))
	, m_Type(ColliderType::BOX)
	, m_Tag(tag)
{
	
}

Collider::Collider(XMFLOAT3 center, float radius, XMFLOAT3 offset, ColliderTag tag)
	: m_xmf3Offset(offset)
	, m_Type(ColliderType::SPHERE)
	, m_Sphere(BoundingSphere(center, radius))
	, m_Tag(tag)
{

}

void Collider::Update(XMFLOAT3 position, XMFLOAT4 rotation)
{
	SetPosition(position, GetRotatedOffset(rotation));
	SetRotation(rotation);
}

bool Collider::IsCollide(const Collider & other)
{
	/*********************************************************************
	2019-07-05
	���⸮�Ϲ��� ���� �ʹ�!

	if�� ���ʺ��� �˻��ϰ� �ƴϸ� �н����״ϱ� ������ �ص� ��������???
	*********************************************************************/
	if (m_Type == ColliderType::BOX) {
		if (other.m_Type == ColliderType::BOX)  return m_Box.Intersects(other.m_Box);
		else									return m_Box.Intersects(other.m_Sphere);
	}
	else {
		if (other.m_Type == ColliderType::BOX)  return m_Sphere.Intersects(other.m_Box);
		else									return m_Sphere.Intersects(other.m_Sphere);
	}
	return false;
}

void Collider::SetTag(const string tag)
{
	if ("Default" == tag)		{ m_Tag = ColliderTag::DEFAULT;		 return; }
	if ("GroundCheck" == tag)	{ m_Tag = ColliderTag::GROUNDCHECK;	 return; }
	if ("HeightCheck" == tag)	{ m_Tag = ColliderTag::HEIGHTCHECK;	 return; }
	if ("Hitbox" == tag)		{ m_Tag = ColliderTag::HITBOX;		 return; }
}

XMFLOAT3 Collider::GetLook()
{
	XMFLOAT3 look(0, 0, 1);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&look, XMVector3Rotate(XMLoadFloat3(&look), XMLoadFloat4(&m_Box.Orientation)));

	return look;
}

XMFLOAT3 Collider::GetUp()
{
	XMFLOAT3 up(0, 1, 0);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&up), XMLoadFloat4(&m_Box.Orientation)));

	return up;
}

XMFLOAT3 Collider::GetCenter()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Center;
	else								return m_Sphere.Center;
}

XMFLOAT3 Collider::GetExtents()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Extents;
	else return XMFLOAT3(0,0,0);
}

XMFLOAT3 GetBetweenVector(const Collider & A, const Collider & B)
{
	if (A.m_Type == ColliderType::BOX) {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center,	A.m_Box.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Box.Center);
	}
	else {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center,	A.m_Sphere.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Sphere.Center);
	}
	return XMFLOAT3(0, 0, 0);
}

XMFLOAT3 Collider::GetRotatedOffset(XMFLOAT4 rotation)
{
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Rotate(XMLoadFloat3(&m_xmf3Offset), XMLoadFloat4(&rotation)));
	return result;
}

void Collider::SetPosition(XMFLOAT3 position, XMFLOAT3 rotatedOffset)
{
	if (m_Type == ColliderType::BOX)	m_Box.Center	= Vector3::Add(position, rotatedOffset);
	else								m_Sphere.Center = Vector3::Add(position, rotatedOffset);
	
}

void Collider::SetRotation(XMFLOAT4 rotation)
{
	if (m_Type == ColliderType::BOX) 
		m_Box.Orientation = rotation;
	
}
