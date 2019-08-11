#include "stdafx.h"
#include "Player.h"
#include "AnimationController.h"
#include "AI.h"
#include "Collider.h"
#include "ObjectManager.h"

/*********************************************************************
2019-06-17
CPlayer
*********************************************************************/
CPlayer::CPlayer() : CObject()
{

	m_fSpeed = g_fDefaultPlayerSpeed;
	m_fRemainingTimeOfFire = 0.0f;
	m_fRPM = (1 / static_cast<float>(g_DefaultRPM)) * 60.0f;
	m_xmf3Move = XMFLOAT3(0, 0, 0);
	m_xmf3SpawnPoint = GetPosition();
	m_HealthPoint = g_DefaultHealthPoint;
	m_fRemainingTimeOfRespawn = 0.0f;
	m_rotationInput = XMFLOAT2(0,0);
	for (int i = 0; i < static_cast<int>(Move::count); ++i) m_trigInput[i] = false;
	m_AIBrain = new CTinMan(new CMoveBehavior(), this);
}
CPlayer::~CPlayer()
{
	delete m_AIBrain;
}
void CPlayer::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (-1 != m_AnimCtrlIdx) {

		UINT ncbElementBytes = (((sizeof(XMMATRIX) * g_nAnimBone) + 255) & ~255); //256의 배수
		m_pd3dcbAnimation = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_pd3dcbAnimation->Map(0, NULL, (void **)&m_pcbxmAnimation);
	}
}
void CPlayer::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_pd3dcbAnimation)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbAnimation->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(g_RootParameterAnimation, d3dcbBoneOffsetsGpuVirtualAddress);

		for (int i = 0; i < g_nAnimBone; i++)
		{
			m_pcbxmAnimation[i] = m_a[i];
		}
	}
}
void CPlayer::Update(float fTimeElapsed)
{
	/* 죽은 상태에서 해줘야 하는 것:
		- 리스폰까지 남은 시간을 흐른 시간만큼 감소
		- 만약 리스폰까지 남은 시간이 0보다 작다면 플레이어를 다시 소생*/
	if (!m_IsAlive) {
		m_fRemainingTimeOfRespawn -= fTimeElapsed;
		if (m_fRemainingTimeOfRespawn <= 0.0f) CPlayer::Enable();
		return;
	}

	m_AIBrain->Update();


	/* 안 죽은 상태에서 해줘야 하는 것:
		- 공격가능까지 남은 시간을 흐른 시간만큼 감소
		- AnimTime을 흐른 시간만큼 증가
		- AnimState를 조건에 따라 관리*/
	if (m_fRemainingTimeOfFire > 0) m_fRemainingTimeOfFire -= fTimeElapsed;
	if (m_fRemainingTimeOfSkill1 > 0) m_fRemainingTimeOfSkill1 -= fTimeElapsed;
	if (m_fRemainingTimeOfSlow > 0) m_fRemainingTimeOfSlow -= fTimeElapsed;
	else m_fSlowFactor = 1.0f;

	if (-1 != m_AnimCtrlIdx) {
		m_AnimationTime += fTimeElapsed;

		if (static_cast<int>(AnimationState::IDLE) == m_AnimationState) {
			if (IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::RUN);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::RUN) == m_AnimationState) {
			if (!IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::FIRE) == m_AnimationState) {
			//m_AnimationController->ChangeAnimClip("PlayerFire");
			if (g_vecAnimCtrl[m_AnimCtrlIdx]->IsClipEnd(m_AnimationTime)) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::ENDJUMP) == m_AnimationState) {
			//m_AnimationController->ChangeAnimClip("PlayerFire");
			if (g_vecAnimCtrl[m_AnimCtrlIdx]->IsClipEnd(m_AnimationTime)) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
	}
}
void CPlayer::LateUpdate(float fTimeElapsed)
{
	bool Falling = false;
	if (!m_IsAlive) return;

	m_fHeightVelocity -= fTimeElapsed * g_Gravity;
	if (m_fHeightVelocity < -1.5) {
		m_fHeightVelocity = -1.5;
		Falling = true;
	}

	//if(m_trigInput[static_cast<int>(Move::SPACE)]) m_fHeightVelocity += 
	if (m_IsOnGround && m_trigInput[static_cast<int>(Move::SPACE)]) {
		m_AnimationState = static_cast<int>(AnimationState::STARTJUMP);
		m_AnimationTime = 0;

		m_fHeightVelocity = 3;
	}
	m_IsOnGround = false;

	// 진행 방향 구함
	m_xmf3Move = Move(fTimeElapsed);
	float radius = g_fDefaultUnitScale / 2.0f;

	/* For Test */
	//static std::vector< Test > t;
	//int s = 0;


	while (true) {
		Collider DetailedGroundCollider(XMFLOAT3(0, 0, 0), radius);
		DetailedGroundCollider.Update(Vector3::Add(GetPosition(), m_xmf3Move), XMFLOAT4(0, 0, 0, 1));
		Collider* collider = m_pObjMng->GetCollider(DetailedGroundCollider, ColliderTag::PROP, false);
		if (NULL == collider) break;

		//s++;
		//if (s > 10) collider->m_trigCollided = true;
		//Test a;
		//a.col = collider;
		//a.prevMove = m_xmf3Move;

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

		XMFLOAT3 colliderExtents = collider->GetExtents();
		XMFLOAT3 dir(0, 0, 0);

		//if (xmf3MyExtents.x >= 0) { if (IsIn(xmf3MyExtents.x, colliderExtents.x, colliderExtents.x + radius)) dir = Vector3::Add(dir, Vector3::Normalize(right)); }
		//else { if (IsIn(xmf3MyExtents.x, -1 * colliderExtents.x - radius, -1 * colliderExtents.x)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, right))); }
		//if (xmf3MyExtents.y >= 0) { if (IsIn(xmf3MyExtents.y, colliderExtents.y, colliderExtents.y + radius)) dir = Vector3::Add(dir, Vector3::Normalize(up)); }
		//else { if (IsIn(xmf3MyExtents.y, -1 * colliderExtents.y - radius, -1 * colliderExtents.y)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, up))); }
		//if (xmf3MyExtents.z >= 0) { if (IsIn(xmf3MyExtents.z, colliderExtents.z, colliderExtents.z + radius)) dir = Vector3::Add(dir, Vector3::Normalize(look)); }
		//else { if (IsIn(xmf3MyExtents.z, -1 * colliderExtents.z - radius, -1 * colliderExtents.z)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, look))); }


		//if (xmf3MyExtents.x >= 0)	{ if ( xmf3MyExtents.x - colliderExtents.x < radius) dir = Vector3::Add(dir, Vector3::Normalize(right));}
		//else						{ if (-xmf3MyExtents.x - colliderExtents.x < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, right)));}
		//if (xmf3MyExtents.y >= 0)	{ if ( xmf3MyExtents.y - colliderExtents.y < radius) dir = Vector3::Add(dir, Vector3::Normalize(up));}
		//else						{ if (-xmf3MyExtents.y - colliderExtents.y < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, up)));}
		//if (xmf3MyExtents.z >= 0)	{ if ( xmf3MyExtents.z - colliderExtents.z < radius) dir = Vector3::Add(dir, Vector3::Normalize(look));}
		//else						{ if (-xmf3MyExtents.z - colliderExtents.z < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, look)));}

		XMVECTOR parallel, perpendicular;

		//if (IsIn(abs(xmf3MyExtents.x), colliderExtents.x, colliderExtents.x + radius) || 
		//	IsIn(abs(xmf3MyExtents.y), colliderExtents.y, colliderExtents.y + radius) || 
		//	IsIn(abs(xmf3MyExtents.z), colliderExtents.z, colliderExtents.z + radius)) {
		//	// 이전 위치가 CollisionZone 안일 경우 다음 진행(그 라운드 코너 부분)
		//	if (IsIn(xmf3MyExtents.x, colliderExtents.x, colliderExtents.x + radius))	dir = Vector3::Add(dir, right);
		//	if (IsIn(xmf3MyExtents.y, colliderExtents.y, colliderExtents.y + radius))	dir = Vector3::Add(dir, up);
		//	if (IsIn(xmf3MyExtents.z, colliderExtents.z, colliderExtents.z + radius))	dir = Vector3::Add(dir, look);
		//	if (IsIn(xmf3MyExtents.x, -colliderExtents.x - radius, -colliderExtents.x))	dir = Vector3::Add(dir, Vector3::Multiply(-1, right));
		//	if (IsIn(xmf3MyExtents.y, -colliderExtents.y - radius, -colliderExtents.y))	dir = Vector3::Add(dir, Vector3::Multiply(-1, up));
		//	if (IsIn(xmf3MyExtents.z, -colliderExtents.z - radius, -colliderExtents.z))	dir = Vector3::Add(dir, Vector3::Multiply(-1, look));
		//}
		//else {
		//	// 이전 위치가 CollisionZone 밖일 경우 아래로 진행
		//	float max = abs(xmf3MyExtents.x) - colliderExtents.x; int colliderDir = 0;
		//	if (abs(xmf3MyExtents.y) - colliderExtents.y > max) { max = abs(xmf3MyExtents.y) - colliderExtents.y; colliderDir = 1; }
		//	if (abs(xmf3MyExtents.z) - colliderExtents.z > max) { max = abs(xmf3MyExtents.z) - colliderExtents.z; colliderDir = 2; }

		//	if (colliderDir == 0) {
		//		if (xmf3MyExtents.x > 0)	dir = Vector3::Normalize(right);
		//		else					dir = Vector3::Normalize(Vector3::Multiply(-1, right));
		//	}
		//	else if (colliderDir == 1) {
		//		if (xmf3MyExtents.y > 0)	dir = Vector3::Normalize(up);
		//		else						dir = Vector3::Normalize(Vector3::Multiply(-1, up));
		//	}
		//	else {
		//		if (xmf3MyExtents.z > 0)	dir = Vector3::Normalize(look);
		//		else						dir = Vector3::Normalize(Vector3::Multiply(-1, look));
		//	}
		//}
		float max = abs(xmf3MyExtents.x) - colliderExtents.x; int colliderDir = 0;
		if (abs(xmf3MyExtents.y) - colliderExtents.y > max) { max = abs(xmf3MyExtents.y) - colliderExtents.y; colliderDir = 1; }
		if (abs(xmf3MyExtents.z) - colliderExtents.z > max) { max = abs(xmf3MyExtents.z) - colliderExtents.z; colliderDir = 2; }

		if (colliderDir == 0) {
			if (xmf3MyExtents.x > 0)	dir = Vector3::Normalize(right);
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

		dir = Vector3::Normalize(dir);

		XMVector3ComponentsFromNormal(&parallel, &perpendicular, XMLoadFloat3(&m_xmf3Move), XMLoadFloat3(&dir));

		XMStoreFloat3(&m_xmf3Move, perpendicular);

		//if(Vector3::DotProduct(dir, xmf3Temp) > 0)	m_xmf3Move = xmf3Temp;

		// 지면 판정
		if (Vector3::DotProduct(dir, XMFLOAT3(0, 1, 0)) > 0.8) {
			if (Falling) {
				m_AnimationState = static_cast<int>(AnimationState::ENDJUMP);
				m_AnimationTime = 0;
			}
			m_fHeightVelocity = fTimeElapsed * g_Gravity;
			m_IsOnGround = true;
		}

		/* For Test */
		xmf3MyExtents.x = abs(xmf3MyExtents.x);
		xmf3MyExtents.y = abs(xmf3MyExtents.y);
		xmf3MyExtents.z = abs(xmf3MyExtents.z);
		//a.different = Vector3::Subtract(xmf3MyExtents, colliderExtents);
		//a.nextMove = m_xmf3Move;

		//t.push_back(a);

	}
	m_pObjMng->ColliderTrigInit(ColliderTag::PROP);

	/* For Test */
	//while (t.size() > 100) { t.erase(t.begin()); }

	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	XMFLOAT2 rotationAngle = Rotate(fTimeElapsed);
	//m_CurXAxisRotation += rotationAngle.y;
	m_CurXAxisRotation = Clamp(m_CurXAxisRotation + rotationAngle.y, -85, 85);
	//XMVECTOR rotationVector = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(rotationAngle.x), XMConvertToRadians(rotationAngle.y), 0);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle.x));
	//XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(rotationVector);
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

	XMStoreFloat3(&m_xmf3CameraFocus, XMVector3Rotate(XMLoadFloat3(&m_xmf3CameraFocus), XMQuaternionRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle.x))));


	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

	TriggerOff();
	if (m_HealthPoint <= 0) CPlayer::Disable();
}
void CPlayer::ProcessInput(UCHAR * pKeysBuffer, XMFLOAT2 mouse)
{
	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[static_cast<int>(Move::W)] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[static_cast<int>(Move::A)] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[static_cast<int>(Move::S)] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[static_cast<int>(Move::D)] = true;
	if (pKeysBuffer[VK_SPACE] & 0xF0) m_trigInput[static_cast<int>(Move::SPACE)] = true;

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
	m_AIBrain->ChangeBehavior(new CMoveBehavior());

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
	m_fRemainingTimeOfSlow = 0;
	m_fRemainingTimeOfFire = 0;
	m_fRemainingTimeOfSkill1 = 0;

	CObject::Disable();
}
void CPlayer::Shoot()
{
	m_AnimationState = static_cast<int>(AnimationState::FIRE);
	m_AnimationTime = 0;
	m_fRemainingTimeOfFire = m_fRPM;
}
void CPlayer::Skill(int idx)
{
	m_AnimationState = static_cast<int>(AnimationState::FIRE);
	m_AnimationTime = 0;
	m_fRemainingTimeOfSkill1 = 0.5f;
}
void CPlayer::Slow()
{
	m_fRemainingTimeOfSlow = 3.0f;
	m_fSlowFactor = 0.5;
}
bool CPlayer::IsShootable()
{
	return m_fRemainingTimeOfFire <= 0;
}
bool CPlayer::IsSkillUseable(int idx)
{
	return m_fRemainingTimeOfSkill1 <= 0;
}
void CPlayer::ChangeAnimClip()
{
	if (static_cast<int>(AnimationState::IDLE) == m_AnimationState)
		g_vecAnimCtrl[m_AnimCtrlIdx]->ChangeAnimClip("PlayerIdle");
	if (static_cast<int>(AnimationState::FIRE) == m_AnimationState)
		g_vecAnimCtrl[m_AnimCtrlIdx]->ChangeAnimClip("PlayerFire");
	if (static_cast<int>(AnimationState::RUN) == m_AnimationState)
		g_vecAnimCtrl[m_AnimCtrlIdx]->ChangeAnimClip("PlayerRun");
	if (static_cast<int>(AnimationState::STARTJUMP) == m_AnimationState)
		g_vecAnimCtrl[m_AnimCtrlIdx]->ChangeAnimClip("PlayerStartJump");
	if (static_cast<int>(AnimationState::ENDJUMP) == m_AnimationState)
		g_vecAnimCtrl[m_AnimCtrlIdx]->ChangeAnimClip("PlayerEndJump");
}
void CPlayer::TriggerOff()
{
	for (int i = 0; i < static_cast<int>(Move::count); i++) m_trigInput[i] = false;
	m_rotationInput = XMFLOAT2(0,0);
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

	return Vector3::Multiply(fTimeElapsed * m_fSpeed * m_fSlowFactor, temp);
}
XMFLOAT2 CPlayer::Rotate(float fTimeElapsed)
{
	XMFLOAT2 temp{ 0,0 };
	if (g_IsMouseMode) {
		temp.x += m_rotationInput.x * fTimeElapsed * g_MouseInputSensitivity;
		temp.y += m_rotationInput.y * fTimeElapsed * g_MouseInputSensitivity;
	}
	else {
		//if (m_trigInput[static_cast<int>(Move::Q)]) temp += fTimeElapsed * m_fSpeed * -1;
		//if (m_trigInput[static_cast<int>(Move::E)]) temp += fTimeElapsed * m_fSpeed;
	}

	return temp;
}

