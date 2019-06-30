#include "stdafx.h"
#include "Object.h"
#include "AnimationController.h"
#include "Mesh.h"
#include "Importer.h"

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
	m_Tag					= Tag::Prop;
}
CObject::~CObject()
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
void CObject::AddAnimClip(AnimationClip * animClip)
{
	/*********************************************************************
	2019-06-16
	애니메이션 컨트롤러가 없으면 생성하고 추가해준다.
	*********************************************************************/
	if (NULL == m_AnimationController)
		m_AnimationController = new CAnimationController();
	m_AnimationController->AddAnimData(animClip);
}
void CObject::AddCollider(BoundingOrientedBox box)
{
	m_CollisionBox.push_back(CollisionBox(box, box.Center));
}
void CObject::AddCollider(BoundingSphere sphere)
{
	m_CollisionSphere.push_back(CollisionSphere(sphere, sphere.Center));
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
XMFLOAT3 const CObject::GetCameraTargetPos()
{
	return Vector3::Add(GetPosition(), m_xmf3CameraTargetOffset);
	//return m_xmf3CameraTargetOffset;
}
const bool CObject::IsCollide(CObject* other)
{
	/*********************************************************************
	2019-06-18
	other의 충돌체들과 내 충돌체들을 전부 비교해야 함.
	*********************************************************************/
	for (auto otb = other->m_CollisionBox.cbegin(); otb != other->m_CollisionBox.cend(); ++otb) {
		for (auto myb = m_CollisionBox.cbegin(); myb != m_CollisionBox.cend(); ++myb)		if (otb->collisionBox.Intersects(myb->collisionBox)) return true;
		for (auto mys = m_CollisionSphere.cbegin(); mys != m_CollisionSphere.cend(); ++mys) if (otb->collisionBox.Intersects(mys->collisionSphere)) return true;
	}
	for (auto ots = other->m_CollisionSphere.cbegin(); ots != other->m_CollisionSphere.cend(); ++ots) {
		for (auto myb = m_CollisionBox.cbegin(); myb != m_CollisionBox.cend(); ++myb)		if (ots->collisionSphere.Intersects(myb->collisionBox)) return true;
		for (auto mys = m_CollisionSphere.cbegin(); mys != m_CollisionSphere.cend(); ++mys) if (ots->collisionSphere.Intersects(mys->collisionSphere)) return true;
	}

	return false;
}
void CObject::ProcessInput(UCHAR * pKeysBuffer)
{
}
void CObject::AddCollideInfo(CObject * obj)
{
	m_CollideInfo.push(obj);
}

/*********************************************************************
2019-06-17
CPlayer
*********************************************************************/
CPlayer::CPlayer() : CObject()
{
	m_fSpeed					= g_PlayerDefaultSpeed;
	m_fRemainingTimeOfFire		= 0.0f;
	m_fRPM						= (1 / static_cast<float>(g_DefaultRPM)) * 60.0f;
	m_Tag						= Tag::Player;
	m_xmf3Move					= XMFLOAT3(0, 0, 0);
	m_xmf3SpawnPoint			= GetPosition();
	m_HealthPoint				= g_DefaultHealthPoint;
	m_fRemainingTimeOfRespawn	= 0.0f;
	m_xmf4x4Hand				= Matrix4x4::Identity();
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
	SetHandMatrix();

	/*********************************************************************
	2019-06-17
	이동 파트
	*********************************************************************/
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	m_xmf3Move = Move(fTimeElapsed);

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

	/*********************************************************************
	2019-06-18
	충돌체 파트
	*********************************************************************/
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->collisionBox.Center = Vector3::Add(myb->collisionBox.Center, m_xmf3Move);
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->collisionSphere.Center = Vector3::Add(mys->collisionSphere.Center, m_xmf3Move);

	/*********************************************************************
	2019-06-17
	애니메이션 파트
	*********************************************************************/
	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

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




	//if (static_cast<int>(AnimationState::IDLE) == m_AnimationState) {
	//	if (IsMoving()) {
	//		m_AnimationState = static_cast<int>(AnimationState::RUN);
	//		m_AnimationController->ChangeAnimClip("PlayerRun");
	//	}
	//}
	//else {
	//	if (!IsMoving()) {
	//		m_AnimationState = static_cast<int>(AnimationState::IDLE);
	//		m_AnimationController->ChangeAnimClip("PlayerIdle");
	//	}
	//}

	TriggerOff();
}
void CPlayer::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;

	bool alreadyRestore = false;	// 이동을 무효 처리 했는지 여부
	while (!m_CollideInfo.empty()) {
		if (!alreadyRestore && Tag::Prop == m_CollideInfo.front()->m_Tag) {
			alreadyRestore = true;
			m_xmf4x4World._41 -= m_xmf3Move.x;
			m_xmf4x4World._42 -= m_xmf3Move.y;
			m_xmf4x4World._43 -= m_xmf3Move.z;
			for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
				myb->collisionBox.Center = Vector3::Subtract(myb->collisionBox.Center, m_xmf3Move);
			for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
				mys->collisionSphere.Center = Vector3::Subtract(mys->collisionSphere.Center, m_xmf3Move);
		}
		if (!alreadyRestore && Tag::Player == m_CollideInfo.front()->m_Tag) {
			alreadyRestore = true;
			m_xmf4x4World._41 -= m_xmf3Move.x;
			m_xmf4x4World._42 -= m_xmf3Move.y;
			m_xmf4x4World._43 -= m_xmf3Move.z;
			for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
				myb->collisionBox.Center = Vector3::Subtract(myb->collisionBox.Center, m_xmf3Move);
			for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
				mys->collisionSphere.Center = Vector3::Subtract(mys->collisionSphere.Center, m_xmf3Move);
		}
		if (Tag::Projectile == m_CollideInfo.front()->m_Tag) {
			dynamic_cast<CProjectile*>(m_CollideInfo.front())->Damage(this);
		}

		m_CollideInfo.pop();
	}

	if (m_HealthPoint <= 0) CPlayer::Disable();

}
void CPlayer::ProcessInput(UCHAR * pKeysBuffer)
{
	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[static_cast<int>(Move::W)] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[static_cast<int>(Move::A)] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[static_cast<int>(Move::S)] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[static_cast<int>(Move::D)] = true;
	if (pKeysBuffer[KEY::Q] & 0xF0) m_trigInput[static_cast<int>(Move::Q)] = true;
	if (pKeysBuffer[KEY::E] & 0xF0) m_trigInput[static_cast<int>(Move::E)] = true;
}
void CPlayer::Enable()
{
	m_HealthPoint = g_DefaultHealthPoint;
	SetPosition(m_xmf3SpawnPoint);
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->collisionBox.Center = myb->InitPosition;
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->collisionSphere.Center = mys->InitPosition;
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
	m_AnimationController->ChangeAnimClip("PlayerFire");
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
void CPlayer::TriggerOff()
{
	for (int i = 0; i < static_cast<int>(Move::count); i++) m_trigInput[i] = false;
}
XMFLOAT3 CPlayer::Move(float fTimeElapsed)
{
	XMFLOAT3 temp(0, 0, 0);
	if (m_trigInput[static_cast<int>(Move::W)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetLook())); }
	if (m_trigInput[static_cast<int>(Move::A)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetRight()))); }
	if (m_trigInput[static_cast<int>(Move::S)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetLook()))); }
	if (m_trigInput[static_cast<int>(Move::D)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetRight())); }
	temp = Vector3::Normalize(temp);
	return Vector3::Multiply(fTimeElapsed * m_fSpeed, temp);
}
float CPlayer::Rotate(float fTimeElapsed)
{
	float temp = 0;
	if (m_trigInput[static_cast<int>(Move::Q)]) temp += fTimeElapsed * m_fSpeed * -1;
	if (m_trigInput[static_cast<int>(Move::E)]) temp += fTimeElapsed * m_fSpeed;

	return temp;
}
void CPlayer::SetHandMatrix()
{
	m_xmf4x4Hand = m_AnimationController->GetPositionOfBone("Bip001 L Hand");
}

/*********************************************************************
2019-06-17
CProjectile
*********************************************************************/
CProjectile::CProjectile() : CObject()
{
	m_IsAlive		= false;
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	m_fSpeed		= g_ProjectileDefaultSpeed;
	m_fLifeTime		= g_DefaultProjectileLifeTime;
	m_Tag			= Tag::Projectile;
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
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->collisionBox.Center = pos;
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->collisionSphere.Center = pos;

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
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->collisionBox.Center = Vector3::Add(myb->collisionBox.Center, xmf3Move);
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->collisionSphere.Center = Vector3::Add(mys->collisionSphere.Center, xmf3Move);
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
	delete[] &m_pObjects;
	delete[] &m_pTexture;
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
	/*********************************************************************
	2019-06-29
	포인터 배열로 변경함
	*********************************************************************/
	for (int i = g_IdxProps; i < g_IdxPlayers; ++i) {
		if (!m_pObjects[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPropObjects + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pObjects[i]->m_xmf4x4World)));
	}
	for (int i = g_IdxPlayers; i < g_IdxProjectiles; ++i) {
		if (!m_pObjects[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPlayers + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pObjects[i]->m_xmf4x4World)));
	}
	for (int i = g_IdxProjectiles; i < g_NumObjects; ++i) {
		if (!m_pObjects[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedProjectiles + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pObjects[i]->m_xmf4x4World)));
	}

	m_pd3dCommandList->SetPipelineState(g_PipelineStates[0]);
	for (int i = g_IdxPlayers; i < g_IdxProjectiles; ++i)	m_pObjects[i]->Render(m_pd3dCommandList);
	for (int i = g_IdxProjectiles; i < g_NumObjects; ++i)	m_pObjects[i]->Render(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(g_PipelineStates[1]);
	for (int i = g_IdxProps; i < g_IdxPlayers; ++i)			m_pObjects[i]->Render(m_pd3dCommandList);
}
void CObjectManager::Update(float fTime)
{
	for (int i = g_IdxPlayers; i < g_NumObjects; ++i) m_pObjects[i]->Update(fTime);

	LateUpdate(fTime);
}
void CObjectManager::ProcessInput(UCHAR * pKeysBuffer)
{
	if (m_pObjects[g_IdxPlayers]->IsAlive()) m_pObjects[g_IdxPlayers]->ProcessInput(pKeysBuffer);

	if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
		/*********************************************************************
		2019-06-17
		마우스를 클릭하면 해줘야 하는 것들.

		먼저, IsAlive == false인 투사체를 찾는다.
		찾았으면 그 객체를 초기화하고(위치 변경, 방향 변경 등등)
		IsAlive = true로 변경한다.

		Render와 Update는 IsAlive가 true 일 때만 되게 해야 한다.
		*********************************************************************/
		if (dynamic_cast<CPlayer*>(m_pObjects[g_IdxPlayers])->IsShootable()) {
			dynamic_cast<CPlayer*>(m_pObjects[g_IdxPlayers])->Shoot();

			for (int i = g_IdxProjectiles; i < g_NumObjects; ++i) {
				if (m_pObjects[i]->IsAlive()) {
					dynamic_cast<CProjectile*>(m_pObjects[i])->Initialize(m_pObjects[g_IdxPlayers]);
					m_pObjects[i]->SetAlive(true);
				}
			}
		}
	}
}
void CObjectManager::LateUpdate(float fTime)
{
	CollisionCheck();

	for (int i = g_IdxPlayers; i < g_NumObjects; ++i) m_pObjects[i]->LateUpdate(fTime);
}
void CObjectManager::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = g_NumObjects + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (g_NumObjects));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (g_NumObjects));
}
void CObjectManager::CreateConstantBufferResorce()
{
	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * g_NumProps,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPropResource)	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * g_NumPlayers,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPlayersResource) m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * g_NumProjectiles,
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
		for (unsigned int i = 0; i < g_NumProps; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBPlayersResource) {
		d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
		for (unsigned int i = 0; i < g_NumPlayers; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + g_IdxPlayers));
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBProjectilesResource) {
		d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
		for (unsigned int i = 0; i < g_NumProjectiles; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + g_IdxProjectiles));
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
	*********************************************************************/
	/*********************************************************************
	2019-06-15
	서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
	m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);

	CreateDescriptorHeap();
	/*********************************************************************
	2019-06-15
	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?
	*********************************************************************/
	wstring fileNames[g_NumTextures];
	fileNames[0] = LASSETPATH;
	fileNames[0] += L"0615_Box_diff.dds";
	fileNames[1] = LASSETPATH;
	fileNames[1] += L"character_2_diff_test3.dds";
	fileNames[2] = LASSETPATH;
	fileNames[2] += L"0618_LevelTest_diff.dds";
	for (int i = 0; i < g_NumTextures; ++i) {
		CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
		texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, fileNames[i].c_str());
		CreateTextureResourceView(texture);
		m_pTexture[i] = texture;
	}

	/*********************************************************************
	2019-06-15
	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?
	*********************************************************************/
	CreateConstantBufferResorce();
	CreateConstantBufferView();


	/*********************************************************************
	2019-06-29
	오브젝트 풀을 미리 만들어 놔야 한다.
	되나 그게? 일단 전부 생성하고 초기화하고 하여간.

	처음부터 프롭 개수만큼							g_IdxProps ~ g_IdxPlayers
	프롭 개수(==플레이어 시작 수)부터 플레이어 개수만큼 g_IdxPlayers ~ g_IdxProjectiles
	투사체 시작 수부터 투사체 개수만큼					g_IdxProjectiles ~ g_NumObjects
	*********************************************************************/
	for (int i = g_IdxProps; i < g_IdxPlayers; ++i) {
		CObject* obj = new CObject();
		importer.ImportModel("0615_Box", m_pTexture[0], obj);
		obj->SetPosition(0, 0, i * 64.0f);
		obj->AddCollider(BoundingOrientedBox(obj->GetPosition(), XMFLOAT3(9.69f / 2, 6.689f / 2, 5.122f / 2), XMFLOAT4(0, 0, 0, 1)));
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
		m_pObjects[i] = obj;
	}
	for (int i = g_IdxPlayers; i < g_IdxProjectiles; ++i) {
		CPlayer* obj = new CPlayer();
		importer.ImportModel("0603_CharacterIdle", m_pTexture[1], obj);
		importer.ImportAnimClip("0603_CharacterIdle", obj);
		importer.ImportAnimClip("0603_CharacterRun", obj);
		importer.ImportAnimClip("0603_CharacterFire", obj);
		importer.ImportAnimClip("0603_CharacterStartJump", obj);
		importer.ImportAnimClip("0603_CharacterEndJump", obj);
		importer.ImportAnimClip("0603_CharacterDied", obj);
		obj->SetPosition(100, 0, (i - g_IdxPlayers) * g_DefaultUnitStandard * 3);
		obj->SetSpawnPoint(obj->GetPosition());
		XMFLOAT3 pos = obj->GetPosition();
		pos.y += 2.577f;
		obj->AddCollider(BoundingOrientedBox(pos, XMFLOAT3(16 / 2, 16 / 2, 5.122f / 2), XMFLOAT4(0, 0, 0, 1)));
		pos = obj->GetPosition();
		pos.y += 18.404f;
		obj->AddCollider(BoundingOrientedBox(pos, XMFLOAT3(11.766f / 2, 13.198f / 2, 36.807f / 2), XMFLOAT4(0, 0, 0, 1)));
		obj->SetTeam(((i - g_IdxPlayers) % 2) + 1);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
		obj->SetCameraTargetOffset(XMFLOAT3(0, 47, -21));
		m_pObjects[i] = obj;
	}
	for (int i = g_IdxProjectiles; i < g_NumObjects; ++i) {
		CProjectile* obj = new CProjectile();
		/*********************************************************************
		2019-06-17
		투사체는 전부 미리 만들어두고 IsAlive를 false로 해둔다.
		*********************************************************************/
		importer.ImportModel("0615_Box", m_pTexture[0], obj);
		obj->SetAlive(false);
		obj->AddCollider(BoundingSphere(XMFLOAT3(0, 0, 0), 10.0f));
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * i);
		m_pObjects[i] = obj;

	}
}
void CObjectManager::CollisionCheck()
{
	/*********************************************************************
	2019-06-18
	충돌검사 표
	┼──────────┼────────────┼────────────┼────────────┼
	│		   │    Prop    │   Player   │ Projectile │
	┼──────────┼────────────┼────────────┼────────────┼
	│Prop	   │			│	  O		 │	   O	  │
	┼──────────┼────────────┼────────────┼────────────┼
	│Player	   │	 O	    │	  O		 │	   O	  │
	┼──────────┼────────────┼────────────┼────────────┼
	│Projectile│	 O	    │	  O		 │			  │
	┼──────────┼────────────┼────────────┼────────────┼
	디비전처럼 날아오는 수류탄을 총으로 쏴서 맞추고 할 거면 뭐...

	플레이어로 다 돌리고 투사체로 프롭이랑 하면 될 것 같다.
	충돌하면? 여기서 처리 다 해?

	충돋했을 때 처리들:
	- 플레이어가 프롭과 충돌하면 이동 무효. (슬라이드는 나중에 구현하기)
	- 플레이어가 플레이어와 충돌하면 이동 무효. (서로 밀치기는 나중에 구현하기)
	- 플레이어가 프로젝타일과 충돌하면 프로젝타일의 효과를 플레이어에게 적용.
	- 프로젝타일이 프롭과 충돌하면 프로젝타일 비활성화.

	이동 무효를 하려면 플레이어의 트리거를 꺼버리면 되나?
	근데 충돌체크는 이동 후에 검사해야 하는데?
	그럼 LateUpdate()를 만들자! 충돌검사를 하고 충돌했으면 각 객체에게 트리거를 켜주고
	LateUpdate()를 하게 하기.
	누구랑 충돌했는지 그냥 전달해주기로 함.
	*********************************************************************/
	/*********************************************************************
	2019-06-29
	포인터 배열로 바꾸고 다시 작성함.
	*********************************************************************/
	for (int i = g_IdxPlayers; i < g_IdxProjectiles; ++i) {
		if (m_pObjects[i]->IsAlive()) {
			for (int j = g_IdxProps; j < g_IdxPlayers; ++j) {
				if (IsCollidable(m_pObjects[i], m_pObjects[j])) {
					if (m_pObjects[i]->IsCollide(m_pObjects[j]))
						m_pObjects[i]->AddCollideInfo(m_pObjects[j]);
				}
			}

			for (int j = g_IdxPlayers; j < g_IdxProjectiles; ++j) {
				if (IsCollidable(m_pObjects[i], m_pObjects[j])) {
					if (m_pObjects[i]->IsCollide(m_pObjects[j])) {
						m_pObjects[i]->AddCollideInfo(m_pObjects[j]);
						m_pObjects[j]->AddCollideInfo(m_pObjects[i]);
					}
				}
			}

			for (int j = g_IdxProjectiles; j < g_NumObjects; ++j) {
				if (IsCollidable(m_pObjects[i], m_pObjects[j])) {
					if (m_pObjects[i]->IsCollide(m_pObjects[j])) {
						m_pObjects[i]->AddCollideInfo(m_pObjects[j]);
						m_pObjects[j]->Disable();
					}
				}
			}
		}
	}

	for (int i = g_IdxProjectiles; i < g_NumObjects; ++i) {
		if (m_pObjects[i]->IsAlive()) {
			for (int j = g_IdxProps; j < g_IdxPlayers; ++j) {
				if (IsCollidable(m_pObjects[i], m_pObjects[j])) {
					if (m_pObjects[i]->IsCollide(m_pObjects[j]))
						m_pObjects[i]->Disable();
					break;
				}
			}
		}
	}
}
