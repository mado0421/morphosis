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
	이동 파트
	*********************************************************************/


	/*********************************************************************
	2019-06-17
	애니메이션 파트
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
//	애니메이션 컨트롤러가 없으면 생성하고 추가해준다.
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
	other의 충돌체들과 내 충돌체들을 전부 비교해야 함.
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
	공격 타이머 파트
	*********************************************************************/
	m_fRemainingTimeOfFire -= fTimeElapsed;
	//SetHandMatrix();

	/*********************************************************************
	2019-06-17
	애니메이션 파트
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
	이동 파트

	2019-07-13
	이관되었습니다.
	*********************************************************************/
	// 중력 가속도 적용
	//if (m_IsGround) m_fHeightVelocity = 0.0f;
	//else m_fHeightVelocity -= fTimeElapsed /** g_Gravity*/;
	m_fHeightVelocity -= fTimeElapsed * g_Gravity;

	/*********************************************************************
	2019-07-15
	1. 이동 방향을 구하고 그 곳에 포괄적 충돌체와 정밀 충돌체 생성
	2. 포괄적 충돌체를 가지고 주변에 충돌하는 애들을 전부 찾음.
	3. 걔네들을 대상으로 디테일 충돌체로 검사

	//m_ComprehensiveGroundCollider	= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 2.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	//m_DetailedGroundCollider		= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 4.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	*********************************************************************/

	// Queue Clear
	std::queue<Collider*> empty;
	std::swap(m_CollideInfo, empty);

	// 진행 방향 구함
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
		여기까지 잘 됨
		- MyExtents와 Collider의 Extents를 비교
		한 번에 한 면만 충돌하거나(빗면), 두 면이 충돌하거나(가장자리), 세 면이 충돌할 수 있음(모서리)
		검사 6번을 하면

		2019-07-18
		현재 내 위치를 기준으로 구하게 변경하였습니다~

		절대값을 구해서 MyExtents가 Extents보다 커야 함.
		abs(MyExtent) - Extent 한 값이 가장 큰 면이 충돌면.
		MyExtent의 부호에 따라 충돌면의 방향을 정함.

		기적 같이 같을 수 있다. 그럼 뭐~걍 먼저 나온 면으로 해. 중요한 것도 아니고

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


		// 지면 판정
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













	//bool alreadyRestore = false;	// 이동을 무효 처리 했는지 여부
	//while (!m_CollideInfo.empty()) {
	//	if (!alreadyRestore && ObjectTag::Prop == m_CollideInfo.front()->m_Tag) alreadyRestore = RestorePosition();
	//	if (!alreadyRestore && ObjectTag::Player == m_CollideInfo.front()->m_Tag) alreadyRestore = RestorePosition();	
	//	if (ObjectTag::Projectile == m_CollideInfo.front()->m_Tag) dynamic_cast<CProjectile*>(m_CollideInfo.front())->Damage(this);
	//	
	//	m_CollideInfo.pop();
	//}



	/*********************************************************************
	2019-06-18
	충돌체 파트
	
	2019-07-06
	충돌체를 통합해서 관리하며 업데이트 부분 변경
	
	아니 저걸 그냥 LateUpdate()에서 해주자.
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
	위치를 초기화시킬 때, 플레이어의 위치에 오프셋만큼 더해서(상대좌표 개념) 옮겨준다.

	2019-07-06
	충돌체 위치 원래 변경 해줬었는데 이제 Update()에서 자동으로 변경할 예정이므로 삭제함.

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
	여기서 해줘야 하는 것은
	인자로 받아온 오브젝트로부터 위치와 방향, 팀을 받아야 한다는 것.

	아 투사체가 회전하면서 내려와야 하면 흠. 그러면 흠. 방향을 흠.

	충돌체 위치도 바꿔줘야 함.
	*********************************************************************/
	XMFLOAT3 pos = obj->GetPosition();
	pos.y += 20;
	SetPosition(pos);
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());


	/*********************************************************************
	2019-06-17
	렌더링할 메쉬의 방향
	*********************************************************************/
	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction		= Vector3::Normalize(obj->GetLook());
	m_fLifeTime			= g_DefaultProjectileLifeTime;


	/*********************************************************************
	2019-06-18
	손의 행렬을 얻어왔다. 그럼 그 행렬로부터 위치랑 방향을 뽑아내면?

	지금은 미완성. 위에걸로 잠시 쓰자.
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
	LifeTime 관리
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
	충돌체 파트
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
	렌더 하기 전에 위치 행렬 등을 hlsl에서 쓸 수 있게 map한 주소로 복사해줘야 함.
	*********************************************************************/
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	CB_OBJECT_INFO	*pbMappedcbObject;


	/*********************************************************************
	2019-06-17
	각 객체의 Render와 Update 함수에서 IsAlive를 체크하고 있기 때문에 hlsl의
	정보를 업데이트 하는 부분에만 IsAlive를 체크하게 추가하였다.
	*********************************************************************/
	for (int i = 0; i < m_nProps; i++) {
		if (!m_Props[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPropObjects + (i * ncbElementBytes));
		memset(pbMappedcbObject, NULL, ncbElementBytes);
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));
	}
	for (int i = 0; i < m_nPlayers; i++) {
		if (!m_Players[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPlayers + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Players[i]->m_xmf4x4World)));
	}
	for (int i = 0; i < m_nProjectiles; i++) {
		if (!m_Projectiles[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedProjectiles + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Projectiles[i]->m_xmf4x4World)));
	}

	m_pd3dCommandList->SetPipelineState(m_PSO[1]);
	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Render(m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(m_PSO[0]);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Render(m_pd3dCommandList);
}
void CObjectManager::Update(float fTime)
{
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
		마우스를 클릭하면 해줘야 하는 것들.

		먼저, IsAlive == false인 투사체를 찾는다.
		찾았으면 그 객체를 초기화하고(위치 변경, 방향 변경 등등)
		IsAlive = true로 변경한다.

		Render와 Update는 IsAlive가 true 일 때만 되게 해야 한다.
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
	굳이 count로 할 필요가 없을 거 같은데??
	저 가상 주소는 어짜피 바로 직전에 받아오잖아.
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
	프롭을 생성해야 한다. 레벨 데이터에서 프롭의 종류와 위치 등을 빼와야 함.
	지금은 없으니까 대충 만들자.

	2019-07-01
	이젠 레벨 데이터가 있다. 충돌맵을 만들자.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);

	importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);


	/*********************************************************************
	2019-06-15
	서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
	m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
	*********************************************************************/
	m_nProps = 2;
	m_nPlayers = 1;
	m_nProjectiles = m_nPlayers * g_nProjectilePerPlayer;
	m_nObjects = m_nProps + m_nPlayers + m_nProjectiles;

	CreateDescriptorHeap();
	/*********************************************************************
	2019-06-15
	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?

	2019-07-21
	텍스처를 전역 벡터로 관리하기 시작.
	*********************************************************************/

	g_vecTexture.clear();
	importer.ImportTexture(L"0615_Box_diff",			"Texture_PaperBox");
	importer.ImportTexture(L"character_2_diff_test3",	"Texture_Character");
	importer.ImportTexture(L"0618_LevelTest_diff",		"Texture_Level");
	importer.ImportTexture(L"box_diff",					"Texture_StandardBox");
	for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);
	
	importer.ImportModel("0618_LevelTest",		"Texture_Level",		ImportType::DefaultMesh,	"Model_Level");
	importer.ImportModel("0603_CharacterIdle",	"Texture_Character",	ImportType::AnimatedMesh,	"Model_Character");
	importer.ImportModel("0615_Box",			"Texture_PaperBox",		ImportType::DefaultMesh,	"Model_PaperBox");
	importer.ImportModel("0615_Box",			"Texture_PaperBox",		ImportType::DefaultMesh,	"Model_PaperBox_Resize", 5.0f);

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
			obj->AddModel(importer.GetModelByName("Model_PaperBox_Resize_box_1"));
			obj->SetPosition(0.0f, 50.0f, 0.0f);
		}

		//CModel *model = new CModel();
		//CTestMesh *mesh = new CTestMesh(m_pd3dDevice, m_pd3dCommandList, 100);
		//model->SetMesh(mesh);
		//model->SetTexture(m_TextureList[3]);
		//obj->AddModel(model);
		//obj->SetPosition(0, -200.0f * i, -100.0f * i);
		//obj->AddCollider(
		//	XMFLOAT3(0, 0, 0),
		//	XMFLOAT3(50, 50, 50),
		//	XMFLOAT4(0, 0, 0, 1)
		//);
		//if(i == 0)	obj->SetRotation(XMFLOAT3(-60, 0, 0));

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
//	충돌검사 표
//	┼──────────┼────────────┼────────────┼────────────┼
//	│		   │    Prop    │   Player   │ Projectile │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Prop	   │			│	  O		 │	   O	  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Player	   │	 O	    │	  O		 │	   O	  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Projectile│	 O	    │	  O		 │			  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	디비전처럼 날아오는 수류탄을 총으로 쏴서 맞추고 할 거면 뭐...
//
//	플레이어로 다 돌리고 투사체로 프롭이랑 하면 될 것 같다.
//	충돌하면? 여기서 처리 다 해?
//
//	충돋했을 때 처리들:
//	- 플레이어가 프롭과 충돌하면 이동 무효. (슬라이드는 나중에 구현하기)
//	- 플레이어가 플레이어와 충돌하면 이동 무효. (서로 밀치기는 나중에 구현하기)
//	- 플레이어가 프로젝타일과 충돌하면 프로젝타일의 효과를 플레이어에게 적용.
//	- 프로젝타일이 프롭과 충돌하면 프로젝타일 비활성화.
//
//	이동 무효를 하려면 플레이어의 트리거를 꺼버리면 되나?
//	근데 충돌체크는 이동 후에 검사해야 하는데?
//	그럼 LateUpdate()를 만들자! 충돌검사를 하고 충돌했으면 각 객체에게 트리거를 켜주고
//	LateUpdate()를 하게 하기.
//	누구랑 충돌했는지 그냥 전달해주기로 함.
//	*********************************************************************/
//	/*
//	for (auto player = m_Players.begin(); player != m_Players.end(); ++player) {
//		// 활성화된 애만 처리할 것.
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
//		// 활성화된 애만 처리할 것.
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
충돌체 자체의 쿼터니언은 정해져야 함.
자체의 회전각과 주인 객체의 회전각이 더해져야 하는데 이건 어떻게 만들거?
Quaternion 더하는 함수가 있나?
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
	조기리턴문을 쓰고 싶다!

	if문 왼쪽부터 검사하고 아니면 패스할테니까 저렇게 해도 괜찮겠지???
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
